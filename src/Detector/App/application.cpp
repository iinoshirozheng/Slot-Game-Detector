#include "application.h"
#include "process.h"
#include "utils.h"
#include "../Logic/choise_logic.h"

Application::Application(int argc, char* argv[])
{
    Argc = argc;
    Argv = argv;
}

Application::~Application()
{    
    // close csv file
    CSV.close();

    if (DetectLogicPtr)
    {
        delete DetectLogicPtr;
        DetectLogicPtr = nullptr;
    }
}

void Application::Run()
{
    // init Paramters and Variables
    if(!Init()) return;

    // Create csv file
    CreateCSV();

    // Create Template And Bat
    Parameters.SaveDatas.CreateTemplateAndBat();

    // record and detect screen [ while loop ] 
    RecordAndDetectScreen();
}

void Application::RecordAndDetectScreen()
{
    DebugWindow window = DebugWindow(&Parameters);
    if (!(DetectLogicPtr = GetDetectLogic(&Parameters, &window)))
    {
        Print("Error : Detect Logic Setting Error !");
		return;
    }

    while (window.StartTickFPS()) // grab -> point to next frame, but don't decode
    {
        // should reduce frame
        if (window.CanReduceFrame())
        {
            continue;
        }

        // Check
        {
            if (!window.CheckParamPtr())
            {
                Print("Error : Check Param Ptr");
                return;
            }

            if (!window.CaptureScreenshot())
            {
                return;
            }

            // check pressed key
            if (window.CheckPressedEndKey())
            {
                return;
            }
        }

        // reset canvas
        window.ResetCanvas();

        // copy gray scale image, and use it to detect
        window.TurnFrameToGrayScale();

        if(!DetectLogicPtr->RunLogic()) return;

        // Save this round
        SaveRound();

        // check early stop
        if(EarlyStop()) return;

        UpdatePrevStatus();
        window.PutCanvasStatusText();
        window.Show();
    } // while loop


}

bool Application::Init()
{
    // set root dir
    if (!SetRootDirPath()) return false;

    // set json param
    if (!SetJsonParam()) return false;

    // Set Data
    Parameters = DetectParameters(FileUtils::GetJsonParameter());

    // set window handle
    if (!SetWindowHandle()) return false;

    return true;
}

bool Application::SetRootDirPath()
{
    std::string dir;
    if (Argc == 1)
    {
        std::cout << "Please input your Dataset : ";
        std::cin >> dir;
    }
    else if (Argc == 2)
    {
        dir = std::string(Argv[1]);
    }
    else
    {
        std::cout << "error argc !" << std::endl;
        return false;
    }

    if (dir.back() != '/' || dir.back() != '\\') dir = dir + '/';
    std::cout << dir << std::endl;
    FileUtils::SetRootDirectory(dir);
    
    return true;
}

bool Application::SetJsonParam()
{
    std::cout << FileUtils::GetRootDirectory() + PATH_JSON << std::endl;
    if (!FileUtils::ReadJsonFile(FileUtils::GetRootDirectory() + PATH_JSON))
    {
        Print("Could not read the json file!");
        return false;
    }
    auto jsonParam = FileUtils::GetJsonParameter();
    // Set Money or Round label
    if (jsonParam.text_labels.size() < 2)
    {
        std::cout << "label size :" << jsonParam.text_labels.size();
        Print("Could not found correct Money or Round label, please make sure both are exist !");
        return false;
    }
    return true;
}

bool Application::SetWindowHandle()
{
    // 設定 jasn file 的 handle
    if (!CvUtils::SetWindowHandle(Parameters.JsonParam.window_name.c_str()))
    {
        Print("can't find the windows title, please check the title is correct");
        return false;
    }
    return true;
}

bool Application::EarlyStop()
{
    if (Parameters.Status.temp.preRound == Parameters.Status.temp.round)
    {
        Parameters.Status.countEnd++;
        // 大於 10 分鐘就停止
        if (Parameters.Status.countEnd > CvUtils::GetFPS() * 600)
            return true;
    }
    else Parameters.Status.countEnd = 0;

    return false;
}

void Application::UpdatePrevStatus()
{
    // update previous status
    Parameters.Status.temp.preRound = Parameters.Status.temp.round;
    Parameters.Status.temp.pre_mode = Parameters.Status.temp.mode;
    Parameters.Status.temp.preMoney = Parameters.Status.temp.money;
}

void Application::CreateCSV()
{
    // create csv file
    Parameters.SaveDatas.CreateCSV(Parameters.JsonParam.window_name);

    // open csv and write the csv header
    CSV = std::ofstream(Parameters.SaveDatas.GetCsvPathName());
    Parameters.SaveDatas.CreateHeaderCSV(CSV);
}

void Application::SaveRound()
{
    if(!FileUtils::FilterString(Parameters.Status.temp.money) || !FileUtils::FilterString(Parameters.Status.temp.round) || Parameters.Status.temp.mode == -1) return;
    if (Parameters.Status.nextRound && !Parameters.Status.last_same_count)
    {
        if (Parameters.SaveDatas.IsLock())
        {
            // save previous round
            Parameters.SaveDatas.UnLock();
            Parameters.SaveDatas.SaveMoney(Parameters.Status.temp.money);
            Parameters.SaveDatas.SaveCSV(CSV);

            Parameters.DetectLists.SaveData();
            Parameters.SaveDatas.SaveList(Parameters.DetectLists.GetFinalList());
        }
        Parameters.Status.nextRound = false;
        Parameters.Status.max_last_same_count = INT_MIN;
        Parameters.SaveDatas.Lock();
    }
    else if (Parameters.Status.nextMoney && Parameters.SaveDatas.IsLock())
    {
        Parameters.SaveDatas.UnLock();
        // saveData.SaveList(detectList.GetFinalList());
        Parameters.SaveDatas.SaveMoney(Parameters.Status.temp.money);
        Parameters.SaveDatas.SaveCSV(CSV);
        Parameters.Status.nextMoney = false;
    }
}

