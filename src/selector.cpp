#include "selector.h"

LabelSelector::LabelSelector()
{
    this->mouseParam = new onMouseParam();
    this->frame = new cv::Mat();
}

LabelSelector::~LabelSelector()
{
    if (this->mouseParam != nullptr)
        delete this->mouseParam;

    if (this->frame != nullptr)
        delete this->frame;

    select_labels.clear();
    money_labels.clear();
}

bool LabelSelector::ChoiseFrame(std::string video_path, int delay)
{
    cv::VideoCapture capture;
    // cv::Mat frame;
    while (true)
    {
        capture.open(video_path);
        if (!capture.isOpened())
        {
            printf("failed to open the video, please check the video path...\n");
            return false;
        }

        while (capture.read(*frame))
        {
            int key = cv::waitKey(delay);
            if (key == 13)
                break;
            else if (key == 27)
                return false;

            // show image
            cv::imshow(R"(Pressed "Enter" to choise whitch frame you want to select)", *frame);
        }
        capture.release();
        cv::destroyAllWindows();
        cv::imshow(R"(Pressed any key to continue, or pressed "R" to RE-SELECT frame)", *frame);
        int key = cv::waitKey(0);
        if (key == 'R' || key == 'r')
            cv::destroyAllWindows();
        else if (key == 27)
        {
            cv::destroyAllWindows();
            return false;
        }
        else
        {
            cv::destroyAllWindows();
            break;
        }
    }
    return true;
}

cv::Rect LabelSelector::SelectLabelROI(cv::Mat &image, std::string window_name, onMouseParam *param)
{
    // Mat org_img = image.clone();
    param->label_image = image;

    // std::cout << param->label_name << std::endl;
    param->scale_flag = false;

    cv::namedWindow(window_name);
    cv::setMouseCallback(window_name, onMouse, (void *)param);
    while (true)
    {
        int key = cv::waitKey(20);
        if (key == 13)
        {
            if (param->text_flag)
            {
                int big_size;
                param->label_name = param->clolor == cv::Scalar(0, 255, 255) ? "M" + std::to_string(big_size = money_labels.size()) : "S" + std::to_string(big_size = select_labels.size());

                putText(param->label_image, param->label_name,
                        param->label_roi.br() + cv::Point(big_size < 10 ? -30 : -40, -10),
                        cv::FONT_HERSHEY_PLAIN, 1, param->clolor, 2);

                param->text_flag = false;
                param->money_flag = false;
            }
            break;
        }
        else if (key == 27) // ESC
        {
            param->label_roi = cv::Rect(-1, -1, -1, -1);
            break;
        }
        else if (key == 'b' || key == 'B')
        {
            param->clolor = cv::Scalar(255, 0, 0);
        }
        else if (key == 'r' || key == 'R')
        {
            param->clolor = cv::Scalar(0, 0, 255);
        }
        else if (key == 'w' || key == 'W')
        {
            param->clolor = cv::Scalar(255, 255, 255);
        }
        else if (key == 'k' || key == 'K')
        {
            param->clolor = cv::Scalar(0, 0, 0);
        }
        else if (key == 'g' || key == 'G')
        {
            param->clolor = cv::Scalar(0, 255, 0);
        }
        else if (key == 45) // - text thinkness
        {
            if (param->thinkness > 1)
                --param->thinkness;
        }
        else if (key == 61) // + text thinkness
        {
            if (param->thinkness < 10)
                ++param->thinkness;
        }
        else if (key == 's' || key == 'S')
        {
            cv::destroyAllWindows();
            return cv::Rect();
        }
        else if ((key == 'M' || key == 'm') && !param->money_flag)
        {
            param->temp_color = param->clolor;
            param->clolor = cv::Scalar(0, 255, 255);
            param->money_flag = true;
        }

        cv::imshow(window_name, param->label_image);
    }
    cv::destroyAllWindows();
    cv::Rect param_roi = param->label_roi;
    return param_roi;
}

bool LabelSelector::SelectLabels()
{

    while (true)
    {
        cv::Rect getROI = SelectLabelROI(*frame, "Select Label Box", this->mouseParam);
        // std::cout << getROI << std::endl;
        if (getROI == cv::Rect(-1, -1, -1, -1))
        {
            select_labels.clear();
            money_labels.clear();
            cv::destroyAllWindows();
            return false;
        }
        else if (getROI == cv::Rect())
        {
            break;
        }
        else if (this->mouseParam->clolor == cv::Scalar(0, 255, 255))
        {
            if (money_labels.size() == 0 || getROI != money_labels.back().label_box)
            {
                money_labels.emplace_back(money_labels.size(), getROI, "M" + std::to_string(money_labels.size()));
            }
            this->mouseParam->clolor = this->mouseParam->temp_color;
        }
        else if (select_labels.size() == 0 || getROI != select_labels.back().label_box)
        {
            select_labels.emplace_back(select_labels.size(), getROI, "S" + std::to_string(select_labels.size()));
        }
    }

    FileUtils::SaveLabels(select_labels, money_labels);
    cv::destroyAllWindows();

    return true;
}

void onMouse(int event, int x, int y, int flags, void *param)
{
    onMouseParam *mouseParam = (onMouseParam *)param;
    static cv::Point point;
    static cv::Mat temp = mouseParam->label_image.clone();

    switch (event)
    {
    case cv::EVENT_LBUTTONDOWN:

        if (!mouseParam->scale_flag)
        {
            temp = mouseParam->label_image.clone();
            mouseParam->scale_flag = true;
            mouseParam->text_flag = true;
        }
        point.x = x;
        point.y = y;
        break;
    case cv::EVENT_MOUSEMOVE:
        if (flags == cv::EVENT_FLAG_LBUTTON)
        {
            temp.copyTo(mouseParam->label_image);
            rectangle(mouseParam->label_image, point, cv::Point(x, y), mouseParam->clolor, mouseParam->thinkness, 8);
        }
        break;
    case cv::EVENT_LBUTTONUP:
        if ((x - point.x) != 0 && (y - point.y) != 0)
        {
            cv::Rect roi = cv::Rect(
                               x - point.x > 0 ? point.x : x,
                               y - point.y > 0 ? point.y : y,
                               abs(x - point.x),
                               abs(y - point.y))

                           & cv::Rect(0, 0, temp.size().width, temp.size().height);
            mouseParam->label_roi = roi;

            cv::imshow(mouseParam->label_name, temp(roi));
        }
        break;
    default:
        break;
    }
}
