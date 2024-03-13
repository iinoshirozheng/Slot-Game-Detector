#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <json/json.h>
#include "process.h"
#include <Windows.h>

struct TempStatus
{
    // MODE
    int mode = -1;
    int pre_mode = -1;
    int count_same_mode = 0;
    int renew_mode = -1;

    // Round
    std::string round = "";
    std::string preRound = "";

    // Money
    std::string money = "";
    std::string preMoney = "";
};

struct DetectStatus
{
    DetectStatus() = default;

    int cantDetect = -1;
    int continueCantDetect = 0;
    // ROUND
    bool countRound = false;
    int sameRoundCount = 0;
    bool nextRound = false;

    // MONEY
    bool countMoney = false;
    int sameMoneyCount = 0;
    bool nextMoney = false;
    bool moneyChange = false;
    
    // else
    int last_same_count = 0;
    int max_last_same_count = INT_MIN;
    bool is_all_same = false;


    int detect_frame = 0;
    std::set<int> count_frame = {};
    std::string mode_name = "";
    int countEnd = 0;

    TempStatus temp = TempStatus();
};

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

struct JsonParameter
{
    std::vector<Label> text_labels;
    std::vector<Label> select_labels;
    std::vector<Label> mode_labels;
    std::string window_name;
    // std::string detector_path = DETECTOR_PATH;
    std::vector<std::vector<int>> label_seq;
    int continue_detect_frame = DETECTOR_SAVE_FRAME;
    int continue_number_frame = DETECTOR_SAME_NUMBER_FRAME;
    int continue_mode_frame = DETECTOR_MODE_FRAME;
    // int save_addition = SAVE_ADDITION;
    int delay_frame = DETECTOR_DELAY;
    int detect_logic = 0;
    double detector_threshold = ORB_THRESHOLD;
    bool show_debug_window = true;

    JsonParameter() = default;
};

// 檔案處理
class FileUtils
{
private:
    static std::string rootDirectory;
    static JsonParameter jsonData;
    static Json::Value GetLabelValue(Label label);
    static Json::Value CreateLabelsValue(std::vector<Label> labels);
    static std::string MakeSubDirectory(std::string root_dir, std::string sub_dir);
    static bool MakeDataFiles(std::string root_dir);

public:
    static std::vector<std::string> labels;
    static std::vector<std::string> labels_mode;
    static std::string detector_path;

    static std::string MakeDirectory(std::string root_dir);
    static bool WriteJsonFile(std::string root_dir);
    static bool ReadJsonFile(std::string json_path);
    static void SaveLabels(std::vector<Label>& labels, std::vector<Label>& money_labels, std::vector<Label>& mode_labels);
    static std::vector<std::string> Split(const std::string& str, const char& delimiter);
    static std::string GetVideoName(std::string video_path);
    static void GetLabelFile(std::string path, std::vector<std::string>& file, std::vector<std::string> ext);
    static std::string format(double money, int fixed) { auto str = std::to_string(money) ;  return str.substr(0, str.find(".") + fixed + 1); }
    static bool FilterString(std::string str);

    // Get value
    inline static JsonParameter GetJsonParameter() { return jsonData; };
    inline static std::vector<std::string> GetDetectLabels() { return labels; };
    inline static std::string GetRootDirectory() { return rootDirectory; };
    inline static void SaveWindowNameToJson(const char* windowName) { jsonData.window_name = std::string(windowName); }
    inline static void SetRootDirectory(std::string root_dir) { rootDirectory = root_dir; };
    inline static bool CreateDataset() { return MakeDataFiles(MakeSubDirectory(MakeDirectory(PATH_DATASET), PATH_SUB_DATASET)); };
};

// 影像處理
class CvUtils
{
public:
    static void GetDetectImages(std::string file_path, std::vector<cv::Mat>& detectImages, std::vector<cv::Mat>& specialModeImages);
    static void ShowFPS(const cv::Mat& image);
    static void ShowDetectLabel(const std::string& label_name, cv::Mat& image, cv::Point point = cv::Point(50, 50));
    static void CaptureScreenshot(cv::Mat& screenshot);
    static bool SetWindowHandle(const char* windowName);
    inline static bool HasHandle() { return true ? hwnd != nullptr : false; }
    inline static bool StartTickFPS() { CvUtils::tick = static_cast<double>(cv::getTickCount());  return true; }
    inline static int GetTick() { return CvUtils::tick; }
    inline static double GetFPS() { return 1.0 / ((cv::getTickCount() - CvUtils::tick) / cv::getTickFrequency()); }

private:
    static double fps, tick;
    static HWND hwnd;
};