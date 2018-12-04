/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 *
 * 		Copyright (c) 2015-2016 MINKWAN KIM
 *
 * @file processorinfo.cpp
 * @author minkwan
 * @date Mar 10, 2016
 * @brief (Type brief description of file
 *
 * Detailed description
 */

#include "error_codes.hpp"
#include "error_exception.hpp"
#include "processorinfo.hpp"

namespace Common {

processor_info_MIC::processor_info_MIC()
    :num_CPU(-1), taskID(-1), num_threads(-1), num_MIC(-1),
	 m_set_CPU(false), m_set_MIC(false)
{
	for (int i = 0; i <= MAX_MIC_PER_NODE-1; i++) MIC_ID[i] = -1;
	for (int i = 0; i <= MAX_MIC_PER_NODE-1; i++) num_threads_MIC[i] = -1;
}


processor_info_MIC::processor_info_MIC(int nthreads_CPU, int nMIC)
    :num_CPU(-1), taskID(-1), num_threads(nthreads_CPU), num_MIC(nMIC)
{
	for (int i = 0; i <= MAX_MIC_PER_NODE-1; i++) MIC_ID[i] = i;
	for (int i = 0; i <= MAX_MIC_PER_NODE-1; i++) num_threads_MIC[i] = -1;

	m_set_CPU = true;
	m_set_MIC = true;
}


processor_info_MIC::processor_info_MIC(int nthreads_CPU, int nMIC, std::vector<int> i_MIC_ID)
    :num_CPU(-1), taskID(-1), num_threads(nthreads_CPU), num_MIC(nMIC)
{
	if (i_MIC_ID.size() < nMIC)	Common::ExceptionError(FromHere(), "Problem in number of MIC per node", Common::ErrorCodes::ExceedLimit());

	for (int i = 0; i <= nMIC; i++) MIC_ID[i] = i_MIC_ID[i];
	for (int i = 0; i <= 3; i++) num_threads_MIC[i] = -1;

	m_set_CPU = true;
	m_set_MIC = true;
}





void processor_info_MIC::set_info(int nthreads_CPU, int nMIC)
{
	if (m_set_CPU != true)
	{
		num_threads = nthreads_CPU;
		m_set_CPU = true;
	}


	if (m_set_MIC != true)
	{
		num_MIC     = nMIC;
		for (int i = 0; i <= nMIC-1; i++) MIC_ID[i] = i;
		for (int i = 0; i <= nMIC-1; i++) num_threads_MIC[i] = -1;

		m_set_MIC = true;
	}
}



void processor_info_MIC::set_info(int nthreads_CPU, int nMIC, std::vector<int> i_MIC_ID)
{
	if (m_set_CPU != true)
	{
		num_threads = nthreads_CPU;
		m_set_CPU = true;
	}


	if (m_set_MIC != true)
	{
		num_MIC     = nMIC;
		for (int i = 0; i <= nMIC-1; i++) MIC_ID[i] = i_MIC_ID[i];
		for (int i = 0; i <= nMIC-1; i++) num_threads_MIC[i] = -1;

		m_set_MIC = true;
	}
}

void processor_info_MIC::update_MIC_info(int nMIC, std::vector<int> i_MIC_ID)
{
	num_MIC     = nMIC;
	for (int i = 0; i <= nMIC-1; i++) MIC_ID[i] = i_MIC_ID[i];
	for (int i = 0; i <= nMIC-1; i++) num_threads_MIC[i] = -1;

	m_set_MIC = true;
}

void processor_info_MIC::update_MIC_info(int nMIC)
{
	num_MIC     = nMIC;
	for (int i = 0; i <= nMIC-1; i++) MIC_ID[i] = i;
	for (int i = 0; i <= nMIC-1; i++) num_threads_MIC[i] = -1;
}


void processor_info_MIC::update_CPU_info(int nthreads_CPU)
{
	num_threads = nthreads_CPU;
	m_set_CPU = true;
}



void processor_info_MIC::show_info()
{
	std::cout << "[Processor Information]" << std::endl;
	std::cout << "=======================" << std::endl << std::endl;
	std::cout << "Total number of CPUs:             " << num_CPU << std::endl;
	std::cout << "Total number of threads (OpenMP): " << num_threads << std::endl;

#ifdef MIC
	std::cout << "Total number of MICs:             " << num_CPU*num_MIC << std::endl;

	int tmp_thread_mic = 0;
	for (int k = 0; k <= num_MIC-1; k++) tmp_thread_mic += num_threads_MIC[k];
	std::cout << "Total number of threads (MIC):    " << num_CPU*tmp_thread_mic << std::endl;
#else
	std::cout << "(MIC is not used)" << std::endl;
#endif

	std::cout << std::endl << std::endl;
}


} /* namespace Common */
