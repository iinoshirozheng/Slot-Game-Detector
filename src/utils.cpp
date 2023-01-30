#include "utils.h"

#ifdef __APPLE__
#include <dirent.h>
#include <sys/stat.h>
#elif _WIN32
#include <io.h>
#include <boost/filesystem.hpp>
#endif

std::vector<std::string> FileUtils::labels;
JsonParamater FileUtils::jsonData;
std::string FileUtils::rootDirectory;
std::string FileUtils::video_path;

const std::vector<std::string> FileUtils::Split(const std::string &str, const char &delimiter)
{
	std::vector<std::string> result;
	std::stringstream ss(str);
	std::string tok;

	while (std::getline(ss, tok, delimiter))
	{
		result.emplace_back(tok);
	}
	return result;
}

void FileUtils::GetLabelFile(std::string path, std::vector<std::string> &file, std::vector<std::string> ext)
{

#ifdef __APPLE__
	std::string temp;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			temp = ent->d_name;
			if (std::any_of(ext.begin(), ext.end(), [&](const std::string &e)
							{ return temp.find(e) != std::string::npos; }))
			{
				file.emplace_back(path + "/" + temp);
			}
		}
		closedir(dir);
	}
#elif _WIN32
	std::string temp;
	intptr_t hFind;
	_finddata_t data;

	std::string search_path = path + "/*.*";
	hFind = _findfirst(search_path.c_str(), &data);
	if (hFind != -1)
	{
		do
		{
			temp = data.name;
			if (std::any_of(ext.begin(), ext.end(), [&](const std::string &e)
							{ return temp.find(e) != std::string::npos; }))
			{
				file.emplace_back(path + "/" + temp);
			}
		} while (_findnext(hFind, &data) == 0);
		_findclose(hFind);
	}
#endif

	// for (int i = 0; i < file.size(); i++)
	// {
	// 	std::cout << "File " << i + 1 << " is :  ";
	// 	std::cout << file[i] << std::endl;
	// }
	if (file.size() == 0)
	{
		std::cout << R"(ERROR : Labels file is Empty ! Please put the labels image in the < Labels > file !)" << std::endl;
	}
	else
	{
		std::cout << std::endl
				  << "Find " << file.size() << " file(s)." << std::endl;
	}

	for (const auto &path : file)
	{
		labels.emplace_back(Split(Split(path, '/').back(), '.')[0]);
	}
}

Json::Value FileUtils::GetLabelValue(Label label)
{
	Json::Value value;
	value["index"] = label.index;

	Json::Value rect;
	rect[0] = label.label_box.x;
	rect[1] = label.label_box.y;
	rect[2] = label.label_box.width;
	rect[3] = label.label_box.height;

	value["rect"] = rect;
	value["name"] = label.name;

	return value;
}

Json::Value FileUtils::CreateLabelsValue(std::vector<Label> labels)
{
	Json::Value selectLabelsBox(Json::arrayValue);

	for (const auto &label : labels)
	{
		selectLabelsBox.append(GetLabelValue(label));
	}

	return selectLabelsBox;
}

bool FileUtils::WriteJsonFile(std::string root_dir)
{
	Json::Value root;
	root["money_labels_box"] = CreateLabelsValue(jsonData.money_labels);
	root["select_labels_box"] = CreateLabelsValue(jsonData.select_labels);
	root["detect_video"] = jsonData.detect_video;

	Json::StyledWriter writer;
	std::string output = writer.write(root);
	std::ofstream jsonFile(root_dir);
	// std::cout << jsonData.file_path.file_name << std::endl;
	jsonFile << output;
	if (jsonFile.is_open())
	{
		jsonFile.close();
	}
	else
	{
		std::cout << "Json 寫入失敗 !" << std::endl;
		return false;
	}
	return true;
}

