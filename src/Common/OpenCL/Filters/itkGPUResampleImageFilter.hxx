/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/
#ifndef __itkGPUResampleImageFilter_hxx
#define __itkGPUResampleImageFilter_hxx

#include "itkGPUResampleImageFilter.h"
#include "itkGPUKernelManagerHelperFunctions.h"
#include "itkGPUMath.h"
#include "itkGPUImageBase.h"
#include "itkGPUBSplineBaseTransform.h"
#include "itkGPUBSplineInterpolateImageFunction.h"

#include "itkImageLinearIteratorWithIndex.h"
#include "itkTimeProbe.h"

namespace
{
typedef struct {
  cl_int transform_linear;
  cl_int interpolator_is_bspline;
  cl_int transform_is_bspline;
  cl_float default_value;
  cl_float2 min_max;
  cl_float2 min_max_output;
  cl_float3 delta;
} FilterParameters;
} // end of unnamed namespace

namespace itk
{
//------------------------------------------------------------------------------
template< class TInputImage, class TOutputImage, class TInterpolatorPrecisionType >
GPUResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::GPUResampleImageFilter()
{
  this->m_InputGPUImageBase = GPUDataManager::New();
  this->m_OutputGPUImageBase = GPUDataManager::New();

  this->m_Parameters = GPUDataManager::New();
  this->m_Parameters->Initialize();
  this->m_Parameters->SetBufferFlag( CL_MEM_READ_ONLY );
  this->m_Parameters->SetBufferSize( sizeof( FilterParameters ) );
  this->m_Parameters->Allocate();

  this->m_InterpolatorSourceLoaded = false;
  this->m_TransformSourceLoaded = false;
  this->m_InterpolatorSourceLoadedIndex = 0;
  this->m_TransformSourceLoadedIndex = 0;

  this->m_InterpolatorIsBSpline = false; // make it protected in base class
  this->m_TransformIsBSpline = false;

  this->m_InterpolatorBase = NULL;
  this->m_TransformBase = NULL;

  std::ostringstream defines;
  if ( TInputImage::ImageDimension > 3 || TInputImage::ImageDimension < 1 )
  {
    itkExceptionMacro( "GPUResampleImageFilter supports 1/2/3D image." );
  }

  defines << "#define DIM_" << int(TInputImage::ImageDimension) << "\n";
  defines << "#define INPIXELTYPE ";
  GetTypenameInString( typeid( typename TInputImage::PixelType ), defines );
  defines << "#define OUTPIXELTYPE ";
  GetTypenameInString( typeid( typename TOutputImage::PixelType ), defines );
  defines << "#define INTERPOLATOR_PRECISION_TYPE ";
  GetTypenameInString( typeid( TInterpolatorPrecisionType ), defines );

  // Resize m_Sources
  const unsigned int numberOfIncludes = 3; // Defines, GPUMath, GPUImageBase
  const unsigned int numberOfSources  = 3; // GPUInterpolator, GPUTransform,
                                           // GPUResampleImageFilter
  m_Sources.resize( numberOfIncludes + numberOfSources );
  m_SourceIndex = 0;

  // Add defines
  m_Sources[m_SourceIndex++] = defines.str();

  // Add GPUMath source
  const std::string oclGPUMathSource( GPUMathKernel::GetOpenCLSource() );
  m_Sources[m_SourceIndex++] = oclGPUMathSource;

  // Load GPUImageBase
  const std::string oclImageBaseSource( GPUImageBaseKernel::GetOpenCLSource() );
  m_Sources[m_SourceIndex++] = oclImageBaseSource;
}

//------------------------------------------------------------------------------
template< class TInputImage, class TOutputImage, class TInterpolatorPrecisionType >
void GPUResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::SetInterpolator( InterpolatorType *_arg )
{
  itkDebugMacro( "setting Interpolator to " << _arg );
  CPUSuperclass::SetInterpolator( _arg );

  const GPUInterpolatorBase *interpolatorBase =
    dynamic_cast< const GPUInterpolatorBase * >( _arg );

  if ( interpolatorBase )
  {
    this->m_InterpolatorBase = (GPUInterpolatorBase *)interpolatorBase;

    // Test for a GPU BSpline interpolator
    typedef GPUBSplineInterpolateImageFunction< InputImageType,
                                                TInterpolatorPrecisionType > GPUBSplineInterpolatorType;
    const GPUBSplineInterpolatorType *GPUBSplineInterpolator =
      dynamic_cast< const GPUBSplineInterpolatorType * >( _arg );
    if ( GPUBSplineInterpolator )
    {
      m_InterpolatorIsBSpline = true;
    }
    else
    {
      m_InterpolatorIsBSpline = false;
    }
  }
  else
  {
    itkExceptionMacro( "Setting unsupported GPU interpolator to " << _arg );
  }
}

//------------------------------------------------------------------------------
template< class TInputImage, class TOutputImage, class TInterpolatorPrecisionType >
void GPUResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::SetTransform( const TransformType *_arg )
{
  itkDebugMacro( "setting Transform to " << _arg );
  CPUSuperclass::SetTransform( _arg );

  const GPUTransformBase *transformBase =
    dynamic_cast< const GPUTransformBase * >( _arg );

  if ( transformBase )
  {
    this->m_TransformBase = (GPUTransformBase *)transformBase;

    // Test for a GPU BSpline transforms
    typedef GPUBSplineBaseTransform< TInterpolatorPrecisionType,
                                     InputImageDimension > BSplineTransformType;
    const BSplineTransformType *bsplineTransformBase =
      dynamic_cast< const BSplineTransformType * >( _arg );
    if ( bsplineTransformBase )
    {
      m_TransformIsBSpline = true;
    }
    else
    {
      m_TransformIsBSpline = false;
    }

    // Get transform source
    std::string source;
    if ( !transformBase->GetSourceCode( source ) )
    {
      m_TransformSourceLoaded = false;
      itkExceptionMacro( << "Unable to get transform source code." );
    }
    else
    {
      m_TransformSourceLoaded = true;
      if ( m_TransformSourceLoadedIndex == 0 )
      {
        m_TransformSourceLoadedIndex = m_SourceIndex;
        m_Sources[m_SourceIndex++] = source;
      }
      else
      {
        m_Sources[m_TransformSourceLoadedIndex] = source;
      }
    }
  }
  else
  {
    itkExceptionMacro( "Setting unsupported GPU transform to " << _arg );
  }
}

//------------------------------------------------------------------------------
template< class TInputImage, class TOutputImage, class TInterpolatorPrecisionType >
void GPUResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::AfterThreadedGenerateData()
{
  // Nothing here
  // \todo check for memory leaks with 
  // ResampleImageFilter::AfterThreadedGenerateData()
}

//------------------------------------------------------------------------------
template< class TInputImage, class TOutputImage, class TInterpolatorPrecisionType >
void GPUResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::CompileOpenCLCode()
{
  if ( m_InterpolatorSourceLoaded && m_TransformSourceLoaded )
  {
    std::ostringstream loadedSources;
    for ( unsigned int i = 0; i < m_Sources.size(); i++ )
    {
      loadedSources << m_Sources[i] << std::endl;
    }

    // OpenCL source path
    const char *GPUSource = GPUResampleImageFilterKernel::GetOpenCLSource();
    // Load and create kernel
    bool loaded = this->m_GPUKernelManager->LoadProgramFromString( GPUSource, loadedSources.str().c_str() );
    if ( loaded )
    {
      if ( !m_InterpolatorIsBSpline )
      {
        if ( !m_TransformIsBSpline )
        {
          m_FilterGPUKernelHandle = this->m_GPUKernelManager->CreateKernel( "ResampleImageFilter" );
        }
        else
        {
          m_FilterGPUKernelHandle = this->m_GPUKernelManager->CreateKernel( "ResampleImageFilter_TransformBSpline" );
        }
      }
      else
      {
        if ( !m_TransformIsBSpline )
        {
          m_FilterGPUKernelHandle = this->m_GPUKernelManager->CreateKernel( "ResampleImageFilter_InterpolatorBSpline" );
        }
        else
        {
          m_FilterGPUKernelHandle = this->m_GPUKernelManager->CreateKernel(
            "ResampleImageFilter_InterpolatorBSpline_TransformBSpline" );
        }
      }
    }
    else
    {
      itkExceptionMacro( << "Kernel has not been loaded from:\n" << GPUSource );
    }
  }
}

//------------------------------------------------------------------------------
template< class TInputImage, class TOutputImage, class TInterpolatorPrecisionType >
void GPUResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >::GPUGenerateData()
{
  // Profiling
#ifdef OPENCL_PROFILING
  itk::TimeProbe gputimer;
  gputimer.Start();
#endif

  typename GPUInputImage::Pointer inPtr = dynamic_cast< GPUInputImage * >( this->ProcessObject::GetInput( 0 ) );
  typename GPUOutputImage::Pointer otPtr = dynamic_cast< GPUOutputImage * >( this->ProcessObject::GetOutput( 0 ) );

  // Perform the safe check
  if ( inPtr.IsNull() )
  {
    itkExceptionMacro( << "The GPU InputImage is NULL. Filter unable to perform." );
    return;
  }
  if ( otPtr.IsNull() )
  {
    itkExceptionMacro( << "The GPU OutputImage is NULL. Filter unable to perform." );
    return;
  }

  typename GPUOutputImage::SizeType outSize = otPtr->GetLargestPossibleRegion().GetSize();

  // Get interpolator source
  std::string source;
  if ( !this->m_InterpolatorBase->GetSourceCode( source ) )
  {
    m_InterpolatorSourceLoaded = false;
    itkExceptionMacro( << "Unable to get interpolator source code." );
  }
  else
  {
    m_InterpolatorSourceLoaded = true;
    if ( m_InterpolatorSourceLoadedIndex == 0 )
    {
      m_InterpolatorSourceLoadedIndex = m_SourceIndex;
      m_Sources[m_SourceIndex++] = source;
    }
    else
    {
      m_Sources[m_InterpolatorSourceLoadedIndex] = source;
    }
  }

  CompileOpenCLCode();

  FilterParameters parameters;

  //
  unsigned int imgSize[3];
  imgSize[0] = imgSize[1] = imgSize[2] = 1;

  const unsigned int ImageDim = (unsigned int)TInputImage::ImageDimension;
  for ( unsigned int i = 0; i < ImageDim; i++ )
  {
    imgSize[i] = outSize[i];
  }

  size_t localSize[3], globalSize[3];
  localSize[0] = localSize[1] = localSize[2] = OpenCLGetLocalBlockSize( ImageDim );

  for ( unsigned int i = 0; i < ImageDim; i++ )
  {
    // total # of threads
    globalSize[i] = localSize[i] * (unsigned int)ceil( (float)outSize[i] / (float)localSize[i] );
  }

  // arguments set up
  cl_uint argidx = 0;
  itk::SetKernelWithITKImage< GPUInputImage >( this->m_GPUKernelManager,
                                               m_FilterGPUKernelHandle,
                                               argidx,
                                               inPtr,
                                               m_InputGPUImageBase );
  itk::SetKernelWithITKImage< GPUOutputImage >( this->m_GPUKernelManager,
                                                m_FilterGPUKernelHandle,
                                                argidx,
                                                otPtr,
                                                m_OutputGPUImageBase );

  // Set transform linear
  parameters.transform_linear = static_cast< int >( this->GetTransform()->IsLinear() );
  // Set interpolator is BSpline
  parameters.interpolator_is_bspline = static_cast< int >( this->m_InterpolatorIsBSpline );
  // Set transform is BSpline
  parameters.transform_is_bspline = static_cast< int >( this->m_TransformIsBSpline );
  // Set defaultValue, minValue/maxValue, minOutputValue/maxOutputValue
  //typedef typename InterpolatorType::OutputType OutputType;
  parameters.default_value = static_cast< float >( this->GetDefaultPixelValue() );
  // Min/max values of the output pixel type AND these values
  // represented as the output type of the interpolator
  parameters.min_max.s[0] = static_cast< float >( NumericTraits< OutputImagePixelType >::NonpositiveMin() );
  parameters.min_max.s[1] = static_cast< float >( NumericTraits< OutputImagePixelType >::max() );
  parameters.min_max_output.s[0] = parameters.min_max.s[0];
  parameters.min_max_output.s[1] = parameters.min_max.s[1];

  // Calculate delta
  float delta[3];
  CalculateDelta( inPtr, otPtr, &delta[0] );
  for ( unsigned int i = 0; i < OutputImageType::ImageDimension; ++i )
  {
    parameters.delta.s[i] = delta[i];
  }

  this->m_Parameters->SetCPUBufferPointer( &parameters );
  this->m_Parameters->SetGPUDirtyFlag( true );
  this->m_Parameters->UpdateGPUBuffer();

  this->m_GPUKernelManager->SetKernelArgWithImage( m_FilterGPUKernelHandle, argidx++, this->m_Parameters );

  // Set image function
  this->m_GPUKernelManager->SetKernelArgWithImage( m_FilterGPUKernelHandle, argidx++,
                                                   this->m_InterpolatorBase->GetParametersDataManager() );

  if ( m_InterpolatorIsBSpline )
  {
    typedef GPUBSplineInterpolateImageFunction< InputImageType,
                                                TInterpolatorPrecisionType > GPUBSplineInterpolatorType;
    typedef typename GPUBSplineInterpolatorType::GPUCoefficientImageType    GPUCoefficientImageType;
    typedef typename GPUBSplineInterpolatorType::GPUCoefficientImagePointer GPUCoefficientImagePointer;
    typedef typename GPUBSplineInterpolatorType::GPUDataManagerPointer      GPUDataManagerPointer;

    const GPUBSplineInterpolatorType *GPUBSplineInterpolator =
      dynamic_cast< const GPUBSplineInterpolatorType * >( m_InterpolatorBase );

    if ( GPUBSplineInterpolator )
    {
      GPUCoefficientImagePointer coefficient =
        GPUBSplineInterpolator->GetGPUCoefficients();
      GPUDataManagerPointer coefficientbase =
        GPUBSplineInterpolator->GetGPUCoefficientsImageBase();

      SetKernelWithITKImage< GPUCoefficientImageType >(
        this->m_GPUKernelManager,
        m_FilterGPUKernelHandle, argidx, coefficient, coefficientbase );
    }
    else
    {
      itkExceptionMacro( << "Could not get coefficients from GPU BSpline interpolator." );
    }
  }

  if ( m_TransformIsBSpline )
  {
    typedef GPUBSplineBaseTransform< TInterpolatorPrecisionType,
                                     InputImageDimension > GPUBSplineTransformType;
    typedef typename GPUBSplineTransformType::GPUCoefficientImageType      GPUCoefficientImageType;
    typedef typename GPUBSplineTransformType::GPUCoefficientImageArray     GPUCoefficientImageArray;
    typedef typename GPUBSplineTransformType::GPUCoefficientImageBaseArray GPUCoefficientImageBaseArray;
    typedef typename GPUBSplineTransformType::GPUCoefficientImagePointer   GPUCoefficientImagePointer;
    typedef typename GPUBSplineTransformType::GPUDataManagerPointer        GPUDataManagerPointer;

    const GPUBSplineTransformType *GPUBSplineTransformBase =
      dynamic_cast< const GPUBSplineTransformType * >( m_TransformBase );

    if ( GPUBSplineTransformBase )
    {
      GPUCoefficientImageArray GPUCoefficientImages =
        GPUBSplineTransformBase->GetGPUCoefficientImages();
      GPUCoefficientImageBaseArray GPUCoefficientImagesBases =
        GPUBSplineTransformBase->GetGPUCoefficientImagesBases();

      for ( unsigned int i = 0; i < ImageDim; i++ )
      {
        GPUCoefficientImagePointer coefficient = GPUCoefficientImages[i];
        GPUDataManagerPointer      coefficientbase = GPUCoefficientImagesBases[i];

        itk::SetKernelWithITKImage< GPUCoefficientImageType >(
          this->m_GPUKernelManager,
          m_FilterGPUKernelHandle, argidx, coefficient, coefficientbase );
      }
    }
    else
    {
      itkExceptionMacro( << "Could not get coefficients from GPU BSpline transform." );
    }
  }
  else
  {
    this->m_GPUKernelManager->SetKernelArgWithImage( m_FilterGPUKernelHandle, argidx++,
                                                     this->m_TransformBase->GetParametersDataManager() );
  }

  // Profiling
#ifdef OPENCL_PROFILING
  gputimer.Stop();
  std::cout << "GPU ResampleImageFilter before LaunchKernel() took " << gputimer.GetMean() << " seconds." << std::endl;
#endif

  // launch kernel
  //this->m_GPUKernelManager->LaunchKernel(m_FilterGPUKernelHandle,
  // (int)TInputImage::ImageDimension, globalSize, localSize);
  this->m_GPUKernelManager->LaunchKernel( m_FilterGPUKernelHandle, (int)TInputImage::ImageDimension, globalSize );
  //std::cout<<"LaunchKernel finished." <<std::endl;
}

//------------------------------------------------------------------------------
template< class TInputImage, class TOutputImage, class TInterpolatorPrecisionType >
void GPUResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::CalculateDelta( const typename GPUInputImage::Pointer & _inputPtr,
                  const typename GPUOutputImage::Pointer & _outputPtr,
                  float *_delta )
{
  if ( !this->GetTransform()->IsLinear() )
  {
    for ( unsigned int i = 0; i < OutputImageType::ImageDimension; i++ )
    {
      _delta[i] = 0.0;
    }
    return;
  }

  // Create an iterator that will walk the output region for this thread.
  typedef ImageLinearIteratorWithIndex< TOutputImage > OutputIterator;
  OutputIterator outIt( _outputPtr, _outputPtr->GetLargestPossibleRegion() );
  outIt.SetDirection( 0 );

  // Define a few indices that will be used to translate from an input pixel
  // to an output pixel
  typedef Point< float, OutputImageType::ImageDimension >           FloatPointType;
  typedef ContinuousIndex< float, OutputImageType::ImageDimension > FloatContinuousInputIndexType;
  FloatPointType outputPoint;         // Coordinates of current output pixel
  FloatPointType inputPoint;          // Coordinates of current input pixel
  FloatPointType tmpOutputPoint;
  FloatPointType tmpInputPoint;

  FloatContinuousInputIndexType inputIndex;
  FloatContinuousInputIndexType tmpInputIndex;

  //typedef typename PointType::VectorType VectorType;
  typedef Vector< float, OutputImageType::ImageDimension > FloatVectorType;
  FloatVectorType delta;          // delta in input continuous index coordinate
                                  // frame

  IndexType index;

  // Determine the position of the first pixel in the scanline
  index = outIt.GetIndex();
  _outputPtr->TransformIndexToPhysicalPoint( index, outputPoint );

  // Compute corresponding input pixel position
  inputPoint = this->GetTransform()->TransformPoint( outputPoint );
  _inputPtr->TransformPhysicalPointToContinuousIndex( inputPoint, inputIndex );

  // As we walk across a scan line in the output image, we trace
  // an oriented/scaled/translated line in the input image.  Cache
  // the delta along this line in continuous index space of the input
  // image. This allows us to use vector addition to model the
  // transformation.
  //
  // To find delta, we take two pixels adjacent in a scanline
  // and determine the continuous indices of these pixels when
  // mapped to the input coordinate frame. We use the difference
  // between these two continuous indices as the delta to apply
  // to an index to trace line in the input image as we move
  // across the scanline of the output image.
  //
  // We determine delta in this manner so that Images
  // are both handled properly (taking into account the direction cosines).
  //

  ++index[0];
  _outputPtr->TransformIndexToPhysicalPoint( index, tmpOutputPoint );
  tmpInputPoint = this->GetTransform()->TransformPoint( tmpOutputPoint );
  _inputPtr->TransformPhysicalPointToContinuousIndex( tmpInputPoint, tmpInputIndex );

  delta = tmpInputIndex - inputIndex;

  for ( unsigned int i = 0; i < OutputImageType::ImageDimension; i++ )
  {
    _delta[i] = delta[i];
  }
}

//------------------------------------------------------------------------------
template< class TInputImage, class TOutputImage, class TInterpolatorPrecisionType >
void GPUResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::PrintSelf( std::ostream & os, Indent indent ) const
{
  CPUSuperclass::PrintSelf( os, indent );
  GPUSuperclass::PrintSelf( os, indent );
}

} // end namespace itk

#endif /* __itkGPUResampleImageFilter_hxx */
