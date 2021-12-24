#pragma once


#include "lexicalanalyzer.h"
#include "errorinformation.h"
#include "defs.h"
#include <QFile>
#include <QDebug>



class Manager
{
public:
    Manager();
    void debugLexicalAnalyzer();
private:
    LexicalAnalyzer lexicalAnalyzer;
};
