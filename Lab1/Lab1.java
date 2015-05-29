import java.util.*;

/*
 REPORT: http://collabedit.com/cqjv4
 Generator: http://collabedit.com/abudg
 Packet: http://collabedit.com/qqnpw
 */
public class Lab1 {
  private static Queue<Packet> buffer;

  // arrival
  private static Generator generator;
  private static int createNextPacketAt = 1;
  // 1 second = 10^6 ticks
  private static final int TICK_TIME = 1000000;

  private static int totalTicks;
  private static int totalPackets = 0;
  private static int totalPacketDelay = 0;
  private static int lambda;
  private static int maxBufferSize = -1;
  // leave at -1 if buffer size is not specified

  private static int packetLength;
  private static int transmitRate;

  private static int serviceTime;

  // Counters
  private static int droppedPacketCount = 0;
  private static int idleServerCount = 0;
  private static int runningBufferSizeCount = 0;

  /*
   * Initialize important terms such as t_arrival = exponential r.v, # of
   * packets in queue = 0, t_departure = t_arrival ( this implies that first
   * time departure will be called as soon as a packet arrives in the queue
   */

  public static void main(String args[]) {

    /*
     * ¦Ë (packets / sec) L (bits) C (bits / sec) p (L¦Ë / c)
     */
    if (args.length < 4 || args.length > 5) {
      System.out.println("Arguments: totalTicks(N) Lambda(¦Ë) packetLength(L) transmissionRate(C) [bufferSize(K)]");
      return;
    }

    totalTicks = Integer.parseInt(args[0]);
    lambda = Integer.parseInt(args[1]);
    packetLength = Integer.parseInt(args[2]);
    transmitRate = Integer.parseInt(args[3]);
    if (args.length == 5) {
      maxBufferSize = Integer.parseInt(args[4]);
    }

    serviceTime = (int) ((double) packetLength / transmitRate * TICK_TIME);
    System.out.println(serviceTime);
    generator = new Generator(lambda);
    buffer = new LinkedList<Packet>();

    start_simulation(totalTicks);
    compute_performances();
  }

  private static void start_simulation(int ticks) {
    for (int t = 1; t <= ticks; t++) {
      arrival(t);
      runningBufferSizeCount += buffer.size();
      departure(t);
    }
  }

  /*
   * Generate a packet as per the exponential distribution and insert the packet
   * in the queue (an array or a linked list)
   */
  private static void arrival(int tick) {
    if (tick < createNextPacketAt) {
      return;
    }

    double x = generator.generateExpRandomNum();
    System.out.println(x);
    int ticksUntilNext = (int) Math.round(x * TICK_TIME);
    createNextPacketAt = tick + ticksUntilNext;

    if (maxBufferSize < 0 || buffer.size() < maxBufferSize) {
      // if buffer is unbounded, or bounded buffer is not full
      buffer.add(new Packet(tick, serviceTime));
    } else {
      // buffer full; drop packet
      droppedPacketCount++;
    }
    totalPackets++;
  }

  /*
   * Check the queue for the packet, if head of the queue is empty, return 0
   * else if the queue is non-empty delete the packet from the queue after an
   * elapse of the deterministic service time.
   */
  private static void departure(int tick) {
    Packet headPacket = buffer.peek();
    if (headPacket == null) {
      // Server is idle.
      idleServerCount++;
      return;
    }

    if (headPacket.getRemoveAtTicks() == 0) {
      // Packet hasn't been served yet. Serve now and call delay finished.
      headPacket.setDelayFinished(tick);
      headPacket.setRemoveAtTicks(tick);
    } else if (tick >= headPacket.getRemoveAtTicks()) {
      // Packet finished processing. Remove packet
      Packet finished = buffer.remove();
      totalPacketDelay += finished.getDelay();
    }
  }

  public static String round(double val) {
    return String.format("%.2f", val);
  }

  /*
   * Calculate and display the results such as average number of packets in
   * queue, average delay in queue and idle time for the server.
   */
  private static void compute_performances() {
    System.out.println("E[N]: Average number of packets in the buffer/queue: " + round((double) runningBufferSizeCount / totalTicks));
    System.out.println("E[T]: Average sojourn time: " + round((double) totalPacketDelay / totalPackets + (double) serviceTime) + "/packet");
    System.out.println("P(idle): The proportion of time the server is idle: " + round((double) idleServerCount / totalTicks * 100) + "%");
    if (maxBufferSize >= 0) {
      System.out.println("P(loss): The packet loss probability: " + round((double) droppedPacketCount / totalPackets * 100) + "%");
    }
  }
}