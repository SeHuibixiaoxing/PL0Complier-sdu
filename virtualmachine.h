#ifndef VIRTUALMACHINE_H
#include "instruction.h"
#include <vector>
#include <QString>
#include <list>
#include <QDebug>
#define VIRTUALMACHINE_H


class VirtualMachine
{
public:
  VirtualMachine();

  void restart(const std::list<int> &_screenInputBuffer);
  void runStep(int step);
  void runUntilStop();

  void sendScreenInputBuffer(const std::list<int>& _screenInputBuffer);
  void sendInstructionList(const InstructionList* _instructionList);

  void printInfor();
  void debugRun();

private:
  std::vector<int> stack;
  Instruction I;
  int P;
  int T;
  int B;
  QString screenOutputBuffer;
  std::list<int> screenInputBuffer;

  const int stackBottom = -100000;

  const InstructionList* instructionList;

  int runOneStep();

  void getInstruction();
  void runInstruction();
  void addP();

  int getStackTop();


  void LIT(int levelDiff, int value);
  void LOD(int levelDiff, int value);
  void STO(int levelDiff, int value);
  void CAL(int levelDiff, int value);
  void INT(int levelDiff, int value);
  void JMP(int levelDiff, int value);
  void JPC(int levelDiff, int value);
  void OPR(int levelDiff, int value);
};

#endif // VIRTUALMACHINE_H
