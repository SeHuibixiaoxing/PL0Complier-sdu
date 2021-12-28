#include "token.h"

Token::Token()
{
  this->string = "";
  this->type = (TokenType)SYM_undefine;
  this->id = "";
  this->num = 0;
  this->line = 0;
}

Token::Token(uint64 _line, const QString& _string, TokenType _type)
{
  set(_line,_string, _type);
}
Token::Token(uint64 _line, const QString& _string, TokenType _type, const QString& _id)
{
  set(_line,_string, _type, _id);
}
Token::Token(uint64 _line, const QString& _string, TokenType _type, int64 _num)
{
  set(_line,_string, _type, _num);
}

void Token::set(uint64 _line, const QString& _string, TokenType _type)
{
  this->line = _line;
  this->string = _string;
  this->type = _type;
  this->id = "";
  this->num = 0;
}
void Token::set(uint64 _line, const QString& _string, TokenType _type, const QString &_id)
{
  this->line = _line;
  this->string = _string;
  this->type = _type;
  this->id = _id;
  this->num = 0;
}
void Token::set(uint64 _line, const QString& _string, TokenType _type, int64 _num)
{
  this->line = _line;
  this->string = _string;
  this->type = _type;
  this->id = "";
  this->num = _num;
}

bool Token::isDelimiter() const
{
  return (int)this->type >= SYM_min_delimiter && (int)this->type <= SYM_max_delimiter;
}

bool Token::isOperator() const
{
  return (int)this->type >= SYM_min_operator && (int)this->type <= SYM_max_operator;
}

bool Token::isKeyWord() const
{
  return (int)this->type >= SYM_min_keyword && (int)this->type <= SYM_max_keyword;
}

bool Token::isIdent() const
{
  return (int)this->type == SYM_ident;
}

bool Token::isNumber() const
{
  return (int)this->type == SYM_number;
}

bool Token::isUndefine() const
{
  return (int)this->type == SYM_undefine;
}

TokenType Token::getType() const
{
  return this->type;
}

int64 Token::getNum() const
{
  return this->num;
}

uint64 Token::getLine() const
{
  return this->line;
}

QString Token::getId() const
{
  return this->id;
}

QString Token::getString() const
{
  return this->string;
}

void Token::debugPrint() const
{
  if(this->isDelimiter() || this->isOperator() || this->isKeyWord() || this->isUndefine())
  {
    qDebug() << "[" << this->string;
    qDebug() << ", " << "line: " << this->line;
    qDebug() << ", " << "type: " << keyWord[(int)this->type];
    qDebug() << "]";
  }
  else if(this->isNumber())
  {
    qDebug() << "[" << this->string;
    qDebug() << ", " << "line: " << this->line;
    qDebug() << ", " << "type: " << keyWord[(int)this->type];
    qDebug() << ", " << "value: " << this->num;
    qDebug() << "]";
  }
  else if(this->isIdent())
  {
    qDebug() << "[" << this->string;
    qDebug() << ", " << "line: " << this->line;
    qDebug() << ", " << "type: " << keyWord[(int)this->type];
    qDebug() << ", " << "id: " << this->id;
    qDebug() << "]";
  }
  else
  {
    qDebug() << "Error: cant identify the type of this word.";
    qDebug() << "[" << this->string;
    qDebug() << ", " << "line: " << this->line;
    qDebug() << ", " << "type: " << keyWord[(int)this->type];
    qDebug() << "]";
  }
}
