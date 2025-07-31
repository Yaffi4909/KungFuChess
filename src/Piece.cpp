#include "../../It1_interfaces/Piece.hpp"
#include "../../It1_interfaces/State.hpp"
// #include "../../It1_interfaces/ScoreManager.hpp"
// #include "img.hpp"
// #include <opencv2/opencv.hpp>
// #include <iostream>
// #include <tuple>

Piece::Piece(const std::string &piece_id_,const State &init_state)
    : id(piece_id_), state(init_state), captured(false)
{
    // TODO: אתחול נוסף אם נדרש
}

// void Piece::on_command(const Command &cmd, int now_ms)
// {
//     // TODO: טיפול בפקודה עבור החתיכה
// }

void Piece::reset(const Command &cmd, std::vector<std::shared_ptr<Piece>> &pieces)
{
    // std::cout << "1" << std::endl;

    // if it is Move
    int r, c, new_r, new_c;
    // command params
    try
    {
        r = std::stoi(cmd.get_params()[0]);
        c = std::stoi(cmd.get_params()[1]);
        new_r = std::stoi(cmd.get_params()[2]);
        new_c = std::stoi(cmd.get_params()[3]);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error parsing command parameters: " << e.what() << std::endl;
        return; // או טיפול אחר בשגיאה
    }

    int available_move = 0;
    std::shared_ptr<Piece> piece = nullptr;
    std::tuple<int, int, std::string> move;
    // בודק אם הפסיעה קיימת לחייל
    if (state.get_moves().get_moves(r, c).size() == 0)
    {
        std::cout << "No moves available for piece " << id << " at (" << r << ", " << c << ")" << std::endl;
        return;
         // יציאה אם אין מהלכים חוקיים
    }
    // for (auto &&i : state.get_moves().get_moves(r, c))
    // {
    //     // מוצא את הפקודה - אם היא חוקית
    //     if (std::get<0>(i) == new_r && std::get<1>(i) == new_c)
    //     {
    //         move = i;
    //         available_move = 1;
    //         break;
    //     }
    // }
    // std::cout << "id.substr(0, 1) " << id.substr(0, 1) << std::endl;
    // if (!available_move)
    // {
    //     std::cout << "Illegal move for piece " << id << " from (" << r << ", " << c << ") to (" << new_r << ", " << new_c << ")" << std::endl;
    //     return; // יציאה אם הפסיעה לא חוקית
    // }
    // for (auto &&p : pieces)
    // {
    //     // במדיקה מי החייל שנמצא בתא המבוקש אם בכלל
    //     if (p->get_pos() == std::make_pair(new_r, new_c) && p->get_id() != id)
    //     {
    //         piece = p;
    //         break;
    //     }
    // }
    // // אם יש חייל יריב אחר בתא החדש, נרצה להרוג אותו וכן אם זה מהלך של חיסול
    // if (piece && piece->get_id().substr(1, 1) != id.substr(1, 1) && !piece->is_captured() && (std::get<2>(move) == "" || std::get<2>(move) == "capture"))
    // {
    //     // כל מקרי החיסול
    //     piece->mark_as_captured();
    //     ScoreManager::get_instance().on_piece_killed(piece->get_id());
    //     std::cout << "Piece " << piece->get_id() << " at (" << new_r << ", " << new_c << ") is captured by " << id << std::endl;
    //     // הפעלת סאונד //     ../../sounds/gun.wav

    //     std::cout << "sound ????????????????????????????????????????????????????????????????? " << id << std::endl;

    //     // ה - הריגה
    //     piece->captured = true;
    // }

    // long long current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
    //                              std::chrono::steady_clock::now().time_since_epoch())
    //                              .count();

    // start_move_animation({new_r, new_c}, current_time);
    this->state.get_physics().set_pos({new_r, new_c});
}

void Piece::update(int now_ms)
{
    // TODO: עדכון מצב לפי זמן
}

void Piece::draw_on_board(std::shared_ptr<Board> &board, int now_ms)
{
    if (captured) return;

    // Graphics &graphics = state.get_graphics();
    // Img sprite = graphics.get_img_by_id(id);

    Img sprite = state.get_graphics().get_img_by_id(id);

    long long current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::steady_clock::now().time_since_epoch())
                                 .count();
    
    std::pair<double, double> pixel_pos = get_visual_pixel_position(current_time);
    
    int x = (int)pixel_pos.second + 5;  // col * cell_width
    int y = (int)pixel_pos.first + 5;   // row * cell_height

    sprite.draw_on(board->img, x, y);
}

