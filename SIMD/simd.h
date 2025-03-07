/***************************************************************************/
/*                                                                         */
/*  SIMD.h                                                                 */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

#ifndef __SIMD_H__
#define __SIMD_H__


//	First include global macros definitions
#ifndef __SIMDMACROS_H__
	#include "simdMacros.h"
#endif


//	SIMD CPU register classes
#ifndef AFX_SIMDREGISTER_H__7A170774_BE41_42B3_8BA3_415C50B6FD6D__INCLUDED_
    #ifndef SIMD_NO_ASSEMBLY
	    #include "SIMDRegister.cxx"
    #endif
#endif


//	Generic Vector template class definition 
#ifndef __GENERIC_VECTOR_IMPL__
	#include "GenericVector.cxx"
#endif

#ifndef __GENERIC_ALIGNED_VECTOR_IMPL__
	#include "GenericAlignedVector.cxx"
#endif

#ifndef SIMD_NO_ASSEMBLY
	//	4 Float values Vector
	#if !defined(AFX_SSEFLOATVECTOR_H__9AAB93A4_DB7C_4486_802A_1B4CE4E6CEC4__INCLUDED_)
		#include "SSEFVector.h"
	#endif

	#ifndef __VECTOR_4F_H__
		#include "Vector4f.h"
	#endif

	//	4 Word values Vector
	#if !defined(AFX_WVector_H__7493DD94_5C3A_4025_9FA6_C8F0EC4BE053__INCLUDED_)
		#include "WVector.h"
	#endif

	#if !defined(AFX_MMXWVector_H__EDBB336A_5AF7_4AF0_9000_1F0F0CE7C0C0__INCLUDED_)
		#include "MMXWVector.h"
	#endif

	// 4 Byte values Vector
	#if !defined(AFX_BYTEVECTOR_H__A9932FAD_211E_4F03_BF5B_C0E608372FE8__INCLUDED_)
		#include "BVector.h"
	#endif

	#if !defined(AFX_MMXBYTEVECTOR_H__8C6D4565_1B55_4069_9E6A_54484A482CA1__INCLUDED_)
		#include "MMXBVector.h"
	#endif
#endif	// SIMD_NO_ASSEMBLY

//	Generic Matrix template class definition 
#ifndef __GENERIC_MATRIX_IMPL__
	#include "GenericMatrix.cxx"
#endif

#ifndef __GENERIC_ALIGNED_MATRIX_H__
	#include "GenericAlignedMatrix.h"
#endif

#ifndef SIMD_NO_ASSEMBLY
	//	16 Word values Matrix
	#if !defined(AFX_MMXWMATRIX_H__6CB7BBD5_103F_4C28_AA90_88AA8AD9E466__INCLUDED_)
		#include "MMXWMatrix.h"
	#endif

	//	16 Float values Matrix
	#if !defined(AFX_SSEFMATRIX_H__7923E89B_7F26_4EE5_8038_15480F2B7985__INCLUDED_)
		#include "SSEFMatrix.h"
	#endif

	#ifndef __MATRIX_4F_H__
		#include "Matrix4f.h"
	#endif
#endif	// SIMD_NO_ASSEMBLY

#endif	// __SIMD_H__

