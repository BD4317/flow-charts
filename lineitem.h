#ifndef LINEITEM_H
#define LINEITEM_H

#include <QGraphicsLineItem>
#include <qmath.h>

#include "chartitem.h"

class LineItem : public QObject , public QGraphicsLineItem
{
      Q_OBJECT
public:
    LineItem(ChartItem* startItem, ChartItem* endItem, QGraphicsItem* parent = nullptr);

    QString Uid="";                                                                                     // 唯一识别id
    QColor color;                                                                                       // 颜色
    ChartItem* startItem;                                                                               // 起始图形
    ChartItem* endItem;                                                                                 // 终止图形

    enum { Type = UserType + 3 };                                                                       // 类型标识

    int type() const override;
    QPointF getBoundedIntersection(ChartItem * _startitem,QLineF line);                                 // 获取中心对线的相交线

protected:
    QRectF boundingRect() const override;                                                               // 返回边界矩形
    QPainterPath shape() const override;                                                                // 返回图形的形状，用于碰撞检测
    void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;    // 绘制图形
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;                               // 处理鼠标双击事件

private:
    QPolygonF arrowHead;                                                                                // 箭头
    QLineF lines;                                                                                       // 线

signals:
    void doubleClickItem();                                                                             // 双击事件
    void itemPositionHasChanged();                                                                      // 位置改变
};

#endif // LINEITEM_H
