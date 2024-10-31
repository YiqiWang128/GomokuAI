#ifndef GOMOKUBOARD_H
#define GOMOKUBOARD_H

#include <QObject>
#include <QWidget>

class GomokuBoard : public QWidget
{
    Q_OBJECT
public:
    explicit GomokuBoard(QWidget *parent = nullptr);

    static int getBoardSize();
    static int getCellSize();
    int getRowColBoard(int row,int col);
    void setBoard(int row,int col,int player);
    bool checkDirection(int row, int col, int dRow, int dCol, int player);
    bool checkWin(int player);
    bool isEmpty(int row, int col) const;

signals:
    void cellClicked(int row,int col);

protected:
    // 绘图事件
    void paintEvent(QPaintEvent * event) override;
    // 鼠标移动事件
    void mouseMoveEvent(QMouseEvent * event) override;
    // 鼠标点击事件
    void mousePressEvent(QMouseEvent * event) override;

private:
    static const int m_boardSize = 15; // 棋盘有15条线
    static const int m_cellSize = 40; // 棋盘格子的大小 //40
    // 棋盘的边缘，大小是棋盘格子大小的一半
    static const int m_margin = m_cellSize >> 1;
    // 交点的棋子：0表示没有棋子，1表示玩家，2表示人机
    int m_board[m_boardSize][m_boardSize];

    // 悬停的横坐标
    int m_hoverRow;
    // 悬停的纵坐标
    int m_hoverCol;
    // 悬停
    bool m_isHovering;
};

#endif // GOMOKUBOARD_H
