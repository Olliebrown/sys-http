#include "routes.h"

#include "game_reader.h"
#include "utils.h"
#include <memory>

#include "httplib.h"
#include "res_macros.h"

using namespace httplib;

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

  void readHeap(const Request &req, Response &res, std::shared_ptr<GameReader> game) {
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
      auto rc = game->ReadHeap(offset, buffer, size);

      if (R_FAILED(rc)) {
        res.status = 500;
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
    res.set_header("Access-Control-Allow-Origin", "*");
    routes::refreshMetadata(req, res, game);
  });

  server.Get("/titleId", [game](const Request &req, Response &res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    routes::titleId(req, res, game);
  });

  server.Get("/readHeap", [game](const Request &req, Response &res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    routes::readHeap(req, res, game);
  });

  server.Get("/", [](const Request &req, Response &res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    routes::root(req, res, nullptr);
  });

  while (appletMainLoop()) {
    server.listen("0.0.0.0", 3000);
    svcSleepThread(10000000L); // 0.01 seconds
  }

  return 0;
}
