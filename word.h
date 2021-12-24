#pragma once

#include "defs.h"
#include <qstring.h>
#include <QDebug>


class Word
{
public:

    Word();
    Word(uint64 _line, const QString& _string, WordType _type);
    Word(uint64 _line, const QString& _string, WordType _type, const QString& _id);
    Word(uint64 _line, const QString& _string, WordType _type, int64 _num);

    void set(uint64 _line, const QString& _string, WordType _type);
    void set(uint64 _line, const QString& _string, WordType _type, const QString& _id);
    void set(uint64 _line, const QString& _string, WordType _type, int64 _num);

    bool isDelimiter() const;
    bool isOperator() const;
    bool isKeyWord() const;
    bool isIdent() const;
    bool isNumber() const;
    bool isUndefine() const;

    WordType getType();
    int64 getNum();
    uint64 getLine();
    QString getId();
    Qstring getString();

    /*测试使用*/
    void debugPrint() const;


  private:
    QString string;
    WordType type;
    QString id;
    int64 num;
    uint64 line;
};

