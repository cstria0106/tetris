#include "tetromino.h"
#include "console/console.h"

Tetromino::Tetromino(std::string name, int size, std::string shape) {
  name_ = name;
  size_ = size;
  original_ = this;

  for (int x = 0; x < size; x++) {
    for (int y = 0; y < size; y++) {
      shape_[x][y] = shape[y * size + x] == 'O';
    }
  }
}

Tetromino Tetromino::rotatedCW() {
  Tetromino rotated = Tetromino(name_, size_, original_);
  for (int x = 0; x < size_; x++) {
    for (int y = 0; y < size_; y++) {
      rotated.shape_[x][y] = shape_[y][size_ - x - 1];
    }
  }
  return rotated;
}

Tetromino Tetromino::rotatedCCW() {
  Tetromino rotated = Tetromino(name_, size_, original_);
  for (int x = 0; x < size_; x++) {
    for (int y = 0; y < size_; y++) {
      rotated.shape_[x][y] = shape_[size_ - y - 1][x];
    }
  }
  return rotated;
}

void Tetromino::drawAt(std::string s, int x, int y) {
  for (int dx = 0; dx < size_; dx++) {
    for (int dy = 0; dy < size_; dy++) {
      if (check(dx, dy)) {
        console::draw(x + dx, y + dy, s);
      }
    }
  }
}

Tetromino Tetromino::I("I", 4, "XXXXOOOOXXXXXXXX");

Tetromino Tetromino::O("O", 2, "OOOO");

Tetromino Tetromino::T("T", 3, "XOXOOOXXX");

Tetromino Tetromino::S("S", 3, "XOOOOXXXX");

Tetromino Tetromino::Z("Z", 3, "OOXXOOXXX");

Tetromino Tetromino::J("J", 3, "OXXOOOXXX");

Tetromino Tetromino::L("L", 3, "XXOOOOXXX");
