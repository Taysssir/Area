#ifndef SENTINEL_USERLOADER_H
#define SENTINEL_USERLOADER_H

#include "user.h"

class UserLoader
{
public:
  static Users load(QString const& filePath);
  static bool save(QString const& filePath, Users const& users);
};

#endif // SENTINEL_USERLOADER_H
