#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QSvgGenerator>

#include "textitem.h"
#include "pixmapitem.h"

enum Mode { NoMode, InsertChart, InsertLine, InsertText, MoveItem };

class Scene: public QGraphicsScene
{
     Q_OBJECT
public:
    Scene(QObject *parent = nullptr);
    ~Scene() override;

    FlowEnumItem flowType;                                                      // 选中的图形类型
    QList<TextItem*> allTexts;                                                  // 所有文本
    QList<LineItem*> allLines;                                                  // 所有线
    QList<TextItem*> containTexts;                                              // 包含选中文本的文本框
    int currentIndex;                                                           // 选中的文本下标
    QString currentText;                                                        // 当前查找的文本

    void saveSceneToImage(QString filename);                                    // 保存为png图片
    void saveSceneToSVG(QString filename);                                      // 保存为svg图片
    void removeAllSelect(QList<QGraphicsItem *> item);                          // 删除所有被选中的对象
    QList<TextItem*> getConnectText(QGraphicsItem* item);                       // 获取相关联的文本
    QList<LineItem*> getConnectLine(QGraphicsItem* item);                       // 获取相关联的线
    Mode getMode() const;
    void clearAllItems();  // 清除所有图形项的函数
//protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;        // 按下鼠标
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;         // 移动鼠标
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;      // 释放鼠标

    void keyPressEvent(QKeyEvent *event) override;                              // 按下键盘
    void keyReleaseEvent(QKeyEvent *event) override;                            // 松开键盘
private:
     Mode mode;                                                                 // 屏幕的模式
     QGraphicsLineItem* lineItem;                                               // 选中的连接线
     ChartItem* chartItem;                                                      // 选中的图形
     TextItem* textItem;
     PixmapItem* pixmapItem;
     QPointF startPosition;                                                     // 图形的起始位置
     QPointF endPosition;                                                       // 图形的最终位置
     QList<QGraphicsLineItem*> verticalLine;                                    // 选中框的竖线
     QList<QGraphicsLineItem*> horizontalLine;                                  // 选中框的横线
     bool isMove;                                                               // 是否在移动一个图形
     bool shiftIsClicked;                                                       // Shift是否一直按住

     double distance(QPointF pos1, QPointF pos2);                               // 计算距离

public slots:
    void setMode(Mode mode);                                                    // 设置模式
    void doubleClickItem();                                                     // 双击选中或创建文本框
};

#endif // SCENE_H
