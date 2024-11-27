#pragma once
#ifndef OPERATION_H
#define OPERATION_H

#include <QObject>
#include <QWidget>
#include <QPointF>
#include <QGraphicsItem>
#include "textitem.h"
// 基类
class Operation : public QObject
{
    Q_OBJECT
public:
    explicit Operation(QObject* parent = nullptr);

    virtual void undo() const = 0;  // 撤销操作
    virtual void redo() const = 0;  // 重做操作
};

// 移动操作
class ViewMoveOperation : public Operation
{
    Q_OBJECT
public:
    explicit  ViewMoveOperation(QPointF oldPosition, QPointF newPosition, QObject* parent = nullptr);

    void undo() const override;
    void redo() const override;

private:
    QPointF oldPosition;
    QPointF newPosition;
};

// 缩放操作
class ScaleOperation : public Operation
{
    Q_OBJECT
public:
    explicit ScaleOperation(int delta, double multiple, QPointF position, QObject* parent = nullptr);

    void undo() const override;
    void redo() const override;

private:
    int delta;
    double multiple;
    QPointF position;
};

// 添加操作
class AppendOperation : public Operation
{
    Q_OBJECT
public:
    explicit AppendOperation(QList<QGraphicsItem*> addToItems, QObject* parent = nullptr);

    void redo() const override;
    void undo() const override;

private:
    QList<QGraphicsItem*> addToItems;
};

// 删除操作
class DeleteOperation : public AppendOperation
{
    Q_OBJECT
public:
    explicit DeleteOperation(QList<QGraphicsItem*> addToItems, QObject* parent = nullptr);

    void undo() const override;
    void redo() const override;

private:
    QList<QGraphicsItem*> addToItems;
};

class MoveOperation : public Operation
{
    Q_OBJECT
public:
    explicit MoveOperation(QPointF startPosition, QPointF endPosition, QList<QGraphicsItem*> items, QObject* parent = nullptr);

    void undo() const override;
    void redo() const override;

private:
    QList<QGraphicsItem*> moveItems;
    QPointF startPosition;
    QPointF endPosition;
};

// 改变操作
class ChangeOperation : public Operation
{
    Q_OBJECT
public:
    explicit ChangeOperation(QTransform oldTransform, QTransform newTransform, QGraphicsItem* item, QObject* parent = nullptr);

    void undo() const override;
    void redo() const override;

private:
    QGraphicsItem* item;
    QTransform oldTransform;
    QTransform newTransform;
};

// 涂色操作
class ColorOperation : public Operation
{
    Q_OBJECT
public:
    explicit ColorOperation(QList<QPair<QString, QString>> fillColors, QList<QPair<QString, QString>> borderColors, QList<QGraphicsItem*> items, QObject* parent = nullptr);

    void undo() const override;
    void redo() const override;

private:
    QList<QPair<QString, QString>> fillColors;
    QList<QPair<QString, QString>> borderColors;
    QList<QGraphicsItem*> items;
};

// 文本替换操作
class ReplacceTextOperation : public Operation
{
public:
    explicit ReplacceTextOperation(QString oldText, QString newText, QList<QGraphicsItem*> changedItems, QObject* parent = nullptr);

    void undo() const override;
    void redo() const override;

private:
    QString oldText;
    QString newText;
    QList<QGraphicsItem*> changedItems;
};

// 文本样式更改操作
class ChangeFontOperation : public Operation
{
public:
    explicit ChangeFontOperation(QFont oldFont, QColor oldColor, QFont newFont, QColor newColor, QList<QGraphicsItem*> changedItems, QObject* parent = nullptr);

    void undo() const override;
    void redo() const override;

private:
    QFont oldFont;
    QColor oldColor;
    QFont newFont;
    QColor newColor;
    QList<QGraphicsItem*> changedItems;
};

// 添加背景操作
class AppendBackgroundOperation : public Operation
{
public:
    explicit AppendBackgroundOperation(QBrush oldBackground, QBrush newBackground, QObject* parent = nullptr);

    void undo() const override;
    void redo() const override;

private:
    QBrush oldBackground;
    QBrush newBackground;
};

// 改变图片大小操作
class ChangeRectOperation : public Operation
{
public:
    explicit ChangeRectOperation(QRectF oldRect, QRectF newRect, QGraphicsItem* item, QObject* parent = nullptr);

    void undo() const override;
    void redo() const override;

private:
    QRectF oldRect;
    QRectF newRect;
    QGraphicsItem* item;
};

#endif // OPERATION_H
