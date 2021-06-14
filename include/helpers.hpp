#pragma once

#include <string>

#include "gaia_access_control.h"

namespace helpers
{

void park_in_building(
    gaia::common::gaia_id_t person_id,
    gaia::common::gaia_id_t building_id);

bool person_has_registrations(gaia::common::gaia_id_t person_id);

bool person_has_event_now(gaia::common::gaia_id_t person_id);

bool person_has_event_now(
    gaia::common::gaia_id_t person_id,
    gaia::access_control::room_t room);

void allow_person_into_room(
    gaia::common::gaia_id_t person_id,
    gaia::access_control::room_t room);

void disconnect_parked_buildings(gaia::access_control::vehicle_t vehicle);

void disconnect_person_from_room(gaia::common::gaia_id_t person_id);
void disconnect_person_from_building(gaia::common::gaia_id_t person_id);

void delete_their_room_permissions(gaia::common::gaia_id_t person_id);

void let_them_in(
    gaia::common::gaia_id_t person_id,
    gaia::common::gaia_id_t scan_id);

gaia::access_control::person_t insert_stranger(std::string face_signature);

void insert_stranger_vehicle(
    gaia::access_control::person_t stranger, std::string license);

// Time-related helpers:

void set_time(uint64_t time);

uint64_t get_time_now();

bool time_is_between(uint64_t time, uint64_t low_time, uint64_t high_time);

} // namespace helpers
