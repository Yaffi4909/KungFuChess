// #include <cassert>
// #include <vector>
// #include <string>
// #include "../../../It1_interfaces/Command.hpp"


// int main() {
//     // יצירה ראשונית
//     Command cmd(3000, "Pawn1", "Move", {"e2", "e4"});

//     // בדיקה ראשונית
//     assert(cmd.get_timestamp() == 3000);
//     assert(cmd.get_piece_id() == "Pawn1");
//     assert(cmd.get_type() == "Move");
//     assert(cmd.get_params().size() == 2);
//     assert(cmd.get_params()[0] == "e2");
//     assert(cmd.get_params()[1] == "e4");

//     // שינוי ערכים
//     cmd.set_timestamp(4000);
//     cmd.set_piece_id("Knight2");
//     cmd.set_type("Jump");
//     cmd.set_params({"g1", "f3"});

//     // בדיקה אחרי שינוי
//     assert(cmd.get_timestamp() == 4000);
//     assert(cmd.get_piece_id() == "Knight2");
//     assert(cmd.get_type() == "Jump");
//     assert(cmd.get_params().size() == 2);
//     assert(cmd.get_params()[0] == "g1");
//     assert(cmd.get_params()[1] == "f3");

//     return 0;
// }
