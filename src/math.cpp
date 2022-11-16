#include <string>
#include <stack>
#include <math.h>

#include "include/math.hpp"
#include "include/utils.hpp"




static float precedence( char& op) {
	if (op == '+' || op == '-'){return 1;}
	if (op == '*' || op == '/'){return 2;}
	if (op == '^' || op== '%'){return 3;}
	return 0;
}
static float applyOp(float& a,float& b,char& op){
	switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '^': return pow(a, b);
        case '%': return fmod(a, b);
	}
	std::string s(1, op);
	return 0;
}
float evaluate( std::string& text){
	std::string tokens = tools::replace(text, " ", "");
	float i;
	std::stack <float> values;
	std::stack <char> ops;
	bool negative = false;
	for(i = 0; i < tokens.length(); i++){
		if(tokens[i] == '('){ops.push(tokens[i]);}
		else if(isdigit(tokens[i])){
			float val = 0;
			bool encounteredDecimal = false;
			int decPlaces = 0;
			while(i < tokens.length() && (isdigit(tokens[i]) || tokens[i] == '.')){
				if(tokens[i] == '.'){
					encounteredDecimal = true;
					i++;
					decPlaces++;
					continue;
				}
				if(encounteredDecimal == true){
					val = (val)+(tokens[i] - '0') / pow(10.0f, decPlaces);
					decPlaces++;
				}
				else{val = (val * 10) + (tokens[i] - '0');}
				i++;
			}
			values.push(val);
			i--;
		}
		else if(tokens[i] == ')'){
			while(!ops.empty() && ops.top() != '('){
				float val2 = values.top();
				values.pop();
				float val1 = values.top();
				values.pop();
				char op = ops.top();
				ops.pop();
				values.push(applyOp(val1, val2, op));
			}
            if(!ops.empty()){ops.pop();}
		}
		else{
			if(tokens[i] == '-' && (i == 0 || tokens[i-1] == '*' || tokens[i-1] == '/' || tokens[i-1] == '+' || tokens[i-1] == '-' || tokens[i-1] == '^' || tokens[i-1] == '(' || tokens[i-1] == ')')){
				negative = true;
				continue;
			}
			else{
				while(!ops.empty() && precedence(ops.top()) >= precedence(tokens[i])){
					float val2 = values.top();
					values.pop();
					float val1 = values.top();
					values.pop();
					char op = ops.top();
					ops.pop();
					values.push(applyOp(val1, val2, op));
				}
				ops.push(tokens[i]);
			}
		}

		if (negative){
			values.top() *= -1;
			negative = false;
		}
	}
	while(!ops.empty()){
		float val2 = values.top();
		values.pop();
		float val1 = values.top();
		values.pop();
		char op = ops.top();
		ops.pop();
		values.push(applyOp(val1, val2, op));
	}
	return values.top();
}
