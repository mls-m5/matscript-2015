/*
 * tokenizer.h
 *
 *  Created on: 13 okt 2014
 *      Author: Mattias Larsson Sköld
 */

#pragma once
#include <iostream>
#include <sstream>
#include <list>
#include <string>

//A pointer to a file
//Todo: make one instance of this per file and pointers that references to it
class FilePointer {
public:
	FilePointer(std::string fname):
		_filename(fname) {

	}

	std::string getFilename() {
		return _filename;
	}

protected:
	std::string _filename;
};

class Location {
public:
	Location(std::string filename = "<commandline>", int line = 1, int col = 1):
	line(line),
	 column(col),
	 file(filename){

	}

	void operator += (const std::string &str) {
		for (auto c : str) {
			if (c == '\n') {
				line += 1;
				column = 1;
			} else {
				column += 1;
			}
		}
	}

	void operator -= (char c) {
		if (c == '\n') {
			line -= 1;
			column = 1; //Hard to undo this
		} else {
			column -= 1;
		}
	}

	int line = 1;
	int column = 1;
	FilePointer file;
};

class Token: public std::string{
public:
	enum TokenType {
		Space,
		Word,
		KeyWord,
		Digit,
		String,
		Char,
		StartScope,
		SpacedOutCharacter,
		OperatorOrPunctuator,
		PreprocessorCommand,
		None,
		Last,
	};

	Token () {};
	Token (std::string str): std::string(str) {}

	//Initialize a token with different properties
	//str: The string that consist the token
	//type: the token type
	//filePosition: a pointer to a iterator that keeps track of the position in the file
	Token (std::string str, TokenType type, Location *filePosition, bool hasNewline = false):
		type(type),
		std::string(str),
		hasNewLine(hasNewline){
		if (filePosition) {
			location = *filePosition;
			*filePosition += str;
		}
	}
	void printLocation(std::ostream  *stream = 0);

	std::string getStringContent(){
		if (size() > 2){
			return substr(1, size() - 2);
		}
		else{
			return std::string();
		}
	}

	void operator+= (const Token &token) {
		if (token.hasNewLine) {
			hasNewLine = true;
		}

		std::string::operator+=(token);
	}

	TokenType type = None;
	bool hasNewLine = false;
	Location location;
};

class Tokenizer {
public:
	Tokenizer(const std::string &code, Location location = Location()):
	ss(code),
	lastLocation(location){}
	Tokenizer(std::istream &stream, Location location = Location()):
	otherStream(&stream),
	lastLocation(location){
	}

	inline std::istream &getStream() {
		if (otherStream) {
			return *otherStream;
		}
		else {
			return ss;
		}
	}

	Token getNextToken() {
		return GetNextToken(getStream(), &lastLocation);
	}
	static Token GetNextToken(std::istream& stream, Location *locationIterator);
	Token getNextToken(std::istream& stream) {
		return GetNextToken(stream, &lastLocation);
	}
	static std::list<Token> splitStringToToken(Token);

	std::list<Token> splitToToken();

	operator bool() {
		return bool(getStream());
	}

	void unget() {
		getStream().unget();
		lastLocation -= getStream().peek();
	}

	std::istringstream ss;
	std::istream *otherStream = 0;

	Location lastLocation;
};

