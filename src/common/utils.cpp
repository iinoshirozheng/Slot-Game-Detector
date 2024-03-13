#include "utils.h"
#include <fstream>
#include <io.h>
#include <boost\filesystem.hpp>


std::vector<std::string> FileUtils::labels;
std::vector<std::string> FileUtils::labels_mode;
JsonParameter FileUtils::jsonData;
std::string FileUtils::rootDirectory;
std::string FileUtils::detector_path;
double CvUtils::fps = 0;
double CvUtils::tick = 0;
HWND CvUtils::hwnd;

std::vector<std::string> FileUtils::Split(const std::string& str, const char& delimiter)
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

std::string FileUtils::GetVideoName(std::string videoPath)
{
	if (videoPath.find("\\") != std::string::npos) return Split(videoPath, '\\').back();
	else if (videoPath.find("/") != std::string::npos) return Split(videoPath, '/').back();
	else return videoPath;
}

void FileUtils::GetLabelFile(std::string path, std::vector<std::string>& file, std::vector<std::string> ext)
{

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
			if (std::any_of(ext.begin(), ext.end(), [&](const std::string& e)
				{ return temp.find(e) != std::string::npos; }))
			{
				file.emplace_back(path + "/" + temp);
			}
		} while (_findnext(hFind, &data) == 0);
		_findclose(hFind);
	}

	if (file.size() == 0)
	{
		std::cout << R"(ERROR : Labels file is Empty ! Please put the labels image in the < Labels > file !)" << std::endl;
	}
	else
	{
		std::cout << std::endl
			<< "Find " << file.size() << " file(s)." << std::endl;
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

	for (const auto& label : labels)
	{
		selectLabelsBox.append(GetLabelValue(label));
	}

	return selectLabelsBox;
}

std::string FileUtils::MakeDirectory(std::string root_dir)
{
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
	return std::string(root_dir).append("/");
}

std::string FileUtils::MakeSubDirectory(std::string root_dir, std::string sub_dir)
{
	std::string dir_name = root_dir + sub_dir;
	std::string new_dir_name;
	int i = 1;
	while (1) {
		new_dir_name = dir_name + std::to_string(i);
		std::cout << new_dir_name << std::endl;
		if (!boost::filesystem::exists(new_dir_name)) {
			try {
				boost::filesystem::create_directory(new_dir_name);
				std::cout << "Directory created successfully" << std::endl;
				break;
			}
			catch (const boost::filesystem::filesystem_error& e) {
				std::cout << "Error: Failed to create directory. Reason: " << e.what() << std::endl;
				break;
			}
		}
		i++;
	}
	return new_dir_name + "/";
}

bool FileUtils::MakeDataFiles(std::string root_dir)
{
	SetRootDirectory(root_dir);
	// MakeDirectory(root_dir + PATH_VIDEO);
	MakeDirectory(root_dir + PATH_LABELS);
	MakeDirectory(root_dir + PATH_OUTPUT);
	return WriteJsonFile(root_dir + PATH_JSON);
}

bool FileUtils::FilterString(std::string str)
{
	if (str.size() == 0) return false;

	if (str == "0" || str == "." || str == "") return false;

	return true;
}

