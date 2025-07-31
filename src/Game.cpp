#include "../../It1_interfaces/Game.hpp"
#include "../../It1_interfaces/Piece.hpp"
#include "../../It1_interfaces/Board.hpp"
#include "../../It1_interfaces/Command.hpp"
#include "../../It1_interfaces/ScoreManager.hpp"
#include "../../It1_interfaces/PubSubQueue.hpp"
#include "../../It1_interfaces/Moves.hpp"
#include <opencv2/opencv.hpp> // בשביל cv::destroyAllWindows, cv::imshow וכו'
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <chrono>
#include <sstream>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

namespace fs = std::filesystem;
std::vector<std::shared_ptr<Piece>> Game::pieces;

Game::Game(std::vector<std::shared_ptr<Piece>> &pieces_, std::shared_ptr<Board> board)
    : board(board)
{
    set_pieces(pieces_);

    // אתחול ScoreManager
    ScoreManager::get_instance();
    // אתחול מאזין ל bub sub
    PubSubQueue::instance().subscribe("changeScore", [](const std::string &topic, const std::string &payload)
                                      {
                                          std::cout << topic << ", " << payload << std::endl;
                                          ScoreManager::get_instance().score_listener(topic, payload);
                                          // לדוגמה: פיענוח payload ועדכון ציון
                                      });
    // זמן נוכחי (לצורך ציור האנימציות)
    int now_ms = 0; // או לקבל מה־Game::game_time_ms()

    // ציור כל החיילים שלא נמחקו על הלוח
    draw_pieces_on_board(pieces_, board, now_ms);

    std::cout << "Game initialized with " << pieces.size() << " pieces." << std::endl;
}

long long Game::game_time_ms() const
{
    // מחזיר זמן מונוטוני במילישניות
    auto now = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return ms.count();
}

// Board Game::clone_board()
// {
//     // יוצרת עותק חדש של הלוח
//     Board b = board->clone();
//     return b;
// }

// // נניח שהגדרת הלוח היא:
// const int CELL_SIZE = 60;
// const int BOARD_SIZE = 8;

// void Game::start_user_input_thread()
// {
//     std::cout << "Starting user input thread..." << std::endl;

//     std::thread([this]()
//                 {
//                     cv::namedWindow("ChessBoard");

//                     // Callback של העכבר
//                     cv::setMouseCallback("ChessBoard", [](int event, int x, int y, int, void *userdata)
//                                          {
//             if (event != cv::EVENT_LBUTTONDOWN) return;

//             Game* game = static_cast<Game*>(userdata);
//             int col = x / CELL_SIZE;
//             int row = y / CELL_SIZE;

//             if (row >= BOARD_SIZE || col >= BOARD_SIZE) return;

//             static int first_click_row = -1;
//             static int first_click_col = -1;
//             static bool waiting_for_second_click = false;

//             int now_ms = static_cast<int>(
//                 std::chrono::duration_cast<std::chrono::milliseconds>(
//                     std::chrono::system_clock::now().time_since_epoch()
//                 ).count()
//             );
//             std::string piece_id = game->get_piece_id_at(row, col);
//             std::cout << "Clicked on (" << row << ", " << col << ") - Piece ID: " << piece_id << std::endl;

//             std::cout << "waiting_for_second_click: " << waiting_for_second_click << std::endl;

//             if (piece_id == "none" && !waiting_for_second_click){
//                 return;
//             }
//             // if(piece_id != "none" && (get_piece_by_pos(row, col)->get_graphics().get_action() == "long_rest" || get_piece_by_pos(row, col)->get_graphics().get_action() == "short_rest")){
//             //     std::cout << "rest................" << std::endl;
//             //     return;
//             // }

