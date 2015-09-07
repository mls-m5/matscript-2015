/*
 * sourcetree-test.cpp
 *
 *  Created on: 30 aug 2015
 *      Author: Mattias Larsson Sköld
 */


#include "unittest.h"
#include "sourcetree.h"
#include <sstream>
using namespace std;

TEST_SUIT_BEGIN

TEST_CASE("sourcetree basic test") {
	string apa = R"tag(
int main() {
   //Comment
}

)tag";
	Tokenizer tokenizer(apa);
	SourceTree st(tokenizer);

	st.print();
}

TEST_CASE("blockify") {
	string code = R"raw(
function apa (int argA, int argB) {

   def apa[2] = {1, 2};
}

def afterApa = 1;

)raw";

	Tokenizer tokenizer(code);
	SourceTree st(tokenizer);

	st.print();

	//Test case here...

}


TEST_CASE("binarise") {
	string code = R"raw(
def i = 1 * 3 + 3 * 4 ^ 5, j = 2 + 3 * 3
)raw";

	Tokenizer tokenizer(code);
	SourceTree st(tokenizer);

	st.print();

	//Test case here...

}


TEST_CASE("source to source") {
	//First simple test

	{
		Tokenizer tokenizer(" hej ");
		SourceTree st(tokenizer);

		ASSERT_EQ(st.toToken(), " hej ");
	}


	{
		string code = R"raw(
   def apa = 1 + 1
   def bepa = apa 
)raw";

		Tokenizer tokenizer(code);
		SourceTree st(tokenizer);

		auto returnCode = st.toToken();

		ASSERT_EQ(code, returnCode);
	}
}


TEST_SUIT_END


