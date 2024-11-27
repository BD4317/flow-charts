#ifndef PIXMAPITEM_H
#define PIXMAPITEM_H

#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QPainter>

class PixmapItem : public QGraphicsPixmapItem
{
public:
    PixmapItem(const QPixmap& pixmap, QGraphicsItem* parent = nullptr);

    enum { Type = UserType + 5 };

    int type() const override;
    void changeRect(QRectF newRect);                                    // 改变形状
protected:
    enum ResizeMode { None, TopLeft, TopRight, BottomLeft, BottomRight };

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;     // 鼠标点击事件
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;      // 鼠标移动事件
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;   // 鼠标松开事件
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;      // 鼠标悬停事件

private:
    bool resizing;                                                      // 是否可拖动和缩放
    ResizeMode resizeMode;                                              // 拖动模式
    QPointF lastMousePos;                                               // 鼠标上次出现的位置
    QPixmap originalPixmap;                                             // 用于保存原始的高分辨率图像
    QRectF oldRect;                                                     // 形状
    QPointF oldPos;
    ResizeMode getResizeMode(const QPointF &pos) const;                 // 获取拖动模式
    QRectF getResizeRect(ResizeMode mode) const;                        // 获取缩放矩形

    const int resizeHandleSize = 10;
};

#endif // PIXMAPITEM_H
