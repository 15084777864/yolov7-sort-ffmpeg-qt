/********************************************************************************
** @auth£º taify
** @date£º 2023/04/21
** @Ver :  V1.0.0
** @desc£º kalmanboxtrackerÍ·ÎÄ¼þ
*********************************************************************************/


#pragma once


#include <iostream>
#include <opencv2/opencv.hpp>



class KalmanBoxTracker
{
public:

	KalmanBoxTracker(std::vector<int> bbox);
	

	void update(std::vector<int> bbox);	
	

	std::vector<float> predict();

	std::vector<float> get_state();
	
public:

	cv::KalmanFilter* m_kf;


	int m_time_since_update;


	static int count;


	int m_id;

	std::vector<std::vector<float>> m_history;

	int m_hits;

	int m_hit_streak;

	int m_age;
};
