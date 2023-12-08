
#include <optional>
#include "http_conference_web_server.cpp"

int main(int argc, char*argv[]) 
{
    HTTPConferenceWebServer app;
    return app.run(argc, argv);
}