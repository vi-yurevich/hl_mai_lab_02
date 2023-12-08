#include "user.h"
#include "lecture.h"
#include "conference.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void Conference::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Conference` ("
                            << "`id` INT NOT NULL AUTO_INCREMENT,"
                            << "`location` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,"
                            << "`title` VARCHAR(2048) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,"
                            << "`description` VARCHAR(4096) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,"
                            << "`format` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,"
                            << "PRIMARY KEY (`id`));",
                now;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr Conference::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("location", _location);
        root->set("title", _title);
        root->set("description", _description);
        root->set("format", _format);

        return root;
    }

    Conference Conference::fromJSON(const std::string &str)
    {
        Conference conference;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        conference.id() = object->getValue<long>("id");
        conference.location() = object->getValue<std::string>("location");
        conference.title() = object->getValue<std::string>("title");
        conference.description() = object->getValue<std::string>("description");
        conference.format() = object->getValue<std::string>("format");

        return conference;
    }

    std::optional<Conference> Conference::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Conference conference;
            select << "SELECT id, location, title, description, format FROM Conference where id=?",
                into(conference._id),
                into(conference._location),
                into(conference._title),
                into(conference._description),
                into(conference._format),
                use(id),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) return conference;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            
        }
        return {};
    }

    std::vector<Conference> Conference::read_all()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Conference> result;
            Conference conference;
            select << "SELECT id, location, title, description, format FROM Conference",
                into(conference._id),
                into(conference._location),
                into(conference._title),
                into(conference._description),
                into(conference._format),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(conference);
            }
            return result;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    bool Conference::check_conference_exists_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            int count = 0;
            
            select << "SELECT COUNT(*) FROM `Conference` WHERE id = ?",
                into(count),
                use(id);
            
            select.execute();

            if (count > 0)
                return true;
            else
                return false;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::optional<std::vector<User>> Conference::find_all_authors_of_conference_by_conference_id(long conference_id) 
    {
        try
        {
            if (database::Conference::check_conference_exists_by_id(conference_id)) {
                Poco::Data::Session session = database::Database::get().create_session();
                Statement select(session);
                std::vector<User> result;
                User a;
                select << "SELECT U.id, first_name, last_name, email, phone_number, `login`, `password` "
                    << "FROM `Conference` C INNER JOIN `Lecture` L ON C.id = L.conference_id "
                    << "INNER JOIN `User` U ON L.author_id = U.id "
                    << "WHERE C.id = ?",
                    into(a.id()),
                    into(a.first_name()),
                    into(a.last_name()),
                    into(a.email()),
                    into(a.phone_number()),
                    into(a.login()),
                    into(a.password()),
                    use(conference_id),
                    range(0, 1); //  iterate over result set one row at a time

                while (!select.done())
                {
                    if (select.execute())
                        result.push_back(a);
                }
                return result;
            }
            else 
                return {};
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }
    
    std::optional<std::vector<Lecture>> Conference::find_all_lectures_of_conference_by_conference_id(long conference_id)
    {
        try
        {
            if (database::Conference::check_conference_exists_by_id(conference_id))             
            {
                Poco::Data::Session session = database::Database::get().create_session();
                Statement select(session);
                std::vector<Lecture> result;
                Lecture lecture;
                select << "SELECT L.id, L.`title`, L.`description`, record_permission, author_id, conference_id "
                    << "FROM `Conference` C INNER JOIN `Lecture` L ON C.id = L.conference_id "
                    << "WHERE C.id = ?",
                    into(lecture.id()),
                    into(lecture.title()),
                    into(lecture.description()),
                    into(lecture.author_id()),
                    into(lecture.conference_id()),
                    use(conference_id),
                    range(0, 1); //  iterate over result set one row at a time

                while (!select.done())
                {
                    if (select.execute())
                        result.push_back(lecture);
                }
                return result;
            }
            else
                return {};
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    long Conference::get_id() const
    {
        return _id;
    }

    const std::string &Conference::get_location() const
    {
        return _location;
    }

    const std::string &Conference::get_title() const
    {
        return _title;
    }

    const std::string &Conference::get_description() const
    {
        return _description;
    }

    const std::string &Conference::get_format() const
    {
        return _format;
    }

    long &Conference::id()
    {
        return _id;
    }

    std::string &Conference::location()
    {
        return _location;
    }

    std::string &Conference::title()
    {
        return _title;
    }

    std::string &Conference::description()
    {
        return _description;
    }

    std::string &Conference::format()
    {
        return _format;
    }
}