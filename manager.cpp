#include "manager.h"

Manager::Manager()
{

}

void Manager::debugLexicalAnalyzer()
{
  QString *code = new QString();
  WordList *wordList = new WordList();
  ErrorInformation *errorInfor = new ErrorInformation();

  QFile file {"./input.txt"};

  if(file.open(QIODevice::ReadOnly))
  {
    qDebug() << "Open file successfully.";
  }

  *code = file.readAll();

  qDebug() << *code;

  this->lexicalAnalyzer.send(wordList, errorInfor, *code);
  this->lexicalAnalyzer.analyse();
  this->lexicalAnalyzer.debugPrintWordList();


  delete code;
  delete wordList;
  delete errorInfor;
}
