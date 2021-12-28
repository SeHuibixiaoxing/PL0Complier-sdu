#pragma once
#include <QString>

typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef int int32;


#define SYM_min_all 0
#define SYM_max_all 26

//Delimiter
#define SYM_comma 0
#define SYM_semicolon 1
#define SYM_dot 2
#define SYM_left_parenthesis 3
#define SYM_right_parenthesis 4

#define SYM_min_delimiter 0
#define SYM_max_delimiter 4


//Operator
#define SYM_equal 9
#define SYM_assign 10
#define SYM_add 11
#define SYM_sub 12
#define SYM_mul 13
#define SYM_div 14
#define SYM_unequal 15
#define SYM_less 16
#define SYM_lessequal 17
#define SYM_greater 18
#define SYM_greaderequal 19

#define SYM_min_operator 9
#define SYM_max_operator 19

//Keyword
#define SYM_const 26
#define SYM_var 27
#define SYM_procedure 28
#define SYM_begin 29
#define SYM_end 30
#define SYM_if 31
#define SYM_then 32
#define SYM_call 33
#define SYM_while 34
#define SYM_do 35
#define SYM_read 36
#define SYM_write 37
#define SYM_odd 38

#define SYM_min_keyword 26
#define SYM_max_keyword 38

//标识符
#define SYM_ident 51

//数字
#define SYM_number 52

//未定义
#define SYM_undefine 53


enum class TokenType
{
    //Delimiter
    COMMASYM =  SYM_comma,
    SEMICOLONSYM = SYM_semicolon,
    DOTSYM = SYM_dot,
    LEFTPARENTHESISSYM = SYM_left_parenthesis,
    RIGHTPARENTHESISSYM = SYM_right_parenthesis,


    //Operator
    EQUALSYM = SYM_equal,
    ASSIGNSYM = SYM_assign,
    ADDSYM = SYM_add,
    SUBSYM = SYM_sub,
    MULSYM = SYM_mul,
    DIVSYM = SYM_div,
    UNEQUALSYM = SYM_unequal,
    LESSSYM = SYM_less,
    LESSEQUALSYM = SYM_lessequal,
    GREATERSYM = SYM_greater,
    GREADEREQUALSYM = SYM_greaderequal,

    //Keyword
    CONSTSYM = SYM_const,
    VARSYM = SYM_var,
    PROCEDURESYM = SYM_procedure,
    BEGINSYM = SYM_begin,
    ENDSYM = SYM_end,
    IFSYM = SYM_if,
    THENSYM = SYM_then,
    CALLSYM = SYM_call,
    WHILESYM = SYM_while,
    DOSYM = SYM_do,
    READSYM = SYM_read,
    WRITESYM = SYM_write,
    ODDSYM = SYM_odd,


    //标识符
    IDENT = SYM_ident,

    //数字
    NUMBER = SYM_number,

    //未定义
    UNDEFINE = SYM_undefine,

};

extern QString keyWord[];

