#include "selector.h"
#include<opencv2/highgui/highgui_c.h>

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
    text_labels.clear();
    mode_labels.clear();
}

bool LabelSelector::ChoiseFrame(const char* handle_name, int delay)
{
    if (!CvUtils::SetWindowHandle(handle_name))
    {
        std::cout << "can't find the windows title, please check the title is correct" << std::endl;
        return false;
    }
    else {
        FileUtils::SaveWindowNameToJson(handle_name);
    }

    const char* choise_1 = R"(Pressed "Enter" to choise whitch frame you want to select)";
    const char* choise_2 = R"(Pressed any key to continue, or pressed "R" to RE-SELECT frame)";
    int key = 0;
    while (true)
    {
        cv::namedWindow(choise_1, cv::WINDOW_AUTOSIZE);
        SetForegroundWindow((HWND)cvGetWindowHandle(choise_1));

        while ((key = cv::waitKey(delay)))
        {
            if (key == 13)
                break;
            else if (key == 27)
            {
                cv::destroyAllWindows();
                return false;
            }
            CvUtils::CaptureScreenshot(*frame);
            // show image
            cv::imshow(choise_1, *frame);
        }
        cv::destroyAllWindows();


        // Create a named window
        cv::namedWindow(choise_2, cv::WINDOW_AUTOSIZE);
        // Set the window property to keep it on top
        SetForegroundWindow((HWND)cvGetWindowHandle(choise_2));
        cv::imshow(choise_2, *frame);
        
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

cv::Rect LabelSelector::SelectLabelROI(cv::Mat& image, const char* window_name, onMouseParam* param)
{
    // Mat org_img = image.clone();
    param->label_image = image;

    // std::cout << param->label_name << std::endl;
    param->scale_flag = false;

    cv::namedWindow(window_name);
    cv::setMouseCallback(window_name, onMouse, (void*)param);

    // Set the window property to keep it on top
    SetForegroundWindow((HWND)cvGetWindowHandle(window_name));
    
    static bool money_flag = false;
    static bool mode_flag = false;
    static bool round_flag = false;
    static bool free_flag = false;
    
    while (true)
    {
        int key = cv::waitKey(1);
        if (key == 13)
        {
            if (param->text_flag)
            {
                int big_size;
                if (param->clolor == cv::Scalar(0, 255, 0))
                {
                    param->label_name = "S" + std::to_string(big_size = select_labels.size());
                }
                else if (param->clolor == cv::Scalar(255, 0, 255))
                {
                    param->label_name = "Mode";
                }
                else if (param->clolor == cv::Scalar(0, 0, 255))
                {
                    param->label_name = "Round";
                }
                else if (param->clolor == cv::Scalar(0, 255, 255))
                {
                    param->label_name = "Money";
                }
                else if (param->clolor == cv::Scalar(255, 0, 0))
                {
                    param->label_name = "Free";
                }

                putText(param->label_image, param->label_name,
                    param->label_roi.br() + cv::Point(param->label_name.length() * -10 - 10, -10),
                    cv::FONT_HERSHEY_PLAIN, 1, param->clolor, 2);

                param->text_flag = false;

            }
            break;
        }
        else if (key == 27) // ESC
        {
            param->label_roi = cv::Rect(-1, -1, -1, -1);
            break;
        }
        else if ((key == 'r' || key == 'R') && !round_flag)
        {
            param->temp_color = param->clolor;
            param->clolor = cv::Scalar(0, 0, 255);
            round_flag = true;
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
        else if ((key == 'M' || key == 'm') && !mode_flag)
        {
            param->temp_color = param->clolor;
            param->clolor = cv::Scalar(255, 0, 255);
            mode_flag = true;
        }
        else if ((key == 'G' || key == 'g') && !money_flag)
        {
            param->temp_color = param->clolor;
            param->clolor = cv::Scalar(0, 255, 255);
            money_flag = true;
        }
        else if ((key == 'F' || key == 'f') && !money_flag)
        {
            param->temp_color = param->clolor;
            param->clolor = cv::Scalar(255, 0, 0);
            free_flag = true;
        }


        cv::imshow(window_name, param->label_image);
    }
    cv::destroyAllWindows();
    cv::Rect param_roi = param->label_roi;
    return param_roi;
}

bool LabelSelector::SelectLabels()
{
    select_labels.clear();
    text_labels.clear();
    mode_labels.clear();
    while (true)
    {
        cv::Rect getROI = SelectLabelROI(*frame, "Select Label Box", this->mouseParam);
        // std::cout << getROI << std::endl;
        if (getROI == cv::Rect(-1, -1, -1, -1))
        {
            select_labels.clear();
            text_labels.clear();
            mode_labels.clear();
            cv::destroyAllWindows();
            return false;
        }
        else if (getROI == cv::Rect())
        {
            break;
        }
        else if (this->mouseParam->clolor == cv::Scalar(0, 0, 255))
        {
            if (text_labels.size() == 0 || getROI != text_labels.back().label_box)
            {
                text_labels.emplace_back(text_labels.size(), getROI, "Round");
            }
            this->mouseParam->clolor = this->mouseParam->temp_color;
        }
        else if (this->mouseParam->clolor == cv::Scalar(255, 0, 255))
        {
            if (mode_labels.size() == 0 || getROI != mode_labels.back().label_box)
            {
                mode_labels.emplace_back(mode_labels.size(), getROI, "Mode");
            }
            this->mouseParam->clolor = this->mouseParam->temp_color;
        }
        else if (this->mouseParam->clolor == cv::Scalar(0, 255, 255))
        {
            if (text_labels.size() == 0 || getROI != text_labels.back().label_box)
            {
                text_labels.emplace_back(text_labels.size(), getROI, "Money");
            }
            this->mouseParam->clolor = this->mouseParam->temp_color;
        }
        else if (this->mouseParam->clolor == cv::Scalar(255, 0, 0))
        {
            if (text_labels.size() == 0 || getROI != text_labels.back().label_box)
            {
                text_labels.emplace_back(text_labels.size(), getROI, "Free");
            }
            this->mouseParam->clolor = this->mouseParam->temp_color;
        }
        else if (select_labels.size() == 0 || getROI != select_labels.back().label_box)
        {
            select_labels.emplace_back(select_labels.size(), getROI, "S" + std::to_string(select_labels.size()));
        }
    }

    FileUtils::SaveLabels(select_labels, text_labels, mode_labels);
    cv::destroyAllWindows();

    return true;
}

void onMouse(int event, int x, int y, int flags, void* param)
{
    onMouseParam* mouseParam = (onMouseParam*)param;
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