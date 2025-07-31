#include <string>
#include <vector>
#include <iostream>
#include "../../It1_interfaces/Command.hpp"

// מבנה Command
Command::Command(int ts, const std::string &id, const std::string &t, const std::vector<std::string> &p)
    : timestamp_ms(ts), piece_id(id), type(t), params(p){}

// Getters
int Command::get_timestamp() const
{
    return Command::timestamp_ms;
}
const std::string &Command::get_piece_id() const
{
    std::cout << "Command::get_piece_id(): " << piece_id << std::endl;
    return piece_id;
}
const std::string &Command::get_type() const
{
    return Command::type;
}
const std::vector<std::string> &Command::get_params() const
{
    return Command::params;
}

// Setters
void Command::set_timestamp(int ts)
{
    Command::timestamp_ms = ts;
}
void Command::set_piece_id(const std::string &id)
{
    Command::piece_id = id;
}
void Command::set_type(const std::string &t)
{
    Command::type = t;
}
void Command::set_params(const std::vector<std::string> &p)
{
    Command::params = p;
}
