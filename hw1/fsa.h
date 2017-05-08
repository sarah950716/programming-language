// PL homework: hw1
// fsa.h

#ifndef _PL_HOMEWORK_FSA_H_
#define _PL_HOMEWORK_FSA_H_
#define ROWS 100
#define COLS 27

#include <vector>
#include <string>
#include <queue>
#include <map>
#include <set>

using namespace std;

// Valid characters are alphanumeric and underscore (A-Z,a-z,0-9,_).
// Epsilon moves in NFA are represented by empty strings.

struct FSATableElement {
  int state;
  int next_state;
  std::string str;
};

struct FiniteStateAutomaton {
  // Make your own FSA struct here.
	int nfa_state_num, dfa_state_num;
	set<char> alphabets;
	vector<pair<int,char> >  nfa[100];
	vector<int> reachables[100][27];
	int dfa[100][27];
	map<vector<int>,int> state_converter;
	vector<int> accept_states;
};

// Run FSA and return true if str is matched by fsa, and false otherwise.
bool RunFSA(const FiniteStateAutomaton& fsa, const std::string& str);

void BuildNFA(const std::vector<FSATableElement>& elements,
							FiniteStateAutomaton* fsa);

void dfsReachableStates(int start_state, 
												int cur_state,
												char path,
												bool is_visited[ROWS][COLS],
												FiniteStateAutomaton* fsa);

void getReachableStates(int start_state,
												FiniteStateAutomaton* fsa);

void dfsStartStates(int cur_state, 
										bool is_visited[ROWS], 
										vector<int>& start_states, 
										FiniteStateAutomaton* fsa);

vector<int> getStartStates(int start_state,
													 FiniteStateAutomaton* fsa);

void convertSetToVector(set<int>& s, 
												vector<int>& v);

void getAcceptStates(const std::vector<int>& nfa_accept_states, FiniteStateAutomaton* fsa);

bool BuildFSA(const std::vector<FSATableElement>& elements,
              const std::vector<int>& accept_states,
              FiniteStateAutomaton* fsa);

#endif //_PL_HOMEWORK_FSA_H_

