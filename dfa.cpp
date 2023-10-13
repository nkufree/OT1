#include "dfa.h"
#include <stack>
#include <iostream>
using namespace std;


void addPath(DFA* dfa, DFANode* n1, DFANode* n2, char ch)
{
    dfa->dtrain[n1][ch] = n2;
}

static set<NFANode*> epsClosure(set<NFANode*> v)
{
    stack<NFANode*> st;
    for(auto it = v.begin(); it != v.end(); it++)
    {
        st.push(*it);
    }
    set<NFANode*> ret;
    ret.insert(v.begin(), v.end());
    NFANode* node;
    while (!st.empty())
    {
        node = st.top();
        st.pop();
        auto pair = node->nextNode.find(0);
        vector<NFANode*> epsEdge;
        if(pair != node->nextNode.end())
            epsEdge = pair->second;
        for(auto espIt = epsEdge.begin(); espIt != epsEdge.end(); espIt++)
        {
            auto it = ret.find(*espIt);
            if(it == ret.end())
            {
                ret.insert(*espIt);
                st.push(*espIt);
            }
        }
    }
    return ret;
}

static set<NFANode*> nextState(Dstate* stat, char ch)
{
    set<NFANode*> v;
    for(auto it = stat->nodes.begin(); it != stat->nodes.end(); it++)
    {
        if((*it)->nextNode.find(ch) == (*it)->nextNode.end()) continue;
        v.insert((*it)->nextNode[ch].begin(), (*it)->nextNode[ch].end());
    }
    return v;
}

static Dstate* isExist(set<Dstate*>* stats, set<NFANode*>* s)
{
    for(auto it = stats->begin(); it != stats->end(); it++)
    {
        if((*it)->nodes.size() != s->size())
            continue;
        set<NFANode*> temp = (*it)->nodes;
        temp.insert(s->begin(), s->end());
        if(temp.size() == (*it)->nodes.size())
            return *it;
    }
    return nullptr;
}

DFA* NFA2DFA(NFA* nfa)
{
    DFANode* node = new DFANode(START, 0);
    DFA* dfa = new DFA(node);
    dfa->dtrain[node].clear();
    dfa->inputChar.insert(nfa->inputChar.begin(), nfa->inputChar.end());
    set<Dstate*> dstates;   // 所有的Dstate
    stack<Dstate*> stateStack;  // 未标记的Dstate
    map<Dstate*, DFANode*> state2node;
    Dstate* init = new Dstate();
    init->nodeType = START;
    set<NFANode*> sinit;
    sinit.insert(nfa->start);
    init->nodes = epsClosure(sinit);
    stateStack.push(init);
    dstates.insert(init);
    state2node[init] = node;
    while (!stateStack.empty())
    {
        Dstate* stat, *currStat = new Dstate;
        stat = stateStack.top();
        stateStack.pop();
        for(auto setIt = dfa->inputChar.begin(); setIt != dfa->inputChar.end(); setIt++)
        {
            currStat->nodes = nextState(stat,  *setIt);
            set<NFANode*> u;
            u = epsClosure(currStat->nodes);
            if(u.empty())
            {
                continue;
            }
            Dstate* nextStat = isExist(&dstates, &u);
            if(nextStat == nullptr)
            {
                Dstate* newStat = new Dstate();
                if(u.find(nfa->end) != u.end())
                {
                    newStat->nodeType = END;
                }
                else
                {
                    newStat->nodeType = PATH;
                }
                DFANode* newNode = new DFANode(newStat->nodeType, 1);
                if(newNode->nodeType == END)
                    dfa->end.push_back(newNode);
                newStat->nodes.insert(u.begin(), u.end());
                dstates.insert(newStat);
                stateStack.push(newStat);
                state2node[newStat] = newNode;
                dfa->dtrain[newNode].clear();
                addPath(dfa, state2node[stat], newNode, *setIt);
            }
            else
            {
                addPath(dfa, state2node[stat], state2node[nextStat], *setIt);
            }
        }
    }
    return dfa;
}

static bool isEquiv(DFA* dfa, map<DFANode*, set<DFANode*>>& s, DFANode* n1, DFANode* n2)
{
    map<char, DFANode*>& m1 = dfa->dtrain[n1];
    map<char, DFANode*>& m2 = dfa->dtrain[n2];
    if(m1.size() != m2.size())
        return false;
    for(auto it = m1.begin(); it != m1.end(); it++)
    {
        if(m2.find(it->first) == m2.end() || s[m1[it->first]] != s[m2[it->first]])
            return false;
    }
    return true;
}

