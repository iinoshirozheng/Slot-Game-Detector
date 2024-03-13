#pragma once
#include <iostream>
#include <vector>
#include "utils.h"


struct DetectLabel
{
	int label = -1;
	int pre_label = -1;
	int step_label = -1;
	std::string box_name = "";
	int box_index = -1;

	DetectLabel(std::string box_name, int box_index = -1, int label = -1, int pre_label = -1)
	{
		this->box_name = box_name;
		this->label = label;
		this->pre_label = pre_label;
		this->box_index = box_index;
	}
};

class DetectList
{
public:
	DetectList() = default;
	DetectList(const JsonParameter paramater);
	void UpdateStepData(const int& box_index, const int& detect_index);
	bool UpdateData();
	void SaveData();
	void CheckData(const std::vector<DetectLabel>& label_list, int index);
	int Get2DIndexLabel(int box_index);
	//inline std::vector<std::vector<int>> GetContinueFinalList() { return continue_final_list; }
	// inline void NextRound() { ++round; }
	inline void SetRound(int round) { this->round = round; }

	inline void SetTempList(){}
	inline std::string GetRound() { return round; }
	inline void PrintRound() { std::cout << "Round : " << this->round << std::endl; }
	void PrintOneList(int index);
	void PrintFinalList();
	//void PrintContinueFinalList();
	//void RefactorContinueFinalList();
	//void ClearContinueList();
	inline std::vector<std::vector<int>> GetFinalList() { return this->final_list; }
	inline std::vector<std::vector<int>> GetPreFinalList() { return this->pre_final_list; }

private:
	std::string round = "";
	// int money_label = -1;
	std::vector<std::vector<DetectLabel>> detect_labels;
	std::vector<std::vector<int>> final_list;
	std::vector<std::vector<int>> pre_final_list;
	// std::vector<std::vector<int>> continue_final_list;
	// void AddContinueFinalList(int index, int data);

};


