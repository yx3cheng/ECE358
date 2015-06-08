public class Server {

  private long totalPacketDelay = 0;

  // Counters
  private int droppedPacketCount = 0;
  private long idleServerCount = 0;

  private Packet currentPacket;

  public boolean isBusy() {
    return (currentPacket != null);
  }

  public void work(long tick) {
    if (tick >= currentPacket.getRemoveAtTicks()) {
      // Packet finished processing. Remove packet
      totalPacketDelay += currentPacket.getDelay();
      currentPacket = null;
    }
  }

  public void idle() {
    idleServerCount++;
  }

  /*
   * Check the queue for the packet, if head of the queue is empty, return 0
   * else if the queue is non-empty delete the packet from the queue after an
   * elapse of the deterministic service time.
   */
  public void departure(Packet packet, long tick) throws Exception {
    if (currentPacket != null)
      throw new Exception("server is busy");
    if (packet.getRemoveAtTicks() != 0)
      throw new Exception("Packet has already been processed");

    currentPacket = packet;
    // Packet hasn't been served yet. Serve now and call delay finished.
    currentPacket.setDelayFinished(tick);
    currentPacket.setRemoveAtTicks(tick);
  }

  public long getTotalPacketDelay() {
    return totalPacketDelay;
  }

  public int getDroppedPacketCount() {
    return droppedPacketCount;
  }

  public long getIdleServerCount() {
    return idleServerCount;
  }
}
