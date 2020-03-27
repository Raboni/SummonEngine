#include "stdafx.h"
#include "PixelShader.h"

ID3D11PixelShader* PixelShader::GetShader()
{
	return myShader;
}

PixelShader::PixelShader()
{
	myShader = nullptr;
	myUserCount = 0;
}

void PixelShader::AddUser()
{
	myUserCount++;
}
void PixelShader::RemoveUser()
{
	myUserCount--;
}
unsigned int PixelShader::GetUserCount() const
{
	return myUserCount;
}