#ifndef FUZZYDB_ISESSION
#define FUZZYDB_ISESSION

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>

class ISession
{
public:
	virtual boost::asio::ip::tcp::socket& socket() = 0;
	virtual void start() = 0;
	virtual void respond(const std::string response) = 0;
	virtual void terminate() = 0;
        virtual boost::uuids::uuid uuid() = 0;
	virtual ~ISession() {};
protected:

	virtual void handle_read(const boost::system::error_code& error, size_t bytes_transferred) = 0;
	virtual void handle_write(const boost::system::error_code& error) = 0;
};

#endif