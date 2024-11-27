#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H
#include <QObject>
#include <qmath.h>
#include <QPointF>
#include <QColor>
#include <QFont>


// 全局变量
class Global
{
public:
    Global();

    static QFont globalFont;    // 全局字体设置
    static QColor globalColor; // 连接线的颜色和文本的颜色
};



#endif // GLOBAL_H