bool FileUtils::WriteJsonFile(std::string root_dir)
{
	Json::Value root;
	root["add_label_seq"] = Json::Value(Json::arrayValue);
	root["app_detect_logic"] = jsonData.detect_logic;
	root["app_show_debug_window"] = jsonData.show_debug_window;
	root["text_labels_box"] = CreateLabelsValue(jsonData.text_labels);
	root["select_labels_box"] = CreateLabelsValue(jsonData.select_labels);
	root["mode_labels_box"] = CreateLabelsValue(jsonData.mode_labels);
	root["continue_detect_frame"] = jsonData.continue_detect_frame;
	root["continue_number_frame"] = jsonData.continue_number_frame;
	root["continue_mode_frame"] = jsonData.continue_mode_frame;
	root["add_window_name"] = jsonData.window_name;
	root["delay_frame"] = jsonData.delay_frame;
	root["detector_threshold"] = jsonData.detector_threshold;
	


	// Add comments in front of each object
	// root.setComment(std::string("// Detector Setteings"), Json::CommentPlacement::commentBefore);
	root["add_label_seq"].setComment(std::string("// Label sequence (需要自行輸入調整).// 3x1 機台: \"label_seq\" : [1,1,1],// 3x3 機台: \"label_seq\" : [3,3,3],// 3x3 機台: \"label_seq\" : [3,3,3,3],// 3x5 機台: \"label_seq\" : [3,3,3,3,3]"), Json::CommentPlacement::commentBefore);
	// root["add_save_addition"].setComment(std::string("// Save the addition detected boxes (輪帶停止時，鄰近 Lable 的數量).// 假如設定為 1 每條輪代表會多一個判斷，而設定 2 每條輪代表則會多兩個判斷，以此類推"), Json::CommentPlacement::commentBefore);
	root["add_window_name"].setComment(std::string("// Detect Window's name (輸入要偵測程式的視窗名稱，一定要完全正確才行)"), Json::CommentPlacement::commentBefore);
	root["app_show_debug_window"].setComment(std::string("// 設定是否顯示辨識視窗"), Json::CommentPlacement::commentBefore);
	
	root["app_detect_logic"].setComment(std::string("// 使用的 Label 判斷邏輯 預設 0 必須設定"), Json::CommentPlacement::commentBefore);
	root["text_labels_box"].setComment(std::string("// Text and numbers labels "), Json::CommentPlacement::commentBefore);
	root["select_labels_box"].setComment(std::string("// Select labels "), Json::CommentPlacement::commentBefore);
	root["mode_labels_box"].setComment(std::string("// Mode labels "), Json::CommentPlacement::commentBefore);
	root["continue_detect_frame"].setComment(std::string("// Continue detect frame "), Json::CommentPlacement::commentBefore);
	root["continue_number_frame"].setComment(std::string("// Continue number frame "), Json::CommentPlacement::commentBefore);
	root["continue_mode_frame"].setComment(std::string("// Continue mode frame "), Json::CommentPlacement::commentBefore);
	root["delay_frame"].setComment(std::string("// Detector delay frame "), Json::CommentPlacement::commentBefore);
	root["detector_threshold"].setComment(std::string("// Detector threshold "), Json::CommentPlacement::commentBefore);



	Json::StreamWriterBuilder builder;
	builder["indentation"] = "    "; // set indentation to tab
	builder.settings_["precision"] = 1; // Set precision to 1 decimal place
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

	std::stringstream output;
	writer->write(root, &output);


	//Json::StyledWriter writer;
	std::string outputStr = output.str();
	size_t pos = 0;
	while ((pos = outputStr.find("//", pos)) != std::string::npos) 
	{
		outputStr.replace(pos, 2, "\n    //");
		pos += 7;
	}
	outputStr = "// Detector Setteings\n" + outputStr;


	std::ofstream jsonFile(root_dir);
	// std::cout << jsonData.file_path.file_name << std::endl;
	jsonFile << outputStr;
	if (!jsonFile.is_open())
	{
		std::cout << "Json write failed !" << std::endl;
		return false;
	}

	jsonFile.close();
	return true;
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
	auto& text_labels_boxes = root["text_labels_box"];
	for (unsigned int i = 0; i < text_labels_boxes.size(); i++)
	{
		auto& text_label_box = text_labels_boxes[i];
		jsonData.text_labels.emplace_back(text_label_box["index"].asInt(), cv::Rect(text_label_box["rect"][0].asInt(), text_label_box["rect"][1].asInt(), text_label_box["rect"][2].asInt(), text_label_box["rect"][3].asInt()), text_label_box["name"].asString());
	}
	// Get the select_labels_box
	auto& select_labels_boxes = root["select_labels_box"];
	for (unsigned int i = 0; i < select_labels_boxes.size(); i++)
	{
		auto& select_label_box = select_labels_boxes[i];
		jsonData.select_labels.emplace_back(select_label_box["index"].asInt(), cv::Rect(select_label_box["rect"][0].asInt(), select_label_box["rect"][1].asInt(), select_label_box["rect"][2].asInt(), select_label_box["rect"][3].asInt()), select_label_box["name"].asString());
	}
	// Get the mode_labels_box
	auto& mode_labels_boxes = root["mode_labels_box"];
	for (unsigned int i = 0; i < mode_labels_boxes.size(); i++)
	{
		auto& mode_label_boxes = mode_labels_boxes[i];
		jsonData.mode_labels.emplace_back(mode_label_boxes["index"].asInt(), cv::Rect(mode_label_boxes["rect"][0].asInt(), mode_label_boxes["rect"][1].asInt(), mode_label_boxes["rect"][2].asInt(), mode_label_boxes["rect"][3].asInt()), mode_label_boxes["name"].asString());
	}

	// Get the label_seq
	auto& label_seq_json = root["add_label_seq"];
	if (label_seq_json.size()) {
		for (int i = 0; i < label_seq_json.size(); i++) {
			std::vector<int> row;
			for (int j = 0; j < label_seq_json[i].asInt(); j++) {
				row.emplace_back(0);
			}
			jsonData.label_seq.emplace_back(row);
		}

		std::cout << "label sequence :" << std::endl;
		for (int i = 0; i < jsonData.label_seq.size(); i++) {
			for (int j = 0; j < jsonData.label_seq[i].size(); j++) {
				std::cout << jsonData.label_seq[i][j] << " ";
			}
			std::cout << std::endl;
		}
	}
	else 
	{
		std::cout << "the data label_seq is empty, please add the label_seq element to setting.json file !" << std::endl;
		return false;
	}

	jsonData.window_name = root["add_window_name"].asString();
	jsonData.continue_detect_frame = root["continue_detect_frame"] ? root["continue_detect_frame"].asInt() : DETECTOR_SAVE_FRAME;
	jsonData.detect_logic = root["app_detect_logic"] ? root["app_detect_logic"].asInt() : 0;
	jsonData.continue_mode_frame = root["continue_mode_frame"] ? root["continue_mode_frame"].asInt() : DETECTOR_MODE_FRAME;
	jsonData.continue_number_frame = root["continue_number_frame"] ? root["continue_number_frame"].asInt() : DETECTOR_SAME_NUMBER_FRAME;
	// jsonData.save_addition = root["add_save_addition"] ? root["add_save_addition"].asInt() : SAVE_ADDITION;
	jsonData.delay_frame = root["delay_frame"] ? root["delay_frame"].asInt() : DETECTOR_DELAY;
	jsonData.detector_threshold = root["detector_threshold"] ? root["detector_threshold"].asDouble() : ORB_THRESHOLD;
	jsonData.show_debug_window = root["app_show_debug_window"] ? root["app_show_debug_window"].asBool() : true;



	// access the data
	std::cout << std::endl;
	std::cout << "Show Debug Window : " << (jsonData.show_debug_window ? "true" : "false") << std::endl;
	std::cout << "Detect Logic : " << jsonData.detect_logic << std::endl;
	std::cout << "Number of text labels : " << jsonData.text_labels.size() << std::endl;
	std::cout << "Number of select labels : " << jsonData.select_labels.size() << std::endl;
	std::cout << "add_window_name: " << jsonData.window_name << std::endl;
	std::cout << "continue_detect_frame: " << jsonData.continue_detect_frame << std::endl;
	std::cout << "continue_number_frame: " << jsonData.continue_number_frame << std::endl;
	std::cout << "continue_mode_frame: " << jsonData.continue_mode_frame << std::endl;
	// std::cout << "add_save_addition: " << jsonData.save_addition << std::endl;
	std::cout << "delay_frame: " << jsonData.delay_frame << std::endl;
	std::cout << "detector_threshold: " << jsonData.detector_threshold << std::endl ;
	std::cout << std::endl;
	return true;
}

