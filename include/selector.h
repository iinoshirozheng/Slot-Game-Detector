#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include "utils.h"

// override function use (onMouse)
void onMouse(int event, int x, int y, int flags, void *param);

struct onMouseParam
{
    std::string label_name;
    cv::Mat label_image;
    cv::Rect label_roi;
    cv::Scalar clolor = cv::Scalar(0, 255, 0);
    cv::Scalar temp_color = cv::Scalar(0, 255, 0);
    int thinkness = 1;
    bool scale_flag;
    bool text_flag;
    bool money_flag = false;
};

class LabelSelector
{
private:
    onMouseParam *mouseParam;
    cv::Mat *frame;
    std::vector<Label> select_labels;
    std::vector<Label> money_labels;
    bool ChoiseFrame(std::string video_path, int delay);
    cv::Rect SelectLabelROI(cv::Mat &image, std::string window_name, onMouseParam *param);
    bool SelectLabels();

public:
    LabelSelector();
    ~LabelSelector();
    inline bool Run(std::string video_path, int delay) { return ChoiseFrame(video_path, delay) ? SelectLabels() : false; }
    inline void SaveSelectedFrame(std::string save_path) { cv::imwrite(save_path, *frame); };
};
