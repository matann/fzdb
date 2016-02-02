#include "./EntityManager.h"
#include <cassert>
#include <set>
#include <sstream>
#include "../FileSystem.h"
#include "GraphSerialiser.h"
#include "../Util.h"

EntityManager::EntityManager()
{
	_lastHandle = Entity::INVALID_EHANDLE;
	_lastProperty = 0;
}

EntityManager::~EntityManager()
{
}

std::shared_ptr<Entity> EntityManager::createEntity()
{
	_lastHandle++;
	assert(_lastHandle != Entity::INVALID_EHANDLE);

	auto e = std::make_shared<Entity>(0, _lastHandle);
	_entities.insert(std::pair<Entity::EHandle_t, std::shared_ptr<Entity>>(_lastHandle, e));

	return e;
}

QueryResult EntityManager::SeparateTriples(std::vector<model::Triple> conditions)
{
	std::map<std::string, std::vector<std::string>> variables;

	for (int i = 0; i < conditions.size(); i++) {
		auto vars = conditions[i].variables();

		switch (vars.size()) {
		case 0: 
			//if the triple doesn't have any variables, then it doesn't affect the output
			break;
		case 1:
			if (variables.find(vars[0]) == variables.end()) {
				variables[vars[0]] = vars;
			}
			break;
		case 2:

			if (variables.find(vars[0]) == variables.end()) {
				variables[vars[0]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[1]);
			}

			if (variables.find(vars[1]) == variables.end()) {
				variables[vars[1]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[0]);
			}

			break;
		case 3:

			if (variables.find(vars[0]) == variables.end()) {
				variables[vars[0]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[1]);
				variables[vars[0]].push_back(vars[2]);
			}

			if (variables.find(vars[1]) == variables.end()) {
				variables[vars[1]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[0]);
				variables[vars[0]].push_back(vars[2]);
			}

	 		if (variables.find(vars[2]) == variables.end()) {
				variables[vars[2]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[0]);
				variables[vars[0]].push_back(vars[1]);
			}

			break;
		}
	}

	std::vector<std::set<std::string>> buckets;
	std::set<std::string> visited;

	for (auto iter = variables.begin(); iter != variables.end(); iter++) {

	}


	return QueryResult();
}

// Basic Graph Pattern
// When presented with a sanatised list of triples finds values for variables that satisfy that condition
VariableSet EntityManager::BGP(TriplesBlock triplesBlock)
{
	VariableSet result(triplesBlock.variables);
	auto conditions = triplesBlock.triples;

	//sort by entropy
	std::sort(conditions.begin(), conditions.end(), [](model::Triple t1, model::Triple t2) { return t1.Entropy() < t2.Entropy(); });

	//iterate over conditions
	for (auto conditionsIter = conditions.cbegin(); conditionsIter != conditions.end(); conditionsIter++) {

		//There are 7 valid combinations

		if (conditionsIter->subject.type == model::Subject::Type::VARIABLE) {
			if (conditionsIter->predicate.type == model::Predicate::Type::PROPERTY) {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//option 1 - $a <prop> value
					this->Scan1(std::move(result), conditionsIter->subject.value, std::move(conditionsIter->predicate), std::move(conditionsIter->object));
				}
				else {
					//option 2 - $a <prop> $b
					this->Scan2(std::move(result), conditionsIter->subject.value, std::move(conditionsIter->predicate), conditionsIter->object.value);
				}
			}
			else {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//option3 - $a $b value
					//this->Scan1(std::move(result), conditionsIter->subject.value, std::move(conditionsIter->predicate), std::move(conditionsIter->object));
					throw NotImplementedException("Queries of the form $a $b value are not yet implemented");
				}
				else {
					//option 4 - $a $b $c
					this->Scan4(std::move(result), conditionsIter->subject.value, conditionsIter->predicate.value, conditionsIter->object.value);
				}
			}
		}
		else {
			if (conditionsIter->predicate.type == model::Predicate::Type::PROPERTY) {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//doesn't contain any variables.. is meaningless
				}
				else {
					//option 5 - entity <prop> $c
					this->Scan5(std::move(result), std::move(conditionsIter->subject), std::move(conditionsIter->predicate), conditionsIter->object.value);
				}
			}
	 		else {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//option 7 - entity $b value
					throw NotImplementedException("Queries of the form entity $b value are not yet implemented");
				}
				else {
					//option 8 - entity $b $c
					throw NotImplementedException("Queries of the form entity $b $c are not yet implemented");
				}
			}
		}
	}

	return result;
}

