#ifndef PETRINETVIEW_H
#define PETRINETVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class PetriNetView : public QGraphicsView
{
    Q_OBJECT
public:
	explicit PetriNetView(QWidget *parent = 0);
	qreal currentScale() const { return transform().m11(); }
	void scaleBy(qreal factor);
	static const qreal minScale = 0.1;
	static const qreal maxScale = 100;
signals:
	void zoomChanged(double percent);
public slots:
	void setZoom(double percent);
private:
protected:
	void wheelEvent(QWheelEvent *event);
};

#endif // PETRINETVIEW_H
