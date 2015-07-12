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
#define BUSY 5
#define COLLISION 6


#define ONE_PERSISTENT 1
//#define NON_PERSISTENT 1
//#define PRB_PERSISTENT 1 


struct Node {
    int m_id;
    int m_state;
    int m_nextState;
    int m_time;
    int m_genTime;
    int m_transTime;
    // 
    int m_waitTime;
};

struct Packet {
    int m_node_id;
    int m_generated_time;
    int m_i;
};

int busy_counter = 0;
int successfulPackets = 0;
int T = 10;
int ticks = 1000000;

struct Generator {
    double lambda;

    Generator() {}
    Generator(double lambda): lambda(lambda) {}
    int generateExpRandomNum() {
        return (int)(((-1 / lambda) * log(1 - ((double)rand() / RAND_MAX))) * ticks);
    }
};

std::vector<Node*> nodes;
std::vector<Packet*> packets;

Generator generator;

bool hasPacketInQueue (int a_id) {
    for (int i = 0; i < packets.size(); i++) {
        if (packets[i]->m_node_id == a_id) {
            return true;
        }
    }
    return false;
}

int medium_Sense () {
    int busy_counter = 0;
    
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i]->m_time < nodes[i]->m_transTime) {
            busy_counter++;
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


void updateState (struct Node* a_node) {
    
    // TODO: initialize nextState to current state when program first runs
    a_node -> m_nextState = a_node -> m_state;

    if (a_node->m_time == a_node->m_genTime) {
        Packet* packet = new Packet[sizeof(Packet)];
        packet->m_node_id = a_node->m_id;
    }

    switch (a_node->m_state) {
        case IDLE:
            if (hasPacketInQueue(a_node->m_id)) {
                a_node->m_nextState = SENSING;
            }
            break;
        case SENSING:
        #ifdef ONE_PERSISTENT
            if (medium_Sense() != IDLE) {
                a_node->m_time = 0;
            } else if (a_node->m_time == 96) {
                a_node->m_time = 0;
                a_node->m_state = TRANSMIT;
            }
        #elif NON_PERSISTENT
            
        
        #elif PRB_PERSISTENT
        
        #endif
            break;
        case TRANSMIT:
            // TODO calculate transmission time
            if (medium_Sense() != IDLE) {
                a_node->m_state = JAMMING;
                a_node->m_time = 0;
            } else {
                successfulPackets++;
                a_node->m_state = IDLE;
            }
            break;
        case JAMMING:
           	if (a_node->m_time == 48) {
                a_node->m_state = BACKOFF;
            }
            break;
        case BACKOFF:
            // Ricky Sucks
            
            //^ second that
            // TODO back off for R*512 bit time
            int R = rand() % pow(2,i);
            
            a_node->m_waitTime = R * 512;
            a_node -> m_nextState = SENSING;
            
            
            
            break;
        default:
            break;
    }
}

void Start_simulation (int a_totalticks,int a_N,int a_A,int a_W,int a_L) {
    for (int i = 0; i <= a_totalticks; i++) {
        for (int n = 0; n <= a_N; n++) { // loop for number of nodes
            if (nodes[n]->m_waitTime == 0) { // update state only when wait counter is 0.
                updateState(nodes[n]);
            } else { // else decrement wait counter
                nodes[n]->m_waitTime--;
            }
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
        node->m_waitTime = 0;
        nodes.push_back(node);
    }

    Start_simulation (T*ticks,N,A,W,L);
    Compute_performances();
    
    for (int i = 0; i < N; i++)
        delete nodes[i];
}