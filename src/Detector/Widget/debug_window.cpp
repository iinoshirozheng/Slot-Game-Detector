#include "debug_window.h"


DebugWindow::DebugWindow()
{
    this->ReduceFrame = 0;
}

DebugWindow::DebugWindow(DetectParameters* ParamPtr)
{
    this->ShowWindow = ParamPtr->JsonParam.show_debug_window;
    this->DetectParamPtr = ParamPtr;
    this->ReduceFrame = 0;
}

DebugWindow::~DebugWindow()
{
	Release();
    cv::destroyAllWindows();
}

void DebugWindow::ResetCanvas()
{
    if(!ShowWindow) return;
	Canvas = cv::Mat::zeros(cv::Size(canvas_w, canvas_h), CV_8UC3);
}

void DebugWindow::Release()
{
	Canvas.release();
	Frame.release();
	ShowFrame.release();
}

void DebugWindow::CopyFrame()
{
if(!ShowWindow) return;
	Frame.copyTo(ShowFrame);
}

void DebugWindow::TurnFrameToGrayScale()
{
    CopyFrame();
	cv::cvtColor(Frame, Frame, cv::COLOR_BGR2GRAY);
}

int DebugWindow::GetWaitKey()
{
	return cv::waitKey(DETECTOR_DELAY);
}

void DebugWindow::DrawModeLabel(cv::Rect& ModeRect)
{
    if (!ShowWindow) return;
	cv::rectangle(ShowFrame, ModeRect, cv::Scalar(255, 0, 255), 2);

	cv::putText(ShowFrame,
		DetectParamPtr->Status.mode_name,
		ModeRect.tl() - cv::Point(-10, -30),
		cv::FONT_HERSHEY_COMPLEX, 1,
		cv::Scalar(255, 0, 255), 2);
}

void DebugWindow::DrawLabelRect(cv::Rect& LabelRect)
{
    if (!ShowWindow) return;
	cv::rectangle(ShowFrame, LabelRect, cv::Scalar(0, 255, 0), 2);
}

void DebugWindow::DrawMoneyLabel(cv::Rect& MoneyRect)
{
    if (!ShowWindow) return;
    cv::rectangle(ShowFrame, MoneyRect, cv::Scalar(0, 255, 255), 2);
    cv::putText(ShowFrame,
        DetectParamPtr->Status.temp.money,
        MoneyRect.tl() - cv::Point(-10, 10),
        cv::FONT_HERSHEY_COMPLEX, 1,
        cv::Scalar(0, 255, 255), 2);
}

void DebugWindow::DrawRoundLabel(cv::Rect& RoundRect)
{
    if (!ShowWindow) return;
    cv::rectangle(ShowFrame, RoundRect, cv::Scalar(0, 0, 255), 2);
    cv::putText(ShowFrame,
        DetectParamPtr->Status.temp.round,
        RoundRect.tl() - cv::Point(-10, 10),
        cv::FONT_HERSHEY_COMPLEX, 1,
        cv::Scalar(0, 0, 255), 2);
}

void DebugWindow::PutLabelsText()
{
	if (!ShowWindow) return;
	for (int index = 0; index < DetectParamPtr->JsonParam.select_labels.size(); ++index)
	{
		cv::putText(ShowFrame, FileUtils::labels[DetectParamPtr->DetectLists.Get2DIndexLabel(index)],
			DetectParamPtr->JsonParam.select_labels[index].label_box.tl() - cv::Point(-10, -30),
			cv::FONT_HERSHEY_COMPLEX, 1,
			cv::Scalar(0, 255, 0), 2);
	}
}

