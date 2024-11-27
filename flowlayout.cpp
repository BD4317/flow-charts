#include "flowlayout.h"

// 构造函数
FlowLayout::FlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent), hSpace(hSpacing), vSpace(vSpacing)
{
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing)
    : hSpace(hSpacing), vSpace(vSpacing)
{
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::~FlowLayout()
{
    QLayoutItem* item;
    while ((item = takeAt(0)))
    {
        delete item;
    }
}

void FlowLayout::addItem(QLayoutItem *item)
{
    itemList.append(item);
}

int FlowLayout::count() const
{
    return itemList.size();
}

QLayoutItem *FlowLayout::itemAt(int index) const
{
    return itemList.value(index);
}

QLayoutItem *FlowLayout::takeAt(int index)
{
    if (index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    return nullptr;
}

Qt::Orientations FlowLayout::expandingDirections() const
{
    return nullptr; // 禁止布局扩展
}

bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

int FlowLayout::heightForWidth(int width) const
{
    return doLayout(QRect(0, 0, width, 0), true);
}

void FlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize FlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
    QSize size;
    for (const QLayoutItem* item : qAsConst(itemList))
    {
        size = size.expandedTo(item->minimumSize());
    }
    const QMargins margins = contentsMargins();
    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    return size;
}

int FlowLayout::horizontalSpacing() const
{
    if (hSpace >= 0)
    {
        return hSpace; // 如果水平间距已设置，返回设置的值
    }
    else
    {
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing); // 否则，计算推荐的智能间距
    }
}

int FlowLayout::verticalSpacing() const
{
    if (vSpace >= 0)
    {
        return vSpace; // 如果垂直间距已设置，返回设置的值
    }
    else
    {
        return smartSpacing(QStyle::PM_LayoutVerticalSpacing); // 否则，计算推荐的智能间距
    }
}

int FlowLayout::doLayout(const QRect &rect, bool testOnly) const
{
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;
    int itemCount = 0; // 每行的项目计数

    for (QLayoutItem *item : qAsConst(itemList))
    {
        const QWidget *wid = item->widget();
        int spaceX = horizontalSpacing();
        if (spaceX == -1)
        {
            spaceX = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        }
        int spaceY = verticalSpacing();
        if (spaceY == -1)
        {
            spaceY = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
        }
        int nextX = x + item->sizeHint().width() + spaceX;
        // 每行最多放1个项目，超出则换行
        if (itemCount >= 1)
        {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
            itemCount = 0; // 重置每行的项目计数
        }

        if (!testOnly)
        {
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
        }
        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
        itemCount++; // 增加当前行的项目计数
    }
    return y + lineHeight - rect.y() + bottom;
}

int FlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject *parent = this->parent();
    if (!parent)
    {
        return -1;
    }
    else if (parent->isWidgetType())
    {
        QWidget* pw = static_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    }
    else
    {
        return static_cast<QLayout *>(parent)->spacing();
    }
}
