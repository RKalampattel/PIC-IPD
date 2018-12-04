/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 * 
 *         Copyright (c) 2014-2016 MINKWAN KIM
 *
 * StringOps.hpp
 *
 *  Created on: 21 Mar 2016
 *      Author: a1633125
 *
 *	@brief Brief description of the file
 */

#ifndef COMM_STRINGOPS_HPP_
#define COMM_STRINGOPS_HPP_

#include <vector>

#include "common.hpp"
#include "noninstantiable.hpp"


namespace Common {

class StringOps: public NonInstantiable<StringOps> {

public:

	/*
	 * Joins the array of parts, using the delimiter
	 * [delim]  = Missing documentation
	 * [parts]  = Missing documentation
	 * [nparts] = Missing documentation
	 * [out]    = string to modify
	 */
	static void join (const std::string& delim, std::string parts[], unsigned int nparts, std::string& out);
	static void join (const std::string& delim, std::string& part, std::string& out);



	/*
	 * Modifies string to lowercase.
	 */
	static void toLower (std::string& out);

	/*
	 * Modifies string to uppercase.
	 */
	static void toUpper (std::string& out);


	/*
	 * Substitute the rhs for the lhs wherever it appears in the string
	 * [lhs]	= partial std::string to match
	 * [rhs]	= partial std::string to substitute
	 * [out[	= string to modify
	 */
	static void subst (const std::string& lhs, const std::string& rhs, std::string& out);


	// Removes the leading whitespace.
	static void trimFront (std::string& out);

	// Removes the trailing whitespaces.
	static void trimRear (std::string& out);

	// Removes the trailing and leading whitespace.
	static void trim (std::string& out);

	// Removes the trailing and leading whitespace.
	static void trim2 (std::string& out);

	// Splits the given std::string on whitespace, and puts the words into the given vector.
	static std::vector<std::string> getWords (const std::string& in);

	/*
	 * Splits the given std::string on the passed characters, and puts the words into the given vector.
	 * [in]	 = string to split
	 * [set] = the separator character
	 */
	static std::vector<std::string> getWords  (const std::string& in, const char sep );

	/*
	 * Returns whether this std::string starts with the given std::string.
	 * [in]	 = string to split
	 * [str] = string start with
	 */
	static bool startsWith (const std::string& in, const std::string& str);

	/*
	 * Returns whether this std::string ends with the given std::string.
	 * [in]	 = string to split
	 * [str] = string end with
	 */
	static bool endsWith (const std::string& in, const std::string& str);



	// Converts from std::string
	template <class T>
	static T from_str (const std::string& str)
	{
		T v;
		if (str.length() > 0)
		{
			std::istringstream iss(str.c_str());
			iss >> v;
		}
		else
		{
			v = T();
		}
		return v;
	}

    // Converts to std::string
    template <class T>
    static std::string to_str (const T& value)
    {
        std::stringstream ss;		//create a stringstream
        ss << value;           //add number to the stream
        return ss.str();		//return a string with the contents of the stream
    }


	static std::string convertYesNo (const int i);
    



	static std::string makeVAR_Sub(const std::string& var, const std::string& sub)
	{
		std::ostringstream oss;
		oss << var <<"_" <<sub;
		return oss.str();
	}

	static std::string makeVAR_Sub_Num(const std::string& var, const std::string& sub, int num)
	{
		std::ostringstream oss;
		oss << var <<"_" << sub << num;
		return oss.str();
	}

	static std::string makeVAR_Sub_Unit(const std::string& var, const std::string& sub, const std::string& unit)
	{
		std::ostringstream oss;
		oss << var <<"_" <<sub <<" [" <<unit<<"]";
		return oss.str();
	}


	static std::string makeVAR_Sub_Tecplot(const std::string& var, const std::string& sub)
	{
		std::ostringstream oss;
		oss << var <<"<sub>" << sub <<"</sub>";
		return oss.str();
	}

	static std::string makeVAR_Sub_Unit_Tecplot(const std::string& var, const std::string& sub, const std::string& unit)
	{
		std::ostringstream oss;
		oss << var <<"<sub>" << sub <<"</sub>" << " [" <<unit<<"]";;
		return oss.str();
	}


	static void split(const std::string& s, char c, std::vector<std::string>& v)
	{
		std::string::size_type i = 0;
		std::string::size_type j = s.find(c);

		while (j != std::string::npos)
		{
			v.push_back(s.substr(i, j-i));
			i = ++j;
			j = s.find(c, j);

			if (j == std::string::npos)
				v.push_back(s.substr(i, s.length()));
		}
	}

	template <class T>
	static T extractingNumber(std::string& s)
	{
		T c = -1;
		std::stringstream str_temp(s);
		str_temp >> c >> s;

		if (c == -1 || c == 0)	c = 1.0;
		return (c);
	}




};

} /* namespace Common */

#endif /* COMM_STRINGOPS_HPP_ */
