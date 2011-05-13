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
#ifndef BITFIELD_H
#define BITFIELD_H

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

namespace PetriEngine {
namespace Structures {

/** Representation of a BitField */
class BitField {
private:
	/** Definition of underlying type for the bitfield*/
	typedef unsigned int _word;

	/** Number of bits per word */
	static const size_t _bitsPerWord = CHAR_BIT * sizeof(_word);

	/** Number of words for size */
	size_t _words() const{
		return (_size + _bitsPerWord - 1) / _bitsPerWord;
	}

	/** Word offset for a given position */
	static size_t _wordOffset(size_t pos) {
		return pos / _bitsPerWord;
	}

	/** Bit offset offset for a given position */
	static size_t _bitOffset(size_t pos) {
		return pos % _bitsPerWord;
	}

	/** Bitmask for pos in a word */
	static _word _bitmask(size_t pos) {
		return ((_word)1)<<_bitOffset(pos);
	}

	/** Underlying word array */
	_word* _bits;
	size_t _size;
public:
	/** Create a new empty bitfield */
	BitField(size_t size){
		_size = size;
		_bits = new _word[_words()];
		clear();
	}

	/** Copy create a bitfield from another */
	BitField(const BitField& bf){
		_size = bf._size;
		_bits = new _word[_words()];
		for(size_t i = 0; i < _words(); i++)
			_bits[i] = bf._bits[i];
	}

	/** Destructor */
	~BitField(){
		if(_bits){
			delete[] _bits;
			_bits = NULL;
		}
	}

	/** Get the size of the bitfield*/
	size_t size() const{
		return _size;
	}

	/** Clear all bits */
	BitField& clear(){
		for(size_t i = 0; i < _words(); i++)
			_bits[i] = 0;
		return *this;
	}

	/** Clear bit at position pos */
	BitField& clear(size_t pos){
		_bits[_wordOffset(pos)] &= ~_bitmask(pos);
		return *this;
	}

	/** Test bit at position pos */
	bool test(size_t pos) const{
		return (_bits[_wordOffset(pos)] & _bitmask(pos)) != (_word)0;
	}

	/** Set all bits */
	BitField& set(){
		memset(_bits, 0xff, sizeof(_word) * _words());
		//Sanitize
		_bits[_words()-1] = _bits[_words()-1] & ~((~(_word)0)<< _bitOffset(_size));
		return *this;
	}

	/** Set bit a position pos */
	BitField& set(size_t pos){
		_bits[_wordOffset(pos)] |= _bitmask(pos);
		return *this;
	}

	/** Set bit a position pos to value */
	BitField& set(size_t pos, bool value){
		if(value)
			set(pos);
		else
			clear(pos);
		return *this;
	}

	/** Flip all bits */
	BitField& flip(){
		for(size_t i = 0; i < _words()-1; i++)
			_bits[i] = ~_bits[i];
		//Sanitize
		_bits[_words()-1] = ~_bits[_words()-1] & ~((~(_word)0)<<_bitOffset(_size));
		return *this;
	}

	/** Flip bit at position pos */
	BitField& flip(size_t pos){
		_bits[_wordOffset(pos)] ^= _bitmask(pos);
		return *this;
	}

	/** Test if any bit is set */
	bool any() const{
		for(size_t i = 0; i < _words(); i++)
			if(_bits[i] != (_word)0)
				return true;
		return false;
	}

	/** Test if no bits are set */
	bool none() const{
		return !any();
	}

	/** Get the first set element, -1 if none */
	int first() const{
		for(size_t i = 0; i < _size; i++)
			if(test(i))
				return i;
		return -1;
	}

	BitField& operator= (const BitField& rhs){
		for(size_t i = 0; i < _words(); i++)
			_bits[i] = rhs._bits[i];
		return *this;
	}

	BitField& operator&= (const BitField& rhs){
		for(size_t i = 0; i < _words(); i++)
			_bits[i] &= rhs._bits[i];
		return *this;
	}

	BitField& operator|= (const BitField& rhs){
		for(size_t i = 0; i < _words(); i++)
			_bits[i] |= rhs._bits[i];
		return *this;
	}

	BitField& operator^= (const BitField& rhs){
		for(size_t i = 0; i < _words(); i++)
			_bits[i] ^= rhs._bits[i];
		return *this;
	}

	BitField operator~() const{
		return BitField(*this).flip();
	}

	bool operator== (const BitField& rhs) const{
		for(size_t i = 0; i < _words(); i++)
			if(_bits[i] != rhs._bits[i])
				return false;
		return true;
	}

	bool operator!= (const BitField& rhs) const{
		for(size_t i = 0; i < _words(); i++)
			if(_bits[i] != rhs._bits[i])
				return true;
		return false;
	}
};

inline BitField operator&(const BitField& lhs, const BitField& rhs){
	BitField retval(lhs);
	retval &= rhs;
	return retval;
}

inline BitField operator|(const BitField& lhs, const BitField& rhs){
	BitField retval(lhs);
	retval |= rhs;
	return retval;
}

inline BitField operator^(const BitField& lhs, const BitField& rhs){
	BitField retval(lhs);
	retval ^= rhs;
	return retval;
}

} // Structures
} // PetriEngine

#endif // BITFIELD_H
