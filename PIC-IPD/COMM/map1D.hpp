/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 * 
 *         Copyright (c) 2014-2016 MINKWAN KIM
 *
 * map1D.hpp
 *
 *  Created on: 21 Mar 2016
 *      Author: a1633125
 *
 *	@brief Brief description of the file
 */

#ifndef COMM_MAP1D_HPP_
#define COMM_MAP1D_HPP_



#include "assert_mk.hpp"
#include "StringOps.hpp"
#include "error_codes.hpp"
#include "error_exception.hpp"


#define DEFAULT_SIZE_MAP1D	5

namespace Common{

template <typename KEY, typename VALUE>
class Map1D
{
public:
	Map1D(size_t maxSize) : m_sorted(false)
	{
		op_assert(maxSize > 0);
		m_vectorMap.reserve(maxSize);
	}

	Map1D() : m_sorted(false)
	{
		m_vectorMap.reserve(DEFAULT_SIZE_MAP1D);
	}


	~Map1D()
	{

	}


	/*
	 * =========================================
	 * Internal Functions
	 * =========================================
	 */
	// F01 - reserve
	// @param size of the map to be set before starting inserting pairs in the  map
	// @post  the memory corresponding to the given size will be reserved for the future insertions.
	void reserve(size_t maxSize)
	{
		m_vectorMap.reserve(maxSize);
	}


	// F02 - insert
	// @param key   new key to be inserted
	// @param value new value to be inserted, corresponding to the given key
	// @post a new std::pair<KEY, VALUE> will be created and pushed back in the map
	// @post the capacity of the map will increase only if the memory hasn't been reserved properly at start up.
	void insert(const KEY& aKey, const VALUE& aValue)
	{
		m_sorted = false;
		m_vectorMap.push_back(std::pair<KEY, VALUE>(aKey, aValue));
	}


	// F03 - findBounds
	// @brief Find the upper and lower pairs of KEY'S and VALUE'S bounding the supplied KEY.
	// @pre before using findBounds() the Map1D has to be sorted with sortKeys()
	std::pair<typename std::vector<typename std::pair<KEY,VALUE> >::iterator, typename std::vector<typename std::pair<KEY,VALUE> >::iterator> findBounds(const KEY& aKey)
	{
		if(m_vectorMap.empty())
		{
			valueNotFound(aKey);
		}

		if(!m_sorted)
		{
			sortKeys();
		}

		if(aKey < m_vectorMap.front().first)
		{
			return std::make_pair(m_vectorMap.begin(),m_vectorMap.begin());
		}

		if(aKey > m_vectorMap.back().first)
		{
			return std::make_pair(--m_vectorMap.end(),--m_vectorMap.end());
		}

		typename std::vector<std::pair<KEY,VALUE> >::iterator after = lower_bound(m_vectorMap.begin(),m_vectorMap.end(), aKey,Compare());
		typename std::vector<std::pair<KEY,VALUE> >::iterator before = after;

		--before;

		return std::make_pair(before,after);
	}


	// F04 - find
	// @brief Find VALUE with the given KEY
	// @param key  key to be looked-up
	// @pre before using find() the CFMap has to be sorted with sortKeys()
	// @post if the key is not in the map, a NoSuchValueException is thrown
	// @return copy of found VALUE
	// @exception NoSuchValueException is thrown if key not in map
	VALUE find(const KEY& aKey)
	{
		if(m_vectorMap.empty())
		{
			valueNotFound(aKey);
		}

		if(!m_sorted)
		{
			sortKeys();
		}

		typename std::vector<std::pair<KEY,VALUE> >::iterator itr = std::lower_bound(m_vectorMap.begin(), m_vectorMap.end(), aKey, Compare());

		if (itr == m_vectorMap.end())
		{
			valueNotFound(aKey);
		}

		if (itr->first != aKey)
		{
			valueNotFound(aKey);
		}

		op_assert(itr->first == aKey); // must have one at least

		return itr->second;
	}

	/// @param key      key to be looked-up
	/// @param isFound  flag telling if the value has been found
	/// @pre before using find() the CFMultiMap has to be sorted with sortKeys()
	/// @return copy of found VALUE
	VALUE find(const KEY& aKey, bool& isFound)
	{
		op_assert(!m_vectorMap.empty());
		op_assert(m_sorted);

		typename std::vector<std::pair<KEY,VALUE> >::iterator itr = std::lower_bound(m_vectorMap.begin(),m_vectorMap.end(), aKey, Compare());

		// this can fail if only one key is inserted !!!
		isFound = (itr->first == aKey);
		op_assert(m_vectorMap.size() > 1);

		return itr->second;
	}


	// F05 - exitst
	// @bries Check if the given KEY is associated to a VALUE
	// @param key  key to be looked-up
	// @pre before using exists() the CFMap has to be sorted with sortKeys()
	// @return flag to know if key exists
	bool exists(const KEY& aKey)
	{
		if(m_vectorMap.empty())
		{
			return false;
		}

		if(!m_sorted)
		{
			sortKeys();
		}

		typename std::vector<std::pair<KEY,VALUE> >::iterator itr = std::lower_bound(m_vectorMap.begin(), m_vectorMap.end(), aKey, Compare());

		if (itr == m_vectorMap.end())
		{
			return false;
		}

		if (itr->first != aKey)
		{
			return false;
		}

		op_assert(itr->first == aKey); // must have one at least

		return true;
	}


	// F06 - print
	// @brief print the content of the map
	void print()
	{
		const unsigned int sizeM = m_vectorMap.size();

		for (unsigned int i = 0; i < sizeM; ++i)
		{
			std::cout << m_vectorMap[i].first << " => " << m_vectorMap[i].second << "\n";
		}
	}


