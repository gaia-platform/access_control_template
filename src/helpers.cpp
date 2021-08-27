#include <chrono>
#include <limits>
#include <random>
#include <vector>

#include "helpers.hpp"

void helpers::park_in_building(
    gaia::common::gaia_id_t person_id,
    gaia::common::gaia_id_t building_id)
{
    auto person = gaia::access_control::person_t::get(person_id);
    auto building = gaia::access_control::building_t::get(building_id);

    person.registrations();

    building.parked_people().insert(person);
}

bool helpers::person_has_registrations(gaia::common::gaia_id_t person_id)
{
    auto person = gaia::access_control::person_t::get(person_id);
    auto registration_list = person.registrations();

    return (registration_list.begin() != registration_list.end());
}

bool helpers::person_has_event_now(gaia::common::gaia_id_t person_id)
{
    auto person = gaia::access_control::person_t::get(person_id);

    for(auto registration : person.registrations())
    {
        auto event = registration.occasion();
        if (event && time_is_between(get_time_now(),
            event.start_timestamp(), event.end_timestamp()))
        {
            return true;
        }
    }
    return false;
}

bool helpers::person_has_event_now(
    gaia::common::gaia_id_t person_id,
    gaia::access_control::room_t room)
{
    auto person = gaia::access_control::person_t::get(person_id);

    for(auto registration : person.registrations())
    {
        auto event = registration.occasion();
        if((!room || room == event.held_in_room())
            && event && time_is_between(get_time_now(),
            event.start_timestamp(), event.end_timestamp()))
        {
            return true;
        }
    }
    return false;
}

static std::random_device rand_device;
static std::mt19937 rand_num_gen{rand_device()};
static std::uniform_int_distribution<uint64_t> distribution(
    std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());

void helpers::allow_person_into_room(
    gaia::common::gaia_id_t person_id,
    gaia::access_control::room_t room)
{
    uint64_t permitted_room_id = distribution(rand_num_gen);
    auto permitted_room = gaia::access_control::permitted_room_t::insert_row(permitted_room_id);

    auto person = gaia::access_control::person_t::get(person_id);
    // Connect permitted_room to person.
    person.permitted_in().insert(permitted_room);
    // Connect permitted_room to room.
    room.permissions().insert(permitted_room);
}

void helpers::disconnect_parked_buildings(gaia::access_control::vehicle_t vehicle)
{
    auto vehicle_owner = vehicle.owner();
    auto parking_building = vehicle_owner.parked_in();

    parking_building.parked_people().remove(vehicle_owner);
}

void helpers::disconnect_person_from_room(gaia::common::gaia_id_t person_id)
{
    auto person = gaia::access_control::person_t::get(person_id);
    if (person.inside_room()) {
        person.inside_room().people_inside().remove(person);
    }
}

void helpers::disconnect_person_from_building(gaia::common::gaia_id_t person_id)
{
    auto person = gaia::access_control::person_t::get(person_id);
    if (person.entered_building()) {
        person.entered_building().people_entered().remove(person);
    }
}

void helpers::delete_their_room_permissions(gaia::common::gaia_id_t person_id)
{
    auto person = gaia::access_control::person_t::get(person_id);
    std::vector<gaia::access_control::permitted_room_t> permitted_rooms;
    for(auto permitted_room : person.permitted_in())
    {
        permitted_rooms.push_back(permitted_room);
    }

    person.permitted_in().clear();
    for(auto permitted_room : permitted_rooms)
    {
        permitted_room.delete_row();
    }
}

void helpers::let_them_in(
    gaia::common::gaia_id_t person_id,
    gaia::common::gaia_id_t scan_id)
{
    auto person = gaia::access_control::person_t::get(person_id);
    auto scan = gaia::access_control::scan_t::get(scan_id);

    if (scan.seen_in_room())
    {
        disconnect_person_from_room(person_id);
        scan.seen_in_room().people_inside().insert(person);
    }
    if (scan.seen_at_building())
    {
        scan.seen_at_building().people_entered().insert(person);
    }
}

gaia::access_control::person_t helpers::insert_stranger(std::string face_signature)
{
    auto stranger_w = gaia::access_control::person_writer();
    stranger_w.stranger = true;
    stranger_w.face_signature = face_signature;

    return gaia::access_control::person_t::get(stranger_w.insert_row());
}

void helpers::insert_stranger_vehicle(
    gaia::access_control::person_t stranger, std::string license)
{
    auto vehicle_w = gaia::access_control::vehicle_writer();
    vehicle_w.license = license;
    // Connect the vehicle to its owner, the stranger.
    stranger.vehicles().insert(vehicle_w.insert_row());
}

// Time-related helpers:

uint64_t g_current_time = 0;

void helpers::set_time(uint64_t time)
{
    g_current_time = time;
}

uint64_t helpers::get_time_now()
{
    return g_current_time;
}

bool helpers::time_is_between(uint64_t time,
    uint64_t low_time, uint64_t high_time)
{
    return (low_time <= time) && (time <= high_time);
}