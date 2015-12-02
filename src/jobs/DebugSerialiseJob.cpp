#include "./DebugSerialiseJob.h"
#include <sstream>
#include <string>
#include <iomanip>

#include "../model/Variant.h"
#include "../model/PropertyValue.h"

std::string testSerialise(const ISerialisable* ser)
{
	std::stringstream log;

	std::size_t length = ser->serialise(NULL, 0).first;
	log << "Variant has serialised length " << length << "\n";

	char* buffer = new char[length];
	auto result = ser->serialise(buffer, length);

	log << "Variant serialisation success: " << (result.second ? "TRUE" : "FALSE") << "\n";
	log << "Variant serialisation wrote " << result.first << " bytes.\nBytes written:\n";

	log << std::hex << std::setfill ('0') << std::setw(2);
	// i progresses in multiples of 8.
	for (int i = 0; i < length; i += 8)
	{
		// j selects the characters in each batch of 8.
		for(int j = 0; j < 8 && i+j < length; j++)
		{
			if ( j > 0 )
				log << " ";

			log << std::setfill ('0') << std::setw(2) << std::hex << static_cast<int>(buffer[i+j]);
		}

		log << "\t";

		for(int j = 0; j < 8 && i+j < length; j++)
		{
			if ( buffer[i+j] < 32 || buffer[i+j] > 126 )
				log << '.';
			else
				log << buffer[i+j];
		}

		log << "\n";
	}

	delete[] buffer;
	return log.str();
}

DebugSerialiseJob::DebugSerialiseJob(TCPSession* session) : Job(session)
{
}

void DebugSerialiseJob::execute()
{
	Variant vs("Sample string");
	Variant vi(5);
	PropertyValue val1(vs, 1.0f);
	PropertyValue val2(vi, 0.2f);

	std::stringstream log;
	log << "======== Testing string variant  ========\n";
	log << "======== String: \"Sample String\" ========\n";
	log << testSerialise(&vs) << "\n";

	log << "======== Testing integer variant ========\n";
	log << "======== Integer: 5              ========\n";
	log << testSerialise(&vi) << "\n";

	log << "======== Testing Property Value 1  ========\n";
	log << testSerialise(&val1) << "\n";

	log << "======== Testing Property Value 2  ========\n";
	log << testSerialise(&val2) << "\n";

	_session->respond(log.str());
}
