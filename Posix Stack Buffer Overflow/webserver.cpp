#include <iostream>
#include "pistache/endpoint.h"
using namespace Pistache;
using namespace std;


namespace HttpServer {
  class Logger {
  public:
    static void LogResourceName(const Http::Request &request) {
      char buffer[1024];
      const char* resourceName = request.resource().c_str();
      sprintf(buffer, "[*] Resource accessed: %s", resourceName);
      cout << (char*) buffer << endl;
    }

    static void LogUserAgent(const Http::Request &request) {
      char buffer[1024];
      auto header = request.headers().tryGet<Pistache::Http::Header::UserAgent>();
      const char* userAgent = (char*) header->agent().c_str();
      sprintf(buffer, "[*] User-Agent is: %s", userAgent);
      cout << (char*) buffer << endl;
    }
  };

  class VulnerableHandler : public Http::Handler {
  public:
    HTTP_PROTOTYPE(VulnerableHandler)

    void onRequest(const Http::Request &request, Http::ResponseWriter response) {
      if (request.resource() == "/" || request.resource() == "/index.html" || request.resource() == "/default.html")
      {
        Logger::LogResourceName(request);
        Logger::LogUserAgent(request);
        response.send(Http::Code::Ok, "Hello World!\n");
      }
      else
      {
        response.send(Http::Code::Not_Found);
      }
    }
  };
}

int main(void) {
  cout << "[*] Server is listening on *:8080" << endl;;
  Http::listenAndServe<HttpServer::VulnerableHandler>("*:8080");
  return 0;
}