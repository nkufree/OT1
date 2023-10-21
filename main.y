%{
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "nfa.h"
#include "dfa.h"
#ifndef YYSTYPE
#define YYSTYPE NFA*
#endif
int yylex();
extern int yyparse();
FILE* yyin;
void yyerror(const char* s);
%}

//TODO:给每个符号定义一个单词类别
%token ALPHA DIGIT SYM LINEEND
%left '|' '-'
%%


lines   :       lines regex LINEEND { 
            printNFA($2);
            DFA* dfa = NFA2DFA($2);
            printDFA(dfa); 
            printDFA(minDFA(dfa));
            }
        |       lines LINEEND
        |
        ;

regex   :       join '|' regex {$$ = joinNFA($1, $3);}
        |       join
        ;

join    :       expr join       {$$ = catNFA($1, $2);}
        |       expr
        ;

expr    :       expr '*'        {$$ = repeatNFA($1);}
        // |       expr '+'        {}
        // |       expr '?'        {}
        |       '(' regex ')'    {$$ = $2;}
        // |       '[' charset ']' {}
        |       onechar         {$$ = $1;}
        ;

// charset :       charrange charset    {}
//         |       onechar charset
//         |       onechar
//         ;

// charrange:      DIGIT '-' DIGIT     {}
//         |       ALPHA '-' ALPHA     {}
//         ;

onechar :       DIGIT       {$$ = $1;}
        |       ALPHA       {$$ = $1;}
        |       SYM         {$$ = $1;}
        ;

%%

// programs section

int yylex()
{
    int t;
    while(1){
        t=getchar();
        if(t==' '||t=='\t'){
            //do noting
        }else if(t == '\n' || t == '\r'){
            return LINEEND;
        }
        else if(isalpha(t)){
            yylval = oneNFA((char)t);
            return ALPHA;
        }else if(isdigit(t)){
            yylval = oneNFA((char)t);
            return DIGIT;
        }else if(t == '\\'){
            t = getchar();
            if(t == '0')
            {
                yylval = oneNFA(0);
                return ALPHA;
            }else
            {
                yylval = oneNFA((char)t);
                return ALPHA;
            }
        }else if( t == '|' || t == '(' || t == ')' || t == '*')
        {
            return t;
        }
        else if(isgraph(t))
        {
            yylval = oneNFA((char)t);
            return SYM;
        }
        else    
        {
            return t;
        }
    }
}

int main(void)
{
    yyin=stdin;
    do{
        yyparse();
    }while(!feof(yyin));
    return 0;
}
void yyerror(const char* s){
    fprintf(stderr,"Parse error: %s\n",s);
    exit(1);
}