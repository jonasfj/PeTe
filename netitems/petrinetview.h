#ifndef PETRINETVIEW_H
#define PETRINETVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class PetriNetView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PetriNetView(QWidget *parent = 0);

signals:

public slots:
protected:
	void wheelEvent(QWheelEvent *event);
};

#endif // PETRINETVIEW_H
