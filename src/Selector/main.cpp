#include "process.h"
#include "utils.h"
#include "selector.h"
#include <fstream>

// SELECTOR code
int main(int argc, char* argv[])
{
    int delay = SELECTOR_DELAY;

    if (argc != 2)
    {
        std::cout << "didn't setting the argument, please cheak you have setting window name with argument 1" << std::endl;
        return -1;
    }

    LabelSelector selector = LabelSelector();
    if (selector.Run(argv[1], delay))
    {
        // if didn't give the arg video path, output a empty template

        if (FileUtils::CreateDataset())
        {
            std::cout << "Create Dataset successfully!" << std::endl;
            // FileUtils::CopyVideoFile(FileUtils::video_path);
            // std::cout << R"(Copy video to "Videos" directory!)" << std::endl;
            selector.SaveSelectedFrame(FileUtils::GetRootDirectory() + "save_selected_image.png" );
            std::cout << R"(Save selected image to root directory!)" << std::endl;
            std::cout << "save_selected_image : " << FileUtils::GetRootDirectory() + "save_selected_image.png" << std::endl;
            // Create bat file
            {
                std::ofstream batFile(FileUtils::GetRootDirectory() + "Detect.bat");
                batFile << "@echo off" << std::endl;
                batFile << "start " + std::string(DETECTOR_PATH) +" %~dp0" << std::endl;
                batFile.close();
            }
            // Create txt file include detect logic
            {
				std::ofstream txtFile(FileUtils::GetRootDirectory() + "DetectLogic.txt");
                txtFile << "0 --> Detect All   無 (需要設定數字才會辨識) " << std::endl;
                txtFile << "1 --> Detect First 偵測第一個牌面" << std::endl;
                txtFile << "2 --> Detect Last  偵測最後一個牌面" << std::endl;
				txtFile.close();
			}
        }
        else
        {
            std::cout << "Create Dataset failed!" << std::endl;
            std::cerr << "Error : FileUtils::CreateDataset() --> Create Dataset failed!" << std::endl;
            system("pause");
            return -1;
        }
        
    }
    system("pause");
    return 0;
}