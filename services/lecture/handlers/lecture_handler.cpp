#ifndef LECTURE_HANDLER_H
#define LECTURE_HANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "../../../database/user.h"
#include "../../../database/lecture.h"
#include "../../../database/conference.h"

static bool hasSubstr(const std::string &str, const std::string &substr)
{
    if (str.size() < substr.size())
        return false;
    for (size_t i = 0; i <= str.size() - substr.size(); ++i)
    {
        bool ok{true};
        for (size_t j = 0; ok 
        && (j < substr.size()); ++j)
            ok = (str[i + j] == substr[j]);
        if (ok)
            return true;
    }
    return false;
}

class LectureHandler : public HTTPRequestHandler
{
private:
    bool check_title(const std::string &title, std::string &reason)
    {
        if (title.find('\t') != std::string::npos)
        {
            reason = "Title can't contain character tabulation";
            return false;
        }
        if (title.find('\n') != std::string::npos)
        {
            reason = "Title can't contain character '\\n'";
            return false;
        }

        return true;
    };

    bool check_description(const std::string &description, std::string &reason)
    {
        if (description.find('\t') != std::string::npos)
        {
            reason = "description can't contain character tabulation";
            return false;
        }
        if (description.find('\n') != std::string::npos)
        {
            reason = "description can't contain character '\\n'";
            return false;
        }

        return true;
    };

    bool check_permission(const std::string &permission, std::string &reason)
    {
        if (permission != "true" || permission != "false")
        {
            std::ostringstream oss;
            oss << "Wrong record_permission identifier: " << permission << ".";
            reason = oss.str();
            return false;
        }

        return true;
    };

    bool check_author_id(const std::string &author_id, std::string &reason)
    {
        long id = atol(author_id.c_str());
        if (!database::User::check_user_exists_by_id(id))
        {
            std::ostringstream oss;
            oss << "User with id: " << author_id << "doesn't exist.";
            reason = oss.str();
            return false;
        }

        return true;
    };

    bool check_conference_id(const std::string &conference_id, std::string &reason)
    {
        long id = atol(conference_id.c_str());
        if (!database::Conference::check_conference_exists_by_id(id))
        {
            std::ostringstream oss;
            oss << "Conference with id: " << conference_id << "doesn't exist.";
            reason = oss.str();
            return false;
        }

        return true;
    };

    bool query_string_boolean_to_bool(const std::string &str) 
    {
        if (str == "true")
            return true;
        else
            return false;
    }

public:
    LectureHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());
        try
        {
            if (form.has("id") 
                && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
                && (hasSubstr(request.getURI(), "/lecture")))
            {
                long id = atol(form.get("id").c_str());

                std::optional<database::Lecture> result = database::Lecture::read_by_id(id);
                if (result)
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(result->toJSON(), ostr);
                    return;
                }
                else
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("type", "/errors/not_found");
                    root->set("title", "Internal exception");
                    root->set("status", "404");
                    root->set("detail", "user ot found");
                    root->set("instance", "/user");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(root, ostr);
                    return;
                }
            }
            else if (form.has("author_id")
                    && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
                    && (hasSubstr(request.getURI(), "/lectures/by_author")))
            {
                long author_id = atol(form.get("author_id").c_str());
                auto result = database::Lecture::search_by_author_id(author_id);
                if (result)
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(
                        database::Lecture::vectorToJSON(result.value()), 
                        ostr);
                    return;
                }
                else
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("type", "/errors/not_found");
                    root->set("title", "Internal exception");
                    root->set("status", "404");
                    root->set("detail", "user ot found");
                    root->set("instance", "/user");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(root, ostr);
                    return;
                }
            }
            else if (hasSubstr(request.getURI(), "/lectures")
                    && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET))
            {
                auto results = database::Lecture::read_all();
                auto result_json = database::Lecture::vectorToJSON(results);
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(result_json, ostr);

                return;
            }
            else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
            {
                if (form.has("title") 
                    && form.has("description") 
                    && form.has("record_permission") 
                    && form.has("author_id") 
                    && form.has("conference_id"))
                {
                    bool check_result = true;
                    std::string message;
                    std::string reason;

                    if (!check_title(form.get("title"), reason))
                    {
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }

                    if (!check_description(form.get("description"), reason))
                    {
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }

                    if (!check_permission(form.get("record_permission"), reason))
                    {
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }

                    if (!check_author_id(form.get("author_id"), reason))
                    {
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }
                    
                    if (!check_conference_id(form.get("conference_id"), reason))
                    {
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }

                    if (check_result)
                    {
                        database::Lecture lecture;
                        lecture.title() = form.get("title");
                        lecture.description() = form.get("description");
                        lecture.record_permission() = query_string_boolean_to_bool(form.get("record_permission"));
                        lecture.author_id() = atol(form.get("author_id").c_str());
                        lecture.conference_id() = atol(form.get("conference_id").c_str());

                        lecture.save_to_mysql();
                        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("application/json");
                        std::ostream &ostr = response.send();
                        ostr << lecture.get_id();
                        return;
                    }
                    else
                    {
                        response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                        std::ostream &ostr = response.send();
                        ostr << message;
                        response.send();
                        return;
                    }
                }
            }
        }
        catch (...)
        {
        }

        response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/not_found");
        root->set("title", "Internal exception");
        root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        root->set("detail", "request ot found");
        root->set("instance", "/user");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

private:
    std::string _format;
};
#endif