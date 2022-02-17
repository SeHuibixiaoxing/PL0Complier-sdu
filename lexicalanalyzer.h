#pragma once

#include "defs.h"
#include <vector>
#include <tuple>
#include "token.h"
#include "errorinformation.h"


class Lexer
{
public:
    Lexer();
    bool analyse();                         //进行词法分析
    void send(TokenList *_wordlist, ErrorInformation* _errorInformation, const QString& _code);

    bool hasFinish();                       //是否分析完成
    void clear();                           //清空

    /*测试用*/
    void debugPrintTokenList();              //输出WordList

private:
    TokenList *tokenList;

    QString code;                           //代码

    ErrorInformation* errorInformation;     //存储错误信息

    std::tuple<int32, Token> analysisToken(int beginRank, uint64 line) const;  //从beginRank开始(非空格、换行)，识别一个单词，返回终止下标和识别到的单词

    void sendTokenList(TokenList *_tokenList);
    void sendErrorInformation(ErrorInformation* _errorInformation);
    void sendCode(const QString& _code);     //输入代码

    bool hasAnalyze = false;                //是否进行过词法分析



};

