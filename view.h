#ifndef VIEW_H
#define VIEW_H

#include <QPushButton>
#include <QIcon>
#include <QMimeData>
#include <QShortcut>

#include "scene.h"
#include "operationstack.h"

class View: public QGraphicsView
{
     Q_OBJECT
public:
    View(QWidget *parent = nullptr);

    OperationStack* operationStack;                                     // 撤销栈和重做栈
    Scene* graphicsScene;                                               // 界面
    double scaleMultiple;                                               // 缩放的倍数

    void scaleByWheel(double scaleX,double scaleY, QPointF position);   // 滚轮引发的缩放
    void addChartItem(FlowEnumItem type, QPointF position);             // 添加图形
    void setScene(Scene *scene);                                        // 设置布局
    void updateButtonPosition();                                        // 撤销和重做按钮位置实时更新

protected:
    void wheelEvent(QWheelEvent *event) override;                       // 滚轮事件
    void paintEvent(QPaintEvent *event) override;                       // 绘制事件
    // 拖动控件事件
    void dragEnterEvent(QDragEnterEvent *event) override;               // 拖动前
    void dragMoveEvent(QDragMoveEvent *event) override;                 // 拖动中
    void dropEvent(QDropEvent *event) override;                         // 拖动后
    // 鼠标事件
    void mousePressEvent(QMouseEvent *event) override;                  // 按下鼠标
    void mouseMoveEvent(QMouseEvent *event) override;                   // 移动鼠标
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPushButton* undoButton;                                            // 撤销按钮
    QPushButton* redoButton;                                            // 重做按钮
    QGraphicsProxyWidget* undoWidget;                                   // 放置撤销按钮的集成控件
    QGraphicsProxyWidget* redoWidget;                                   // 放置重做按钮的集成控件

    double multiple=1.25;                                               // 增加的放大倍数
    QPoint movePosition;                                                // 鼠标的位置
    QPoint pressPosition;                                               // 按下鼠标的位置
    bool isMoveView;                                                    // 视图是否在移动

public slots:
    void buttonChange(int undoCount, int redoCount);                    // 栈内操作数量改变引起按钮状态改变
    void outTextEdit();                                                 // Esc键退出文本编辑状态

signals:
   void scaleMultipleChanged(double scaleMultiple);                     // 缩放比例改变的信号
};

#endif // VIEW_H
