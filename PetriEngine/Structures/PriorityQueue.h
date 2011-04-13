#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <assert.h>
#include <map>

namespace PetriEngine{
namespace Structures{


/** Implementation of simple priority queue, that uses unsigned int as priority */
template<class Item>
class PriorityQueue{
private:
	typedef std::multimap<unsigned int, Item> queue;
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
	void push(unsigned int priority, const Item& item){
		_queue.insert(std::pair<unsigned int, Item>(priority, item));
	}

	/** Iterator over a set of elements from PriorityQueue */
	class Iterator{
	private:
		/** Whether or not to skip increment operation */
		bool _skip;
		/** Current iterator and end iterator */
		iter _i, _end;
	public:
		/** Create empty, invalid iterator
		 * This is needed so that you can allocate the variable and later assign to it.
		 */
		Iterator(){
			_i = _end;
			_skip = true;
		}
		/** Iterate through first elements in the queue
		 * This constructor creates an iterator through the elements with the lowest table size.
		 */
		Iterator(PriorityQueue& queue) {
			_skip = true;
			_i = queue._queue.begin();
			_end = queue._queue.upper_bound(_i->first);
		}
		/** Iterate through elements with specified tablesize */
		Iterator(PriorityQueue& queue, unsigned int priority) {
			std::pair<iter,iter> r = queue._queue.equal_range(priority);
			_i = r.first;
			_end = r.second;
			_skip = true;
		}
		/** Move to the next element */
		bool operator++(int){
			assert(_skip || _i != _end);
			if(!_skip)
				_i++;
			_skip = false;
			return _i != _end;
		}
		/** Member access on item */
		Item* operator->(){
			assert(!_skip);
			assert(_i != _end);
			return &_i->second;
		}
		/** Access the current item */
		Item& item() {
			assert(!_skip);
			assert(_i != _end);
			return _i->second;
		}
		/** Test if this iterator is valid */
		bool valid() const{
			return !(_i == _end && _skip);
		}
		template <typename> friend class PriorityQueue;
	};

	/** Remove item from queue
	 * Notice that it and possibly all other iterators instances created before this operation
	 * may act undefined after this operation.
	 */
	void remove(Iterator& it){
		assert(it._i != it._end);
		assert(!it._skip);
		_queue.erase(it._i);
		it = Iterator();
	}
	/** Get iterator for elements with specified priority */
	Iterator range(unsigned int priority){
		return Iterator(*this, priority);
	}
	/** Get iterator for elements with lowest priority
	 * Notice that the items will not be poppped, this iterator just shows which elements are
	 * to be popped in the queue, you must choose the best of them, and then remove that one.
	 */
	Iterator pop(){
		return Iterator(*this);
	}
};

} // Structures
} // PetriEngine

#endif // PRIORITYQUEUE_H
