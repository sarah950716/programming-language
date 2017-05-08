// PL homework: hw1
// fsa.cc

#include <iostream>
#include <string>
#include "fsa.h"

#define DISABLE_LOG true
#define LOG \
    if (DISABLE_LOG) {} else std::cerr
#define ROWS 100
#define COLS 27 //a-z: 0-25, epsilon: 26
#define EPSILON 26

using namespace std;

bool RunFSA(const FiniteStateAutomaton& fsa, const string& str) {
  // Implement this function.
	int cur_state = 1, next_state = 1;
	for(int i=0; i<str.size(); i++) {
		next_state = fsa.dfa[cur_state][str[i]-'a'];
		cur_state = next_state;
	}

	return binary_search(fsa.accept_states.begin(), fsa.accept_states.end(), cur_state);
}

//Build nfa of fsa 
void BuildNFA(const std::vector<FSATableElement>& elements,
							FiniteStateAutomaton* fsa) {
	int trans_num = elements.size();
	int state_num = 0;

	for(int i=0; i<trans_num; i++) {
		FSATableElement cur_trans = elements[i];

		int alpha_num = cur_trans.str.size();
		//입실론 transition인 경우
		if(alpha_num == 0) {
			(fsa->nfa)[cur_trans.state].push_back(make_pair(cur_trans.next_state, ' '));
		} else {
			for(int j=0; j<alpha_num; j++) {
				(fsa->nfa)[cur_trans.state].push_back(make_pair(cur_trans.next_state, cur_trans.str[j]));
				fsa->alphabets.insert(cur_trans.str[j]);
			}
		}
		state_num = max(max(state_num, cur_trans.state), cur_trans.next_state);
	}

	fsa->nfa_state_num = state_num;
}

void dfsReachableStates(int start_state, int cur_state, char path, bool is_visited[ROWS][COLS], FiniteStateAutomaton* fsa) {
	if(path != ' ') {
		fsa->reachables[start_state][path - 'a'].push_back(cur_state);
	}
	
	int next_states_num = fsa->nfa[cur_state].size();

	for(int i=0; i<next_states_num; i++) {
		pair<int,char> next_state = fsa->nfa[cur_state][i];
	
		if(path == ' ') {
			if(next_state.second == ' ') {
				if(is_visited[next_state.first][EPSILON] == false) {
					is_visited[next_state.first][EPSILON] = true;
					dfsReachableStates(start_state, next_state.first, path, is_visited, fsa);
				}
			} else {
				if(is_visited[next_state.first][next_state.second - 'a'] == false) {
					is_visited[next_state.first][next_state.second - 'a'] = true;
					dfsReachableStates(start_state, next_state.first, next_state.second, is_visited, fsa);
				}
			}
		} else {
			if(next_state.second == ' ') {
				if(is_visited[next_state.first][path - 'a'] == false) {
				  is_visited[next_state.first][path - 'a'] = true;
					dfsReachableStates(start_state, next_state.first, path, is_visited, fsa);
				}
			}
		}
	}
}

void getReachableStates(int start_state, FiniteStateAutomaton* fsa) {
	bool is_visited[ROWS][COLS];

	int state_num = fsa->nfa_state_num;

	for(int i=1; i<=state_num; i++) {
		for(int j=0; j<COLS; j++) {
			is_visited[i][j] = false;
		}
	}

	is_visited[start_state][EPSILON] = true;
	dfsReachableStates(start_state, start_state, ' ', is_visited, fsa);
}

void dfsStartStates(int cur_state, bool is_visited[ROWS], vector<int>& start_states, FiniteStateAutomaton* fsa) {
	int state_num = fsa->nfa_state_num;
	start_states.push_back(cur_state);

	int next_states_num = fsa->nfa[cur_state].size();

	for(int i=0; i<next_states_num; i++) {
		pair<int,char> next_state = fsa->nfa[cur_state][i];
		
		if(next_state.second == ' ' && is_visited[next_state.first] == false) {
			is_visited[next_state.first] = true;
			dfsStartStates(next_state.first, is_visited, start_states, fsa);
		}
	}	
}

