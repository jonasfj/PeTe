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

#ifndef __xmlsp_dom_H
#define __xmlsp_dom_H

#include <map>
#include <vector>
#include <string>

#include "xmlsp.h"

namespace XMLSP {
	
	class DOMElement;
	
	typedef std::vector<DOMElement*> DOMElements;
	typedef std::map<std::string, DOMElements> DOMElementMap;
	typedef std::map<std::string, std::string> DOMAttributes;
	typedef std::vector<std::string> DOMStringList;
	
	class DOMElement
	{
	public:
		DOMElement(const std::string& tag_name);
		~DOMElement();
	
		static DOMElement* loadXML(const std::string& xml);	
		//static std::string getXML(DOMElement* element, int depth = 0);
		static std::string getLastError();
	
		int attributeCount() { return attributes.size(); }
		int hasAttribute(const std::string& name);
		const std::string& getAttribute(const std::string& name);
		void setAttribute(const std::string& name, const std::string& value);
		DOMStringList getAttributeList();
	
		bool hasChilds() { return childs.size() ? true : false; }
		int childCount();
		DOMStringList getChildsNames();
		DOMElements getElementsByName(const std::string& name);
		DOMElements getElementsByAttribute(const std::string& attribute, const std::string& value);
		DOMElements getChilds();
		void addChild(DOMElement* element);
		void removeChild(DOMElement* element);
		
		const std::string& getCData() { return cdata; }
		void setCData(const std::string& data) { cdata = data; }
	
		std::string getElementName() { return elementName; }
	
	protected:
		DOMAttributes attributes;
		DOMElementMap childs;
		std::string cdata;
		std::string elementName;
	};
	
}; // namespace XMLSP


#endif // !__xmlsp_dom_H
