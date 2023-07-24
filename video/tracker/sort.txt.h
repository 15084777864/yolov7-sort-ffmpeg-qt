/********************************************************************************
** @auth£º taify
** @date£º 2023/04/21
** @Ver :  V1.0.0
** @desc£º sortÔ´ÎÄ¼þ
*********************************************************************************/


#pragma once


#include <iostream>
#include <opencv2/opencv.hpp>

#include "utils.h"
#include "kalmanboxtracker.h"


class Sort
{
public:
	Sort(int max_age, int min_hits, float iou_threshold);
	std::vector<std::vector<float>> update(std::vector<cv::Rect> dets);

private:	
	int m_max_age;

	int m_min_hits;

	float m_iou_threshold;

	std::vector<KalmanBoxTracker> m_trackers;

	int m_frame_count;
};
