#include "SelectorWidget.h"
#include <string>
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QScreen>
#include <QMouseEvent>
#include <QPainterPath>
#include <QPushButton>

SelectorWidget::SelectorWidget(QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setGeometry(QApplication::desktop()->geometry());
    m_desktopPixmap = grabScreenshot();
    m_OkBtn = new QPushButton(this);
    m_OkBtn->setIcon(QIcon("images/ok.png"));
    m_OkBtn->hide();
    m_OkBtn->setFixedSize(32, 32);

    connect(m_OkBtn, SIGNAL(clicked()), this, SLOT(savePixmap()));
}

SelectorWidget::~SelectorWidget()
{
}

void SelectorWidget::mousePressEvent(QMouseEvent* event)
{
    m_selectedRect.setRect(0, 0, 0, 0);
    m_OkBtn->hide();
    m_selectedRect.setTopLeft(event->globalPos());
}

void SelectorWidget::mouseReleaseEvent(QMouseEvent* event)
{
    m_OkBtn->move(event->globalX(), event->globalY());
    m_OkBtn->show();
}

void SelectorWidget::mouseMoveEvent(QMouseEvent* event)
{
    m_selectedRect.setBottomRight(event->globalPos());
    update();
}

void SelectorWidget::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.save();
    p.drawPixmap(0, 0, m_desktopPixmap);

    QPainterPath path;
    path.addRect(rect());
    path.addRect(m_selectedRect);
    p.fillPath(path, QColor::fromRgb(255, 255, 255, 200));

    QPen pen(Qt::red, 2);
    p.setPen(pen);
    p.drawRect(m_selectedRect);
    p.restore();
}

void SelectorWidget::savePixmap()
{
    m_selectedPixmap = m_desktopPixmap.copy(m_selectedRect.normalized());
    m_selectedPixmap.save("shot.jpg");
    accept();
    close();
}

QPixmap grabScreenshot()
{
    QPixmap desktopPixmap = QPixmap(QApplication::desktop()->geometry().size());
    QPainter p(&desktopPixmap);

    for (QScreen* screen : QApplication::screens())
    {
        p.drawPixmap(screen->geometry().topLeft(), screen->grabWindow(0));
    }

    return desktopPixmap;
}
