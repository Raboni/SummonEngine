#include "NodePin.h"

NodePin::NodePin()
{
	myDataType = PinDataType::None;
}
NodePin::NodePin(const std::string& aName, const PinDataType& aDataType)
{
	myName = aName;
	myDataType = aDataType;
}
NodePin::~NodePin()
{
}
