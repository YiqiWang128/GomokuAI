#include "gomokuai.h"
#include <algorithm>
#include <vector>
#include <QDebug>

const int INF = std::numeric_limits<int>::max();
const int NEG_INF = std::numeric_limits<int>::min();


// 筛选出可能造成威胁（如“冲四”或“活三”）的候选位置，下这个位置，不论造成威胁的是自己还是对手，都找出来
std::vector<std::pair<int, int>> GomokuAI::getThreateningMoves(GomokuBoard* board) {
    std::vector<std::pair<int, int>> allMoves = getRoundCells(board); // 获取所有空位
    //qDebug() << "length of allMoves:" << allMoves.size();
    std::vector<std::pair<int, int>> threatMoves;  // 存储可能有威胁的落子点

    // 遍历所有空位
    for (const auto& move : allMoves) {
        int row = move.first;
        int col = move.second;

        // 模拟当前玩家落子
        board->setBoard(row, col, 1);

        // 检查此位置是否可以形成“冲四”或“活三”
        if (isThreateningMove(board, row, col)) {
            threatMoves.push_back({row, col});  // 添加到威胁点列表
            // 撤回落子
            board->setBoard(row, col, 0);
            continue;
        }

        // 撤回落子
        board->setBoard(row, col, 0);

        // 模拟当前AI落子
        board->setBoard(row, col, 2);

        // 检查此位置是否可以形成“冲四”或“活三”
        if (isThreateningMove(board, row, col)) {
            threatMoves.push_back({row, col});  // 添加到威胁点列表
        }

        // 撤回落子
        board->setBoard(row, col, 0);
    }

    return threatMoves;  // 返回威胁点列表
}

// 判断某个落子点是否构成威胁（如形成冲四或活三）
bool GomokuAI::isThreateningMove(GomokuBoard* board, int row, int col) {
    // 检查水平方向、垂直方向、以及两个对角方向
    if (evaluateThreatInDirection(board, row, col, 1, 0) ||  // 水平
        evaluateThreatInDirection(board, row, col, 0, 1) ||  // 垂直
        evaluateThreatInDirection(board, row, col, 1, 1) ||  // 左上到右下
        evaluateThreatInDirection(board, row, col, 1, -1)) { // 右上到左下
        return true;
    }

    return false;
}

// 检查在某个方向上是否有潜在威胁（如形成冲四或活三）
bool GomokuAI::evaluateThreatInDirection(GomokuBoard* board, int row, int col, int dRow, int dCol) {

    int boardSize = board->getBoardSize();
    int pattern[6] = {0};  // 存储当前方向上的模式

    // 正向探测6格
    for (int i = 0; i < 6; ++i) {
        int newRow = row + i * dRow;
        int newCol = col + i * dCol;

        if (newRow < 0 || newRow >= boardSize || newCol < 0 || newCol >= boardSize) {
            pattern[i] = 3;  // 超出边界
        } else {
            pattern[i] = board->getRowColBoard(newRow, newCol);  // 获取棋盘上的值
        }
    }


    int status = chessStatus[getIndex(pattern[0], pattern[1], pattern[2], pattern[3], pattern[4], pattern[5])];
    //return status == WIN || status == LOSE || status == FLEX4 || status == flex4 || status == BLOCK4 || status == block4 || status == FLEX3 || status == flex3;
    return status == WIN || status == LOSE || status == FLEX4 || status == flex4 || status == BLOCK4 || status == block4;
}

// 修改Minimax算法，优先搜索威胁点
int GomokuAI::killSearchMinimax(GomokuBoard* board, int killSearchDepth, int alpha, int beta, bool isMaximizingPlayer) {

    // 如果某方胜利或达到最大深度，停止递归
    if (checkWin(board, 2)) { // AI 胜利
        return 1;
    } else if (checkWin(board, 1)) { // 玩家胜利
        return -1;
    } else if (killSearchDepth == 0) {
        return 0;
    }


    std::vector<std::pair<int, int>> candidateMoves = getThreateningMoves(board);

    // 如果没有威胁点，则不必算杀
    if (candidateMoves.empty()) {
        return 0;
    }

    //qDebug() << "there is a threatening move";

    if (isMaximizingPlayer) {
        int maxEval = NEG_INF;
        for (const auto& move : candidateMoves) {
            int row = move.first;
            int col = move.second;
            board->setBoard(row, col, 2); // AI 落子

            int eval = killSearchMinimax(board, killSearchDepth - 1, alpha, beta, false);
            board->setBoard(row, col, 0); // 撤回落子
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) {
                break; // 剪枝
            }
        }
        return maxEval;
    } else {
        int minEval = INF;
        for (const auto& move : candidateMoves) {
            int row = move.first;
            int col = move.second;
            board->setBoard(row, col, 1); // 玩家落子

            int eval = killSearchMinimax(board, killSearchDepth - 1, alpha, beta, true);
            board->setBoard(row, col, 0); // 撤回落子
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                break; // 剪枝
            }
        }
        return minEval;
    }
}


// 返回第一个值为1、0、-1，第二个值为下棋坐标
// player=2 则是ai尝试走杀棋推演，否则是玩家
GomokuAI::killSearchResult GomokuAI::killSearch(GomokuBoard* board, int killSearchDepth, int player)
{
    killSearchResult res; // 定义函数的返回值

    // 2. 获取候选点并进行启发式排序
    int bestScore = std::numeric_limits<int>::min();
    std::pair<int,int> bestMove = {-1, -1};
    std::vector<std::pair<int, int>> candidateMoves = getThreateningMoves(board);

    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    // 3. 使用 Minimax 算法继续搜索
    for (const auto& move: candidateMoves) {
        int row = move.first;
        int col = move.second;

        // AI(或玩家) 尝试走杀棋推演，搜索ai的必胜策略则是ai尝试走杀棋推演，搜索ai避免的必败策略则是在ai落子后的玩家尝试走杀棋推演
        board->setBoard(row, col, player);
        int score = killSearchMinimax(board, killSearchDepth - 1, alpha, beta, false);
        alpha = std::max(alpha,score);
        board->setBoard(row, col, 0);  // 撤回落子

        if (score > bestScore) {
            bestScore = score;
            bestMove = {row, col};
        }
    }


    res.flag = bestScore;
    res.bestMove = bestMove;

    return res;  // 返回最佳落子位置
}