vector<int> getStartStates(int start_state, FiniteStateAutomaton* fsa) {
	int state_num = fsa->nfa_state_num;
	vector<int> start_states;
	bool is_visited[ROWS];
	
	for(int i=1; i<=state_num; i++) {
		is_visited[i] = false;
	}

	is_visited[start_state] = true;
	dfsStartStates(start_state, is_visited, start_states, fsa);
	return start_states;
}

void convertSetToVector(set<int>& s, vector<int>& v) {
	set<int>::iterator it;
	for(it=s.begin(); it!=s.end(); it++) {
		v.push_back(*it);
	}
}

void getAcceptStates(const std::vector<int>& nfa_accept_states,
										 FiniteStateAutomaton* fsa) {
	map<vector<int>, int>::iterator it;
	vector<int> dfa_accept_states;

	for(it=fsa->state_converter.begin(); it!=fsa->state_converter.end(); it++) {
		for(int i=0; i<it->first.size(); i++) {
			if(binary_search(nfa_accept_states.begin(), nfa_accept_states.end(), it->first[i])) {
				dfa_accept_states.push_back(it->second);
				break;
			}
		}
	}

	sort(dfa_accept_states.begin(), dfa_accept_states.end());
	fsa->accept_states = dfa_accept_states;
}

bool BuildFSA(const std::vector<FSATableElement>& elements,
              const std::vector<int>& accept_states,
              FiniteStateAutomaton* fsa) {
	BuildNFA(elements, fsa);

	//각 state들에서 알파벳 하나와 입실론 트랜지션들로 갈 수 있는 모든 state들을 구한다
	for(int i=1; i<=fsa->nfa_state_num; i++) {
		getReachableStates(i, fsa);	
	}

	vector<int> start_states = getStartStates(1, fsa);
	
	queue<vector<int> > new_states;
	
	new_states.push(start_states);
	fsa->dfa_state_num++;
	fsa->state_converter[start_states] = fsa->dfa_state_num;

	set<char>::iterator it1, it2;
	map<vector<int>, int>::iterator it3, it4;

	while(!new_states.empty()) {
		vector<int> new_state = new_states.front();
		new_states.pop();
		
		int dfa_state = (fsa->state_converter.find(new_state))->second;

		for(it1=fsa->alphabets.begin(); it1!=fsa->alphabets.end(); it1++) {
			char alphabet = *it1;
			set<int> reachables;
			
			//new_state[j]에서 alphabet transition을 이용해 갈 수 있는 모든 state를 구한다.
			for(int j=0; j<new_state.size(); j++) {
				int reachables_num = fsa->reachables[new_state[j]][alphabet - 'a'].size();
				for(int k=0; k<reachables_num; k++) {
					reachables.insert(fsa->reachables[new_state[j]][alphabet - 'a'][k]);
				}
			}
	
			vector<int> next_states;
			convertSetToVector(reachables, next_states);

			it4 = fsa->state_converter.find(next_states);

			if(it4 == fsa->state_converter.end()) {
				new_states.push(next_states);
				fsa->dfa_state_num++;
				fsa->state_converter[next_states] = fsa->dfa_state_num;
				fsa->dfa[dfa_state][alphabet - 'a'] = fsa->dfa_state_num;
			} else {
				fsa->dfa[dfa_state][alphabet - 'a'] = it4->second;
			}
		}
	}

	vector<int> sorted_accept_states = accept_states;
	sort(sorted_accept_states.begin(), sorted_accept_states.end());
	getAcceptStates(sorted_accept_states, fsa);

	// Implement this function.
  LOG << "num_elements: " << elements.size()
      << ", accept_states: " << accept_states.size() << endl;
  return true;
}