// void Piece::mark_as_captured()
// {
//     captured = true;
// }

bool Piece::is_captured() const
{
    return captured;
}

// bool Piece::is_available_move(int r, int c, int r1, int c1, int new_r, int new_c, std::vector<std::shared_ptr<Piece>> &pieces)
// {
//     if (r == new_r && c == new_c)
//     {
//         return true;
//     }
//     std::shared_ptr<Piece> &current_place = Piece::get_piece_by_pos(r, c, pieces);

//     if (current_place != nullptr && (r != r1 || c != c1))
//     {
//         return false;
//     }
//     if (r > new_r)
//     {
//         r--;
//     }
//     else if (r < new_r)
//     {
//         r++;
//     }

//     if (c > new_c)
//     {
//         c--;
//     }
//     else if (c < new_c)
//     {
//         c++;
//     }
//     return is_available_move(r, c, r1, c1, new_r, new_c, pieces);
// }

// std::shared_ptr<Piece> Piece::get_piece_by_pos(int r, int c, std::vector<std::shared_ptr<Piece>> &pieces)
// {
//     for (auto &&p : pieces)
//     {
//         if (p->get_pos() == std::make_pair(r, c) && !p->is_captured())
//         {
//             return p;
//         }
//     }
//     return nullptr;
// }

// void Piece::move_next_pos(Piece *old_piece, std::pair<int, int> new_pos)
// {

//     std::cout << "move_next_pos " << std::endl;

//     // old_piece->erase_from_board(this->get_state().get_physics().get_board(), 1);
//     old_piece->get_state().get_physics().set_pos(new_pos);
//     old_piece->captured = false;
//     // old_piece->draw_on_board(this->get_state().get_physics().get_board(), 1);

//     std::cout << "move_next_pos seccesfuly " << std::endl;
// }

// Animation
std::pair<double, double> Piece::get_visual_pixel_position(long long current_time)
{
    if (move_animation.is_active())
    {
        std::pair<int, int> pixel_pos = move_animation.get_current_position(current_time);
        // if(pixel_pos == move_animation.get_end_pos()){
        //     this->state.get_graphics().set_action("long_rest");
        // }
        if (move_animation.is_finished(current_time))
        {
            int cell_width = 60;
            int cell_height = 60;
            std::pair<int, int> final_cell = {pixel_pos.first / cell_height, pixel_pos.second / cell_width};
            state.get_physics().set_pos(final_cell);
            move_animation.stop();
        }
        
        return {static_cast<double>(pixel_pos.first), static_cast<double>(pixel_pos.second)};
    }
    
    std::pair<int, int> cell = state.get_physics().get_pos();
    return {static_cast<double>(cell.first * 60), static_cast<double>(cell.second * 60)};
}

// std::pair<int, int> Piece::get_visual_position(long long current_time)
// {
//     if (move_animation.is_active())
//     {
//         std::pair<int, int> pixel_pos = move_animation.get_current_position(current_time);
        
//         // המר חזרה לתאים
//         int cell_width = 60;
//         int cell_height = 60;
//         std::pair<int, int> cell_pos = {pixel_pos.first / cell_height, pixel_pos.second / cell_width};
        
//         if (move_animation.is_finished(current_time))
//         {
//             state.get_physics().set_pos(cell_pos);
//             move_animation.stop();
//         }
        
//         return cell_pos;
//     }
//     return state.get_physics().get_pos();
// }

// void Piece::start_move_animation(std::pair<int, int> target, long long current_time)
// {
//     std::pair<int, int> current_pos = state.get_physics().get_pos();
    
//     // המר לפיקסלים
//     int cell_width = 60;  // או הערך הנכון מהלוח
//     int cell_height = 60; // או הערך הנכון מהלוח
    
//     std::pair<int, int> start_pixels = {current_pos.first * cell_height, current_pos.second * cell_width};
//     std::pair<int, int> end_pixels = {target.first * cell_height, target.second * cell_width};
    
//     move_animation.start(start_pixels, end_pixels, 200, current_time);
// }

