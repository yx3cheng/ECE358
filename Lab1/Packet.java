public class Packet {

  private long startTime;
  private long delayFinished;
  private long serviceTime;
  private long removeAtTicks;

  public Packet(long newStartTime, long newServiceTime) {
    startTime = newStartTime;
    serviceTime = newServiceTime;
    removeAtTicks = 0;
  }

  public long getRemoveAtTicks() {
    return this.removeAtTicks;
  }

  public void setDelayFinished(long currentTick) {
    delayFinished = currentTick;
  }

  public void setRemoveAtTicks(long currentTick) {
    removeAtTicks = currentTick + serviceTime;
  }

  public long getDelay() {
    return delayFinished - startTime;
  }

}