#ifndef INSTRUCTION_H
#include <functional>
#include <QDebug>
#define INSTRUCTION_H

enum class InstructionType
{
  LIT = 0,
  LOD = 1,
  STO = 2,
  CAL = 3,
  INT = 4,
  JMP = 5,
  JPC = 6,
  OPR = 7,
};

class Instruction
{
public:
  Instruction();
  Instruction(InstructionType type, int levelDiff, int operand);

  InstructionType getType() const;
  void setType(InstructionType newType);

  int getLevelDiff() const;
  void setLevelDiff(int newLevelDiff);

  int getOperand() const;
  void setOperand(int newOperand);

  void print();

private:
  InstructionType type;
  int levelDiff;
  int operand;
};

typedef std::vector<Instruction> InstructionList;

#endif // INSTRUCTION_H
