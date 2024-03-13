#pragma once
#include <opencv2/opencv.hpp>
#include "process.h"
#include "utils.h"
#include "../Data/detect_list.h"
#include "../Data/save_data.h"
#include "../Data/detect_parameters.h"

enum class KeyCode {
	ESCAPE = 27,
	ENTER = 13,
	SPACE = 32,
	// Add more keycodes as needed
};

class DebugWindow
{
public:
	DebugWindow();
	DebugWindow(DetectParameters* ParamPtr);
	~DebugWindow();

	void ResetCanvas();
	void Release();
	void CopyFrame();
	void TurnFrameToGrayScale();
	int  GetWaitKey();
	void DrawModeLabel(cv::Rect& ModeRect);
	void DrawLabelRect(cv::Rect& LabelRect);
	void DrawMoneyLabel(cv::Rect& MoneyRect);
	void DrawRoundLabel(cv::Rect& RoundRect);
	void PutLabelsText();
	void PutCanvasStatusText();
	void TurnFrameToThreshold();
	void Show();

	bool StartTickFPS();
	bool CheckParamPtr();
	bool CheckPressedEndKey();
	bool CheckScreenSizeMatch();

	bool CanReduceFrame();
	bool CaptureScreenshot();

	cv::Mat ShowFrame, Frame, Canvas;
	int PressedKey = 0;
private:
	int canvas_w = 600;
	int canvas_h = 500;
	bool ShowWindow = true;

	int ReduceFrame = 0;
	cv::Size PreSize;
	DetectParameters* DetectParamPtr = nullptr;
};

