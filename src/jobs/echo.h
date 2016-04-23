#ifndef JOBS_ECHOJOB_H
#define JOBS_ECHOJOB_H
#include "../session.h"

#include "../job.h"
#include "../query_result.h"
#include <string>

class EchoJob : public Job
{

    /**
     * @brief Debugging command. Responds with whatever input was passed in. 
     */
public:

    EchoJob(std::shared_ptr<ISession> session, const std::string &message);

    virtual bool constOperation() const override { return true; }
    virtual QueryResult executeConst() const override;

private:
    std::string _message;

};

#endif    // JOBS_ECHOJOB_H
