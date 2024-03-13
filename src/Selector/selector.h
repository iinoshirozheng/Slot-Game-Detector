#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include "utils.h"

// override function use (onMouse)
void onMouse(int event, int x, int y, int flags, void* param);

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
};

class LabelSelector
{
private:
    onMouseParam* mouseParam;
    cv::Mat* frame;
    std::vector<Label> select_labels;
    std::vector<Label> text_labels;
    std::vector<Label> mode_labels;
    bool ChoiseFrame(const char* window_name, int delay);
    cv::Rect SelectLabelROI(cv::Mat& image, const char* window_name, onMouseParam* param);
    bool SelectLabels();


public:
    LabelSelector();
    ~LabelSelector();
    inline bool Run(const char* window_name, int delay = 1) { return ChoiseFrame(window_name, delay) ? SelectLabels() : false; }
    inline void SaveSelectedFrame(std::string save_path) { cv::imwrite(save_path, *frame); };
};