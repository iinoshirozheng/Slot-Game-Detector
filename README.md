# Feature_Detection

此專案為為了驗證網路上的 slot game 機率是否正確所做的程式

---

## 相關設定  

- opencv 4.6.0  

> MacOS M1 Silicon 的安裝方式：  
>
> 1. 安裝 [Homebrew](https://brew.sh/)：  
>
> ```
> /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
> ```  
>
> 2. 安裝 [CMake](https://cmake.org/)：  
>
> ```  
> brew install cmake  
> ```  
>
> 3. 建立資料夾到你的儲存位置：
>
> ```  
> mkdir OpenCV && cd OpenCV
> ```  
>
> 4. 下 git 指令複製 [OpenCV](https://opencv.org/)：
>
> ```  
> git clone https://github.com/opencv/opencv.git
> ```  
>
> 5. 建立 build 資料夾：  
>
> ```  
> mkdir build && cd build
> ```  
>
> 6. cmake 設定編譯
>
> ```  
> cmake ../opencv/.
> ```  
>
> 7. cmake 設定編譯參數：  
>
> ```  
> arch -arm64 cmake ../opencv/ -DWITH_QT=OFF -DWITH_OPENGL=OFF -DFORCE_VTK=OFF -DWITH_TBB=OFF -DWITH_GDAL=OFF -DWITH_XINE=OFF -DBUILD_EXAMPLES=OFF -DBUILD_ZLIB=OFF -DBUILD_TESTS=OFF .`  
> ```  
>
> 8. 設定 Mac 編譯的線程數 (需要到 zprofile 中增加)：
>
> ```
> vim ~./zprofile
> ```
> >
> > ```
> > # 設定 CPU 線程數
> > export CPUS=$(sysctl -n hw.physicalcpu)  
> > ```
> >
> ```  
> source ~/.zprofile
> ```
>
> 9. 編譯 opencv：  
>
> ```  
> arch -arm64 sudo make -j $CPUS
> ```  
>
> 10. 最後在 make install 一下：
>
> ```  
> arch -arm64 sudo make install  
> ```  
>
> - PS. 這是我電腦成功的方式，每台電腦可能略有差異，此方法有效不代表所有裝置都有效。  
>
> 11. 設定編譯好的 opencv 環境路徑  
> 12. 順便記一下最後這個指令可以整理 homebrew package
> ```
> brew update && brew upgrade && brew cleanup; brew doctor
> ```

使用語言

- C/C++  
  
## 參考資料與連結
