#pragma once
#include <string>

enum class PinDataType
{
	None,
	Run,
	Call,
	Int,
	Float,
	String,
	Bool
};

class NodePin
{
public:
	NodePin();
	NodePin(const std::string& aName, const PinDataType& aDataType);
	~NodePin();

public:
	std::string myName;
	PinDataType myDataType;
};