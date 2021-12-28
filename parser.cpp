#include "parser.h"

Parser::Parser()
{
  this->tokenList = nullptr;
  this->errorInformation = nullptr;
  this->instructionList = nullptr;
}

Parser::Parser(const TokenList *_tokenList, ErrorInformation *_errorInformation, InstructionList* _instructionList)
{
  this->tokenList = _tokenList;
  this->errorInformation = _errorInformation;
  this->instructionList = _instructionList;
}

void Parser::sendTokenList(const TokenList *_tokenList)
{
  this->tokenList = _tokenList;
}

void Parser::sendErrorInformation(ErrorInformation *_errorInformation)
{
  this->errorInformation = _errorInformation;
}

void Parser::sendInstructionList(InstructionList *_instructionList)
{
  this->instructionList = _instructionList;
}

bool Parser::empty()
{
  return this->tokenList == nullptr || this->errorInformation == nullptr || this->instructionList == nullptr;
}

void Parser::insertTableItem(const LRState& _state, const ProductionChar& _terminal, const ParsingTableItem& _item)
{
  this->parsingTable.push_back(std::make_pair(std::make_pair(_state, _terminal), _item));
}

void Parser::insertTableItem(const LRState &_state, const ProductionChar &_terminal, const ParsingTableItemType &_type, int _id)
{
  this->insertTableItem(_state, _terminal, ParsingTableItem{_type, _id});
}

Parser::ParsingTableItem Parser::searchParsingTable(const LRState &_state, const ProductionChar &_terminal)
{
  ParsingTableItem re;
  for(auto item : parsingTable)
  {
    if(item.first.first == _state && item.first.second.getType() == _terminal.getType() && item.first.second.getTokenName() == _terminal.getTokenName())
    {
      re = item.second;
      break;
    }
  }
  return re;
}

int Parser::insertProduction(const Production &_production)
{
  this->productionList.push_back(_production);
  return productionList.size() - 1;
}

int Parser::insertProduction(const QString &_leftName, const std::vector<std::pair<int, QString>>& _right, std::function<void(ProductionChar &left, std::vector<ProductionChar>&)> const & _func)
{
  ProductionCharList tmp;
  for(auto a : _right)
  {
    if((ProductionCharType)a.first == ProductionCharType::EMPTY) continue;
    tmp.push_back(ProductionChar((ProductionCharType)a.first, a.second, a.second));
  }
  return this->insertProduction(Production(ProductionChar(ProductionCharType::UNTERMINAL, _leftName, _leftName), tmp, _func));
}




