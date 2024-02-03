#include "title.h"
#include "res_macros.h"

#include <memory>
#include <cstring>

Title::Title(FsSaveDataInfo& saveInfo) {
  initFromApplicationId(saveInfo.application_id);
}

Title::Title(u64 applicationId) {
  initFromApplicationId(applicationId);
}

Title::~Title() {
  free(m_titleIcon);
}

Result Title::initFromApplicationId(u64 applicationId) {
  Result rc = 0;

  // Allocate local buffer data
  std::unique_ptr<NsApplicationControlData> buf = std::make_unique<NsApplicationControlData>();
  if (buf == nullptr) {
    m_errorCode = 1;
    m_errorResult = MAKERESULT(module_generic, generic_allocation);
    return m_errorResult;
  }
  memset(buf.get(), 0, sizeof(NsApplicationControlData));

  // Initialize the ns service
  // rc = nsInitialize();
  // if (R_FAILED(rc)) {
  //   m_errorCode = 2;
  //   m_errorResult = rc;
  //   return m_errorResult;
  // }

  // Populate buffer with data
  size_t outsize = 0;
  rc = nsGetApplicationControlData(NsApplicationControlSource_StorageOnly, applicationId, buf.get(), sizeof(NsApplicationControlData), &outsize);
  if (R_FAILED(rc)) {
    m_errorCode = 3;
    m_errorResult = rc;
    return m_errorResult;
  }

  // Sanity check for returned size value
  if (outsize < sizeof(buf->nacp)) {
    m_errorCode = 4;
    m_errorResult = MAKERESULT(module_generic, generic_allocation);
    return m_errorResult;
  }

  // Retrieve the language specific entry for this title
  NacpLanguageEntry *langentry = nullptr;
  rc = nacpGetLanguageEntry(&buf->nacp, &langentry);
  if (R_FAILED(rc) || langentry==nullptr) {
    m_errorCode = 5;
    m_errorResult = rc || MAKERESULT(module_generic, generic_allocation);
    return m_errorResult;
  }

  // Copy all essential data
  m_titleName = std::string(langentry->name);
  m_titleAuthor = std::string(langentry->author);
  m_titleVersion = std::string(buf->nacp.display_version);
  m_titleID = applicationId;

  // Allocate and copy icon
  m_titleIconSize = outsize - sizeof(buf->nacp);
  m_titleIcon = (u8*)malloc(m_titleIconSize);
  memcpy(m_titleIcon, buf->icon, m_titleIconSize);

  return R_SUCCESS;
}

u8 Title::getErrorCode() const {
  return m_errorCode;
}

Result Title::getErrorResult() const {
  return m_errorResult;
}

const std::string& Title::getTitleName() const {
  return m_titleName;
}

const std::string& Title::getTitleAuthor() const {
  return m_titleAuthor;
}

const std::string& Title::getTitleVersion() const {
  return m_titleVersion;
}

u8* Title::getTitleIcon() const {
  return m_titleIcon;
}

u64 Title::getTitleIconSize() const {
  return m_titleIconSize;
}

u64 Title::getTitleID() const {
  return m_titleID;
}
