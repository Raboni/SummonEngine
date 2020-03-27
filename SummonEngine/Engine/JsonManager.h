#pragma once
#undef min
#undef max
#include "rapidjson.h"
#include "document.h"
#include <HashMap.hpp>
#include <string>

class JsonManager
{
public:
	static void CreateInstance(unsigned int aMaxLoadedAmount);
	static JsonManager* GetInstance();
	static void DestroyInstance();

	bool Load(const std::string& aPath);
	bool Load(const std::string& aPath, const std::string& aAlias);

	rapidjson::Document* Get(const std::string& aAlias);
	
private:
	JsonManager(unsigned int aMaxLoadedAmount);
	~JsonManager();

private:
	static JsonManager* ourInstance;

	CU::HashMap<std::string, int> myDocuments;
	CU::GrowingArray< rapidjson::Document, int> myDocs;
};