#include <vector>
#include <iostream>
#include <chrono>

unsigned int gen = 23232356;
unsigned int genRandInt() {
	gen ^= 0xaaaaaaaa;
	gen *= 777;
	gen /= 3;
	gen ^= 0xaaaaaaaa;
	gen *= 128;
	gen /= 7;
	gen ^= 0xaaaaaaaa;
	return gen;
}

float genRandFloat() {
	int i = genRandInt();
	i &= 0b00000000011111111111111111111111; // delete sign & exponent
	i ^= 0b00111111100000000000000000000000; // add exponent
	// now i is uniformly distributed [1.0; 2.0)

	return *(float*)&i - 1.f;
}

int main() {
	std::vector<float> arr;

	using namespace std::chrono;

	auto pusht1 = high_resolution_clock::now();
	for (int i = 0; i < 10000000; ++i) {
		arr.push_back(genRandFloat());
	}
	auto pusht2 = high_resolution_clock::now();

	std::cout << "-- len: " << arr.size() << '\n';
	std::cout << "-- capacity: " << arr.capacity() << '\n';
	
	std::cout << "-- random selection:\n";
	for (int i = 0; i < 8; ++i) {
		std::cout << "  " << arr[genRandInt() % arr.size()] << '\n';
	}

	auto popt1 = high_resolution_clock::now();
	for (int i = 0; i < 10000000; ++i) {
		arr.pop_back();
	}
	auto popt2 = high_resolution_clock::now();
	std::cout << "-- len: " << arr.size() << '\n';
	std::cout << "-- capacity: " << arr.capacity() << '\n';

	duration<double> push_span = duration_cast<duration<double>>(pusht2 - pusht1);
	duration<double> pop_span = duration_cast<duration<double>>(popt2 - popt1);

	std::cout << "push: " << push_span.count() << "s.\n";
	std::cout << "pop: " << pop_span.count() << "s.\n";
}