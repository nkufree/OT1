#ifndef __NFA__
#define __NFA__

#include <map>
#include <vector>
#include <list>
#include <set>

enum NODETYPE{START,PATH,END};
struct NFANode
{
    NODETYPE nodeType;
    int id;
    std::map<char, std::vector<NFANode*>> nextNode;
    NFANode(NODETYPE t, int id): nodeType(t),id(id) {};
};

struct NFA
{
    NFANode* start;
    NFANode* end;
    std::list<NFANode*> nodeList;
    std::set<char> inputChar;
    NFA(NFANode* s, NFANode* e): start(s), end(e){};
};


void addPath(NFANode* n1, NFANode* n2, char ch);


NFA* oneNFA(char ch);
NFA* catNFA(NFA* nfa1, NFA* nfa2);
NFA* joinNFA(NFA* nfa1, NFA* nfa2);
NFA* repeatNFA(NFA* nfa);
void printNFA(NFA* nfa);

#endif