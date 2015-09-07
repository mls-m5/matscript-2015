/*
 * tokenizer.cpp
 *
 *  Created on: 13 okt 2014
 *      Author: Mattias Larsson Sköld
 */

#include "tokenizer.h"
#include "preprocessing_op_or_punc.h"

#include <sstream>
#include <vector>
#include <fstream>
using namespace std;

string specialCharacters = "_{}[]#()<>%:;.?*+-/^&|∼!=,\'";


static class KeyWords: public vector<string>{
public:
	KeyWords(){
		ifstream file("keywords.txt");
		string word;
		while (file >> word){
			push_back(word);
		}
	}

	bool isKeyWord(const string &c){
		for (auto &it: *this){
			if (it == c){
				return true;
			}
		}
		return false;
	}
} keyWords;

//Return 0 if no match is found
//-1 if several matches is found or one with a length larger than the expression
//index of operator if only one is found with the same length
int getPreprocessOperorOrSubString(std::string &beginning){
	int foundstring = 0;
	int count = 0;
	for (int s = 1; s < preprocessingOpOrPunc.size(); ++s){
		auto it = preprocessingOpOrPunc[s];
		bool same = true;
		if (beginning.size() > it.size()){
			//not the same
			continue;
		}
		for (int i = 0; i < beginning.size(); ++i){
			if (beginning[i] != it[i]){
				same = false;
				break;
			}
		}
		if (same){
			if (foundstring){
				return -1; //Several matches
			}
			else{
				foundstring = s;
			}
		}
	}

	return foundstring;
}

Token Tokenizer::GetNextToken(std::istream& stream, Location *location) {
	stringstream ss;

	Token::TokenType mode = Token::None;
	bool hasNewLine = false;

	while (stream){
		char c = stream.get();
		switch (mode){
		case Token::None:
		{
			if (isspace(c)){
				if (c == '\n') {
					hasNewLine = true;
				}
				ss.put(c);
				mode = Token::Space;
				continue;
			}
			else if (isalpha(c) or c == '_'){
				ss.put(c);
				mode = Token::Word;
				continue;
			}
			else if (isdigit(c) || c == '.'){
				ss.put(c);
				mode = Token::Digit;
				continue;
			}
			else if(c == '"'){
				mode = Token::String;
				ss.put(c);
				continue;
			}
			else if(c == '#'){
				mode = Token::PreprocessorCommand;
				ss.put(c);

				char line[500];
				stream.getline(line, 500);
				ss << line;
				auto token = Token(ss.str(), Token::PreprocessorCommand, location);

				if (token.substr(0, 3) == "#1 ") { //Processing rests of includes
					auto f1 = token.find('"');
					auto f2 = token.find('"', f1 + 1);
					if (f1 != string::npos and f2 != string::npos) {
						string filename = token.substr(f1 + 1, f2 - f1 - 1);
						*location = Location(filename, 1, 1);
					}
				}

				return token;
			}
			if (c == '/'){
				//possible comment
				if (stream.peek() == '/' or stream.peek() == '*'){
					mode = Token::Space;
					stream.unget();
					continue;
				}
			}
			if (c == '\''){
				c = stream.get();
				if (c == '\''){
					return Token("", Token::Char, location);
				}
				if (stream.get() == '\''){
					char tmp[2] = {c, 0};
					return Token(tmp, Token::Char, location);
				}
			}
			else if(specialCharacters.find(c) != string::npos && stream){
				mode = Token::SpacedOutCharacter;
				ss.put(c);
				continue;
//				return Token(ss.str(), Token::SpacedOutCharacter);
			}
		}
		break;
		case Token::Space:
			while ((isspace(c) or c == '/') && stream){
				if (c == '/'){ //Ignore comments
					if (stream.peek() == '/'){
						while (c != '\n' and stream){
							ss.put(c);
							c = stream.get();
						}
					}
					else if (stream.peek() == '*'){ //c-style comment
						while (!(c == '*' and stream.peek() == '/') and stream){
							ss.put(c);
							c = stream.get();
						}
						ss.put(c);
						c = stream.get();
					}
					else{
						return Token("/", Token::SpacedOutCharacter, location);
					}
				}
				else if (c == '\n') {
					hasNewLine = true;
				}

				ss.put(c);
				c = stream.get();
			}
			stream.unget();
			return Token(ss.str(), Token::Space, location, hasNewLine);

		case Token::Word:
		{
			while ((isalnum(c) or c == '_') && stream){
				ss.put(c);
				c = stream.get();
			}
			stream.unget();
			auto tokenString = ss.str();
			bool isKeyword = keyWords.isKeyWord(tokenString);
			return Token(tokenString, isKeyword? Token::KeyWord : Token::Word, location);
		}

		case Token::Digit:
		{
			while ((isdigit(c) || c == '.') && stream){
				ss.put(c);
				c = stream.get();
			}
			stream.unget();

			auto token = Token(ss.str(), Token::Digit, location);
			if (token == ".") {
				token.type = Token::OperatorOrPunctuator;
			}
			return token;
		}

		case Token::String:
			while (c != '"' && stream){
				ss.put(c);
				c = stream.get();
				if (c == '\\'){
					ss.put(c);
					c = stream.get();
					ss.put(c);
					c = stream.get();
				}
			}
			ss.put('"');

			return Token(ss.str(), Token::String, location);
		case Token::SpacedOutCharacter:
			std::string tmp = ss.str();
			bool isOperatorIsh = getPreprocessOperorOrSubString(tmp) != 0;
			ss.put(c);
			tmp = ss.str(); //Todo: Optimize this
			while ((getPreprocessOperorOrSubString(tmp)) && stream){
				c = stream.get();
				ss.put(c);
				tmp = ss.str();
				isOperatorIsh = true;
			}
			stream.unget();
			return Token(tmp.substr(0, tmp.size() -1),
					isOperatorIsh? Token::OperatorOrPunctuator: Token::SpacedOutCharacter, location);

			break;
		}
	}

	return Token(ss.str(), Token::None, location);
}

std::list<Token> Tokenizer::splitStringToToken(Token token) {
	std::list<Token> ret;
	Tokenizer tokenizer(token, token.location);
	while (tokenizer) {
		auto token = tokenizer.getNextToken();
		ret.push_back(token);
	}
	return ret;
}

void Token::printLocation(std::ostream* stream) {
	if (!stream) {
		stream = &cout;
	}
	*stream << "fil xxx:" << "xx:xx" << endl; //Todo implemente location markers for tokens
}

std::list<Token> Tokenizer::splitToToken() {
	cout << "splitting to tokens" << endl;
	std::list<Token> ret;

	while(*this) {
		auto token = getNextToken();
		ret.push_back(token);
	}

	return ret;
}
