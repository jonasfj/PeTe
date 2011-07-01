// Copyright (c) 2007, Przemyslaw Grzywacz
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Przemyslaw Grzywacz nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Przemyslaw Grzywacz ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Przemyslaw Grzywacz BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include <sstream>
#include <cstring>

#include "xmlsp_dom.h"


namespace XMLSP {
	
	static std::string lastParseError;
	
	class DOMParser: public Parser
	{
	public:
		virtual ~DOMParser();
	
		DOMElement* getDOM() { return rootElement; }
	
		// parse events
		virtual bool on_tag_open(const std::string& tag_name, StringMap& attributes);
		virtual bool on_cdata(const std::string& cdata);
		virtual bool on_tag_close(const std::string& tag_name);
		virtual bool on_document_begin();
		virtual bool on_document_end();
		virtual void on_error(int errnr, int line, int col, const std::string& message);
	protected:
	
		std::stack<DOMElement*> elementStack;
		DOMElement* rootElement;
	};
	
	
	//===================================================================
	//===================================================================
	//===================================================================
	
	
	DOMParser::~DOMParser()
	{
	
	}
	
	
	//===================================================================
	
	
	bool DOMParser::on_tag_open(const std::string& tag_name, StringMap& attributes)
	{
		DOMElement *e = new DOMElement(tag_name);
		StringMap::iterator i;
	
		// copy attributes
		for(i = attributes.begin(); i != attributes.end(); i++)
			e->setAttribute(i->first, i->second);
	
		// get parent element
		if (elementStack.size()) {
			// add child to parent
			elementStack.top()->addChild(e);
		} else {
			// save root element
			rootElement = e;
		}
	
		// add element to stack
		elementStack.push(e);
		return true;
	}
	
	
	//===================================================================
	
	
	bool DOMParser::on_cdata(const std::string& cdata)
	{
		DOMElement *e = elementStack.top();
		std::string data;
		data.assign(e->getCData());
		data.append(cdata);
		e->setCData(data);
		return true;
	}
	
	
	//===================================================================
	
	
	bool DOMParser::on_tag_close(const std::string& tag_name)
	{
		elementStack.pop();
		return true;
	}
	
	
	//===================================================================
	
	
	bool DOMParser::on_document_begin()
	{
		rootElement = NULL;
		return true;
	}
	
	
	//===================================================================
	
	
	bool DOMParser::on_document_end()
	{
		// ?? why ?
		while(elementStack.size()) elementStack.pop();
		return true;
	}
	
	
	//===================================================================
	
	
	void DOMParser::on_error(int errnr, int line, int col, const std::string& message)
	{
		std::stringstream s;
		s<<"Error("<<errnr<<"): "<<message<<" on "<<line<<":"<<col;
		lastParseError = s.str();
	
		// free dom
		delete rootElement;
		while(elementStack.size()) elementStack.pop();
	}
	
	
	//===================================================================
	//===================================================================
	//===================================================================
	
	
	/* NOT NEEDED while getXML is not implemented
	static std::string encodeEntities(const std::string& text)
	{
		std::string out;
		int ch, i, l;
		static char hex[] = "0123456789abcdef";
	
		l = text.size();
		for(i = 0; i < l; i++) {
			ch = text[i];
			switch (ch) {
			case '&': out.append("&amp;"); break;
			case '"': out.append("&quot;"); break;
			case '\'': out.append("&apos;"); break;
			case '<': out.append("&lt;"); break;
			case '>': out.append("&gt;"); break;
			default:
				if (isalnum(ch)) {
					out.append(1, ch);
				} else {
					out.append("&#");
					out.append(1, hex[(ch & 0xf0) >> 4]);
					out.append(1, hex[(ch & 0x0f)]);
				}
			}
		}	
	
		return out;
	}
	*/
	
	
	//===================================================================
	
	
	std::string DOMElement::getLastError()
	{
		return lastParseError;
	}
	
	
	//===================================================================
	
	
	DOMElement* DOMElement::loadXML(const std::string& xml)
	{
		DOMParser parser;
		if (parser.parse(xml)) {
			return parser.getDOM();
		} else {
			return NULL;
		}
	}
	
	
	//===================================================================
	
	
	/*std::string DOMElement::getXML(DOMElement* root, int depth)
	{
	
	}*/
	
	
	//===================================================================
	
	
	DOMElement::DOMElement(const std::string& tag_name)
	{
		elementName = tag_name;
	}
	
	
	//===================================================================
	
	
	DOMElement::~DOMElement()
	{
		DOMElementMap::iterator mi;
		DOMElements::iterator ei;
	
		for(mi = childs.begin(); mi != childs.end(); mi++) {
			for(ei = mi->second.begin(); ei != mi->second.end(); ei++) {
				delete *ei;
			}
		}
	}
	
	
	//===================================================================
	
	
	int DOMElement::hasAttribute(const std::string& name)
	{
		DOMAttributes::iterator i;
		for(i = attributes.begin(); i != attributes.end(); i++) {
			if (i->first == name) return true;
		}
		return false;
	}
	
	
	//===================================================================
	
	
	const std::string& DOMElement::getAttribute(const std::string& name)
	{
		return attributes[name];
	}
	
	
	//===================================================================
	
	
	void DOMElement::setAttribute(const std::string& name, const std::string& value)
	{
		attributes[name] = value;
	}
	
	
	//===================================================================
	
	
	DOMStringList DOMElement::getAttributeList()
	{
		DOMStringList r;
		DOMAttributes::iterator i;
		for(i = attributes.begin(); i != attributes.end(); i++)
			r.push_back(i->first);
	
		return r;
	}
	
	
	//===================================================================
	
	
	int DOMElement::childCount()
	{
		DOMElementMap::iterator mi;
		DOMElements::iterator ei;
		int count = 0;
	
		for(mi = childs.begin(); mi != childs.end(); mi++) {
			for(ei = mi->second.begin(); ei != mi->second.end(); ei++)
				count++;
		}
	
		return count;
	}
	
	
	//===================================================================
	
	
	DOMStringList DOMElement::getChildsNames()
	{
		DOMElementMap::iterator mi;
		DOMStringList list;
	
		for(mi = childs.begin(); mi != childs.end(); mi++) {
			list.push_back(mi->first);
		}
	
		return list;
	}
	
	
	//===================================================================
	
	
	DOMElements DOMElement::getElementsByName(const std::string& name)
	{
		DOMElementMap::iterator mi;
		DOMElements::iterator ei;
		DOMElements elements;
	
		for(mi = childs.begin(); mi != childs.end(); mi++) {
			if (mi->first == name) {
				for(ei = mi->second.begin(); ei != mi->second.end(); ei++) {
					elements.push_back(*ei);
				}
			}
		}
	
		return elements;
	}
	
	
	//===================================================================
	
	
	DOMElements DOMElement::getElementsByAttribute(const std::string& attribute, const std::string& value)
	{
		DOMElementMap::iterator mi;
		DOMElements::iterator ei;
		DOMElements elements;
	
		for(mi = childs.begin(); mi != childs.end(); mi++) {
			for(ei = mi->second.begin(); ei != mi->second.end(); ei++) {
				if ((*ei)->hasAttribute(attribute) && (*ei)->getAttribute(attribute) == value) {
					elements.push_back(*ei);
				}
			}
		}
	
		return elements;
	}
	
	
	//===================================================================
	
	
	DOMElements DOMElement::getChilds()
	{
		DOMElementMap::iterator mi;
		DOMElements::iterator ei;
		DOMElements elements;
	
		for(mi = childs.begin(); mi != childs.end(); mi++) {
			for(ei = mi->second.begin(); ei != mi->second.end(); ei++) {
				elements.push_back(*ei);
			}
		}
	
		return elements;
	}
	
	
	//===================================================================
	
	
	void DOMElement::addChild(DOMElement* element)
	{
		childs[element->getElementName()].push_back(element);
	}
	
	
	//===================================================================
	
	
	void DOMElement::removeChild(DOMElement* element)
	{
		DOMElementMap::iterator mi;
		DOMElements::iterator ei;

		for(mi = childs.begin(); mi != childs.end(); mi++) {
			if (mi->first == element->getElementName()) {
				for(ei = mi->second.begin(); ei != mi->second.end(); ei++) {
					if (*ei == element) {
						mi->second.erase(ei);
						return;
					}
				}
			}
		}
	}
	
}; // namespace XMLSP
