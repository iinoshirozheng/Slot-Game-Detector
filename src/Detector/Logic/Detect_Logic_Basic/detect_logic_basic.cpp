#include "detect_logic_basic.h"

// class DetectLogicFirst

DetectLogicFirst::DetectLogicFirst(DetectParameters* params, DebugWindow* debug_window)
{
    DetectLogic::Init(params, debug_window);
}

DetectLogicFirst::~DetectLogicFirst()
{
    DetectLogic::~DetectLogic();
}

void DetectLogicFirst::RunStatement()
{
#pragma omp barrier
    if (ParametersPtr->Status.cantDetect == 0)
    {
        if (ParametersPtr->DetectLists.UpdateData())
        {
            ParametersPtr->Status.is_all_same = false;
            ParametersPtr->DetectLists.SaveData();
        }
        else ParametersPtr->Status.is_all_same = true;

        DebugWindowPtr->PutLabelsText();
    }
    else ParametersPtr->Status.is_all_same = true;

    if (ParametersPtr->Status.is_all_same)
    {
        ParametersPtr->Status.last_same_count++;

        if (!ParametersPtr->SaveDatas.IsLock() && ParametersPtr->Status.last_same_count >= ParametersPtr->JsonParam.continue_detect_frame)
        {
            if (ParametersPtr->Status.continueCantDetect >= ParametersPtr->JsonParam.continue_detect_frame / 2)
            {
                ParametersPtr->Status.last_same_count = 0;
            }
            else
            {
                ParametersPtr->SaveDatas.UnLock();
                ParametersPtr->SaveDatas.SaveList(ParametersPtr->DetectLists.GetFinalList());
                ParametersPtr->SaveDatas.Lock();
            }
        }
    }
    else ParametersPtr->Status.last_same_count = 0;
}

// class DetectLogicLast

DetectLogicLast::DetectLogicLast(DetectParameters* params, DebugWindow* debug_window)
{
    DetectLogic::Init(params, debug_window);
}

DetectLogicLast::~DetectLogicLast()
{
    DetectLogic::~DetectLogic();
}

void DetectLogicLast::RunStatement()
{
#pragma omp barrier
    if (ParametersPtr->Status.cantDetect == 0)
    {
        if (ParametersPtr->DetectLists.UpdateData())
        {
            ParametersPtr->Status.is_all_same = false;
            ParametersPtr->DetectLists.SaveData();
        }
        else ParametersPtr->Status.is_all_same = true;

        DebugWindowPtr->PutLabelsText();
    }
    else ParametersPtr->Status.is_all_same = true;

    if (ParametersPtr->Status.is_all_same)
    {
        ParametersPtr->Status.last_same_count++;

        if (ParametersPtr->Status.max_last_same_count <= ParametersPtr->Status.last_same_count ||
            std::abs(ParametersPtr->Status.max_last_same_count - ParametersPtr->Status.last_same_count) <= 2 ||
            (ParametersPtr->Status.max_last_same_count > ParametersPtr->Status.last_same_count && ParametersPtr->Status.last_same_count >= ParametersPtr->JsonParam.continue_detect_frame))
        {
            if (ParametersPtr->Status.continueCantDetect >= ParametersPtr->JsonParam.continue_detect_frame / 2)
            {
                ParametersPtr->Status.last_same_count = 0;
            }
            else
            {
                ParametersPtr->Status.max_last_same_count = ParametersPtr->Status.last_same_count;
                ParametersPtr->SaveDatas.UnLock();
                ParametersPtr->SaveDatas.SaveList(ParametersPtr->DetectLists.GetFinalList());
                ParametersPtr->SaveDatas.Lock();
            }
        }
    }
    else ParametersPtr->Status.last_same_count = 0;
}
