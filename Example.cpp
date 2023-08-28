#include <iostream>
#include <string>
#include <cassert>
#include "XorValue.hpp"
#include "XorConstant.hpp"
#include "XorString.hpp"

class TestObject
{
public:
	TestObject(std::string name) : m_name(name) { }

	const std::string name() { return m_name; }
private:
	std::string m_name = "";
};

int main()
{
	// test byte string
	auto protectedString = MakeXorString("hello, world! i am encrypted");
	printf("protectedString.Decrypt(): %s\n", protectedString.GetCrypt().c_str());

	// test wide string
	auto protectedWString = MakeXorString(L"hello, world! i am encrypted");
	printf("protectedWString.Decrypt(): %ls\n", protectedWString.GetCrypt().c_str());

	// test constant u64
	auto protectedConstantU64 = MakeXorConstant(0xDEADBEEFDEADBEEF);
	printf("protectedConstantU64.GetCrypt(): %llx\n", protectedConstantU64.GetCrypt());
	assert(protectedConstantU64.GetCrypt() == 0xDEADBEEFDEADBEEF);

	// test constant float
	auto protectedConstantFloat = MakeXorConstant(5.f);
	printf("protectedConstantFloat.GetCrypt(): %f\n", protectedConstantFloat.GetCrypt());
	assert(protectedConstantFloat.GetCrypt() == 5.f);

	// test variable pointer
	auto protectedPointer = MakeXorVariable(new TestObject("protected"));
	printf("protectedPointer: %p (name: %s)\n", protectedPointer.GetCrypt(), protectedPointer->name().c_str());
	assert(protectedPointer->name() == "protected");

	//// test variable float
	auto protectedFloat = MakeXorVariable(2.f);
	printf("protectedFloat: %f\n", protectedFloat.GetCrypt());
	protectedFloat = 10.f;
	assert(protectedFloat.GetCrypt() == 10.f);

	(void)std::getchar();
	return 0;
}