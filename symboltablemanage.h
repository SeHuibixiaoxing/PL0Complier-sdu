#ifndef SYMBOLTABLEMANAGE_H
#include "symboltable.h"
#include "errorinformation.h"
#define SYMBOLTABLEMANAGE_H


class SymbolTableManage
{
public:
  SymbolTableManage();
  SymbolTableManage(ErrorInformation *_errorInformation);

  void pushNewTable(const QString& name);
  void popDeleteTable();

  void enterConstant(const QString& name, int value);
  void enterVariable(const QString& name);
  void enterProcedure(const QString& name, int address, int size);

  void sendErrorInformation(ErrorInformation *_errorInformation);

  SymbolTableItem* search(const QString& name);
  int searchAddress(const QString& name, int instructionNumber);
  int searchSize(const QString& name, int instructionNumber);

  int refillAddres(const QString& name, int address);
  int refillSize(const QString& name, int size);

  QString getTopTableName();

  int getNowLevel();

private:
  std::vector<SymbolTable> symbolTableList;
  ErrorInformation *errorInformation;
};

#endif // SYMBOLTABLEMANAGE_H
