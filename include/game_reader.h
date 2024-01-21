#pragma once

#include "dmntcht.h"
#include <functional>

class GameReader {
 public:
  Result RefreshMetadata();
  Result GetTitleId(u64 *titleId);
  Result ReadHeap(u64 offset, void *buffer, size_t size);

 private:
  bool m_hasMetadata = false;
  DmntCheatProcessMetadata m_metadata;
  Result doWithDmntchtSession(std::function<Result()> func);
};