void FileUtils::SaveLabels(std::vector<Label>& select_labels, std::vector<Label>& text_labels, std::vector<Label>& mode_labels)
{
	// std::sort(labels.begin(), labels.end(), [](Label a, Label b)
	//	{ return a.index < b.index; });
	// bool findMoneyBox = false;
	jsonData.text_labels = text_labels;
	jsonData.select_labels = select_labels;
	jsonData.mode_labels = mode_labels;
	jsonData.window_name = FileUtils::jsonData.window_name;

	if (text_labels.size() != 2)
	{
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << "====================================================================================================================" << std::endl;
		std::cout << std::endl;
		std::cout << "Warning! Money or Round detection box not set. This operation will not detect money.\nPlease verify that this is correct. Press the 'M' key to select the money detection box. (If there is an error, please relabel.)" << std::endl;
		std::cout << std::endl;
		std::cout << "====================================================================================================================" << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
	}
}

void CvUtils::GetDetectImages(std::string file_path, std::vector<cv::Mat>& detectImages, std::vector<cv::Mat>& specialModeImages)
{
	const std::vector<std::string> need_extension = { ".jpg", ".png", ".jepg", ".JPG", ".PNG", ".JEPG" };

	std::vector<std::string> imagePath;
	std::cout << file_path << std::endl;
	FileUtils::GetLabelFile(file_path, imagePath, need_extension);
	FileUtils::labels_mode.clear();
	FileUtils::labels.clear();

	for (auto& path : imagePath)
	{
		std::string name = FileUtils::Split(path, '/').back();

		if (!name.find(MODE_TYPE)) // find mode type
		{
			FileUtils::labels_mode.emplace_back(FileUtils::Split(FileUtils::Split(FileUtils::Split(path, '/').back(), '.').front(), '_').back());
			specialModeImages.emplace_back(cv::imread(path, cv::IMREAD_GRAYSCALE));
			// cv::imshow("test"+labels_mode.back(), specialModeImages.back());
		}
		else
		{
			FileUtils::labels.emplace_back(FileUtils::Split(FileUtils::Split(FileUtils::Split(path, '/').back(), '.').front(), '_').back());
			detectImages.emplace_back(cv::imread(path, cv::IMREAD_GRAYSCALE));
		}
	}
}

