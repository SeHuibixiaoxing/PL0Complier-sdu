#include "productionchar.h"

ProductionChar::ProductionChar(const ProductionCharType &_type, const QString &_name, const QString &_tokenName)
{
  this->setType(_type);
  this->setName(_name);
  this->setTokenName(_tokenName);
}

ProductionChar::ProductionChar()
{
  this->type = ProductionCharType::NONE;
}

QString ProductionChar::getName() const
{
  return this->name;
}

bool ProductionChar::empty() const
{
  return this->type == ProductionCharType::NONE;
}

void ProductionChar::setType(const ProductionCharType &_type)
{
  this->type = _type;
}

void ProductionChar::setType(const int &_type)
{
  this->setType((ProductionCharType)_type);
}

void ProductionChar::setName(const QString &_name)
{
  this->name = _name;
}

QString ProductionChar::toString() const
{
  return getName();
}

Attribute ProductionChar::getAttribute(const QString &name) const
{
  return this->attribute.at(name);
}

void ProductionChar::setAttribute(const QString &name, const Attribute &_attribute)
{
  this->attribute[name] = _attribute;
}

bool ProductionChar::operator==(const ProductionChar &b) const
{
  return this->type == b.type && this->name == b.name;
}

bool ProductionChar::operator!=(const ProductionChar &b) const
{
  return !(*this == b);
}

const QString &ProductionChar::getTokenName() const
{
  return tokenName;
}

void ProductionChar::setTokenName(const QString &newTokenName)
{
  tokenName = newTokenName;
}

ProductionCharType ProductionChar::getType() const
{
  return this->type;
}
