#include "stdafx.h"
#include "GeometryShader.h"

ID3D11GeometryShader* GeometryShader::GetShader()
{
	return myShader;
}

GeometryShader::GeometryShader()
{
	myShader = nullptr;
	myUserCount = 0;
}

void GeometryShader::AddUser()
{
	myUserCount++;
}
void GeometryShader::RemoveUser()
{
	myUserCount--;
}
unsigned int GeometryShader::GetUserCount() const
{
	return myUserCount;
}