void DebugWindow::PutCanvasStatusText()
{
if (!ShowWindow) return;
    std::cout << "cantDetect:" << DetectParamPtr->Status.cantDetect
        << ", countMoney:" << DetectParamPtr->Status.countMoney
        << ", countRound:" << DetectParamPtr->Status.countRound
        << ", is_all_same:" << DetectParamPtr->Status.is_all_same
        << ", detect_frame:" << DetectParamPtr->Status.detect_frame
        << ", last_same_count:" << DetectParamPtr->Status.last_same_count
        << ", max_last_same_count:" << DetectParamPtr->Status.max_last_same_count
        << "\nmode_name:" << DetectParamPtr->Status.mode_name
        << ", moneyChange:" << DetectParamPtr->Status.moneyChange
        << ", nextMoney:" << DetectParamPtr->Status.nextMoney
        << ", nextRound:" << DetectParamPtr->Status.nextRound
        << ", sameMoneyCount:" << DetectParamPtr->Status.sameMoneyCount
        << ", sameRoundCount:" << DetectParamPtr->Status.sameRoundCount
        << std::endl;

    std::cout
        //<< ", count_same_mode:" << DetectParamPtr->Status.temp.count_same_mode
        << "pre_mode:" << DetectParamPtr->Status.temp.pre_mode
        << ", mode:" << DetectParamPtr->Status.temp.mode
        << ", preMoney:" << DetectParamPtr->Status.temp.preMoney
        << ", money:" << DetectParamPtr->Status.temp.money
        << ", preRound:" << DetectParamPtr->Status.temp.preRound
        << ", round:" << DetectParamPtr->Status.temp.round
        << ", isLock:" << DetectParamPtr->SaveDatas.IsLock()
        << ", isEarlyLock:" << DetectParamPtr->SaveDatas.IsEarlyLock()
        << std::endl << std::endl;

    int pos_x = 0;
    int pos_y = 0;
    int canvas_x = 10;
    int canvas_y = 25;

    int count_text = 0;
    int text_think = 1;
    double text_size = 0.6;

    cv::putText(Canvas, "Can't Detect : " + std::to_string(DetectParamPtr->Status.cantDetect), cv::Point(canvas_x, canvas_y * ++count_text), cv::FONT_HERSHEY_COMPLEX, text_size, cv::Scalar(255, 255, 255), text_think);
    cv::putText(Canvas, "Last Same Count : " + std::to_string(DetectParamPtr->Status.last_same_count), cv::Point(canvas_x, canvas_y * ++count_text), cv::FONT_HERSHEY_COMPLEX, text_size, cv::Scalar(255, 255, 255), text_think);
    cv::putText(Canvas, "Is All Same : " + std::to_string(DetectParamPtr->Status.is_all_same), cv::Point(canvas_x, canvas_y * ++count_text), cv::FONT_HERSHEY_COMPLEX, text_size, cv::Scalar(255, 255, 255), text_think);
    cv::putText(Canvas, "Detect Count Frame : " + std::to_string(DetectParamPtr->Status.detect_frame), cv::Point(canvas_x, canvas_y * ++count_text), cv::FONT_HERSHEY_COMPLEX, text_size, cv::Scalar(255, 255, 255), text_think);
    cv::putText(Canvas, "Count Frame : ", cv::Point(canvas_x, canvas_y * ++count_text), cv::FONT_HERSHEY_COMPLEX, text_size, cv::Scalar(255, 255, 255), text_think);
    for (auto& count : DetectParamPtr->Status.count_frame) {
        cv::putText(Canvas, std::to_string(count), cv::Point(canvas_x + 170 + pos_x * 30, canvas_y * (count_text + pos_y)), cv::FONT_HERSHEY_COMPLEX, text_size, cv::Scalar(255, 255, 255), text_think);
        pos_x++;
        if (170 + pos_x * 30 > canvas_w - 70) {
            pos_x = 0;
            pos_y++;
        }
    }
    count_text += 1;
    cv::putText(Canvas, DetectParamPtr->SaveDatas.IsLock() ? "( Lock )" : "( Save )",
        cv::Point(canvas_x, canvas_y * ++count_text), cv::FONT_HERSHEY_COMPLEX, text_size, DetectParamPtr->SaveDatas.IsLock() ? cv::Scalar(0, 255, 255) : cv::Scalar(0, 255, 0), text_think);
    cv::putText(Canvas, "Round : " + DetectParamPtr->SaveDatas.GetRound() +
        (DetectParamPtr->SaveDatas.GetMode() != "" ? "    Mode : " + DetectParamPtr->SaveDatas.GetMode() : "") +
        (!DetectParamPtr->SaveDatas.IsLock() ? "    Money : " + DetectParamPtr->SaveDatas.GetMoney() : ""),
        cv::Point(canvas_x, canvas_y * ++count_text), cv::FONT_HERSHEY_COMPLEX, text_size, cv::Scalar(255, 255, 255), text_think);
    pos_x = 0;
    pos_y = 0;
    count_text += 2;

    int temp = 0;
    for (auto& list : DetectParamPtr->SaveDatas.GetSaveList())
    {
        for (auto& data : list)
        {
            cv::putText(Canvas, data == -1 ? "NULL" : FileUtils::labels[data], cv::Point(canvas_x + pos_x * canvas_y, canvas_y * (count_text + pos_y)), cv::FONT_HERSHEY_COMPLEX, text_size, cv::Scalar(0, 255, 0), text_think);
            pos_y++;
        }
        pos_y = 0;
        pos_x += 5;
        temp = list.size();
    }
}

void DebugWindow::TurnFrameToThreshold()
{
    cv::threshold(Frame, Frame, DETECTOR_THRESHOLD, 255, cv::THRESH_BINARY);
}

void DebugWindow::Show()
{
if (!ShowWindow) return;
    CvUtils::ShowFPS(ShowFrame);

    // show image
    cv::imshow("Debug Status", Canvas);
    cv::resize(ShowFrame, ShowFrame, cv::Size(1024, 576), 0, 0, cv::INTER_AREA);
    cv::imshow("Slot Detector", ShowFrame);
}

bool DebugWindow::StartTickFPS()
{
    ReduceFrame++;
    return CvUtils::StartTickFPS();
}

bool DebugWindow::CheckParamPtr()
{
	return DetectParamPtr != nullptr;
}

bool DebugWindow::CheckPressedEndKey()
{
    if (!ShowWindow) return false;

    // Set Break key ESC
    if ((PressedKey = cv::waitKey(DETECTOR_DELAY)) == static_cast<int>(KeyCode::ESCAPE))
    {
		return true;
	}
    // pressed Enter to pause
    if (PressedKey == static_cast<int>(KeyCode::ENTER) && cv::waitKey(0) == static_cast<int>(KeyCode::ESCAPE))
    {
        Release();
        return true;
    }

    return false;
}

bool DebugWindow::CheckScreenSizeMatch()
{
    if (!PreSize.empty() && abs(PreSize.area() - Frame.size().area()) > 100000 || Frame.empty())
    {
        Print(PreSize.area());
        Print(Frame.size().area());
        Print("tracking window failed !");
        return false;
    }

    PreSize = Frame.size();
    return true;
}

bool DebugWindow::CanReduceFrame()
{
    return DETECTOR_REDUCE_FRAME && !(ReduceFrame % DETECTOR_REDUCE_FRAME);
}

bool DebugWindow::CaptureScreenshot()
{
    // Capture Screenshot
    CvUtils::CaptureScreenshot(Frame);
    return CheckScreenSizeMatch();
}
