//2018-12554 Jang Seung Hyun
#define _USE_MATH_DEFINES
#include <iostream>
#include <cstring>
#include <cmath>
#include<cfloat>
using namespace std;

double parseNumber(const char s[], int& idx);
double evaluateFunction(const char s[], int& idx);
double evaluateTerm(const char s[], int& idx);
int operatorPriority(char op);
double evaluateOperator(double left, double right, char op);
double evaluateExpression(const char s[], int& idx, int level = 1);
void skipoverSpace(const char s[], int& idx);

const int MAX_EXPR_LENGTH = 1000;
const int MAX_OPERATOR_PRIORITY_LEVEL = 3;

int main() {
	while (true) {
		char s[MAX_EXPR_LENGTH + 1];
		int idx = 0;
		cout << ">>";
		cin.getline(s, sizeof(s));
		if (!strcmp(s, "exit")) {
			cout << "Exiting..." << endl;
			break;
		}
		double result = evaluateExpression(s, idx);
		if (result == INFINITY||isnan(result)||result == -INFINITY) {
			cout << "Error while evaluating!" << endl;
		}
		else {
			cout << result << endl;
		}
	}
	return 0;
}

double evaluateFunction(const char s[], int& idx) {
	const char loga[4] = "log";
	const char cosi[4] = "cos";
	const char sine[4] = "sin";
	const char expo[4] = "exp";
	const char sqrt2[5] = "sqrt";
	double input, output;
	if (!strncmp(s+idx, "log", strlen(loga))) {
		while (s[idx] != '(') {
			idx++;
		}
		input = evaluateExpression(s, idx, 1);
		if (input < 0) {
			return NAN;
		}
		else if (input == 0) {
			return INFINITY;
		}
		output = log(input);
	}
	else if (!strncmp(s+idx, "exp", strlen(expo))) {
		while (s[idx] != '(') {
			idx++;
		}
		input = evaluateExpression(s, idx, 1);
		output = exp(input);
	}
	else if (!strncmp(s + idx, "sin", strlen(sine))) {
		while (s[idx] != '(') {
			idx++;
		}
		input = evaluateExpression(s, idx, 1);
		output = sin(input);
	}
	else if (!strncmp(s + idx, "cos", strlen(cosi))) {
		while (s[idx] != '(') {
			idx++;
		}
		input = evaluateExpression(s, idx, 1);
		output = cos(input);
	}
	else if (!strncmp(s + idx, "sqrt", strlen(sqrt2))) {
		while (s[idx] != '(') {
			idx++;
		}
		input = evaluateExpression(s, idx, 1);
		if (input < 0) {
			return NAN;
		}
		output = sqrt(input);
	}
	else {
		return NAN;
	}
	return output;
}

double evaluateTerm(const char s[], int& idx) { 
	double term;
	if (isdigit(s[idx])) { 
		term = parseNumber(s, idx); 
	}
	else if (s[idx] == '(') { 
		idx++;
		skipoverSpace(s, idx);
		int parentheses = 1;
		char innerbracket[MAX_EXPR_LENGTH+1];
		int bracket_index = 0;
		while(idx < strlen(s)){
			if (s[idx] == ')') {
				if (parentheses == 1) {
					break;
				}
				else {
					parentheses--;
				}
			}
			else if (s[idx] == '(') {
				parentheses++;
			}
			innerbracket[bracket_index] = s[idx];
			bracket_index++;
			idx++;
			skipoverSpace(s, idx);
		}
		innerbracket[bracket_index] = '\0';
		bracket_index = 0;
		term = evaluateExpression(innerbracket, bracket_index , 1);
	}
	else if (s[idx] == '-') {
		idx++;
		skipoverSpace(s, idx);
		if (isdigit(s[idx])) {
			term = (-1) * parseNumber(s, idx);
			skipoverSpace(s, idx);
		}
		else {
			term = (-1) * evaluateFunction(s, idx);
			skipoverSpace(s, idx);
		}
	}
	else { 
		char functerm[MAX_EXPR_LENGTH + 1];
		int funcindex = 0;
		while (s[idx]!='(') {
			functerm[funcindex] = s[idx];
			idx++;
			funcindex++;
		}
		int parentheses = 0;
		while (idx < strlen(s)) {
			if (s[idx] == ')') {
				if (parentheses == 1) {
					break;
				}
				else {
					parentheses--;
				}
			}
			else if (s[idx] == '(') {
				parentheses++;
			}
			functerm[funcindex] = s[idx];
			idx++;
			funcindex++;
			skipoverSpace(s, idx);
		}
		functerm[funcindex] = '\0';
		funcindex = 0;
		term = evaluateFunction(functerm, funcindex);
		skipoverSpace(s, idx);
	}
	while(idx <= strlen(s) && s[idx] != '+' && s[idx] != '-' && s[idx] != '*' && s[idx] != '/' && s[idx] != '^'){
		idx++;
	}
	return term;
}

double evaluateExpression(const char s[], int& idx, int level) {
	double left, right, term;
	int length = strlen(s);
	char now_op;
	if ((s[idx] != '+' && s[idx] != '*' && s[idx] != '/' && s[idx] != '^') && level <= MAX_OPERATOR_PRIORITY_LEVEL) { 
		left = evaluateExpression(s, idx, level + 1);
		if (left == NAN || left == INFINITY) {
			return left;
		}
		if (level > operatorPriority(s[idx])||idx>length) {
			return left;
		}
		else if (level == operatorPriority(s[idx])) {
			do {
				now_op = s[idx];
				skipoverSpace(s, idx);
				right = evaluateExpression(s, ++idx, level + 1);
				skipoverSpace(s, idx);
				left = evaluateOperator(left, right, now_op);
				if (level > operatorPriority(s[idx])) {
					return left;
				}
			} while (idx < length);
		 }
		return left;
	}

	else if (level > MAX_OPERATOR_PRIORITY_LEVEL) {
		term = evaluateTerm(s, idx);
		skipoverSpace(s, idx);		
		return term;
	}
	else if (s[idx] == ' ') {
		skipoverSpace(s, idx);
	}
}

double evaluateOperator(double left,double right, char op) { 
	if (op == '+') {
		return left + right;
	}
	else if (op == '-') {
		return left - right;
	}
	else if (op == '*') {
		return left * right;
	}
	else if (op == '/') {
		if (right == 0) {
			return INFINITY;
		}
		else {
			return left / right;
		}

	}
	else if (op == '^') {
		return pow(left, right);
	}
}

double parseNumber(const char s[], int& idx) {
	if (s[idx] != '-') {
		return atof(s+idx);
	}
	else if (s[idx] == '-') {
		return (-1)*atof(s+idx+1);
	}
	skipoverSpace(s, idx);
}

int operatorPriority(char op) { 
	if (op == '+' || op == '-') {
		return 1;
	}
	else if (op == '*' || op == '/')
	{
		return 2;
	}
	else if (op == '^') {
		return 3;
	}
}

void skipoverSpace(const char s[], int& idx) {
	while (s[idx] == ' ') {
		idx++;
	}
}