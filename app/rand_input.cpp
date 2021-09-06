#include <iostream>
#include <chrono>
#include <set>
#include <vector>
#include <random>

int main(){
	unsigned int count, seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 g(seed);
	std::cin >> count;
	std::set<unsigned int> s;
	std::vector<unsigned int> v;
	while(s.size() < 3 * count){
		auto x = g();
		if(!s.count(x)){
			s.insert(x);
			v.push_back(x);
		}
	}
	s.clear();
	int i = 0;
	for(auto n: v) {
		std::cout << n * (long double) 1e-8L;
		if(++i % 3 == 0) std::cout << std::endl;
		else std::cout << " ";
	}
	return 0;
}
