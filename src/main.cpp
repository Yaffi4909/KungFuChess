#include "img.hpp"
#include "../../It1_interfaces/Game.hpp"
#include "../../It1_interfaces/Board.hpp"
// #include "../../It1_interfaces/PubSubQueue.hpp"
// #include "../../It1_interfaces/ScoreManager.hpp"
#include "../../It1_interfaces/PieceFactory.hpp"
#include <iostream>
#include <fstream>
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

int main()
{
    std::cout << "well done yaffi!!!!!!!!!!" << std::endl;

    try
    {
        std::cout << "Testing Img class..." << std::endl;

        Img img;
        img.read(R"(../../board.png)", {480, 480}, true);
        Board board(480, 480, 8, 8, img, "../../board.png");

        std::vector<std::vector<std::string>> p_in_b = read_board_from_file();

        // יצירת ה pieces//
        PieceFactory pieceFactory("../../pieces", board);
        std::vector<std::shared_ptr<Piece>> pieces = pieceFactory.create_all_pieces(p_in_b);

        Game game(pieces, std::make_shared<Board>(board));
        // board.img.show();

        // cv::waitKey(2000);

        // pieces[0]->erase_from_board(board, 1);

        // cv::waitKey(20000);
        // board.img.show();

        game.run();

        std::cout << "Img class test completed successfully!" << std::endl;

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
