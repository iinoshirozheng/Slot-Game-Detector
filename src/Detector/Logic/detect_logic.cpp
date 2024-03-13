#include "detect_logic.h"
#include "utils.h"

DetectLogic::DetectLogic()
{
}

DetectLogic::DetectLogic(DetectParameters* ParamPtr, DebugWindow* WindowPtr)
{
    Init(ParamPtr, WindowPtr);
}

DetectLogic::~DetectLogic()
{
    if (DetectorORB != nullptr)
    {
        delete DetectorORB;
        DetectorORB = nullptr;
    }
    if (ModeDetectorORB != nullptr)
    {
        delete ModeDetectorORB;
        ModeDetectorORB = nullptr;
    }
    if (!this->ParametersPtr)
    {
		this->ParametersPtr = nullptr;
	}
    if (!this->DebugWindowPtr)
    {
		this->DebugWindowPtr = nullptr;
	}
}

void DetectLogic::Init(DetectParameters* ParamPtr, DebugWindow* WindowPtr)
{
    // check max thread
    std::cout << "omp_thread : " << omp_get_max_threads() << std::endl;

    this->ParametersPtr = ParamPtr;
    this->DebugWindowPtr = WindowPtr;
    SetORB();
}

bool DetectLogic::RunLogic()
{
    // Check Pointers
    if(!CheckPointers()) return false;
    
    // MODE
    if (!DetectMode()) return false;
    
    // LABELS
    if (DetectLabels() == -1) return false;
    
    // COUNT CAN'T DETECT LABELS
    CountCanNotDetect();
    
    // ROUND AND MONEY
    DetectRoundAndMoney();

    // Run Statement Logic
    RunStatement();

    return true;
}

bool DetectLogic::DetectMode()
{
    if (!ModeDetectorORB)
    {
        Print("Error : ModeDetectorORB is nullptr");
        return false;
    }
    for (auto& mode_label : ParametersPtr->JsonParam.mode_labels)
    {
        if (!ParametersPtr->Status.last_same_count)
        {
            ParametersPtr->Status.temp.mode = ModeDetectorORB->OMP_FindCurrentLabel(DebugWindowPtr->Frame(mode_label.label_box));

            if (ParametersPtr->Status.temp.mode == ParametersPtr->Status.temp.pre_mode) ParametersPtr->Status.temp.count_same_mode++;
            else ParametersPtr->Status.temp.count_same_mode = 0;

            if (ParametersPtr->Status.temp.count_same_mode == ParametersPtr->JsonParam.continue_mode_frame)
            {
                // std::cout << "Save Mode" << std::endl;
                if (ParametersPtr->Status.temp.renew_mode != ParametersPtr->Status.temp.mode) ParametersPtr->SaveDatas.UnLock();
                ParametersPtr->Status.temp.renew_mode = ParametersPtr->Status.temp.mode;
                //ParametersPtr->Status.mode_name = "";
                //Print(FileUtils::labels_mode.size());
                ParametersPtr->Status.mode_name = ParametersPtr->Status.temp.renew_mode != -1 ? FileUtils::labels_mode[ParametersPtr->Status.temp.renew_mode] : "";
                if (ParametersPtr->Status.temp.renew_mode != -1) ParametersPtr->SaveDatas.SaveMode(ParametersPtr->Status.mode_name);
            }
        }

        DebugWindowPtr->DrawModeLabel(mode_label.label_box);

        break; // now only can detect one mode box
    } // Mode

    return true;
}

int DetectLogic::DetectLabels()
{
    if (!DetectorORB)
    {
        Print("Error : DetectorORB is nullptr");
        return -1;
    }
    int cantDetect = 0;
#pragma omp parallel for reduction(+:cantDetect)
    for (int i = 0; i < ParametersPtr->JsonParam.select_labels.size(); i++)
    {
        //detector_ORB.FindCurrentLabel(frame(ParametersPtr->JsonParam.select_labels[i].label_box));
        //int index = detector_ORB.GetMaxPercentIndex();
        //int index = detector_ORB.GetMaxIndex();
        int index = DetectorORB->OMP_FindCurrentLabel(DebugWindowPtr->Frame(ParametersPtr->JsonParam.select_labels[i].label_box));

        DebugWindowPtr->DrawLabelRect(ParametersPtr->JsonParam.select_labels[i].label_box);

        if (index == -1) cantDetect++;

        ParametersPtr->DetectLists.UpdateStepData(ParametersPtr->JsonParam.select_labels[i].index, index);
    } // SELECT LABELS
    
    return ParametersPtr->Status.cantDetect = cantDetect;
}

void DetectLogic::CountCanNotDetect()
{
    // check can't detect
    ParametersPtr->Status.cantDetect ? ParametersPtr->Status.continueCantDetect++ : ParametersPtr->Status.continueCantDetect = 0;
}

