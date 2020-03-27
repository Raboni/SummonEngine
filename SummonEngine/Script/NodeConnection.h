#pragma once
#include <any>

class NodeInstance;

class NodeConnection
{
public:
	NodeConnection();
	~NodeConnection();

public:
	std::any myData;
	NodeInstance* myOtherNode;
	int myOtherPinIndex;
};