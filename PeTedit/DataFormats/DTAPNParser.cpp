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
		//TODO: Read queries
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
			query();
		}else
			xml.skipCurrentElement();
	}
}

void DTAPNParser::query(){
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
	QString name;
	QString id = xml.attributes().value("id").toString();
	int initialMarking = 0;

	while(xml.readNextStartElement()){
		if(xml.name() == "graphics"){
			position(x,y);
		}else if(xml.name() == "name"){
			value(name);
		}else if(xml.name() == "initialMarking"){
			QString val;
			value(val);
			initialMarking = val.toInt();
		}else
			xml.skipCurrentElement();
	}
	//Create place
	builder->addPlace(name.toStdString(), initialMarking, x, y);
	//Map id to name
	idmap[id] = NodeName(Place, name);
}

void DTAPNParser::transition(){
	qreal x = 0, y = 0;
	QString name;
	QString id = xml.attributes().value("id").toString();

	while(xml.readNextStartElement()){
		if(xml.name() == "graphics"){
			position(x,y);
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

	while(xml.readNextStartElement()){
		if(xml.name() == "inscription"){
			QString v;
			value(v);
			QRegExp r("(\\(|\\[)\\d+,(\\d+|inf)((\\)\\]))");
			if(r.indexIn(v) > 2){
				start = r.cap(0).toInt();
				if(r.cap(1) == "inf")
					end = -1;
				else
					end = r.cap(1).toInt();
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
