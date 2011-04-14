#include "PNMLParser.h"
#include "../Misc/QueryModel.h"
#include "../NetItems/PetriNetSceneBuilder.h"

#include <QtGlobal>
#include <QDebug>

void PNMLParser::parse(QIODevice* input, PetriNetSceneBuilder* builder){
	qBuilder = builder;
	parse(input, (PetriEngine::AbstractPetriNetBuilder*)builder);
	qBuilder = NULL;
}

void PNMLParser::parse(QIODevice *input, PetriEngine::AbstractPetriNetBuilder *builder){
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
			builder->addInputArc(src, dst, entry.weight);
		else if(idmap[entry.src].type == Transition && idmap[entry.dst].type == Place)
			builder->addOutputArc(src, dst, entry.weight);
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

void PNMLParser::pnml(){
	bool readOneNet = false;
	while(xml.readNextStartElement()){
		if(xml.name() == "net" && !readOneNet){
			net();
			readOneNet = true;
		}else if(xml.name() == "queries" && qBuilder){
			queries();
		}else
			xml.skipCurrentElement();
	}
}

void PNMLParser::queries(){
	Q_ASSERT(qBuilder);
	while(xml.readNextStartElement()){
		if(xml.name() == "query"){
			QueryModel::Query query;
			query.name = xml.attributes().value("name").toString();
			query.strategy = xml.attributes().value("strategy").toString();
			query.query = xml.readElementText(QXmlStreamReader::SkipChildElements);
			qBuilder->addQuery(query);
		}else
			xml.skipCurrentElement();
	}
}


void PNMLParser::net(){
	while(xml.readNextStartElement()){
		if(xml.name() == "page"){
			net();
			break;	//Don't read more than one page
		}else if(xml.name() == "variable"){
			variable();
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

void PNMLParser::variable(){
	QString vname = xml.attributes().value("name").toString();
	bool good = true;
	int initialValue = xml.attributes().value("initial-value").toString().toInt(&good);
	if(!good)
		initialValue = 0;
	int range = xml.attributes().value("range").toString().toInt(&good);
	if(!good)
		range = initialValue;
	builder->addVariable(vname.toStdString(), initialValue, range);

	//Should work fine, even if it's a empty-element
	xml.skipCurrentElement();
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
	builder->addPlace(name.toStdString(), initialMarking, x, y);
	//Map id to name
	idmap[id] = NodeName(Place, name);
}

void PNMLParser::transition(){
	qreal x = 0, y = 0;
	QString name;
	QString id = xml.attributes().value("id").toString();
	QString conditions, assignments;

	while(xml.readNextStartElement()){
		if(xml.name() == "conditions"){
			conditions = xml.readElementText(QXmlStreamReader::SkipChildElements);
		}else if(xml.name() == "assignments"){
			assignments = xml.readElementText(QXmlStreamReader::SkipChildElements);
		}else if(xml.name() == "graphics"){
			position(x,y);
		}else if(xml.name() == "name"){
			value(name);
		}else
			xml.skipCurrentElement();
	}
	//Create transition
	builder->addTransition(name.toStdString(),
						   conditions.toStdString(),
						   assignments.toStdString(),
						   x, y);
	//Map id to name
	idmap[id] = NodeName(Transition, name);
}

void PNMLParser::arc(){
	//TODO: test if already closed, e.g. empty tag
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

	arcs.append(ArcEntry(source, target, inscription));
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

/** Read text from value or text tag, value prefered */
void PNMLParser::value(QString &value){
	value = "";
	while(xml.readNextStartElement()){
		if(xml.name() == "value" || (xml.name() == "text" && value.isEmpty()))
			value = xml.readElementText(QXmlStreamReader::SkipChildElements);
		else
			xml.skipCurrentElement();
	}
}
