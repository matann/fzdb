#include "./command_interpreter.h"

#include "./job.h"
#include "./job_queue.h"

#include "./jobs/ping.h"
#include "./jobs/echo.h"
#include "./jobs/unknown.h"
#include "./jobs/insert.h"
#include "./jobs/delete.h"
#include "./jobs/bgp.h"
#include "./jobs/debug_job.h"
#include "./jobs/load_file.h"
#include "./jobs/save_file.h"
#include "./jobs/link.h"
#include "./jobs/flush.h"
#include "./jobs/add_user.h"
#include "./jobs/delete_user.h"
#include "./jobs/demote_admin.h"
#include "./jobs/promote_editor.h"
#include "./jobs/user_login.h"
#include "./jobs/user_logout.h"
#include "./jobs/user_level.h"
#include "./jobs/user_password.h"

#include "./parser.h"

void CommandInterpreter::ProcessCommand(std::shared_ptr<ISession> session, std::string command) {

  command.erase(std::remove(command.begin(), command.end(), '\n'), command.end());
  command.erase(std::remove(command.begin(), command.end(), '\r'), command.end());

  auto tokens = FSparqlParser::Tokenize(command);

  try {
    Query query = FSparqlParser::ParseAll(tokens);

    switch (query.type) {
      case QueryType::PING:
        JobQueue::AddJob(new PingJob(session));
        break;
      case QueryType::DEBUGECHO:
        JobQueue::AddJob(new EchoJob(session, query.data0));
        break;
      case QueryType::INSERT:
        JobQueue::AddJob(new Insert(session, query));
        break;
      case QueryType::DELETECMD:
        JobQueue::AddJob(new Delete(session, query));
        break;
      case QueryType::SELECT:
        JobQueue::AddJob(new BGP(session, query));
        break;
      case QueryType::DEBUGOTHER:
        JobQueue::AddJob(new DebugJob(session, query.data0));
        break;
      case QueryType::LOAD:
        JobQueue::AddJob(new LoadFileJob(session, query.data0));
        break;
      case QueryType::SAVE:
        JobQueue::AddJob(new SaveFileJob(session, query.data0));
        break;
      case QueryType::LINK:
        JobQueue::AddJob(new jobs::Link(session, query.entities[0], query.entities[1]));
        break;
      case QueryType::UNLINK:
        JobQueue::AddJob(new jobs::Unlink(session, query.entities[0], query.entities[1]));
        break;
      case QueryType::MERGE:
        JobQueue::AddJob(new jobs::Merge(session, query.entities[0], query.entities[1]));
        break;
      case QueryType::FLUSH:
        JobQueue::AddJob(new Flush(session));
        break;
      case QueryType::USER_ADD:
        JobQueue::AddJob(new AddUserJob(session, query.data0, query.data1));
        break;
      case QueryType::USER_DELETE:
        JobQueue::AddJob(new DeleteUserJob(session, query.data0));
        break;
      case QueryType::USER_PASSWORD:
        JobQueue::AddJob(new UserPasswordJob(session, query.data0, query.data1)); 
        break;
      case QueryType::USER_PROMOTE:
        JobQueue::AddJob(new PromoteEditorJob(session, query.data0));
        break;
      case QueryType::USER_DEMOTE:
        JobQueue::AddJob(new DemoteAdminJob(session, query.data0));
        break;
      case QueryType::USER_LOGIN:
        JobQueue::AddJob(new UserLoginJob(session, query.data0, query.data1));
        break;
      case QueryType::USER_LOGOUT:
        JobQueue::AddJob(new UserLogoutJob(session));
        break;
      case QueryType::USER_LEVEL:
        JobQueue::AddJob(new UserLevelJob(session));
        break;
      default:
        JobQueue::AddJob(new UnknownJob(session, command));
    }
  } catch (ParseException ex) {
    session->respond(QueryResult::generateError(QueryResult::ErrorCode::ParseError, std::string("Parse error: ") +  ex.what()).toJson());
  } catch (std::exception& ex) {
    session->respond(QueryResult::generateError(QueryResult::ErrorCode::GenericError, std::string("Unexpected error: ") + ex.what()).toJson());
  }
}