void Parser::buildProduction()
{
  /*
   * 程序:E
   * 分程序：BLOCK
   * 常量说明部分：CONST_PART
   * 变量说明部分：VARIABLE_PART
   * 过程说明部分：PROCEDURE_PART
   * 语句：STATEMENT
   * 常量定义：CONST_DEFINE
   * 标识符：identifier
   * 无符号整数：identifier
   * 过程首部：PROCEDURE_HEADER
   * 赋值语句：ASSIGN_STATEMENT
   * 条件语句：CONDITION_STATEMENT
   * 当型循环语句：WHILE_STATEMENT
   * 过程调用语句：CALL_STATEMENT
   * 读语句：READ_STATEMENT
   * 写语句：WRITE_STATEMENT
   * 复合语句：COMPOUND_STATEMENT
   * 表达式：EXPRESSION
   * 条件：CONDITION
   * 关系运算符：RELATION_OPT
   * 项：ITEM
   * 加减运算符：ADD_SUB_OPT
   * 因子：FACTOR
   * 乘除运算符：MULTIPLY_DIVIDE_OPT
   * 空：^
 一些属性名及作用：
 nxq:最后一条指令的地址
 needFillAddr:待重填指令的地址
 beginAddr:过程第一条指令的开始地址
 size:变量个数

 一些方法名及作用：
 getName():返回非终结符的名称（不在Attribute里,identifier的名称就是代码中的id名，number的名称就是代码中的数字的字符串表示）
 gen(address, type, levelDiff, operand):三地址代码，address表示指令地址，type表示操作类型，levelDiff表示层次差，operand表示操作数

 pushNewTable():在符号表栈中压入并新建一个符号表
 popDeleteTable():在符号表栈中弹出并删除一个符号表
 enterConstant(name, value)
 enterVariable(name)
 enterProcedure(name, address, size)
 search(name):返回一个item，它包含若干属性：name, type(PROCEDURE、VARIABLE、CONSTANT), value(常量值), level(变量、过程所在层次), address(变量相对地址，过程跳转地址), size(过程中变量个数+3)
 refillAddress(name, address):查找表中name过程名，回填address
 refillSize(name, size):查找表中name过程名，回填size


 E' -> E
 E -> EM BLOCK_FIRST  {gen(0, JMP, 0, BLOCK_FIRST.beginAddr)}
                      重填跳转到主程序入口
 EM -> ^ {nxq = 0;
          pushNewTable("0main");
          gen(nxq, JMP, 0, -1)；}
         设置第一条指令位置，新建主程序的符号表，填写跳转到主程序执行过程的指令（待重填）

 BLOCK_FIRST -> BLOCKM_FIRST STATEMENT {
          BLOCK_FIRST.beginAddr = BLOCKM_FIRST.beginAddr;
          nxq = nxq + 1;
          gen(nxq, OPR, 0, 0);}
          填写第一条指令位置;生成返回指令
 BLOCKM_FIRST -> CONST_PART' VARIABLE_PART' PROCEDURE_PART'  {
              nxq = nxq + 1;
              gen(nxq, INT, 0, VARIABLE_PART'.size);
              BLOCKM_FIRST.beginAddr = nxq;}
          生成BLOCKM_FIRST的第一条指令INT，开辟数据空间；设置第一条指令的地址


 CONST_PART' -> CONST_PART
 CONST_PART' -> ^
 VARIABLE_PART' -> VARIABLE_PART    {VARIABLE_PART'.size = VARIABLE_PART.size;}
 VARIABLE_PART' -> ^                {VARIABLE_PART'.size = 3;}
 PROCEDURE_PART' -> PROCEDURE_PART
 PROCEDURE_PART' -> ^

 CONST_PART -> const CONST_DEFINE CONST_MORE ;
 CONST_MORE ->  ,CONST_DEFINE CONST_MORE
 CONST_MORE ->  ^
 CONST_DEFINE -> identifier = number {enterConstant( identifier.getName(), str2int(number.getName()) )}
                  将常量加入栈顶符号表

 VARIABLE_PART -> var identifier VARIABLE_MORE ; {enterVariable(identifier.getName()); VARIABLE_PART.size = VARIABLE_MORE.size + 1;}
                  将变量加入符号表,计算变量个数
 VARIABLE_MORE -> , identifier VARIABLE_MORE {enterVariable(identifier.getName()); VARIABLE_MORE_Left.size = 1 + VARIABLE_MORE_Right.size;}
                   将变量加入符号表,计算变量个数
 VARIABLE_MORE -> ^ {VARIABLE_MORE.size = 3;}
                  设置变量个数初始值

 PROCEDURE_PART -> PROCEDURE_MORE PROCEDURE_HEADER BLOCK ;  {
                      refillAddress(getTopTableName(),BLOCK.beginAddr);
                      refillSize(getTopTableName(), BLOCK.size);
                      popDeleteTable();
                      }
                   回填Address和size;弹出并移除栈顶符号表;
 PROCEDURE_MORE -> PROCEDURE_PART
 PROCEDURE_MORE -> ^
 PROCEDURE_HEADER -> procedure identifier ; {
                    enterProcedure(identifier.getName(), -1, -1);
                    pushNewTable(identifier.getName());}
                    将过程加入符号表;新建一个符号表,压入栈顶

 BLOCK -> BLOCKM STATEMENT {
            BLOCK.beginAddr = BLOCKM.beginAddr;
            nxq = nxq + 1;
            gen(nxq, OPR, 0, 0);
            BLOCK.size = BLOCKM.size}
            填写该BLOCK第一条指令位置;生成返回指令;记录size

 BLOCKM -> CONST_PART' VARIABLE_PART' PROCEDURE_PART'  {
             BLOCKM.size = VARIABLE_PART'.size;
             nxq = nxq + 1;
             gen(nxq, INT, 0, searchSize(getTopTableName, nxq));
             BLOCKM.beginAddr = nxq;}
             填写当前过程变量大小;生成该BLOCK的第一条指令INT，开辟数据空间;设置第一条指令的地址


 STATEMENT -> ASSIGN_STATEMENT
 STATEMENT -> CONDITION_STATEMENT
 STATEMENT -> WHILE_STATEMENT
 STATEMENT -> CALL_STATEMENT
 STATEMENT -> READ_STATEMENT
 STATEMENT -> WRITE_STATEMENT
 STATEMENT -> COMPOUND_STATEMENT
 STATEMENT -> ^


 ASSIGN_STATEMENT -> identifier := EXPRESSION
                      {
                        item = search(identifier.getName());
                        nxq = nxq + 1;
                        gen(nxq, STO, getNowLevel() - item.getLevel(), item.getAddress());
                      }

 CONDITION -> EXPRESSION RELATION_OPT EXPRESSION
              {
                nxq = nxq + 1;
                gen(nxq, OPR, 0, RELATION_OPT.type);
              }
 CONDITION -> odd EXPRESSION
              {
                nxq = nxq + 1;
                gen(nxq, OPR, 0, 6);
              }

 EXPRESSION -> EXPRESSION_FIRST_CALCULATE EXPRESSION_MORE_OPT
 EXPRESSION_FIRST_CALCULATE -> EXPRESSION_HEADER ITEM
                                {
                                  if(EXPRESSION_HEADER.type == 1)
                                  {
                                      nxq = nxq + 1;
                                      gen(nxq, OPR, 0, 1);
                                  }
                                }
 EXPRESSION_HEADER -> + {EXPRESSION_HEADER.type = 0;}
 EXPRESSION_HEADER -> - {EXPRESSION_HEADER.type = 1;}
 EXPRESSION_HEADER -> ^ {EXPRESSION_HEADER.type = 0;}
 EXPRESSION_MORE_OPT -> EXPRESSION_MORE_OPT_CALCULATE EXPRESSION_MORE_OPT
 EXPRESSION_MORE_OPT_CALCULATE -> ADD_SUB_OPT ITEM
                                  {
                                    nxq = nxq + 1;
                                    gen(nxq, OPR, 0, ADD_SUB_OPT.type);
                                  }
 EXPRESSION_MORE_OPT -> ^



 ITEM -> FACTOR ITEM_MORE_OPT
 ITEM_MORE_OPT -> ITEM_MORE_OPT CALCULATE_ITEM

 CALCULATE_ITEM -> MULTIPLY_DIVIDE_OPT FACTOR
                    {
                      nxq = nxq + 1;
                      gen(nxq, OPR, 0, MULTIPLY_DIVIDE_OPT.type);
                    }
 ITEM_MORE_OPT -> ^

 FACTOR  -> identifier      {item = search(identifier.getString());nxq = nxq +1;gen(nxq, LOD, getNowLevel() - item.level, item.address);} 将操作数放到栈顶
 FACTOR  -> number          {nxq = nxq + 1;gen(nxq, LIT, 0, number.getString()).toInt());} 将常数放到栈顶
 FACTOR  -> ( EXPRESSION )

 ADD_SUB_OPT -> + {ADD_SUB_OPT.type = 2;}
 ADD_SUB_OPT -> - {ADD_SUB_OPT.type = 3;}
 MULTIPLY_DIVIDE_OPT -> * {MULTIPLY_DIVIDE_OPT.type = 4;}
 MULTIPLY_DIVIDE_OPT -> / {MULTIPLY_DIVIDE_OPT.type = 5;}

 RELATION_OPT -> = {RELATION_OPT.type = 8;}
 RELATION_OPT -> # {RELATION_OPT.type = 9;}
 RELATION_OPT -> < {RELATION_OPT.type = 10;}
 RELATION_OPT -> >= {RELATION_OPT.type = 11;}
 RELATION_OPT -> > {RELATION_OPT.type = 12;}
 RELATION_OPT -> <= {RELATION_OPT.type = 13;}


 COMPOUND_STATEMENT -> begin STATEMENT STATEMENT_MORE end
 STATEMENT_MORE -> ; STATEMENT STATEMENT_MORE
 STATEMENT_MORE -> ^

 CONDITION_STATEMENT -> if CONDITION then TRUE_OUT STATEMENT
                        {
                          gen(TRUE_OUT.nxq, JPC, 0, nxq + 1)
                        }
 TRUE_OUT -> ^
            {
              nxq = nxq + 1;
              gen(nxq, JPC, 0, -1);
              TRUE_OUT.nxq = nxq;
              TRUE_OUT.address = nxq + 1;
            }


 CALL_STATEMENT  -> call identifier
                  {
                    nxq = nxq + 1;
                    item = search(identifier.getName());
                    gen(nxq, CAL, getNowLevel() - item.getLevel(), searchAddress(identifier.getName(), nxq))
                  }

 WHILE_STATEMENT -> WHILE_BEGIN while CONDITION do TRUE_OUT STATEMENT
                  {
                    nxq = nxq +1;
                    gen(nxq, JMP, 0, WHILE_BEGIN.nxq);
                    gen(TRUE_OUT.nxq, JPC, 0, nxq + 1);
                  }
 WHILE_BEGIN -> ^
{
  WHILE_BEGIN.nxq = nxq + 1;
}

 READ_STATEMENT -> READ_FIRST READ_MORE )
 READ_FIRST -> read( identifier
              {
                item = search(identifier.getName())
                nxq = nxq + 1;
                gen(nxq, OPR, 0, 16);
                nxq = nxq + 1;
                gen(nxq, STO, getNowLevel() - item.getLevel(), item.getAddress());
              }
 READ_MORE -> , identifier READ_MORE
              {
                item = search(identifier.getName())
                nxq = nxq + 1;
                gen(nxq, OPR, 0, 16);
                nxq = nxq + 1;
                gen(nxq, STO, getNowLevel() - item.getLevel(), item.getAddress());
              }
 READ_MORE -> ^

 WRITE_STATEMENT -> WRITE_FIRST WRITE_MORE )
 WRITE_FIRST -> write ( EXPRESSION
              {
                nxq = nxq + 1;
                gen(nxq, OPR, 0, 14)
                nxq = nxq + 1;
                gen(nxq, OPR, 0, 15)
              }

 WRITE_MORE -> , EXPRESSION WRITE_MORE_M WRITE_MORE
 WRITE_MORE_M -> ^
              {
                nxq = nxq + 1;
                gen(nxq, OPR, 0, 14)
                nxq = nxq + 1;
                gen(nxq, OPR, 0, 15)
              }
 WRITE_MORE -> ^

仅文法版：
 0:                            E' -> E
 1:                             E -> EM BLOCK_FIRST
 2:                            EM -> ^
 3:                   BLOCK_FIRST -> BLOCKM_FIRST STATEMENT
 4:                  BLOCKM_FIRST -> CONST_PART' VARIABLE_PART' PROCEDURE_PART'
 5:                   CONST_PART' -> CONST_PART
 6:                   CONST_PART' -> ^
 7:                VARIABLE_PART' -> VARIABLE_PART
 8:                VARIABLE_PART' -> ^
 9:               PROCEDURE_PART' -> PROCEDURE_PART
10:               PROCEDURE_PART' -> ^
11:                    CONST_PART -> const CONST_DEFINE CONST_MORE ;
12:                    CONST_MORE -> , CONST_DEFINE CONST_MORE
13:                    CONST_MORE -> ^
14:                  CONST_DEFINE -> identifier = number
15:                 VARIABLE_PART -> var identifier VARIABLE_MORE ;
16:                 VARIABLE_MORE -> , identifier VARIABLE_MORE
17:                 VARIABLE_MORE -> ^
18:                PROCEDURE_PART -> PROCEDURE_MORE PROCEDURE_HEADER BLOCK ;
19:                PROCEDURE_MORE -> PROCEDURE_PART
20:                PROCEDURE_MORE -> ^
21:              PROCEDURE_HEADER -> procedure identifier ;
22:                         BLOCK -> BLOCKM STATEMENT
23:                        BLOCKM -> CONST_PART' VARIABLE_PART' PROCEDURE_PART'
24:                     STATEMENT -> ASSIGN_STATEMENT
25:                     STATEMENT -> CONDITION_STATEMENT
26:                     STATEMENT -> WHILE_STATEMENT
27:                     STATEMENT -> CALL_STATEMENT
28:                     STATEMENT -> READ_STATEMENT
29:                     STATEMENT -> WRITE_STATEMENT
30:                     STATEMENT -> COMPOUND_STATEMENT
31:                     STATEMENT -> ^
32:              ASSIGN_STATEMENT -> identifier := EXPRESSION
33:                     CONDITION -> EXPRESSION RELATION_OPT EXPRESSION
34:                     CONDITION -> odd EXPRESSION
35:                    EXPRESSION -> EXPRESSION_FIRST_CALCULATE EXPRESSION_MORE_OPT
36:    EXPRESSION_FIRST_CALCULATE -> EXPRESSION_HEADER ITEM
37:             EXPRESSION_HEADER -> +
38:             EXPRESSION_HEADER -> -
39:             EXPRESSION_HEADER -> ^
40:           EXPRESSION_MORE_OPT -> EXPRESSION_MORE_OPT EXPRESSION_MORE_OPT_CALCULATE
41:           EXPRESSION_MORE_OPT -> ^
42: EXPRESSION_MORE_OPT_CALCULATE -> ADD_SUB_OPT ITEM
43:                          ITEM -> FACTOR ITEM_MORE_OPT
44:                 ITEM_MORE_OPT -> ITEM_MORE_OPT CALCULATE_ITEM
45:                 ITEM_MORE_OPT -> ^
46:                CALCULATE_ITEM -> MULTIPLY_DIVIDE_OPT FACTOR
47:                        FACTOR -> identifier
48:                        FACTOR -> number
49:                        FACTOR -> ( EXPRESSION )
50:                   ADD_SUB_OPT -> +
51:                   ADD_SUB_OPT -> -
52:           MULTIPLY_DIVIDE_OPT -> *
53:           MULTIPLY_DIVIDE_OPT -> /
54:                  RELATION_OPT -> =
55:                  RELATION_OPT -> #
56:                  RELATION_OPT -> <
57:                  RELATION_OPT -> >=
58:                  RELATION_OPT -> >
59:                  RELATION_OPT -> <=
60:            COMPOUND_STATEMENT -> begin STATEMENT STATEMENT_MORE end
61:                STATEMENT_MORE -> ^
62:                STATEMENT_MORE -> ; STATEMENT STATEMENT_MORE
63:           CONDITION_STATEMENT -> if CONDITION then TRUE_OUT STATEMENT
64:                      TRUE_OUT -> ^
65:                CALL_STATEMENT -> call identifier
66:               WHILE_STATEMENT -> WHILE_BEGIN while CONDITION do TRUE_OUT STATEMENT
67:                   WHILE_BEGIN -> ^
68:                READ_STATEMENT -> READ_FIRST READ_MORE )
69:                    READ_FIRST -> read ( identifier
70:                     READ_MORE -> , identifier READ_MORE
71:                     READ_MORE -> ^
72:               WRITE_STATEMENT -> WRITE_FIRST WRITE_MORE )
73:                   WRITE_FIRST -> write ( EXPRESSION
74:                    WRITE_MORE -> , EXPRESSION WRITE_MORE_M WRITE_MORE
75:                    WRITE_MORE -> ^
76:                  WRITE_MORE_M -> ^
*/

  //0: E'->E
  this->insertProduction("E'", std::vector<std::pair<int, QString>>{
                                    std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "E")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：E'->E\n";
                         });
  //1: E -> EM BLOCK_FIRST
  this->insertProduction("E", std::vector<std::pair<int, QString>>{
                                    std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "EM"),
                                    std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "BLOCK_FIRST")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：E -> EM BLOCK_FIRST\n";
                           this->gen(0, InstructionType::JMP, 0, right[1].getAttribute("beginAddr")[0]);
                         });

  //2: EM -> ^
  this->insertProduction("EM", std::vector<std::pair<int, QString>>{
                                    std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：EM -> ^\n";
                           this->nxq = 0;
                           this->symbolTableManage.pushNewTable("0main");
                           this->gen(this->nxq, InstructionType::JMP, 0, -1);
                         });
  //3: BLOCK_FIRST -> BLOCKM_FIRST STATEMENT
  this->insertProduction("BLOCK_FIRST", std::vector<std::pair<int, QString>>{
                                    std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "BLOCKM_FIRST"),
                                    std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "STATEMENT")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：BLOCK_FIRST -> BLOCKM_FIRST STATEMENT\n";
                           left.setAttribute("beginAddr", Attribute {right[0].getAttribute("beginAddr")[0]});
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::OPR, 0, 0);
                         });
  //4: BLOCKM_FIRST -> CONST_PART' VARIABLE_PART' PROCEDURE_PART'
  this->insertProduction("BLOCKM_FIRST", std::vector<std::pair<int, QString>>{
                                     std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "CONST_PART'"),
                                     std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "VARIABLE_PART'"),
                                     std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "PROCEDURE_PART'")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "VARIABLE_PART'.size:" << right[1].getAttribute("size")[0] << "\n归约：BLOCKM_FIRST -> CONST_PART' VARIABLE_PART' PROCEDURE_PART'\n";
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::INT, 0, right[1].getAttribute("size")[0]);
                           left.setAttribute("beginAddr", Attribute {this->nxq});
                         });
  //5: CONST_PART' -> CONST_PART
  this->insertProduction("CONST_PART'", std::vector<std::pair<int, QString>>{
                                      std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "CONST_PART")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：CONST_PART' -> CONST_PART\n";
                         });
  //6: CONST_PART' -> ^
  this->insertProduction("CONST_PART'", std::vector<std::pair<int, QString>>{
                                      std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：CONST_PART' -> ^\n";
                         });
  //7: VARIABLE_PART' -> VARIABLE_PART
  this->insertProduction("VARIABLE_PART'", std::vector<std::pair<int, QString>>{
                                      std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "VARIABLE_PART")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "VARIABLE_PART.size:" << right[0].getAttribute("size")[0] << "\n归约：VARIABLE_PART' -> VARIABLE_PART\n";
                           left.setAttribute("size", Attribute {right[0].getAttribute("size")[0]});
                         });
  //8: VARIABLE_PART' -> ^
  this->insertProduction("VARIABLE_PART'", std::vector<std::pair<int, QString>>{
                                      std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           left.setAttribute("size", Attribute {3});
                           //qDebug() << "VARIABLE_PART'.size:" << left.getAttribute("size")[0] << "\n归约：VARIABLE_PART' -> ^\n";
                         });
  //9: PROCEDURE_PART' -> PROCEDURE_PART
  this->insertProduction("PROCEDURE_PART'", std::vector<std::pair<int, QString>>{
                                      std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "PROCEDURE_PART")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：PROCEDURE_PART' -> PROCEDURE_PART\n";
                         });
  //10: PROCEDURE_PART' -> ^
  this->insertProduction("PROCEDURE_PART'", std::vector<std::pair<int, QString>>{
                                      std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：PROCEDURE_PART' -> ^\n";
                         });
  //11: CONST_PART -> const CONST_DEFINE CONST_MORE ;
  this->insertProduction("CONST_PART", std::vector<std::pair<int, QString>>{
                                      std::make_pair(PRODUCTION_TYPE_TERMINAL, "const"),
                                      std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "CONST_DEFINE"),
                                      std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "CONST_MORE"),
                                      std::make_pair(PRODUCTION_TYPE_TERMINAL, ";")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：CONST_PART -> const CONST_DEFINE CONST_MORE ;\n";
                         });
  //12: CONST_MORE -> , CONST_DEFINE CONST_MORE
  this->insertProduction("CONST_MORE", std::vector<std::pair<int, QString>>{
                                      std::make_pair(PRODUCTION_TYPE_TERMINAL, ","),
                                      std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "CONST_DEFINE"),
                                      std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "CONST_MORE")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：CONST_MORE -> , CONST_DEFINE CONST_MORE\n";
                         });
  //13: CONST_MORE -> ^
  this->insertProduction("CONST_MORE", std::vector<std::pair<int, QString>>{
                                      std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：CONST_MORE -> ^\n";
                         });
  //14: CONST_DEFINE -> identifier = number
  this->insertProduction("CONST_DEFINE", std::vector<std::pair<int, QString>>{
                                       std::make_pair(PRODUCTION_TYPE_TERMINAL, "identifier"),
                                       std::make_pair(PRODUCTION_TYPE_TERMINAL, "="),
                                       std::make_pair(PRODUCTION_TYPE_TERMINAL, "number")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：CONST_DEFINE -> identifier = number\n";
                           this->symbolTableManage.enterConstant(right[0].getName(), right[2].getName().toInt());
                         });
  //15: VARIABLE_PART -> var identifier VARIABLE_MORE ;
  this->insertProduction("VARIABLE_PART", std::vector<std::pair<int, QString>>{
                                        std::make_pair(PRODUCTION_TYPE_TERMINAL, "var"),
                                        std::make_pair(PRODUCTION_TYPE_TERMINAL, "identifier"),
                                        std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "VARIABLE_MORE"),
                                        std::make_pair(PRODUCTION_TYPE_TERMINAL, ";")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           this->symbolTableManage.enterVariable(right[1].getName());
                           left.setAttribute("size", Attribute {right[2].getAttribute("size")[0] + 1});
                           //qDebug() << "VARIABLE_PART.size:" << left.getAttribute("size")[0] << "\n归约：VARIABLE_PART -> var identifier VARIABLE_MORE ;\n";
                         });
  //16: VARIABLE_MORE -> , identifier VARIABLE_MORE
  this->insertProduction("VARIABLE_MORE", std::vector<std::pair<int, QString>>{
                                        std::make_pair(PRODUCTION_TYPE_TERMINAL, ","),
                                        std::make_pair(PRODUCTION_TYPE_TERMINAL, "identifier"),
                                        std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "VARIABLE_MORE")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：VARIABLE_MORE -> , identifier VARIABLE_MORE\n";
                           this->symbolTableManage.enterVariable(right[1].getName());
                           left.setAttribute("size", Attribute {right[2].getAttribute("size")[0] + 1});
                         });
  //17: VARIABLE_MORE -> ^
  this->insertProduction("VARIABLE_MORE", std::vector<std::pair<int, QString>>{
                                         std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：VARIABLE_MORE -> ^\n";
                           left.setAttribute("size", Attribute {3});
                         });
  //18: PROCEDURE_PART -> PROCEDURE_MORE PROCEDURE_HEADER BLOCK ;
  this->insertProduction("PROCEDURE_PART", std::vector<std::pair<int, QString>>{
                                          std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "PROCEDURE_MORE"),
                                          std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "PROCEDURE_HEADER"),
                                          std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "BLOCK"),
                                          std::make_pair(PRODUCTION_TYPE_TERMINAL, ";")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：PROCEDURE_PART -> PROCEDURE_MORE PROCEDURE_HEADER BLOCK ;\n";

                           //qDebug() << "beginAddr:" << right[2].getAttribute("beginAddr")[0];
                           //qDebug() << "size:" << right[2].getAttribute("size")[0];

                           int value1 = right[2].getAttribute("beginAddr")[0];
                           int t1 = this->symbolTableManage.refillAddres(this->symbolTableManage.getTopTableName(), value1);
                           this->reFill(t1, value1);

                           int value2 = right[2].getAttribute("size")[0];
                           int t2 = this->symbolTableManage.refillSize(this->symbolTableManage.getTopTableName(), value2);
                           this->reFill(t2, value2);

                           this->symbolTableManage.popDeleteTable();
                         });
  //19: PROCEDURE_MORE -> PROCEDURE_PART
  this->insertProduction("PROCEDURE_MORE", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "PROCEDURE_PART")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：PROCEDURE_MORE -> PROCEDURE_PART\n";
                         });
  //20: PROCEDURE_MORE -> ^
  this->insertProduction("PROCEDURE_MORE", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：PROCEDURE_MORE -> ^\n";
                         });
  //21: PROCEDURE_HEADER -> procedure identifier ;
  this->insertProduction("PROCEDURE_HEADER", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_TERMINAL, "procedure"),
                                           std::make_pair(PRODUCTION_TYPE_TERMINAL, "identifier"),
                                           std::make_pair(PRODUCTION_TYPE_TERMINAL, ";")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：PROCEDURE_HEADER -> procedure identifier ;\n";
                           this->symbolTableManage.enterProcedure(right[1].getName(), -1, -1);
                           this->symbolTableManage.pushNewTable(right[1].getName());
                         });
  //22: BLOCK -> BLOCKM STATEMENT
  this->insertProduction("BLOCK", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "BLOCKM"),
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "STATEMENT")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：BLOCK -> BLOCKM STATEMENT\n";
                           left.setAttribute("beginAddr", Attribute {right[0].getAttribute("beginAddr")[0]});
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::OPR, 0, 0);
                           left.setAttribute("size", Attribute {right[0].getAttribute("size")[0]});
                         });
  //23: BLOCKM -> CONST_PART' VARIABLE_PART' PROCEDURE_PART'
  this->insertProduction("BLOCKM", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "CONST_PART'"),
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "VARIABLE_PART'"),
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "PROCEDURE_PART'")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：BLOCKM -> CONST_PART' VARIABLE_PART' PROCEDURE_PART'\n";
                           left.setAttribute("size", Attribute {right[1].getAttribute("size")[0]});
                           ++ this->nxq;
                           gen(nxq, InstructionType::INT, 0, this->symbolTableManage.searchSize(this->symbolTableManage.getTopTableName(), this->nxq));
                           left.setAttribute("beginAddr", Attribute {this->nxq});
                         });
  //24: STATEMENT -> ASSIGN_STATEMENT
  this->insertProduction("STATEMENT", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "ASSIGN_STATEMENT")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：STATEMENT -> ASSIGN_STATEMENT\n";
                         });
  //25: STATEMENT -> CONDITION_STATEMENT
  this->insertProduction("STATEMENT", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "CONDITION_STATEMENT")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：STATEMENT -> CONDITION_STATEMENT\n";
                         });
  //26: STATEMENT -> WHILE_STATEMENT
  this->insertProduction("STATEMENT", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "WHILE_STATEMENT")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：STATEMENT -> WHILE_STATEMENT\n";
                         });
  //27: STATEMENT -> CALL_STATEMENT
  this->insertProduction("STATEMENT", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "CALL_STATEMENT")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：STATEMENT -> CALL_STATEMENT\n";
                         });
  //28: STATEMENT -> READ_STATEMENT
  this->insertProduction("STATEMENT", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "READ_STATEMENT")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：STATEMENT -> READ_STATEMENT\n";
                         });
  //29: STATEMENT -> WRITE_STATEMENT
  this->insertProduction("STATEMENT", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "WRITE_STATEMENT")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：STATEMENT -> WRITE_STATEMENT\n";
                         });
  //30: STATEMENT -> COMPOUND_STATEMENT
  this->insertProduction("STATEMENT", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "COMPOUND_STATEMENT")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：STATEMENT -> COMPOUND_STATEMENT\n";
                         });
  //31: STATEMENT -> ^
  this->insertProduction("STATEMENT", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：STATEMENT -> ^\n";
                         });
  //32: ASSIGN_STATEMENT -> identifier := EXPRESSION
  this->insertProduction("ASSIGN_STATEMENT", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_TERMINAL, "identifier"),
                                           std::make_pair(PRODUCTION_TYPE_TERMINAL, ":="),
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "EXPRESSION")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：ASSIGN_STATEMENT -> identifier := EXPRESSION\n";
                           const auto& item = this->symbolTableManage.search(right[0].getName());
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::STO, this->symbolTableManage.getNowLevel() - item->getLevel(), item->getAddress());
                         });
  //33: CONDITION -> EXPRESSION RELATION_OPT EXPRESSION
  this->insertProduction("CONDITION", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "EXPRESSION"),
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "RELATION_OPT"),
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "EXPRESSION")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：CONDITION -> EXPRESSION RELATION_OPT EXPRESSION\n";
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::OPR, 0, right[1].getAttribute("type")[0]);
                         });
  //34: CONDITION -> odd EXPRESSION
  this->insertProduction("CONDITION", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_TERMINAL, "odd"),
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "EXPRESSION")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：CONDITION -> odd EXPRESSION\n";
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::OPR, 0, 6);
                         });
  //35: EXPRESSION -> EXPRESSION_FIRST_CALCULATE EXPRESSION_MORE_OPT
  this->insertProduction("EXPRESSION", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "EXPRESSION_FIRST_CALCULATE"),
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "EXPRESSION_MORE_OPT")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：EXPRESSION -> EXPRESSION_FIRST_CALCULATE EXPRESSION_MORE_OPT\n";
                         });
  //36: EXPRESSION_FIRST_CALCULATE -> EXPRESSION_HEADER ITEM
  this->insertProduction("EXPRESSION_FIRST_CALCULATE", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "EXPRESSION_HEADER"),
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "ITEM")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：EXPRESSION_FIRST_CALCULATE -> EXPRESSION_HEADER ITEM\n";
                           if(right[0].getAttribute("type")[0] == 1)
                           {
                             ++ this->nxq;
                             this->gen(nxq, InstructionType::OPR, 0, 1);
                           }
                         });
  //37: EXPRESSION_HEADER -> +
  this->insertProduction("EXPRESSION_HEADER", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_TERMINAL, "+")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：EXPRESSION_HEADER -> +\n";
                           left.setAttribute("type", Attribute{0});
                         });
  //38: EXPRESSION_HEADER -> -
  this->insertProduction("EXPRESSION_HEADER", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_TERMINAL, "-")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：EXPRESSION_HEADER -> -\n";
                           left.setAttribute("type", Attribute{1});
                         });
  //39: EXPRESSION_HEADER -> ^
  this->insertProduction("EXPRESSION_HEADER", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：EXPRESSION_HEADER -> ^\n";
                           left.setAttribute("type", Attribute{0});
                         });
  //40: EXPRESSION_MORE_OPT -> EXPRESSION_MORE_OPT EXPRESSION_MORE_OPT_CALCULATE
  this->insertProduction("EXPRESSION_MORE_OPT", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "EXPRESSION_MORE_OPT"),
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "EXPRESSION_MORE_OPT_CALCULATE")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：EXPRESSION_MORE_OPT -> EXPRESSION_MORE_OPT EXPRESSION_MORE_OPT_CALCULATE\n";
                         });
  //41: EXPRESSION_MORE_OPT -> ^
  this->insertProduction("EXPRESSION_MORE_OPT", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：EXPRESSION_MORE_OPT -> ^\n";
                         });
  //42: EXPRESSION_MORE_OPT_CALCULATE -> ADD_SUB_OPT ITEM
  this->insertProduction("EXPRESSION_MORE_OPT_CALCULATE", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "ADD_SUB_OPT"),
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "ITEM"),},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：EXPRESSION_MORE_OPT_CALCULATE -> ADD_SUB_OPT ITEM\n";
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::OPR, 0, right[0].getAttribute("type")[0]);
                         });
  //43: ITEM -> FACTOR ITEM_MORE_OPT
  this->insertProduction("ITEM", std::vector<std::pair<int, QString>>{
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "FACTOR"),
                                           std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "ITEM_MORE_OPT"),},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：ITEM -> FACTOR ITEM_MORE_OPT\n";
                         });
  //44: ITEM_MORE_OPT -> ITEM_MORE_OPT CALCULATE_ITEM
  this->insertProduction("ITEM_MORE_OPT", std::vector<std::pair<int, QString>>{
                                     std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "ITEM_MORE_OPT"),
                                     std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "CALCULATE_ITEM"),},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：ITEM_MORE_OPT -> ITEM_MORE_OPT CALCULATE_ITEM\n";
                         });
  //45: ITEM_MORE_OPT -> ^
  this->insertProduction("ITEM_MORE_OPT", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：ITEM_MORE_OPT -> ^\n";
                         });
  //46: CALCULATE_ITEM -> MULTIPLY_DIVIDE_OPT FACTOR
  this->insertProduction("CALCULATE_ITEM", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "MULTIPLY_DIVIDE_OPT"),
                                              std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "FACTOR")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：CALCULATE_ITEM -> MULTIPLY_DIVIDE_OPT FACTOR\n";
                           ++ this->nxq;
                           gen(this->nxq, InstructionType::OPR, 0, right[0].getAttribute("type")[0]);
                         });
  //47: FACTOR -> identifier
  this->insertProduction("FACTOR", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, "identifier")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << right[0].getName() << "归约：FACTOR -> identifier\n";
                           const auto& item = this->symbolTableManage.search(right[0].getName());
                           ++ this->nxq;
                           if(item->getType() == SymbolType::CONSTANT)
                             this->gen(nxq, InstructionType::LIT, 0, item->getValue());
                           else
                             this->gen(nxq, InstructionType::LOD, this->symbolTableManage.getNowLevel() - item->getLevel(), item->getAddress());
                         });
  //48: FACTOR -> number
  this->insertProduction("FACTOR", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, "number")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：FACTOR -> number\n";
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::LIT, 0, right[0].getName().toInt());
                         });
  //49: FACTOR -> ( EXPRESSION )
  this->insertProduction("FACTOR", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, "("),
                                              std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "EXPRESSION"),
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, ")")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：FACTOR -> ( EXPRESSION )\n";
                         });
  //50: ADD_SUB_OPT -> +
  this->insertProduction("ADD_SUB_OPT", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, "+")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：ADD_SUB_OPT -> +\n";
                           left.setAttribute("type", Attribute {2});
                         });
  //51: ADD_SUB_OPT -> -
  this->insertProduction("ADD_SUB_OPT", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, "-")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：ADD_SUB_OPT -> -\n";
                           left.setAttribute("type", Attribute {3});
                         });
  //52: MULTIPLY_DIVIDE_OPT -> *
  this->insertProduction("MULTIPLY_DIVIDE_OPT", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, "*")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：MULTIPLY_DIVIDE_OPT -> *\n";
                           left.setAttribute("type", Attribute {4});
                         });
  //53: MULTIPLY_DIVIDE_OPT -> /
  this->insertProduction("MULTIPLY_DIVIDE_OPT", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, "/")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：MULTIPLY_DIVIDE_OPT -> /\n";
                           left.setAttribute("type", Attribute {5});
                         });
  //54: RELATION_OPT -> =
  this->insertProduction("RELATION_OPT", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, "=")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：RELATION_OPT -> =\n";
                           left.setAttribute("type", Attribute {8});
                         });
  //55: RELATION_OPT -> #
  this->insertProduction("RELATION_OPT", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, "#")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：RELATION_OPT -> #\n";
                           left.setAttribute("type", Attribute {9});
                         });
  //56: RELATION_OPT -> <
  this->insertProduction("RELATION_OPT", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, "<")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：RELATION_OPT -> <\n";
                           left.setAttribute("type", Attribute {10});
                         });
  //57: RELATION_OPT -> >=
  this->insertProduction("RELATION_OPT", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, ">=")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：RELATION_OPT -> >=\n";
                           left.setAttribute("type", Attribute {11});
                         });
  //58: RELATION_OPT -> >
  this->insertProduction("RELATION_OPT", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, ">")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：RELATION_OPT -> >\n";
                           left.setAttribute("type", Attribute {12});
                         });
  //59: RELATION_OPT -> <=
  this->insertProduction("RELATION_OPT", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, "<=")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：RELATION_OPT -> <=\n";
                           left.setAttribute("type", Attribute {13});
                         });
  //60: COMPOUND_STATEMENT -> begin STATEMENT STATEMENT_MORE end
  this->insertProduction("COMPOUND_STATEMENT", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, "begin"),
                                              std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "STATEMENT"),
                                              std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "STATEMENT_MORE"),
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, "end")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：COMPOUND_STATEMENT -> begin STATEMENT STATEMENT_MORE end\n";
                         });
  //61: STATEMENT_MORE -> ^
  this->insertProduction("STATEMENT_MORE", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：STATEMENT_MORE -> ^\n";
                         });

  //62: STATEMENT_MORE -> ; STATEMENT STATEMENT_MORE
  this->insertProduction("STATEMENT_MORE", std::vector<std::pair<int, QString>>{
                                              std::make_pair(PRODUCTION_TYPE_TERMINAL, ";"),
                                              std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "STATEMENT"),
                                              std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "STATEMENT_MORE")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：STATEMENT_MORE -> ; STATEMENT STATEMENT_MORE\n";
                         });
  //63: CONDITION_STATEMENT -> if CONDITION then TRUE_OUT STATEMENT
  this->insertProduction("CONDITION_STATEMENT", std::vector<std::pair<int, QString>>{
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, "if"),
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "CONDITION"),
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, "then"),
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "TRUE_OUT"),
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "STATEMENT"),},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：CONDITION_STATEMENT -> if CONDITION then TRUE_OUT STATEMENT\n";
                           this->gen(right[3].getAttribute("nxq")[0], InstructionType::JPC, 0, this->nxq + 1);
                         });
  //64: TRUE_OUT -> ^
  this->insertProduction("TRUE_OUT", std::vector<std::pair<int, QString>>{
                                               std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：TRUE_OUT -> ^\n";
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::JPC, 0, -1);
                           left.setAttribute("nxq", Attribute{this->nxq});
                           left.setAttribute("address", Attribute {this->nxq + 1});
                         });
  //65: CALL_STATEMENT -> call identifier
  this->insertProduction("CALL_STATEMENT", std::vector<std::pair<int, QString>>{
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, "call"),
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, "identifier")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：CALL_STATEMENT -> call identifier\n";
                           ++ this->nxq;
                           const auto& item = this->symbolTableManage.search(right[1].getName());
                           this->gen(this->nxq, InstructionType::CAL, this->symbolTableManage.getNowLevel() - item->getLevel(), this->symbolTableManage.searchAddress(right[1].getName(), this->nxq));
                         });
  //66: WHILE_STATEMENT -> WHILE_BEGIN while CONDITION do TRUE_OUT STATEMENT
  this->insertProduction("WHILE_STATEMENT", std::vector<std::pair<int, QString>>{
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "WHILE_BEGIN"),
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, "while"),
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "CONDITION"),
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, "do"),
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "TRUE_OUT"),
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "STATEMENT")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：WHILE_STATEMENT -> WHILE_BEGIN while CONDITION do TRUE_OUT STATEMENT\n";
                           ++ this->nxq;
                           this->gen(nxq, InstructionType::JMP, 0, right[0].getAttribute("nxq")[0]);
                           this->gen(right[4].getAttribute("nxq")[0], InstructionType::JPC, 0, this->nxq +1);
                         });
  //67:  WHILE_BEGIN -> ^
  this->insertProduction("WHILE_BEGIN", std::vector<std::pair<int, QString>>{
                                               std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：WHILE_BEGIN -> ^\n";
                           left.setAttribute("nxq", Attribute {this->nxq + 1});
                         });
  //68: READ_STATEMENT -> READ_FIRST READ_MORE )
  this->insertProduction("READ_STATEMENT", std::vector<std::pair<int, QString>>{
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "READ_FIRST"),
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "READ_MORE"),
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, ")")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：READ_STATEMENT -> READ_FIRST READ_MORE )\n";
                         });
  //69: READ_FIRST -> read ( identifier
  this->insertProduction("READ_FIRST", std::vector<std::pair<int, QString>>{
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, "read"),
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, "("),
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, "identifier")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：READ_FIRST -> read ( identifier\n";
                           const auto& item = this->symbolTableManage.search(right[2].getName());
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::OPR, 0, 16);
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::STO, this->symbolTableManage.getNowLevel() - item->getLevel(), item->getAddress());
                         });
  //70: READ_MORE -> , identifier READ_MORE
  this->insertProduction("READ_MORE", std::vector<std::pair<int, QString>>{
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, ","),
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, "identifier"),
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "READ_MORE")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &right)->void{
                           //qDebug() << "归约：READ_MORE -> , identifier READ_MORE\n";
                           const auto& item = this->symbolTableManage.search(right[1].getName());
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::OPR, 0, 16);
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::STO, this->symbolTableManage.getNowLevel() - item->getLevel(), item->getAddress());
                         });
  //71: READ_MORE -> ^
  this->insertProduction("READ_MORE", std::vector<std::pair<int, QString>>{
                                               std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：READ_MORE -> ^\n";
                         });
  //72: WRITE_STATEMENT -> WRITE_FIRST WRITE_MORE )
  this->insertProduction("WRITE_STATEMENT", std::vector<std::pair<int, QString>>{
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "WRITE_FIRST"),
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "WRITE_MORE"),
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, ")")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：WRITE_STATEMENT -> WRITE_FIRST WRITE_MORE )\n";
                         });
  //73: WRITE_FIRST -> write ( EXPRESSION
  this->insertProduction("WRITE_FIRST", std::vector<std::pair<int, QString>>{
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, "write"),
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, "("),
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "EXPRESSION")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：WRITE_FIRST -> write ( EXPRESSION\n";
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::OPR, 0, 14);
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::OPR, 0, 15);
                         });
  //74: WRITE_MORE -> , EXPRESSION WRITE_MORE_M WRITE_MORE
  this->insertProduction("WRITE_MORE", std::vector<std::pair<int, QString>>{
                                               std::make_pair(PRODUCTION_TYPE_TERMINAL, ","),
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "EXPRESSION"),
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "WRITE_MORE_M"),
                                               std::make_pair(PRODUCTION_TYPE_UNTERMINAL, "WRITE_MORE")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：WRITE_MORE -> , EXPRESSION WRITE_MORE_M WRITE_MORE\n";
                         });
  //75: WRITE_MORE -> ^
  this->insertProduction("WRITE_MORE", std::vector<std::pair<int, QString>>{
                                               std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：WRITE_MORE -> ^\n";
                         });
  //76: WRITE_MORE_M -> ^
  this->insertProduction("WRITE_MORE_M", std::vector<std::pair<int, QString>>{
                                               std::make_pair(PRODUCTION_TYPE_EMPTY, "")},
                         [=](ProductionChar &left, std::vector<ProductionChar> &)->void{
                           //qDebug() << "归约：WRITE_MORE_M -> ^\n";
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::OPR, 0, 14);
                           ++ this->nxq;
                           this->gen(this->nxq, InstructionType::OPR, 0, 15);
                         });
}
void Parser::buildParsingTable()
{
  int terminalSize = 0, unterminalSize = 0, stateSize = 0;
  std::vector<QString> terminal;
  std::vector<QString> unterminal;

  std::ifstream infile;
  infile.open("SLRTable.txt");

  infile >> terminalSize;
  //qDebug() << terminalSize << "\n";
  for(int i = 0;i < terminalSize;++ i)
  {
    std::string tmp;
    infile >> tmp;
    terminal.push_back(QString::fromStdString(tmp));
    //qDebug() << terminal[terminal.size() - 1] << "\n";
  }

  infile >> unterminalSize;
  //qDebug() << unterminalSize << "\n";

  for(int i = 0;i < unterminalSize;++ i)
  {
    std::string tmp;
    infile >> tmp;
    unterminal.push_back(QString::fromStdString(tmp));
    //qDebug() << unterminal[unterminal.size() - 1] << "\n";
  }

  infile >> stateSize;
  //qDebug() << stateSize << "\n";

  for(int j = 0;j < stateSize;++ j)
  {
    for(int i = 0;i < terminalSize + unterminalSize;++ i)
    {
      LRState nowState;
      infile >> nowState;
      if(nowState == 0) continue;
      if(i < terminalSize)
      {
        //qDebug() << j << " " << i << ":" <<nowState << " " << terminal[i] << "\n";
        if(nowState < 0)
          this->insertTableItem(j, ProductionChar(PRODUCTION_TYPE_TERMINAL, terminal[i], terminal[i]), ParsingTableItemType::PRODUCTION, -nowState);
        else if(nowState > stateSize)
          this->insertTableItem(j, ProductionChar(PRODUCTION_TYPE_TERMINAL, terminal[i], terminal[i]), ParsingTableItemType::SUCCESS, nowState);
        else
          this->insertTableItem(j, ProductionChar(PRODUCTION_TYPE_TERMINAL, terminal[i], terminal[i]), ParsingTableItemType::STATE, nowState);
      }
      else
      {
        //qDebug() << j << " " << i << ":" <<nowState << " " << unterminal[i - terminalSize] << "\n";
        this->insertTableItem(j, ProductionChar(PRODUCTION_TYPE_UNTERMINAL, unterminal[i - terminalSize], unterminal[i - terminalSize]), ParsingTableItemType::GOTO, nowState);
      }
    }
  }

}

