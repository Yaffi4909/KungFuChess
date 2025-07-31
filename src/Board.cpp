#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include "Img.hpp"
#include "../../It1_interfaces/Board.hpp"

Board::Board(int cell_H_pix,
             int cell_W_pix,
             int W_cells,
             int H_cells,
             const Img &img,
             const std::string &path)
    : cell_H_pix(cell_H_pix),
      cell_W_pix(cell_W_pix),
      W_cells(W_cells),
      H_cells(H_cells),
      img(img),
      path(path)
{
}

// Board::Board(std::shared_ptr<Board> board_ptr)
// {
//   // העתקת הערכים מהאובייקט המשותף
//   cell_H_pix = board_ptr->cell_H_pix;
//   cell_W_pix = board_ptr->cell_W_pix;
//   W_cells = board_ptr->W_cells;
//   H_cells = board_ptr->H_cells;
//   img = board_ptr->img; // הנחה ש‑Img תומך בהעתקה
// }

// // פעולה שיוצרת עותק של האובייקט
// std::shared_ptr<Board>  Board::clone() const
// {
//   // כאן אני מניח ש‑Img עצמו תומך בהעתקה (copy constructor)
//   return std::make_shared<Board>(cell_H_pix, cell_W_pix, W_cells, H_cells, img);
// }

std::vector<std::vector<std::string>> read_board_from_file()
{
  const std::string file_path = "../../pieces/board.csv"; // שים לב לנתיב היחסי מהמיקום של התוכנית שלך
  std::ifstream file(file_path);
  std::vector<std::vector<std::string>> board(8, std::vector<std::string>(8, ""));

  if (!file.is_open())
  {
    std::cerr << "Failed to open file: " << file_path << std::endl;
    return board; // מחזיר לוח ריק במקרה של שגיאה
  }

  std::string line;
  int row = 0;

  while (std::getline(file, line) && row < 8)
  {
    std::istringstream iss(line);
    std::string cell;
    int col = 0;

    while (std::getline(iss, cell, ',') && col < 8)
    {
      board[row][col] = cell;
      col++;
    }

    row++;
  }

  file.close();

  // הדפסת הבורד לבדיקה
  for (const auto &row_vec : board)
  {
    for (const auto &cell : row_vec)
    {
      std::cout << (cell.empty() ? "--" : cell) << " ";
    }
    std::cout << std::endl;
  }

  return board;
}