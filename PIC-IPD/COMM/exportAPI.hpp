/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 *
 * 		Copyright (c) 2015-2016 MINKWAN KIM
 *
 * @file exportAPI.hpp
 * @author minkwan
 * @date Mar 1, 2016
 * @brief This contains the setting for API
 */

#ifndef EXPORTAPI_HPP_
#define EXPORTAPI_HPP_


// For Window (Visual studio)
#if defined (_MSC_VER)
    #define MK_LOCAL_API
    #define MK_IMPORT_API	__declspec(dllimport)
    #define MK_EXPORT_API	__declspec(dllexport)
#endif


// For Linux (GNU)
#if defined (__GNUC__)
    #define MK_LOCAL_API    __attribute__ ((visibility("hidden")))
    #define MK_EXPORT_API   __attribute__ ((visibility("default")))
    #define MK_IMPORT_API   __attribute__ ((visibility("default")))
#endif


// For Other Compliers
#ifndef MK_LOCAL_API
    #if defined (ACCEPT_ANY_COMPILER)
        #define OP_LOCAL_API
        #define OP_EXPORT_API
        #define OP_IMPORT_API
    #else
        #error "Unrecognized compiler and/or platform."
    #endif
#endif



#endif /* EXPORTAPI_HPP_ */
