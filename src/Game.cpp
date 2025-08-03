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

int num = 0;
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
    // draw_pieces_on_board(pieces_, board, now_ms);

    std::cout << "Game initialized with " << pieces.size() << " pieces." << std::endl;
}

long long Game::game_time_ms() const
{
    // מחזיר זמן מונוטוני במילישניות
    auto now = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return ms.count();
}

// נניח שהגדרת הלוח היא:
const int CELL_SIZE = 60;
const int BOARD_SIZE = 8;

void Game::start_user_input_thread()
{
    std::cout << "Starting user input thread..." << std::endl;

    std::thread([this]()
                {
                    cv::namedWindow("ChessBoard");

                    // Callback של העכבר
                    cv::setMouseCallback("ChessBoard", [](int event, int x, int y, int, void *userdata)
                                         {
            if (event != cv::EVENT_LBUTTONDOWN) return;

            Game* game = static_cast<Game*>(userdata);
            // int col = x / CELL_SIZE;
            // int row = y / CELL_SIZE;

            // התאם למיקום החדש של הלוח (200 פיקסלים מהשמאל)
            int panel_width = 200;
            if (x < panel_width || x > panel_width + 480) return; // מחוץ ללוח
    
            int adjusted_x = x - panel_width;
            int col = adjusted_x / CELL_SIZE;
            int row = y / CELL_SIZE;

            if (row >= BOARD_SIZE || col >= BOARD_SIZE) return;

            static int first_click_row = -1;
            static int first_click_col = -1;
            static bool waiting_for_second_click = false;

            int now_ms = static_cast<int>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count()
            );
            std::string piece_id = game->get_piece_id_at(row, col);
            std::cout << "Clicked on (" << row << ", " << col << ") - Piece ID: " << piece_id << std::endl;

            std::cout << "waiting_for_second_click: " << waiting_for_second_click << std::endl;

            if (piece_id == "none" && !waiting_for_second_click){
                //לחיצה ראשונה על משבצת ריקה
                return;
            }
            if(piece_id != "none" && !waiting_for_second_click && get_piece_by_pos(row, col)->get_graphics().get_action() != "idle" ){
                //לחיצה על חייל בסטטוס המתנה
                std::cout << "rest................" << std::endl;
                return;
            }

            if (waiting_for_second_click)
            {
                waiting_for_second_click = game->treat_second_click(row, col, piece_id, {first_click_row, first_click_col}, now_ms);
            
            }

            if (!waiting_for_second_click){
                std::cout << " -> First click on (" << row << ", " << col << ")" << std::endl;
                game->available_steps = game->get_all_available_steps(row, col);
            }

            waiting_for_second_click = !waiting_for_second_click;
            first_click_row = row;
            first_click_col = col;
            game->last_clicked_row = row;
            game->last_clicked_col = col; }, this);

                    // 🔁 לולאה גרפית שפועלת כל עוד running == true
                    while (running)
                    { // ציור הלוח
                        draw_all_pieces_on_board(pieces, available_steps, {last_clicked_row, last_clicked_col});

                        // זמן המתנה בין ציורים
                        if (cv::waitKey(10) == 27) // Esc
                            break;
                    }

                    std::cout << "User input thread finished." << std::endl;
                    // cv::destroyWindow("ChessBoard");
                })
        .detach();
}

void Game::show_start_screen()
{

    // טען תמונת פתיחה
    Img start_img;
    start_img.read(R"(../../pepole_play.png)", {880, 480}, true);

    cv::Mat start_mat = start_img.get_mat().clone();

    // הוסף טקסט "Click to Start Game"
    std::string text = "Click to Start Game";
    int font = cv::FONT_HERSHEY_DUPLEX;
    double fontScale = 1.0;
    int thickness = 2;

    cv::Size textSize = cv::getTextSize(text, font, fontScale, thickness, nullptr);
    cv::Point textPos(
        start_mat.cols / 2 - textSize.width / 2,
        40);

    cv::putText(start_mat, text, textPos, font, fontScale, cv::Scalar(0, 0, 255), thickness);

    cv::namedWindow("ChessBoard");
    cv::imshow("ChessBoard", start_mat);

    // המתן ללחיצה
    bool game_started = false;
    cv::setMouseCallback("ChessBoard", [](int event, int x, int y, int, void *userdata)
                         {
        if (event == cv::EVENT_LBUTTONDOWN) {
            bool* started = static_cast<bool*>(userdata);
            *started = true;
        } }, &game_started);

    // המתן עד שילחצו
    while (!game_started)
    {
        if (cv::waitKey(30) == 27)
            break; // Esc לצאת
    }
    cv::destroyAllWindows();
}

