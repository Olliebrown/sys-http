#include "game_reader.h"

#include "res_macros.h"
#include <switch.h>
#include <vector>

Result GameReader::RefreshMetadata() {
  m_hasMetadata = false;
  return doWithDmntchtSession([]() { return 0; });
}

Result GameReader::GetTitleId(u64 *titleId) {
  RETURN_IF_FAIL(RefreshMetadata());
  *titleId = m_metadata.title_id;
  return 0;
}

Result GameReader::ReadHeap(u64 offset, void *buffer, size_t size) {
  return doWithDmntchtSession([this, offset, buffer, size]() {
    return dmntchtReadCheatProcessMemory(m_metadata.heap_extents.base + offset, buffer, size);
  });
}

Result GameReader::doWithDmntchtSession(std::function<Result()> func) {
  RETURN_IF_FAIL(dmntchtInitialize());

  if (!m_hasMetadata) {
    RETURN_IF_FAIL(dmntchtGetCheatProcessMetadata(&m_metadata));
    m_hasMetadata = true;
  }

  auto rc = func();
  dmntchtExit();
  return rc;
}
