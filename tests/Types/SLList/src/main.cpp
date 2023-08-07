#include <iostream>
#include <cstdlib>
#include <random>
#include <string>

#include <cm3d/Types/String.hpp>
#include <cm3d/Types/SLList.hpp>

using namespace cm3d;

int main(int argc, char **argv)
{
	SLList<String> lst;

	while (true) {
		std::string tp;
		std::cout << "> ";
		std::cin >> tp;
		if (tp == "print" || tp == "p") {
			std::cout << "[";
			for (auto &el: lst)
				std::cout << el.c_str() << ", ";
			std::cout << "]\n";
		}
		else if (tp == "b") {
			char k[256];
			std::cin >> k;
			String kk(k);
			lst.pushBack(kk);
		}
		else if (tp == "f") {
			char k[256];
			std::cin >> k;
			String kk(k);
			lst.pushFront(kk);
		}
		else if (tp == "ins" || tp == "i") {
			unsigned ind;
			std::cin >> ind;
			char k[256];
			std::cin >> k;
			String kk(k);
			lst.insert(ind, kk);
		}
		else if (tp == "del") {
			unsigned ind;
			std::cin >> ind;
			lst.erase(ind);
		}
		else if (tp == "pb") {
			lst.popBack();
		}
		else if (tp == "pf") {
			lst.popFront();
		}
		else std::cout << "Invalid command.\n";
	}
}
