#ifndef __elxFullSearchOptimizer_hxx
#define __elxFullSearchOptimizer_hxx

#include "elxFullSearchOptimizer.h"
#include <iomanip>
#include <sstream>
#include <string>
#include "vnl/vnl_math.h"

namespace elastix
{
using namespace itk;


	/**
	 * ********************* Constructor ****************************
	 */
	
	template <class TElastix>
		FullSearch<TElastix>
		::FullSearch() 
	{
		this->m_OptimizationSurface = 0;

	} // end Constructor
	

	/**
	 * ***************** BeforeRegistration ***********************
	 */

	template <class TElastix>
		void FullSearch<TElastix>::
		BeforeRegistration(void)
	{
		/** Add the target cells "ItNr" and "Metric" to xout["iteration"].*/
		xl::xout["iteration"].AddTargetCell("2:Metric");

		/** Format the metric as floats */			
		xl::xout["iteration"]["2:Metric"]		<< std::showpoint << std::fixed;


	} // end BeforeRegistration


	/**
	 * ***************** BeforeEachResolution ***********************
	 */

	template <class TElastix>
		void FullSearch<TElastix>
		::BeforeEachResolution(void)
	{
		/** Get the current resolution level.*/
		unsigned int level = static_cast<unsigned int>(
			this->m_Registration->GetAsITKBaseType()->GetCurrentLevel() );
		
		/**
		 * Read FullSearchRange from the parameter file
		 */

		/** declare variables */

		std::string name("");
		unsigned int param_nr = 0;
		double minimum = 0;
		double maximum = 0;
		double stepsize = 0;
		const std::string prefix = "FS";
		unsigned int entry_nr = 0;
		unsigned int nrOfSearchSpaceDimensions = 0;
		int errorcode = 0;
		int realerror = 0;
		std::ostringstream makeString("");


		/** fullFieldName = "FullSearchSpace0" at level 0; */
		makeString << "FullSearchSpace" << level;
		std::string fullFieldName = makeString.str();


		
		while ( errorcode==0 )  //as long as still search ranges are defined
		{
			/** Try to read (silently) from the parameter file */
			/** \todo check earlier, in BeforeAll, if the searchspace has been defined. */

			if (!realerror && !errorcode)
			{
				errorcode = this->GetConfiguration()->ReadParameter(
					name, fullFieldName.c_str(), entry_nr, true);
				realerror = this->CheckSearchSpaceRangeDefinition(fullFieldName.c_str(), errorcode, entry_nr);
				entry_nr++;
			}
			if (!realerror && !errorcode)
			{
				errorcode = this->GetConfiguration()->ReadParameter(
					param_nr, fullFieldName.c_str(), entry_nr, true);
				realerror = this->CheckSearchSpaceRangeDefinition(fullFieldName.c_str(), errorcode, entry_nr);
				entry_nr++;
			}
			if (!realerror && !errorcode)
			{
				errorcode = this->GetConfiguration()->ReadParameter(
					minimum, fullFieldName.c_str(), entry_nr, true);
				realerror = this->CheckSearchSpaceRangeDefinition(fullFieldName.c_str(), errorcode, entry_nr);
				entry_nr++;
			}
			if (!realerror && !errorcode)
			{
				errorcode = this->GetConfiguration()->ReadParameter(
					maximum, fullFieldName.c_str(), entry_nr, true);
				realerror = this->CheckSearchSpaceRangeDefinition(fullFieldName.c_str(), errorcode, entry_nr);
				entry_nr++;
			}
			if (!realerror && !errorcode)
			{
				errorcode = this->GetConfiguration()->ReadParameter(
					stepsize, fullFieldName.c_str(), entry_nr, true);
				realerror = this->CheckSearchSpaceRangeDefinition(fullFieldName.c_str(), errorcode, entry_nr);
				entry_nr++;
			}

			/** Setup this search range */
			if (!realerror && !errorcode)
			{
				/** Setup the Superclass */
				this->AddSearchDimension(param_nr, minimum, maximum, stepsize);

				/** Create name of this dimension */
				makeString.str("");
				makeString
					<< prefix
					<< ( (entry_nr / 5) - 1 )
					<< ":"
					<< name
					<< ":"
					<< param_nr;

				/** Store the name and create a column in xout["iteration"] */
				this->m_SearchSpaceDimensionNames[param_nr] = makeString.str();
				xl::xout["iteration"].AddTargetCell( makeString.str().c_str() );

				/** Format this xout iteration column as float */
				xl::xout["iteration"][ makeString.str().c_str() ]
					<< std::showpoint << std::fixed;

			} // end if (!realerror)

		} // end while

		if (!realerror)
		{
			
			/** The number of dimensions */
			nrOfSearchSpaceDimensions = this->GetNumberOfSearchSpaceDimensions();
			
			/** Create the image that will store the results of the full search. */
			this->m_OptimizationSurface = 
				NDImageType::NewNDImage(nrOfSearchSpaceDimensions);
			this->m_OptimizationSurface->CreateNewImage();
			/** \todo don't do this if more than max allowable dimensions. */
		
			/** Set the correct size and allocate memory */
			this->m_OptimizationSurface->SetRegions(
				this->GetSearchSpaceSize()  );
			this->m_OptimizationSurface->Allocate();
			/** \todo try/catch block around Allocate? */
					
			/** Set the name of this image on disk. */
			makeString.str("");
			makeString
				<< this->GetConfiguration()->GetCommandLineArgument("-out")
				<< "OptimizationSurface"
				<< this->GetConfiguration()->GetElastixLevel()
				<< ".R"
				<< level
				<< ".mhd" ;
			this->m_OptimizationSurface->SetOutputFileName( makeString.str().c_str() );
			
			elxout
				<< "Total number of iterations needed in this resolution: "
				<< this->GetNumberOfIterations()
				<< "."
				<< std::endl;
				
		} // if (!realerror)

		/** \todo else: quit program or something like that. */

	} // end BeforeEachResolution


