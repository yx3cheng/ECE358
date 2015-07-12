#include <cmath>
#include <cstdlib>
#include <iostream>
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

struct Packet {
  int m_generated_time;
  int m_i;

  Packet(int generatedTime) : m_generated_time(generatedTime), m_i(0) {}
};

struct Node {
  int m_id;
  int m_state;
  int m_nextState;
  int m_time;
  long long m_genTime;
  int m_transTime;

  queue<Packet*> m_packet_queue;
};

int busy_counter = 0;
int successfulPackets = 0;
int T = 10;
int ticksPerSecond = 1000000;
static std::vector<Node*> nodes;

struct Generator {
  double lambda;

  Generator() {
    srand(time(NULL));
  }
  double generateExpRandomNum() {
    double U = ((double) (rand() % 1000) / 1000);
    return ((-1 / lambda) * log(1 - U));
  }
} generator;

int medium_sense(struct Node* a_node) {
  int busy_counter = 0;

  int transmitting_id;
  for (int i = 0; i < nodes.size(); i++) {
    if (nodes[i]->m_state == TRANSMIT) {
      if (a_node->m_id != nodes[i]->m_id)
        transmitting_id = i;
      busy_counter++;
    }
  }
  // Calculate wire distance between transmitting station and current station.
  int distance = abs(transmitting_id - a_node->m_id) * 10;

  if (busy_counter == 0) {
    return IDLE;
  } else if (busy_counter == 1) {
    return BUSY;
  } else {
    return COLLISION;
  }
}

void tick(struct Node* a_node) {
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
        if (medium_sense(a_node) != IDLE) {
          a_node->m_state = TRANSMIT;
          a_node->m_transTime = 0; // ???
        }
      }
#elif NON_PERSISTENT
      if (a_node->m_time == 96 * ticksPerSecond) {
        a_node->m_time = 0;
        if (medium_sense(a_node) != IDLE) {
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
      if (medium_sense(a_node) == COLLISION) {
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
      if (a_node->m_time
            == rand() % (int) pow(2, a_node->m_packet_queue.front()->m_i) * 512 * ticksPerSecond) {
        a_node->m_time = 0;
        a_node->m_state = SENSING;
      }
      break;

    default:
      break;
  }
}

void start_simulation (int a_totalticks,int a_N,int a_A,int a_W,int a_L) {
  for (int current_tick = 0; current_tick <= a_totalticks; current_tick++) {
    for (int n = 0; n < a_N; n++) { // loop for number of nodes

      if (nodes[n]->m_genTime == current_tick) { // generate new packet
        nodes[n]->m_packet_queue.push(new Packet(current_tick));
        nodes[n]->m_genTime = current_tick + generator.generateExpRandomNum() * ticksPerSecond;
      }

      tick(nodes[n]);
    }
  }
}

void compute_performances () {
  // double throughput = successfulPackets / T;
}

int main(int argc, char** argv) {
  int N, A, W, L, P = 0;
  if (argc != 5 && argc != 6) {
    cout << "Usage: N A W L [P]\n";
    return 0;
  }
  N = atoi(argv[1]); // Number of nodes
  A = atoi(argv[2]); // Arrival rate
  W = atoi(argv[3]); // LAN throughput in bits per second
  L = atoi(argv[4]); // Packet length in bits
  if (argc == 6)
    P = atoi(argv[5]); // persistence parameter

  // set generator lamda
  generator.lambda = A;

  // nodes
  for (int i = 0; i < N; i++) {
    Node* node = new Node();
    node->m_id = i;
    node->m_state = IDLE;
    node->m_genTime = generator.generateExpRandomNum() * ticksPerSecond;
    nodes.push_back(node);
  }

  start_simulation(T * ticksPerSecond, N, A, W, L);
  // compute_performances();

  for (int i = 0; i < N; i++)
    delete nodes[i];
}