void printState(const std::vector<LRState>& stateStack)
{
  for(auto i : stateStack)
  {
    qDebug() << i << " ";
  }
  qDebug() << "\n";
}
void printSymbol(std::vector<ProductionChar>& symbolStack)
{
  for(auto i : symbolStack)
  {
    qDebug() << i.getName() << " ";
  }
  qDebug() << "\n";
}
void Parser::runTranslator()
{
  std::vector<LRState> stateStack;
  std::vector<ProductionChar> symbolStack;
  std::vector<ProductionChar> inputStack;

  stateStack.push_back(0);
  symbolStack.push_back(ProductionChar(PRODUCTION_TYPE_TERMINAL, "$", "$"));
  inputStack.push_back(ProductionChar(PRODUCTION_TYPE_TERMINAL, "$", "$"));
  for(auto tokenPoint = tokenList->rbegin();tokenPoint != tokenList->rend();++ tokenPoint)
  {
    auto& token = *tokenPoint;
    QString tokenName = token.getString();
    if(token.isIdent()) tokenName = "identifier";
    if(token.isNumber()) tokenName = "number";
    auto tmp = ProductionChar(PRODUCTION_TYPE_TERMINAL, token.getString(), tokenName);
    tmp.setAttribute("line", Attribute{(int)token.getLine()});
    inputStack.push_back(tmp);
  }

  while(true)
  {
    //printState(stateStack);

    LRState nowState = stateStack[stateStack.size() - 1];
    ProductionChar nowChar = inputStack[inputStack.size() - 1];



    ParsingTableItem item = this->searchParsingTable(nowState, nowChar);


    //qDebug() << "nowState: " << (int)nowState << "\n";
    //qDebug() << "nowChar TokenName: " << nowChar.getTokenName() << "\n";
    //qDebug() << "item type: " << (int)item.type << ", item id:" << item.id << "\n";


    if(item.type == ParsingTableItemType::STATE)
    {
      stateStack.push_back(item.id);
      symbolStack.push_back(nowChar);
      inputStack.pop_back();
      //printSymbol(symbolStack);
    }
    else if(item.type == ParsingTableItemType::PRODUCTION)
    {
      const Production &nowProductionInfor = productionList[item.id];

      ProductionChar newChar(PRODUCTION_TYPE_UNTERMINAL, nowProductionInfor.getLeft().getName(), nowProductionInfor.getLeft().getTokenName());

      std::vector<ProductionChar> nowRight;

      for(int i = symbolStack.size() - nowProductionInfor.getRight().size();i < symbolStack.size();++ i)
      {
        nowRight.push_back(symbolStack[i]);
      }

      auto f = nowProductionInfor.getfunc();
      f(newChar, nowRight);

      for(int i = 0;i < nowProductionInfor.getRight().size();++ i)
      {
        symbolStack.pop_back();
        stateStack.pop_back();
      }
      symbolStack.push_back(newChar);
      //printSymbol(symbolStack);

      nowState = stateStack[stateStack.size() - 1];

      item = this->searchParsingTable(nowState, newChar);
      if(item.type != ParsingTableItemType::GOTO)
      {
        //printState(stateStack);
        qDebug() << "Wrong!归约后item的类型不是GOTO！\n";
        qDebug() << "nowState: " << (int)nowState << "\n";
        qDebug() << "newChar TokenName: " << newChar.getTokenName() << "\n";
        qDebug() << "newChar Name: " << newChar.getName() << "\n";
        qDebug() << "newChar Type: " << (int)newChar.getType() << "\n";

        break;
      }
      stateStack.push_back(item.id);
      //this->debugPrintInstruction();
    }
    else if(item.type == ParsingTableItemType::SUCCESS)
    {
      qDebug() << "Success!\n";
      break;
    }
  }

  //this->debugPrintInstruction();
}