static vector<set<DFANode*>> splitSet(DFA* dfa, vector<set<DFANode*>> status)
{
    // 构建节点与集合的映射关系
    map<DFANode*, set<DFANode*>> node2set;
    for(auto stat : status)
    {
        for(auto n : stat)
        {
            node2set[n] = stat;
        }
    }
    // 遍历每个状态集，分别进行拆分
    vector<set<DFANode*>> newStatus;
    for(auto st : status)
    {
        if(st.size() == 1)
        {
            newStatus.push_back(st);
            continue;
        }
        bool issplit = false;
        map<DFANode*, int> newmap;
        int i = 0;
        for(auto it1 = st.begin(); it1 != st.end(); it1++)
        {
            if(newmap.find(*it1) == newmap.end())
            {
                newmap[*it1] = i;
                i++;
            }
            else
            {
                continue;
            }
            for(auto it2 = it1; it2 != st.end(); it2++)
            {
                if(it2 == it1)
                    continue;
                if(it2 == st.end())
                    break;
                if(isEquiv(dfa, node2set, *it1, *it2))
                {
                    newmap[*it2] = newmap[*it1];
                }
            }
        }
        map<int, set<DFANode*>> res;
        for(auto p : newmap)
        {
            res[p.second].insert(p.first);
        }
        for(auto p : res)
        {
            newStatus.push_back(p.second);
        }
    }
    return newStatus;
}

DFA* minDFA(DFA* dfa)
{
    DFA* minDfa = new DFA(nullptr);
    vector<set<DFANode*>> status;
    set<DFANode*> notEnd;
    set<DFANode*> endNode;
    // 1. 划分终态与非终态，并构建节点到集合的映射关系
    for(auto p : dfa->dtrain)
    {
        if(p.first->nodeType == END)
        {
            endNode.insert(p.first);
        }
        else
        {
            notEnd.insert(p.first);
        }
    }
    status.push_back(notEnd);
    status.push_back(endNode);
    // 2. 使用算法继续划分

    int setNum = 0;
    while(status.size() != setNum)
    {
        setNum = status.size();
        status = splitSet(dfa, status);
    }
    // 3. 每个组选出一个代表，作为该组的状态；为了方便后续查询，在这里创建映射
    int i = 0;
    map<DFANode*, DFANode*> node2id;
    for(auto it = status.begin(); it != status.end(); it++)
    {
        DFANode* node = new DFANode(PATH, i);
        NODETYPE ntype = PATH;
        for(auto setIt = (*it).begin(); setIt != (*it).end(); setIt++)
        {
            node2id[*setIt] = node;
            if((*setIt)->nodeType == START)
            {
                ntype = START;
            }
            else if((*setIt)->nodeType == END)
            {
                ntype = END;
            }
            
        }
        node->nodeType = ntype;
        if(ntype == START)
            minDfa->start = node;
        else if(ntype == END)
            minDfa->end.push_back(node);
        i++;
    }
    // 4. 建立状态之间的转换关系，删除死状态
    for(auto it = node2id.begin(); it != node2id.end(); it++)
    {
        DFANode* node = it->second;
        for(auto charIt = dfa->inputChar.begin(); charIt != dfa->inputChar.end(); charIt++)
        {
            auto nextIt = dfa->dtrain[it->first].find(*charIt);
            if(nextIt != dfa->dtrain[it->first].end())
                addPath(minDfa, node, node2id[nextIt->second], *charIt);
        }
    }
    vector<DFANode*> removeNode;
    for(auto p : minDfa->dtrain)
    {
        if(p.first->nodeType != PATH)
            continue;
        bool flag =  true;
        for(auto nextIt = minDfa->dtrain[p.first].begin(); nextIt != minDfa->dtrain[p.first].end(); nextIt++)
        {
            if(nextIt->second->id != p.first->id)
            {
                flag = false;
                break;
            }
        }
        if(flag)
            removeNode.push_back(p.first);
    }
    for(auto it = removeNode.begin(); it != removeNode.end(); it++)
    {
        minDfa->dtrain.erase(*it);
    }
    return minDfa;
}

static void resetID(DFA* dfa)
{
    int i = 0;
    for(auto p : dfa->dtrain)
    {
        p.first->id = i++;
    }
}

void printDFA(DFA* dfa)
{
    resetID(dfa);
    cout << "------dfa start------" << endl;
    cout << "start node : " << dfa->start->id << endl;
    cout << "end node   : " ;
    for(auto n : dfa->end)
        cout << n->id << " ";
    cout << endl << endl;
    for(auto p : dfa->dtrain)
    {
        for(auto mapit : p.second)
        {
            cout << p.first->id << " -[ " << mapit.first << " ]-> " << mapit.second->id << endl;
        }
        cout << endl;
    }
    cout << "-------dfa end-------" << endl << endl;
}