#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H
#include <vector>
#include <map>

struct EntityKey
{
  std::string modelPath;
  unsigned int instanceID;
};

struct EntityKeyCmp
{
  bool operator()(const EntityKey &left, const EntityKey &right) const;
};

struct IntKey
{
  unsigned int instanceID;
};

struct StringKey
{
  std::string modelPath;
};

typedef std::map<EntityKey, unsigned int, EntityKeyCmp> InstanceMap;

#endif