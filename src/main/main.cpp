#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include "tokenizer.h"

using namespace std;


shared_ptr<class Command> createCommand(Token, Tokenizer&);

class Variable {
	public:
	
	Variable(std::string data):
	data(data) {
		
	}
	
	string data;
};


class Command {
	public:
	virtual ~Command() {}
};

class Block: public Command, public vector<shared_ptr<Command>> {
	public:
	Block(string first, Tokenizer &tokenizer) {
		cout << "creating block" << endl;
		while (tokenizer) {
			auto token = tokenizer.getNextToken();
			push_back(createCommand(token, tokenizer));
			
			if (token == "}") {
				if (tokenizer) {
					tokenizer.getNextToken();
				}
				cout << "end block" << endl;
			}
			if (not tokenizer) {
				//Expected "}" ... error
				break;
			}
		}
	}
	
	string type;
};

class VariableDeclaration: public Command {
	
};

class FunctionCall: public Command {
	public:
	virtual ~FunctionCall() {}
	FunctionCall(string first, Tokenizer &tokenizer): functionName(first) {
		cout << "creating function call to" << first;
	}
	
	string functionName;
};


shared_ptr<Command> createCommand(Token first, Tokenizer &tokenizer) {

	cout << "parsing command " << first << endl;
	if (first == "{") {
		return shared_ptr<Command>(new Block(first, tokenizer));
	}
	else if (first == "print") { //Todo: to something fancy here
		return shared_ptr<Command>(new FunctionCall(first, tokenizer));
	}
}



int main(int argc, char** argv) {
	cout << "hej" << endl;
	
	Tokenizer tokenizer(cin);
	Block commands("{", tokenizer);
}
