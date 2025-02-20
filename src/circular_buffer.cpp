/****************************************************************************/
//    Original author: kode54                                               //
/****************************************************************************/

#include "openpsf/circular_buffer.h"
#include <algorithm>

auto constexpr silence_threshold = 8;

CircularBuffer::CircularBuffer(unsigned int p_size) :
	readptr(0), writeptr(0), size(p_size), used(0)
{
	buffer.reserve(p_size);
}

unsigned CircularBuffer::data_available() noexcept
{
	return used;
}

unsigned CircularBuffer::free_space() noexcept
{
	return size - used;
}

bool CircularBuffer::write(const int16_t* src, unsigned int count)
{
	if (count > free_space()) {
		return false;
	}

	while (count) {
		unsigned delta = size - writeptr;
		if (delta > count) {
			delta = count;
		}
		std::copy(src, src + delta, buffer.begin() + writeptr);
		used += delta;
		writeptr = (writeptr + delta) % size;
		src += delta;
		count -= delta;
	}

	return true;
}

unsigned CircularBuffer::read(int16_t* dst, unsigned int count)
{
	unsigned done = 0;
	for (;;) {
		unsigned delta = size - readptr;
		if (delta > used) delta = used;
		if (delta > count) delta = count;
		if (!delta) break;

		std::copy(buffer.begin() + readptr, buffer.begin() + readptr + delta, dst);
		dst += delta;
		done += delta;
		readptr = (readptr + delta) % size;
		count -= delta;
		used -= delta;
	}
	return done;
}

void CircularBuffer::reset() noexcept
{
	readptr = writeptr = used = 0;
}

void CircularBuffer::resize(unsigned int p_size)
{
	size = p_size;
	buffer.reserve(p_size);
	reset();
}

bool CircularBuffer::test_silence() const noexcept
{
	int16_t* begin = (int16_t*)&buffer[0];
	int16_t first = *begin;
	*begin = silence_threshold * 2;
	int16_t* p = begin + size;
	while ((unsigned int)(*--p + silence_threshold) <= (unsigned int)silence_threshold * 2) {}
	*begin = first;
	return p == begin && ((unsigned int)(first + silence_threshold) <= (unsigned int)silence_threshold * 2);
}
