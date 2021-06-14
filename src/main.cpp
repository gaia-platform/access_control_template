#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>
#include <vector>

#include "gaia_access_control.h"
#include "ui_messaging.hpp"
#include "enums.hpp"
#include "helpers.hpp"
#include "json.hpp"

#include "gaia/db/db.hpp"
#include "gaia/logger.hpp"
#include "gaia/rules/rules.hpp"
#include "gaia/system.hpp"

using json = nlohmann::json;
using namespace gaia::access_control;

void exit_callback(int signal_number)
{
    gaia::system::shutdown();
    std::cout << std::endl
              << "Exiting." << std::endl;
    exit(signal_number);
}

json get_event_json(event_t event)
{
    json j;

    j["name"] = event.name();
    j["start_timestamp"] = event.start_timestamp();
    j["end_timestamp"] = event.end_timestamp();
    j["room_name"] = event.held_in_room().name();

    return j;
}

json get_person_json(person_t person)
{
    json j;

    j["person_id"] = person.person_id();
    j["first_name"] = person.first_name();
    j["employee"] = person.employee();
    j["visitor"] = person.visitor();
    j["stranger"] = person.stranger();
    j["badged"] = person.badged();
    j["parked"] = person.parked();
    j["credentialed"] = person.credentialed();
    j["admissible"] = person.admissible();
    j["on_wifi"] = person.on_wifi();

    j["events"] = json::array();
    for (auto reg_iter = person.registered_registration_list().begin();
         reg_iter != person.registered_registration_list().end();
         reg_iter++)
    {
        j["events"].push_back(get_event_json(reg_iter->occasion_event()));
    }

    if (person.inside_room())
    {
        j["inside_room"] = person.inside_room().name();
    }

    return j;
}

json get_room_json(room_t room)
{
    json j;

    j["room_id"] = room.room_id();
    j["name"] = room.name();
    j["capacity"] = room.capacity();
    j["people"] = json::array();

    for (auto person_iter = room.inside_person_list().begin();
         person_iter != room.inside_person_list().end();
         person_iter++)
    {
        j["people"].push_back(get_person_json(*person_iter));
    }

    j["events"] = json::array();
    for (auto event_iter = room.held_in_event_list().begin();
         event_iter != room.held_in_event_list().end();
         event_iter++)
    {
        j["events"].push_back(get_event_json(*event_iter));
    }

    return j;
}

json get_building_json(building_t building)
{
    json j;

    j["building_id"] = building.building_id();
    j["name"] = building.name();
    j["rooms"] = json::array();
    for (auto room_iter = building.inside_room_list().begin();
         room_iter != building.inside_room_list().end();
         room_iter++)
    {
        j["rooms"].push_back(get_room_json(*room_iter));
    }

    j["people"] = json::array();

    for (auto person_iter = building.entered_person_list().begin();
         person_iter != building.entered_person_list().end();
         person_iter++)
    {
        if (!person_iter->inside_room()) {
            j["people"].push_back(get_person_json(*person_iter));
        }
    }

    return j;
}

void update_ui()
{
    gaia::db::begin_transaction();

    json j;

    j["buildings"] = json::array();

    for (auto building_iter = building_t::list().begin();
         building_iter != building_t::list().end();
         building_iter++)
    {
        j["buildings"].push_back(get_building_json(*building_iter));
    }

    j["people"] = json::array();
    for (auto person_iter = person_t::list().begin();
         person_iter != person_t::list().end();
         person_iter++)
    {
        if (!person_iter->inside_room() && !person_iter->entered_building())
        {
            j["people"].push_back(get_person_json(*person_iter));
        }
    }

    ui_messaging::send(j.dump());

    gaia::db::commit_transaction();
}

event_t add_event(std::string name, uint64_t start_timestamp,
                    uint64_t end_timestamp, room_t room)
{
    auto event_w = event_writer();
    event_w.name = name;
    event_w.start_timestamp = start_timestamp;
    event_w.end_timestamp = end_timestamp;
    event_t new_event = event_t::get(event_w.insert_row());

    room.held_in_event_list().insert(new_event);

    return new_event;
}

registration_t add_registration(person_t person, event_t occasion)
{
    registration_t registration = registration_t::get(registration_t::insert_row("", 0));

    occasion.occasion_registration_list().insert(registration);
    person.registered_registration_list().insert(registration);

    return registration;
}

room_t add_room(uint64_t room_id, std::string room_name,
              uint32_t capacity, building_t headquarters)
{
    auto room_w = room_writer();
    room_w.room_id = room_id;
    room_w.name = room_name;
    room_w.capacity = capacity;
    room_t new_room = room_t::get(room_w.insert_row());

    // Connect the room to the "headquarters" building
    headquarters.inside_room_list().insert(new_room);

    return new_room;
}

person_t add_person(uint64_t person_id, std::string first_name,
                bool employee, bool visitor, bool stranger)
{
    auto person_w = person_writer();
    person_w.person_id = person_id;
    person_w.first_name = first_name;
    person_w.employee = employee;
    person_w.visitor = visitor;
    person_w.stranger = stranger;
    person_w.entry_time = 0;
    person_w.leave_time = 100000;
    return person_t::get(person_w.insert_row());
}

