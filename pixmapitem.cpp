#include "pixmapitem.h"
#include "view.h"

PixmapItem::PixmapItem(const QPixmap& pixmap, QGraphicsItem* parent)
    : QGraphicsPixmapItem(pixmap, parent), resizing(false),  resizeMode(None), originalPixmap(pixmap)
{
    // 可移动、可选择、可聚焦，并且会在几何变化时发送信号
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemSendsGeometryChanges);
    setTransformationMode(Qt::SmoothTransformation);    // 平滑缩放和变换
    setAcceptHoverEvents(true);                         // 启用鼠标悬停事件

    oldRect = boundingRect();
    oldPos = pos();  // 记录初始位置
}

int PixmapItem::type() const
{
    return Type;
}

void PixmapItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    resizeMode = getResizeMode(event->pos());   // 设置缩放模式
    if (resizeMode != None)
    {
        resizing = true;                        // 设置可缩放和拖动
        lastMousePos = event->scenePos();       // 记录鼠标位置
        setCursor(Qt::SizeAllCursor);           // 改变光标
    }
    else
    {
        QGraphicsPixmapItem::mousePressEvent(event);
    }
}

void PixmapItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (resizing)
    {
        QPointF delta = event->scenePos() - lastMousePos;   // 计算鼠标移动的增量
        QRectF newRect = boundingRect();
        // 根据缩放模式调整矩形边界
        if (resizeMode == TopLeft)
        {
            newRect.setTopLeft(newRect.topLeft() + delta);
        }
        else if (resizeMode == TopRight)
        {
            newRect.setTopRight(newRect.topRight() + delta);
        }
        else if (resizeMode == BottomLeft)
        {
            newRect.setBottomLeft(newRect.bottomLeft() + delta);
        }
        else if (resizeMode == BottomRight)
        {
            newRect.setBottomRight(newRect.bottomRight() + delta);
        }
        // 确保矩形的长度和宽度不小于50
        if (newRect.width() >= 50 && newRect.height() >= 50)
        {
            changeRect(newRect);
        }
        // 更新鼠标位置，便于下次更新鼠标移动的增量
        lastMousePos = event->scenePos();
    }
    else
    {
        QGraphicsPixmapItem::mouseMoveEvent(event);
    }
}

void PixmapItem::changeRect(QRectF newRect)
{
    prepareGeometryChange();
    setPixmap(originalPixmap.scaled(newRect.size().toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void PixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (resizing)
    {
        resizing = false;           // 取消缩放
        setCursor(Qt::ArrowCursor); // 恢复光标
    }
    else
    {
        QGraphicsPixmapItem::mouseReleaseEvent(event);
    }
    // 大小不同时加入撤销重做栈
    if (oldRect != boundingRect())
    {
        qobject_cast<View*>(this->scene()->parent())->operationStack->addOperation(new ChangeRectOperation(oldRect, boundingRect(), this));
    }
}

void PixmapItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    ResizeMode mode = getResizeMode(event->pos());
    if (mode != None)
    {
        setCursor(Qt::SizeAllCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

PixmapItem::ResizeMode PixmapItem::getResizeMode(const QPointF &pos) const
{
    QRectF topLeftRect = getResizeRect(TopLeft);
    QRectF topRightRect = getResizeRect(TopRight);
    QRectF bottomLeftRect = getResizeRect(BottomLeft);
    QRectF bottomRightRect = getResizeRect(BottomRight);

    if (topLeftRect.contains(pos))
    {
        return TopLeft;
    }
    else if (topRightRect.contains(pos))
    {
        return TopRight;
    }
    else if (bottomLeftRect.contains(pos))
    {
        return BottomLeft;
    }
    else if (bottomRightRect.contains(pos))
    {
        return BottomRight;
    }
    else
    {
        return None;
    }
}

QRectF PixmapItem::getResizeRect(ResizeMode mode) const
{
    QRectF rect = boundingRect();
    if (mode == TopLeft)
    {
        return QRectF(rect.topLeft(), QSizeF(resizeHandleSize, resizeHandleSize));
    }
    else if (mode == TopRight)
    {
        return QRectF(rect.topRight() - QPointF(resizeHandleSize, 0), QSizeF(resizeHandleSize, resizeHandleSize));
    }
    else if (mode == BottomLeft)
    {
        return QRectF(rect.bottomLeft() - QPointF(0, resizeHandleSize), QSizeF(resizeHandleSize, resizeHandleSize));
    }
    else if (mode == BottomRight)
    {
        return QRectF(rect.bottomRight() - QPointF(resizeHandleSize, resizeHandleSize), QSizeF(resizeHandleSize, resizeHandleSize));
    }
    return QRectF();
}
