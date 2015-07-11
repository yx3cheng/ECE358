#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <vector>
#include <queue>

using namespace std;
#define IDLE 0
#define SENSING 1
#define TRANSMIT 2
#define JAMMING 3
#define BACKOFF 4
#define BUSY 5
#define COLLISION 6

#define ONE_PERSISTENT 1
//#define NON_PERSISTENT 1
//#define PRB_PERSISTENT 1

struct Packet {
    int m_generated_time;
    int m_i;


    Packet(int a) : m_generated_time(a) {}
};

struct Node {
    int m_id;
    int m_state;
    int m_nextState;
    int m_time;
    int m_genTime;
    int m_transTime;

    queue<Packet*> m_packet_queue;
};


int busy_counter = 0;
int successfulPackets = 0;
int T = 10;
int ticksPerSecond = 1000000;

struct Generator {
    double lambda;

    Generator() {}
    Generator(double lambda): lambda(lambda) {}
    int generateExpRandomNum() {
        return (int)(((-1 / lambda) * log(1 - ((double)rand() / RAND_MAX))) * ticksPerSecond);
    }
};

std::vector<Node*> nodes;

Generator generator;

int medium_Sense (struct Node* a_node) {
    int busy_counter = 0;

    int transmitting_id;
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i]->m_state == TRANSMIT) {
            transmitting_id = i;
        }
    }
    if (busy_counter == 0) {
        return IDLE;
    } else if (busy_counter == 1) {
        return BUSY;
    } else {
        return COLLISION;
    }
}


void updateSimulation (struct Node* a_node) {
    a_node->m_time++;

    switch (a_node->m_state) {
        case IDLE:
            if (!a_node->m_packet_queue.empty()) {
                a_node->m_nextState = SENSING;
            }
            break;
        case SENSING:
        #ifdef ONE_PERSISTENT
            if (a_node->m_time == 96 * ticksPerSecond) {
                a_node->m_time = 0;
                if (medium_Sense(a_node) != IDLE) {
                	a_node->m_state = TRANSMIT;
                	a_node->m_transTime =
                }
            }
        #elif NON_PERSISTENT
            if (a_node->m_time == 96 * ticksPerSecond) {
                a_node->m_time = 0;
                if (medium_Sense(a_node) != IDLE) {
                	a_node->m_state = TRANSMIT;
                } else {
                	// wait randomly and sense again.
                }
            }
        #elif PRB_PERSISTENT

        #endif
            break;
        case TRANSMIT:
            // TODO calculate transmission time
            if (medium_Sense() == COLLISION) {
                a_node->m_state = JAMMING;
                a_node->m_time = 0;
            } else {
                successfulPackets++;
                a_node->m_state = IDLE;
            }
            break;
        case JAMMING:
            if (a_node->m_time == 48 * ticksPerSecond) {
                a_node->m_state = BACKOFF;
                a_node->m_time = 0;
            }
            break;
        case BACKOFF:
            if (a_node->m_time == (rand() % pow(2,i)) * 512 * ticksPerSecond) {
                a_node->m_time = 0;
                a_node->m_state = SENSING;
            }
            break;
        default:
            break;
    }
}

void Start_simulation (int a_totalticks,int a_N,int a_A,int a_W,int a_L) {
    for (int current_tick = 0; current_tick <= a_totalticks; current_tick++) {
        for (int n = 0; n <= a_N; n++) { // loop for number of nodes

            if (nodes[n]->m_genTime == current_tick) { // generate new packet
                nodes[n]->m_packet_queue.push(new Packet(current_tick));
                nodes[n]->m_genTime = current_tick + generator.generateExpRandomNum();
            }

            updateSimulation(nodes[n]);
        }
    }
}

void Compute_performances () {
    double throughput = successfulPackets / T;
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
        node->m_genTime = generator.generateExpRandomNum();
        nodes.push_back(node);
    }

    Start_simulation (T*ticksPerSecond,N,A,W,L);
    Compute_performances();

    for (int i = 0; i < N; i++)
        delete nodes[i];
}