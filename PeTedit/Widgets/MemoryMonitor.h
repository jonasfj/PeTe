#ifndef MEMORYMONITOR_H
#define MEMORYMONITOR_H

#include <QLabel>

/** Label that displays current memory usage, by this process */
class MemoryMonitor : public QLabel
{
    Q_OBJECT
public:
    explicit MemoryMonitor(QWidget *parent = 0);

signals:

public slots:
protected:
	void timerEvent(QTimerEvent *);

};

#endif // MEMORYMONITOR_H
