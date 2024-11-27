#ifndef SIZEHANDLERECT_H
#define SIZEHANDLERECT_H

#include <QGraphicsRectItem>

enum RectDirection { LeftTop, LeftBtn, RightTop, RightBtn, BottomCenter};

class ControlPoint:public QObject , public QGraphicsRectItem
{
    Q_OBJECT
public:
    RectDirection rectdirection;
    ControlPoint(RectDirection _rectdirection,QGraphicsItem* parent = nullptr);

    enum { Type = UserType + 2 };

    int type() const override;
    void setParentItemTransform(QPointF positionTop, QPointF positionBottom);                           // 根据控制点的方向执行缩放或旋转操作
protected:
    QRectF boundingRect() const override;                                                               // 返回控制点的边界矩形
    QPainterPath shape() const override;                                                                // 绘制控制点
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;    // 返回控制点的形状

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    bool isCheck;
    QPointF clickPosition;
};

#endif // SIZEHANDLERECT_H
