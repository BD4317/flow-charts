#ifndef CHARTITEM_H
#define CHARTITEM_H

#include <QBitmap>
#include <QUuid>
#include <QGraphicsSvgItem>
#include <qsvgrenderer.h>

#include "qpainter.h"
#include "controlpoint.h"

// 图形类型
enum FlowEnumItem
{
    StartOrEnd = 1,
    Flow1 = 2,
    Flow2 = 3,
    Judge = 4,
    Document = 5,
    Data = 6,
    SubProcess = 7,
    Contact = 8,
    Remark = 9,
    Annotation = 10,
};

// 文字说明
static const QStringList FlowTypeStrings = QStringList()
<<"流程图：开始或结束"
<<"流程图：流程"
<<"流程图：流程"
<<"流程图：判定"
<<"流程图：文档"
<<"流程图：数据"
<<"流程图：子流程"
<<"流程图：接点"
<<"流程图：备注"
<<"流程图：注释";

class ChartItem : public QGraphicsSvgItem
{
    Q_OBJECT
public:
    ChartItem(FlowEnumItem flowtype = FlowEnumItem::StartOrEnd, QGraphicsItem* parent = nullptr, QString Uid = "");

    QSvgRenderer* svgRender = nullptr;                                                                  // 数据源
    QString text;                                                                                       // 文本内容
    QString Uid = "";                                                                                   // 唯一识别id

    enum { Type = UserType + 1 };                                                                       // 类型标识

    FlowEnumItem getChartType();                                                                        // 返回图形类型
    void setChartType(FlowEnumItem type);                                                               // 设置图形类型
    QString getCurrentFillColor() const;                                                                // 返回填充的颜色
    void setCurrentFillColor(const QString& color);                                                     // 设置填充的颜色
    QString getCurrentBorderColor() const;                                                              // 返回边框颜色
    void setCurrentBorderColor(const QString& color);                                                   // 设置边框的颜色
    QString getCurrentPath();                                                                           // 获取SVG路径
    void setCurrentPath(QString currentPath);                                                           // 更新SVG路径
    int type() const override;                                                                          // 返回图片类型

protected:
    QPainterPath shape() const override;                                                                // 返回图形项的形状用于碰撞检测
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;                     // 绘制控制点
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;    // 处理图形项的变更事件


private:
    FlowEnumItem chartType;                             // 图形类型
    QString currentFillColor;                           // 当前填充颜色
    QString currentBorderColor;                         // 当前边框颜色
    QString currentSvgPath;                             // 当前数据源文本
    QPointF clickPosition;                              // 鼠标按下时的位置

    void getPolys();                                    // 获取外接轮廓

signals:
    void itemPositionHasChanged();
};

#endif // CHARTITEM_H
