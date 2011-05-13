/* PeTe - Petri Engine exTremE
 * Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
