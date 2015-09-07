/*
 * SourceTree.h
 *
 *  Created on: 30 aug 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once
#include "tokenizer.h"
#include <vector>
#include <memory>

class CompositeToken: public Token {
public:
	CompositeToken() {

	}
	CompositeToken(const CompositeToken &token) {
		*(Token*)this = token;
		hasSpace= token.hasSpace;
		_leadingSpace = token._leadingSpace;
	}
	CompositeToken(const Token &token) {
		*(Token*)this = token;
	}

	CompositeToken(std::string str) {
		*(Token*)this = str;
	}

	Token toToken() {
		return _leadingSpace + *(Token *)this;
	}

	Token _leadingSpace;
	bool hasSpace = false;

	enum Type{
		Line = Token::Last
	};
};

class SourceTree: public std::list<SourceTree>{
public:
	enum Type {
		Line,
		Block,
		Paranthesis,
		Brackets,
		BinaryExpression,
	};


	SourceTree() {}
	SourceTree(const SourceTree &) = default;
	SourceTree(CompositeToken &token):
	_name(token){
	}
	SourceTree(Tokenizer&);
	virtual ~SourceTree();

	Token toToken();
	void print(int recursionDepth = 0);

	void Blockify(std::string startTag, std::string endTag, Type type);

	void Binarise(const std::string &operatorTag);

public:

	CompositeToken _name;
	CompositeToken _endToken;
	Type _type = Block;
};

