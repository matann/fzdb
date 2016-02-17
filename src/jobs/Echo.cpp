#include "Echo.h"

EchoJob::EchoJob(std::shared_ptr<ISession> session, const std::string &message) : Job(session)
{
	_message = message;
}

QueryResult EchoJob::executeConst() const
{
	QueryResult result;
	result.setValue("type", "string");
	result.setValue(std::string("response"), _message);
	return result;
}
