public class Generator {

    private double lamda;

    public Generator(double lamda) {
        this.lamda = lamda;
    }

    public double getLamda() {
        return lamda;
    }

    public void setLamda(double lamda) {
        this.lamda = lamda;
    }

    public double generateExpRandomNum() {
        return Math.log(1 - Math.random())
                * (-1.0 / getLamda());
    }
}
