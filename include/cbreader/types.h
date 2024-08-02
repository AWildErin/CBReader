#pragma once

#include <cstdint>

namespace cbtypes
{
	struct Vector3
	{
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;
	};

	struct Vector2
	{
		float x = 0.f;
		float y = 0.f;
	};

	struct Triangle
	{
		std::uint32_t index1 = 0;
		std::uint32_t index2 = 0;
		std::uint32_t index3 = 0;
	};
}
