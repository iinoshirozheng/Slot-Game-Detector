#include "detector.h"

// static ORB
std::vector<cv::Mat> cv_detector::ORB::descriptors;
cv::Ptr<cv::ORB> cv_detector::ORB::detector;
// static OCR
tesseract::TessBaseAPI *cv_detector::OCR::detector;

cv_detector::ORB::ORB(const std::vector<cv::Mat> &detect_images, int nfeature, double threshold)
{
    this->detector = cv::ORB::create(nfeature);
    // Match
    // 比對器 使用遍歷方法中的漢明距離  BruteForce-Hamming(2)
    this->matcher = cv::DescriptorMatcher::create("BruteForce-Hamming(2)");
    this->threshold = threshold;
    SetDescriptors(detect_images);
}

// 定義 ORB 特徵檢測器
void cv_detector::ORB::SetDescriptors(const std::vector<cv::Mat> &detect_images)
{
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptor;
    for (auto &image : detect_images)
    {
        this->detector->detectAndCompute(image, cv::Mat(), keypoints, descriptor);
        this->descriptors.emplace_back(descriptor);
    }
}

void cv_detector::ORB::FindCurrentLabel(const cv::Mat &frame)
{
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptor_frame;
    this->detector->detectAndCompute(frame, cv::Mat(), keypoints, descriptor_frame);
    matchList.clear();
    currentList.clear();
    // 將特徵的 descriptor 和 影像中的 descriptor Match 關係儲存到 knn_matches 中
    try
    {
        for (auto &descriptor : descriptors)
        {
            knn_matches.clear();
            matcher->knnMatch(descriptor, descriptor_frame, knn_matches, 2);

            // 儲存好的比對點
            good_matches.clear();
            all_matches.clear();

            for (int i = 0; i < knn_matches.size(); i++)
            {
                if (knn_matches[i][0].distance < 0.85 * knn_matches[i][1].distance)
                {
                    good_matches.emplace_back(knn_matches[i][0]);
                }
                all_matches.emplace_back(knn_matches[i][0]);
            }
            matchList.emplace_back(good_matches.size());
            currentList.emplace_back((double)good_matches.size() / (double)all_matches.size());
        }
    }
    catch (const cv::Exception &e)
    {
        // pass
    }

    // show index good number
    // std::cout << "label number : [ ";
    // for (auto match : matchList)
    // {
    //     std::cout << match << " ";
    // }
    // std::cout << "]    ";
}

int cv_detector::ORB::GetMaxIndex()
{
    // get max_index
    auto it = std::max_element(matchList.begin(), matchList.end());
    if (*it < threshold)
        return -1;

    return std::find_if(matchList.begin(), matchList.end(), [it](int i)
                        { return i == *it; }) -
           matchList.begin();
}

int cv_detector::ORB::GetMaxPercentIndex()
{
    auto it = std::max_element(currentList.begin(), currentList.end());
    if (*it < threshold)
        return -1;

    return std::find_if(currentList.begin(), currentList.end(), [it](double i)
                        { return i == *it; }) -
           currentList.begin();
}

cv_detector::OCR::OCR()
{
    this->detector = new tesseract::TessBaseAPI();
    this->detector->Init(NULL, "eng", tesseract::OEM_DEFAULT);
    this->detector->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
}

cv_detector::OCR::~OCR()
{
    this->detector->End();
    this->detector = nullptr;
}

std::string cv_detector::OCR::DetectTextLine(cv::Mat &input_image)
{
    this->detector->SetImage(input_image.data, input_image.cols, input_image.rows, 3, input_image.step);
    return detector->GetUTF8Text();
}
