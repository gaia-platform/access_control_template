#pragma once

namespace ui_messaging
{

typedef void (*message_callback_t)(const std::string &message);

void init(message_callback_t cb = nullptr);
void send(const std::string &message);
void send_alert(const std::string &message);

} // namespace ui_messaging
