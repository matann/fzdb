#ifndef MODEL_ENTITY_H
#define MODEL_ENTITY_H

#include <unordered_map>
#include "./EntityProperty.h"

// Represents an entity in the graph database.
// Each entity has a handle, which is a unique identifier.
// An entity's handle cannot be changed once it is instanciated.
class Entity
{
public:
	// Typedef for an entity handle. This should uniquely identify an entity.
	typedef unsigned long long EHandle_t;

	// Invalid entity handle. This implies that the entity is null.
	static const EHandle_t INVALID_EHANDLE;

	// Constructs a null entity.
	Entity();

	// Constructs an entity with the given handle.
	Entity(EHandle_t handle);

	// Returns whether this entity is null, ie. whether it has an invalid handle.
	bool isNull() const;

	// Getters:

	// Returns the property with the given key, or a null property if this is not found.
	// TODO: This may be slow unless/until properties and their related classes are made
	// to be implicitly shared, as a copy of the property has to be made.
	EntityProperty getProperty(const std::string &key) const;

	// Returns this entity's handle.
	EHandle_t getHandle() const;

	// Setters:

	// Inserts the given property into this entity.
	// If a property with this key already exists, it is replaced.
	void insertProperty(const EntityProperty &prop);

	// Removes the property with the given key.
	void removeProperty(const std::string &key);

	// Clears all properties on the entity.
	void clearProperties();

	// Returns the number of properties present.
	int propertyCount() const;

private:
	// Here (if C++ will let us), we use a const string reference as the key.
	// This avoids us from storing duplicate strings.
	typedef std::unordered_map<std::string, EntityProperty> PropertyTable;

	EHandle_t	handle_;
	PropertyTable	propertyTable_;
};

#endif	// MODEL_ENTITY_H