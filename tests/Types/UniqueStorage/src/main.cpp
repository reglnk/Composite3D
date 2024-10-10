#include <iostream>
#include <cstdlib>
#include <random>
#include <string>

#include <cm3d/Tl/String.hpp>
#include <cm3d/Tl/UniqueStorage.hpp>

using namespace cm3d;

void expect(bool v)
{
	if (!v)
		throw "Test failed";
}

int main(int argc, char **argv)
{
	UniqueStorage<String> us;
	expect(us.push("first") == 0);
	expect(us.push("second") == 1);
	expect(us.push("third") == 2);
	
	us.erase(1);
	auto save = us;
	
	us.clear();
	expect(us.length() == 0 && us.size() == 0);
	
	us = std::move(save);
	expect(us.length() == 2 && us.size() == 3);
	
	expect(us.push("4th") == 1);
	expect(us.length() == 3 && us.size() == 3);
	expect(us.push("5th") == 3);
	expect(us.length() == 4 && us.size() == 4);
	
	for (int i = 0; i < 100000; ++i)
		expect(us.push("somestr") == 4 + i);
	
	expect(us.length() == 100004 && us.size() == 100004);
	
	for (int i = 100; i < 1000; ++i)
		us.erase(i);
	
	expect(us.length() == 99104 && us.size() == 100004);
	expect(us[1000] == "somestr");
	expect(us[100003] == "somestr");
	
	std::cout << "OK" << std::endl;
}
