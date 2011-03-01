#include "PNMLParser.h"

#include <QtGlobal>
#include <QDebug>

void PNMLParser::parse(QIODevice *input, PetriEngine::AbstractPetriNetFactory *factory){
	bool wasOpen = input->isOpen();
	if(!wasOpen)
		input->open(QIODevice::ReadOnly);
	xml.setDevice(input);
	this->factory = factory;
	idmap.clear();

	while(xml.readNextStartElement()){
		if(xml.name() == "pnml"){
			pnml();
			break;	//Don't read more than one net
		}else
			xml.skipCurrentElement();
	}

	if(!wasOpen)
		input->close();
	this->factory = NULL;
}

void PNMLParser::pnml(){
	while(xml.readNextStartElement()){
		if(xml.name() == "net"){
			net();
			break;	//Don't read more than one net
		}else
			xml.skipCurrentElement();
	}
}

void PNMLParser::net(){
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
		}else
			xml.skipCurrentElement();
	}
}

void PNMLParser::place(){
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
	factory->addPlace(name.toStdString(), initialMarking, x, y);
	//Map id to name
	idmap[id] = {Place, name};
}

void PNMLParser::transition(){
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
	factory->addTransition(name.toStdString(), x, y);
	//Map id to name
	idmap[id] = {Transition, name};
}

void PNMLParser::arc(){
	QString source = xml.attributes().value("source").toString();
	QString target = xml.attributes().value("target").toString();
	int inscription = 1;

	while(xml.readNextStartElement()){
		if(xml.name() == "inscription"){
			QString v;
			value(v);
			inscription = v.toInt();
		}else
			xml.skipCurrentElement();
	}

	//Create arc
	std::string src = idmap[source].name.toStdString();
	std::string dst = idmap[target].name.toStdString();
	if(idmap[source].type == Place && idmap[target].type == Transition)
		factory->addInputArc(src, dst, inscription);
	else if(idmap[source].type == Transition && idmap[target].type == Place)
		factory->addOutputArc(src, dst, inscription);
	else{
		//TODO: Consider reporting an error
	}
}

void PNMLParser::position(qreal &x, qreal &y){
	while(xml.readNextStartElement()){
		if(xml.name() == "position"){
			x = xml.attributes().value("x").toString().toDouble();
			y = xml.attributes().value("y").toString().toDouble();
			xml.skipCurrentElement();
		}else
			xml.skipCurrentElement();
	}
}

void PNMLParser::value(QString &value){
	while(xml.readNextStartElement()){
		if(xml.name() == "value"){
			xml.readNext();
			if(xml.tokenType() == QXmlStreamReader::Characters)
				value = xml.text().toString();
		}
		xml.skipCurrentElement();
	}
}
