#include "../include/process.h"
#include "../include/utils.h"
#include "../include/detector.h"
#include "../include/selector.h"

// test include
// #include <opencv2/xfeatures2d.hpp>

#if defined(SELECTOR)
// SELECTOR code
int main(int argc, char *argv[])
{
    int delay = SELECTOR_DELAY;
    if (argc == 1)
    {
        std::cout << R"(Input the Video Path : )";
        std::getline(std::cin, FileUtils::video_path);
        auto args = FileUtils::Split(FileUtils::video_path, ' ');
        if (args.size() > 1 && std::stoi(args[1]))
        {
            delay = std::stoi(args[1]);
        }
        FileUtils::video_path = args[0];
    }
    else if (argc == 2)
    {
        FileUtils::video_path = argv[1];
    }
    else if (argc == 3)
    {
        if (argc > 2 && std::stoi(argv[2]))
        {
            delay = std::stoi(argv[2]);
        }
        FileUtils::video_path = argv[1];
    }

    LabelSelector selector = LabelSelector();
    if (selector.Run(FileUtils::video_path, delay))
    {
        // if didn't give the arg video path, output a empty template

        if (FileUtils::CreateDataset())
        {
            std::cout << "Create Dataset successfully!" << std::endl;
            FileUtils::CopyVideoFile(FileUtils::video_path);
            std::cout << R"(Copy video to "Videos" directory!)" << std::endl;
        }
        else
        {
            std::cout << "Create Dataset failed!" << std::endl;
            return -1;
        }
    }
    return 0;
}

#elif defined(DETECTOR)

int main(int argc, char *argv[])
{
    FileUtils::SetRootDirectory("../Datasets/Dataset_2/");
    FileUtils::ReadJsonFile(FileUtils::GetRootDirectory() + PATH_JSON);
    const std::string labels_path = FileUtils::GetRootDirectory() + PATH_LABELS;
    const std::vector<std::string> need_extension = {".jpg", ".png", ".jepg", ".PNG", ".JPG", ".JPEG"};
    const std::string video_path = (FileUtils::GetRootDirectory() + PATH_VIDEO) + "/test.mp4";
    std::cout << video_path << std::endl;

    std::vector<cv::Mat> label_images;
    CvUtils::GetDetectImages(labels_path, need_extension, label_images);
    static cv_detector::ORB detector_ORB = cv_detector::ORB(label_images, ORB_FEATURE_NUMBER, ORB_THRESHOLD);
    static cv_detector::OCR detector_OCR = cv_detector::OCR();
    cv::VideoCapture capture;

    capture.open(video_path);
    if (!capture.isOpened())
    {
        printf("could not read this video file...\n");
        return -1;
    }

    cv::Mat frame, showFrame;
    cv::namedWindow("Slot Detector", cv::WINDOW_AUTOSIZE);
    JsonParamater jsonParam = FileUtils::GetJsonParamater();

    // vector<int> matchList;
    // while loop
    while (capture.read(frame))
    {
        if (cv::waitKey(1) == 27)
            break;

        showFrame = frame.clone();

        // process and compute
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

        // cv::Mat roi;
        for (auto &select_label : jsonParam.select_labels)
        {
            detector_ORB.FindCurrentLabel(frame(select_label.label_box));
            int index = detector_ORB.GetMaxPercentIndex();
            // int index = detector_ORB.GetMaxIndex();
            cv::rectangle(showFrame, select_label.label_box, cv::Scalar(0, 255, 0), 2);
            if (index != -1)
                cv::putText(showFrame, FileUtils::labels[index],
                            select_label.label_box.tl() - cv::Point(-10, -30),
                            cv::FONT_HERSHEY_COMPLEX, 1,
                            cv::Scalar(0, 255, 0), 2);
        }

        CvUtils::ShowFPS(capture, showFrame);

        // show image
        cv::imshow("Slot Detector", showFrame);
    }
    capture.release();
    cv::waitKey(0);

    return 0;
}

#elif defined(OPENCV_VERSION)

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    cout << "OpenCV version : " << CV_VERSION << endl;
    cout << "Major version : " << CV_MAJOR_VERSION << endl;
    cout << "Minor version : " << CV_MINOR_VERSION << endl;
    cout << "Subminor version : " << CV_SUBMINOR_VERSION << endl;

    return 0;
}
#else

// tset code
int main(int argc, char *argv[])
{
    FileUtils::SetRootDirectory("../Datasets/test1/");
    FileUtils::ReadJsonFile(FileUtils::GetRootDirectory() + PATH_JSON);
    const std::string labels_path = FileUtils::GetRootDirectory() + PATH_LABELS;
    const std::vector<std::string> need_extension = {".jpg", ".png", ".jepg"};
    const std::string video_path = (FileUtils::GetRootDirectory() + PATH_VIDEO) + "/test.mp4";
    std::cout << video_path << std::endl;

    std::vector<cv::Mat> label_images;
    CvUtils::GetDetectImages(labels_path, need_extension, label_images);
    static cv_detector::ORB detector_ORB = cv_detector::ORB(label_images, ORB_FEATURE_NUMBER, ORB_THRESHOLD);
    static cv_detector::OCR detector_OCR = cv_detector::OCR();
    cv::VideoCapture capture;

    std::vector<cv::Mat> detect_images;

    capture.open(video_path);
    if (!capture.isOpened())
    {
        printf("could not read this video file...\n");
        return -1;
    }

    cv::Mat frame, showFrame;
    // cv::namedWindow("slot_video showFrame", cv::WINDOW_AUTOSIZE);
    JsonParamater jsonParam = FileUtils::GetJsonParamater();

    // vector<int> matchList;
    // while loop
    while (capture.read(frame))
    {
        if (cv::waitKey(1) == 27)
            break;

        showFrame = frame.clone();

        // process and compute
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

        // cv::Mat roi;
        for (auto &select_label : jsonParam.select_labels)
        {
            int index = detector_ORB.FindCurrentLabel(frame(select_label.label_box));
            cv::rectangle(showFrame, select_label.label_box, cv::Scalar(0, 255, 0), 2);
            if (index != -1)
                cv::putText(showFrame, FileUtils::labels[index], select_label.label_box.tl() - cv::Point(-10, -30), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 0), 2);
        }

        CvUtils::ShowFPS(capture, showFrame);

        // show image
        cv::imshow("Slot Detector", showFrame);
    }
    capture.release();
    cv::waitKey(0);

    return 0;
}
#endif