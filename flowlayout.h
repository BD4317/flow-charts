#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QStyle>
#include <QWidget>

class FlowLayout : public QLayout
{
public:
    explicit FlowLayout(QWidget* parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~FlowLayout() override;

    void addItem(QLayoutItem* item) override;               // 添加按钮到布局上
    int count() const override;                             // 返回按钮的数量
    QLayoutItem* itemAt(int index) const override;          // 返回指定索引处的按钮
    QLayoutItem* takeAt(int index) override;                // 移除并返回指定索引处的按钮
    Qt::Orientations expandingDirections() const override;  // 返回布局在各个方向上的扩展
    bool hasHeightForWidth() const override;                // 表示布局与高度和宽度有关
    int heightForWidth(int) const override;                 // 根据高度设置合适的宽度
    void setGeometry(const QRect &rect) override;           // 设置布局的几何形状，并根据新几何形状调整所有布局项的位置
    QSize sizeHint() const override;                        // 返回布局的建议大小
    QSize minimumSize() const override;                     // 计算并返回布局的最小大小
    int horizontalSpacing() const;                          // 水平间距
    int verticalSpacing() const;                            // 垂直间距

private:
    QList<QLayoutItem*> itemList;
    int hSpace;                                             // 水平间距
    int vSpace;                                             // 垂直间距

    int doLayout(const QRect &rect, bool testOnly) const;   // 核心布局算法，实现了水平排列
    int smartSpacing(QStyle::PixelMetric pm) const;         // 根据父控件的样式计算智能间距
};

#endif // FLOWLAYOUT_H