void CvUtils::ShowFPS(const cv::Mat& image)
{
	std::ostringstream fps;
	fps << std::fixed << std::setprecision(2) << GetFPS();
	cv::putText(image, "FPS : " + fps.str(), cv::Point(image.size().width - 200, image.size().height - 50), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 255, 0), 2);
}


void CvUtils::ShowDetectLabel(const std::string& label_name, cv::Mat& image, cv::Point point)
{
	cv::putText(image, label_name, point, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
}

void CvUtils::CaptureScreenshot(cv::Mat& screenshot)
{
	RECT rect;
	// Get the size of the window
	GetWindowRect(CvUtils::hwnd, &rect);

	// Create a device context for the window
	HDC hdcWindow = GetDC(CvUtils::hwnd);

	// Create a memory device context for the screenshot
	HDC hdcMemDC = CreateCompatibleDC(hdcWindow);

	// Create a bitmap for the screenshot
	HBITMAP hbmScreen = CreateCompatibleBitmap(hdcWindow, rect.right - rect.left, rect.bottom - rect.top);

	// Select the bitmap into the memory device context
	SelectObject(hdcMemDC, hbmScreen);

	// Capture the window as an image
	PrintWindow(CvUtils::hwnd, hdcMemDC, PW_RENDERFULLCONTENT);

	// Convert the screenshot to a Mat object
	screenshot.create(rect.bottom - rect.top, rect.right - rect.left, CV_8UC4);
	GetBitmapBits(hbmScreen, screenshot.total() * screenshot.elemSize(), screenshot.data);

	// Release the resources
	DeleteObject(hbmScreen);
	DeleteDC(hdcMemDC);
	ReleaseDC(CvUtils::hwnd, hdcWindow);
}

bool CvUtils::SetWindowHandle(const char* windowName)
{
	CvUtils::hwnd = FindWindowA(NULL, windowName);
	return CvUtils::hwnd != nullptr ;
}