std::string FileUtils::MakeDirectory(std::string root_dir)
{
#ifdef __APPLE__
	// use <sys/stat.h> in macOS -> boost not support M1
	struct stat st;
	if (stat(root_dir.c_str(), &st) != 0)
	{
		int status = mkdir(root_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (status == 0)
		{
			std::cout << root_dir << " : ";
			std::cout << "Directory created successfully" << std::endl;
		}
		else
		{
			std::cout << root_dir << " : ";
			std::cout << "Error: Failed to create directory" << std::endl;
		}
	}
	else
	{
		std::cout << "Directory already exists" << std::endl;
	}
#elif _WIN32
	// use boost
	if (!boost::filesystem::exists(root_dir))
	{
		boost::filesystem::create_directory(root_dir);
		std::cout << root_dir << " : ";
		std::cout << "Directory created successfully" << std::endl;
	}
	else
	{
		std::cout << root_dir << " : ";
		std::cout << "Directory already exists" << std::endl;
	}
#endif
	return std::string(root_dir).append("/");
}

std::string FileUtils::MakeSubDirectory(std::string root_dir, std::string sub_dir)
{
#ifdef __APPLE__
	struct stat st;
	int status;
	int i = 1;
	std::string dir_name = root_dir.append(sub_dir);
	std::string new_dir_name;
	while (1)
	{
		new_dir_name = dir_name + std::to_string(i);
		std::cout << new_dir_name << std::endl;
		if (stat(new_dir_name.c_str(), &st) != 0)
		{
			status = mkdir(new_dir_name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			if (status == 0)
			{
				std::cout << "Directory created successfully" << std::endl;
				break;
			}
			else
			{
				std::cout << "Error: Failed to create directory" << std::endl;
				break;
			}
		}
		i++;
	}
	return new_dir_name.append("/");

#elif _WIN32
	int i = 1;
	// std::string dir_name = sub_dir;
	std::string new_dir_name;
	while (true)
	{
		new_dir_name.append(root_dir).append(sub_dir).append(std::to_string(i));
		if (!boost::filesystem::exists(new_dir_name))
		{
			boost::filesystem::create_directory(new_dir_name);
			std::cout << "Directory created successfully" << std::endl;
			break;
		}
		i++;
	}
#endif
}

bool FileUtils::MakeDataFiles(std::string root_dir)
{
	SetRootDirectory(root_dir);
	MakeDirectory(root_dir + PATH_VIDEO);
	MakeDirectory(root_dir + PATH_LABELS);
	MakeDirectory(root_dir + PATH_OUTPUT);
	return WriteJsonFile(root_dir + PATH_JSON);
}

bool FileUtils::ReadJsonFile(std::string json_path)
{
	std::ifstream jsonFile(json_path);
	if (jsonFile.fail())
	{
		std::cout << R"(Reading file failed, please check your "Setting.json" is exist ? )" << std::endl;
		return false;
	}
	Json::Value root;
	jsonFile >> root;

	// Get the money_box
	auto &money_labels_boxes = root["money_labels_box"];
	for (unsigned int i = 0; i < money_labels_boxes.size(); i++)
	{
		auto &money_label_box = money_labels_boxes[i];
		jsonData.money_labels.emplace_back(money_label_box["index"].asInt(), cv::Rect(money_label_box["rect"][0].asInt(), money_label_box["rect"][1].asInt(), money_label_box["rect"][2].asInt(), money_label_box["rect"][3].asInt()), money_label_box["name"].asString());
	}
	// Get the select_labels_box
	auto &select_labels_boxes = root["select_labels_box"];
	for (unsigned int i = 0; i < select_labels_boxes.size(); i++)
	{
		auto &select_label_box = select_labels_boxes[i];
		jsonData.select_labels.emplace_back(select_label_box["index"].asInt(), cv::Rect(select_label_box["rect"][0].asInt(), select_label_box["rect"][1].asInt(), select_label_box["rect"][2].asInt(), select_label_box["rect"][3].asInt()), select_label_box["name"].asString());
	}
	jsonData.detect_video = root["detect_video"].asString();

	// access the data
	std::cout << "Number of Money labels : " << jsonData.money_labels.size() << std::endl;
	std::cout << "Number of select labels : " << jsonData.select_labels.size() << std::endl;

	std::cout << "videos_directory: " << jsonData.detect_video << std::endl;
	return true;
}

void FileUtils::CopyVideoFile(const std::string &video_path)
{
	std::ifstream originalFile(video_path, std::ios::binary);
	auto video_name = Split(video_path, '/');
	std::string destinationPath = (rootDirectory + PATH_VIDEO) + ("/" + video_name.back());
	std::ofstream newFile(destinationPath, std::ios::binary);

	newFile << originalFile.rdbuf();

	originalFile.close();
	newFile.close();
}

void FileUtils::SaveLabels(std::vector<Label> &select_labels, std::vector<Label> &money_labels)
{
	// std::sort(labels.begin(), labels.end(), [](Label a, Label b)
	//	{ return a.index < b.index; });
	// bool findMoneyBox = false;
	jsonData.money_labels = money_labels;
	jsonData.select_labels = select_labels;

	if (money_labels.size() == 0)
	{
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << "====================================================================================================================" << std::endl;
		std::cout << std::endl;
		std::cout << "警告 ! : 未設定 金錢 偵測框，此操作不會檢測金錢，請注意是否正確，案下 M鍵 可以選擇金錢偵測框  ( 如有錯誤請重新標籤 )" << std::endl;
		std::cout << std::endl;
		std::cout << "====================================================================================================================" << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
	}
}

void CvUtils::GetDetectImages(std::string file_path, std::vector<std::string> need_extension, std::vector<cv::Mat> &detectImages)
{
	std::vector<std::string> imagePath;
	std::cout << file_path << std::endl;
	FileUtils::GetLabelFile(file_path, imagePath, need_extension);

	for (auto &path : imagePath)
	{
		// GRAY
		detectImages.emplace_back(cv::imread(path, cv::IMREAD_GRAYSCALE));
		// detectImages.emplace_back(cv::imread(path));
	}
}

void CvUtils::ShowFPS(const cv::VideoCapture &capture, const cv::Mat &image)
{
	std::string fpsText = "FPS : " + std::to_string((capture.get(cv::CAP_PROP_FPS)));
	cv::putText(image, fpsText, cv::Point(image.size().width - 145, image.size().height - 50), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 255, 0), 2);
}

void CvUtils::ShowDetectLabel(const std::string &label_name, cv::Mat &image, cv::Point point)
{
	cv::putText(image, label_name, point, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
}
