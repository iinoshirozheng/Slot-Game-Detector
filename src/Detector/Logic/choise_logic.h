#pragma once
#include "detect_logic.h"
#include "Detect_Logic_Basic/detect_logic_basic.h"
//#include "detect_last_logic.h"

enum class DetectLogicType
{
    None,
    DetectFirst,	// FishGame,
    DetectLast,		// Tropicana,
    Size
};

static DetectLogic* GetDetectLogic(DetectParameters* ParamPtr, DebugWindow* WindowPtr)
{
    if(!ParamPtr || !WindowPtr) return nullptr;

    switch (static_cast<DetectLogicType>(ParamPtr->JsonParam.detect_logic))
    {
    case DetectLogicType::None:
        break;
    case DetectLogicType::DetectFirst:
        return new DetectLogicFirst(ParamPtr, WindowPtr);
    case DetectLogicType::DetectLast:
        return new DetectLogicLast(ParamPtr, WindowPtr);
    case DetectLogicType::Size:
        break;
    default:
        break;
    }

    // Not found
    std::cout << "DetectLogicType is not found : " << ParamPtr->JsonParam.detect_logic << std::endl;
    return nullptr;
}