#include "virtualmachine.h"

VirtualMachine::VirtualMachine()
{

}

void VirtualMachine::restart(const std::list<int> &_screenInputBuffer)
{
  this->stack.clear();
  stack.push_back(this->stackBottom);
  this->P = 0;
  this->T = 0;
  this->B = 1;
  stack.push_back(0);
  stack.push_back(0);
  stack.push_back(-1);

  screenOutputBuffer = "";
  this->sendScreenInputBuffer(_screenInputBuffer);
}

void VirtualMachine::runStep(int step)
{
  for(int i = 0;i < step;++ i)
  {
    if(runOneStep() < 0) break;
  }
}

void VirtualMachine::runUntilStop()
{
  while(runOneStep() >= 0);
}

void VirtualMachine::sendScreenInputBuffer(const std::list<int> &_screenInputBuffer)
{
  this->screenInputBuffer = _screenInputBuffer;
}

void VirtualMachine::sendInstructionList(const InstructionList *_instructionList)
{
  this->instructionList = _instructionList;
}

void VirtualMachine::printInfor()
{
  qDebug() << "[P: " << this->P << "], [T: " << this->T << "], [B: " << this->B << "]\n";
  I.print();
  qDebug() << screenInputBuffer;
  qDebug() << screenOutputBuffer;
  for(int i = 0;i < stack.size();++ i) {
    qDebug() << stack[i] <<" ";
  }
}

void VirtualMachine::debugRun()
{
  printInfor();
  while(runOneStep() >= 0)
  {
    printInfor();
  }
}

int VirtualMachine::runOneStep()
{
  if(P < 0) return -1;
  this->getInstruction();
  this->addP();
  this->runInstruction();

  return 1;
}

void VirtualMachine::getInstruction()
{
  this->I = (*(this->instructionList))[P];
}

void VirtualMachine::runInstruction()
{
  int levelDiff = I.getLevelDiff();
  int value = I.getOperand();
  auto type = I.getType();
  if(type == InstructionType::LIT)
  {
    LIT(levelDiff, value);
  }
  else if(type == InstructionType::CAL)
  {
    CAL(levelDiff, value);
  }
  else if(type == InstructionType::INT)
  {
    INT(levelDiff, value);
  }
  else if(type == InstructionType::JMP)
  {
    JMP(levelDiff, value);
  }
  else if(type == InstructionType::JPC)
  {
    JPC(levelDiff, value);
  }
  else if(type == InstructionType::LOD)
  {
    LOD(levelDiff, value);
  }
  else if(type == InstructionType::OPR)
  {
    OPR(levelDiff, value);
  }
  else if(type == InstructionType::STO)
  {
    STO(levelDiff, value);
  }

}

void VirtualMachine::addP()
{
  ++ this->P;
}

int VirtualMachine::getStackTop()
{
  return stack[stack.size() - 1];
}

void VirtualMachine::LIT(int levelDiff, int value)
{
  this->stack.push_back(value);
}

void VirtualMachine::LOD(int levelDiff, int value)
{
  int now = this->B;
  while(levelDiff)
  {
    now = stack[now];
    -- levelDiff;
  }
  stack.push_back(stack[now + value]);
}

void VirtualMachine::STO(int levelDiff, int value)
{
  int now = this->B;
  while(levelDiff)
  {
    now = this->stack[now];
    -- levelDiff;
  }
  this->stack[now+value] = getStackTop();
  this->stack.pop_back();
}

void VirtualMachine::CAL(int levelDiff, int value)
{
  int SL = B;
  while(levelDiff)
  {
    SL = stack[SL];
    -- levelDiff;
  }
  stack.push_back(SL);
  stack.push_back(B);
  stack.push_back(P);
  B = T + 1;
  P = value;
}

void VirtualMachine::INT(int levelDiff, int value)
{
  this->T += value;
  while(this->stack.size() < this->T +1)
    this->stack.push_back(0);
}

void VirtualMachine::JMP(int levelDiff, int value)
{
  this->P = value;
}

void VirtualMachine::JPC(int levelDiff, int value)
{
  if(!getStackTop())
  {
    this->P = value;
  }
  stack.pop_back();
}

void VirtualMachine::OPR(int levelDiff, int value)
{
  if(value == 0)
  {
    T = B - 1;
    P = stack[T + 3];
    B = stack[T + 2];
    while(stack.size() > T + 1) stack.pop_back();
  }
  else if(value == 1)
  {
    stack[stack.size() - 1] = -1 * getStackTop();
  }
  else if(value >= 2 && value <= 5)
  {
    int l1 = getStackTop();
    stack.pop_back();
    int l2 = getStackTop();
    stack.pop_back();
    int ans = 0;

    if(value == 2) ans = (l2 + l1);
    else if(value == 3) ans = (l2 - l1);
    else if(value == 4) ans = (l2 * l1);
    else if(value == 5) ans = (l2 / l1);

    stack.push_back(ans);
  }
  else if(value == 6)
  {
    int l1 = getStackTop();
    stack.pop_back();
    if(l1 & 1)
      stack.push_back(1);
    else
      stack.push_back(0);
  }
  else if(value == 7)
  {

  }
  else if(value >= 8 && value <= 13)
  {
    int l1 = getStackTop();
    stack.pop_back();
    int l2 = getStackTop();
    stack.pop_back();
    int ans = 0;
    if(value == 8) ans = (l2 == l1);
    else if(value == 9) ans = (l2 != l1);
    else if(value == 10) ans = (l2 < l1);
    else if(value == 11) ans = (l2 >= l1);
    else if(value == 12) ans = (l2 > l1);
    else if(value == 13) ans = (l2 <= l1);

    stack.push_back(ans);
  }
  else if(value == 14)
  {
    this->screenOutputBuffer += QString::number(getStackTop());
    stack.pop_back();
  }
  else if(value == 15)
  {
    this->screenOutputBuffer += "\n";
  }
  else if(value == 16)
  {
    stack.push_back(screenInputBuffer.front());
    screenInputBuffer.pop_front();
  }
}
