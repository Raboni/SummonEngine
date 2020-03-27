#include "stdafx.h"
#include "JsonManager.h"
#include "istreamwrapper.h"
#include <fstream>

JsonManager* JsonManager::ourInstance = nullptr;

JsonManager::JsonManager(unsigned int aMaxLoadedAmount) : myDocuments(aMaxLoadedAmount)
{
}
JsonManager::~JsonManager()
{
}

void JsonManager::CreateInstance(unsigned int aMaxLoadedAmount)
{
	if (ourInstance == nullptr)
	{
		ourInstance = new JsonManager(aMaxLoadedAmount);
	}
}
JsonManager* JsonManager::GetInstance()
{
	return ourInstance;
}
void JsonManager::DestroyInstance()
{
	if (ourInstance != nullptr)
	{
		delete ourInstance;
		ourInstance = nullptr;
	}
}

bool JsonManager::Load(const std::string& aPath)
{
	return Load(aPath, aPath);
}
bool JsonManager::Load(const std::string& aPath, const std::string& aAlias)
{
	std::ifstream myInputStream(aPath);
	rapidjson::IStreamWrapper myWrapper(myInputStream);
	//rapidjson::Document* doc = nullptr;

	myDocuments.Create(aAlias);
	int* index = myDocuments.Get(aAlias);
	if (index == nullptr)
	{
		return false;
	}
	*index = myDocs.Size();
	//myDocs.Add();
	/*doc = myDocuments.Get(aAlias);
	if (doc == nullptr)
	{
		return false;
	}*/

	myDocs[*index].ParseStream(myWrapper);
	//assert(!doc->HasParseError());
	return true;
}

rapidjson::Document* JsonManager::Get(const std::string& aAlias)
{
	int* index = myDocuments.Get(aAlias);
	if (index == nullptr)
	{
		return nullptr;
	}
	return &myDocs[*index];
}
