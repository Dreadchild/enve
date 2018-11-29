#ifndef CURRENTGRADIENTWIDGET_H
#define CURRENTGRADIENTWIDGET_H

#include "GUI/ColorWidgets/ColorWidgets/glwidget.h"
class GradientWidget;

class CurrentGradientWidget : public GLWidget
{
    Q_OBJECT
public:
    explicit CurrentGradientWidget(GradientWidget *gradientWidget,
                                   QWidget *parent = 0);

    void paintGL();

protected:
    GradientWidget *mGradientWidget;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *);
    int mHoveredX = 0;
signals:

public slots:
};

#endif // CURRENTGRADIENTWIDGET_H