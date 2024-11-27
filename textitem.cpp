#include "textitem.h"
#include "operation.h"
#include "scene.h"
#include "view.h"
#include <QDebug>

TextItem::TextItem(QGraphicsItem* parent)
    : QGraphicsTextItem(parent), text("文本"), connectItem(nullptr), Uid("")
{
    this->setAcceptHoverEvents(true);                                   // 接受悬停事件
    setFlag(QGraphicsItem::ItemIsMovable, true);                        // 设置图形项可移动
    setFlag(QGraphicsItem::ItemIsSelectable, true);                     // 设置图形项可选中
    this->Uid = QUuid::createUuid().toString(QUuid::WithoutBraces);     // 生成唯一ID
    this->setPlainText(text);                                           // 设置图形项的HTML内容

    this->setZValue(1000);                                              // 设置Z值，确保在其他图形项之上
    // 设置文本字体大小
    QFont font = this->font();
    font.setPointSize(12);                                              // 设置字体大小为12
    this->setFont(font);
}

int TextItem::type() const
{
    return Type;
}

void TextItem::setTextEditFlags(Qt::TextInteractionFlags flags)
{
    if (textInteractionFlags() == flags)
    {
        return;
    }
    switch (flags)
    {
        case Qt::NoTextInteraction:
        {
            text = this->document()->toRawText();       // 保存当前文本内容
            this->setPlainText(text);                   // 更新为HTML文本
            updatePosition();                           // 更新位置
            setTextInteractionFlags(flags);             // 设置标志
            break;
        }
        case Qt::TextEditorInteraction:
        {
            if (text != "")
            this->setPlainText(text);                   // 设置纯文本
            setTextInteractionFlags(flags);             // 设置标志
            this->setFocus();                           // 设置焦点
            break;
        }
        default:
            break;
    }
}

void TextItem::setConnectItem(QGraphicsItem* item)
{
    connectItem = item;
    this->updatePosition();
}

void TextItem::parentPositionHasChanged()
{
    updatePosition();  // 更新位置
}

void TextItem::updatePosition()
{
    if (connectItem != nullptr)                                         // 检查关联项是否存在
    {
        switch (connectItem->type())
        {
            case ChartItem::Type:                                       // 流程图项
            {
                ChartItem* flowitem = qgraphicsitem_cast<ChartItem*>(connectItem);
                QPointF flowcenter = flowitem->boundingRegion(flowitem->transform()).boundingRect().center() + flowitem->pos();
                QPointF textcenter = this->boundingRect().center() + this->pos();
                this->setPos(this->pos() + (flowcenter - textcenter));  // 调整文本位置
                break;
            }
            case LineItem::Type:                                        // 连接线项
            {
                LineItem* flowitem = qgraphicsitem_cast<LineItem*>(connectItem);
                QPointF flowcenter = flowitem->line().center();
                QPointF textcenter = this->boundingRect().center() + this->pos();
                this->setPos(this->pos() + (flowcenter - textcenter));  // 调整文本位置
                break;
            }
            default:
                break;
        }
    }
}

QVariant TextItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)    // 检查选择状态是否改变
    {
        if (!isSelected())
        {
            setTextEditFlags(Qt::NoTextInteraction);        // 禁用文本编辑
        }
    }
    return QGraphicsTextItem::itemChange(change, value);    // 返回父类处理结果
}

void TextItem::focusOutEvent(QFocusEvent* event)
{
    // 获取新的文本内容
    QString newText = this->toPlainText();

    // 如果文本内容发生变化
    if (newText != text)
    {
        // 获取父场景
        Scene* parentScene = dynamic_cast<Scene*>(this->scene());
        if (parentScene)
        {
            // 获取 View 的操作栈
            OperationStack* operationStack = qobject_cast<View*>(parentScene->parent())->operationStack;
            if (operationStack)
            {
                // 创建一个文本编辑操作并推入撤销栈
                operationStack->addOperation(new ReplacceTextOperation(text, newText, QList<QGraphicsItem*>() << this));
            }
        }

        // 更新当前文本内容
        text = newText;
    }

    // 调用父类的焦点事件处理
    QGraphicsTextItem::focusOutEvent(event);
}


void TextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsTextItem::mouseDoubleClickEvent(event);  // 调用父类的双击事件处理

    if (textInteractionFlags() == Qt::NoTextInteraction)
    {
        setTextEditFlags(Qt::TextEditorInteraction);  // 启用文本编辑
    }
}
