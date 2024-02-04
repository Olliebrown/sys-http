#pragma once

#include <switch/result.h>

extern "C" {
#include <mkjson.h>
}

#define R_SUCCESS  0

#define module_generic  0
#define module_syshttp  342

/// Generic error codes
enum {
  generic_unknown,
  generic_allocation,
  generic_fileio
};

/// sys-http error codes
enum {
  syshttp_unknown,
  syshttp_noTitleInfo,
  syshttp_badTitleInfo,
  syshttp_dmntcht_error,
  syshttp_badMetadata
};

#define MAKE_ERROR_FROM_RC(rc) \
  mkjson(MKJSON_OBJ, 4, \
		MKJSON_BOOL, "error",       true, \
		MKJSON_INT,  "code",        rc, \
    MKJSON_INT,  "module",      R_MODULE(rc), \
    MKJSON_INT,  "description", R_DESCRIPTION(rc) \
	)

#define MAKE_JSON_ERROR_RESPONSE(res, rc) \
  res.set_content(MAKE_ERROR_FROM_RC(rc), "application/json");

#define SET_STATUS_FROM_RC(res, rc)     \
  if (R_FAILED(rc)) {                   \
    res.status = 500;                   \
    MAKE_JSON_ERROR_RESPONSE(res, rc);  \
  } else {                              \
    res.status = 200;                   \
  }

#define RETURN_IF_FAIL(rc) \
  if (R_FAILED(rc))        \
    return rc;
