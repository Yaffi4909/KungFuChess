#include "../../It1_interfaces/Physics.hpp"

Physics::Physics(std::pair<int, int> start_cell_, const Board &board_, float speed_m_s_)
    : board(board_)
//   ,speed_m_s(speed_m_s_)
{
    start_cell = start_cell_; // תא התחלתי (row, col)
    // TODO: אתחול נוסף אם צריך
}

// void Physics::reset(const Command &cmd)
// {
//     // נוודא שיש לפחות 2 פרמטרים
//     const auto &params = cmd.get_params();
//     if (params.size() >= 2)
//     {
//         try
//         {
//             int row = std::stoi(params[0]);
//             int col = std::stoi(params[1]);
//             start_cell = {row, col}; // המרת פיקסלים לתאים, נניח 8 תאים בשורה
//         }
//         catch (const std::exception &e)
//         {
//             std::cerr << "Invalid parameters in Command for reset: " << e.what() << std::endl;
//         }
//     }
//     else
//     {
//         std::cerr << "Not enough parameters in Command to reset Physics." << std::endl;
//     }
// }

// void Physics::update(int now_ms)
// {
//     // TODO: עדכון מיקום לפי הזמן והמהירות
// }

// bool Physics::can_be_captured() const
// {
//     // TODO: בדיקת אפשרות להיתפס
//     return false;
// }

// bool Physics::can_capture() const
// {
//     // TODO: בדיקת אפשרות ללכוד
//     return false;
// }

std::pair<int, int> Physics::get_pos() const
{
    // std::cout << "Physics::get_pos() called, returning start_cell: ("
    //           << start_cell.first << ", " << start_cell.second << ")" << std::endl;
    return start_cell; // החזרת תא התחלתי, ניתן לשנות לפי הצורך
}

void Physics::set_pos(const std::pair<int,int>& pos)
{
    
    start_cell = pos; // עדכון תא התחלתי
} 
