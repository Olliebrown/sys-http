#include "game_reader.h"

#include <switch.h>
#include <vector>
#include <cstdlib>
#include <cstring>

#include "title.h"
#include "utils.h"
#include "res_macros.h"

// Turn extents struct into JSON
#define MAKE_EXTENTS_JSON(name, extents) \
  MKJSON_JSON_FREE, (name),  mkjson(MKJSON_OBJ, 2, \
    MKJSON_STRING, "base", convertNumToHexString((extents).base, 10, true).c_str(),  \
    MKJSON_STRING, "size", convertNumToHexString((extents).size, 10, true).c_str()  \
  )

#define MAKE_BOUNDS_JSON(name, extents) \
  MKJSON_JSON_FREE, (name),  mkjson(MKJSON_OBJ, 2, \
    MKJSON_STRING, "start", convertNumToHexString((extents).base, 10, true).c_str(),  \
    MKJSON_STRING, "end", convertNumToHexString((extents).base + (extents).size, 10, true).c_str()  \
  )

GameReader::GameReader() {
  m_debugger = nullptr;
  m_runningGameTitle = nullptr;
}

GameReader::~GameReader() {
  if (m_debugger != nullptr) {
    delete m_debugger;
  }

  if (m_runningGameTitle != nullptr) {
    delete m_runningGameTitle;
  }

  dmntchtExit();
}

Result GameReader::RefreshMetadata(bool forceRefresh) {
  if (forceRefresh || !m_hasMetadata) {
    m_hasMetadata = false;
    return doWithDmntchtSession([]() { return 0; });
  }
  return R_SUCCESS;
}

Result GameReader::GetTitleId(u64 *titleId) {
  RETURN_IF_FAIL(RefreshMetadata());
  *titleId = m_metadata.title_id;
  return R_SUCCESS;
}

Result GameReader::GetTitleInfo(std::string &titleName, std::string &titleAuthor, std::string &titleVersion, u8 &errorCode) {
  RETURN_IF_FAIL(RefreshMetadata());
  if (m_runningGameTitle == nullptr) {
    return MAKERESULT(module_syshttp, syshttp_badtitleinfo);
  }

  titleName = m_runningGameTitle->getTitleName();
  titleAuthor = m_runningGameTitle->getTitleAuthor();
  titleVersion = m_runningGameTitle->getTitleVersion();
  errorCode = m_runningGameTitle->getErrorCode();

  return m_runningGameTitle->getErrorResult();
}

Result GameReader::GetAllMetadata(char* &metadataJson) {
  return doWithDmntchtSession([this, &metadataJson]() {
    if (!m_hasMetadata) {
      return MAKERESULT(module_syshttp, syshttp_badmetadata);
    }

    metadataJson = mkjson(MKJSON_OBJ, 9, \
      MKJSON_INT, "process_id", m_metadata.process_id, \
      MKJSON_STRING, "title_id", convertNumToHexString(m_metadata.title_id).c_str(), \
      MKJSON_STRING, "main_nso_build_id", convertByteArrayToHex(m_metadata.main_nso_build_id, 0x20).c_str(),
      MAKE_BOUNDS_JSON("main", this->m_metadata.main_nso_extents),  \
      MAKE_BOUNDS_JSON("heap", this->m_metadata.alias_extents), \
      MAKE_EXTENTS_JSON("main_nso_extents", this->m_metadata.main_nso_extents),  \
      MAKE_EXTENTS_JSON("alias_extents", this->m_metadata.alias_extents), \
      MAKE_EXTENTS_JSON("heap_extents", this->m_metadata.heap_extents),  \
      MAKE_EXTENTS_JSON("address_space_extents", this->m_metadata.address_space_extents)  \
    );
    return R_SUCCESS;
  });
}

Result GameReader::GetIcon(u8* &iconData, u64 &dataSize) {
  RETURN_IF_FAIL(RefreshMetadata());
  if (m_runningGameTitle == nullptr) {
    return MAKERESULT(module_syshttp, syshttp_badtitleinfo);
  }

  iconData = m_runningGameTitle->getTitleIcon();
  dataSize = m_runningGameTitle->getTitleIconSize();
  return m_runningGameTitle->getErrorResult();
}

Result GameReader::ReadHeap(u64 offset, void *buffer, size_t size) {
  return doWithDmntchtSession([this, offset, buffer, size]() {
    return dmntchtReadCheatProcessMemory(m_metadata.heap_extents.base + offset, buffer, size);
  });
}

Result GameReader::doWithDmntchtSession(std::function<Result()> func) {
  if (m_debugger == nullptr) {
    m_debugger = new Debugger();

    if (dmntPresent()) {
      dmntchtInitialize();
      if (!m_debugger->m_dmnt || !dmntchtForceOpenCheatProcess()) {
        m_debugger->attachToProcess();
      }
    } else {
      m_debugger->attachToProcess();
    }

    if (!m_debugger->m_dmnt) {
      m_sysmodulePresent = true;
    }
  }

  if (!m_hasMetadata) {
    RETURN_IF_FAIL(dmntchtGetCheatProcessMetadata(&m_metadata));
    m_hasMetadata = true;
  }

  if (m_runningGameTitle == nullptr) {
    m_runningGameTitle = new Title(m_debugger->getRunningApplicationTID());
  }

  auto rc = func();
  return rc;
}

bool GameReader::dmntPresent() {
  /* Get all process ids */
  u64 process_ids[0x50];
  s32 num_process_ids;
  svcGetProcessList(&num_process_ids, process_ids, sizeof process_ids);  // need to double check

  /* Look for dmnt or dmntgen2 titleID */
  u64 titeID;
  for (s32 i = 0; i < num_process_ids; ++i) {
    if (R_SUCCEEDED(pminfoGetProgramId(&titeID, process_ids[i]))) {
      if (titeID == 0x010000000000000D) {
          return true;
      }
    }
  }
  
  return false;
}
