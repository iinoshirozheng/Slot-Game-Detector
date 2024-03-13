#pragma once

#include <iostream>
#include <vector>
#include "utils.h"

class SaveData
{
public:
    SaveData() = default;
    SaveData(std::vector<std::vector<int>> seq_list);
    void SaveList(std::vector<std::vector<int>> lock_list);
    void PrintSaveList();
    void CreateCSV(std::string video_name);
    void SaveCSV(std::ofstream &csv_file);
    void CreateHeaderCSV(std::ofstream& csv_file);
    void CreateTemplateAndBat();

    inline std::string GetRound() { return this->round; }
    inline std::string GetMoney() { return this->money; }
    inline std::string GetMode() { return this->mode; }
    inline std::string GetCsvPathName() { return this->csv_name; }
    inline std::vector<std::vector<int>> GetMainList() { return main_list; }
    inline std::vector<std::vector<int>> GetSaveList() { return pre_save_csv_list; }
    inline void Lock() { this->lock = true; }
    inline void EarlyLock() { this->early_lock = true; }
    inline void UnLock() { this->lock = false; this->early_lock = false; }
    inline bool IsLock() { return this->lock; }
    inline bool IsEarlyLock() { return this->early_lock ; }
    inline void SaveRound(std::string round) { if (this->lock) return; this->round = round; }
    inline void SaveMode(std::string mode_name) { if (this->lock) return; this->mode = mode_name; }
    inline void SaveMoney(std::string money) { if (this->lock) return; this->money = money; }

private:
    std::string round = "";
    std::string money = "";
    bool lock = false;
    bool early_lock = false;
    std::string mode;
    std::vector<std::vector<int>> main_list;
    std::vector<std::vector<int>> pre_save_csv_list;
    std::string csv_name;
    std::string pre_round;

    std::string RemoveMoneyComma(std::string money);
};

