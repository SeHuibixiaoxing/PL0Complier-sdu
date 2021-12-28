#include "instruction.h"

Instruction::Instruction()
{

}

Instruction::Instruction(InstructionType type, int levelDiff, int operand) : type(type),
                                                                             levelDiff(levelDiff),
                                                                             operand(operand)
{}

InstructionType Instruction::getType() const
{
  return type;
}

void Instruction::setType(InstructionType newType)
{
  type = newType;
}

int Instruction::getLevelDiff() const
{
  return levelDiff;
}

void Instruction::setLevelDiff(int newLevelDiff)
{
  levelDiff = newLevelDiff;
}

int Instruction::getOperand() const
{
  return operand;
}

void Instruction::setOperand(int newOperand)
{
  operand = newOperand;
}

void Instruction::print()
{
  if(getType() == InstructionType::OPR)
  {
    qDebug() << "OPR " << getLevelDiff() << getOperand();
  }
  else if(getType() == InstructionType::LIT)
  {
    qDebug() << "LIT " << getLevelDiff() << getOperand();
  }
  else if(getType() == InstructionType::LOD)
  {
    qDebug() << "LOD " << getLevelDiff() << getOperand();
  }
  else if(getType() == InstructionType::STO)
  {
    qDebug() << "STO " << getLevelDiff() << getOperand();
  }
  else if(getType() == InstructionType::CAL)
  {
    qDebug() << "CAL " << getLevelDiff() << getOperand();
  }
  else if(getType() == InstructionType::INT)
  {
    qDebug() << "INT " << getLevelDiff() << getOperand();
  }
  else if(getType() == InstructionType::JMP)
  {
    qDebug() << "JMP " << getLevelDiff() << getOperand();
  }
  else if(getType() == InstructionType::JPC)
  {
    qDebug() << "JPC " << getLevelDiff() << getOperand();
  }
}
