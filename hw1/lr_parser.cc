// PL homework: hw2
// lr_parser.cc

#include <assert.h>

#include <iostream>
#include <vector>

#include "lr_parser.h"

#define DISABLE_LOG false
#define LOG \
    if (DISABLE_LOG) {} else std::cerr

using namespace std;

vector<int> getTokens(const std::string& str) {
	string token = "";
	vector<int> tokens;

	for(int i=0; i<str.size(); i++) {
		if(str[i] != ' ') {
			tokens.push_back(str[i]);
		}
	}

	return tokens;
}

bool BuildLRParser(const std::vector<LRTableElement>& elements,
                   const std::vector<LRRule>& rules,
                   LRParser* lr_parser) {
	int element_num = elements.size();
	
	for(int i=0; i<element_num; i++) {
		LRTableElement cur_element = elements[i];
		if(cur_element.action == GOTO) {
			lr_parser->goto_table[cur_element.state].push_back(make_pair(cur_element.symbol, cur_element.next_state));
		} else {
			lr_parser->lr_parser_table[cur_element.state][cur_element.symbol] = make_pair(cur_element.action, cur_element.next_state);
		}
	}
	
	int rule_num = rules.size();

	for(int i=0; i<rule_num; i++) {
		LRRule cur_rule = rules[i];
		lr_parser->rules[cur_rule.id][0] = cur_rule.lhs_symbol;
		lr_parser->rules[cur_rule.id][1] = cur_rule.num_rhs;
	}

	return true;
}

void shift(vector<int>& tokens, int& tokens_idx, const LRParser& lr_parser, stack<LRParserStackElement>& lr_parser_stack) {
	int cur_token = tokens[tokens_idx++];
	int prev_state = lr_parser_stack.top().next_state;
	int next_state = lr_parser.lr_parser_table[prev_state][cur_token].second;

	cout << "S" << next_state << endl;
	LRParserStackElement cur_element;
	cur_element.symbol = cur_token;
	cur_element.next_state = next_state;

	lr_parser_stack.push(cur_element);
}

bool reduce(int rule_id, const LRParser& lr_parser, stack<LRParserStackElement>& lr_parser_stack) {
	int num_rhs = lr_parser.rules[rule_id][1];

	for(int i=0; i<num_rhs; i++) {
		if(lr_parser_stack.empty()) {
			return false;
		}
		lr_parser_stack.pop();
	}

	if(lr_parser_stack.empty()) {
		return false;
	}	

	int prev_state = lr_parser_stack.top().next_state;
	LRParserStackElement cur_element;
	cur_element.symbol = lr_parser.rules[rule_id][0];
	cur_element.next_state = 0;
	
	for(int i=0; i<lr_parser.goto_table[prev_state].size(); i++) {
		if(lr_parser.goto_table[prev_state][i].first == cur_element.symbol) {
			cur_element.next_state = lr_parser.goto_table[prev_state][i].second;
			break;
		}
	}

	lr_parser_stack.push(cur_element);
	return true;
}

bool RunLRParser(const LRParser& lr_parser, const std::string& str) {
	vector<int> tokens = getTokens(str);
	int tokens_idx = 0;
	
	stack<LRParserStackElement> lr_parser_stack;
	LRParserStackElement start_element;
	start_element.symbol = -1;
	start_element.next_state = 0;

	lr_parser_stack.push(start_element);

	while(!lr_parser_stack.empty()) {
		int action = lr_parser.lr_parser_table[lr_parser_stack.top().next_state][tokens[tokens_idx]].first;

		if(action == SHIFT) {
			shift(tokens, tokens_idx, lr_parser, lr_parser_stack);
		} else if(action == REDUCE) {
			int rule_id = lr_parser.lr_parser_table[lr_parser_stack.top().next_state][tokens[tokens_idx]].second;
			cout << "R" << rule_id << endl;
			reduce(rule_id, lr_parser, lr_parser_stack);
		} else if(action == ACCEPT) {
			cout << "A" << endl;
			return true;
		}
	}

	return false;
}

