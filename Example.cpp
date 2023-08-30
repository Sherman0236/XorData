#include <iostream>
#include <string>
#include <cassert>
#include "XorVariable.hpp"
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

	// test constant integer
#ifdef _WIN64
	auto protectedConstantU64 = MakeXorConstant(0xDEADBEEFDEADBEEF);
	printf("protectedConstantU64.GetCrypt(): %llx\n", protectedConstantU64.GetCrypt());
	assert(protectedConstantU64.GetCrypt() == 0xDEADBEEFDEADBEEF);
#else
	auto protectedConstantU32 = MakeXorConstant(0xDEADBEEF);
	printf("protectedConstantU32.GetCrypt(): %lx\n", protectedConstantU32.GetCrypt());
	assert(protectedConstantU32.GetCrypt() == 0xDEADBEEF);
#endif

	// test constant float
	auto protectedConstantFloat = MakeXorConstant(5.f);
	printf("protectedConstantFloat.GetCrypt(): %f\n", protectedConstantFloat.GetCrypt());
	assert(protectedConstantFloat.GetCrypt() == 5.f);

	// test variable pointer
	auto protectedPointer = MakeXorVariable(new TestObject("protected"));
	printf("protectedPointer: %p (name: %s)\n", protectedPointer.GetCrypt(), protectedPointer->name().c_str());
	assert(protectedPointer->name() == "protected");

	// test variable int
	int intValue = rand();
	auto protectedInt = MakeXorVariable(intValue);
	printf("protectedInt: %i\n", protectedInt.GetCrypt());
	assert(protectedInt.GetCrypt() == intValue);

	// test variable float
	const float floatValue = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	auto protectedFloat = MakeXorVariable(floatValue);
	printf("protectedFloat: %f\n", protectedFloat.GetCrypt());
	assert(protectedFloat.GetCrypt() == floatValue);

	(void)std::getchar();
	return 0;
}