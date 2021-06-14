#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "ui_messaging.hpp"

#include "gaia/system.hpp"

namespace ui_messaging
{

const std::string &read_path = "/tmp/pipe-to-gaia";
const std::string &write_path = "/tmp/pipe-from-gaia";

message_callback_t message_callback = nullptr;
std::thread *worker = nullptr;

void flush_pipe()
{
    std::ifstream ifile{read_path};
    std::string line;
    while (std::getline(ifile, line))
    {
        // do nothing
    }    
}

void process_pipe()
{
    gaia::system::initialize();

    flush_pipe();

    while (true)
    {
        std::ifstream ifile{read_path};
        std::string line;
        while (std::getline(ifile, line))
        {
            message_callback(line);
        }
    }
}

void init(message_callback_t cb)
{
    if (!cb)
    {
        return;
    }
        
    message_callback = cb;
    worker = new std::thread(&process_pipe);
}

void send(const std::string &message)
{
    std::ofstream ofile{write_path};
    
    ofile << message << std::endl;
}

void send_alert(const std::string &message)
{
    std::string json = "{ \"alert\" : \"";
    json += message;
    json += "\" }";
    send(json);
}

} // namespace ui_messaging