void Parser::gen(int address, InstructionType type, int levelDiff, int operand)
{
  if(address > this->instructionList->size())
  {
    qDebug() << "Panic: instruction address is larger than it should be!\n";
    return;
  }

  /*替换or新建*/
  if(address < this->instructionList->size())
  {
    (*(this->instructionList))[address] = Instruction(type, levelDiff, operand);
  }
  else //==
  {
    (*(this->instructionList)).push_back(Instruction(type, levelDiff, operand));
  }
}

void Parser::reFill(int nxq, int value)
{
  while(nxq != -1)
  {
    int tmp = (*(this->instructionList))[nxq].getOperand();
    (*(this->instructionList))[nxq].setOperand(value);
    nxq = tmp;
  }
}

void Parser::debugPrintInstruction()
{
  int k = 0;
  for(auto i : (*(this->instructionList)))
  {
    if(i.getType() == InstructionType::OPR)
    {
      qDebug() << k << ": " << "OPR " << i.getLevelDiff() << i.getOperand();
    }
    else if(i.getType() == InstructionType::LIT)
    {
      qDebug() << k << ": " << "LIT " << i.getLevelDiff() << i.getOperand();
    }
    else if(i.getType() == InstructionType::LOD)
    {
      qDebug() << k << ": " << "LOD " << i.getLevelDiff() << i.getOperand();
    }
    else if(i.getType() == InstructionType::STO)
    {
      qDebug() << k << ": " << "STO " << i.getLevelDiff() << i.getOperand();
    }
    else if(i.getType() == InstructionType::CAL)
    {
      qDebug() << k << ": " << "CAL " << i.getLevelDiff() << i.getOperand();
    }
    else if(i.getType() == InstructionType::INT)
    {
      qDebug() << k << ": " << "INT " << i.getLevelDiff() << i.getOperand();
    }
    else if(i.getType() == InstructionType::JMP)
    {
      qDebug() << k << ": " << "JMP " << i.getLevelDiff() << i.getOperand();
    }
    else if(i.getType() == InstructionType::JPC)
    {
      qDebug() << k << ": " << "JPC " << i.getLevelDiff() << i.getOperand();
    }
    ++ k;
  }
}

void Parser::build()
{
  if(this->empty())
    return;
  buildProduction();
  buildParsingTable();
  runTranslator();
}

