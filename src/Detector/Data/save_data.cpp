#include "save_data.h"
#include <fstream>
#define _CRT_SECURE_NO_WARNINGS
#include <ctime>


SaveData::SaveData(std::vector<std::vector<int>> seq_list)
{
	for (auto &list : seq_list)
	{
		std::vector<int> temp;
		for (int i = 0; i < list.size(); i++)
		{
			temp.emplace_back(-1);
		}
		this->main_list.emplace_back(temp);
	}
}

void SaveData::SaveList(std::vector<std::vector<int>> lock_list)
{
	if (this->lock || !lock_list.size()) return;
	// reset data


	for (auto & list : this->main_list)
		for (auto& data : list)
			data = -1;

	for (int i = 0; i < this->main_list.size(); i++)
	{
		for (int j = 0; j < main_list[i].size(); j++)
		{
			main_list[i][j] = lock_list[i][j];
			// lock_list[i].erase(lock_list[i].begin());
		}
		for (int j = 0; j < main_list[i].size(); j++)
		{
			lock_list[i].erase(lock_list[i].begin());
		}
	}

	// refactor data
	// this->add_list = lock_list;
}



void SaveData::PrintSaveList()
{
	if (this->round == "") return;

	// GET MAX SIZE
	int max = INT_MIN;
	int space = 20;
	for (auto& list : this->main_list)
	{
		if (max > list.size()) max = list.size();
	}

	std::cout << std::endl;
	for (int j = 0; j < main_list.size()* space; j++)
	{
		std::cout << "=" ;
	}
	std::cout << std::endl << std::endl;

	std::cout << "\tRound : " << this->round ;
	std::cout << "\tMode : " << this->mode ;
	std::cout << "\tMoney : " << this->money ;
	std::cout << std::endl << std::endl << std::endl;


	for (int i = 0; i < max; i++)
	{
		for (int j = 0; j < main_list.size(); j++)
		{
			std::cout << "\t"  << FileUtils::labels[main_list[j][i]] << "\t";
		}
		std::cout << std::endl << std::endl;
	}
	std::cout << std::endl;


	for (int j = 0; j < main_list.size() * space; j++)
	{
		std::cout << "=";
	}
	std::cout << std::endl;
	std::cout << std::endl;

}

void SaveData::CreateCSV(std::string video_name)
{
	video_name.erase(std::remove_if(video_name.begin(), video_name.end(), [](char c) {
		return c == '\\' || c == '|' || c == '^' || c == '/' || c == '>' || c == '<' || c == ':' || c == '*' || c == '?' || c == '\"';
		}), video_name.end());

	video_name = FileUtils::Split(video_name, ' ').front();

	// create output file if not exist
	FileUtils::MakeDirectory(FileUtils::GetRootDirectory()+PATH_OUTPUT);
	
	// Create directory with current date
	std::time_t now = std::time(nullptr);
	std::tm tm;
	localtime_s(&tm, &now);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y-%m-%d");
	std::string directoryName = oss.str();
	FileUtils::MakeDirectory(FileUtils::GetRootDirectory() + PATH_OUTPUT + "/" + directoryName);

	// Create CSV file with current time
	std::ostringstream oss2;
	oss2 << std::put_time(&tm, "%H-%M-%S");
	this->csv_name = FileUtils::GetRootDirectory() + PATH_OUTPUT + "/" + directoryName + "/(" + directoryName + "-" + oss2.str() + ")-" + video_name + ".csv";
	std::ofstream file(this->csv_name);
	file.close();

}

void SaveData::SaveCSV(std::ofstream& csv_file)
{
	if (this->round == "0" || this->round == "" || this->round == "." || this->pre_save_csv_list == this->main_list || this->pre_round == this->round) return;

	// return if have any can't detect
	for (int i = 0; i < main_list.size(); i++)
	{
		for (int j = 0; j < main_list[i].size(); j++)
		{
			if (main_list[i][j] == -1) return;
		}
	}

	this->money = RemoveMoneyComma(this->money);

	csv_file << this->round << "," << this->mode << "," << this->money;
	for (int i = 0; i < main_list.size(); i++)
	{
		for (int j = 0; j < main_list[i].size(); j++)
		{
			if (main_list[i][j] == -1) csv_file << ",NULL";
			else csv_file << "," << FileUtils::labels[main_list[i][j]];
		}
	}
	csv_file << std::endl;
	PrintSaveList();
	std::cout << "FPS : " << CvUtils::GetFPS() << std::endl;
	this->pre_round = this->round;
	this->pre_save_csv_list = this->main_list;
	
}

void SaveData::CreateHeaderCSV(std::ofstream& csv_file)
{
	if (!main_list.size()) return;
	csv_file << "Round,Mode,Money";
	for (int i = 0; i < main_list.size(); i++)
	{
		for (int j = 0; j < main_list[i].size(); j++)
		{
			csv_file << ",L" << i+1 << j+1;
		}
	}
	csv_file << std::endl;
}

void SaveData::CreateTemplateAndBat()
{
	// create template
	std::ofstream temp(FileUtils::GetRootDirectory() + PATH_OUTPUT + "/Template.csv");
	temp << "Round";
	for (int i = 0; i < main_list.size() - 1; i++)
	{
		temp << ",";
	}
	temp << std::endl;

	temp << "Mode";
	for (int i = 0; i < main_list.size() - 1; i++)
	{
		temp << ",";
	}
	temp << std::endl;

	temp << "Money";
	for (int i = 0; i < main_list.size() - 1; i++)
	{
		temp << ",";
	}
	temp << std::endl;

	int maxTemp = std::max_element(main_list.begin(), main_list.end(),
		[](const auto& a, const auto& b) { return a.size() < b.size(); })->size();
	// std::cout << maxTemp << std::endl;

	for (int i = 0; i < maxTemp; i++)
	{
		for (int j = 0; j < main_list.size(); j++)
		{
			if (j) temp << ",";
			if (i < main_list[j].size()) temp << "L" << j+1 << i+1;
		}
		temp << std::endl;
	}

	temp.close();

	std::ofstream batFile(FileUtils::GetRootDirectory() + PATH_OUTPUT + "/Convert.bat");
	batFile << "@echo off" << std::endl;
	batFile << "start " + std::string(TEMPLATE_PATH) << std::endl;
	batFile.close();
}

std::string SaveData::RemoveMoneyComma(std::string money)
{
	std::string result = money;

	// Remove ',' '.' ' ' characters
	result.erase(std::remove(result.begin(), result.end(), ','), result.end());
	result.erase(std::remove(result.begin(), result.end(), '.'), result.end());
	result.erase(std::remove(result.begin(), result.end(), ' '), result.end());

	return result;
}
