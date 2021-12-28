#ifndef DEBUGMANAGER_H
#define DEBUGMANAGER_H

#include "manager.h"


class DebugManager
{
public:
  DebugManager();

  void debugLexicalAnalyzer();
  void debugParser();
private:
  Manager manager;
};

#endif // DEBUGMANAGER_H
