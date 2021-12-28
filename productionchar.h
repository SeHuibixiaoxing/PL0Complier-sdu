#ifndef PRODUCTIONCHAR_H
#include <QString>
#include <utility>
#include <vector>
#include <map>
#define PRODUCTIONCHAR_H

typedef std::vector<int> Attribute;

#define PRODUCTION_TYPE_NONE 0
#define PRODUCTION_TYPE_TERMINAL 1
#define PRODUCTION_TYPE_UNTERMINAL 2
#define PRODUCTION_TYPE_EMPTY 3


enum class ProductionCharType
{
  NONE = PRODUCTION_TYPE_NONE,
  TERMINAL = PRODUCTION_TYPE_TERMINAL,
  UNTERMINAL = PRODUCTION_TYPE_UNTERMINAL,
  EMPTY = PRODUCTION_TYPE_EMPTY,
};

class ProductionChar
{
public:
  ProductionChar(const ProductionCharType& _type, const QString& _name, const QString& _tokenName);
  ProductionChar(const int& _type, const QString& _name, const QString& _tokenName) : ProductionChar((ProductionCharType) _type, _name, _tokenName) {}
  ProductionChar();

  ProductionCharType getType() const;
  QString getName() const;
  Attribute getAttribute(const QString& name) const;
  bool empty() const;

  void setType(const ProductionCharType& _type);
  void setType(const int& _type);
  void setName(const QString& _name);
  void setAttribute(const QString& name, const Attribute& _attribute);
  QString toString() const;


  bool operator==(const ProductionChar& b) const;
  bool operator!=(const ProductionChar& b) const;

  const QString &getTokenName() const;
  void setTokenName(const QString &newTokenName);

private:
  ProductionCharType type;
  QString name;
  QString tokenName;

  std::map<QString, Attribute> attribute;   //综合属性。pair<属性名，属性值>
};

typedef std::vector<ProductionChar> ProductionCharList;

#endif // PRODUCTIONCHAR_H
