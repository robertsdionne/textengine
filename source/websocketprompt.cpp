#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "checks.h"
#include "commandqueue.h"
#include "websocketprompt.h"

namespace textengine {

  libwebsocket_protocols WebSocketPrompt::kProtocols[] = {
    {
      "http-only",
      WebSocketPrompt::HttpCallback,
      0,
      0
    }, {
      "interactive-fiction-protocol",
      WebSocketPrompt::InteractiveFictionCallback,
      0,
      4096
    },
    {nullptr, nullptr, 0, 0}
  };

  WebSocketPrompt *WebSocketPrompt::instance = nullptr;

  WebSocketPrompt::WebSocketPrompt(CommandQueue &queue, const std::string &prompt)
  : queue(queue), prompt(prompt) {
    instance = this;
  }

  WebSocketPrompt::~WebSocketPrompt() {
    instance = nullptr;
  }

  int WebSocketPrompt::HttpCallback(libwebsocket_context *context,
                                   libwebsocket *wsi,
                                   enum libwebsocket_callback_reasons reason,
                                   void *user,
                                   void *in,
                                   size_t length) {
    switch (reason) {
      case LWS_CALLBACK_HTTP: {
        const std::string url_path = reinterpret_cast<const char *>(in);
        std::string resource_path, content_type;
        if ("/" == url_path) {
          resource_path = "../resource/index.html";
          content_type = "text/html";
        } else if ("/client.js" == url_path) {
          resource_path = "../resource/client.js";
          content_type = "application/javascript";
        } else {
          return -1;
        }
        std::cout << "serving " << resource_path << " (" << content_type << ")" << std::endl;
        if (libwebsockets_serve_http_file(context, wsi,
                                          resource_path.c_str(), content_type.c_str())) {
          return -1;
        }
        break;
      }
      case LWS_CALLBACK_HTTP_FILE_COMPLETION: {
        return -1;
        break;
      }
      default:
        break;
    }
    return 0;
  }

  int WebSocketPrompt::InteractiveFictionCallback(libwebsocket_context *context,
                                                  libwebsocket *wsi,
                                                  enum libwebsocket_callback_reasons reason,
                                                  void *user,
                                                  void *in,
                                                  size_t length) {
    switch (reason) {
      case LWS_CALLBACK_SERVER_WRITEABLE: {
        const std::unique_ptr<picojson::value> response = instance ? instance->HandleResponse() : nullptr;
        if (response) {
          std::ostringstream out;
          out << *response;
          const std::string json = out.str();
          unsigned char buffer[LWS_SEND_BUFFER_PRE_PADDING + 4096 + LWS_SEND_BUFFER_POST_PADDING];
          unsigned char *p = &buffer[LWS_SEND_BUFFER_PRE_PADDING];
          std::copy(json.begin(), json.end(), p);
          CHECK_STATE(!libwebsocket_write(wsi, p, json.size(), LWS_WRITE_TEXT));
        } else {
          return -1;
        }
        break;
      }
      case LWS_CALLBACK_RECEIVE: {
        if (instance) {
          const std::string json = reinterpret_cast<const char *>(in);
          std::istringstream input(json);
          picojson::value message;
          input >> message;
          instance->HandleRequest(message);
        }
        break;
      }
      default:
        break;
    }
    return 0;
  }

  void WebSocketPrompt::HandleRequest(picojson::value &message) {
    std::cout << "received: " << message << std::endl;
  }

  std::unique_ptr<picojson::value> WebSocketPrompt::HandleResponse() {
    picojson::object response;
    response["asdf"] = picojson::value("hi");
    std::unique_ptr<picojson::value> result{new picojson::value(response)};
    return result;
  }

  void WebSocketPrompt::Loop() {
    lws_context_creation_info context_creation_info = {
      8888,
      "",
      kProtocols,
      libwebsocket_get_internal_extensions(),
      nullptr,
      nullptr,
      nullptr,
      -1,
      -1,
      0,
      nullptr,
      0,
      0,
      0
    };
    context = libwebsocket_create_context(&context_creation_info);
    CHECK_STATE(context);
    std::chrono::high_resolution_clock clock;
    auto old_time = clock.now();
    int result = 0;
    while (result >= 0) {
      auto new_time = clock.now();
      if (new_time - old_time > std::chrono::milliseconds(16)) {
        libwebsocket_callback_on_writable_all_protocol(&kProtocols[1]);
        old_time = new_time;
      }
      result = libwebsocket_service(context, 50);
    }
    libwebsocket_context_destroy(context);
    context = nullptr;
  }

  void WebSocketPrompt::Run() {
    thread = std::thread(&WebSocketPrompt::Loop, this);
    thread.detach();
  }

}  // namespace textengine
