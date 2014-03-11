#ifndef TEXTENGINE_WEBSOCKETPROMPT_H_
#define TEXTENGINE_WEBSOCKETPROMPT_H_

#include <libwebsockets.h>
#include <memory>
#include <picojson.h>
#include <string>
#include <thread>
#include <unordered_map>

#include "prompt.h"

namespace textengine {

  class Log;
  class SynchronizedQueue;
  
  struct Resource {
    std::string path, content_type;
  };

  class WebSocketPrompt : public Prompt {
  public:
    WebSocketPrompt(SynchronizedQueue &reply_queue, const std::string &prompt, Log &log);

    virtual ~WebSocketPrompt();

    void Run();
    
    static std::unordered_map<std::string, Resource> resource_map;

  private:
    static constexpr const char *kApplicationJavascript = u8"application/javascript";
    static constexpr const char *kApplicationOpenTypeFont = u8"application/vnd.ms-opentype";
    static constexpr const char *kApplicationTrueTypeFont = u8"application/x-font-ttf";
    static constexpr const char *kImagePng = u8"image/png";
    static constexpr const char *kTextHtml = u8"text/html";
    
    static int HttpCallback(libwebsocket_context *context,
                            libwebsocket *wsi,
                            enum libwebsocket_callback_reasons reason,
                            void *user,
                            void *in,
                            size_t length);

    static int InteractiveFictionCallback(libwebsocket_context *context,
                                          libwebsocket *wsi,
                                          enum libwebsocket_callback_reasons reason,
                                          void *user,
                                          void *in,
                                          size_t length);

    static libwebsocket_protocols kProtocols[];

    static WebSocketPrompt *instance;

    void HandleRequest(picojson::value &message);

    std::unique_ptr<picojson::value> HandleResponse();

    void Loop();

  private:
    SynchronizedQueue &reply_queue;
    const std::string &prompt;
    Log &log;
    std::thread thread;
    libwebsocket_context *context;
  };

}  // namespace textengine

#endif  // TEXTENGINE_WEBSOCKETPROMPT_H_
