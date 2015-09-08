/*
 * SourceTree.cpp
 *
 *  Created on: 30 aug 2015
 *      Author: Mattias Larsson Sköld
 */

#include "sourcetree.h"
#include "tokenizer.h"
#include <list>
using namespace std;

vector<string> SourceTreeTypes = {
	"Line",
	"Block",
	"Paranthesis",
	"Brackets",
	"BinaryExpression",
};

SourceTree createSubTree(list<CompositeToken> &l) {
	SourceTree subTree;
	for (auto it: l) {
		subTree.push_back(it);
	}
	return subTree;
}


SourceTree::iterator BlockifySourceTree(SourceTree &st, SourceTree::iterator begin, SourceTree::iterator end, string &beginToken, string &endToken, SourceTree::Type type) {
	for (auto it = begin; it != end; ) {
		if (it->_name == beginToken) {
			auto beginIt = it;
			auto beginToken = beginIt->_name;

			beginIt = st.erase(beginIt);
//			++beginIt;
			auto endIt = BlockifySourceTree(st, beginIt, end, beginToken, endToken, type);
			SourceTree newSt;

			newSt.splice(newSt.end(), st, beginIt, endIt);
			newSt._type = type;
			newSt._name = beginToken;
			if (newSt.back()._name == endToken) {
				newSt._endToken = newSt.back()._name;
				newSt.pop_back();
			}
			st.insert(endIt, newSt);
			it = endIt;
		}
		else if(it->_name == endToken) {
			++it;
			return it;
		}
		else {
			++it;
		}
	}

	return end;
}


SourceTree::SourceTree(Tokenizer &tokenizer)
{
	auto tokens = tokenizer.splitToToken();
	Token leadingSpace;

	for (auto &token: tokens) {
		if (token.type == Token::Space) {
			leadingSpace = token;
			continue;
		}
		CompositeToken t = token;
		if (not leadingSpace.empty()) {
			t._leadingSpace = leadingSpace;
			leadingSpace.clear();
		}
		push_back(SourceTree(t));
	}

	if (not leadingSpace.empty()) {
		CompositeToken t = leadingSpace;
		push_back(SourceTree(t));
	}

	Blockify("{", "}", Block);
	Blockify("(", ")", Paranthesis);
	Blockify("[", "]", Brackets);


	static const vector<string> binaryOperators = {
			",",
			"=",
			"+",
			"-",
			"*",
			"/",
			"^",

	};
	for (auto &operatorTag: binaryOperators) {
		Binarise(operatorTag);
	}
}

SourceTree::~SourceTree() {
}

Token SourceTree::toToken() {
	if (empty()) {
		return _name.toToken();
	}
	Token returnToken;

	if (_type == BinaryExpression) {
		if (size() != 2) {
			cerr << "error: wrong size of binary expression" << endl;
		}
		auto it = begin();
		returnToken += it->toToken();
		returnToken += _name.toToken();
		++it;
		returnToken += it->toToken();
	}
	else { //standard statement
		returnToken = _name.toToken();
		for (auto &it: *this) {
			returnToken += it.toToken();
		}
		if (not _endToken.empty()) {
			returnToken += _endToken.toToken();
		}
	}


	return returnToken;
}

void indent (ostream &stream, int recursionDepth) {
	for (int i = 0; i < recursionDepth; ++i) {
		stream << "  ";
	}
}

void SourceTree::print(int recursionDepth) {
	if (_type == BinaryExpression) {
		if (size() == 2) {
			indent(cout, recursionDepth);
			cout << "binary operator " << _name << ": " << endl;
			auto it = begin();

			it->print(recursionDepth + 1);

			indent(cout, recursionDepth);
			cout << _name << endl;

			++it;
			it->print(recursionDepth + 1);

		}
		else {
			cerr << "wrong size of binary expression" << endl;
		}
	}
	else {
		if (recursionDepth == 0) {
			cout << "#root" << endl;
		}
		indent(cout, recursionDepth);
		cout << "'" << _name << "'             ----(" << SourceTreeTypes[_type] << ")" << endl;
		for (auto &it : *this) {
			it.print(recursionDepth + 1);
		}
		if (not _endToken.empty()) {
			indent(cout, recursionDepth);
			cout << _endToken << " << endTag" << endl;
		}
	}
}

void SourceTree::Blockify(std::string startTag, std::string endTag, Type type) {
	BlockifySourceTree(*this, begin(), end(), startTag, endTag, type);

	for (auto &it: *this) {
		it.Blockify(startTag, endTag, type);
	}

}

void SourceTree::Binarise(const std::string& operatorTag) {
	for (auto it = begin(); it != end(); ++it) {
		if (it->_name == operatorTag) {
			if (it == begin()) {
				continue;
			}
			SourceTree binaryExpression;
			binaryExpression._type = BinaryExpression;
			binaryExpression._name = it->_name;
			SourceTree stA;
			SourceTree stB;
			stA.splice(stA.begin(), *this, begin(), it);
			it = erase(it);
			if (it == end()) {
				cerr << "binary expression in end of expression when searching for " << operatorTag << ", expecting another expression" << endl;
				return;
			}
			stB.splice(stB.begin(), *this, it, end());
			stB.Binarise(operatorTag);

			for (auto &jt: stA) {
				jt.Binarise(operatorTag);
			}

			for (auto &jt: stB) {
				jt.Binarise(operatorTag);
			}

			binaryExpression.push_back(stA);
			binaryExpression.push_back(stB);
			push_back(binaryExpression);
			return;
		}
	}

	for (auto &it: *this) {
		it.Binarise(operatorTag);
	}
}
