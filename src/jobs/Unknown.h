
#include "../session.h"

#include "../Job.h"

#include "QueryResult.h"

class UnknownJob : public Job
{
private:
  std::string _name;

public:
  
	UnknownJob(ISession* session, std::string name);
	// Inherited via Job
	virtual QueryResult execute() override;

};