//             if (waiting_for_second_click)
//             {
//                 std::cout << "Second click on (" << row << ", " << col << ")" << std::endl;
//                 bool is_valid = false;
//                 for (auto&& step : game->available_steps)
//                 {
//                     if (std::get<0>(step) == row && std::get<1>(step) == col)
//                         is_valid = true;
//                 }
//                 std::cout << "is_valid: " << is_valid << std::endl;
//                 if (is_valid)
//                 {
//                     std::string first_piece_id = game->get_piece_id_at(first_click_row, first_click_col);
//                     std::string type = (piece_id == first_piece_id) ? "Jump" : "Move";

//                     std::vector<std::string> params = {
//                         std::to_string(first_click_row), std::to_string(first_click_col),
//                         std::to_string(row), std::to_string(col)
//                     };

//                     Command cmd(now_ms, first_piece_id, type, params);

//                     std::lock_guard<std::mutex> lock(game->queue_mutex);
//                     std::cout << "running: " << game->running << std::endl;
//                     game->user_input_queue.push(cmd);

//                     game->available_steps.clear();
//                 }
//                 else
//                 {
//                     waiting_for_second_click = false;
//                     // ביטול בחירה
//                     game->available_steps.clear();
//                 }
//             }

//             if (!waiting_for_second_click){
//                 std::cout << " -> First click on (" << row << ", " << col << ")" << std::endl;
//                 game->available_steps = game->get_all_available_steps(row, col);
//             }

//             waiting_for_second_click = !waiting_for_second_click;
//             first_click_row = row;
//             first_click_col = col;
//             game->last_clicked_row = row;
//             game->last_clicked_col = col; }, this);

//                     // 🔁 לולאה גרפית שפועלת כל עוד running == true
//                     while (running)
//                     { // ציור הלוח

//                         // צור תמונה חדשה עם הלוח הנקי + כל החיילים
//                         cv::Mat board_img = board->img.get_mat().clone();

//                         // צייר את כל החיילים על התמונה
//                         for (const auto &p : pieces)
//                         {
//                             if (!p->is_captured())
//                             {
//                                 // קבל את מיקום החייל
//                                 // std::pair<int, int> pos = p->get_pos();
//                                 long long current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
//                                                              std::chrono::steady_clock::now().time_since_epoch())
//                                                              .count();

//                                 std::pair<double, double> pixel_pos = p->get_visual_pixel_position(current_time);
//                                 int x = (int)pixel_pos.second + 5; // ישירות פיקסלים
//                                 int y = (int)pixel_pos.first + 5;  // ישירות פיקסלים

//                                 // צייר את החייל על board_img

//                                 // Graphics &graphics = p->get_state().get_graphics();
//                                 // Img sprite = graphics.get_img_by_id(p->get_id());

//                                 Img sprite = p->get_graphics().get_img_by_id(p->get_id());

//                                 // צייר ישירות על cv::Mat
//                                 cv::Mat sprite_mat = sprite.get_mat();
//                                 if (!sprite_mat.empty() && x >= 0 && y >= 0 &&
//                                     x + sprite_mat.cols <= board_img.cols &&
//                                     y + sprite_mat.rows <= board_img.rows)
//                                 {
//                                     // וודא שיש אותו מספר ערוצים
//                                     cv::Mat sprite_converted;
//                                     if (sprite_mat.channels() != board_img.channels())
//                                     {
//                                         if (sprite_mat.channels() == 4 && board_img.channels() == 3)
//                                         {
//                                             cv::cvtColor(sprite_mat, sprite_converted, cv::COLOR_BGRA2BGR);
//                                         }
//                                         else if (sprite_mat.channels() == 3 && board_img.channels() == 4)
//                                         {
//                                             cv::cvtColor(sprite_mat, sprite_converted, cv::COLOR_BGR2BGRA);
//                                         }
//                                         else
//                                         {
//                                             sprite_converted = sprite_mat;
//                                         }
//                                     }
//                                     else
//                                     {
//                                         sprite_converted = sprite_mat;
//                                     }

//                                     sprite_converted.copyTo(board_img(cv::Rect(x, y, sprite_converted.cols, sprite_converted.rows)));
//                                 }
//                             }
//                         }

//                         for (const auto &step : available_steps)
//                         {
//                             int r = std::get<0>(step);
//                             int c = std::get<1>(step);

