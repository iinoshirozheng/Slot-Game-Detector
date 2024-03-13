#pragma once
#include <fstream>
#include "../Data/detect_parameters.h"

class Application
{
private:
	int		Argc;
	char**	Argv;
	std::ofstream	CSV;
	DetectParameters Parameters;
public:
	Application(int argc, char* argv[]);
	~Application();
	void Run();
	void RecordAndDetectScreen();

private:
	bool Init();
	bool SetRootDirPath();
	bool SetJsonParam();
	bool SetWindowHandle();
	bool EarlyStop();

	void UpdatePrevStatus();
	void CreateCSV();
	void SaveRound();
	class DetectLogic* DetectLogicPtr = nullptr;
};

