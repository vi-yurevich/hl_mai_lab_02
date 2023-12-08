#include "lecture.h"
#include "user.h"
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

    void Lecture::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Lectures` ("
                            << "`id` INT NOT NULL AUTO_INCREMENT,"
                            << "`title` VARCHAR(2048) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,"
                            << "`description` VARCHAR(4096) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,"
                            << "`record_permission` BOOLEAN NOT NULL,"
                            << "`author_id` INT NOT NULL,"
                            << "`conference_id` INT NOT NULL,"
                            << "PRIMARY KEY (`id`),"
                            << "FOREIGN KEY (`author_id`) REFERENCES `User` (`id`),"
                            << "FOREIGN KEY (`conference_id`) REFERENCES `Conference` (`id`));",                   
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

    Poco::JSON::Object::Ptr Lecture::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("title", _title);
        root->set("description", _description);
        root->set("record_permission", _record_permission);
        root->set("author_id", _author_id);
        root->set("conference_id", _conference_id);

        return root;
    }

    Poco::JSON::Array::Ptr Lecture::vectorToJSON(std::vector<Lecture>& lectures)
    {
        Poco::JSON::Array::Ptr json_lecture_array = new Poco::JSON::Array();

        for (const auto& lecture : lectures) {
            json_lecture_array->add(
                Poco::Dynamic::Var(lecture.toJSON())
            );
        }

        return json_lecture_array;
    }
    
    Lecture Lecture::fromJSON(const std::string &str)
    {
        Lecture lecture;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        lecture.id() = object->getValue<long>("id");
        lecture.title() = object->getValue<std::string>("title");
        lecture.description() = object->getValue<std::string>("description");
        lecture.record_permission() = object->getValue<bool>("record_permission");
        lecture.author_id() = object->getValue<long>("author_id");
        lecture.conference_id() = object->getValue<long>("conference_id");

        return lecture;
    }

    std::optional<Lecture> Lecture::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Lecture lecture;
            select << "SELECT id, title, description, author_id, conference_id FROM Lecture WHERE id=?",
                into(lecture._id),
                into(lecture._title),
                into(lecture._description),
                into(lecture._author_id),
                into(lecture._conference_id),
                use(id),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) return lecture;
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

    std::vector<Lecture> Lecture::read_all()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Lecture> result;
            Lecture lecture;
            select << "SELECT id, title, description, author_id, conference_id FROM Lecture",
                into(lecture._id),
                into(lecture._title),
                into(lecture._description),
                into(lecture._author_id),
                into(lecture._conference_id),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(lecture);
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

    std::optional<std::vector<Lecture>> Lecture::search_by_author_id(long author_id)
    {
        try
        {
            if (database::User::check_user_exists_by_id(author_id)) {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Lecture> result;
            Lecture lecture;
            select << "SELECT id, title, description, author_id, conference_id FROM Lecture WHERE author_id= ?",
                into(lecture._id),
                into(lecture._title),
                into(lecture._description),
                into(lecture._author_id),
                into(lecture._conference_id),
                use(author_id),
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

    bool Lecture::check_lecture_exists_by_id(long id) 
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            int count = 0;
            
            select << "SELECT COUNT(*) FROM `Lecture` WHERE id = ?",
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

    void Lecture::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Lecture (title, description, author_id, conference_id) VALUES(?, ?, ?, ?)",
                use(_title),
                use(_description),
                use(_author_id),
                use(_conference_id),

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
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

    long Lecture::get_id() const
    {
        return _id;
    }

    const std::string &Lecture::get_title() const
    {
        return _title;
    }

    const std::string &Lecture::get_description() const
    {
        return _description;
    }

    const bool &Lecture::get_record_permission() const
    {
        return _record_permission;
    }

    const long &Lecture::get_author_id() const
    {
        return _author_id;
    }    
    
    const long &Lecture::get_conference_id() const
    {
        return _conference_id;
    }

    long &Lecture::id()
    {
        return _id;
    }

    std::string &Lecture::title()
    {
        return _title;
    }

    std::string &Lecture::description()
    {
        return _description;
    }

    bool &Lecture::record_permission()
    {
        return _record_permission;
    }

    long &Lecture::author_id()
    {
        return _author_id;
    }

    long &Lecture::conference_id()
    {
        return _conference_id;
    }    
}