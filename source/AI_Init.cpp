#include <array>
#include <unordered_map>
#include <vector>
#include <gomokuai.h>



void GomokuAI::initChessStatus() {
    // 从状态映射到得分
    stateScoreMap = {
        {WIN, 100000},
        {LOSE, -100000000},
        {FLEX4, 50000},
        {flex4, -1000000},
        {BLOCK4, 900},
        {block4, -200000},
        {FLEX3, 400},
        {flex3, -80000},
        {BLOCK3, 20},
        {block3, -50},
        {FLEX2, 20},
        {flex2, -50},
        {BLOCK2, 1},
        {block2, -3},
        {FLEX1, 1},
        {flex1, -3},
        {NONE, 0}
    };

    chessStatus.fill(NONE); // 初始化为某一个状态，如果后面检测到是这个状态，那就是没赋值

    std::vector<std::vector<int>> winPatterns = {
        {2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 0}, {0, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 1}, {1, 2, 2, 2, 2, 2}, {3, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 3}
    };
    setPatterns(winPatterns, WIN);

    std::vector<std::vector<int>> losePatterns = {
        {1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 0}, {0, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 2}, {2, 1, 1, 1, 1, 1}, {3, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 3}
    };
    setPatterns(losePatterns, LOSE);


    setPatterns({{0, 2, 2, 2, 2, 0}}, FLEX4);
    setPatterns({{0, 1, 1, 1, 1, 0}}, flex4);


    std::vector<std::vector<int>> flex3Patterns = {
        {0, 2, 2, 2, 0, 0}, {0, 0, 2, 2, 2, 0},
        {0, 2, 0, 2, 2, 0}, {0, 2, 2, 0, 2, 0}
    };
    setPatterns(flex3Patterns, FLEX3);

    std::vector<std::vector<int>> flex3OpponentPatterns = {
        {0, 1, 1, 1, 0, 0}, {0, 0, 1, 1, 1, 0},
        {0, 1, 0, 1, 1, 0}, {0, 1, 1, 0, 1, 0}
    };
    setPatterns(flex3OpponentPatterns, flex3);


    std::vector<std::vector<int>> flex2Patterns = {
        {0, 2, 2, 0, 0, 0}, {0, 2, 0, 2, 0, 0}, {0, 2, 0, 0, 2, 0},
        {0, 0, 2, 2, 0, 0}, {0, 0, 2, 0, 2, 0}, {0, 0, 0, 2, 2, 0}
    };
    setPatterns(flex2Patterns, FLEX2);

    std::vector<std::vector<int>> flex2OpponentPatterns = {
        {0, 1, 1, 0, 0, 0}, {0, 1, 0, 1, 0, 0}, {0, 1, 0, 0, 1, 0},
        {0, 0, 1, 1, 0, 0}, {0, 0, 1, 0, 1, 0}, {0, 0, 0, 1, 1, 0}
    };
    setPatterns(flex2OpponentPatterns, flex2);


    std::vector<std::vector<int>> flex1Patterns = {
        {0, 2, 0, 0, 0, 0}, {0, 0, 2, 0, 0, 0},
        {0, 0, 0, 2, 0, 0}, {0, 0, 0, 0, 2, 0}
    };
    setPatterns(flex1Patterns, FLEX1);

    std::vector<std::vector<int>> flex1OpponentPatterns = {
        {0, 1, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0},
        {0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 1, 0}
    };
    setPatterns(flex1OpponentPatterns, flex1);

    // 初始化其余的模式
    for (int p1 = 0; p1 < 4; ++p1) {
        for (int p2 = 0; p2 < 4; ++p2) {
            for (int p3 = 0; p3 < 4; ++p3) {
                for (int p4 = 0; p4 < 4; ++p4) {
                    for (int p5 = 0; p5 < 4; ++p5) {
                        for (int p6 = 0; p6 < 4; ++p6) {
                            // 跳过已经初始化的模式
                            if (chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] != NONE) continue;

                            int x = 0, y = 0, ix = 0, iy = 0, z = 0, iz = 0;

                            auto count = [&](int p, bool isStart) {
                                if (p == 1) { x++; if (!isStart) ix++; }
                                else if (p == 2) { y++; if (!isStart) iy++; }
                                else if (p == 3) { z++; if (!isStart) iz++; }
                            };

                            count(p1, true);
                            count(p2, false);
                            count(p3, false);
                            count(p4, false);
                            count(p5, false);
                            count(p6, false);

                            bool hasLeftBoundary = (p1 == 3);
                            bool hasRightBoundary = (p6 == 3);

                            if (hasLeftBoundary || hasRightBoundary) {
                                if (hasLeftBoundary && !hasRightBoundary) {
                                    if (ix + iz == 0 && iy == 4)
                                        chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = BLOCK4;
                                    else if (iy + iz == 0 && ix == 4)
                                        chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = block4;
                                    else if (ix + iz == 0 && iy == 3)
                                        chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = BLOCK3;
                                    else if (iy + iz == 0 && ix == 3)
                                        chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = block3;
                                    else if (ix + iz == 0 && iy == 2)
                                        chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = BLOCK2;
                                    else if (iy + iz == 0 && ix == 2)
                                        chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = block2;
                                }
                                else if (hasRightBoundary && !hasLeftBoundary) {
                                    if (x + z == 0 && y == 4)
                                        chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = BLOCK4;
                                    else if (y + z == 0 && x == 4)
                                        chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = block4;
                                    else if (x + z == 0 && y == 3)
                                        chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = BLOCK3;
                                    else if (y + z == 0 && x == 3)
                                        chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = block3;
                                    else if (x + z == 0 && y == 2)
                                        chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = BLOCK2;
                                    else if (y + z == 0 && x == 2)
                                        chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = block2;
                                }
                            } else {
                                if ((x == 0 && y == 4) || (ix == 0 && iy == 4))
                                    chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = BLOCK4;
                                else if ((x == 4 && y == 0) || (ix == 4 && iy == 0))
                                    chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = block4;
                                else if ((x == 0 && y == 3) || (ix == 0 && iy == 3))
                                    chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = BLOCK3;
                                else if ((x == 3 && y == 0) || (ix == 3 && iy == 0))
                                    chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = block3;
                                else if ((x == 0 && y == 2) || (ix == 0 && iy == 2))
                                    chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = BLOCK2;
                                else if ((x == 2 && y == 0) || (ix == 2 && iy == 0))
                                    chessStatus[getIndex(p1, p2, p3, p4, p5, p6)] = block2;
                            }
                        }
                    }
                }
            }
        }
    }
}
