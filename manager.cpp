#include "manager.h"

Manager::Manager()
{

}

void Manager::debugLexicalAnalyzer()
{
  QString *code = new QString();
  TokenList *tokenList = new TokenList();
  ErrorInformation *errorInfor = new ErrorInformation();

  QFile file {"./input.txt"};

  if(file.open(QIODevice::ReadOnly))
  {
    qDebug() << "Open file successfully.";
  }

  *code = file.readAll();

  qDebug() << *code;

  this->lexicalAnalyzer.send(tokenList, errorInfor, *code);
  this->lexicalAnalyzer.analyse();
  this->lexicalAnalyzer.debugPrintTokenList();


  delete code;
  delete tokenList;
  delete errorInfor;
}

void Manager::debugParser()
{
  QString *code = new QString();
  TokenList *tokenList = new TokenList();
  ErrorInformation *errorInfor = new ErrorInformation();
  InstructionList *instructionList = new InstructionList();

  QFile file {"./input.txt"};

  if(file.open(QIODevice::ReadOnly))
  {
    qDebug() << "Open file successfully.";
  }

  *code = file.readAll();

  //qDebug() << *code;

  this->lexicalAnalyzer.send(tokenList, errorInfor, *code);
  this->lexicalAnalyzer.analyse();
  this->lexicalAnalyzer.debugPrintTokenList();

  this->parser.sendErrorInformation(errorInfor);
  this->parser.sendTokenList(tokenList);
  this->parser.sendInstructionList(instructionList);
  this->parser.build();

  this->parser.debugPrintInstruction();

  std::list<int> input {8, 19, 36, 9, 72, 48, 5, 0};

  this->virtualMachine.sendInstructionList(instructionList);
  this->virtualMachine.restart(input);
  this->virtualMachine.debugRun();


  delete code;
  delete tokenList;
  delete errorInfor;

}
