#include "production.h"

Production::Production(const ProductionChar &_left, const ProductionCharList &_right, const std::function<void(ProductionChar &left, std::vector<ProductionChar>&)> & _func)
{
  setLeft(_left);
  setRight(_right);
  setFunc(_func);
}

Production::Production()
{

}

ProductionChar Production::getLeft() const
{
  return this->left;
}

ProductionCharList Production::getRight() const
{
  return this->right;
}

std::function<void (ProductionChar &left, std::vector<ProductionChar> &)> Production::getfunc() const
{
  return this->func;
}

void Production::setLeft(const ProductionChar &_left)
{
  this->left = _left;
}

void Production::setRight(const ProductionCharList &_right)
{
  this->right = _right;
}

void Production::setFunc(const std::function<void (ProductionChar &, std::vector<ProductionChar> &)> &_func)
{
  this->func = _func;
}

bool Production::empty() const
{
  return this->left.empty();
}
