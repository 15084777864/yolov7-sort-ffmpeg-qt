#ifndef KALMANBOXTRACKER_H
#define KALMANBOXTRACKER_H

#include <iostream>
#include <opencv2/opencv.hpp>

class kalmanboxtracker
{
public:
    kalmanboxtracker(std::vector<int> bbox);
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

#endif // KALMANBOXTRACKER_H
