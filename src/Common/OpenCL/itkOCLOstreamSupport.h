/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/
#ifndef __itkOCLOstreamSupport_h
#define __itkOCLOstreamSupport_h

#ifdef __APPLE__
#include <OpenCL/cl_platform.h>
#else
#include <CL/cl_platform.h>
#endif
#include <ostream>
#include <iomanip>

//------------------------------------------------------------------------------
// uchar
namespace itk
{
template< class ucharT, class traits >
inline
std::basic_ostream< ucharT, traits > & operator<<(
  std::basic_ostream< ucharT, traits > & strm,
  const cl_uchar & _v )
{
  strm << "(uchar)(" << _v << ")";
  return strm;
}

template< class ucharT, class traits >
inline
std::basic_ostream< ucharT, traits > & operator<<(
  std::basic_ostream< ucharT, traits > & strm,
  const cl_uchar2 & _v )
{
  strm << "(uchar2)(";
  for ( unsigned int i = 0; i < 2; ++i )
  {
    strm << _v.s[i];
    if ( i != 1 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class ucharT, class traits >
inline
std::basic_ostream< ucharT, traits > & operator<<(
  std::basic_ostream< ucharT, traits > & strm,
  const cl_uchar4 & _v )
{
  strm << "(uchar4)(";
  for ( unsigned int i = 0; i < 4; ++i )
  {
    strm << _v.s[i];
    if ( i != 3 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class ucharT, class traits >
inline
std::basic_ostream< ucharT, traits > & operator<<(
  std::basic_ostream< ucharT, traits > & strm,
  const cl_uchar8 & _v )
{
  strm << "(uchar8)(";
  for ( unsigned int i = 0; i < 8; ++i )
  {
    strm << _v.s[i];
    if ( i != 7 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class ucharT, class traits >
inline
std::basic_ostream< ucharT, traits > & operator<<(
  std::basic_ostream< ucharT, traits > & strm,
  const cl_uchar16 & _v )
{
  strm << "(uchar16)(";
  for ( unsigned int i = 0; i < 16; ++i )
  {
    strm << _v.s[i];
    if ( i != 15 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

//------------------------------------------------------------------------------
// char
template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_char & _v )
{
  strm << "(char)(" << _v << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_char2 & _v )
{
  strm << "(char2)(";
  for ( unsigned int i = 0; i < 2; ++i )
  {
    strm << _v.s[i];
    if ( i != 1 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_char4 & _v )
{
  strm << "(char4)(";
  for ( unsigned int i = 0; i < 4; ++i )
  {
    strm << _v.s[i];
    if ( i != 3 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_char8 & _v )
{
  strm << "(char8)(";
  for ( unsigned int i = 0; i < 8; ++i )
  {
    strm << _v.s[i];
    if ( i != 7 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_char16 & _v )
{
  strm << "(char16)(";
  for ( unsigned int i = 0; i < 16; ++i )
  {
    strm << _v.s[i];
    if ( i != 15 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

//------------------------------------------------------------------------------
// ushort
template< class ushortT, class traits >
inline
std::basic_ostream< ushortT, traits > & operator<<(
  std::basic_ostream< ushortT, traits > & strm,
  const cl_ushort & _v )
{
  strm << "(ushort)(" << _v << ")";
  return strm;
}

template< class ushortT, class traits >
inline
std::basic_ostream< ushortT, traits > & operator<<(
  std::basic_ostream< ushortT, traits > & strm,
  const cl_ushort2 & _v )
{
  strm << "(ushort2)(";
  for ( unsigned int i = 0; i < 2; ++i )
  {
    strm << _v.s[i];
    if ( i != 1 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class ushortT, class traits >
inline
std::basic_ostream< ushortT, traits > & operator<<(
  std::basic_ostream< ushortT, traits > & strm,
  const cl_ushort4 & _v )
{
  strm << "(ushort4)(";
  for ( unsigned int i = 0; i < 4; ++i )
  {
    strm << _v.s[i];
    if ( i != 3 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class ushortT, class traits >
inline
std::basic_ostream< ushortT, traits > & operator<<(
  std::basic_ostream< ushortT, traits > & strm,
  const cl_ushort8 & _v )
{
  strm << "(ushort8)(";
  for ( unsigned int i = 0; i < 8; ++i )
  {
    strm << _v.s[i];
    if ( i != 7 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class ushortT, class traits >
inline
std::basic_ostream< ushortT, traits > & operator<<(
  std::basic_ostream< ushortT, traits > & strm,
  const cl_ushort16 & _v )
{
  strm << "(ushort16)(";
  for ( unsigned int i = 0; i < 16; ++i )
  {
    strm << _v.s[i];
    if ( i != 15 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

//------------------------------------------------------------------------------
// short
template< class shortT, class traits >
inline
std::basic_ostream< shortT, traits > & operator<<(
  std::basic_ostream< shortT, traits > & strm,
  const cl_short & _v )
{
  strm << "(short)(" << _v << ")";
  return strm;
}

template< class shortT, class traits >
inline
std::basic_ostream< shortT, traits > & operator<<(
  std::basic_ostream< shortT, traits > & strm,
  const cl_short2 & _v )
{
  strm << "(short2)(";
  for ( unsigned int i = 0; i < 2; ++i )
  {
    strm << _v.s[i];
    if ( i != 1 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class shortT, class traits >
inline
std::basic_ostream< shortT, traits > & operator<<(
  std::basic_ostream< shortT, traits > & strm,
  const cl_short4 & _v )
{
  strm << "(short4)(";
  for ( unsigned int i = 0; i < 4; ++i )
  {
    strm << _v.s[i];
    if ( i != 3 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class shortT, class traits >
inline
std::basic_ostream< shortT, traits > & operator<<(
  std::basic_ostream< shortT, traits > & strm,
  const cl_short8 & _v )
{
  strm << "(short8)(";
  for ( unsigned int i = 0; i < 8; ++i )
  {
    strm << _v.s[i];
    if ( i != 7 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class shortT, class traits >
inline
std::basic_ostream< shortT, traits > & operator<<(
  std::basic_ostream< shortT, traits > & strm,
  const cl_short16 & _v )
{
  strm << "(short16)(";
  for ( unsigned int i = 0; i < 16; ++i )
  {
    strm << _v.s[i];
    if ( i != 15 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

//------------------------------------------------------------------------------
// uint
template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_uint & _v )
{
  strm << "(uint)(" << _v << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_uint2 & _v )
{
  strm << "(uint2)(";
  for ( unsigned int i = 0; i < 2; ++i )
  {
    strm << _v.s[i];
    if ( i != 1 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_uint4 & _v )
{
  strm << "(uint4)(";
  for ( unsigned int i = 0; i < 4; ++i )
  {
    strm << _v.s[i];
    if ( i != 3 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_uint8 & _v )
{
  strm << "(uint8)(";
  for ( unsigned int i = 0; i < 8; ++i )
  {
    strm << _v.s[i];
    if ( i != 7 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_uint16 & _v )
{
  strm << "(uint16)(";
  for ( unsigned int i = 0; i < 16; ++i )
  {
    strm << _v.s[i];
    if ( i != 15 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

//------------------------------------------------------------------------------
// int
template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_int & _v )
{
  strm << "(int)(" << _v << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_int2 & _v )
{
  strm << "(int2)(";
  for ( unsigned int i = 0; i < 2; ++i )
  {
    strm << _v.s[i];
    if ( i != 1 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_int4 & _v )
{
  strm << "(int4)(";
  for ( unsigned int i = 0; i < 4; ++i )
  {
    strm << _v.s[i];
    if ( i != 3 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_int8 & _v )
{
  strm << "(int8)(";
  for ( unsigned int i = 0; i < 8; ++i )
  {
    strm << _v.s[i];
    if ( i != 7 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_int16 & _v )
{
  strm << "(int16)(";
  for ( unsigned int i = 0; i < 16; ++i )
  {
    strm << _v.s[i];
    if ( i != 15 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

//------------------------------------------------------------------------------
// ulong
template< class ulongT, class traits >
inline
std::basic_ostream< ulongT, traits > & operator<<(
  std::basic_ostream< ulongT, traits > & strm,
  const cl_ulong & _v )
{
  strm << "(ulong)(" << _v << ")";
  return strm;
}

template< class ulongT, class traits >
inline
std::basic_ostream< ulongT, traits > & operator<<(
  std::basic_ostream< ulongT, traits > & strm,
  const cl_ulong2 & _v )
{
  strm << "(ulong2)(";
  for ( unsigned int i = 0; i < 2; ++i )
  {
    strm << _v.s[i];
    if ( i != 1 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class ulongT, class traits >
inline
std::basic_ostream< ulongT, traits > & operator<<(
  std::basic_ostream< ulongT, traits > & strm,
  const cl_ulong4 & _v )
{
  strm << "(ulong4)(";
  for ( unsigned int i = 0; i < 4; ++i )
  {
    strm << _v.s[i];
    if ( i != 3 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class ulongT, class traits >
inline
std::basic_ostream< ulongT, traits > & operator<<(
  std::basic_ostream< ulongT, traits > & strm,
  const cl_ulong8 & _v )
{
  strm << "(ulong8)(";
  for ( unsigned int i = 0; i < 8; ++i )
  {
    strm << _v.s[i];
    if ( i != 7 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class ulongT, class traits >
inline
std::basic_ostream< ulongT, traits > & operator<<(
  std::basic_ostream< ulongT, traits > & strm,
  const cl_ulong16 & _v )
{
  strm << "(ulong16)(";
  for ( unsigned int i = 0; i < 16; ++i )
  {
    strm << _v.s[i];
    if ( i != 15 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

//------------------------------------------------------------------------------
// long
template< class longT, class traits >
inline
std::basic_ostream< longT, traits > & operator<<(
  std::basic_ostream< longT, traits > & strm,
  const cl_long & _v )
{
  strm << "(long)(" << _v << ")";
  return strm;
}

template< class longT, class traits >
inline
std::basic_ostream< longT, traits > & operator<<(
  std::basic_ostream< longT, traits > & strm,
  const cl_long2 & _v )
{
  strm << "(long2)(";
  for ( unsigned int i = 0; i < 2; ++i )
  {
    strm << _v.s[i];
    if ( i != 1 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class longT, class traits >
inline
std::basic_ostream< longT, traits > & operator<<(
  std::basic_ostream< longT, traits > & strm,
  const cl_long4 & _v )
{
  strm << "(long4)(";
  for ( unsigned int i = 0; i < 4; ++i )
  {
    strm << _v.s[i];
    if ( i != 3 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class longT, class traits >
inline
std::basic_ostream< longT, traits > & operator<<(
  std::basic_ostream< longT, traits > & strm,
  const cl_long8 & _v )
{
  strm << "(long8)(";
  for ( unsigned int i = 0; i < 8; ++i )
  {
    strm << _v.s[i];
    if ( i != 7 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class longT, class traits >
inline
std::basic_ostream< longT, traits > & operator<<(
  std::basic_ostream< longT, traits > & strm,
  const cl_long16 & _v )
{
  strm << "(long16)(";
  for ( unsigned int i = 0; i < 16; ++i )
  {
    strm << _v.s[i];
    if ( i != 15 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

//------------------------------------------------------------------------------
// float
template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_float & _v )
{
  strm << "(float)(" << _v << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_float2 & _v )
{
  strm << "(float2)(";
  for ( unsigned int i = 0; i < 2; ++i )
  {
    strm << std::fixed << std::setprecision( 8 ) << _v.s[i];
    if ( i != 1 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_float4 & _v )
{
  strm << "(float4)(";
  for ( unsigned int i = 0; i < 4; ++i )
  {
    strm << std::fixed << std::setprecision( 8 ) << _v.s[i];
    if ( i != 3 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_float8 & _v )
{
  strm << "(float8)(";
  for ( unsigned int i = 0; i < 8; ++i )
  {
    strm << std::fixed << std::setprecision( 8 ) << _v.s[i];
    if ( i != 7 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

template< class charT, class traits >
inline
std::basic_ostream< charT, traits > & operator<<(
  std::basic_ostream< charT, traits > & strm,
  const cl_float16 & _v )
{
  strm << "(float16)(";
  for ( unsigned int i = 0; i < 16; ++i )
  {
    strm << std::fixed << std::setprecision( 8 ) << _v.s[i];
    if ( i != 15 ) { strm << ", "; }
  }
  strm << ")";
  return strm;
}

} // end namespace itk

#endif
