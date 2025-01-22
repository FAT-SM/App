#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

/* The gnuplot interface. */
class Gnuplot : public QObject {
    Q_OBJECT

private:
    QTemporaryFile _plotFile;
    QString _commandBuffer;

public:

    /* Gnuplot interface constructor. */
    explicit Gnuplot(QObject* parent = nullptr);

    /* Adds the command to the internal buffer. */
    void append(const QString& command);

    /* Sets the internal state ready for a new figure. */
    void figure(const QSize& size);

    /* Calls gnuplot with the buffered command data. */
    QPixmap plot();

};
