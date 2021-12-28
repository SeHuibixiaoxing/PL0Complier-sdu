#include "debugmanager.h"

DebugManager::DebugManager()
{

}

void DebugManager::debugLexicalAnalyzer()
{
  this->manager.debugLexicalAnalyzer();
}

void DebugManager::debugParser()
{
  this->manager.debugParser();
}