//Inserts new data into the data store
void EntityManager::Insert(std::vector<model::Triple> triples) {
	auto iter = triples.cbegin();
	auto end = triples.cend();
	for (; iter != end; iter++) {
		auto triple = *iter;

		auto entity_id = std::stoll(triple.subject.value);

		//create the entity if it doesn't exist
		//TODO: are we doing incremental handles or what's in the query or both?
		if (_entities.find(entity_id) == _entities.end()) {
			_entities[entity_id] = std::make_shared<Entity>(1, entity_id);
		}

		std::shared_ptr<Entity> currentEntity = _entities[entity_id];
		unsigned int propertyId;

		switch (triple.object.type) {
		case model::Object::Type::STRING:
			propertyId = this->getPropertyName(triple.predicate.value, model::types::Base::Subtype::TypeString, true);
			addToEntity<model::types::String>(currentEntity, propertyId, std::move(triple.object));
			break;
		case model::Object::Type::ENTITYREF:
			propertyId = this->getPropertyName(triple.predicate.value, model::types::Base::Subtype::TypeEntityRef, true);
			addToEntity<model::types::EntityRef>(currentEntity, propertyId, std::move(triple.object));
			break;
		case model::Object::Type::INT:
			propertyId = this->getPropertyName(triple.predicate.value, model::types::Base::Subtype::TypeInt32, true);
			addToEntity<model::types::Int>(currentEntity, propertyId, std::move(triple.object));
			break;
		}
	}
}

std::vector<std::shared_ptr<Entity>> EntityManager::entityList() const
{
    std::vector<std::shared_ptr<Entity>> list;
    
    for ( auto it = _entities.cbegin(); it != _entities.cend(); ++it )
    {
        list.push_back(it->second);
    }
    
    return list;
}

// TODO: Do we need some sort of handle renumber function too?
void EntityManager::insertEntity(std::shared_ptr<Entity> ent)
{
    assert(ent->getHandle() != Entity::INVALID_EHANDLE);
    assert(_entities.find(ent->getHandle()) == _entities.end());
    
    if ( _lastHandle < ent->getHandle() )
        _lastHandle = ent->getHandle();
    
    _entities.insert(std::pair<Entity::EHandle_t, std::shared_ptr<Entity>>(ent->getHandle(), ent));
}

void EntityManager::clearAll()
{
    _entities.clear();
    _lastHandle = Entity::INVALID_EHANDLE;
    _lastProperty = 0;
    _entityTypeNames.clear();
    _propertyNames.clear();
    _propertyTypes.clear();
}

std::size_t EntityManager::entityCount() const
{
    return _entities.size();
}

std::string EntityManager::dumpContents() const
{
    std::stringstream str;
    str << "Number of entities: " << _entities.size() << "\n";
    
    for ( auto it = _entities.cbegin(); it != _entities.cend(); ++it )
    {
        const std::shared_ptr<Entity> ent = it->second;
        str << ent->logString() << "\n";
        if ( ent->propertyCount() > 0 )
        {
            str << "{\n";
            
            const std::map<unsigned int, IEntityProperty*> &propMap = ent->properties();
            for ( auto it2 = propMap.cbegin(); it2 != propMap.cend(); ++it2 )
            {
                IEntityProperty* prop = it2->second;
                str << "  " << prop->logString() << "\n";
                
                if ( prop->count() > 0 )
                {
                    str << "  {\n";
                    
                    for ( int i = 0; i < prop->count(); i++ )
                    {
                        BasePointer val = prop->baseValue(i);
                        str << "    " << val->logString() << "\n";
                    }
                    
                    str << "  }\n";
                }
            }
            
            str << "}\n";
        }
    }
    
    return str.str();
}

