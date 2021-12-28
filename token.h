#pragma once

#include "defs.h"
#include <qstring.h>
#include <vector>
#include <QDebug>

class Token
{
public:

    Token();
    Token(uint64 _line, const QString& _string, TokenType _type);
    Token(uint64 _line, const QString& _string, TokenType _type, const QString& _id);
    Token(uint64 _line, const QString& _string, TokenType _type, int64 _num);

    void set(uint64 _line, const QString& _string, TokenType _type);
    void set(uint64 _line, const QString& _string, TokenType _type, const QString& _id);
    void set(uint64 _line, const QString& _string, TokenType _type, int64 _num);

    bool isDelimiter() const;
    bool isOperator() const;
    bool isKeyWord() const;
    bool isIdent() const;
    bool isNumber() const;
    bool isUndefine() const;

    TokenType getType() const;
    int64 getNum() const;
    uint64 getLine() const;
    QString getId() const;
    QString getString() const;

    /*测试使用*/
    void debugPrint() const;


  private:
    QString string;
    TokenType type;
    QString id;
    int64 num;
    uint64 line;
};

typedef std::vector<Token> TokenList;
