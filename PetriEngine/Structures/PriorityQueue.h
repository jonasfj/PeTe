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
#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <assert.h>
#include <map>

namespace PetriEngine{
namespace Structures{


/** Implementation of simple priority queue, that uses double as priority
 * @remarks Lower priority is better, e.g. lowest priority comes out first!
 */
template<class Item>
class PriorityQueue{
private:
	typedef std::multimap<double, Item> queue;
	typedef typename queue::iterator iter;
	/** Underlying queue */
	queue _queue;
public:
	/** Test if queue is empty */
	bool empty() const{
		return _queue.empty();
	}

	/** Get the size of the queue */
	size_t size() const{
		return _queue.size();
	}

	/** Delete everything in the queue */
	void clear(){
		_queue.clear();
	}

	/** Put a new item in the queue */
	void push(double priority, const Item& item){
		_queue.insert(std::pair<double, Item>(priority, item));
	}

	/** Pop item with lowest priority */
	Item pop(){
		assert(_queue.begin() != _queue.end());
		iter i = _queue.begin();
		Item retval = i->second;
		_queue.erase(i);
		return retval;
	}
};

} // Structures
} // PetriEngine

#endif // PRIORITYQUEUE_H