bool EntityManager::saveToFile(const std::string &filename)
{
    Serialiser serialiser;
    GraphSerialiser gSer(this);
    gSer.serialise(serialiser);
	
	bool success = true;
	try
	{
		FileSystem::writeFile(filename, serialiser);
	}
	catch (const std::exception &)
	{
		success = false;
	}
    return success;
}

bool EntityManager::loadFromFile(const std::string &filename)
{
    std::size_t size = FileSystem::dataLength(filename);
    if ( size < 1 )
        return false;

    char* buffer = new char[size];
	
	bool success = true;
	try
	{
		FileSystem::readFile(filename, buffer, size);
	}
	catch (const std::exception &)
	{
		success = false;
	}
    
    if ( !success )
    {
        delete[] buffer;
        return false;
    }

    clearAll();
    GraphSerialiser gSer(this);
    gSer.unserialise(buffer);
    delete[] buffer;

    return true;
}

#pragma region linkingandmerging

void EntityManager::linkEntities(Entity::EHandle_t entityId, Entity::EHandle_t entityId2) {

	if (_links.find(entityId) == _links.end()) _links[entityId] = std::set<Entity::EHandle_t>();
	if (_links.find(entityId2) == _links.end()) _links[entityId2] = std::set<Entity::EHandle_t>();

	std::set<Entity::EHandle_t> newSet;

	std::set_union(_links[entityId].begin(), _links[entityId].end(),
					_links[entityId2].begin(), _links[entityId2].end(), 
					std::inserter(newSet, newSet.begin()));

	newSet.insert(entityId2);
	newSet.insert(entityId);

	_links[entityId] = newSet;
	_links[entityId2] = newSet;
}

void EntityManager::unlinkEntities(Entity::EHandle_t entityId, Entity::EHandle_t entityId2) {

	//if they are not actually linked, return
	if (_links.find(entityId) == _links.end() || _links.find(entityId2) == _links.end() ||
		_links.find(entityId)->second.find(entityId2) == _links.find(entityId)->second.end() ||
		_links.find(entityId2)->second.find(entityId) == _links.find(entityId2)->second.end()) {
		return;
	}
}

// Merges the entities with the given Ids. The entity with the higher Id number is deleted.
void EntityManager::mergeEntities(Entity::EHandle_t entityId, Entity::EHandle_t entityId2) {

	//we will keep the entity with the lower id
	Entity::EHandle_t keep, lose;
	auto sorted = std::minmax(entityId, entityId2);
	keep = sorted.first;
	lose = sorted.second;

	//get the entities
	std::shared_ptr<Entity> keepEntity = _entities[keep],
		loseEntity = _entities[lose];

	//check we are merging entities of the same type
	if (keepEntity->getType() != loseEntity->getType()) {
		throw new std::runtime_error("Attempted to merge entities of different types");
	}

	//copy the properties of loseEntity to keepEntity
	for (auto prop : loseEntity->properties()) {
		keepEntity->insertProperty(prop.second);
	}

	//delete EntityManager's ownership of loseEntity (will be automatically deleted)
	_entities.erase(lose);
}

#pragma endregion linkingandmerging

#pragma region scan_functions

void EntityManager::Scan1(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const model::Object&& object) {
	
	//get the property id
	unsigned int propertyId = this->getPropertyName(predicate.value, model::types::Base::Subtype::TypeString, false);

	//the variable has been used before, we only need to iterate over valid values from before
	if (variableSet.used(variableName)) {
		if (variableSet.typeOf(variableName) == model::types::Base::Subtype::TypeEntityRef) {

			unsigned char varIndex = variableSet.indexOf(variableName);

			variableSet.getData()->erase(std::remove_if(variableSet.getData()->begin(), variableSet.getData()->end(),
				[&, this, varIndex](std::vector<std::shared_ptr<model::types::Base>> row) {
					Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>(row[varIndex])->value();
					auto currentEntity = _entities[entityHandle];
					return !currentEntity->meetsCondition(propertyId, std::move(object));
			}), variableSet.getData()->end());

		}
		else {
			//TODO: TypeException
		}
		return;
	}

	//the variable has not been used, add all the values found!
	for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {

		auto currentEntity = iter->second;
		if (currentEntity->meetsCondition(propertyId, std::move(object))) {
			variableSet.add(std::move(variableName), std::make_shared<model::types::EntityRef>(currentEntity->getHandle()), model::types::Base::Subtype::TypeEntityRef);
		}

	}
}

