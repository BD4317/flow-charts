#ifndef CHARTBUTTON_H
#define CHARTBUTTON_H

#include <QObject>
#include <QWidget>
#include <QToolButton>
#include <QMimeData>
#include <QMouseEvent>
#include <QDrag>

#include "view.h"

class ChartButton : public QToolButton
{
    Q_OBJECT
public:
    ChartButton(QWidget *parent = nullptr);

protected:
    // 鼠标事件
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void hasSelectChart(FlowEnumItem type);
};

#endif // CHARTBUTTON_H