//                             cv::rectangle(
//                                 board_img,
//                                 cv::Rect(c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE, CELL_SIZE),
//                                 cv::Scalar(0, 128, 255), 2);
//                         }

//                         if (last_clicked_row >= 0 && last_clicked_col >= 0)
//                         {
//                             cv::rectangle(
//                                 board_img,
//                                 cv::Rect(last_clicked_col * CELL_SIZE, last_clicked_row * CELL_SIZE, CELL_SIZE, CELL_SIZE),
//                                 cv::Scalar(0, 0, 255), 2);
//                         }
//                         if (!is_win())
//                             cv::imshow("ChessBoard", board_img);

//                         // זמן המתנה בין ציורים
//                         if (cv::waitKey(10) == 27) // Esc
//                             break;
//                     }

//                     std::cout << "User input thread finished." << std::endl;
//                     // cv::destroyWindow("ChessBoard");
//                 })
//         .detach(); // ← detach אם אין צורך join
// }

void Game::run()
{

    std::cout << "Game started!" << std::endl;
    // Board :: img.draw_on(Img& other_img, int x, int y)// הצגת הלוח הראשוני
    // start_user_input_thread();
    std::cout << "                                     ///////////////////////////continue run!" << std::endl;
    long long start_ms = game_time_ms();
    for (auto &p : pieces)
    {
        if (!p->is_captured())
        {
            p->update(start_ms); // עדכון כל החיילים עם הזמן הנוכחי

        } // אם הכלי לא נלכד
    }

    while (running)
    {
        long long now = game_time_ms();

        // (1) update physics & animations
        for (auto &p : pieces)
        {
            if (!p->is_captured()) // אם הכלי לא נלכד
            {
                (*p).update(now);
            }
        }
        // (2) handle queued Commands
        while (!user_input_queue.empty())
        {
            std::cout << "treat cmd" << std::endl;
            Command cmd = user_input_queue.front();
            user_input_queue.pop();
            process_input(cmd);
        }

    //     // (3) draw current position
    //     if (!show())
    //     {
    //         break;
    //     }
    //     if (this->is_win())
    //     {
    //         std::cout << "1" << std::endl;
    //         announce_win();
    //         std::cout << "2" << std::endl;

    //         std::this_thread::sleep_for(std::chrono::seconds(5)); // בשניות
    //         std::cout << "3" << std::endl;
    //         running = false; // עצור את ה-thread של input לפני שמציגים הודעה
    //     }
    //     // (4) detect captures - במקרה של התנגשות
    //     // resolve_collisions();
    }
    std::cout << "4" << std::endl;

    running = false; // עצור את ה-thread של input לפני שמציגים הודעה
    // std::this_thread::sleep_for(std::chrono::seconds(3)); // המתן שה-thread יסיים
    cv::destroyAllWindows(); // רק אחרי שהמשתמש לחץ

    // announce_win(); // הצג את הודעת הניצחון
}

