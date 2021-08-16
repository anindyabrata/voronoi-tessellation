#include <iostream>
#include <chrono>
#include <set>
#include <random>

int main(){
	unsigned int count, seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 g(seed);
	std::cin >> count;
	std::set<unsigned int> s;
	while(s.size() < 3 * count) s.insert(g());
	int i = 0;
	for(auto n: s) {
		std::cout << n * (long double) 1e-6L;
		if(++i % 3 == 0) std::cout << std::endl;
		else std::cout << " ";
	}
	return 0;
}
