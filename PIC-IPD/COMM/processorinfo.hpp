/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 *
 * 		Copyright (c) 2015-2016 MINKWAN KIM
 *
 * @file processorinfo.hpp
 * @author minkwan
 * @date Mar 10, 2016
 * @brief (Type brief description of file
 *
 * Detailed description
 */

#ifndef PROCESSORINFO_HPP_
#define PROCESSORINFO_HPP_

#include <vector>

#define MAX_CPUs			100
#define MAX_MIC_PER_NODE	4
#define THREAD_PER_MIC_CORE	4
#define THREAD_PER_CPU		2



namespace Common {

class processor_info_MIC {
public:
	processor_info_MIC();
	processor_info_MIC(int nthreads_CPU, int nMIC);
	processor_info_MIC(int nthreads_CPU, int nMIC, std::vector<int> i_MIC_ID);

	int num_CPU;		    // Total number of Processor (CPUs)
	int num_MIC;		    // Number of MICs

	// CPU info
	int taskID;			    // Current CPU ID
	int num_threads;	    // Number of thread for CPUs

	// MIC info
	int MIC_ID[MAX_MIC_PER_NODE];		   // MIC ID list
	int num_threads_MIC[MAX_MIC_PER_NODE]; // Number of thread for MIC

	void show_info();
	void set_info(int nthreads_CPU, int nMIC);
	void set_info(int nthreads_CPU, int nMIC, std::vector<int> i_MIC_ID);

	void update_MIC_info(int nMIC, std::vector<int> i_MIC_ID);
	void update_MIC_info(int nMIC);
	void update_CPU_info(int nthreads_CPU);

private:
	bool m_set_CPU;
	bool m_set_MIC;
};



} /* namespace Common */

#endif /* PROCESSORINFO_HPP_ */
