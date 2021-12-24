#include "lexicalanalyzer.h"

LexicalAnalyzer::LexicalAnalyzer()
{

}

void LexicalAnalyzer::sendErrorInformation(ErrorInformation* _errorInformation)
{
  this->errorInformation = _errorInformation;
}

void LexicalAnalyzer::sendCode(const QString& _code)
{
  this->code = _code;
}

void LexicalAnalyzer::debugPrintWordList()
{
  for(auto word : *(this->wordList))
  {
    word.debugPrint();
    qDebug() << "\n";
  }
}

void LexicalAnalyzer::sendWordList(WordList *_wordList)
{
  this->wordList = _wordList;
}

std::tuple<int32, Word> LexicalAnalyzer::analysisWord(int beginRank, uint64 line) const
{
  Word word;
  QString string;
  int endRank = beginRank;

  //是否为数字
  if(this->code[beginRank].isNumber())
  {
    //如果第一个字符为数字，将它和它后面的数字解析为数字
    uint64 number = 0;
    for(;endRank < code.size() && beginRank + 9 > endRank;++ endRank)
    {
      if(this->code[endRank] < '0' || this->code[endRank] > '9')
      {
        -- endRank;
        break;
      }
    }

    //如果长度超过10，返回未定义
    if(beginRank + 9 < endRank)
    {
      this->errorInformation->addLexicalAnalyzerErrorNumberTooLong(line);
      word.set(line, string, (WordType)SYM_undefine);
    }
    else
    {
      //截取数字转换为int存入wordlist
      string = code.mid(beginRank, endRank - beginRank + 1);
      word.set(line, string, (WordType)SYM_number, string.toInt());
    }
    return std::make_tuple(endRank, word);
  }

  //是否为阶符
  for(int i = SYM_min_delimiter;i <= SYM_max_delimiter;++ i)
  {
    //越界
    if(beginRank + keyWord[i].size() > code.size())
    {
      continue;
    }
    if(code.mid(beginRank, keyWord[i].size()) == keyWord[i])
    {
      return std::make_tuple(beginRank + keyWord[i].size() - 1, Word(line, keyWord[i], (WordType)i));
    }
  }

  //是否为运算符
  for(int i = SYM_min_operator;i <= SYM_max_operator;++ i)
  {
    //越界
    if(beginRank + keyWord[i].size() > code.size())
    {
      continue;
    }
    if(code.mid(beginRank, keyWord[i].size()) == keyWord[i])
    {
      return std::make_tuple(beginRank + keyWord[i].size() - 1, Word(line, keyWord[i], (WordType)i));
    }
  }

  //是否为关键字
  for(int i = SYM_min_keyword;i <= SYM_max_keyword;++ i)
  {
    //越界
    if(beginRank + keyWord[i].size() > code.size())
    {
      continue;
    }
    if(code.mid(beginRank, keyWord[i].size()) == keyWord[i])
    {
      //如果下一个字符既不是字母，也不是数字，或者没有下一个字符，则识别为关键字
      if(beginRank + keyWord[i].size() + 1 > code.size() ||
          (!code[beginRank + keyWord[i].size()].isLetterOrNumber()))
      {
        return std::make_tuple(beginRank + keyWord[i].size() - 1, Word(line, keyWord[i], (WordType)i));
      }
    }
  }

  //否则尝试识别为标识符
  if(!code[beginRank].isLetterOrNumber())
  {
    //如果开始字符不是数字或字母，不是合法标识符，报错
    this->errorInformation->addLexicalAnalyzerErrorIdentifierBegin(line);
    return std::make_tuple(beginRank + 1, Word(line, code[beginRank], (WordType)SYM_undefine));
  }

  for(;endRank < code.size();++ endRank)
  {
    if(!code[endRank].isLetterOrNumber())
    {
      -- endRank;
      break;
    }
  }
  return std::make_tuple(endRank, Word(line, code.mid(beginRank, endRank - beginRank + 1), (WordType)SYM_ident, code.mid(beginRank, endRank - beginRank + 1)));

}

bool LexicalAnalyzer::analyse()
{
    if(this->hasAnalyze) return false;

    int line = 1;

    //去除前置空格回车
    for(auto c = code.begin();c != code.end();++ c)
    {
      if(*c == '\n') ++ line;
      if(*c == ' ' || *c == '\n' || *c == '\r' || *c == '\t')
      {
        c = code.erase(c, c+1);
      }
      else break;
    }

    int nowRank = 0;
    QChar& nowChar = code[nowRank];
    for(;nowRank < code.size();++ nowRank)
    {
      nowChar = code[nowRank];
      if(nowChar == '\n') ++ line;
      else if(nowChar != ' ' && nowChar != '\r' && nowChar != '\t')
      {
        auto [newRank, tmpWord] = this->analysisWord(nowRank, line);
        this->wordList->push_back(tmpWord);
        nowRank = newRank;
      }
    }

    this->hasAnalyze = true;
    return true;
}

void LexicalAnalyzer::send(WordList *_wordlist, ErrorInformation *_errorInformation, const QString &_code)
{
  clear();
  this->sendCode(_code);
  this->sendErrorInformation(_errorInformation);
  _errorInformation->clear();
  this->sendWordList(_wordlist);

  return;
}

bool LexicalAnalyzer::hasFinish()
{
  return this->hasAnalyze;
}

void LexicalAnalyzer::clear()
{
    this->hasAnalyze = false;
    this->code.clear();
    this->wordList = nullptr;
    this->errorInformation = nullptr;
}

