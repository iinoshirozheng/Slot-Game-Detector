# Slot_Detector 自動辨識牌面程式


## 專案介紹

&emsp;&emsp;使用自動辨識拉霸機偵測程式可以快速而有效的判斷出拉霸機的牌面，省去手動判斷的時間，避免人為錯誤。可以應用在博弈產業、遊戲機台等方面，大幅提高作業效率與正確性。該程式能夠即時識別出拉霸機中的各種圖案及其排列，往後可以進一步透過數據分析，提供更多有用的資訊，如 RTP、CV 值等等，讓使用者更容易分析遊戲機台的設定機率。此方案專門設計為適用於各種不同型號的拉霸機，進一步提高其實用性，以下為偵測的效果畫面，詳細如何使用可以點擊至 [自動辨識牌面程式使用範本](https://gitlab.uj.com.tw/ray1105/slot_detector/-/tree/main/exe) 查看細項 :  

![Demo Detector Gif](./readme_src/Detector.gif)  

### --> 下載並使用，點擊此處的 [專案](https://gitlab.uj.com.tw/ray1105/slot_detector/-/archive/main/slot_detector-main.zip) 資料夾。


## 如何安裝?

### 使用 vcpkg 開源庫管理工具
&emsp;&emsp;vcpkg 是一款微軟開發的跨平台的C/C++開源庫管理器(open source package manager)。猶如pip之於python、npm之於node.js，有許多語言都有這個一個套件、庫的管理工具。我們在開發一個應用程式時，往往會需要使用到一些第三方的函式庫，你可以自行將原代碼複製到你的環境中自己編譯、工作，但要讓其正常的在你的專案中運作是不簡單的，常常做到最後，還沒實際開始編寫專案的部分就在上面花了不少時間了。開源庫管理器能極大的讓你在環境中建置、安裝第三方庫的作業量大幅減少。

&emsp;&emsp;使用 C++ 時每次都要自己編譯庫非常麻煩，但是使用了這個工具便會自動下載 source 並且自動幫你編譯 (下載時同時編譯)，並放在一個 package 集中管理，可以一次更新等等，環境也不會亂掉，可以說是對 C++ 的使用者非常友善的一個工具。
### 安裝

1. 首先，我們可以先前往他的 [vcpkg Github](https://github.com/Microsoft/vcpkg) 頁面並下載下來。
    ``` bat
    # 放在你要的位置
    >  cd "PATH"
    
    # 下載 vcpkg
    >  git clone https://github.com/Microsoft/vcpkg
    ```

2. 接著透過腳本編譯 vcpkg 庫管理工具
    ``` bat
    # 執行此批次檔案來編譯
    >  .\vcpkg\bootstrap-vcpkg.bat
    ```
3. 然後我們需要將 vcpkg 加入到 Visual Studio 裡面，。
    ``` bat
    # 將 vcpkg 加入到 Visual Studio 中
    >  vcpkg integrate install
    ```
    集成這個動作讓你使用Visual Studio裡面建立專案的時候，內建的MSBuild去建構專案，你可以直接inculde vcpkg所安裝的第三方套件，而編譯時也會自行進行linking的動作。而若是你使用CMake來建置你的專案時，需要加上 `-DCMAKE_TOOLCHAIN_FILE=` 的參數才能尋找到vcpkg安裝的第三方套件的位置。
4. 加入至環境變數
    ``` 
    # 在 Windows 中加入環境變數
    Winodws 收尋 > 環境變數 > 新增 Path > "剛剛下載的路徑"\vcpkg

    # 新增環境變數 (如果將要預設改為下載 x64 版本，不設定則預設為 x86)
    VCPKG_DEFAULT_TRIPLET=x64-windows
    ```
5. 下載需要的 Package，所需安裝時間可能會很久請耐心等待
    ``` bat
    # 下載 opencv
    >  vcpkg install opencv4

    # 下載 boost
    >  vcpkg install boost

    # 下載 json
    >  vcpkg install jsoncpp
    
    ```
    下載完成如果想查看安裝的檔案位置可以到 `"Path"\vcpkg\installed\x64-windows` 的位置查看
6. 由於文字辨識庫較難安裝，如安裝失敗可以參考 `startup-version` 的標籤查看詳細設定
    ``` bat
    # 可以先嘗試下載看看你的電腦是否能安裝 tesseract，如果可以則可以直接使用
    >  vcpkg install tesseract

    # 如果上面 tesseract 無法下載請刪除 tesseract ，下載 leptonica 就好(文字辨識所需要的庫，注意這個只需要上面無法下載時在下載)
    >  vcpkg remove tesseract
    >  vcpkg install leptonica
    ```  

    如果以上都下載安裝成功就不需要連結任何函示庫，失敗的話可以參考 `startup-version` 的標籤查看如何設定連結的庫。

---
### 編譯程式
1. 點擊整個 `Slot_Detector` 方案
2. 右鍵選擇 `建置方案 (F7)`
3. 編譯完後的檔案會在 `exe` 檔案夾中
4. 詳情請參考 [自動辨識牌面程式使用範本](https://gitlab.uj.com.tw/ray1105/slot_detector/-/tree/main/exe) 查看細項
