#ifndef __DFA__
#define __DFA__

#include "nfa.h"

struct DFANode
{
    NODETYPE nodeType;
    int id;
    DFANode(NODETYPE t, int id): nodeType(t),id(id) {};
};

struct Dstate
{
    std::set<NFANode*> nodes;
    NODETYPE nodeType;
};

struct DFA
{
    DFANode* start;
    std::vector<DFANode*> end;
    std::map<DFANode*, std::map<char, DFANode*>> dtrain;
    std::set<char> inputChar;
    DFA(DFANode* s): start(s){};
};

void addPath(DFA* dfa, DFANode* n1, DFANode* n2, char ch);

DFA* NFA2DFA(NFA* nfa);
DFA* minDFA(DFA* dfa);
void printDFA(DFA* dfa);


#endif