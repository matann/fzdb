#include "./Variant.h"
#include <cassert>
#include <algorithm>
#include <cstring>
#include "../platform.h"

Variant::Variant()
{
	setSafeDefaults();
}

Variant::Variant(const Variant &other)
{
	setSafeDefaults();

	// Easiest way is to use our assignment operator.
	*this = other;
}

Variant::~Variant()
{
	// Clean up any dynamically allocated objects.
	cleanData();
}

Variant::Variant(int value)
{
	setSafeDefaults();
	set(value);
}

Variant::Variant(const std::string &value)
{
	setSafeDefaults();
	set(value);
}

bool Variant::operator ==(const Variant &other) const
{
	if ( type_ != other.type_ ) return false;

	switch (type_)
	{
		case UNDEFINED:
			return true;	// All undefined (null) variants are the same.

		case INTEGER:
			return dataEqual<int>(other);

		case STRING:
			return dataDereferenceEqual<std::string>(other);

		default:
			// Someone's added a new type and not updated this!
			assert(false);
			return false;
	}
}

Variant& Variant::operator =(const Variant &other)
{
	cleanData();

	switch (other.type_)
	{
		case UNDEFINED:
			type_ = UNDEFINED;
			break;

		case INTEGER:
			set(other.getInteger());
			break;

		case STRING:
			set(other.getString());
			break;

		default:
			// Someone's added a new type and not updated this!
			assert(false);
			break;
	}

	return *this;
}

void Variant::setSafeDefaults()
{
	// NOTE: Nothing is cleaned up with this method!
	type_ = UNDEFINED;
	data_ = NULL;
}

bool Variant::isNull() const
{
	return type_ == UNDEFINED;
}

Variant::Type Variant::type() const
{
	return type_;
}

void Variant::cleanData()
{
	// If we have allocated anything in the data pointer
	// for this variant, destroy it.
	
	switch (type_)
	{
		case UNDEFINED:
		case INTEGER:
			break;

		case STRING:
			delete static_cast<std::string*>(data_);
			break;

		default:
			// Someone's added a new type and not updated this!
			assert(false);
			break;
	}
}

void Variant::set(int value)
{
	cleanData();

	// We can store an integer within the data pointer itself,
	// as an integer should be 4 bytes.
	assert(sizeof(void*) >= sizeof(int));

	int* i = reinterpret_cast<int*>(&data_);
	*i = value;
	type_ = INTEGER;
}

void Variant::set(const std::string &value)
{
	cleanData();
	
	// Allocate a string on the heap.
	data_ = static_cast<void*>(new std::string(value));
	type_ = STRING;
}

int Variant::getInteger(bool* ok) const
{
	if ( type_ != INTEGER )
	{
		if (ok) *ok = false;
		return 0;
	}

	if (ok) *ok = true;

#if defined(ENVIRONMENT32)
	return reinterpret_cast<int>(data_);
#elif defined(ENVIRONMENT64)
	long long ret = reinterpret_cast<long long>(data_);
	return static_cast<int>(ret);
#else
	assert(false);
	return 0;
#endif
}

std::string Variant::getString(bool* ok) const
{
	if ( type_ != STRING )
	{
		if (ok) *ok = false;
		return std::string();
	}

	if (ok) *ok = true;
	return *(static_cast<std::string*>(data_));
}

template <typename T>
bool Variant::dataEqual(const Variant &other) const
{
	return *(reinterpret_cast<const T*>(&data_)) == *(reinterpret_cast<const T*>(&other.data_));
}

template <typename T>
bool Variant::dataDereferenceEqual(const Variant &other) const
{
	return *(reinterpret_cast<const T*>(data_)) == *(reinterpret_cast<const T*>(other.data_));
}

std::size_t Variant::internalDataSize() const
{
	switch (type_)
	{
		case UNDEFINED:
			return 0;

		case INTEGER:
			return sizeof(int);

		case STRING:
			return static_cast<std::string*>(data_)->size();

		default:
			// Someone's added a new type and not updated this!
			assert(false);
			return 0;
	}
}

std::pair<std::size_t,bool> Variant::serialise(char* buffer, std::size_t maxSize) const
{
	// Serialisation format:
	// + SerialHeader
	// - Type
	// - Data
	
	std::size_t dataSize = internalDataSize();
	std::size_t bytesRequired = sizeof(Variant::SerialHeader) + sizeof(Variant::Type) + dataSize;

	// If the buffer is null, return the amount of bytes we need.
	if ( !buffer )
	{
		return std::pair<std::size_t,bool>(bytesRequired, false);
	}
	
	// Record how much we need to write.
	std::size_t bytesToWrite = std::min(bytesRequired, maxSize);
	bool canWriteAll = true;
	if ( bytesToWrite < bytesRequired ) canWriteAll = false;

	// If there's nothing to write, return 0.
	if ( bytesToWrite < 1 )
		return std::pair<std::size_t,bool>(0, canWriteAll);

	SerialHeader header;
	header.dataSize = dataSize;

	// Write the header.
	std::size_t idealWrite = sizeof(Variant::SerialHeader);
	std::size_t written = 0;
	std::size_t canWrite = std::min(idealWrite, bytesToWrite);
	memcpy(buffer, &header, canWrite);
	bytesToWrite -= canWrite;
	written += canWrite;

	if ( bytesToWrite < 1 )
	{
		return std::pair<std::size_t,bool>(written, canWriteAll);
	}

	// Write the type information.
	idealWrite = sizeof(Variant::Type);
	canWrite = std::min(idealWrite, bytesToWrite);
	memcpy(buffer, &type_, canWrite);
	bytesToWrite -= canWrite;
	written += canWrite;

	if ( bytesToWrite < 1 )
	{
		return std::pair<std::size_t,bool>(written, canWriteAll);
	}
	
	// Write however many bytes required.
	const void* src = NULL;
	switch (type_)
	{
	// Cases where we're just writing the contents of the pointer itself:
	case INTEGER:
		src = &data_;
		break;

	// Cases where we must dereference the pointer first (different for each item):
	case STRING:
		src = static_cast<std::string*>(data_)->c_str();
		break;

	default:
		// Someone's added a new type and not updated this!
		assert(false);
	}

	std::memcpy(buffer + written, src, bytesToWrite);
	written += bytesToWrite;

	return std::pair<std::size_t,bool>(written, canWriteAll);
}
