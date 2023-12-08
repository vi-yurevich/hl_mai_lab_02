#ifndef LECTURE_H
#define LECTURE_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class Lecture{
        private:
            long        _id;
            std::string _title;
            std::string _description;
            bool        _record_permission;
            long        _author_id;
            long        _conference_id;

        public:

            static Lecture fromJSON(const std::string & str);

            long               get_id() const;
            const std::string &get_title() const;
            const std::string &get_description() const;
            const bool        &get_record_permission() const;
            const long        &get_author_id() const;
            const long        &get_conference_id() const;

            long        &id();
            std::string &title();
            std::string &description();
            bool        &record_permission();
            long        &author_id();
            long        &conference_id();

            static void init();
            static std::optional<Lecture> read_by_id(long id);
            static std::vector<Lecture> read_all();
            static bool check_lecture_exists_by_id(long id);
            static std::optional<std::vector<Lecture>> search_by_author_id(long author_id);
            void save_to_mysql();

            Poco::JSON::Object::Ptr toJSON() const;
            static Poco::JSON::Array::Ptr vectorToJSON(std::vector<Lecture>& lectures);

    };
}

#endif