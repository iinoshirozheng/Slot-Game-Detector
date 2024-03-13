#pragma once
#include "../detect_logic.h"
#include "../../Data/detect_parameters.h"
#include "../../Widget/debug_window.h"

class DetectLogicFirst : public DetectLogic
{
public:
	DetectLogicFirst(DetectParameters *params, DebugWindow *debug_window);
	~DetectLogicFirst();
protected:
	void RunStatement() override;
};

class DetectLogicLast : public DetectLogic
{
public:
	DetectLogicLast(DetectParameters* params, DebugWindow* debug_window);
	~DetectLogicLast();
protected:
	void RunStatement() override;

};