	// F07 - clear
	// @brief Clear the content of the map
	void clear()
	{
		std::vector<std::pair<KEY,VALUE> >().swap(m_vectorMap);
	}


	// F08 - clearContent
	// Clear the content of the map while keeping its size
	void clearContent()
	{
		if (size() > 0 ) {m_vectorMap.clear();}
		op_assert(size() == 0);
	}

	// F09 - size
	/// Get the number of pairs already inserted
	size_t size() const
	{
		return m_vectorMap.size();
	}


	// F10 - []
	// @brief Overloading of the operator"[]" for assignment
	// @return m_vectorMap[i].second
	VALUE& operator[] (const unsigned int i)
	{
		op_assert(i < size());
		return m_vectorMap[i].second;
	}

	// F11 - sortKeys
	// @brief Sort all the pairs in the map by key
	// @pre before using find() the Map1D has to be sorted
	void sortKeys()
	{
		std::sort(m_vectorMap.begin(), m_vectorMap.end(), LessThan());
		m_sorted = true;
	}


	// F12 - begin
	// @return the iterator pointing at the first element
	typename std::vector<std::pair<KEY, VALUE> >::iterator begin()
	{
		return m_vectorMap.begin();
	}


	// F13 - end
	// @return the iterator pointing at the last element
	typename std::vector<std::pair<KEY, VALUE> >::iterator end()
	{
		return m_vectorMap.end();
	}


	//F14 - replaceValue
	// @brief replace the value of the key: Key, with the value: newValue
	void replaceValue(KEY Key, VALUE newValue)
	{
		if(!m_sorted)
		{
			sortKeys();
		}

		VALUE oldValue = find(Key);
		std::pair<KEY, VALUE > oldPair(Key, oldValue);
		std::pair<KEY, VALUE > newPair(Key, newValue);

		replace(m_vectorMap.begin(), m_vectorMap.end(), oldPair, newPair);
	}


	// F15 - getKey
	// @brief get the key corresponding to position i
	KEY getKey(const unsigned int i)
	{
		op_assert(i < size());

		if(!m_sorted)
		{
			sortKeys();
		}

		return m_vectorMap[i].first;
	}

	// F16 - get Key corresponding value
	KEY findKey(VALUE value)
	{
		KEY	key;

		if(m_vectorMap.empty())	keyNotFound(value);

		for (int i = 0; i <= m_vectorMap.size(); i++)
		{
			if (m_vectorMap[i].second == value)
			{
				key = m_vectorMap[i].first;
				return(key);
			}
		}

		if(m_vectorMap.empty())	keyNotFound(value);
		return(key);
	}

	// F17 - Description of the Map variable
	void setDescrip(std::string& description)
	{
		m_description = description;
	}

	std::string getDescrip()
	{
		return (m_description);
	}


/* ============================================
 * Nested Classes
 * ============================================
 */
private:
	/*
	 * This class represents a functor object that is passed as an argument in the std::sort to compare two given pairs.
	 */
	class LessThan
	{
	public:
		/// Overloading of the operator() that makes this class acting as a functor
		/// @param p1  pair to be used as first term of comparison
		/// @param p2  pair to be used as second term of comparison
		/// @return true  if(p1.first < p2.first)
		/// @return false if(p1.first >= p2.first)
		/// @post the pairs will be ordered according to the increasing order of
		///       their keys
		/// @post sortKeys() uses these function to order the inserted pairs
		bool operator() (const std::pair<KEY,VALUE>& p1,	const std::pair<KEY,VALUE>& p2) const
		{
			return (p1.first < p2.first) ? true : false;
		}
	}; // end class LessThan


	/*
	 * This class represents a functor object that is passed as an argument in the std::pair to compare two given pairs.
	 * This functor is passed as an argument in the std::equal_range function in order to find all the pairs containing the specified key
	 */
	class Compare
	{
	public:

		/// Overloading of the operator() that makes this class acting as a functor
		/// @param p1   pair whose key is used as first term of comparison
		/// @param key  given key used as second term of comparison
		/// @return true  if(p1.first < key)
		/// @return false if(p1.first >= key)
		/// @post this is the first test to see if p1.first is == key during the
		///       search with find()
		bool operator() (const std::pair<KEY,VALUE>& p1, const KEY& key) const
		{
			return (p1.first < key) ? true : false;
		}

		/// Overloading of the operator() that makes this class acting as a functor
		/// @param key  given key used as first term of comparison
		/// @param p1   pair whose key is used as second term of comparison
		/// @return true  if(p1.first > key)
		/// @return false if(p1.first <= key)
		/// @post this is the second test to see if p1.first is == key during the
		///       search with find()
		bool operator() (const KEY& key, const std::pair<KEY,VALUE>& p1) const
		{
			return (p1.first > key) ? true : false;
		}
	};


private:
	void valueNotFound(const KEY& aKey)
	{
		std::string msg = "CFMap: KEY not found: ";
		msg += Common::StringOps::to_str(aKey);
        throw Common::ExceptionError(FromHere(), msg, Common::ErrorCodes::NoSuchValue());
	}

	void keyNotFound(const VALUE& aValue)
	{
		std::string msg = "CFMap: VALUE not found: ";
		msg += Common::StringOps::to_str(aValue);
		throw Common::ExceptionError(FromHere(), msg, Common::ErrorCodes::NoSuchValue());
	}


	/*
	 * DATA [Private]
	 */
private:
	bool m_sorted;
	std::vector<std::pair<KEY,VALUE> >  m_vectorMap;
	std::string m_description;
};


}


#endif /* COMM_MAP1D_HPP_ */
