#pragma once

#include "defs.h"


class ErrorInformation
{
public:
  ErrorInformation();
  QString getError();

  void clear();


  //词法分析错误
  void addLexicalAnalyzerErrorNumberTooLong(uint64 lines);                //常数太长，长度应小于等于10
  void addLexicalAnalyzerErrorIdentifierBegin(uint64 lines);              //标识符应以数字或字母开头
  void addLexicalAnalyzerErrorUnknowError(uint64 lines);                  //未知词法分析错误


private:
  std::vector<QString> errors;
};
