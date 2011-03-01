#include "PNMLFactory.h"

#include <QString>

PNMLFactory::PNMLFactory(QIODevice* device)
{
	xml.setDevice(device);
	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(5);
	xml.writeStartDocument("1.0");
	xml.writeStartElement("pnml");
	xml.writeStartElement("net");
}

void PNMLFactory::addPlace(const std::string &name, int tokens, double x, double y){
	xml.writeStartElement("place");
	xml.writeAttribute("id", name.c_str());
	graphics(x, y);
	this->name(name);
	xml.writeStartElement("initialMarking");
	xml.writeTextElement("value", QString::number(tokens));
	xml.writeEndElement();
	xml.writeEndElement();
}

void PNMLFactory::addTransition(const std::string &name, double x, double y){
	xml.writeStartElement("transition");
	xml.writeAttribute("id", name.c_str());
	graphics(x, y);
	this->name(name);
	xml.writeEndElement();
}

void PNMLFactory::addInputArc(const std::string &place, const std::string &transition, int weight){
	xml.writeStartElement("arc");
	xml.writeAttribute("source", place.c_str());
	xml.writeAttribute("target", transition.c_str());
	xml.writeStartElement("inscription");
	xml.writeTextElement("value", QString::number(weight));
	xml.writeEndElement();
	xml.writeEndElement();
}

void PNMLFactory::addOutputArc(const std::string &transition, const std::string &place, int weight){
	xml.writeStartElement("arc");
	xml.writeAttribute("source", transition.c_str());
	xml.writeAttribute("target", place.c_str());
	xml.writeStartElement("inscription");
	xml.writeTextElement("value", QString::number(weight));
	xml.writeEndElement();
	xml.writeEndElement();
}

void PNMLFactory::makePNMLFile(){
	xml.writeEndElement();
	xml.writeEndElement();
	xml.writeEndDocument();
}

void PNMLFactory::graphics(double x, double y){
	xml.writeStartElement("graphics");
	xml.writeEmptyElement("position");
	xml.writeAttribute("x", QString::number(x));
	xml.writeAttribute("y", QString::number(y));
	xml.writeEndElement();
}

void PNMLFactory::name(std::string name){
	xml.writeStartElement("name");
	xml.writeTextElement("value", name.c_str());
	xml.writeEndElement();
}
