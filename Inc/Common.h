#pragma once

#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#define V_MAJOR 0
#define V_MINOR 0
#define V_REV 0

#define LIB_VERSION V_MAJOR.V_MINOR.V_REV
const string LIB_NAME = "TexIO";

namespace TexIO
{
	using byte = unsigned char;
	using uint = uint32_t;
	using ushort = uint16_t;
}