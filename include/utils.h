#pragma once

#include <string>
#include <switch.h>

std::string convertByteArrayToHex(u8 *bytes, size_t size);
std::string convertNumToHexString(u64 num, int width = 16, bool withPrefix = false);
