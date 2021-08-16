#include <iostream>
#include <random>

int main(){
	unsigned int num;
	std::cin >> num;
	std::default_random_engine eng;
	while(num--) std::cout << eng() << " " << eng() << " " << eng() << std::endl;
	return 0;
}
