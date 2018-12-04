/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 *
 * 		Copyright (c) 2015-2016 MINKWAN KIM
 *
 * @file error_check.cpp
 * @author minkwan
 * @date Mar 3, 2016
 * @brief (Type brief description of file
 *
 * Detailed description
 */
#include "error_check.hpp"

#include <limits>

namespace Common
{

void ErrorCheck::NumericValue(const Common::CodeLocation& where, const std::string& valueName, double value)
{
	std::ostringstream error_message;

	if (value != value)
	{
		error_message << valueName << " is NaN";
		ExceptionError(where, error_message.str(), Common::ErrorCodes::NaN());
	}

	if (value == std::numeric_limits<double>::infinity())
	{
		error_message << valueName << " is Inf";
		ExceptionError(where, error_message.str(), Common::ErrorCodes::Inf());
	}
}

void ErrorCheck::NumericValue(const Common::CodeLocation& where, const char valueName[], double value)
{
    std::ostringstream error_message;
        
    if (value != value)
    {
        error_message << valueName << " is NaN";
        ExceptionError(where, error_message.str(), Common::ErrorCodes::NaN());
    }
        
    if (value == std::numeric_limits<double>::infinity())
    {
        error_message << valueName << " is Inf";
        ExceptionError(where, error_message.str(), Common::ErrorCodes::Inf());
    }
}
    
    

void ErrorCheck::NonNegative(const Common::CodeLocation& where, const std::string& valueName, double value)
{
	std::ostringstream error_message;

	ErrorCheck::NumericValue(where, valueName, value);
	if (value < 0.0)
	{
		error_message << valueName << " is negative";
		ExceptionError(where, error_message.str(), Common::ErrorCodes::Negative());
	}
}

void ErrorCheck::NonNegative(const Common::CodeLocation& where, const char valueName[], double value)
{
    std::ostringstream error_message;
        
    ErrorCheck::NumericValue(where, valueName, value);
    if (value < 0.0)
    {
        error_message << valueName << " is negative";
        ExceptionError(where, error_message.str(), Common::ErrorCodes::Negative());
    }
}

} /* namespace Common */