void populate_all_tables()
{
    using namespace gaia::access_control;

    helpers::set_time(480);

    person_t john = add_person(1, "John", true, false, false);
    person_t jane = add_person(2, "Jane", false, true, false);
    person_t stranger = add_person(3, "Mr. Stranger", false, false, true);

    // Headquarters building.
    auto headquarters_w = building_writer();
    headquarters_w.building_id = 10;
    headquarters_w.name = "HQ Building";
    building_t headquarters = building_t::get(headquarters_w.insert_row());

    room_t room;
    event_t event;

    room = add_room(102, "Auditorium", 200, headquarters);
    event = add_event("Happy hour", 720, 840, room);
    add_registration(john, event);
    add_registration(jane, event);

    room = add_room(104, "Big Room", 4, headquarters);
    event = add_event("The best meeting", 660, 720, room);
    event = add_event("Boring meeting", 600, 660, room);
    add_registration(john, event);
    add_registration(jane, event);

    room = add_room(103, "Little Room", 3, headquarters);
    event = add_event("Important meeting", 540, 600, room);
    add_registration(john, event);
}

void clear_all_tables()
{
    using namespace gaia::access_control;

    for (auto building = building_t::get_first(); building; building = building_t::get_first())
    {
        building.inside_room_list().clear();
        building.parked_in_person_list().clear();
        building.entered_person_list().clear();
        building.seen_at_scan_list().clear();
        building.delete_row();
    }
    for (auto room = room_t::get_first(); room; room = room_t::get_first())
    {
        room.inside_person_list().clear();
        room.allowed_in_permitted_room_list().clear();
        room.held_in_event_list().clear();
        room.seen_in_scan_list().clear();
        room.delete_row();
    }
    for (auto person = person_t::get_first(); person; person = person_t::get_first())
    {
        person.permittee_permitted_room_list().clear();
        person.registered_registration_list().clear();
        person.vehicle_owner_vehicle_list().clear();
        person.seen_who_scan_list().clear();
        person.delete_row();
    }
    for (auto permitted_room = permitted_room_t::get_first(); permitted_room;
         permitted_room = permitted_room_t::get_first())
    {
        permitted_room.delete_row();
    }
    for (auto event = event_t::get_first(); event; event = event_t::get_first())
    {
        event.occasion_registration_list().clear();
        event.delete_row();
    }
    for (auto registration = registration_t::get_first(); registration;
         registration = registration_t::get_first())
    {
        registration.delete_row();
    }
    for (auto vehicle = vehicle_t::get_first(); vehicle; vehicle = vehicle_t::get_first())
    {
        vehicle.seen_license_scan_list().clear();
        vehicle.delete_row();
    }
    for (auto scan = scan_t::get_first(); scan; scan = scan_t::get_first())
    {
        scan.delete_row();
    }
}

bool get_person(uint64_t person_id, person_t& person)
{
    auto person_iter = person_t::list().where(person_t::expr::person_id == person_id).begin();
    if (person_iter == person_t::list().end())
    {
        return false;
    }

    person = *person_iter;  
    return true;
}

bool get_room(uint64_t room_id, room_t& room)
{
    auto room_iter = room_t::list().where(room_t::expr::room_id == room_id).begin();
    if (room_iter == room_t::list().end())
    {
        return false;
    }

    room = *room_iter;  
    return true;
}

bool get_building(uint64_t building_id, building_t& building)
{
    auto building_iter = building_t::list().where(building_t::expr::building_id == building_id).begin();
    if (building_iter == building_t::list().end())
    {
        return false;
    }

    building = *building_iter;  
    return true;
}

void add_scan(const json &j)
{
    using namespace enums::scan_table;
    gaia::db::begin_transaction();

    auto scan_w = scan_writer();
    if (j["scan_type"] == "badge")
    {
        scan_w.scan_type = e_scan_type::badge;
    } 
    else if (j["scan_type"] == "vehicle_entering")
    {
        scan_w.scan_type = e_scan_type::vehicle_entering;
    } 
    else if (j["scan_type"] == "vehicle_departing")
    {
        scan_w.scan_type = e_scan_type::vehicle_departing;
    } 
    else if (j["scan_type"] == "joining_wifi")
    {
        scan_w.scan_type = e_scan_type::joining_wifi;
    } 
    else if (j["scan_type"] == "leaving_wifi")
    {
        scan_w.scan_type = e_scan_type::leaving_wifi;
    } 
    else if (j["scan_type"] == "face")
    {
        scan_w.scan_type = e_scan_type::face;
    } 
    else if (j["scan_type"] == "leaving")
    {
        scan_w.scan_type = e_scan_type::leaving;
    } 

    scan_t new_scan = scan_t::get(scan_w.insert_row());

    person_t person;
    if (get_person(j["person_id"], person))
    {
        person.seen_who_scan_list().insert(new_scan);
    }

    room_t room;
    if (!j["room_id"].is_null() && get_room(j["room_id"], room))
    {
        room.seen_in_scan_list().insert(new_scan);
        room.inside_building().seen_at_scan_list().insert(new_scan);
    }

    building_t building;
    if (!j["building_id"].is_null() && get_building(j["building_id"], building))
    {
        building.seen_at_scan_list().insert(new_scan);
    }

    gaia::db::commit_transaction();
}

void process_ui_message(const std::string &message)
{
    auto j = json::parse(message);

    if (j["database"] == "reset")
    {
        gaia::db::begin_transaction();
        clear_all_tables();
        populate_all_tables();
        gaia::db::commit_transaction();
    }
    else if (j.contains("scan"))
    {
        add_scan(j["scan"]);
    }
    else if (j.contains("time"))
    {
        helpers::set_time(j["time"]);
    }

    update_ui();
}

int main()
{
    std::cout << "Access Control application" << std::endl;

    signal(SIGINT, exit_callback);

    gaia::system::initialize();

    gaia::db::begin_transaction();
    clear_all_tables();
    populate_all_tables();
    gaia::db::commit_transaction();

    ui_messaging::init(process_ui_message);

    std::cout << "System ready (Ctrl+C to exit)." << std::endl;

    std::string user_input_line = "";
    while (true);
    exit_callback(EXIT_SUCCESS);
}