// השמה ראשונית של כל החיילים על הלוח
void Game::draw_pieces_on_board(const std::vector<std::shared_ptr<Piece>> &pieces, std::shared_ptr<Board> &board, int now_ms)
{
    if (this->is_win())
        return;
    try
    {
        board->img.read(board->path, {board->cell_W_pix, board->cell_H_pix}, true);
        for (const auto &p : pieces)
        {
            if (!p->is_captured()) // אם הכלי לא נלכד
            {
                p->draw_on_board(board, now_ms); // ציור הכלי על הלוח
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error drawing pieces on board: " << e.what() << std::endl;
    }
}

// // ─── מתודות פנימיות ────────────────────────────────
// void Game::draw()
// {
//     // יצירת עותק של הלוח
//     Board current = board->clone();
//     int now_ms = static_cast<int>(game_time_ms());

//     for (const auto &p : pieces)
//     {
//         if (!p->is_captured()) // אם הכלי לא נלכד
//         {
//             // ציור החייל על הלוח הנוכחי
//             p->draw_on_board(std::make_shared<Board>(current), now_ms);
//         }
//     }

//     // current.img.show(); // הצגת התמונה
// }

// bool Game::show()
// {

//     return (*board).img.is_loaded();
// }

// // התנגשויות בין כלים
// void Game::resolve_collisions()
// {
//     // מפה של מיקום -> כל הכלים שנמצאים שם
//     std::map<std::pair<int, int>, std::vector<std::shared_ptr<Piece>>> cell_map;

//     // מעבר על כל הכלים והוספתם למפה
//     for (const auto &p : pieces)
//     {
//         if (!p->is_captured()) // אם הכלי נלכד, לא נרצה לבדוק אותו
//         {
//             cell_map[p->get_pos()].push_back(p);
//         }
//     }

//     // אם יש יותר מאחד בתא – אחד שורד
//     for (const auto &[cell, piece_list] : cell_map)
//     {
//         if (piece_list.size() > 1)
//         {
//             // נשאיר את הראשון, נמחק את השאר
//             for (size_t i = 1; i < piece_list.size(); ++i)
//             {

//                 ScoreManager::get_instance().on_piece_killed(piece_list[i]->get_id());
//                 // הסרת הכלי מהמשחק

//                 piece_list[i]->mark_as_captured();
//             }
//         }
//     }
// }

bool Game::is_win() const
{
    if (ScoreManager::get_instance().get_score().first >= 100)
    {
        // std::cout << "White wins!" << std::endl;
        return true;
    }
    else if (ScoreManager::get_instance().get_score().second >= 100)
    {
        // std::cout << "Black wins!" << std::endl;
        return true;
    }
    return false;
}

// void Game::announce_win()
// {

//     std::string message;
//     auto score = ScoreManager::get_instance().get_score();
//     if (score.first >= 100)
//     {
//         message = "White Wins!!!";
//     }
//     else if (score.second >= 100)
//     {
//         message = "Black Wins!!!";
//     }
//     else
//     {
//         message = "The game is finished.";
//     }

//     std::cout << "Announcing winner: " << message << std::endl;

//     // שיכפול תמונת הלוח
//     cv::Mat board_img = board->img.get_mat().clone();

//     // === הוספת קונפטי ===
//     for (int i = 0; i < 300; ++i)
//     {
//         cv::Point pt(rand() % board_img.cols, rand() % board_img.rows);
//         cv::Scalar color(rand() % 256, rand() % 256, rand() % 256); // צבע אקראי
//         int radius = rand() % 6 + 2;
//         cv::circle(board_img, pt, radius, color, cv::FILLED);
//     }

//     // === הוספת הכתר ===
//     cv::Mat crown = cv::imread("crown.png", cv::IMREAD_UNCHANGED); // עם שקיפות
//     if (!crown.empty())
//     {
//         // הקטנה לגודל מתאים (20%)
//         cv::Mat resized_crown;
//         double scale = 0.2;
//         cv::resize(crown, resized_crown, cv::Size(), scale, scale);

//         // מיקום הכתר (למעלה במרכז)
//         int x = board_img.cols / 2 - resized_crown.cols / 2;
//         int y = board_img.rows / 10;

//         // הדבקה תוך שימוש בערוץ שקיפות
//         std::vector<cv::Mat> channels;
//         cv::split(resized_crown, channels);
//         if (channels.size() == 4)
//         {
//             cv::Mat mask = channels[3]; // alpha
//             cv::Mat crown_rgb;
//             cv::merge(std::vector<cv::Mat>{channels[0], channels[1], channels[2]}, crown_rgb);
//             crown_rgb.copyTo(board_img(cv::Rect(x, y, resized_crown.cols, resized_crown.rows)), mask);
//         }
//     }

//     // === ציור רקע שקוף להודעה ===
//     cv::Rect box(board_img.cols / 4, board_img.rows / 2, board_img.cols / 2, board_img.rows / 5);
//     cv::Mat overlay;
//     board_img.copyTo(overlay);
//     cv::rectangle(overlay, box, cv::Scalar(255, 255, 255), cv::FILLED);
//     double alpha = 0.7;
//     cv::addWeighted(overlay, alpha, board_img, 1.0 - alpha, 0, board_img);

//     // === כתיבת הודעה ===
//     int font = cv::FONT_HERSHEY_DUPLEX;
//     double fontScale = 1.2;
//     int thickness = 2;
//     int baseline = 0;

//     cv::Size textSize = cv::getTextSize(message, font, fontScale, thickness, &baseline);
//     cv::Point textOrg(
//         board_img.cols / 2 - textSize.width / 2,
//         board_img.rows / 2 + textSize.height / 2);
//     cv::putText(board_img, message, textOrg, font, fontScale, cv::Scalar(0, 0, 200), thickness);

//     // Moves::playMP3(L"applause.mp3");
//     // Sleep(5000); // תן ל-MP3 להתנגן 5 שניות
//     // mciSendStringW(L"close mp3", NULL, 0, NULL);

//     // === הצגה ===
//     cv::imshow("ChessBoard", board_img);
//     cv::waitKey(5000); // המתנה 5 שניות
// }

void Game::process_input(const Command &cmd)
{
    // איזה חייל ("BB", "KW")
    const std::string &target_id = cmd.get_piece_id();
    std::pair<int, int> old_pos = std::make_pair(std::stoi(cmd.get_params()[0]), std::stoi(cmd.get_params()[1]));
    std::pair<int, int> new_pos = std::make_pair(std::stoi(cmd.get_params()[2]), std::stoi(cmd.get_params()[3]));
    // )חיפוש הכלי המתאים במערך החיילים )ראשון
    auto it = std::find_if(pieces.begin(), pieces.end(),
                           [&target_id](const std::shared_ptr<Piece> &p)
                           {
                               return (p->get_id() == target_id && !p->is_captured()); // רק אם לא נלכד
                           });

    if (it != pieces.end())
    {
        std::shared_ptr<Piece> piece = *it;

        if (cmd.get_type() == "Move")
        {
            piece->get_graphics().set_action("move");
            std::cout << " +++ I am moving from " << old_pos.first << ", " << old_pos.second << " to " << new_pos.first << ", " << new_pos.second << std::endl;
            piece->reset(cmd, pieces);
            draw_pieces_on_board(pieces, board, game_time_ms());
            std::cout << " --- " << std::endl;
        }
        else if (cmd.get_type() == "Jump")
        {
            piece->get_graphics().set_action("jump");

            std::cout << "  --- I am Jumpinggggggggg   " << target_id << std::endl;
            // הפעלת reset על הכלי הרלוונטי עם הפקודה
        }
        else
        {
            std::cerr << "Unknown command type: " << cmd.get_type() << std::endl;
        }
    }
    else
    {
        std::cerr << "No piece found with ID: " << target_id << std::endl;
    }
}

// std::string Game::get_piece_id_at(int row, int col)
// {
//     // מחפש את ה־ID של הכלי בתא (row, col)
//     for (const auto &p : pieces)
//     {
//         if (!p->is_captured() && p->get_pos() == std::make_pair(row, col))
//         {
//             return p->get_id();
//         }
//     }
//     return "none"; // אם לא נמצא כלי בתא הזה
// }

// std::vector<std::tuple<int, int, std::string>> Game::get_all_available_steps(int row, int col)
// {
//     std::shared_ptr<Piece> piece = get_piece_by_pos(row, col);
//     if (!piece)
//     {
//         std::vector<std::tuple<int, int, std::string>> a(0);
//         return a;
//     }
//     return piece->get_state().get_moves().get_moves(row, col);
// }

std::shared_ptr<Piece> Game::get_piece_by_pos(int r, int c)
{
    for (auto &&piece : pieces)
    {
        if (piece->get_pos() == std::make_pair(r, c) && !piece->is_captured())
        {
            return piece;
        }
    }
    return nullptr;
}
