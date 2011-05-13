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
#ifndef BLOCKSTACK_H
#define BLOCKSTACK_H

#include <assert.h>

namespace PetriEngine{
namespace Structures{

/** Block allocating stack */
template<class Item, size_t blocksize>
class BlockStack{
	/** Internal block */
	struct Block{
		Block* parent;
		Item items[blocksize];
	};
public:
	/** Allocate block stack */
	BlockStack(){
		assert(blocksize > 0);
		offset = 0;
		block = new Block;
		block->parent = NULL;
	}

	/** Release allocated memory */
	~BlockStack(){
		while(block){
			Block* b = block;
			block = block->parent;
			delete b;
		}
	}

	/** Get the top, NULL if none */
	Item* top(){
		if(offset)
			return &block->items[offset-1];
		return NULL;
	}

	/** Pop item of stack, true if successful */
	bool pop(){
		if(offset > 1){
			offset--;
			return true;
		}else if(block->parent){
			offset = blocksize;
			Block* b = block;
			block = block->parent;
			delete b;
			return true;
		}else if(offset == 1){
			offset = 0;
			return true;
		}
		return false;
	}

	/** Push item onto the stack */
	void push(const Item& item){
		if(offset == blocksize){
			offset = 0;
			Block* parent = block;
			block = new Block;
			block->parent = parent;
		}
		block->items[offset++] = item;
	}
private:
	size_t offset;
	Block* block;
};

}
}

#endif /* BLOCKSTACK_H */
