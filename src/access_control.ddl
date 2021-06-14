create table if not exists building (
    building_id uint64,
    name string
);

create table if not exists room (
    room_id uint64,
    name string,
    capacity uint32,

    inside references building
);

create table if not exists person (
    person_id uint64,
    face_signature string,
    badge_number string,
    first_name string,
    last_name string,

    employee bool,
    visitor bool,
    stranger bool,
    badged bool,
    parked bool,
    credentialed bool,
    admissible bool,
    on_wifi bool,

    entry_time uint64,
    leave_time uint64,

    parked_in references building,

    entered references building,
    
    inside references room
);

create table if not exists permitted_room (
    permitted_room_id uint64,

    permittee references person,

    allowed_in references room
);

create table if not exists event (
    event_id string,
    name string,
    start_timestamp uint64,
    end_timestamp uint64,

    held_in references room
);

create table if not exists registration (
    registration_id string,
    timestamp uint64,

    registered references person,

    occasion references event
);

create table if not exists vehicle (
    vehicle_id uint64,
    make string,
    model string,
    year string,
    license_state string,
    license string,
    parked_time uint64,

    vehicle_owner references person
);

create table if not exists scan (
    scan_id uint64,
    scan_type uint8,
    badge_scan bool,
    vehicle_entering bool,
    vehicle_departing bool,
    joining_wifi bool,
    leaving_wifi bool,
    face_scan bool,
    leaving bool,
    badge_id string,
    face_signature string,
    license string,
    timestamp uint64,

    seen_at references building,

    seen_in references room,

    seen_who references person,

    seen_license references vehicle
);
