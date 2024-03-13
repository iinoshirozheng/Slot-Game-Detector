#pragma once
#define SHOW_WINDOW 1
#define Print(msg) (std::cout << msg << std::endl)
// ORB setting
constexpr auto ORB_FEATURE_NUMBER = 500;
constexpr auto ORB_THRESHOLD = 0.1;

// SELECTOR setting
constexpr auto SELECTOR_DELAY = 1;

// DETECTOR setting
constexpr auto DETECTOR_DELAY = 1;
constexpr auto DETECTOR_SAVE_FRAME = 5;
constexpr auto DETECTOR_SAME_NUMBER_FRAME = 10;
constexpr auto DETECTOR_MODE_FRAME = 1;
constexpr auto DETECTOR_REDUCE_FRAME = 0;
constexpr auto DETECTOR_PATH = "%~dp0/../../bin/Detector.exe";
constexpr auto TEMPLATE_PATH = "%~dp0/../../../bin/TemplateCSV.exe";
constexpr auto DETECTOR_THRESHOLD = 220;
constexpr auto DETECTOR_BAT_COST = 10000;

// SAVE setting
// constexpr auto SAVE_ADDITION = 2;

// PATH setting
constexpr auto PATH_JSON = "settings.json";
constexpr auto PATH_DATASET = "./Datasets";
constexpr auto PATH_SUB_DATASET = "Dataset_";

// constexpr auto PATH_VIDEO = "Videos";

constexpr auto PATH_LABELS = "Labels";
constexpr auto PATH_OUTPUT = "Output";

constexpr auto MODE_TYPE = "MODE_";

