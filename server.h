#ifndef FUZZYDB_SERVER
#define FUZZYDB_SERVER

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "./session.h"

using boost::asio::ip::tcp;

class TCPServer
{
public:
	TCPServer(boost::asio::io_service& io_service, unsigned short port);

	void handle_accept(TCPSession* new_session, const boost::system::error_code& error);

private:
	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
};

#endif