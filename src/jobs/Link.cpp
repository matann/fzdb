#include "Link.h"
#include "../singletons.h"
#include "../model/EntityManager.h"

#include "../Exceptions.h"

using namespace jobs;

Link::Link(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2) 
	: Job(session), _entity1(entity1), _entity2(entity2)
{}

QueryResult Link::executeNonConst()
{
	QueryResult result;
	result.setValue("result", "success");
	_database->entityManager().linkEntities(_entity1, _entity2);
	return result;
}

Unlink::Unlink(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2)
	: Link(session, entity1, entity2)
{}

QueryResult Unlink::executeNonConst()
{
	QueryResult result;
	result.setValue("result", "success");
	_database->entityManager().unlinkEntities(_entity1, _entity2);
	return result;
}

Merge::Merge(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2)
	: Link(session, entity1, entity2)
{}

QueryResult Merge::executeNonConst()
{
	QueryResult result;
	result.setValue("result", "success");
	_database->entityManager().mergeEntities(_entity1, _entity2);
	return result;
}
