#include "errorinformation.h"

ErrorInformation::ErrorInformation()
{

}

QString ErrorInformation::getError()
{
  QString re;
  for(auto s : this->errors)
  {
    re += s;
    re += "\n";
  }
  return re;
}

void ErrorInformation::clear()
{
  this->errors.clear();
}

void ErrorInformation::addLexicalAnalyzerErrorIdentifierBegin(uint64 lines)
{
  this->errors.push_back("Error in line " + QString::number(lines) + ":Expect a identifier.It should start with a letter or number.");
}

void ErrorInformation::addLexicalAnalyzerErrorUnknowError(uint64 lines)
{
  this->errors.push_back("Error in line " + QString::number(lines) + ":Expect a identifier.It should start with a letter or number.");
}

void ErrorInformation::addLexicalAnalyzerErrorNumberTooLong(uint64 lines)
{
  this->errors.push_back("Error in line " + QString::number(lines) + ":Unknow error on lexical analyse.");
}
