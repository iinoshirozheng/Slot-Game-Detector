#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

namespace cv_detector
{

    class ORB
    {
    private:
        cv::Ptr<cv::ORB> detector;
        std::vector<cv::Mat> descriptors;

        std::vector<int> matchList;
        std::vector<double> currentList;
        cv::Ptr<cv::DescriptorMatcher> matcher;
        std::vector<std::vector<cv::DMatch>> knn_matches; // 儲存 MATCH 關係
        std::vector<cv::DMatch> good_matches;             // 儲存好的比對點
        std::vector<cv::DMatch> all_matches;              // 儲存所有的比對點
        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptor_frame;

        double threshold;

    public:
        ORB();
        ORB(const std::vector<cv::Mat>& detect_images, int nfeature = 500, double threshold = 0.0);
        void SetDescriptors(const std::vector<cv::Mat>& detect_images);
        void FindCurrentLabel(const cv::Mat& frame);
        int OMP_FindCurrentLabel(const cv::Mat& frame);
        int GetMaxIndex();
        int GetMaxPercentIndex();
    };

    class OCR
    {
    private:
        tesseract::TessBaseAPI* detector;

    public:
        OCR();
        ~OCR();
        std::string DetectTextLine(const cv::Mat& frame, bool graymode = true);
    };

}