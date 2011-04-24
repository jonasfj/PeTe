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
	void setZoom(int percent);
signals:
	void zoomChanged(int zoom);
public slots:

private:
	qreal minScale, maxScale;
protected:
	void wheelEvent(QWheelEvent *event);
};

#endif // PETRINETVIEW_H
