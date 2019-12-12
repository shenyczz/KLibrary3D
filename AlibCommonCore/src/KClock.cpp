#include "stdafx.h"
#include "KClock.h"


KClock::KClock()
	: m_BaseTime(0)
	, m_PausedTime(0)
	, m_StopTime(0)
	, m_Stopped(false)
	, m_PrevTime(0)
	, m_CurrTime(0)
	, m_DeltaTime(-1.0)
	, m_SecondsPerCount(0)
{
	__int64 countsPerSec;
	::QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_SecondsPerCount = 1.0 / (double)countsPerSec;
}

KClock::~KClock()
{
}

// Call before message loop.
void KClock::Reset()
{
	__int64 currTime;
	::QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = 0;
	m_Stopped = false;

	return;
}

// Call when unpaused.
void KClock::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	// Accumulate the time elapsed between stop and start pairs.
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  m_BaseTime      m_StopTime        startTime     

	if (m_Stopped)
	{
		m_PausedTime += (startTime - m_StopTime);

		m_PrevTime = startTime;
		m_StopTime = 0;
		m_Stopped = false;
	}

	return;
}

// Call when paused.
void KClock::Stop()
{
	if (!m_Stopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_StopTime = currTime;
		m_Stopped = true;
	}

	return;
}

// Call every frame.
void KClock::Tick()
{
	if (m_Stopped)
	{
		m_DeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_CurrTime = currTime;

	// Time difference between this frame and the previous.
	m_DeltaTime = (m_CurrTime - m_PrevTime)*m_SecondsPerCount;

	// Prepare for next frame.
	m_PrevTime = m_CurrTime;

	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.
	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}

	return;
}


float KClock::DeltaTime()const
{
	return (float)m_DeltaTime;
}


// Returns the total time elapsed since Reset() was called, NOT counting any
// time when the clock is stopped.
float KClock::TotalTime() const
{
	float totalTime = 0;

	// If we are stopped, do not count the time that has passed since we stopped.
	// Moreover, if we previously already had a pause, the distance 
	// mStopTime - mBaseTime includes paused time, which we do not want to count.
	// To correct this, we can subtract the paused time from mStopTime:  
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------------*------> time
	//  m_BaseTime      m_StopTime       startTime     m_StopTime   m_CurrTime

	if (m_Stopped)
	{
		totalTime = (float)(((m_StopTime - m_PausedTime) - m_BaseTime)*m_SecondsPerCount);
	}

	// The distance mCurrTime - mBaseTime includes paused time,
	// which we do not want to count.  To correct this, we can subtract 
	// the paused time from mCurrTime:  
	//
	//  (mCurrTime - mPausedTime) - mBaseTime 
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mCurrTime

	else
	{
		totalTime = (float)(((m_CurrTime - m_PausedTime) - m_BaseTime)*m_SecondsPerCount);
	}

	return totalTime;
}

