#include "game.h"
#include "console/console.h"
#include "tetromino.h"
#include <cstdlib>
#include <string>

Tetromino *randomTetromino() {
  Tetromino *minos[] = {&Tetromino::I, &Tetromino::O, &Tetromino::T,
                        &Tetromino::S, &Tetromino::Z, &Tetromino::J,
                        &Tetromino::L};

  return minos[rand() % (sizeof(minos) / sizeof(minos[0]))];
}

void Game::resetPosition() {
  x_ = BOARD_WIDTH / 2 - current()->size() / 2;
  y_ = 0;
}

bool Game::spawned() { return current_ != nullptr; }

void Game::despawn() {
  if (!spawned())
    return;
  delete current_;
  current_ = nullptr;
}

void Game::setCurrent(Tetromino *tetromino) {
  if (spawned()) {
    despawn();
  }
  current_ = new Tetromino(*tetromino);
  resetPosition();

  if (check(*current_, x_, y_)) {
    lose_ = true;
  }
}

void Game::spawn() {
  setCurrent(next_);
  next_ = randomTetromino();
}

void Game::hold() {
  if (hold_ == nullptr) {
    hold_ = current()->original();
    spawn();
  } else {
    Tetromino *temp = current()->original();
    setCurrent(hold_);
    hold_ = temp;
  }
}

void Game::freeze() {
  for (int dx = 0; dx < current()->size(); dx++) {
    for (int dy = 0; dy < current()->size(); dy++) {
      if (current()->check(dx, dy)) {
        board_[x_ + dx][y_ + dy] = true;
      }
    }
  }
  scanAndClear();
  spawn();
  holdUsed_ = false;
}

void Game::softDrop() {
  if (check(*current(), x_, y_ + 1)) {
    freeze();
  } else {
    y_ += 1;
  }
}

void Game::hardDrop() {
  y_ = floorY();
  softDrop();
}

int Game::floorY() {
  int dy = 0;
  while (!check(*current(), x_, y_ + dy + 1)) {
    dy++;
  }
  return y_ + dy;
}

bool Game::check(Tetromino &tetromino, int x, int y) {
  for (int dx = 0; dx < tetromino.size(); dx++) {
    for (int dy = 0; dy < tetromino.size(); dy++) {
      int xx = x + dx;
      int yy = y + dy;

      if (tetromino.check(dx, dy)) {
        if (check(xx, yy) || xx < 0 || xx >= BOARD_WIDTH || yy < 0 ||
            yy >= BOARD_HEIGHT) {
          return true;
        }
      }
    }
  }
  return false;
}

bool Game::check(int x, int y) { return board_[x][y]; }

bool Game::checkYFilled(int y) {
  for (int x = 0; x < BOARD_WIDTH; x++) {
    if (!board_[x][y])
      return false;
  }

  return true;
}

void Game::clearLine(int y) {
  for (int yy = y; yy >= 0; yy--) {
    for (int xx = 0; xx < BOARD_WIDTH; xx++) {
      if (yy == 0) {
        board_[xx][yy] = false;
      } else {
        board_[xx][yy] = board_[xx][yy - 1];
      }
    }
  }

  remainingLines_ -= 1;
  if (remainingLines_ == 0)
    win_ = true;
}

void Game::scanAndClear() {
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    if (checkYFilled(y)) {
      clearLine(y);
    }
  }
}

Tetromino *Game::current() {
  if (!spawned()) {
    spawn();
  }
  return current_;
}

void Game::update() {
  if (gameOver())
    return;

  // handle drop
  bool forceSoftDrop = console::key(console::K_DOWN);
  dropTimer_--;
  if (dropTimer_ == 0 || forceSoftDrop) {
    softDrop();
    dropTimer_ = DROP_DELAY;
  }

  if (console::key(console::K_UP)) {
    hardDrop();
  }

  // hold
  if (console::key(console::K_SPACE) && !holdUsed_) {
    holdUsed_ = true;
    hold();
  }

  // handle horizontal movement
  int moveX = 0;
  if (console::key(console::K_LEFT)) {
    moveX -= 1;
  }
  if (console::key(console::K_RIGHT)) {
    moveX += 1;
  }
  if (!check(*current(), x_ + moveX, y_)) {
    x_ += moveX;
  }

  // handle rotation
  if (console::key(console::K_Z)) {
    Tetromino rotated = current()->rotatedCCW();
    if (!check(rotated, x_, y_)) {
      *current() = rotated;
    }
  } else if (console::key(console::K_X)) {
    Tetromino rotated = current()->rotatedCW();
    if (!check(rotated, x_, y_)) {
      *current() = rotated;
    }
  }

  // game
  elapsedFrames_ += 1;
}

void Game::drawMinoPanel(std::string title, Tetromino *tetromino, int x,
                         int y) {
  console::drawBox(x, y, x + 5, y + 5);
  console::draw(x + 1, y, title);

  if (tetromino != nullptr) {
    tetromino->drawAt(BLOCK_STRING, x + 3 - tetromino->size() / 2,
                      y + 3 - tetromino->size() / 2);
  }
}

std::string zeroPadding(std::string s) {
  while (s.length() < 2) {
    s = "0" + s;
  }
  return s;
}

std::string timeToString(int frames) {
  std::string s;
  int seconds = frames / console::FPS;
  int minutes = seconds / 60;
  int miniSeconds = (frames * 100 / console::FPS) % 100;

  return zeroPadding(std::to_string(minutes)) + ":" +
         zeroPadding(std::to_string(seconds)) + ":" +
         zeroPadding(std::to_string(miniSeconds));
}

void Game::drawCenterText(std::string s, int y) {
  int x = 1 + BOARD_WIDTH / 2 - s.length() / 2;
  console::draw(x, y, s);
}

void Game::draw() {
  // draw board bound
  console::drawBox(0, 0, BOARD_WIDTH + 1, BOARD_HEIGHT + 1);

  if (!gameOver()) {
    // draw shadow
    int dy = 0;
    current()->drawAt(SHADOW_STRING, 1 + x_, 1 + floorY());

    // draw current tetromino
    current()->drawAt(BLOCK_STRING, 1 + x_, 1 + y_);
  }

  // draw blocks
  for (int x = 0; x < BOARD_WIDTH; x++) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
      if (check(x, y)) {
        console::draw(1 + x, 1 + y, BLOCK_STRING);
      }
    }
  }

  // draw next and hold
  drawMinoPanel("Next", next_, BOARD_WIDTH + 3, 0);
  drawMinoPanel("Hold", hold_, BOARD_WIDTH + 9, 0);

  // draw lines and time
  drawCenterText(std::to_string(remainingLines_) + " lines left",
                 BOARD_HEIGHT + 2);
  drawCenterText(timeToString(elapsedFrames_), BOARD_HEIGHT + 3);

  // draw game over messages
  if (lose_) {
    drawCenterText("You Lost", 1 + BOARD_HEIGHT / 2);
  } else if (win_) {
    drawCenterText("You Win", 1 + BOARD_HEIGHT / 2);
    drawCenterText(timeToString(elapsedFrames_), 2 + BOARD_HEIGHT / 2);
  }
}

bool Game::shouldExit() { return gameOver(); }

Game::Game() : current_(nullptr), next_(randomTetromino()), hold_(nullptr) {
  for (int x = 0; x < BOARD_WIDTH; x++) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
      board_[x][y] = false;
    }
  }

  spawn();
}