void Game::run()
{
    std::cout << "Game started!" << std::endl;
    show_start_screen();

    // Board :: img.draw_on(Img& other_img, int x, int y)// הצגת הלוח הראשוני
    start_user_input_thread();
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
                p->update(now);
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

        // (3) draw current position
        if (!show())
        {
            break;
        }
        if (this->is_win())
        {
            std::cout << "Game over!1" << std::endl;
            running = false;                                             // עצור את ה-thread של input לפני שמציגים הודעה
            std::this_thread::sleep_for(std::chrono::milliseconds(200)); // הוסף המתנה יותר ארוכה

            announce_win();

            std::cout << "Game over!2" << std::endl;

            std::cout << "Game over!3" << std::endl;

            break;
        }
    }
    std::cout << "4" << std::endl;

    running = false; // עצור את ה-thread של input לפני שמציגים הודעה
    // cv::destroyAllWindows(); // רק אחרי שהמשתמש לחץ
}

// // ─── מתודות פנימיות ────────────────────────────────

bool Game::show()
{
    return board->img.is_loaded();
}

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

void Game::announce_win()
{
    std::cout << "Announcing winner..." << std::endl;
    std::string message;
    auto score = ScoreManager::get_instance().get_score();
    if (score.first >= 100)
    {
        message = "White Wins!!!";
    }
    else if (score.second >= 100)
    {
        message = "Black Wins!!!";
    }
    else
    {
        message = "The game is finished.";
    }

    std::cout << "Announcing winner: " << message << std::endl;

    // יצירת חלון עם פאנלים (כמו ב-draw_all_pieces_on_board)
    int board_size = 480;
    int panel_width = 200;
    int total_width = board_size + 2 * panel_width;
    int total_height = board_size;

    cv::Mat full_img = cv::Mat::zeros(total_height, total_width, CV_8UC3);
    full_img.setTo(cv::Scalar(50, 50, 50));

    // יצירת תמונת ניצחון לאמצע
    cv::Mat win_img = cv::Mat::zeros(board_size, board_size, CV_8UC3);
    win_img.setTo(cv::Scalar(255, 255, 255)); // רקע ירוק כהה

    // === הוספת הכתר ===
    try
    {
        cv::Mat crown = cv::imread("../../crown.png", cv::IMREAD_COLOR);
        if (!crown.empty())
        {
            cv::Mat crown_converted;
            if (crown.channels() != win_img.channels())
            {
                if (crown.channels() == 4 && win_img.channels() == 3)
                {
                    cv::cvtColor(crown, crown_converted, cv::COLOR_BGRA2BGR);
                }
                else
                {
                    crown_converted = crown;
                }
            }
            else
            {
                crown_converted = crown;
            }

            cv::Mat resized_crown;
            double scale = 0.15;
            cv::resize(crown_converted, resized_crown, cv::Size(), scale, scale);

            int x = win_img.cols / 2 - resized_crown.cols / 2;
            int y = win_img.rows / 6;

            if (x >= 0 && y >= 0 &&
                x + resized_crown.cols <= win_img.cols &&
                y + resized_crown.rows <= win_img.rows)
            {
                resized_crown.copyTo(win_img(cv::Rect(x, y, resized_crown.cols, resized_crown.rows)));
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading crown image: " << e.what() << std::endl;
    }

    // === הוספת קונפטי ===
    for (int i = 0; i < 300; ++i)
    {
        cv::Point pt(rand() % win_img.cols, rand() % win_img.rows);
        cv::Scalar color(rand() % 256, rand() % 256, rand() % 256);
        int radius = rand() % 6 + 2;
        cv::circle(win_img, pt, radius, color, cv::FILLED);
    }

    // === כתיבת הודעה ===
    int font = cv::FONT_HERSHEY_DUPLEX;
    double fontScale = 1.5;
    int thickness = 3;

    cv::Size textSize = cv::getTextSize(message, font, fontScale, thickness, nullptr);
    cv::Point textOrg(
        win_img.cols / 2 - textSize.width / 2,
        win_img.rows / 2 + textSize.height / 2 + 100);
    cv::putText(win_img, message, textOrg, font, fontScale, cv::Scalar(255, 0, 0), thickness);

    // הדבק את תמונת הניצחון באמצע
    win_img.copyTo(full_img(cv::Rect(panel_width, 0, board_size, board_size)));

    // פאנל שמאל - שחקן לבן
    cv::rectangle(full_img, cv::Rect(0, 0, panel_width, total_height), cv::Scalar(200, 200, 200), cv::FILLED);
    cv::putText(full_img, "WHITE PLAYER", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 0), 2);
    cv::putText(full_img, "Score: " + std::to_string(score.first), cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);

    // פאנל ימין - שחקן שחור
    cv::rectangle(full_img, cv::Rect(panel_width + board_size, 0, panel_width, total_height), cv::Scalar(100, 100, 100), cv::FILLED);
    cv::putText(full_img, "BLACK PLAYER", cv::Point(panel_width + board_size + 10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    cv::putText(full_img, "Score: " + std::to_string(score.second), cv::Point(panel_width + board_size + 10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    // cv::destroyAllWindows();
    std::cout << "going to show announce_win" << std::endl;
    cv::destroyAllWindows();

    cv::namedWindow("WinScreen");
    cv::imshow("WinScreen", full_img);



    cv::waitKey(1); // הוסף את זה

    std::cout << "show announce_win" << std::endl;

    // המתן ללחיצה
    bool game_finished = false;
    cv::setMouseCallback("WinScreen", [](int event, int x, int y, int, void *userdata)
                         {
        if (event == cv::EVENT_LBUTTONDOWN) {
            bool* started = static_cast<bool*>(userdata);
            *started = true;
        } }, &game_finished);

    // המתן עד שילחצו
    while (!game_finished)
    {
        if (cv::waitKey(30) == 27)
            break; // Esc לצאת
    }
    Img I_love;
    I_love.read(R"(../../I_love_chess.png)", {880, 480}, true);
    game_finished = false;
cv::Mat I_love_resized;
cv::resize(I_love.get_mat(), I_love_resized, cv::Size(board_size, board_size));
I_love_resized.copyTo(full_img(cv::Rect(panel_width, 0, board_size, board_size)));
    cv::imshow("WinScreen", full_img);
    cv::setMouseCallback("WinScreen", [](int event, int x, int y, int, void *userdata)
                         {
        if (event == cv::EVENT_LBUTTONDOWN) {
            bool* started = static_cast<bool*>(userdata);
            *started = true;
        } }, &game_finished);
    while (!game_finished)
    {
        if (cv::waitKey(30) == 27)
            break; // Esc לצאת
    }
    cv::destroyAllWindows();
}

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
            // draw_pieces_on_board(pieces, board, game_time_ms());
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

std::string Game::get_piece_id_at(int row, int col)
{
    // מחפש את ה־ID של הכלי בתא (row, col)
    for (const auto &p : pieces)
    {
        if (!p->is_captured() && p->get_pos() == std::make_pair(row, col))
        {
            return p->get_id();
        }
    }
    return "none"; // אם לא נמצא כלי בתא הזה
}

std::vector<std::tuple<int, int, std::string>> Game::get_all_available_steps(int row, int col)
{
    std::shared_ptr<Piece> piece = get_piece_by_pos(row, col);
    if (!piece)
    {
        std::vector<std::tuple<int, int, std::string>> a(0);
        return a;
    }
    return piece->get_state().get_moves().get_moves(row, col);
}

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

bool Game::treat_second_click(int row, int col, std::string piece_id, std::pair<int, int> first_click_pos, int now_ms)
{
    std::cout << "Second click on (" << row << ", " << col << ")" << std::endl;
    bool is_valid = false;
    for (auto &&step : available_steps)
    {
        if (std::get<0>(step) == row && std::get<1>(step) == col)
            is_valid = true;
    }
    std::cout << "is_valid: " << is_valid << std::endl;
    // אם מפסיעה נמצאה ברשימת הפסיעות
    if (is_valid)
    {
        std::string first_piece_id = get_piece_id_at(first_click_pos.first, first_click_pos.second);
        std::string type = (piece_id == first_piece_id) ? "Jump" : "Move";

        std::vector<std::string> params = {
            std::to_string(first_click_pos.first), std::to_string(first_click_pos.second),
            std::to_string(row), std::to_string(col)};

        Command cmd(now_ms, first_piece_id, type, params);

        std::lock_guard<std::mutex> lock(queue_mutex);
        std::cout << "running: " << running << std::endl;
        user_input_queue.push(cmd);

        available_steps.clear();
        return true;
    }
    // אם הפסיעה אינה חוקית
    // ביטול בחירה
    available_steps.clear();
    return false;
}

void Game::draw_all_pieces_on_board(std::vector<std::shared_ptr<Piece>> pieces, std::vector<std::tuple<int, int, std::string>> available_steps, std::pair<int, int> last_clicked_pos)
{
    // יצירת חלון רחב יותר (לוח + 2 פאנלים)
    int board_size = 480;
    int panel_width = 200;
    int total_width = board_size + 2 * panel_width;
    int total_height = board_size;

    // יצירת תמונה כוללת
    cv::Mat full_img = cv::Mat::zeros(total_height, total_width, CV_8UC3);
    full_img.setTo(cv::Scalar(50, 50, 50)); // רקע אפור כהה

    // לוח השחמט באמצע
    cv::Mat board_img = board->img.get_mat().clone();
    cv::resize(board_img, board_img, cv::Size(board_size, board_size));

    // צייר את כל החיילים על הלוח
    for (const auto &p : pieces)
    {
        if (!p->is_captured())
        {
            long long current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                                         std::chrono::steady_clock::now().time_since_epoch())
                                         .count();

            std::pair<double, double> pixel_pos = p->get_visual_pixel_position(current_time);
            int x = (int)pixel_pos.second + 5;
            int y = (int)pixel_pos.first + 5;

            Img sprite = p->get_graphics().get_img_by_id(p->get_id());
            cv::Mat sprite_mat = sprite.get_mat();

            if (!sprite_mat.empty() && x >= 0 && y >= 0 &&
                x + sprite_mat.cols <= board_img.cols &&
                y + sprite_mat.rows <= board_img.rows)
            {
                // וודא שיש אותו מספר ערוצים
                cv::Mat sprite_converted;
                if (sprite_mat.channels() != board_img.channels())
                {
                    if (sprite_mat.channels() == 4 && board_img.channels() == 3)
                    {
                        cv::cvtColor(sprite_mat, sprite_converted, cv::COLOR_BGRA2BGR);
                    }
                    else if (sprite_mat.channels() == 3 && board_img.channels() == 4)
                    {
                        cv::cvtColor(sprite_mat, sprite_converted, cv::COLOR_BGR2BGRA);
                    }
                    else
                    {
                        sprite_converted = sprite_mat;
                    }
                }
                else
                {
                    sprite_converted = sprite_mat;
                }

                sprite_converted.copyTo(board_img(cv::Rect(x, y, sprite_converted.cols, sprite_converted.rows)));
            }
        }
    }

    // צייר מהלכים זמינים
    for (const auto &step : available_steps)
    {
        int r = std::get<0>(step);
        int c = std::get<1>(step);
        cv::rectangle(board_img, cv::Rect(c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE, CELL_SIZE),
                      cv::Scalar(0, 128, 255), 2);
    }

    // צייר לחיצה אחרונה
    if (last_clicked_pos.first >= 0 && last_clicked_pos.second >= 0)
    {
        cv::rectangle(board_img, cv::Rect(last_clicked_pos.second * CELL_SIZE, last_clicked_pos.first * CELL_SIZE, CELL_SIZE, CELL_SIZE),
                      cv::Scalar(0, 0, 255), 2);
    }

    // וודא שהלוח באותו פורמט כמו התמונה הכוללת
    cv::Mat board_converted;
    if (board_img.channels() != full_img.channels())
    {
        if (board_img.channels() == 4 && full_img.channels() == 3)
        {
            cv::cvtColor(board_img, board_converted, cv::COLOR_BGRA2BGR);
        }
        else if (board_img.channels() == 3 && full_img.channels() == 4)
        {
            cv::cvtColor(board_img, board_converted, cv::COLOR_BGR2BGRA);
        }
        else
        {
            board_converted = board_img;
        }
    }
    else
    {
        board_converted = board_img;
    }

    // הדבק את הלוח באמצע
    board_converted.copyTo(full_img(cv::Rect(panel_width, 0, board_size, board_size)));

    // פאנל שמאל - שחקן לבן
    cv::rectangle(full_img, cv::Rect(0, 0, panel_width, total_height), cv::Scalar(200, 200, 200), cv::FILLED);
    cv::putText(full_img, "WHITE PLAYER", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 0), 2);

    auto score = ScoreManager::get_instance().get_score();
    cv::putText(full_img, "Score: " + std::to_string(score.first), cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);

    // פאנל ימין - שחקן שחור
    cv::rectangle(full_img, cv::Rect(panel_width + board_size, 0, panel_width, total_height), cv::Scalar(100, 100, 100), cv::FILLED);
    cv::putText(full_img, "BLACK PLAYER", cv::Point(panel_width + board_size + 10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    cv::putText(full_img, "Score: " + std::to_string(score.second), cv::Point(panel_width + board_size + 10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

    cv::imshow("ChessBoard", full_img);
}