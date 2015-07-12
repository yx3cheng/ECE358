#include <cmath>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <vector>
#include <climits>
using namespace std;

#ifdef DEBUG
  #define debug_out cout << current_tick << '\t' << a_node->m_id << '\t'
#else
  #define debug_out 0 && cout
#endif

enum station_states { IDLE, SENSING, SENSING_WAIT, TRANSMIT, JAMMING, BACKOFF };
enum medium_states { FREE = 0x1, BUSY = 0x2, COLLISION = 0x4 };

struct Packet {
  long m_generated_time;
  int m_i;

  Packet(long generatedTime) : m_generated_time(generatedTime), m_i(0) {}
};

struct Node {
  queue<Packet*> m_packet_queue;

  int m_id;
  station_states m_state;
  long m_time;
  long m_generate_at_tick;
  long m_tick_at_start_transmission;

  int m_sense_result_mask;
  long m_time_to_backoff;

  long m_finished_transmit_at_tick;

#if NON_PERSISTENT
  int m_next_sensing_at_tick;
#endif
};

struct Generator {
  double lambda;

  Generator() {
    srand(time(NULL));
  }
  double generateExpRandomNum() {
    double U = ((double) (rand() % INT_MAX) / INT_MAX);
    return ((-1.0 / lambda) * log(1 - U));
  }
} generator;

int T = 2; // Number of seconds to simulate.
long ticksPerSecond = 100000000; // Resolution of simulation.
vector<Node*> nodes;
long transmittedPackets = 0;
long totalDelay = 0;

medium_states medium_sense(struct Node* a_node, long current_tick) {
  int num_broadcasting_nodes = 0;

  for (int i = 0; i < nodes.size(); i++) {
    int distance = abs(nodes[i]->m_id - a_node->m_id) * 10;
    double prop_delay = (double) ticksPerSecond * distance / 200000000; // 2 * 10^8

    if (nodes[i]->m_state == TRANSMIT || nodes[i]->m_state == JAMMING) {
      if (a_node->m_id == nodes[i]->m_id) {
        num_broadcasting_nodes++;
      } else {
        if (prop_delay <= current_tick - nodes[i]->m_tick_at_start_transmission) {
          // Current node sees node i's transmission since it has been transmitting
          // longer than the propagation delay.
          num_broadcasting_nodes++;
        }
      }
    } else {
      if (prop_delay > current_tick - nodes[i]->m_finished_transmit_at_tick) {
        // Even though node i is not transmitting, it was recently transmitting
        // and the current node still senses the medium as busy.
        num_broadcasting_nodes++;
      }
    }
  }

  if (num_broadcasting_nodes == 0) {
    return FREE;
  } else if (num_broadcasting_nodes == 1) {
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
          a_node->m_tick_at_start_transmission = current_tick + 1;
          debug_out << "clear to transmit" << endl;
        } else {
          // debug_out << "redo sensing" << endl;
          a_node->m_sense_result_mask = 0;
        }
      }

#elif NON_PERSISTENT
      if (a_node->m_time == ticksPerSecond * 96.0 / a_W) {
        a_node->m_time = 0;
        if (a_node->m_sense_result_mask == FREE) {
          a_node->m_state = TRANSMIT;
          a_node->m_tick_at_start_transmission = current_tick + 1;
          debug_out << "clear to transmit" << endl;
          a_node->m_packet_queue.front()->m_i = 0;
        } else {
          a_node->m_packet_queue.front()->m_i++;
          if (a_node->m_packet_queue.front()->m_i > 10) {
            debug_out << "error reached sensing trials saturation" << endl;
            a_node->m_state = IDLE;
            a_node->m_time = 0;
            delete a_node->m_packet_queue.front();
            a_node->m_packet_queue.pop();
            break;
          }
          double U = ((double) (rand() % INT_MAX) / INT_MAX);
          a_node->m_next_sensing_at_tick = (ticksPerSecond) *
              U * pow(2, a_node->m_packet_queue.front()->m_i);
          debug_out << "wait for next sensing in "
                    << a_node->m_next_sensing_at_tick
                    << " ticks tries="
                    << a_node->m_packet_queue.front()->m_i << endl;
          a_node->m_sense_result_mask = 0;
          a_node->m_state = SENSING_WAIT;
        }
      }

#elif PRB_PERSISTENT

#endif

      break;

#ifdef NON_PERSISTENT
    case SENSING_WAIT:
      if (a_node->m_time == a_node->m_next_sensing_at_tick) {
        a_node->m_state = SENSING;
        a_node->m_time = 0;
      }
      break;
#endif

    case TRANSMIT:
      if (medium_sense(a_node, current_tick) == COLLISION) {
        a_node->m_state = JAMMING;
        a_node->m_time = 0;
        a_node->m_time_to_backoff = 0;
        debug_out << "jamming" << endl;
      } else if (a_node->m_time == (double) ticksPerSecond * a_L / a_W)  {
        debug_out << "end transmission" << endl;
        a_node->m_state = IDLE;
        a_node->m_time = 0;
        a_node->m_finished_transmit_at_tick = current_tick;
        transmittedPackets++;
        totalDelay += current_tick - a_node->m_packet_queue.front()->m_generated_time;
        delete a_node->m_packet_queue.front();
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
        double U = ((double) (rand() % INT_MAX) / INT_MAX);
        a_node->m_packet_queue.front()->m_i++;
        a_node->m_time_to_backoff = (ticksPerSecond * 512.0 / a_W) *
            U * pow(2, a_node->m_packet_queue.front()->m_i);
        debug_out << "backoff for " << a_node->m_time_to_backoff << " ticks i="
                  << a_node->m_packet_queue.front()->m_i
                  << " R=" << U * pow(2, a_node->m_packet_queue.front()->m_i) << endl;
      }

      if (a_node->m_packet_queue.front()->m_i > 10) {
        // Error. Give up on this frame.
        debug_out << "error reached saturation" << endl;
        a_node->m_state = IDLE;
        a_node->m_time = 0;
        delete a_node->m_packet_queue.front();
        a_node->m_packet_queue.pop();
      } else if (a_node->m_time == a_node->m_time_to_backoff) {
        debug_out << "backoff done start sensing" << endl;
        a_node->m_state = SENSING;
        a_node->m_time = 0;
        a_node->m_time_to_backoff = 0;
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
  double throughput = (double) transmittedPackets / T;
  double averageDelay = ((double) totalDelay / ticksPerSecond) / transmittedPackets;
  cout << "Throughput is " << throughput << " packets per second." << endl;
  cout << "The average delay is " << averageDelay << " seconds." << endl;
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
    node->m_sense_result_mask = 0;
    node->m_time_to_backoff = 0;
    node->m_finished_transmit_at_tick = 0;

    nodes.push_back(node);
  }

  start_simulation(T * ticksPerSecond, N, A, W, L);
  compute_performances();

  for (int i = 0; i < N; i++) {
    while (!nodes[i]->m_packet_queue.empty()) {
      delete nodes[i]->m_packet_queue.front();
      nodes[i]->m_packet_queue.pop();
    }
    delete nodes[i];
  }
}
