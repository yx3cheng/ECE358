#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace std;

struct Channel {
	bool busy;

	Channel(): busy(false);
};

struct Station {

};

struct Generator {
	double lambda;

	Generator(double lambda): lambda(lambda) {}
	int generateExpRandomNum() {
		return (int)((-1 / lambda) * log(1 - rand()));
	}
};

int main(int argc, char** argv) {
	int N, A, W, L, P = 0;
	if (argc != 5 && argc != 6) {
		cout << "Usage: N A W L [P]\n";
		return 0;
	}
	N = atoi(argv[1]);
	A = atoi(argv[2]);
	W = atoi(argv[3]);
	L = atoi(argv[4]);
	if(argc == 6)
		P = atoi(argv[5]);

}

