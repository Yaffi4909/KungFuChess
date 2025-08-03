#include "../../It1_interfaces/Animation.hpp"
#include <algorithm>

Animation::Animation() : active(false), duration_ms(0), start_time(0) {}

Animation::Animation(std::pair<int, int> from, std::pair<int, int> to, int duration)
    : start_pos({(double)from.first, (double)from.second}),
      end_pos({(double)to.first, (double)to.second}),
      duration_ms(duration),
      active(false) {}

void Animation::start(std::pair<int, int> from, std::pair<int, int> to, int duration, long long current_time)
{
    start_pos = {(double)from.first, (double)from.second};
    end_pos = {(double)to.first, (double)to.second};
    duration_ms = duration;
    start_time = current_time;
    active = true;
}

std::pair<int, int> Animation::get_current_position(long long current_time)
{

    if (!active)
    {
        return {(int)end_pos.first, (int)end_pos.second};
    }

    long long elapsed = current_time - start_time;
    if (elapsed >= duration_ms) {
        active = false;
        return {(int)end_pos.first, (int)end_pos.second};
    }

    // חישוב אינטרפולציה ליניארית
    double progress = (double)elapsed / duration_ms;

    double current_x = start_pos.first + (end_pos.first - start_pos.first) * progress;
    double current_y = start_pos.second + (end_pos.second - start_pos.second) * progress;

    return {(int)current_x, (int)current_y};
}

bool Animation::is_finished(long long current_time)
{
    if (!active)
        return true;
    return (current_time - start_time) >= duration_ms;
}

void Animation::stop()
{
    active = false;
}
