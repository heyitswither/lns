#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;
int main(int argc, char const *argv[])
{
	auto start = std::chrono::high_resolution_clock::now();
	int i = 0;
	int max = 100000;
	for(i = 0; i < max; i++){
		//cout << i << endl;
	}
	cout << "Done in " << (double)duration_cast<microseconds>(std::chrono::high_resolution_clock::now() - start).count() << " milliseconds." << endl;
	return 0;
}