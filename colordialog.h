#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QColor>

class ColorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColorDialog(QWidget *parent = nullptr);
    QColor getSelectedColor() const;

private slots:
    void onColorSelected(const QString &colorName);

private:
    QPushButton *whiteButton, *redButton, *yellowButton, *greenButton;
    QColor selectedColor;
};

#endif // COLORDIALOG_H
