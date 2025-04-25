#pragma once
#include <QWidget>
#include <QVector>
#include <QPointF>

class GraphWidget : public QWidget {
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = nullptr);
    void setText(QString s) {return;}
    void paintEvent(QPaintEvent *event) override;
};

