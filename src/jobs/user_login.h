#ifndef JOBS_USER_LOGIN
#define JOBS_USER_LOGIN

#include "../session.h"
#include "../job.h"

#include "../query_result.h"

class UserLoginJob : public Job {
    /**
     * @brief Logs a user into the database. 
     */
public:
    typedef Permission::PermissionType PermType;

    /**
     * @brief Constructor of user login job.
     *
     * @param session Session object of the current user.
     * @param username User name provided by user.
     * @param password Password provided by user.
     */
    UserLoginJob(std::shared_ptr<ISession> session, const std::string &username, const std::string &password);

    virtual bool constOperation() const override { return false; }
    virtual QueryResult executeNonConst() override;

private:
    std::string _username;
    std::string _password;
    std::shared_ptr<ISession> _session;
};

#endif
