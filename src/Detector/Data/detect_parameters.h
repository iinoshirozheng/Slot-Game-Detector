#pragma once
#include "utils.h"
#include "detect_list.h"
#include "save_data.h"

class DetectParameters
{
public:
	inline DetectParameters() = default;
	inline DetectParameters(const JsonParameter& paramater)
	{
		this->JsonParam = paramater;
		this->DetectLists = DetectList(paramater);
		this->SaveDatas = SaveData(paramater.label_seq);
		this->Status = DetectStatus();
	}
	inline ~DetectParameters() = default;

	JsonParameter	JsonParam;
	DetectList		DetectLists;
	SaveData 		SaveDatas;
	DetectStatus	Status;
};

