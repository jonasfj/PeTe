#include <UnitTest++.h>

int main(int, char**){
	return UnitTest::RunAllTests();
}

SUITE(TestTestSuite){
	TEST(TestBadTestTest){
		CHECK(false);
	}
	TEST(TestGoodTestTest){
		CHECK(true);
	}
}
