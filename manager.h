#pragma once


#include "lexicalanalyzer.h"
#include "errorinformation.h"
#include "defs.h"
#include <QFile>
#include <QDebug>
#include "parser.h"
#include "instruction.h"
#include "virtualmachine.h"
#include <list>



class Manager
{
public:
    Manager();
    void debugLexicalAnalyzer();
    void debugParser();
private:
    Lexer lexicalAnalyzer;
    Parser parser;
    InstructionList instructionList;
    VirtualMachine virtualMachine;
};
