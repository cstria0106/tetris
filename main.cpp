#include "console/console.h"
#include "game.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

int main() {
  Game game;

  int lines;
  std::cout << "Set lines! > ";
  std::cin >> lines;
  game.setLines(lines);

  srand((unsigned int)time(nullptr));
  console::init();

  while (!game.shouldExit()) {
    console::clear();

    game.update();
    game.draw();

    // wait은 여기서만 호출되어야 한다.
    console::wait();
  }
}