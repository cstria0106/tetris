
#ifndef GAME_H
#define GAME_H

#include "tetromino.h"

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define LINES 40

#define DROP_DELAY 60

class Game {
private:
  // 게임 판을 나타내는 배열
  // board[x][y]가 true 인 경우 x, y 위치에 고정된 블록이 존재하는 것을 의미한다
  bool board_[BOARD_WIDTH][BOARD_HEIGHT];

  // 남은 라인 수
  int remainingLines_ = LINES;
  // 진행한 프레임
  int elapsedFrames_ = 0;
  // 패배 여부
  bool lose_ = false;
  // 승리 여부
  bool win_ = false;
  bool gameOver() { return lose_ || win_; }

  // 현재 테트로미노
  Tetromino *current_;
  // 넥스트
  Tetromino *next_;
  // 홀드
  Tetromino *hold_;

  // 홀드가 사용되었는지 여부
  bool holdUsed_;

  // 현재 테트로미노의 위치
  int x_, y_;

  // 현재 테트로미노의 위치를 중앙 상단으로 정렬한다
  void resetPosition();

  // 테트로미노가 소환돼있는지 확인한다
  bool spawned();
  // 테트로미노를 소환한다. 이미 존재하는 경우 그것을 지우고 새로 생성한다
  void spawn();
  // 현재 테트로미노를 지운다.
  void despawn();
  // 현재 테트로미노를 포인터로 구한다. 다만 현재 테트로미노가 없을 시 새로
  // 소환하여 반환한다.
  Tetromino *current();
  // 현재 테트로미노를 변경한다. 기존 테트로미노가 있다면 제거한다.
  void setCurrent(Tetromino *tetromino);

  // 현재 블록을 보드에 고정한 후 새로운 테트로미노를 소환한다
  void freeze();
  // 소프트 드롭
  void softDrop();
  // 하드 드롭
  void hardDrop();
  // 현재 테트로미노가 바닥에 닿았을 때 y 위치를 구한다
  int floorY();
  // 홀드
  void hold();

  // y 라인이 꽉 차있는지 확인한다
  bool checkYFilled(int y);
  // y 라인을 삭제한다
  void clearLine(int y);
  // 보드를 검사하여 꽉 찬 라인을 찾아 삭제한다
  void scanAndClear();

  // 소프트 드롭 타이머. 0이 될 때 소프트 드롭을 수행하고 다시 초기화한다
  int dropTimer_ = DROP_DELAY;

  // tetromino가 x, y 위치에 있을 경우 벽이나 고정된 블록에 충돌할 지 검사한다
  bool check(Tetromino &tetromino, int x, int y);
  // x, y 위치에 벽이나 고정된 블록이 있는지 검사한다
  bool check(int x, int y);

  // 텍스트를 보드와 x 가운데 정렬하여 그린다
  void drawCenterText(std::string s, int y);
  // 테트로미노를 표시하는 패널을 그린다
  void drawMinoPanel(std::string title, Tetromino *tetromino, int x, int y);

public:
  // 게임의 한 프레임을 처리한다.
  void update();

  // 게임 화면을 그린다.
  void draw();

  // 게임 루프가 종료되어야 하는지 여부를 반환한다.
  bool shouldExit();

  Game();

  // 예제 용 코드
  void setLines(int lines) { remainingLines_ = lines; }
};
#endif