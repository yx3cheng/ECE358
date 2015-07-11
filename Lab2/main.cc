#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>

using namespace std;
#define IDLE 0
#define SENSING 1
#define TRANSMIT 2
#define JAMMING 3
#define BACKOFF 4

struct Node {
	int m_id;
	int m_state;
	int m_time;
	int m_genTime;
};

struct Packet {
	int m_node_id;
};

struct Generator {
	double lambda;

	Generator() {}
	Generator(double lambda): lambda(lambda) {}
	int generateExpRandomNum() {
		return (int)((-1 / lambda) * log(1 - rand()));
	}
};


std::vector<Node*> nodes;
std::vector<Packet*> packets;
int busy_counter = 0;
Generator generator;

bool hasPacketInQueue (int a_id) {
	for (int i = 0; i < packets.size(); i++) {
		if (packets[i]->m_node_id == a_id) {
			return true;
		}
	}
	return false;
}

void updateSimulation (struct Node* a_node) {
	a_node->m_time++;
	if (a_node->m_time == a_node->m_genTime) {
		Packet* packet = new Packet[sizeof(Packet)];
		packet->m_node_id = a_node->m_id;
	}

	switch (a_node->m_state) {
		case IDLE:
			if (hasPacketInQueue(a_node->m_id)) {
				a_node->m_state = SENSING;
			}
			break;
		case SENSING:
			// TODO sense for 96 bit-time
			if (busy_counter == 0) {
				a_node->m_state = TRANSMIT;
			} else {
				a_node->m_state = SENSING;
			}
			break;
		case TRANSMIT:
			busy_counter++;
			// TODO calculate transmission time
			if (busy_counter >= 2) {
				a_node->m_state = JAMMING;
			} else {
				a_node->m_state = IDLE;
			}
			break;
		case JAMMING:
			busy_counter++;
			// TODO transmit the jamming signal on the medium for 48 bit-time
			a_node->m_state = BACKOFF;
			break;
		case BACKOFF:
			// TODO back off for R*512 bit time
			a_node->m_state = SENSING;
			break;
		default:
			break;
	}
}

void Start_simulation (int a_totalticks,int a_N,int a_A,int a_W,int a_L) {
	for (int i = 0; i <= a_totalticks; i++) {
		for (int n = 0; n <= a_N; n++) {
			updateSimulation(nodes[n]);
		}
	}
}

void Compute_performances () {

}

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

	// set generator lamda
	generator.lambda = A;

	// nodes
	for (int i = 0; i < N; i++) {
		Node* node = new Node[sizeof(Node)];
		node->m_id = i;
		node->m_state = IDLE;
		node->m_time = 0;
		node->m_genTime = generator.generateExpRandomNum();
		nodes.push_back(node);
	}

	int T = 10;
	int ticks = 1000000;
	Start_simulation (T*ticks,N,A,W,L);
	Compute_performances();
}

