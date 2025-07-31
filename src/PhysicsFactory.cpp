// #include "../../It1_interfaces/PhysicsFactory.hpp"

// PhysicsFactory::PhysicsFactory(const Board &b)
//     : board(b)
// {
//     // כאן אפשר לשמור מידע נוסף אם צריך
// }

// Physics PhysicsFactory::create(std::pair<int, int> start_cell, const std::string &cfg)
// {
//     // // TODO: להשתמש ב-cfg אם צריך, וליצור Physics
//     // // לדוגמה – מהירות ברירת מחדל
//     // float speed = 1.0f;

//     // // אם רוצים לקרוא ערך מה-map:
//     // auto it = cfg.find("speed");
//     // if (it != cfg.end())
//     // {
//     //     speed = std::stof(it->second);
//     // }

//     // return Physics(start_cell, *board, speed);

//     // יצירת אובייקט Physics לפי הקונפיגורציה
//     return Physics(start_cell, board);
// }

// //////////

// // PhysicsFactory(const Board& b) ;

// // Physics create(std::pair<int, int> start_cell, const std::string &cfg);
