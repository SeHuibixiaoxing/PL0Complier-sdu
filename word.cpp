#include "word.h"

Word::Word()
{
  this->string = "";
  this->type = (WordType)SYM_undefine;
  this->id = "";
  this->num = 0;
  this->line = 0;
}

Word::Word(uint64 _line, const QString& _string, WordType _type)
{
  set(_line,_string, _type);
}
Word::Word(uint64 _line, const QString& _string, WordType _type, const QString& _id)
{
  set(_line,_string, _type, _id);
}
Word::Word(uint64 _line, const QString& _string, WordType _type, int64 _num)
{
  set(_line,_string, _type, _num);
}

void Word::set(uint64 _line, const QString& _string, WordType _type)
{
  this->line = _line;
  this->string = _string;
  this->type = _type;
  this->id = "";
  this->num = 0;
}
void Word::set(uint64 _line, const QString& _string, WordType _type, const QString &_id)
{
  this->line = _line;
  this->string = _string;
  this->type = _type;
  this->id = _id;
  this->num = 0;
}
void Word::set(uint64 _line, const QString& _string, WordType _type, int64 _num)
{
  this->line = _line;
  this->string = _string;
  this->type = _type;
  this->id = "";
  this->num = _num;
}

bool Word::isDelimiter() const
{
  return (int)this->type >= SYM_min_delimiter && (int)this->type <= SYM_max_delimiter;
}

bool Word::isOperator() const
{
  return (int)this->type >= SYM_min_operator && (int)this->type <= SYM_max_operator;
}

bool Word::isKeyWord() const
{
  return (int)this->type >= SYM_min_keyword && (int)this->type <= SYM_max_keyword;
}

bool Word::isIdent() const
{
  return (int)this->type == SYM_ident;
}

bool Word::isNumber() const
{
  return (int)this->type == SYM_number;
}

bool Word::isUndefine() const
{
  return (int)this->type == SYM_undefine;
}

WordType Word::getType()
{
  return this->type;
}

int64 Word::getNum()
{
  return this->num;
}

uint64 Word::getLine()
{
  return this->line;
}

QString Word::getId()
{
  return this->id;
}

QString Word::getString()
{
  return this->string;
}

void Word::debugPrint() const
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
