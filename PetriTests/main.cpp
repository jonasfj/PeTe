#include <UnitTest++.h>

int main(int, char**){
	return UnitTest::RunAllTests();
}

SUITE(TestTestSuite){

	TEST(TestGoodTestTest){
		CHECK(true);
	}
}
