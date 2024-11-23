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

	struct Color
	{
		float r = 255.f;
		float g = 255.f;
		float b = 255.f;
		float a = 255.f;
	};

	struct Quaternion
	{
		float x = 1.;
		float y = 1.;
		float z = 1.;
		float w = 1.;
	};
}

