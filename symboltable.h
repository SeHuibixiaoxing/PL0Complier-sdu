#ifndef SYMBOLTABLE_H
#include "symboltableitem.h"
#define SYMBOLTABLE_H

class SymbolTable
{
public:
  SymbolTable(const QString &newProcedureName);

  void insertConstant(const QString& name, int value);
  void insertVariable(const QString& name, int level);
  void insertProcedure(const QString& name, int level, int address, int size);

  SymbolTableItem* search(const QString& name);
  int searchAddress(const QString& name, int instructionNumber);
  int searchSize(const QString& name, int instructionNumber);

  int refillAddress(const QString& name, int address);
  int refillSize(const QString& name, int size);


  const QString &getProcedureName() const;

  void setProcedureName(const QString &newProcedureName);


private:
  std::vector<SymbolTableItem> itemList;
  int offset;
  QString procedureName;        //表所代表过程的名字
};

#endif // SYMBOLTABLE_H
