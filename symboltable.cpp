#include "symboltable.h"

SymbolTable::SymbolTable(const QString &newProcedureName)
{
  this->offset = 3;
  this->setProcedureName(newProcedureName);
}

void SymbolTable::insertConstant(const QString &name, int value)
{
  this->itemList.push_back(SymbolTableItem(name, SymbolType::CONSTANT, value, -1, -1, -1, false, -1, false, -1));
}

void SymbolTable::insertVariable(const QString &name, int level)
{
  this->itemList.push_back(SymbolTableItem(name, SymbolType::VARIABLE, -1, level, this->offset, -1, false, -1, false, -1));
  ++ offset;
}

void SymbolTable::insertProcedure(const QString &name, int level, int address, int size)
{
  bool fillAddress = false;
  bool fillSize = false;
  if(address == -1)
  {
    fillAddress = true;
  }
  if(size == -1)
  {
    fillSize = true;
  }
  this->itemList.push_back(SymbolTableItem(name, SymbolType::PROCEDURE, -1, level, address, size, fillAddress, -1, fillSize, -1));

}

SymbolTableItem* SymbolTable::search(const QString &name)
{
  for(auto& i : this->itemList)
  {
    if(i.getName() == name)
    {
      return &i;
    }
  }
  return nullptr;
}

int SymbolTable::searchAddress(const QString &name, int instructionNumber)
{
  for(auto& i : this->itemList)
  {
    if(i.getName() == name)
    {
      if(i.getNeedFillAddress())
      {
        int re = i.getFillAddressNext();
        i.setFillAddressNext(instructionNumber);
        return re;
      }
      else
      {
        return i.getAddress();
      }
    }
  }
  return -1;
}

int SymbolTable::searchSize(const QString &name, int instructionNumber)
{
  for(auto& i : this->itemList)
  {
    if(i.getName() == name)
    {
      if(i.getNeedFillSize())
      {
        int re = i.getFillSizeNext();
        i.setFillSizeNext(instructionNumber);
        return re;
      }
      else
      {
        return i.getSize();
      }
    }
  }
  return -1;
}

int SymbolTable::refillAddres(const QString &name, int address)
{
  for(auto& i : this->itemList)
  {
    if(i.getName() == name)
    {
      if(i.getNeedFillAddress())
      {
        i.setNeedFillAddress(false);
        int re = i.getFillAddressNext();
        i.setFillAddressNext(-1);
        i.setAddress(address);
        return re;
      }
    }
  }
  return -1;
}

int SymbolTable::refillSize(const QString &name, int size)
{
  for(auto& i : this->itemList)
  {
    if(i.getName() == name)
    {
      if(i.getNeedFillSize())
      {
        i.setNeedFillSize(false);
        int re = i.getFillSizeNext();
        i.setFillSizeNext(-1);
        i.setSize(size);
        return re;
      }
    }
  }
  return -1;
}


const QString &SymbolTable::getProcedureName() const
{
  return procedureName;
}

void SymbolTable::setProcedureName(const QString &newProcedureName)
{
  procedureName = newProcedureName;
}
