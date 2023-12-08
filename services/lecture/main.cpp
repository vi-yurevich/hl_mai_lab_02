
#include <optional>
#include "http_lecture_web_server.cpp"

int main(int argc, char*argv[]) 
{
    HTTPLectureWebServer app;
    return app.run(argc, argv);
}