	/**
	 * ***************** AfterEachIteration *************************
	 */

	template <class TElastix>
		void FullSearch<TElastix>
		::AfterEachIteration(void)
	{
		/** Print some information */
		xl::xout["iteration"]["2:Metric"]		<< this->GetValue();

		this->m_OptimizationSurface->SetPixel(
			this->GetCurrentIndexInSearchSpace(),
			this->GetValue()   );

		SearchSpacePointType currentPoint = this->GetCurrentPointInSearchSpace();
		unsigned int nrOfSSDims = currentPoint.GetSize();
		NameIteratorType name_it = this->m_SearchSpaceDimensionNames.begin();

		for ( unsigned int dim = 0; dim < nrOfSSDims; dim++)
		{	
			xl::xout["iteration"][name_it->second.c_str()] << currentPoint[dim];
			name_it++;
		}

		
	} // end AfterEachIteration


	/**
	 * ***************** AfterEachResolution *************************
	 */

	template <class TElastix>
		void FullSearch<TElastix>
		::AfterEachResolution(void)
	{
		//typedef enum {FullRangeSearched,	MetricError	} StopConditionType;
		
		std::string stopcondition;

	
		switch( this->GetStopCondition() )
		{
			
		case FullRangeSearched :
			stopcondition = "The full range has been searched";
			break;
			
		case MetricError :
			stopcondition = "Error in metric";	
			break;	
		
		default:
			stopcondition = "Unknown";
			break;
			
		}
		/** Print the stopping condition */

		elxout << "Stopping condition: " << stopcondition << "." << std::endl;
				
		/** Write the optimization surface to disk */
		this->m_OptimizationSurface->Write(); /** \todo try/catch? */
		elxout
			<< "\nThe scanned optimization surface is saved as: "
			<< this->m_OptimizationSurface->GetOutputFileName()
			<< std::endl;

		/** Print the best metric value */
		elxout 
			<< std::endl 
			<< "Best metric value in this resolution = " 
			<< this->GetBestValue()
			<< std::endl;
		
		/** Print the best index and point */
		SearchSpaceIndexType bestIndex = this->GetBestIndexInSearchSpace();
		SearchSpacePointType bestPoint = this->GetBestPointInSearchSpace();
		unsigned int nrOfSSDims = bestIndex.GetSize();

		elxout <<	"Index of the point in the optimization surface image that has the best metric value: [ ";
		for (unsigned int dim = 0; dim < nrOfSSDims; dim++)
		{
			elxout << bestIndex[dim] << " ";
		}
		elxout << "]" << std::endl;

		elxout << "The corresponding parameter values: [ " ;
		for (unsigned int dim = 0; dim < nrOfSSDims; dim++)
		{
			elxout << bestPoint[dim] << " ";
		}
		elxout << "]\n" << std::endl;
		
		/** Remove the columns from xout["iteration"] */
		NameIteratorType name_it = this->m_SearchSpaceDimensionNames.begin();
		for ( unsigned int dim = 0; dim < nrOfSSDims; dim++)
		{	
			xl::xout["iteration"].RemoveTargetCell( name_it->second.c_str() );
			name_it++;
		}

		/** Clear the dimension names of the previous resolution's search space */
		this->m_SearchSpaceDimensionNames.clear();
		/** Clear the full search ranges */
		this->SetSearchSpace(0);

	} // end AfterEachResolution

	
	/**
	 * ******************* AfterRegistration ************************
	 */
	template <class TElastix>
		void FullSearch<TElastix>
		::AfterRegistration(void)
	{
	  /** Print the best metric value */
		double bestValue = this->GetBestValue();
		elxout << std::endl << "Final metric value  = " << bestValue  << std::endl;

	} // end AfterRegistration


	/**
	 * ************ CheckSearchSpaceRangeDefinition *****************
	 */
	template <class TElastix>
		int FullSearch<TElastix>
		::CheckSearchSpaceRangeDefinition(const std::string & fullFieldName, int errorcode, unsigned int entry_nr)
	{

		/** Complain if not at least one search space dimension has been found,
		 * or if a search dimension is not fully specified */
		if (
					(errorcode != 0) &&
					( (entry_nr == 0) || ((entry_nr % 5) != 0) ) 
			 )
		{
			xl::xout["error"] 
				<< "ERROR:\nNo (valid) range specified for the full search optimizer!"
				<< std::endl
				<< "Please define the field (" 
				<< fullFieldName
				<< " \"name\" parameter_nr min max stepsize) correctly in the parameter file" 
				<< std::endl;
			return 1;
		} 
		else
		{
			return 0;
		}

	} // end CheckSearchSpaceRangeDefinition

	

} // end namespace elastix

#endif // end #ifndef __elxFullSearchOptimizer_hxx

