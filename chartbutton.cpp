#include "chartbutton.h"

ChartButton::ChartButton(QWidget *parent)
    : QToolButton(parent)
{}

// 鼠标按下事件处理函数
void ChartButton::mousePressEvent(QMouseEvent* event)
{
    QMimeData* mimeData = new QMimeData;                            // 创建一个QMimeData对象，用于存储拖放操作的数据
    mimeData->setText(this->objectName());                          // 设置mineData的文本部分为当前按钮的对象名
    QPixmap pixmap = this->icon().pixmap(this->iconSize());         // 获取按钮的图标并转换为QPixmap对象
    QByteArray itemData;                                            // 初始化一个字节数组来存储图像数据和鼠标位置
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << pixmap << QPoint(event->pos());                   // 将QPixmap对象和鼠标点击位置写入数据流中
    mimeData->setData(this->objectName(), itemData);                // 将字节数组作为自定义类型的数据添加到MIME数据中
    emit hasSelectChart(FlowEnumItem(mimeData->text().toInt()));    // 发送标识选中图形为何种图形的信号
    QDrag* drag = new QDrag(this);                                  // 创建一个QDrag对象用于处理拖放操作
    drag->setMimeData(mimeData);                                    // 设置拖动操作的数据
    drag->setPixmap(pixmap);                                        // 设置拖动时显示的图像
    drag->setHotSpot(event->pos());                                 // 设置拖动的热区为鼠标按下的位置
    drag->exec(Qt::MoveAction);                                     // 执行拖动操作，允许移动操作
}
