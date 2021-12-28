#include "symboltablemanage.h"

SymbolTableManage::SymbolTableManage()
{
  this->errorInformation = nullptr;
}

SymbolTableManage::SymbolTableManage(ErrorInformation *_errorInformation)
{
  this->sendErrorInformation(_errorInformation);
}

void SymbolTableManage::pushNewTable(const QString& name)
{
  this->symbolTableList.push_back(SymbolTable(name));
}

void SymbolTableManage::popDeleteTable()
{
  this->symbolTableList.pop_back();
}

void SymbolTableManage::enterConstant(const QString &name, int value)
{
  this->symbolTableList[this->symbolTableList.size() - 1].insertConstant(name, value);
}

void SymbolTableManage::enterVariable(const QString &name)
{
  this->symbolTableList[this->symbolTableList.size() - 1].insertVariable(name, this->symbolTableList.size() - 1);
}

void SymbolTableManage::enterProcedure(const QString &name, int address, int size)
{
  this->symbolTableList[this->symbolTableList.size() - 1].insertProcedure(name, this->symbolTableList.size() - 1, address, size);
}

void SymbolTableManage::sendErrorInformation(ErrorInformation *_errorInformation)
{
  this->errorInformation = _errorInformation;
}

SymbolTableItem* SymbolTableManage::search(const QString &name)
{
  SymbolTableItem *re = nullptr;
  for(auto tablePoint = this->symbolTableList.rbegin();tablePoint != this->symbolTableList.rend();++ tablePoint)
  {
    auto& table = *tablePoint;
    re = table.search(name);
    if(re != nullptr && re->getType() != SymbolType::NONE)
      return re;
  }
  return nullptr;
}

int SymbolTableManage::searchAddress(const QString &name, int instructionNumber)
{
  for(auto tablePoint = this->symbolTableList.rbegin();tablePoint != this->symbolTableList.rend();++ tablePoint)
  {
    auto& table = *tablePoint;
    auto tmp = table.search(name);
    if(tmp == nullptr) continue;
    return table.searchAddress(name, instructionNumber);
  }
  return -1;
}

int SymbolTableManage::searchSize(const QString &name, int instructionNumber)
{
  for(auto tablePoint = this->symbolTableList.rbegin();tablePoint != this->symbolTableList.rend();++ tablePoint)
  {
    auto& table = *tablePoint;
    auto tmp = table.search(name);
    if(tmp == nullptr) continue;
    return table.searchSize(name, instructionNumber);
  }
  return -1;
}

int SymbolTableManage::refillAddres(const QString &name, int address)
{
  int re = -1;
  for(auto tablePoint = this->symbolTableList.rbegin();tablePoint != this->symbolTableList.rend();++ tablePoint)
  {
    auto& table = *tablePoint;
    re = table.refillAddres(name, address);
    if(re != -1)
      return re;
  }
  return -1;
}

int SymbolTableManage::refillSize(const QString &name, int size)
{
  int re = -1;
  for(auto tablePoint = this->symbolTableList.rbegin();tablePoint != this->symbolTableList.rend();++ tablePoint)
  {
    auto& table = *tablePoint;
    re = table.refillSize(name, size);
    if(re != -1)
      return re;
  }
  return -1;
}

QString SymbolTableManage::getTopTableName()
{
  return this->symbolTableList[symbolTableList.size() - 1].getProcedureName();
}

int SymbolTableManage::getNowLevel()
{
  return this->symbolTableList.size() - 1;
}
