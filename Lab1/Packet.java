public class Packet {

  private int startTime;
  private int delayFinished;
  private int serviceTime;
  private int removeAtTicks;

  public Packet(int newStartTime, int newServiceTime) {
    startTime = newStartTime;
    serviceTime = newServiceTime;
    removeAtTicks = 0;
  }

  public int getRemoveAtTicks() {
    return this.removeAtTicks;
  }

  public void setDelayFinished(int currentTick) {
    delayFinished = currentTick;
  }

  public void setRemoveAtTicks(int currentTick) {
    removeAtTicks = currentTick + serviceTime;
  }

  public int getDelay() {
    return delayFinished - startTime;
  }

}