#pragma once

#include <QDialog>
#include <QPixmap>
#include <QRect>

QPixmap grabScreenshot();


class SelectorWidget : public QDialog
{
    Q_OBJECT

public:
    explicit SelectorWidget(QWidget *parent = nullptr);
    ~SelectorWidget();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

public slots:
    void savePixmap();

public:
    QPixmap m_selectedPixmap;

private:
    QPixmap m_desktopPixmap;
    QRect   m_selectedRect;
    QPushButton* m_OkBtn;
};
