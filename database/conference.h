#ifndef CONFERENCE_H
#define CONFERENCE_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>
#include "lecture.h"
#include "user.h"

namespace database
{
    class Conference{
        private:
            long        _id;
            std::string _title;
            std::string _description;
            std::string _location;
            std::string _format;

        public:

            static Conference fromJSON(const std::string & str);

            long               get_id() const;
            const std::string &get_location() const;
            const std::string &get_title() const;
            const std::string &get_description() const;
            const std::string &get_format() const;

            long        &id();
            std::string &location();
            std::string &title();
            std::string &description();
            std::string &format();

            static void init();
            static std::optional<Conference> read_by_id(long id);
            static std::vector<Conference> read_all();
            static bool check_conference_exists_by_id(long id);
            static std::optional<std::vector<User>> find_all_authors_of_conference_by_conference_id(long conference_id);
            static std::optional<std::vector<Lecture>> find_all_lectures_of_conference_by_conference_id(long conference_id);

            Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif