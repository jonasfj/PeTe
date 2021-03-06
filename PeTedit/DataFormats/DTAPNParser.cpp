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
#include "DTAPNParser.h"

#include <QtGlobal>
#include <QDebug>
#include <QRegExp>

void DTAPNParser::parse(QIODevice *input, PetriEngine::DTAPN::AbstractDTAPNBuilder *builder){

	bool wasOpen = input->isOpen();
	if(!wasOpen)
		input->open(QIODevice::ReadOnly);
	xml.setDevice(input);
	this->builder = builder;

	idmap.clear();
	arcs.clear();

	while(xml.readNextStartElement()){
		if(xml.name() == "pnml"){
			pnml();
			break;	//Don't read more than one net
		}else
			xml.skipCurrentElement();
	}

	//Create arcs
	foreach(ArcEntry entry, arcs){
		if(!idmap.contains(entry.src)){
			qDebug()<<"Error during parse: Arc from \""<<entry.src<<"\", which is not defined";
			continue;
		}
		if(!idmap.contains(entry.dst)){
			qDebug()<<"Error during parse: Arc to \""<<entry.dst<<"\", which is not defined";
			continue;
		}
		std::string src = idmap[entry.src].name.toStdString();
		std::string dst = idmap[entry.dst].name.toStdString();
		if(idmap[entry.src].type == Place && idmap[entry.dst].type == Transition)
			builder->addInputArc(src, dst, entry.startInterval, entry.endInterval);
		else if(idmap[entry.src].type == Transition && idmap[entry.dst].type == Place)
			builder->addOutputArc(src, dst);
		else{
			//TODO: Consider reporting an error
			qDebug()<<"Error duing parse: Arc from \""<<src.c_str()<<"\" to \""<<dst.c_str()<<"\" is invalid";
		}
	}

	idmap.clear();
	arcs.clear();

	if(!wasOpen)
		input->close();
	this->builder = NULL;
}

void DTAPNParser::pnml(){
	bool readOneNet = false;
	while(xml.readNextStartElement()){
		if(xml.name() == "net" && !readOneNet){
			net();
			readOneNet = true;
		}else if(xml.name() == "query"){
			query();
		}else
			xml.skipCurrentElement();
	}
}

void DTAPNParser::net(){
	while(xml.readNextStartElement()){
		if(xml.name() == "page"){
			net();
			break;	//Don't read more than one page
		}else if(xml.name() == "place"){
			place();
		}else if(xml.name() == "transition"){
			transition();
		}else if(xml.name() == "arc"){
			arc();
		}else if(xml.name() == "queries"){
			queries();
		}else
			xml.skipCurrentElement();
	}
}

void DTAPNParser::query(){
	Query q;
	q.name = xml.attributes().value("name").toString();
	q.query = xml.attributes().value("query").toString();

	if(q.query.startsWith("EF")){
		q.query = q.query.mid(2);
	}else
		q.query = "";


	if(!q.query.isEmpty())
		_queries.push_back(q);
}

void DTAPNParser::queries(){
	Query q;
	q.name = xml.attributes().value("name").toString();
	while(xml.readNextStartElement()){
		if(xml.name() == "query"){
			value(q.query);
		}else
			xml.skipCurrentElement();
	}

	if(q.query.startsWith("EF")){
		q.query = q.query.mid(2);
	}else
		q.query = "";


	if(!q.query.isEmpty())
		_queries.push_back(q);
}

void DTAPNParser::place(){
	qreal x = 0, y = 0;
	QString name = xml.attributes().value("name").toString();
	QString id = xml.attributes().value("id").toString();
	int initialMarking = 0;
	int maxInvariantAge = -1;
	if(xml.attributes().hasAttribute("positionX"))
		x = xml.attributes().value("positionX").toString().toInt();
	if(xml.attributes().hasAttribute("positionY"))
		y = xml.attributes().value("positionY").toString().toInt();
	if(xml.attributes().hasAttribute("initialMarking"))
		initialMarking = xml.attributes().value("initialMarking").toString().toInt();
	if(xml.attributes().hasAttribute("invariant")){
		QRegExp reg("(<=|<)(\\s*)(\\d+|inf)");
		if(reg.indexIn(xml.attributes().value("invariant").toString()) > -1){
			if(reg.cap(3) == "inf")
				maxInvariantAge = -1;
			else
				maxInvariantAge = reg.cap(3).toInt() - (reg.cap(1) == "<=" ? 0 : 1);
		}
	}

	while(xml.readNextStartElement()){
		if(xml.name() == "graphics"){
			position(x,y);
		}else if(xml.name() == "name"){
			value(name);
		}else if(xml.name() == "initialMarking"){
			QString val;
			value(val);
			initialMarking = val.toInt();
		}else if(xml.name() == "invariant"){
			QString val;
			value(val);
			QRegExp reg("(<=|<)(\\s*)(\\d+|inf)");
			if(reg.indexIn(val) > -1){
				if(reg.cap(3) == "inf")
					maxInvariantAge = -1;
				else
					maxInvariantAge = reg.cap(3).toInt() - (reg.cap(1) == "<=" ? 0 : 1);
			}
		}else
			xml.skipCurrentElement();
	}
	//Create place
	builder->addPlace(name.toStdString(), initialMarking, maxInvariantAge, x, y);
	//Map id to name
	idmap[id] = NodeName(Place, name);
}

void DTAPNParser::transition(){
	qreal x = 0, y = 0;
	QString name = xml.attributes().value("name").toString();
	QString id = xml.attributes().value("id").toString();
	if(xml.attributes().hasAttribute("positionX"))
		x = xml.attributes().value("positionX").toString().toInt();
	if(xml.attributes().hasAttribute("positionY"))
		y = xml.attributes().value("positionY").toString().toInt();

	while(xml.readNextStartElement()){
		if(xml.name() == "graphics"){
			position(x, y);
		}else if(xml.name() == "name"){
			value(name);
		}else
			xml.skipCurrentElement();
	}
	//Create transition
	builder->addTransition(name.toStdString(), x, y);
	//Map id to name
	idmap[id] = NodeName(Transition, name);
}

void DTAPNParser::arc(){
	QString source = xml.attributes().value("source").toString();
	QString target = xml.attributes().value("target").toString();
	int start = 0, end = -1;

	// Parse inscription if it's here
	QString inscription = xml.attributes().value("inscription").toString();
	QRegExp reg("(\\(|\\[)(\\d+)(,)(\\d+|inf)(\\)|\\])");
	if(reg.indexIn(inscription) > -1){
		QString s2 = reg.cap(4);
		start = reg.cap(2).toInt() + (reg.cap(1) == "(" ? 1 : 0);
		if(s2 == "inf")
			end = -1;
		else
			end = s2.toInt() - (reg.cap(5) == ")" ? 1 : 0);
	}

	while(xml.readNextStartElement()){
		if(xml.name() == "inscription"){
			QString v;
			value(v);
			QRegExp r("(\\(|\\[)(\\d+)(,)(\\d+|inf)(\\)|\\])");
			if(r.indexIn(v) > -1){
				QString s2 = r.cap(4);
				start = r.cap(2).toInt() + (r.cap(1) == "(" ? 1 : 0);
				if(s2 == "inf")
					end = -1;
				else
					end = s2.toInt() - (r.cap(5) == ")" ? 1 : 0);
			}
		}else
			xml.skipCurrentElement();
	}
	arcs.append(ArcEntry(source, target, start, end));
}

void DTAPNParser::position(qreal &x, qreal &y){
	while(xml.readNextStartElement()){
		if(xml.name() == "position"){
			x = xml.attributes().value("x").toString().toDouble();
			y = xml.attributes().value("y").toString().toDouble();
			xml.skipCurrentElement();
		}else
			xml.skipCurrentElement();
	}
}

/** Read text from value or text tag, value prefered */
void DTAPNParser::value(QString &value){
	value = "";
	while(xml.readNextStartElement()){
		if(xml.name() == "value" || (xml.name() == "text" && value.isEmpty()))
			value = xml.readElementText(QXmlStreamReader::SkipChildElements);
		else
			xml.skipCurrentElement();
	}
}
