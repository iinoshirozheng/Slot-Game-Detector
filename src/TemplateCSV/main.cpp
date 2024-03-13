// TemplateCSV.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>



struct Data
{
    std::string label;
    int index;

    Data(std::string label, int index)
    {
        this->label = label;
        this->index = index;
    }
};


std::vector<std::string> Split(std::string line, char delimiter, bool skip = true);
std::vector<std::vector<Data>> GetTemplateData(std::string fileName);
bool CheckTemplateSame(std::vector<std::vector<Data>>& templateData, std::vector<std::string> header);
// std::vector<std::string> indexLabel;
void WriteData(std::ofstream& file, std::vector<std::vector<Data>> datas, int enter = 1);
void UpdateData(std::vector<std::vector<Data>>& templateData, std::vector<std::string> line);


int main()
{
    std::string templateFileName = "Template.csv";
    std::string outputFileName;
    std::cout << "Input the csv file you want to convert : ";
    std::cin >> outputFileName;


    std::string outputName = "Convert-";

    // read csv file
    std::vector<std::vector<Data>> templateData = GetTemplateData(templateFileName);
    if (!templateData.size())
    {
        std::cout << "error code -1，please check the Template data is current" << std::endl;
        system("pause");
        return 1;
    }

    // check Template is same with header
    std::ifstream file(outputFileName);

    if (!file.is_open()) {
        std::cout << "Failed to open file" << outputFileName << std::endl;
        system("pause");
        return 1;
    }
    std::string line;
    if (std::getline(file, line) && line != "" && CheckTemplateSame(templateData, Split(line, ',')))
    {
        // indexLabel = Split(line, ',');
        std::ofstream templateFile(outputName + Split(outputFileName, '\\', false).back());

        // check if the file was successfully opened
        if (templateFile.is_open())
        {
            WriteData(templateFile, templateData);
            while (std::getline(file, line))
            {
                UpdateData(templateData, Split(line, ',', false));
                WriteData(templateFile, templateData);
            }
            templateFile.close();
        }
    }
    else
    {
        std::cout << "Check template is not same, please check template is same!" << std::endl;
        file.close();
        system("pause");
        return 2;
    }
    file.close();
    std::cout << "finished!" << std::endl;
    system("pause");
    return 0;
}


void UpdateData(std::vector<std::vector<Data>>& templateData, std::vector<std::string> line)
{
    for (auto& data : templateData)
    {
        for (auto& element : data)
        {
            element.label = line[element.index];
        }
    }
}

void WriteData(std::ofstream& file, std::vector<std::vector<Data>> datas, int enter)
{
    for (auto& data : datas)
    {
        for (int i = 0; i < data.size(); i++)
        {
            if (i) file << ",";
            file << data[i].label;
        }
        file << std::endl;
    }
    for (int i = 0; i < enter; i++)
    {
        file << std::endl;
    }

}

bool CheckTemplateSame(std::vector<std::vector<Data>>& templateData, std::vector<std::string> header)
{
    int size = 0;
    for (auto& data : templateData)
    {
        for (auto& element : data)
        {
            bool findLabel = false;
            for (int i = 0; i < header.size(); i++)
            {
                if (element.label == header[i])
                {
                    element.index = i;
                    findLabel = true;
                }
            }
            size++;
            if (!findLabel) return false;
        }
    }
    if (size != header.size()) return false;
    return true;
}

std::vector<std::vector<Data>> GetTemplateData(std::string fileName)
{
    std::vector<std::vector<Data>> templateCSV;
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cout << "Failed to open file" << fileName << std::endl;
        templateCSV.clear();
        return templateCSV;
    }
    std::string line;
    std::vector<Data> lineData;

    while (std::getline(file, line)) {
        // process each line of the file here
        // std::cout << split(line, ',').size() << std::endl;
        for (auto& label : Split(line, ','))
        {
            lineData.emplace_back(Data(label, -1));
        }
        templateCSV.emplace_back(lineData);
        lineData.clear();
    }
    file.close();

    return templateCSV;
}


std::vector<std::string> Split(std::string line, char delimiter, bool skip)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(line);

    while (std::getline(tokenStream, token, delimiter)) {
        if (skip)
        {
            if (!token.empty()) tokens.push_back(token);
        }
        else
        {
            tokens.push_back(token);
        }

    }
    return tokens;
}