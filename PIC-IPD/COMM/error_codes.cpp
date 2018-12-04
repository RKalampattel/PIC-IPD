/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 *
 * 		Copyright (c) 2015-2016 MINKWAN KIM
 *
 * @file error_codes.cpp
 * @author minkwan
 * @date Mar 3, 2016
 * @brief (Type brief description of file
 *
 * Detailed description
 */
#include "error_codes.hpp"

#include <string>
#include <sstream>

namespace Common {

    std::string ErrorCodes::MakeDescription(unsigned int code_main, unsigned int code_sub, std::string namefunction)
    {
        std::ostringstream res;
        
        switch (code_main)
        {
            case 0:
                res << "CRITICAL ERROR !!";
                break;
                
            case 1:
                res << "IGNOABLE ERROR";
                break;
                
            case 2:
                res << "!!WARNING!!";
                break;
                
            case 3:
                res << "Non-Physical ERROR !!";
                break;
                
            case 4:
                res << " SYSTEM ERROR";
                break;
                
            case 5:
                res << "- DEBUG MODE - ";
                break;
                
        }
        res << " [" << namefunction << "] has ";
        
        switch (code_sub)
        {
            case 0:
                res << "data-value mismatch";
                break;
                
            case 1:
                res << "data-type mismatch";
                break;
                
            case 2:
                res << "data-size/dimension mismatch";
                break;
                
            case 10:
                res << "not allocated data";
                break;
                
            case 11:
                res << "not supported data-type";
                break;
                
            case 12:
                res << "not supported option";
                break;
                
            case 20:
                res << "Negative value";
                break;
                
            case 21:
                res << "NaN value";
                break;
                
            case 22:
                res << "Inf value";
                break;
                
            case 23:
                res << "Noexist value";
                break;
                
            case 24:
                res << "Exceed the limit";
                break;
                
            case 30:
                res << "File opening";
                break;
                
            case 31:
                res << "No such file";
                break;
        }
        
        res << "  ISSUE !!";
        
        return (res.str());
    }
    
    
    std::string ErrorCodes::MakeDescription(unsigned int code_main, unsigned int code_sub, std::string namefunction, std::string varName, const double value)
    {
        std::ostringstream res;

        res << MakeDescription(code_main, code_sub, namefunction);
        res << "  :   " << varName << " = " << value;
        
        return (res.str());
    }

    
    
    
std::string ErrorCodes::Mismatch()
{
    return ("Micmatch");
}

std::string ErrorCodes::Mismatch(std::string &details)
{
	std::ostringstream ss;
	ss << "Mismatch " << details <<":";
	return (ss.str());
}
    
std::string ErrorCodes::MismatchDimension()
{
	return("Dimension");
}

std::string ErrorCodes::MismatchData()
{
	return("Micmatch Data value");
}

std::string ErrorCodes::MismatchDataType()
{
	return("Mismatch Data type:");
}

std::string ErrorCodes::NotSupportedType()
{
	return ("NotSupportedType:");
}

std::string ErrorCodes::NotAllocated()
{
	return ("NotSupportedType:");
}

std::string ErrorCodes::Negative()
{
	return ("Negative value:");
}

std::string ErrorCodes::NaN()
{
	return ("NaN value:");
}

std::string ErrorCodes::Inf()
{
	return ("Inf value:");
}

std::string ErrorCodes::ExceedLimit()
{
    return ("ExceedLimit:");
}

std::string ErrorCodes::FileSystem()
{
	return ("FileSystem:");
}

std::string ErrorCodes::FailAssertion()
{
	return ("FailAssertion:");
}

std::string ErrorCodes::NoSuchValue()
{
	return ("NoSuchValue:");
}
    
    
    
    
    void OP2A_ERROR(const Common::CodeLocation& where, const unsigned int code_main, const unsigned int code_sub, std::string namefunction)
    {
        std::string what;
        what = ErrorCodes::MakeDescription(code_main, code_sub, namefunction);
        Common::ExceptionError(where, what, namefunction);
    }

    void OP2A_ERROR2(const Common::CodeLocation& where, const unsigned int code_main, const unsigned int code_sub, std::string namefunction, std::string varName, const double value)
    {
        std::string what;
        what = ErrorCodes::MakeDescription(code_main, code_sub, namefunction, varName, value);
        Common::ExceptionError(where, what, namefunction);
    }
    
    
    

} /* namespace Common */
