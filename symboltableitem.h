#ifndef SYMBOLTABLEITEM_H
#include "token.h"
#define SYMBOLTABLEITEM_H

enum class SymbolType
{
  CONSTANT = 0,
  VARIABLE = 1,
  PROCEDURE = 2,
  NONE = 3,
};

class SymbolTableItem
{
public:
  SymbolTableItem();
  SymbolTableItem(const QString &name, SymbolType type, int value, int level, int address, int size, bool needFillAddress, int fillAddressNext, bool needFillSize, int fillSizeNext);



  const QString &getName() const;
  void setName(const QString &newName);

  SymbolType getType() const;
  void setType(SymbolType newType);

  int getValue() const;
  void setValue(int newValue);

  int getLevel() const;
  void setLevel(int newLevel);

  int getAddress() const;
  void setAddress(int newAddress);

  int getSize() const;
  void setSize(int newSize);

  bool getNeedFillAddress() const;
  void setNeedFillAddress(bool newNeedFillAddress);

  bool getNeedFillSize() const;
  void setNeedFillSize(bool newNeedFillSize);

  int getFillAddressNext() const;
  void setFillAddressNext(int newFillAddressNext);

  int getFillSizeNext() const;
  void setFillSizeNext(int newFillSizeNext);

private:
  QString name;
  SymbolType type;
  int value;
  int level;
  int address;
  int size;

  bool needFillAddress;
  int fillAddressNext;

  bool needFillSize;
  int fillSizeNext;

};

#endif // SYMBOLTABLEITEM_H
