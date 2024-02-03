#include "utils.h"

#include <iomanip>
#include <vector>

std::string convertByteArrayToHex(u8 *bytes, size_t size) {
  std::stringstream stream;

  stream << std::setfill('0') << std::hex;

  for (u64 i = 0; i < size; i++) {
    stream << std::setw(2) << static_cast<int>(bytes[i]);
  }

  return stream.str();
}

std::string convertNumToHexString(u64 num, int width, bool withPrefix) {
  std::stringstream hex;
  if (withPrefix) { hex << "0x"; }
  hex << std::setfill('0') << std::setw(width) << std::hex << num;
  return hex.str();
}
