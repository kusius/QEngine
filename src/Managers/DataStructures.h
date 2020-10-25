#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H
#include <vector>
#include <map>

struct EntityKey
{
  string modelPath;
  unsigned int instanceID;
};

struct IntKey
{
  unsigned int instanceID;
};

struct StringKey
{
  string modelPath;
};

bool operator<(const EntityKey &ek, const StringKey &sk);
bool operator<(const StringKey &sk, const EntityKey &ek);

bool operator<(const EntityKey &ek, const IntKey &ik);
bool operator<(const IntKey &ik, const EntityKey &ek);

bool operator<(const EntityKey &left, const EntityKey &right);

typedef std::map<EntityKey, unsigned int, std::less<>> InstanceMap;

#endif