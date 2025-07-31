#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <stdexcept>
#include <tuple>
#include "../../It1_interfaces/Moves.hpp"
#include "../../It1_interfaces/piece.hpp"
#include "../../It1_interfaces/game.hpp"



// #include <windows.h>
// #include <mmsystem.h>
// #pragma comment(lib, "winmm.lib")

// // void Moves::playMP3(const char* filename)
// // {
// //     char command[256];
// //     // פותח ונגן את קובץ ה-MP3
// //     printf_s(command, L"open \"%s\" type mpegvideo alias mp3", filename);
// //     mciSendString(command, NULL, 0, NULL);

// //     mciSendString(L"play mp3", NULL, 0, NULL);
// // }

Moves::Moves(const std::string &txt_path, std::pair<int, int> dims)
    : rules_path(txt_path), board_dims(dims)
{
    // טוען את התנועות מהקובץ בעזרת הפונקציה
    load_move_offsets(txt_path);

    // אם תרצה, תוכל להוסיף קוד נוסף לאתחול
}


// /**
//  * קורא קובץ של כללי תזוזה ומחזיר וקטור של זוגות (dy, dx)
//  * פורמט קובץ: כל שורה מכילה זוג מספרים מופרדים בפסיק, לדוגמה: 1,0
//  */

void Moves::load_move_offsets(const std::string &file_path)
{
    std::vector<std::tuple<int, int, std::string>> moves_offsets_;
    std::ifstream file(file_path);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open move offsets file: " + file_path);
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::string coords_part, type;
        size_t colon_pos = line.find(':');

        if (colon_pos != std::string::npos)
        {
            coords_part = line.substr(0, colon_pos);
            type = line.substr(colon_pos + 1);
        }
        else
        {
            coords_part = line;
            type = "";
        }

        std::istringstream iss(coords_part);
        std::string dy_str, dx_str;

        if (!std::getline(iss, dy_str, ','))
            continue;
        if (!std::getline(iss, dx_str))
            continue;

        try
        {
            int dy = std::stoi(dy_str);
            int dx = std::stoi(dx_str);

            moves_offsets_.emplace_back(dy, dx, type);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Invalid line in " << file_path << ": " << line << " (" << e.what() << ")\n";
        }
    }

    if (moves_offsets_.empty())
    {
        throw std::runtime_error("No valid moves found in file: " + file_path);
    }

    moves_offsets = moves_offsets_;
}


// /**
//  * מחזירה את כל המהלכים האפשריים מתא (r,c) על הלוח.
//  * המהלכים מחושבים לפי הזזות שנקראו בקונסטרקטור.
//  *
//  * עבור כל הזזה (dx, dy) מחושבת מיקום חדש (r+dx, c+dy).
//  * אם המיקום החדש נמצא בתוך גבולות הלוח, הוא נוסף לרשימת המהלכים.
//  *
//  * @param r שורה נוכחית
//  * @param c עמודה נוכחית
//  * @return וקטור זוגות (row, col) של מיקומים חוקיים חדשים
//  */
std::vector<std::tuple<int, int, std::string>> Moves::get_moves(int r, int c) const
{
    std::shared_ptr<Piece> piece = Game::get_piece_by_pos(r, c);
    std::vector<std::tuple<int, int, std::string>> result;

    // for (auto &[dx, dy, role] : moves_offsets)
    // {
    //     int nr = r + dx;
    //     int nc = c + dy;
    //     if (!(nr >= 0 && nr < board_dims.first && nc >= 0 && nc < board_dims.second))
    //     {
    //         std::cout << ", nr: " << nr << ", nc: " << nc << "1" << std::endl;

    //         // אם לא בגבולות הלוח
    //     }
    //     else if (piece->get_id().substr(0, 1) != "N" && !Piece::is_available_move(r, c, r, c, nr, nc, Game::get_pieces()))
    //     {
    //         std::cout << ", nr: " << nr << ", nc: " << nc << "2" << std::endl;

    //         // לא סוס ומדלג
    //     }
    //     else if (piece->get_id().substr(0, 1) == "P" && ((piece->get_id().substr(1, 1) == "B" && r != 1 && nr == r + 2) || (piece->get_id().substr(1, 1) == "W" && r != 6 && nr == r - 2)))
    //     {
    //         std::cout << ", nr: " << nr << ", nc: " << nc << "3" << std::endl;

    //         // אם זה חייל שלא עומד במיקום ההתחלתי ורוצה ללכת 2 פסיעות
    //     }
    //     else if (piece->get_id().substr(0, 1) == "P" && c != nc && !Game::get_piece_by_pos(nr, nc))
    //     {
    //         std::cout << ", nr: " << nr << ", nc: " << nc << "4" << std::endl;

    //         // אם זה חייל שרוצה ללכת עקום אפילו שאין שם אף אחד
    //     }
    //     else if (piece->get_id().substr(0, 1) == "P" && (r + 1 == nr || r - 1 == nr) && c == nc && Game::get_piece_by_pos(nr, nc))
    //     {
    //         //אם חייל רוצה ללכת פסיעה אחת ישר ויש שם חייל ב
    //     }
    //     else if (Game::get_piece_by_pos(nr, nc) && Game::get_piece_by_pos(nr, nc)->get_id().substr(1, 1) == piece->get_id().substr(1, 1))
    //     {
    //         std::cout << ", nr: " << nr << ", nc: " << nc << "5" << std::endl;

    //         // אם נמצא שם חייל של אותו צבא
    //     }
    //     else
    //     {
    //         std::cout << ", nr: " << nr << ", nc: " << nc << "6" << std::endl;

    //         // אם לא התקיימה אף בעיה
    //         result.emplace_back(nr, nc, role);
    //     }
    // }
    return result;
}
