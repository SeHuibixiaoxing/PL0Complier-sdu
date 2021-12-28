#ifndef PRODUCTION_H
#include "defs.h"
#include <QString>
#include "productionchar.h"
#include <vector>
#include <functional>
#define PRODUCTION_H


class Production
{
public:

  Production(const ProductionChar& _left, const ProductionCharList& _right, const std::function<void(ProductionChar &left, std::vector<ProductionChar>&)>& _func);
  Production();

  ProductionChar getLeft() const;
  ProductionCharList getRight() const;
  std::function<void(ProductionChar &left, std::vector<ProductionChar>&)> getfunc() const;

  void setLeft(const ProductionChar& _left);
  void setRight(const ProductionCharList& _right);
  void setFunc(const std::function<void(ProductionChar &left, std::vector<ProductionChar>&)>& _func);


  bool empty() const;

private:
  ProductionChar left;
  ProductionCharList right;

  std::function<void(ProductionChar &left, std::vector<ProductionChar>&)> func;
};

typedef std::vector<Production> ProductionList;

#endif // PRODUCTION_H
