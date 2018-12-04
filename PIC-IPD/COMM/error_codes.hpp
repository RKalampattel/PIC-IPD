/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 *
 * 		Copyright (c) 2015-2016 MINKWAN KIM
 *
 * @file error_codes.hpp
 * @author minkwan
 * @date Mar 3, 2016
 * @brief (Type brief description of file
 *
 * Detailed description
 */

#ifndef ERROR_CODES_HPP_
#define ERROR_CODES_HPP_


#include "common.hpp"
#include "noninstantiable.hpp"
#include "error_exception.hpp"


namespace Common {

    
    enum ERROR_CODE_MAIN
    {
        ErrorCritical    = 0,
        ErrorIgnorable   = 1,
        ErrorWarning     = 2,
        ErrorNonPhysical = 3,
        ErrorSystem      = 4,
        ErrorDebugMode   = 5
    };
    
    enum ERROR_CODE_SUB
    {
        MismatchData     = 0,
        MismatchDataType = 1,
        MismatchDataSize = 2,
        
        NotAllocatedData     = 10,
        NotSupportedDataType = 11,
        NotSupportedOption   = 12,

        
        ValueNegative    = 20,
        ValueNaN         = 21,
        ValueInf         = 22,
        ValueNoExist     = 23,
        ValueExceedLimit = 24,
        
        FileCannotOpen   = 30,
        FileNoExit       = 31
        
    };
    
    
class ErrorCodes: NonInstantiable<ErrorCodes>
{
public:
    static std::string MakeDescription(unsigned int code_main, unsigned int code_sub, std::string namefunction);
    static std::string MakeDescription(unsigned int code_main, unsigned int code_sub, std::string namefunction, std::string varName, const double value);

    
    
    
    
	static std::string Mismatch();
	static std::string Mismatch(std::string &details);

	static std::string MismatchDimension();
	static std::string MismatchData();
	static std::string MismatchDataType();

	static std::string NotSupportedType();
	static std::string NotAllocated();

	static std::string Negative();
	static std::string NaN();
	static std::string Inf();

	static std::string ExceedLimit();
	static std::string FileSystem();

	static std::string FailAssertion();
	static std::string NoSuchValue();

};
    
    
    void OP2A_ERROR(const Common::CodeLocation& where, const unsigned int code_main, const unsigned int code_sub, std::string namefunction);
    void OP2A_ERROR2(const Common::CodeLocation& where, const unsigned int code_main, const unsigned int code_sub, std::string namefunction, std::string varName, const double value);



} /* namespace Common */

#endif /* ERROR_CODES_HPP_ */
