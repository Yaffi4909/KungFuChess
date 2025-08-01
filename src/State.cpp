// #include "../../It1_interfaces/State.hpp"

// State::State(Moves moves_, Graphics graphics_, Physics physics_)
//     : moves(std::move(moves_)),
//       graphics(std::move(graphics_)),
//       physics(std::move(physics_))
// {
//     // TODO: אתחול נוסף במידת הצורך
// }

// void State::set_transition(const std::string& event, std::shared_ptr<State> target) {
//     transitions[event] = target;
// }

// void State::reset(const Command& cmd) {
//     // TODO: לאפס רכיבי state עם הפקודה
//     current_command = cmd;
//     physics.reset(cmd);
//     graphics.reset(cmd);
// }

// bool State::can_transition(long long now_ms) const {
//     // TODO: להגדיר תנאי מעבר מותאם
//     return false;
// }

// std::shared_ptr<State> State::get_state_after_command(const Command& cmd,
//                                                       long long now_ms) {
//     // TODO: להחליט לאן לעבור לפי הפקודה
//     // כאן אפשר לבדוק transitions ולהחזיר אחד מהם
//     auto it = transitions.find(cmd.type);
//     if (it != transitions.end()) {
//         return it->second;
//     }
//     return nullptr;
// }

// std::shared_ptr<State> State::update(long long now_ms) {
//     // TODO: לעדכן רכיבי מצב ולבדוק מעבר אוטומטי
//     physics.update(now_ms);
//     graphics.update(now_ms);
//     // החזר מצב חדש אם צריך, או nullptr אם אין מעבר
//     return nullptr;
// }

// Command State::get_command() const {
//     return current_command;
// }