void EntityManager::Scan2(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const std::string variableName2) {
	
	//get the property id
	const unsigned int propertyId = this->getPropertyName(predicate.value);

	//TODO: consider the case where variableName2 is already in variableSet

	//the variable has been used before, we only need to iterate over valid values from before
	if (variableSet.used(variableName)) {
		if (variableSet.typeOf(variableName) == model::types::Base::Subtype::TypeEntityRef) {

			unsigned char varIndex = variableSet.indexOf(variableName),
				varIndex2 = variableSet.indexOf(variableName2);

			variableSet.getData()->erase(std::remove_if(variableSet.getData()->begin(), variableSet.getData()->end(),
				[&, this, varIndex](std::vector<std::shared_ptr<model::types::Base>> row) {
					Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>(row[varIndex])->value();
					auto currentEntityIter = _entities.find(entityHandle);
					std::shared_ptr<Entity> currentEntity;
					if (currentEntityIter != _entities.end()) {
						currentEntity = _entities[entityHandle];
					} else {
						throw new std::runtime_error("Attempted to lookup a non-existent entity");
					}					
					return !currentEntity->hasProperty(propertyId);
			}), variableSet.getData()->end());

			//TODO: but what about the type of the new data being added? :/
			for (auto iter = variableSet.getData()->begin(); iter != variableSet.getData()->end(); iter++) {
				Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>((*iter)[varIndex])->value();
				auto currentEntityIter = _entities.find(entityHandle);
				std::shared_ptr<Entity> currentEntity;
				if (currentEntityIter != _entities.end()) {
					currentEntity = _entities[entityHandle];
				}
				else {
					throw new std::runtime_error("Attempted to lookup a non-existent entity");
				}
				(*iter)[varIndex2] = currentEntity->getProperty(propertyId)->baseValue(0)->Clone();
			}
		}
		else {
			//TODO: TypeException
		}
		return;
	}

	//the variable has not been used, add all the values found!
	for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {

		auto currentEntity = iter->second;
		if (currentEntity->hasProperty(propertyId)) {
			auto rowId = variableSet.add(std::move(variableName), std::make_shared<model::types::EntityRef>(currentEntity->getHandle()), model::types::Base::Subtype::TypeEntityRef);
			variableSet.add(std::move(variableName2),
				currentEntity->getProperty(propertyId)->baseValue(0)->Clone(),
				std::move(_propertyTypes[propertyId]), rowId);
		}

	}
}

void EntityManager::Scan4(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3) {
	
	//TODO: Check variable types

	//TODO: Cover cases where the variables have been used before :/
	if (variableSet.contains(variableName) || variableSet.contains(variableName2) || variableSet.contains(variableName3)) {
		return;
	}

	//If no variables have been used so far, get EVERYTHING!

	for (auto entity : _entities) {
		for (auto prop : entity.second->properties()) {

			variableSet.add(std::move(variableName), std::make_shared<model::types::EntityRef>(entity.first), model::types::Base::Subtype::TypeEntityRef);
			variableSet.add(std::move(variableName2), std::make_shared<model::types::Int>(prop.first), model::types::Base::Subtype::PropertyReference);

			auto type = _propertyTypes[prop.first];
			std::shared_ptr<model::types::Base> val = entity.second->getProperty(prop.first)->baseValue(0)->Clone();
			variableSet.add(std::move(variableName3), std::move(val), std::move(type));
		}
	}
}

void EntityManager::Scan5(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, const std::string variableName) {
	
	//TODO: Check variable types

	//get the entity handle
	Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());

	//get the property id
	const unsigned int propertyId = this->getPropertyName(predicate.value, model::types::Base::Subtype::TypeString, false);

	if (EntityExists(entityRef)) {
		auto entity = _entities[entityRef];
		if (entity->hasProperty(propertyId)) {
			variableSet.add(std::move(variableName),
				entity->getProperty(propertyId)->baseValue(0)->Clone(),
				std::move(_propertyTypes[propertyId]));
		}
	}
}

#pragma endregion scan_functions