#include "stdafx.h"
#include "Text.h"

Text::Text()
{
	myUserCount = 0;
}
Text::~Text()
{
	myText.clear();
	myFont.clear();
}

void Text::AddUser()
{
	myUserCount++;
}
void Text::RemoveUser()
{
	myUserCount--;
}
const unsigned int Text::GetUserCount() const
{
	return myUserCount;
}
