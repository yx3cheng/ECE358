public class Packet {

    private int startTime;
    private int delayFinished;
    private int serviceTime;
    private int removeAtTicks;

    public Packet(int startTime, int serviceTime) {
        this.startTime = startTime;
        this.serviceTime = serviceTime;
        removeAtTicks = 0;
    }

    public int getRemoveAtTicks() {
        return this.removeAtTicks;
    }

    public void setDelayFinished(int currentTick) {
        this.delayFinished = currentTick;
        removeAtTicks = currentTick + serviceTime;
    }

    public int getDelay() {
        return this.delayFinished - this.startTime;
    }

}