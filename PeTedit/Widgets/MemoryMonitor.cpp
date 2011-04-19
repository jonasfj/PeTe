#include "MemoryMonitor.h"

#include <proc/readproc.h>

MemoryMonitor::MemoryMonitor(QWidget *parent) :
    QLabel(parent)
{
	startTimer(500);
}

void MemoryMonitor::timerEvent(QTimerEvent *){
	struct proc_t usage;
	look_up_our_self(&usage);
	setText(tr("%1 MiB").arg(usage.vsize/ (1<<20)));
}
