#include <sstream>
#include "db.h"
#include <algorithm>

void GasWorksDB::insert_record(const Record &external_new_record) {
    Record new_record = external_new_record;    // to allow setting id
    new_record.id = table.empty() ? 0 : table.size();
    table.push_back(new_record);
    update_index(new_record, new_record.id);
}

bool GasWorksDB::find_by_age_range(unsigned int age_lower_bound, unsigned int age_upper_bound,
                                   std::vector<Record> &found_records) {

    if (age_lower_bound > age_upper_bound) {
        return false;
    }

    if (enable_index) {
        std::vector<unsigned> record_ids;
        index_group->get_age_index().find_records_by_age_range(age_lower_bound, age_upper_bound, record_ids);

        if (record_ids.empty()) {
            return false;
        }

        for (auto id: record_ids) {
            found_records.push_back(table[id]);
        }
        return true;
    }

    // search by iterating the table records (slooow)
    for (auto &record: table) {
        if (record.age >= age_lower_bound && record.age <= age_upper_bound) {
            found_records.push_back(record);
        }
    }
    return !found_records.empty();
}

bool GasWorksDB::find_by_age(unsigned age, std::vector<Record> &found_records) {
    if (enable_index) {
        std::vector<unsigned> record_ids;
        index_group->get_age_index().find_records(age, record_ids);

        if (record_ids.empty()) {
            return false;
        }

        for (auto id: record_ids) {
            found_records.push_back(table[id]);
        }
        return true;
    }

    // search by iterating the table records (slooow)
    for (auto &record: table) {
        if (record.age == age) {
            found_records.push_back(record);
        }
    }
    return !found_records.empty();
}

bool GasWorksDB::find_by_id(unsigned int id, Record &found_record) {
    for (auto &record: table) {
        if (record.id == id) {
            found_record = record;
            return true;
        }
    }
    return false;
}

bool GasWorksDB::find_by_name(const std::string &name, std::vector<Record> &found_records) {
    if (enable_index) {
        std::vector<unsigned> record_ids;
        index_group->get_name_index().find_records(name, record_ids);

        if (record_ids.empty()) {
            return false;
        }

        for (auto id: record_ids) {
            found_records.push_back(table[id]);
        }
        return true;
    }
    for (auto &record: table) {
        if (record.get_name() == name) {
            found_records.push_back(record);
        }
    }
    return !found_records.empty();

}

/*
 * homework
 */
// return exact match
// would be nice to do partial match
bool GasWorksDB::find_by_hobbies(const std::unordered_set<std::string> &hobbies, std::vector<Record> &found_records) {
    if (enable_index) {
        std::vector<unsigned> record_ids;
        index_group->get_hobby_index().find_records(hobbies_key_hasher(hobbies), record_ids);

        if (record_ids.empty()) {
            return false;
        }

        for (auto id: record_ids) {
            found_records.push_back(table[id]);
        }
        return true;
    }

    // search by iterating the table records (slooow)
    for (auto &record: table) {
        auto &record_hobbies = record.hobbies;
        if (record_hobbies.size() != hobbies.size()) {
            continue;
        }

        bool full_match = true;
        for (auto &hobby: record_hobbies) {
            if (hobbies.find(hobby) == hobbies.end()) {
                full_match = false;
                break;
            }
        }

        if (full_match) {
            found_records.push_back(record);
        }
    }
    return !found_records.empty();
}

void GasWorksDB::update_index(const Record &new_record, unsigned record_id) {
    index_group->get_name_index().insert_record(new_record.name, record_id);
    index_group->get_age_index().insert_record(new_record.age, record_id);
    index_group->get_hobby_index().insert_record(hobbies_key_hasher(new_record.hobbies), record_id);
}

bool GasWorksDB::get_enable_index() const {
    return enable_index;
}

void GasWorksDB::set_enable_index(bool enableIndex) {
    enable_index = enableIndex;
}

std::string GasWorksDB::hobbies_key_hasher(std::unordered_set<std::string> hobbies) {
    if (hobbies.empty()) {
        return "";
    }
    auto sorted_hobbies = std::vector<std::string>(hobbies.begin(), hobbies.end());
    std::sort(sorted_hobbies.begin(), sorted_hobbies.end());

    // feelin' lazy. Use this:
    // https://stackoverflow.com/questions/34076547/convert-a-set-of-strings-into-a-simple-string-c
    std::ostringstream stream;
    std::copy(sorted_hobbies.begin(),
              sorted_hobbies.end(),
              std::ostream_iterator<std::string>(stream, ","));
    return stream.str();
}

