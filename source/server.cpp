#include <algorithm>
#include <chrono>
#include <iostream>
#include <libwebsockets.h>
#include <string>

#include "checks.h"

struct PerSessionDataHttp {
  int file_descriptor;
};

struct PerSessionDataInteractiveFiction {
  int blah;
};

int HttpCallback(libwebsocket_context *context,
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

int InteractiveFictionCallback(libwebsocket_context *context,
                               libwebsocket *wsi,
                               enum libwebsocket_callback_reasons reason,
                               void *user,
                               void *in,
                               size_t length) {
  switch (reason) {
    case LWS_CALLBACK_ESTABLISHED: {
      std::cout << "LWS_CALLBACK_ESTABLISHED" << std::endl;
      break;
    }
    case LWS_CALLBACK_SERVER_WRITEABLE: {
      const std::string response = "hallo";
      unsigned char buffer[LWS_SEND_BUFFER_PRE_PADDING + 128 + LWS_SEND_BUFFER_POST_PADDING];
      unsigned char *p = &buffer[LWS_SEND_BUFFER_PRE_PADDING];
      std::copy(response.begin(), response.end(), p);
      CHECK_STATE(!libwebsocket_write(wsi, p, response.size(), LWS_WRITE_TEXT));
      break;
    }
    case LWS_CALLBACK_RECEIVE: {
      const std::string message = reinterpret_cast<const char *>(in);
      std::cout << "received: " << message << std::endl;
      break;
    }
    default:
      break;
  }
  return 0;
}

libwebsocket_protocols protocols[] = {
  {
    "http-only",
    HttpCallback,
    sizeof(PerSessionDataHttp),
    0
  }, {
    "interactive-fiction-protocol",
    InteractiveFictionCallback,
    sizeof(PerSessionDataInteractiveFiction),
    128
  },
  {nullptr, nullptr, 0, 0}
};

int main(int argument_count, char *arguments[]) {
  lws_context_creation_info context_creation_info = {
    8888,
    "",
    protocols,
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
  auto *context = libwebsocket_create_context(&context_creation_info);
  CHECK_STATE(context);
  std::chrono::high_resolution_clock clock;
  auto old_time = clock.now();
  int result = 0;
  while (result >= 0) {
    auto new_time = clock.now();
    if (std::chrono::duration_cast<std::chrono::microseconds>(new_time - old_time).count() > 50000) {
      libwebsocket_callback_on_writable_all_protocol(&protocols[1]);
      old_time = new_time;
    }
    result = libwebsocket_service(context, 50);
  }
  libwebsocket_context_destroy(context);
  context = nullptr;
  return 0;
}
