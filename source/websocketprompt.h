#ifndef TEXTENGINE_WEBSOCKETPROMPT_H_
#define TEXTENGINE_WEBSOCKETPROMPT_H_

#include <libwebsockets.h>
#include <memory>
#include <picojson.h>
#include <string>
#include <thread>

#include "prompt.h"

namespace textengine {

  class SynchronizedQueue;

  class WebSocketPrompt : public Prompt {
  public:
    WebSocketPrompt(SynchronizedQueue &command_queue,
                    SynchronizedQueue &reply_queue, const std::string &prompt);

    virtual ~WebSocketPrompt();

    void Run();

  private:static int HttpCallback(libwebsocket_context *context,
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
    SynchronizedQueue &command_queue, &reply_queue;
    const std::string &prompt;
    std::thread thread;
    libwebsocket_context *context;
  };

}  // namespace textengine

#endif  // TEXTENGINE_WEBSOCKETPROMPT_H_
