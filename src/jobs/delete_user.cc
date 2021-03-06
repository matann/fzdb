#include "./delete_user.h"
#include "../user/user_attributes.h"
#include "../user/hashing.h"
#include "../user/user_operation.h"
#include "../user/user_exceptions.h"

using namespace jobs;

DeleteUser::DeleteUser(std::shared_ptr<ISession> session, const std::string &username)
  :Job(session, PermType::UserOp ) {
  _username = username;
}

QueryResult DeleteUser::executeNonConst() {
  try {
    _database->users().removeUser(_username);
  } catch (const std::exception &ex) {
    return QueryResult::generateError(QueryResult::ErrorCode::UserDataError, ex.what());
  }

  QueryResult result;
  result.setResultDataText(std::string("User ") + _username + std::string(" deleted successfully."));
  return result;
}
