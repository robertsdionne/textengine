#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CFBundle.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "checks.h"
#include "synchronizedqueue.h"
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
  
  
  std::unordered_map<std::string, Resource> WebSocketPrompt::resource_map{
    {u8"/", Resource{u8"../resource/web/index.html", kTextHtml}},
    {u8"/client.js", Resource{u8"../resource/web/client.js", kApplicationJavascript}},
    {u8"/EVA1.ttf", Resource{u8"../resource/fonts/EVA1.ttf", kApplicationTrueTypeFont}},
    {u8"/Stela_UT.otf", Resource{u8"../resource/fonts/Stela_UT.otf", kApplicationOpenTypeFont}},
    {u8"/Ubuntu-M.ttf", Resource{u8"../resource/fonts/ubuntu-font-family-0.80/Ubuntu-M.ttf",
      kApplicationTrueTypeFont}},
    {u8"/Ubuntu-R.ttf", Resource{u8"../resource/fonts/ubuntu-font-family-0.80/Ubuntu-R.ttf",
      kApplicationTrueTypeFont}},
    {u8"/Vetka.otf", Resource{u8"../resource/fonts/Vetka.otf", kApplicationOpenTypeFont}}
  };

  WebSocketPrompt *WebSocketPrompt::instance = nullptr;

  WebSocketPrompt::WebSocketPrompt(SynchronizedQueue &reply_queue,
                                   const std::string &prompt)
  : reply_queue(reply_queue), prompt(prompt), thread(), context() {
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
        if (resource_map.cend() == resource_map.find(url_path)) {
          return -1;
        }
        const auto &resource = resource_map[url_path];
        std::cout << "serving " << resource.path << " (" << resource.content_type << ")" << std::endl;
        if (libwebsockets_serve_http_file(context, wsi,
                                          resource.path.c_str(), resource.content_type.c_str())) {
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
    if (!instance) {
      return -1;
    }
    switch (reason) {
      case LWS_CALLBACK_SERVER_WRITEABLE: {
        const std::unique_ptr<picojson::value> response = instance->HandleResponse();
        if (response) {
          std::ostringstream out;
          out << *response;
          const std::string json = out.str();
          unsigned char buffer[LWS_SEND_BUFFER_PRE_PADDING + 4096 + LWS_SEND_BUFFER_POST_PADDING];
          unsigned char *p = &buffer[LWS_SEND_BUFFER_PRE_PADDING];
          std::copy(json.begin(), json.end(), p);
          CHECK_STATE(!libwebsocket_write(wsi, p, json.size(), LWS_WRITE_TEXT));
        }
        break;
      }
      default:
        break;
    }
    return 0;
  }

  std::unique_ptr<picojson::value> WebSocketPrompt::HandleResponse() {
    if (reply_queue.HasMessage()) {
      std::unique_ptr<picojson::value> result{new picojson::value(reply_queue.PopMessage()->ToJson())};
      return result;
    } else {
      return nullptr;
    }
  }

  void WebSocketPrompt::Loop() {
    lws_context_creation_info context_creation_info = {
      8888,
      nullptr,
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

    const std::string url_string = "http://localhost:8888";
    CFURLRef url = CFURLCreateWithBytes(nullptr, (UInt8 *)(url_string.c_str()),
                                        url_string.size(), kCFStringEncodingUTF8, nullptr);
    LSOpenCFURLRef(url, nullptr);
    CFRelease(url);

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
