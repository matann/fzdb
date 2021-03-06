#ifndef MODEL_DATABASE_H
#define MODEL_DATABASE_H

#include "./entity_manager.h"
#include "../user/user_operation.h"

// Convenience class that encapsulates the main data within the database application.
class Database {
 public:
  Database(const std::string&& dataFilePath, const std::string&& userFilePath);
  ~Database();

  void init();

  // Entities
  EntityManager& entityManager();
  const EntityManager& entityManager() const;

  // Users
  UserOperation& users();
  const UserOperation& users() const;
  const std::string dataFilePath() const;
  const std::string userFilePath() const;

 private:
  EntityManager _entityManager;
  UserOperation _users;
  const std::string _dataFilePath;
  const std::string _userFilePath;
};

#endif // MODEL_DATABASE_H
