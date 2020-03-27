#pragma once
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/rapidjson.h"
#include <string>
#include <fstream>

namespace CU
{
	typedef rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> JsonData;
	typedef rapidjson::GenericDocument<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> JsonDocument;

	class JsonReader
	{
	public:
		JsonReader();
		JsonReader(std::string aFilePath);
		~JsonReader() = default;

		void Read(std::string aFilePath);

		JsonDocument& GetDoc();
		std::string GetFilePath();

		//std::string GetString(const std::string& aKey);
		/*JsonData& operator[](const std::string& aKey);
		const JsonData& operator[](const std::string& aKey) const;
		JsonData& operator[](const char* aKey);
		const JsonData& operator[](const char* aKey) const;*/

	private:
		std::string myFilePath;
		rapidjson::Document myDocument;
	};
}