#include <iostream>
#include <libwebsockets.h>

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
  return -1;
}

int InteractiveFictionCallback(libwebsocket_context *context,
                               libwebsocket *wsi,
                               enum libwebsocket_callback_reasons reason,
                               void *user,
                               void *in,
                               size_t length) {
  return -1;
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
  int result = 0;
  while (result >= 0) {
    libwebsocket_callback_on_writable_all_protocol(&protocols[1]);
    result = libwebsocket_service(context, 50);
  }
  libwebsocket_context_destroy(context);
  context = nullptr;
  return 0;
}
