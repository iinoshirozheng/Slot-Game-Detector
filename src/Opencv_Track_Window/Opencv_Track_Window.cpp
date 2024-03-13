// Opencv_Track_Window.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <Windows.h>
#include <opencv2/opencv.hpp>

void CaptureScreenshot(HWND hwnd, cv::Mat& screenshot) {
    
    RECT rect;
    // Get the size of the window
    GetWindowRect(hwnd, &rect);

    // Create a device context for the window
    HDC hdcWindow = GetDC(hwnd);

    // Create a memory device context for the screenshot
    HDC hdcMemDC = CreateCompatibleDC(hdcWindow);

    // Create a bitmap for the screenshot
    HBITMAP hbmScreen = CreateCompatibleBitmap(hdcWindow, rect.right - rect.left, rect.bottom - rect.top);

    // Select the bitmap into the memory device context
    SelectObject(hdcMemDC, hbmScreen);

    // Capture the window as an image
    PrintWindow(hwnd, hdcMemDC, PW_RENDERFULLCONTENT);

    // Convert the screenshot to a Mat object
    screenshot.create(rect.bottom - rect.top, rect.right - rect.left, CV_8UC4);
    GetBitmapBits(hbmScreen, screenshot.total() * screenshot.elemSize(), screenshot.data);

    // Release the resources
    DeleteObject(hbmScreen);
    DeleteDC(hdcMemDC);
    ReleaseDC(hwnd, hdcWindow);
}


int main()
{
    // Replace "Window Title" with the title of the window you want to track
    const char* windowTitle = "BlueStacks App Player 1";
    HWND hwnd = FindWindowA(NULL, windowTitle);

    // Free the memory used for the wide-character string
    if (hwnd == nullptr) return -1;

    
    cv::Mat screenshot;
    double tick;
    int waitKey = -1;
    cv::Rect roi;
    cv::Size preSize;
    // Replace hwnd with the window handle obtained from FindWindow()
    while ((tick = cv::getTickCount()) && (waitKey = cv::waitKey(1)) != 27) {
        CaptureScreenshot(hwnd, screenshot);
        if (!preSize.empty() && screenshot.size() != preSize)
        {
            std::cout << "tracking windows failed" << std::endl;
            return-1;
        }
        // cv::cvtColor(screenshot, screenshot, cv::COLOR_RGBA2GRAY);

        // Display the processed screenshot
        double fps = 1.0 / ((cv::getTickCount() - tick) / cv::getTickFrequency());
        
        cv::putText(screenshot, "FPS : " + std::to_string(fps), cv::Point(screenshot.size().width - 200, screenshot.size().height - 50), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(10, 255, 255), 2);
        cv::imshow("Window Tracking", screenshot);
        std::cout << screenshot.size() << std::endl;
        preSize = screenshot.size();
    }

    cv::destroyAllWindows();
}


