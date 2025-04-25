#include "graphwidget.h"
#include "data.h"
#include <QPainter>
#include <QPointF>
#include <QVector>
#include <QDebug>

GraphWidget::GraphWidget(QWidget *parent) : QWidget(parent)
{
    update();
}

void GraphWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // get the data points from the Data singleton
    const QVector<CgmReading> &readings = Data::getInstance().getCgmReadings();

    if (readings.isEmpty())
    {
        // Draw axes and labels even if no data points
        painter.drawLine(40, height() - 30, width() - 10, height() - 30);
        painter.drawLine(40, 10, 40, height() - 30);
        double yMin = 0;
        double yMax = 15;
        double y10 = height() - 30 - (10 - yMin) * (height() - 40) / (yMax - yMin == 0 ? 1 : yMax - yMin);
        double y4 = height() - 30 - (4 - yMin) * (height() - 40) / (yMax - yMin == 0 ? 1 : yMax - yMin);
        painter.drawText(10, 20, "Glucose");
        painter.drawText(10, height() - 20, "0");
        painter.drawText(10, y4 + 5, "4");
        painter.drawText(10, y10 + 5, "10");
        painter.drawText(width() / 2, height() - 10, "Time");
        painter.setPen(Qt::DashLine);
        painter.drawLine(40, y10, width() - 10, y10);
        painter.drawLine(40, y4, width() - 10, y4);
        return;
    }

    // Draw axes
    painter.drawLine(40, height() - 30, width() - 10, height() - 30);
    painter.drawLine(40, 30, 40, height() - 30);

    double yMin = 0;
    double yMax = 15;

    double y10 = height() - 30 - (10 - yMin) * (height() - 40) / (yMax - yMin == 0 ? 1 : yMax - yMin);
    double y4 = height() - 30 - (4 - yMin) * (height() - 40) / (yMax - yMin == 0 ? 1 : yMax - yMin);

    // Set Axis Labels
    painter.drawText(10, 20, "Glucose");
    painter.drawText(10, height() - 20, "0");
    painter.drawText(10, y4 + 5, "4");
    painter.drawText(10, y10 + 5, "10");
    painter.drawText(width() / 2, height() - 10, "Time"); // Adjust Y slightly if it overlaps axis

    // Set normal Bounds
    painter.setPen(Qt::DashLine);
    painter.drawLine(40, y10, width() - 10, y10);
    painter.drawLine(40, y4, width() - 10, y4);

    int graphHours = Data::getInstance().getGraphHours();
    int N = graphHours * 20;

    // Determine the visible portion of the data
    int dataSize = readings.size();
    int startIdx = std::max(0, dataSize - N);
    int visibleCount = dataSize - startIdx;

    if (visibleCount <= 0)
        return;

    int plotWidth = width() - 50;
    int plotHeight = height() - 40;

    // Painting the graph
    QVector<QPointF> pixelPoints;

    // Calculate pixel coordinates for all visible points
    for (int i = startIdx; i < dataSize; ++i)
    {
        const CgmReading &reading = readings[i];
        float pt_y_value = reading.value;
        int idx = i - startIdx;

        double x = 40.0;
        if (visibleCount > 1) {
            x = 40.0 + (double(idx) * plotWidth) / (visibleCount - 1.0);
        } else {
            x = 40.0 + plotWidth / 2.0;
        }

        double y = (height()-30) - (pt_y_value - yMin) * plotHeight / (yMax - yMin == 0 ? 1 : yMax - yMin);

        pixelPoints.append(QPointF(x, y));
    }


    // Draw line segments and points based on connection status
    for (int i = 0; i < visibleCount; ++i) {
        const CgmReading &reading = readings[startIdx + i];
        if (reading.wasConnected)
             painter.setBrush(Qt::blue); // Fill dots with blue if connected
        else
             painter.setBrush(Qt::gray); // Fill dots with gray if disconnected
        painter.setPen(Qt::NoPen); // No border for dots
        painter.drawEllipse(pixelPoints[i], 2, 2);

        // Draw line segment from previous point to current point (if exists)
        if (i > 0) {
            // Color the segment based on the current point's connection status
            if (reading.wasConnected)
                 painter.setPen(Qt::blue); // Blue line segment if connected
            else
                 painter.setPen(Qt::gray); // Grey line segment if disconnected

            painter.setBrush(Qt::NoBrush); // Don't fill the line segment
            painter.drawLine(pixelPoints[i-1], pixelPoints[i]);
        }
    }
}