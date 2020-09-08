#include <iostream>
#include <cstring>
#include <cmath>
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
	char s[MAX_EXPR_LENGTH + 1];
	int idx;
	return 0;
}