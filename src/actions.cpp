#include "actions.hpp"

#include "ui_messaging.hpp"

#include "gaia/logger.hpp"

void actions::stranger_detected()
{
    gaia_log::app().info("Stranger detected!");
    ui_messaging::send_alert("Stranger detected");
}

void actions::no_eligible_events(uint64_t person_id)
{
    gaia_log::app().info("No eligible events for visitor #{}.", person_id);
    ui_messaging::send_alert("No eligible events for visitor");
}

void actions::base_credentials_required(uint64_t person_id)
{
    gaia_log::app().info("Base credentials required for person #{} to enter.", person_id);
    ui_messaging::send_alert("Base credentials required to enter");
}

void actions::no_entry_right_now(
    uint64_t person_id, std::string room_name, std::string building_name)
{
    gaia_log::app().info("No entry right now for person #{} into room {}, building {}.",
        person_id, room_name, building_name);
    ui_messaging::send_alert("Entry not currently allowed");
}

void actions::not_this_building(uint64_t person_id, std::string building_name)
{
    gaia_log::app().info("Person #{} is not allowed into building {}.",
        person_id, building_name);
    ui_messaging::send_alert("Entry into building not allowed");
}

void actions::not_this_room(
    uint64_t person_id, std::string room_name, std::string building_name)
{
    gaia_log::app().info("Person #{} is not allowed into room {}, building {}.",
        person_id, room_name, building_name);
    ui_messaging::send_alert("Entry into room not allowed");
}