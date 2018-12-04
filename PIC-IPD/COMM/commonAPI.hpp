/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 *
 * 		Copyright (c) 2015-2016 MINKWAN KIM
 *
 * @file commonAPI.hpp
 * @author minkwan
 * @date Mar 1, 2016
 * @brief Define the macro CommonAPI
 *
 */

#ifndef COMMONAPI_HPP_
#define COMMONAPI_HPP_


#include "exportAPI.hpp"

#ifdef COMMON_EXPORT
    #define COMMON_API	MK_EXPORT_API
#else
    #define COMMON_API	MK_IMPORT_API
#endif



#endif /* COMMONAPI_HPP_ */
