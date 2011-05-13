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
#ifndef ENHANCEDPRIORITYQUEUE_H
#define ENHANCEDPRIORITYQUEUE_H

#include <assert.h>
#include <map>
#include <list>

namespace PetriEngine{
namespace Structures{


/** Implementation of simple priority queue, that uses double as priority
 * @remarks Lower priority is better, e.g. lowest priority comes out first!
 * Note this is the same as PriorityQueue except this one uses a map and list
 * internally, also it returns the newest item if two items have the same priority.
 */
template<class Item>
class EnhancedPriorityQueue{
private:
	typedef std::list<Item> ItemList;
	typedef std::map<double, ItemList> Queue;
	typedef typename Queue::iterator Iter;
	typedef typename Queue::const_iterator ConstIter;
	/** Underlying queue */
	Queue _queue;
	size_t _size;
public:
	EnhancedPriorityQueue(){
		clear();
	}

	/** Test if queue is empty */
	bool empty() const{
		return _queue.empty();
	}

	/** Get the size of the queue */
	size_t size() const{
		return _size;
	}

	/** Delete everything in the queue */
	void clear(){
		_queue.clear();
		_size = 0;
	}

	/** Put a new item in the queue */
	void push(double priority, const Item& item){
		_size += 1;
		Iter it = _queue.find(priority);
		if(it != _queue.end())
			it->second.push_back(item);
		else
			_queue.insert(std::pair<double, ItemList>(priority, ItemList(1, item)));
	}

	/** Pop item with lowest priority */
	Item pop(bool preferNewest = true){
		assert(_size > 0);
		_size -= 1;
		assert(_queue.begin() != _queue.end());
		Iter it = _queue.begin();
		assert(!it->second.empty());
		Item retval =  preferNewest ? it->second.back() : it->second.front();
		if(preferNewest)
			it->second.pop_back();
		else
			it->second.pop_front();
		if(it->second.empty())
			_queue.erase(it);
		return retval;
	}

	/** Get the top most item */
	Item& top(bool preferNewest = true){
		assert(_size > 0);
		assert(_queue.begin() != _queue.end());
		Iter it = _queue.begin();
		assert(!it->second.empty());
		return preferNewest ? it->second.back() : it->second.front();
	}

	/** Get priority of top most item in queue */
	double topPriority() const {
		assert(_size > 0);
		assert(_queue.begin() != _queue.end());
		ConstIter it = _queue.begin();
		assert(!it->second.empty());
		return it->first;
	}
};

} // Structures
} // PetriEngine


#endif // ENHANCEDPRIORITYQUEUE_H
