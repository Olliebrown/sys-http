#include "routes.h"

#include "game_reader.h"
#include "utils.h"
#include <memory>

#include "httplib.h"
#include "res_macros.h"

using namespace httplib;

u64 CHUNK_SIZE = 1024;

namespace routes {
  void root(const Request &req, Response &res, std::shared_ptr<GameReader> game) {
    res.set_content("Is this updating?", "text/plain");
  }

  void refreshMetadata(const Request &req, Response &res, std::shared_ptr<GameReader> game) {
    SET_STATUS_FROM_RC(res, game->RefreshMetadata());
    if (res.status == 200) {
      res.set_content("OK", "text/plain");
    }
  }

  void titleId(const Request &req, Response &res, std::shared_ptr<GameReader> game) {
    u64 titleId = 0;
    SET_STATUS_FROM_RC(res, game->GetTitleId(&titleId));
    if (res.status != 200) { return; }
    auto titleIdHex = convertNumToHexString(titleId);
    res.set_content(titleIdHex, "text/plain");
  }

  void titleIcon(const Request &req, Response &res, std::shared_ptr<GameReader> game) {
    u8* titleIcon = nullptr;
    u64 titleIconSize = 0;

    SET_STATUS_FROM_RC(res, game->GetIcon(titleIcon, titleIconSize));
    if (res.status == 200) {
      res.set_content((char*)titleIcon, titleIconSize, "image/jpeg");
    }
  }

  void titleInfo(const Request &req, Response &res, std::shared_ptr<GameReader> game) {
    std::string titleName, titleAuthor, titleVersion;
    u8 errorCode = 0;

    auto rc = game->GetTitleInfo(titleName, titleAuthor, titleVersion, errorCode);
    if (R_FAILED(rc)) {
      res.status = 500;
      res.set_content(
        mkjson(MKJSON_OBJ, 2, \
          MKJSON_INT, "errorCode", errorCode, \
          MKJSON_JSON_FREE, "response",
          MAKE_ERROR_FROM_RC(rc)
        ), "application/json"
      );
    } else {
      res.status = 200;
    }

    if (res.status == 200) {
      res.set_content(
        mkjson(MKJSON_OBJ, 4, \
          MKJSON_STRING, "name", titleName.c_str(), \
          MKJSON_STRING, "author", titleAuthor.c_str(), \
          MKJSON_STRING, "version", titleVersion.c_str(), \
          MKJSON_INT, "errorCode", errorCode \
        ),
        "application/json"
      );
    }
  }

  void metadata(const Request &req, Response &res, std::shared_ptr<GameReader> game) {
    char* metadataJSON = nullptr;
    SET_STATUS_FROM_RC(res, game->GetAllMetadata(metadataJSON));
    if (res.status != 200) { return; }
    res.set_content(metadataJSON, "application/json");
    free(metadataJSON);
  }

  void readMemory(bool heap, const Request &req, Response &res, std::shared_ptr<GameReader> game) {
    u64 offset = 0;
    u64 size = 0;
    std::string offsetStr = "0";
    std::string sizeStr = "4";

    if (req.has_param("offset"))
      offsetStr = req.get_param_value("offset");

    if (req.has_param("size"))
      sizeStr = req.get_param_value("size");

    try {
      offset = stoull(offsetStr, 0, 16);
      size = stoull(sizeStr, 0, 16);
    } catch (const std::invalid_argument &ia) {
      res.set_content("Invalid parameters", "text/plain");
      res.status = 400;
    }

    if (res.status != 400) {
      u8 *buffer = new u8[size];
      SET_STATUS_FROM_RC(res, game->ReadMemoryDirect(heap, offset, buffer, size));

      if (res.status == 200) {
        auto hexString = convertByteArrayToHex(buffer, size);
        res.set_content(hexString, "text/plain");
      }

      delete[] buffer;
    }
  }

  void readPointers(bool heap, const Request &req, Response &res, std::shared_ptr<GameReader> game) {
    // Read URL parameters
    std::string offsetStr = "0";
    std::string sizeStr = "4";
    if (req.has_param("offsets")) {
      offsetStr = req.get_param_value("offsets");
    }

    if (req.has_param("size")) {
      sizeStr = req.get_param_value("size");
    }

    // Split the offset list
    std::stringstream tokenizer(offsetStr);
    std::string segment;
    std::vector<std::string> segList;
    while(std::getline(tokenizer, segment, ',')) {
      segList.push_back(segment);
    }

    // Convert the offset list and size to u64
    std::vector<u64> offsets;
    u64 size = 0;
    try {
      for (auto &seg : segList) {
        offsets.push_back(stoull(seg, 0, 16));
      }
      size = stoull(sizeStr, 0, 16);
    } catch (const std::invalid_argument &ia) {
      res.set_content("Invalid parameters", "text/plain");
      res.status = 400;
    }

    // Follow the pointer offsets and read back the data
    if (res.status != 400) {
      u8 *buffer = new u8[size];
      auto rc = game->ReadMemoryPointer(heap, offsets, buffer, size);

      if (R_FAILED(rc)) {
        res.status = 500;
        res.set_content("Failed to read indirect pointer", "text/plain");
      } else {
        auto hexString = convertByteArrayToHex(buffer, size);
        res.set_content(hexString, "text/plain");
      }

      delete[] buffer;
    }
  }  
} // namespace routes

int startServer() {
  Server server;
  auto game = std::make_shared<GameReader>();

  server.set_pre_routing_handler([](const Request &req, Response &res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    return Server::HandlerResponse::Unhandled;
  });

  server.Get("/refreshMetadata", [game](const Request &req, Response &res) {
    routes::refreshMetadata(req, res, game);
  });

  server.Get("/titleId", [game](const Request &req, Response &res) {
    routes::titleId(req, res, game);
  });

  server.Get("/titleIcon", [game](const Request &req, Response &res) {
    routes::titleIcon(req, res, game);
  });

  server.Get("/titleInfo", [game](const Request &req, Response &res) {
    routes::titleInfo(req, res, game);
  });

  server.Get("/metadata", [game](const Request &req, Response &res) {
    routes::metadata(req, res, game);
  });

  server.Get("/readHeap", [game](const Request &req, Response &res) {
    routes::readMemory(true, req, res, game);
  });

  server.Get("/readHeapPointer", [game](const Request &req, Response &res) {
    routes::readPointers(true, req, res, game);
  });

  server.Get("/readMain", [game](const Request &req, Response &res) {
    routes::readMemory(false, req, res, game);
  });

  server.Get("/readMainPointer", [game](const Request &req, Response &res) {
    routes::readPointers(false, req, res, game);
  });

  server.Get("/", [](const Request &req, Response &res) {
    routes::root(req, res, nullptr);
  });

  while (appletMainLoop()) {
    server.listen("0.0.0.0", 3000);
    svcSleepThread(10000000L); // 0.01 seconds
  }

  return 0;
}
