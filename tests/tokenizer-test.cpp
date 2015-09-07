/*
 * tokenizer-test.cpp
 *
 *  Created on: 6 sep 2015
 *      Author: Mattias Larsson Sköld
 */


#include "unittest.h"
#include "tokenizer.h"

using namespace std;


TEST_SUIT_BEGIN

TEST_CASE("token create") {
	Token token("hej");

	Token shmoken = token;
}

TEST_CASE("split to tokens") {
	string code = R"raw(

testing ( tokens and ) stuff
)raw";

	auto result = Tokenizer::splitStringToToken(code);

	for (auto it: result) {
		cout << it << endl;
	}
}

TEST_SUIT_END
