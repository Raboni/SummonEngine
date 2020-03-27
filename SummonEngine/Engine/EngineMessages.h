#pragma once
#include <PostMaster.hpp>

enum class EngineMessages
{
	ResolutionChanged,
	Count
};
typedef CU::PostMaster<EngineMessages> EnginePostMaster;