#include "stdafx.h"
#include "VertexShader.h"

ID3D11VertexShader* VertexShader::GetShader()
{
	return myShader;
}
ID3D11InputLayout* VertexShader::GetLayout()
{
	return myLayout;
}

VertexShader::VertexShader()
{
	myShader = nullptr;
	myLayout = nullptr;
	myUserCount = 0;
}

void VertexShader::AddUser()
{
	myUserCount++;
}
void VertexShader::RemoveUser()
{
	myUserCount--;
}
unsigned int VertexShader::GetUserCount() const
{
	return myUserCount;
}