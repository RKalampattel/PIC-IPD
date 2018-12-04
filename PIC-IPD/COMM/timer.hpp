/*
* Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
*
* 		Copyright (c) 2015-2016 MINKWAN KIM
*
* @file timer.hpp
* @author minkwan
* @date Mar 3, 2016
* @brief Timer functions
*
*/

#ifndef TIMER_HPP_
#define TIMER_HPP_



#include <time.h>
#include "sys/time.h"
//#include <sys/resource.h>

#include "common.hpp"


namespace Common {


	/*
	* Function to express current wall time in seconds
	*/
	double dtime();



	/*
	* TimeData class
	*/
	class COMMON_API	TimeData
	{
	public:
		// Constructor
		TimeData();
		TimeData(const double &sec);

		// Destructor
		~TimeData();

		void set(const double t_in_sec);	// set the value correspondint to the give time in secconds
		std::string str() const;

		const double secPerHour = 1.0 / 3600.0;
		const double secPerMin = 1.0 / 60.0;
		const double usecPerSec = 1.0e-6;
		const double msecPerSec = 1.0e-3;

	private:
		double m_h;	// Hour
		double m_m;	// Minutes
		double m_s;	// Seconds
	};


	/*
	* Class for Time usages check
	*/
	class COMMON_API TimeInfoUsage
	{
	private:
		double m_StartTime;			// Starting time
		double m_StopTime;			// Stopping time

	private:
		double seconds() const;		// Get time difference

	public:
		TimeInfoUsage();
		void initStartTime();
		void takeStopTime();

		double getDeltaT() const;	// @return Elapsed time in seconds
		void accumulateTime(double& accTime);
	};



	/*
	* Class for Time usages check using Clock
	*/
	class COMMON_API TimeInfoClock
	{
	private:
		double m_StartTime;	// Starting time
		double m_StopTime;	// Stopping time

	private:
		double seconds() const;			// Get time difference

	public:
		TimeInfoClock();

		void initStartTime();
		void takeStopTime();

		double getDeltaT() const;		// @return Elapsed time in seconds
		void accumulateTime(double& accTime);
	};



	typedef TimeInfoUsage CPUTime;
	typedef TimeInfoClock WallTime;

	/*
	* Measure elapsed seconds
	*/
	template <typename TIMEDATA = Common::WallTime>
	class TimeStopWatch
	{
	private:
		bool		m_running;
		double		m_total;
		TIMEDATA	impl;

	private:
		void initStartTime();
		void takeStopTime();
		void accumulateTime();

	public:
		// Constructor
		TimeStopWatch() :m_running(false), m_total(0.0) { };

		// Destructor
		~TimeStopWatch() { };


		/// Checks if the StopWatch is m_running
		/// @return TRUE if it is running
		bool isRunning() const;

		/// Checks if the StopWatch is not running
		/// @return TRUE if it is not running
		bool isNotRunning() const;

		/// Stop timing. Doesn't clear the elapsed time.
		/// No effect if isNotRunning()
		/// @post m_running == false
		/// @post m_total >= 0
		void stop();

		/// Reset the timer
		/// Clears the elapsed time.
		/// @post m_running == false
		/// @post m_total == 0
		void reset();

		/// Resumes counting. Doesn't clear the elapsed time.
		/// No effect if isRunning()
		/// @post m_running == true
		/// @post m_total >= 0
		void resume();

		// Starting timming from 0.0
		// @post	m_running = true
		// @post	m_total = 0.0
		void start();

		/// Restart timing from 0.00. Same as calling start()
		/// @post m_running == true
		/// @post m_total == 0
		void restart();


		/// Read current time.
		/// @return current time in seconds
		double read() const;

		/// Converst the current time to CFdouble
		/// @return current time in seconds
		operator double() const { return read(); }

		/// Read current time.
		/// @return current time in Hour, Minutes and Seconds
		TimeData readTimeHMS();
	};


	template <typename TIMEDATA>
	bool TimeStopWatch<TIMEDATA>::isRunning() const
	{
		return m_running;
	}

	template <typename TIMEDATA>
	bool TimeStopWatch<TIMEDATA>::isNotRunning() const
	{
		return !isRunning();
	}

	template <typename TIMEDATA>
	void TimeStopWatch<TIMEDATA>::stop()
	{
		if (isRunning())
		{
			takeStopTime();
			accumulateTime();
			m_running = false;
		}
	}

	template <typename TIMEDATA>
	void TimeStopWatch<TIMEDATA>::reset()
	{
		stop();
		m_total = 0.;
	}

	template <typename TIMEDATA>
	void TimeStopWatch<TIMEDATA>::resume()
	{
		if (isNotRunning())
		{
			initStartTime();
			m_running = true;
		}
	}


	template <typename TIMEDATA>
	void TimeStopWatch<TIMEDATA>::start()
	{
		reset();
		resume();
	}


	template <typename TIMEDATA>
	void TimeStopWatch<TIMEDATA>::restart()
	{
		start();
	}


	template <typename TIMEDATA>
	double TimeStopWatch<TIMEDATA>::read() const
	{
		if (isNotRunning())	return m_total;

		return impl.getDeltaT() + m_total;
	}


	template <typename TIMEDATA>
	TimeData TimeStopWatch<TIMEDATA>::readTimeHMS()
	{
		return TimeData(read());
	}


	// Private functions
	template <typename TIMEDATA>
	void TimeStopWatch<TIMEDATA>::initStartTime()
	{
		impl.initStartTime();
	}

	template <typename TIMEDATA>
	void TimeStopWatch<TIMEDATA>::takeStopTime()
	{
		impl.takeStopTime();
	}

	template <typename TIMEDATA>
	void TimeStopWatch<TIMEDATA>::accumulateTime()
	{
		impl.accumulateTime(m_total);
	}

}


#endif /* TIMER_HPP_ */
