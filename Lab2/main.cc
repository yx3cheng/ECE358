#include <cmath>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

#ifdef DEBUG
  #define debug_out cout << current_tick << '\t' << a_node->m_id << '\t'
#else
  #define debug_out 0 && cout
#endif

enum station_states { IDLE, SENSING, TRANSMIT, JAMMING, BACKOFF };
enum medium_states { FREE = 0x1, BUSY = 0x2, COLLISION = 0x4 };

struct Packet {
  long m_generated_time;
  int m_i;

  Packet(long generatedTime) : m_generated_time(generatedTime), m_i(0) {}
};

struct Node {
  int m_id;
  station_states m_state;
  long m_time;
  long m_generate_at_tick;
  long m_tick_at_start_transmission;

  int m_sense_result_mask;
  long m_time_to_backoff;

  queue<Packet*> m_packet_queue;
};

struct Generator {
  double lambda;

  Generator() {
    srand(time(NULL));
  }
  double generateExpRandomNum() {
    double U = ((double) (rand() % 1000000000) / 1000000000);
    return ((-1.0 / lambda) * log(1 - U));
  }
} generator;

int T = 100000; // Number of seconds to simulate.
long ticksPerSecond = 100000000; // Resolution of simulation
vector<Node*> nodes;

medium_states medium_sense(struct Node* a_node, long current_tick) {
  int busy_counter = 0;

  for (int i = 0; i < nodes.size(); i++) {
    if (nodes[i]->m_state == JAMMING)
      return COLLISION;

    if (nodes[i]->m_state != TRANSMIT)
      continue;

    if (a_node->m_id == nodes[i]->m_id) {
      busy_counter++;
    } else {
      int distance = abs(nodes[i]->m_id - a_node->m_id) * 10;
      double prop_delay = (double) ticksPerSecond * distance / 200000000; // 2 * 10^8

      if (prop_delay < current_tick - nodes[i]->m_tick_at_start_transmission) {
        // Current node sees node i's transmission since it has been transmitting
        // longer than the propagation delay.
        busy_counter++;
      }
    }
  }

  if (busy_counter == 0) {
    return FREE;
  } else if (busy_counter == 1) {
    return BUSY;
  } else {
    return COLLISION;
  }
}

void tick(struct Node* a_node, long current_tick, int a_W, int a_L) {
  a_node->m_time++;

  switch (a_node->m_state) {
    case IDLE:
      if (!a_node->m_packet_queue.empty()) {
        debug_out << "start sensing" << endl;
        a_node->m_state = SENSING;
        a_node->m_time = 0;
      }
      break;

    case SENSING:
      a_node->m_sense_result_mask |= medium_sense(a_node, current_tick);

#ifdef ONE_PERSISTENT
      if (a_node->m_time == ticksPerSecond * 96.0 / a_W) {
        a_node->m_time = 0;
        if (a_node->m_sense_result_mask == FREE) {
          a_node->m_state = TRANSMIT;
          a_node->m_tick_at_start_transmission = current_tick;
          debug_out << "transmitting" << endl;
        } else {
          debug_out << "redo sensing" << endl;
          a_node->m_sense_result_mask = 0;
        }
      }
#elif NON_PERSISTENT
#elif PRB_PERSISTENT

#endif

      break;

    case TRANSMIT:
      if (medium_sense(a_node, current_tick) == COLLISION) {
        a_node->m_state = JAMMING;
        a_node->m_time = 0;
        debug_out << "jamming" << endl;
      } else if (a_node->m_time >= (double) ticksPerSecond * a_L / a_W)  {
        debug_out << "end transmission" << endl;
        a_node->m_state = IDLE;
        a_node->m_time = 0;
        a_node->m_packet_queue.pop();
      }
      break;

    case JAMMING:
      if (a_node->m_time == ticksPerSecond * 48.0 / a_W) {
        a_node->m_state = BACKOFF;
        a_node->m_time = 0;
        debug_out << "finished jamming" << endl;
      }
      break;

    case BACKOFF:
      if (a_node->m_time_to_backoff == 0) {
        a_node->m_packet_queue.front()->m_i++;
        a_node->m_time_to_backoff =
            rand() % (int) pow(2, a_node->m_packet_queue.front()->m_i)
              * ticksPerSecond * 512.0 / a_W;
        debug_out << "backoff for " << a_node->m_time_to_backoff << " ticks" << endl;
      }

      if (a_node->m_packet_queue.front()->m_i > 10) {
        // Error. Give up on this frame.
        a_node->m_state = IDLE;
        a_node->m_time = 0;
        a_node->m_packet_queue.pop();
      } else if (a_node->m_time == a_node->m_time_to_backoff) {
        debug_out << "backoff done start sensing" << endl;
        a_node->m_state = SENSING;
        a_node->m_time = 0;
      }
      break;

    default:
      break;
  }
}

void start_simulation (long a_totalticks, int a_N, int a_A, int a_W, int a_L) {
  for (long current_tick = 0; current_tick < a_totalticks; current_tick++) {
    for (int n = 0; n < a_N; n++) { // loop for number of nodes

      if (nodes[n]->m_generate_at_tick == current_tick) { // generate new packet
        nodes[n]->m_packet_queue.push(new Packet(current_tick));
        nodes[n]->m_generate_at_tick = current_tick + generator.generateExpRandomNum() * ticksPerSecond;
      }

      tick(nodes[n], current_tick, a_W, a_L);
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
    P = atoi(argv[5]); // Persistence parameter

  // set generator lamda
  generator.lambda = A;

  // nodes
  for (int i = 0; i < N; i++) {
    Node* node = new Node();
    node->m_id = i;
    node->m_state = IDLE;
    node->m_generate_at_tick = generator.generateExpRandomNum() * ticksPerSecond;
    node->m_time_to_backoff = 0;
    nodes.push_back(node);
  }

  start_simulation(T * ticksPerSecond, N, A, W, L);
  // compute_performances();

  for (int i = 0; i < N; i++)
    delete nodes[i];
}
