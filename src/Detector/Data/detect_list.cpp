#include "detect_list.h"


DetectList::DetectList(const JsonParameter paramater)
{
	std::vector<DetectLabel> detect_label;
	// 所有的輪帶表
	for (auto& labels : paramater.label_seq)
	{
		detect_label.clear();
		// 每條輪帶表
		for (auto& label : labels)
		{
			detect_label.emplace_back(DetectLabel(paramater.select_labels[label].name, label));
		}
		this->detect_labels.emplace_back(detect_label);
	}
}

void DetectList::UpdateStepData(const int &box_index, const int &detect_index)
{
	int row_index = 0;
	int col_index = box_index;

	for (const auto& row : detect_labels) {
		if (col_index < row.size()) {
			break;
		}
		col_index -= row.size();
		row_index++;
	}
	this->detect_labels[row_index][col_index].step_label = detect_index;
}

bool DetectList::UpdateData()
{
	int allSameCount = 0;
	int num = 0;
	for (auto& labels : detect_labels)
	{
		int count = 0;
		for (auto& label : labels)
		{
			if (label.label == label.step_label) count++;
			num++;
		}
		allSameCount += count;
		if (count < labels.size())
		{
			for (auto& label : labels)
			{
				label.pre_label = label.label;
				label.label = label.step_label;
			}
		}
	}
	return !(allSameCount == num);
}



void DetectList::SaveData()
{
	if (!final_list.size())
	{
		std::vector<int> temp;
		for (auto& labels : detect_labels)
		{
			temp.clear();
			for (auto& label : labels)
			{
				temp.emplace_back(label.label);
			}
			final_list.emplace_back(temp);
		}
		// continue_final_list.clear();

		for (auto& labels : detect_labels)
		{
			std::vector<int> temp_continue = {};
			// continue_final_list.emplace_back(temp_continue);
		}
		this->pre_final_list = this->final_list;
	}
	else
	{
		if (this->pre_final_list != this->final_list)
			this->pre_final_list = this->final_list;
		//check data is continue
		for (int i = 0; i < detect_labels.size(); i++)
		{
			CheckData(detect_labels[i], i);
		}

	}
}

void DetectList::CheckData(const std::vector<DetectLabel>& label_list, int index)
{

	for (int i = 0; i < label_list.size(); i++)
	{
		if (label_list[i].label == -1) return;
	}

	bool isContinue = true;
	
	for (int i = 0; i < label_list.size() -1; i++)
	{
		if (label_list[i + 1].label != label_list[i].pre_label ) isContinue = false;
	}

	if (isContinue)
	{

		// check final list is continue
		bool same = true;
		for (int i = 0; i < label_list.size(); i++)
		{
			if (label_list[i].pre_label != final_list[index][i] ) same = false;
		}

		if (same)
		{
			final_list[index].insert(final_list[index].begin(), label_list[0].label);

			// AddContinueFinalList(index, label_list[0].label);

		}
		else
		{
			final_list[index].clear();
			for (int i = 0; i < label_list.size(); i++)
			{
				final_list[index].emplace_back(label_list[i].pre_label);
			}
			final_list[index].insert(final_list[index].begin(), label_list[0].label);

			// AddContinueFinalList(index, -1);

		}

	}
	else
	{
		// std::cout << "not continue : " << std::endl;

		final_list[index].clear();
		for (int i = 0; i < label_list.size(); i++)
		{
			final_list[index].emplace_back(label_list[i].label);
		}

		// AddContinueFinalList(index, -1);

	}
}

int DetectList::Get2DIndexLabel(int box_index)
{
	int row_index = 0;
	int col_index = box_index;

	for (const auto& row : detect_labels) {
		if (col_index < row.size()) {
			break;
		}
		col_index -= row.size();
		row_index++;
	}
	return detect_labels[row_index][col_index].label;
}

void DetectList::PrintOneList(int index)
{
	for (auto& data : final_list[index])
	{
		std::cout << FileUtils::labels[data] << ", ";
	}
	std::cout << std::endl;
}

void DetectList::PrintFinalList()
{
	for (auto& list : final_list)
	{
		std::cout << "  ";
		for (auto& element : list)
		{
			std::cout << FileUtils::labels[element] << ", ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

//void DetectList::PrintContinueFinalList()
//{
//	for (auto& list : continue_final_list)
//	{
//		std::cout << "  ";
//		for (auto& element : list)
//		{
//			if (element == -1) std::cout << "-1, ";
//			else std::cout << FileUtils::labels[element] << ", ";
//		}
//		std::cout << std::endl;
//	}
//	std::cout << std::endl;
//}

//void DetectList::RefactorContinueFinalList()
//{
//	int min_size = INT_MAX;
//	for (auto& list : continue_final_list)
//	{
//		// check first is -1 or not
//		if (list.size() && list[0] == -1)
//		{
//			list.erase(list.begin());
//		}
//		min_size = min_size > list.size() ? list.size() : min_size;
//	}
//	for (auto& list : continue_final_list)
//	{
//		while (list.size() > min_size)
//		{
//			list.erase(list.begin());
//		}
//	}
//}

//void DetectList::ClearContinueList()
//{
//	for (auto& list : continue_final_list)
//	{
//		list.clear();
//	}
//}

//void DetectList::AddContinueFinalList(int index, int data)
//{
//	if (!continue_final_list[index].size())
//		continue_final_list[index].emplace_back(data);
//	else if (continue_final_list[index][0] != -1)
//		continue_final_list[index].insert(continue_final_list[index].begin(), data);
//}
