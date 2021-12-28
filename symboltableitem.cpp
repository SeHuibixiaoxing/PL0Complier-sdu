#include "symboltableitem.h"

SymbolTableItem::SymbolTableItem()
{
  this->type = SymbolType::NONE;
}

const QString &SymbolTableItem::getName() const
{
  return name;
}

void SymbolTableItem::setName(const QString &newName)
{
  name = newName;
}

SymbolType SymbolTableItem::getType() const
{
  return type;
}

void SymbolTableItem::setType(SymbolType newType)
{
  type = newType;
}

int SymbolTableItem::getValue() const
{
  return value;
}

void SymbolTableItem::setValue(int newValue)
{
  value = newValue;
}

int SymbolTableItem::getLevel() const
{
  return level;
}

void SymbolTableItem::setLevel(int newLevel)
{
  level = newLevel;
}

int SymbolTableItem::getAddress() const
{
  return address;
}

void SymbolTableItem::setAddress(int newAddress)
{
  address = newAddress;
}

int SymbolTableItem::getSize() const
{
  return size;
}

void SymbolTableItem::setSize(int newSize)
{
  size = newSize;
}

bool SymbolTableItem::getNeedFillAddress() const
{
  return needFillAddress;
}

void SymbolTableItem::setNeedFillAddress(bool newNeedFillAddress)
{
  needFillAddress = newNeedFillAddress;
}

bool SymbolTableItem::getNeedFillSize() const
{
  return needFillSize;
}

void SymbolTableItem::setNeedFillSize(bool newNeedFillSize)
{
  needFillSize = newNeedFillSize;
}

int SymbolTableItem::getFillAddressNext() const
{
  return fillAddressNext;
}

void SymbolTableItem::setFillAddressNext(int newFillAddressNext)
{
  fillAddressNext = newFillAddressNext;
}

int SymbolTableItem::getFillSizeNext() const
{
  return fillSizeNext;
}

void SymbolTableItem::setFillSizeNext(int newFillSizeNext)
{
  fillSizeNext = newFillSizeNext;
}

SymbolTableItem::SymbolTableItem(const QString &name, SymbolType type, int value, int level, int address, int size, bool needFillAddress, int fillAddressNext, bool needFillSize, int fillSizeNext) : name(name),
                                                                                                                                                                                                          type(type),
                                                                                                                                                                                                          value(value),
                                                                                                                                                                                                          level(level),
                                                                                                                                                                                                          address(address),
                                                                                                                                                                                                          size(size),
                                                                                                                                                                                                          needFillAddress(needFillAddress),
                                                                                                                                                                                                          fillAddressNext(fillAddressNext),
                                                                                                                                                                                                          needFillSize(needFillSize),
                                                                                                                                                                                                          fillSizeNext(fillSizeNext)
                                                                                                                                                                                                          {}

