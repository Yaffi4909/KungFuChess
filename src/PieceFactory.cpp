// #define WIN32_LEAN_AND_MEAN
// #include <windows.h>

#include "../../It1_interfaces/PieceFactory.hpp"
// #include "../../It1_interfaces/Piece.hpp"
// // #include <fstream>


PieceFactory::PieceFactory(const std::string &root, Board &board_) : board(board_), pieces_root(root)
{
    std::cout<< " " << std::endl;
    
    // TODO: לטעון תבניות של חלקים מתוך pieces_root
}

std::vector<std::shared_ptr<Piece>> PieceFactory::create_all_pieces(std::vector<std::vector<std::string>> &p_in_b)
{
    std::vector<std::shared_ptr<Piece>> pieces;
        int i = 0;
        int j = 0;
        int index = 0; // אינדקס ייחודי לכל חייל

        for (int i = 0; i < p_in_b.size(); i++)
        {
            for (int j = 0; j < p_in_b[i].size(); j++)
            {
                if (p_in_b[i][j] != "")
                {
                    pieces.push_back(create_piece(p_in_b[i][j], {i, j}));
                }
            }
        }
    return pieces;
}

std::shared_ptr<Piece> PieceFactory::create_piece(const std::string &p_id, std::pair<int, int> cell)
{

    State initial_state{Moves(pieces_root + "/" + p_id.substr(0, 2) + "/moves.txt", {8, 8}),
                        Graphics(pieces_root, {50, 50}),
                        Physics({cell.first, cell.second}, board)};

    int index = cell.first * board.H_cells + cell.second;

    return std::make_shared<Piece>(p_id + std::to_string(index), initial_state);
}
