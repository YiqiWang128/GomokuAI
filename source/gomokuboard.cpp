#include "gomokuboard.h"
#include <QPainter>
#include <QMouseEvent>

#include <QDebug>

GomokuBoard::GomokuBoard(QWidget *parent)
    : QWidget{parent}
{
    // 开启鼠标追踪
    setMouseTracking(true);
    for(int i=0;i<m_boardSize;i++)
    {
        for(int j=0;j<m_boardSize;j++)
            m_board[i][j]=0;
    }
}

// 绘图事件
void GomokuBoard::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    // 设置抗锯齿
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制棋盘，考虑边缘
    for(int i=0;i<m_boardSize;i++)
    {
        //绘制竖线 (y轴不变)
        painter.drawLine(m_margin + i * m_cellSize, m_margin, m_margin + i * m_cellSize, m_margin + (m_boardSize - 1) * m_cellSize);
        //绘制横线 (x轴不变)
        painter.drawLine(m_margin, m_margin + i * m_cellSize, m_margin + (m_boardSize - 1) * m_cellSize, m_margin + i * m_cellSize);
    }

    // 绘制鼠标移动悬停的提示
    if(m_isHovering && m_hoverRow>=0 && m_hoverCol>=0 && m_hoverRow<m_boardSize && m_hoverCol<m_boardSize && m_board[m_hoverRow][m_hoverCol]==0)
    {
        painter.setBrush(Qt::red);
        painter.drawEllipse(m_margin+m_hoverCol*m_cellSize-m_cellSize/8, m_margin+m_hoverRow*m_cellSize-m_cellSize/8, m_cellSize/4, m_cellSize/4);
    }

    // 绘制棋子
    for(int row=0;row<m_boardSize;row++)
    {
        for(int col=0;col<m_boardSize;col++)
        {
            if(m_board[row][col]==1)
            {
                // 玩家
                painter.setBrush(Qt::black);
                painter.drawEllipse(m_margin+col*m_cellSize-m_cellSize/6, m_margin+row*m_cellSize-m_cellSize/6, m_cellSize/3, m_cellSize/3);

            }
            else if(m_board[row][col]==2)
            {
                // 人机
                painter.setBrush(Qt::white);
                painter.drawEllipse(m_margin+col*m_cellSize-m_cellSize/6, m_margin+row*m_cellSize-m_cellSize/6, m_cellSize/3, m_cellSize/3);
            }
        }
    }

}

// 鼠标移动事件
void GomokuBoard::mouseMoveEvent(QMouseEvent * event)
{
    // QT旧版本写法
    //int posX = event->x();
    //int posY = event->y();

    // 使用position()获取坐标，并通过toPoint()转换为整数坐标
    QPoint pos = event->position().toPoint();

    int col = ( pos.x() - m_margin + m_cellSize/2 ) / m_cellSize;
    int row = ( pos.y() - m_margin + m_cellSize/2 ) / m_cellSize;

    // qDebug() << "posX:" << posX << "posY:" << posY;
    // 判断坐标是否位于棋盘内部
    if (row>=0 && row<m_boardSize && col>=0 && col<m_boardSize )
    {
        // 只有坐标改变的时候，才会更新
        if(m_hoverRow!=row || m_hoverCol!=col)
        {
            m_hoverRow = row;
            m_hoverCol = col;
            m_isHovering = true;
            update();
        }
    }
    else
    {
        if(m_isHovering == true)
        {
            m_isHovering = false;
            update();
        }
    }
}


void GomokuBoard::mousePressEvent(QMouseEvent * event)
{
    // 使用position()获取坐标，并通过toPoint()转换为整数坐标
    QPoint pos = event->position().toPoint();

    int col = ( pos.x() - m_margin + m_cellSize/2 ) / m_cellSize;
    int row = ( pos.y() - m_margin + m_cellSize/2 ) / m_cellSize;

    if(row>=0 && row<m_boardSize && col>=0 && col<m_boardSize)
    {
        // 发送信号
        emit cellClicked(row,col);
    }

}

int GomokuBoard::getBoardSize()
{
    return m_boardSize;
}
int GomokuBoard::getCellSize()
{
    return m_cellSize;
}
void GomokuBoard::setBoard(int row,int col,int player)
{
    this->m_board[row][col]=player;
}

int GomokuBoard::getRowColBoard(int row,int col)
{
    return this->m_board[row][col];
}

bool GomokuBoard::checkDirection(int row, int col, int dRow, int dCol, int player) {
    int count = 0;
    for (int i = 0; i < 5; ++i) {
        int r = row + i * dRow;
        int c = col + i * dCol;
        if (r >= 0 && r < getBoardSize() && c >= 0 && c < getBoardSize() &&
            getRowColBoard(r, c) == player) {
            count++;
        } else {
            break;
        }
    }
    return count == 5;
}

bool GomokuBoard::checkWin(int player) {
    int boardSize = getBoardSize();
    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            if (getRowColBoard(row, col) == player) {
                // 检查横向、纵向、两条斜向是否连线
                if (checkDirection(row, col, 1, 0, player) ||  // 横向
                    checkDirection(row, col, 0, 1, player) ||  // 纵向
                    checkDirection(row, col, 1, 1, player) ||  // 左上到右下
                    checkDirection(row, col, 1, -1, player)) { // 右上到左下
                    return true; // 某方胜利
                }
            }
        }
    }
    return false; // 没有胜利
}

bool GomokuBoard::isEmpty(int row, int col) const {
    if (row < 0 || row >= m_boardSize || col < 0 || col >= m_boardSize) return false;
    return m_board[row][col] == 0;
}
