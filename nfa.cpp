#include "nfa.h"
#include <iostream>
using namespace std;

void addPath(NFANode* n1, NFANode* n2, char ch)
{
    map<char,vector< NFANode*>>::iterator it = n1->nextNode.find(ch);
    if(it == n1->nextNode.end())
    {
        vector<NFANode*> v;
        v.push_back(n2);
        n1->nextNode[ch] = v;
    }
    else
    {
        it->second.push_back(n2);
    }
}

static void resetID(NFA* nfa)
{
    int i = 0;
    list<NFANode*>::iterator it = nfa->nodeList.begin();
    while (it != nfa->nodeList.end())
    {
        (*it)->id = i;
        it++;
        i++;
    }
}

// 构造空串和单个字符对应的NFA
NFA* oneNFA(char ch)
{
    NFANode* n1 = new NFANode(START,0);
    NFANode* n2 = new NFANode(END,1);
    addPath(n1, n2, ch);
    struct NFA* nfa = new NFA(n1, n2);
    nfa->nodeList.push_back(n1);
    nfa->nodeList.push_back(n2);
    if(ch != 0)
        nfa->inputChar.insert(ch);
    return nfa;
}

// 处理两个NFA的连接运算，中间连接处重叠，最后不用添加额外的终态
NFA* catNFA(NFA* nfa1, NFA* nfa2)
{
    // NFANode* n1 = new NFANode(START,0);
    // NFANode* n2 = new NFANode(END,1);
    NFANode* n1 = nfa1->start;
    NFANode* n2 = nfa2->end;

    nfa1->start->nodeType = PATH;
    nfa1->end->nodeType = PATH;

    // nfa2->start->nodeType = PATH;
    nfa1->end->nextNode = nfa2->start->nextNode;

    nfa2->end->nodeType = END;
    // addPath(n1, nfa1->start, 0);
    // addPath(nfa1->end, nfa2->start, 0);
    // addPath(nfa2->end, n2, 0);
    nfa1->nodeList.pop_front();
    nfa2->nodeList.pop_front();
    nfa2->nodeList.pop_back();

    
    struct NFA* nfa = new NFA(n1, n2);
    nfa->nodeList.merge(nfa1->nodeList);
    nfa->nodeList.merge(nfa2->nodeList);
    nfa->nodeList.push_front(n1);
    nfa->nodeList.push_back(n2);
    
    // 更新inputChar
    nfa->inputChar.insert(nfa1->inputChar.begin(),nfa1->inputChar.end());
    nfa->inputChar.insert(nfa2->inputChar.begin(),nfa2->inputChar.end());
    return nfa;
}

// 处理两个NFA的 | 运算
NFA* joinNFA(NFA* nfa1, NFA* nfa2)
{
    NFANode* n1 = new NFANode(START,0);
    NFANode* n2 = new NFANode(END,1);
    nfa1->start->nodeType = PATH;
    nfa1->end->nodeType = PATH;
    nfa2->start->nodeType = PATH;
    nfa2->end->nodeType = PATH;
    addPath(n1, nfa1->start, 0);
    addPath(n1, nfa2->start, 0);
    addPath(nfa1->end, n2, 0);
    addPath(nfa2->end, n2, 0);
    struct NFA* nfa = new NFA(n1, n2);
    nfa->nodeList.merge(nfa1->nodeList);
    nfa->nodeList.merge(nfa2->nodeList);
    nfa->nodeList.push_front(n1);
    nfa->nodeList.push_back(n2);

    // 更新inputChar
    nfa->inputChar.insert(nfa1->inputChar.begin(),nfa1->inputChar.end());
    nfa->inputChar.insert(nfa2->inputChar.begin(),nfa2->inputChar.end());
    return nfa;
}

// 处理NFA的 * 运算
NFA* repeatNFA(NFA* nfa)
{
    NFANode* n1 = new NFANode(START,0);
    NFANode* n2 = new NFANode(END,1);
    nfa->start->nodeType = PATH;
    nfa->end->nodeType = PATH;
    addPath(n1, nfa->start, 0);
    addPath(nfa->end, nfa->start, 0);
    addPath(nfa->end, n2, 0);
    addPath(n1, n2, 0);
    struct NFA* newNFA = new NFA(n1, n2);
    newNFA->nodeList.merge(nfa->nodeList);
    newNFA->nodeList.push_front(n1);
    newNFA->nodeList.push_back(n2);

    // 更新inputChar
    newNFA->inputChar.insert(nfa->inputChar.begin(),nfa->inputChar.end());
    return newNFA;
}

void printNFA(NFA* nfa)
{
    resetID(nfa);
    cout << "------nfa start------" << endl;
    cout << "start node : " << nfa->nodeList.front()->id << endl;
    cout << "end node   : " << nfa->nodeList.back()->id << endl << endl;
    list<NFANode*>::iterator lit = nfa->nodeList.begin();
    while (lit != nfa->nodeList.end())
    {
        if((*lit)->nodeType == END) {
            lit++;
            continue;
        }
        map<char, vector<NFANode*>>::iterator mapit;
        vector<NFANode*>::iterator vit;
        for(mapit = (*lit)->nextNode.begin(); mapit != (*lit)->nextNode.end(); mapit++)
        {
            for(vit = mapit->second.begin(); vit != mapit->second.end(); vit++)
            {
                if(mapit->first != 0)
                    cout << (*lit)->id << " -[ " << mapit->first << " ]-> " << (*vit)->id << endl;
                else
                    cout << (*lit)->id << " -[ ε ]-> " << (*vit)->id << endl;
            }
        }
        cout << endl;
        lit++;
    }
    

    cout << "-------nfa end-------" << endl << endl;
}