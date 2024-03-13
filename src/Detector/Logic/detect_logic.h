#pragma once
#include "Detector/detector.h"
#include "../Data/detect_parameters.h"
#include "../Widget/debug_window.h"
#include <omp.h>

class DetectLogic
{
public:
	DetectLogic();
	DetectLogic(DetectParameters* ParamPtr, DebugWindow* WindowPtr);
	~DetectLogic();
	void Init(DetectParameters* ParamPtr, DebugWindow* WindowPtr);
	virtual bool RunLogic();

protected:
	virtual bool CheckPointers();
	virtual bool DetectMode();
	virtual int  DetectLabels();
	virtual void CountCanNotDetect();
	virtual void DetectRoundAndMoney();
	virtual void RunStatement() = 0;
	bool SetORB();

protected:
	cv_detector::ORB* DetectorORB = nullptr;
	cv_detector::ORB* ModeDetectorORB = nullptr;
	cv_detector::OCR DetectorsOCR[2] = { cv_detector::OCR() , cv_detector::OCR() };
	DetectParameters* ParametersPtr = nullptr;
	DebugWindow* DebugWindowPtr = nullptr;
};

