#include "akao.h"

Akao::~Akao() noexcept
{
	if (_data != nullptr) {
		delete[] _data;
	}
}

bool Akao::setDataSize(uint16_t data_size) noexcept
{
	if (_data != nullptr) {
		delete[] _data;
	}

	_data = new (std::nothrow) uint8_t[data_size]();

	if (_data == nullptr) {
		return false;
	}

	_data_size = data_size;

	return true;
}
