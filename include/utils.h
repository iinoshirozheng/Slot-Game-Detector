#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <json/json.h>
#include <fstream>
#include "process.h"

struct Label
{
    int index;
    std::string name;
    cv::Rect label_box;
    Label()
    {
        this->index = 0;
        this->label_box = cv::Rect();
        this->name = "";
    };
    Label(int index, cv::Rect box, std::string name = "")
    {
        this->index = index;
        this->name = name;
        this->label_box = box;
    }
};

struct JsonParamater
{
    std::vector<Label> money_labels;
    std::vector<Label> select_labels;
    std::string detect_video;
};

// 檔案處理
class FileUtils
{
private:
    static JsonParamater jsonData;
    static std::string rootDirectory;

    static Json::Value GetLabelValue(Label label);
    static Json::Value CreateLabelsValue(std::vector<Label> labels);
    static std::string MakeDirectory(std::string root_dir);
    static std::string MakeSubDirectory(std::string root_dir, std::string sub_dir);
    static bool MakeDataFiles(std::string root_dir);

protected:
    static void GetLabelFile(std::string path, std::vector<std::string> &file, std::vector<std::string> ext);

public:
    static std::string video_path;
    static std::vector<std::string> labels;

    static bool WriteJsonFile(std::string root_dir);
    static bool ReadJsonFile(std::string json_path);
    static void SaveLabels(std::vector<Label> &labels, std::vector<Label> &money_labels);
    static void CopyVideoFile(const std::string &video_path);
    const static std::vector<std::string> Split(const std::string &str, const char &delimiter);

    // Get value
    inline static JsonParamater GetJsonParamater() { return jsonData; };
    inline static std::vector<std::string> GetDetectLabels() { return labels; };
    inline static std::string GetRootDirectory() { return rootDirectory; };
    inline static void SetRootDirectory(std::string root_dir) { rootDirectory = root_dir; };
    inline static bool CreateDataset() { return MakeDataFiles(MakeSubDirectory(MakeDirectory(PATH_DATASET), PATH_SUB_DATASET)); };
};

// 影像處理
class CvUtils : public FileUtils
{
public:
    static void GetDetectImages(std::string file_path, std::vector<std::string> need_extension, std::vector<cv::Mat> &detectImages);
    static void ShowFPS(const cv::VideoCapture &capture, const cv::Mat &image);
    static void ShowDetectLabel(const std::string &label_name, cv::Mat &image, cv::Point point = cv::Point(50, 50));
};
