#ifndef PARSER_H
#include "production.h"
#include "defs.h"
#include <vector>
#include <map>
#include "token.h"
#include <utility>
#include <tuple>
#include "errorinformation.h"
#include <functional>
#include <QDebug>
#include <fstream>
#include <stack>
#include "symboltablemanage.h"
#include "instruction.h"
#define PARSER_H

typedef int LRState;

class Parser
{
public:
  Parser();
  Parser(const TokenList* _tokenList, ErrorInformation* _errorInformation, InstructionList* _instrunctionList);

  void sendTokenList(const TokenList* _tokenList);                            //传入Token List
  void sendErrorInformation(ErrorInformation* _errorInformation);             //传入Error Information
  void sendInstructionList(InstructionList* _instructionList);                //传入Instruction List

  bool empty();                                                               //是否未传入Token List,Error Information,Instruction List

  void build();                                                               //创建PL0相关生成式及翻译模式、LR分析表

  void debugPrintInstruction();

private:
  const TokenList* tokenList = nullptr;
  ErrorInformation* errorInformation = nullptr;

  enum class ParsingTableItemType
  {
    STATE = 0,
    PRODUCTION = 1,
    GOTO = 2,
    SUCCESS = 3
  };


  class ParsingTableItem
  {
  public:
    ParsingTableItemType type;
    int id;
  };

  //LR分析表管理
  std::vector<std::pair<std::pair<LRState, ProductionChar>, ParsingTableItem>> parsingTable;

  void insertTableItem(const LRState& _state, const ProductionChar& _variable, const ParsingTableItem& _item);
  void insertTableItem(const LRState& _state, const ProductionChar& _variable, const ParsingTableItemType& _type, int _id);
  ParsingTableItem searchParsingTable(const LRState& _state, const ProductionChar& _terminal);

  void buildParsingTable();


  //语法&语义分析管理
  ProductionList productionList;
  int nxq = 0;

  int insertProduction(const Production& _production);      //创建生成式，返回生成式id
  int insertProduction(const QString& _leftName, const std::vector<std::pair<int, QString>>& _right,
                       std::function<void(ProductionChar &left, std::vector<ProductionChar>&)> const & _func); //创建生成式，返回生成式id

  void buildProduction();
  void runTranslator();


  //表格管理
  SymbolTableManage symbolTableManage;

  //代码生成管理
  InstructionList* instructionList = nullptr;
  void gen(int address, InstructionType type, int levelDiff, int operand);
  void reFill(int nxq, int value);

};

#endif // PARSER_H
