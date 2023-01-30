#pragma once
// #define SELECTOR
#define DETECTOR
// #define OPENCV_VERSION

// ORB setting
constexpr auto ORB_FEATURE_NUMBER = 10000;
constexpr auto ORB_THRESHOLD = 0.15;

// SELECTOR setting
constexpr auto SELECTOR_DELAY = 100;

// PATH setting
constexpr auto PATH_JSON = "settings.json";
constexpr auto PATH_DATASET = "../Datasets";
constexpr auto PATH_SUB_DATASET = "Dataset_";

constexpr auto PATH_VIDEO = "Videos";
constexpr auto PATH_LABELS = "Labels";
constexpr auto PATH_OUTPUT = "Output";
