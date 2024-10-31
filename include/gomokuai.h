#ifndef GOMOKUAI_H
#define GOMOKUAI_H

#include "gomokuboard.h"
#include <set>
#include <array>
#include <unordered_map>
#include <vector>

class GomokuAI
{
public:

    int searchDepth;
    int killSearchDepth;

    GomokuAI(int depth1,int depth2)
    {
        searchDepth = depth1;
        killSearchDepth = depth2;
        initChessStatus();
    }

    // Minimax算法接口，depth为搜索深度，返回最佳的落子位置(row, col)
    std::pair<int, int> getBestMove(GomokuBoard* board, int depth, int killSearchDepth);

private:
    // Minimax 算法，带 alpha-beta 剪枝
    int minimax(GomokuBoard* board, int depth, int alpha, int beta, bool isMaximizingPlayer);

    // 评估当前棋盘的得分
    int evaluateBoard(GomokuBoard* board);

    // 评估某一方向的连珠情况
    int evaluateDirection(GomokuBoard* board, int row, int col, int dRow, int dCol);

    // 获取棋盘上所有棋子周围的空位
    std::vector<std::pair<int, int>> getRoundCells(GomokuBoard* board);

    // 检查是否有一方胜利
    bool checkWin(GomokuBoard* board, int player);

    // 算杀
    std::pair<int, int> checkKillMove(GomokuBoard* board, int player);

    // 初始化chessStatus数组
    void initChessStatus();

    // 定义可能的状态
    enum State {
        WIN,
        LOSE,
        FLEX4,
        flex4,
        BLOCK4,
        block4,
        FLEX3,
        flex3,
        BLOCK3,
        block3,
        FLEX2,
        flex2,
        BLOCK2,
        block2,
        FLEX1,
        flex1,
        NONE // 表示中性状态
    };

    std::unordered_map<State, int> stateScoreMap;
    std::array<State, 4096> chessStatus{}; // 4^6 种可能的模式

    // 将棋盘上相邻六个棋子的情况映射为一个整数，作为索引（index）
    constexpr int getIndex(int p1, int p2, int p3, int p4, int p5, int p6) const {
        return p1 + p2 * 4 + p3 * 16 + p4 * 64 + p5 * 256 + p6 * 1024;
    }

    // 将棋盘上相邻六个棋子的情况映射为一个整数，作为索引（index），再映射为状态（比如冲四 or 活三）
    void setPatterns(const std::vector<std::vector<int>>& patterns, State state) {
        for (const auto& pattern : patterns) {
            chessStatus[getIndex(pattern[0], pattern[1], pattern[2], pattern[3], pattern[4], pattern[5])] = state;
        }
    }


    // 算杀
    std::vector<std::pair<int, int>> getThreateningMoves(GomokuBoard* board);
    bool isThreateningMove(GomokuBoard* board, int row, int col);
    bool evaluateThreatInDirection(GomokuBoard* board, int row, int col, int dRow, int dCol);
    int evaluateKillInDirection(GomokuBoard* board, int row, int col, int dRow, int dCol);
    int killSearchMinimax(GomokuBoard* board, int killSearchDepth, int alpha, int beta, bool isMaximizingPlayer);

    // ai 下了这个位置则必胜
    struct killSearchResult{
        int flag; // 1
        std::pair<int,int> bestMove;
    };

    // ai 下了这个位置则玩家必胜
    struct avoidSearchResult{
        int flag; // -1
        std::set<std::pair<int,int>> worstMove;
    };

    killSearchResult killSearch(GomokuBoard* board, int killSearchDepth, int player);
};

#endif // GOMOKUAI_H