void DetectLogic::DetectRoundAndMoney()
{
#pragma omp parallel for
    for (int i = 0; i < ParametersPtr->JsonParam.text_labels.size(); i++)
    {
        DebugWindowPtr->TurnFrameToThreshold();

        if (ParametersPtr->Status.mode_name == "NORMAL" && ParametersPtr->JsonParam.text_labels[i].name == "Round")
        {
            ParametersPtr->Status.temp.round = DetectorsOCR[0].DetectTextLine(DebugWindowPtr->Frame(ParametersPtr->JsonParam.text_labels[i].label_box));

            // if (ParametersPtr->Status.temp.preRound == "") ParametersPtr->Status.temp.preRound = ParametersPtr->Status.temp.round;
            if (!FileUtils::FilterString(ParametersPtr->Status.temp.money) || !FileUtils::FilterString(ParametersPtr->Status.temp.round) || ParametersPtr->Status.temp.mode == -1) break;
            if (ParametersPtr->Status.temp.round != ParametersPtr->Status.temp.preRound && !ParametersPtr->Status.countRound)
            {
                ParametersPtr->Status.countRound = true;
                ParametersPtr->SaveDatas.UnLock();
                ParametersPtr->SaveDatas.SaveRound(ParametersPtr->Status.temp.preRound);
                ParametersPtr->SaveDatas.Lock();
            }
            else if (ParametersPtr->Status.temp.round == ParametersPtr->Status.temp.preRound && ParametersPtr->Status.countRound)
            {
                if (ParametersPtr->Status.temp.round == ParametersPtr->Status.temp.preRound)
                    ParametersPtr->Status.sameRoundCount++;
                else
                    ParametersPtr->Status.sameRoundCount = 0;

                if (ParametersPtr->Status.sameRoundCount)
                {

                    // std::cout << "NEXT ROUND" << std::endl;
                    ParametersPtr->Status.nextRound = true;
                    ParametersPtr->Status.countRound = false;
                    ParametersPtr->Status.sameRoundCount = 0;
                }
            }

            DebugWindowPtr->DrawRoundLabel(ParametersPtr->JsonParam.text_labels[i].label_box);

        } // if Round

        // detect Money;
        else if (ParametersPtr->JsonParam.text_labels[i].name == "Money")
        {
            ParametersPtr->Status.temp.money = DetectorsOCR[1].DetectTextLine(DebugWindowPtr->Frame(ParametersPtr->JsonParam.text_labels[i].label_box));

            if (ParametersPtr->Status.temp.preMoney == "") ParametersPtr->Status.temp.preMoney = ParametersPtr->Status.temp.money;


            if (std::abs((int)ParametersPtr->Status.temp.preMoney.size() - (int)ParametersPtr->Status.temp.money.size()) > 1 &&
                (ParametersPtr->Status.temp.preMoney.find(ParametersPtr->Status.temp.money) != std::string::npos ||
                    ParametersPtr->Status.temp.money.find(ParametersPtr->Status.temp.preMoney) != std::string::npos))
            {
                ParametersPtr->Status.temp.money = ParametersPtr->Status.temp.preMoney;
            }

            if (ParametersPtr->Status.temp.money != ParametersPtr->Status.temp.preMoney && !ParametersPtr->Status.countMoney)
            {
                // std::cout << "Start Detect Money" << std::endl;
                // Start compare
                ParametersPtr->Status.countMoney = true;
                if (ParametersPtr->Status.mode_name == "FREE")
                    ParametersPtr->Status.temp.preRound = ParametersPtr->Status.temp.preMoney;
            }
            else if (ParametersPtr->Status.temp.money == ParametersPtr->Status.temp.preMoney && ParametersPtr->Status.countMoney)
            {
                ParametersPtr->Status.sameMoneyCount++;
                if (ParametersPtr->Status.sameMoneyCount == ParametersPtr->JsonParam.continue_number_frame)
                {
                    // std::cout << "NEXT Money " << std::endl;
                    ParametersPtr->Status.nextMoney = true;
                    ParametersPtr->Status.countMoney = false;
                    ParametersPtr->Status.sameMoneyCount = 0;
                    if (ParametersPtr->Status.mode_name == "FREE")
                    {
                        ParametersPtr->SaveDatas.UnLock();
                        ParametersPtr->Status.temp.round = ParametersPtr->Status.temp.money;
                        ParametersPtr->SaveDatas.SaveRound(ParametersPtr->Status.temp.preRound);
                        ParametersPtr->Status.nextRound = true;
                        ParametersPtr->Status.nextMoney = false;
                        ParametersPtr->SaveDatas.Lock();
                    }
                }
            }

            DebugWindowPtr->DrawMoneyLabel(ParametersPtr->JsonParam.text_labels[i].label_box);
        } // else if Money

    } // for loop
}

bool DetectLogic::SetORB()
{
    if (ParametersPtr == nullptr)
    {
		std::cout << "Error Set ORB : ParametersPtr is nullptr" << std::endl;
		return false;
	}

    std::vector<cv::Mat> label_images, mode_images;
    CvUtils::GetDetectImages(FileUtils::GetRootDirectory() + PATH_LABELS, label_images, mode_images);

    if (!label_images.size())
    {
        std::cout << "Error Set ORB : Could not found label image, please check the < Labels > file" << std::endl;
        return false;
    }

    // DETECTOR
    DetectorORB = new cv_detector::ORB(label_images, ORB_FEATURE_NUMBER, ParametersPtr->JsonParam.detector_threshold);
    // MODE DETECTOR
    ModeDetectorORB = new cv_detector::ORB(mode_images, ORB_FEATURE_NUMBER, ParametersPtr->JsonParam.detector_threshold);

    return true;
}

bool DetectLogic::CheckPointers()
{
    if (ParametersPtr == nullptr)
    {
        Print("Error Check Pointers : ParametersPtr is nullptr");
		return false;
	}
    if (DebugWindowPtr == nullptr)
    {
		Print("Error Check Pointers : DebugWindowPtr is nullptr");
		return false;
	}
    if (DetectorORB == nullptr)
    {
        Print("Error Check Pointers : DetectorORB is nullptr");
		return false;
	}
    if (ModeDetectorORB == nullptr)
    {
        Print("Error Check Pointers : ModeDetectorORB is nullptr");
		return false;
	}

	return true;
}
