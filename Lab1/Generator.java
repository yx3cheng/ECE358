public class Generator {

  private double lambda;

  public Generator(double newLambda) {
    lambda = newLambda;
  }

  public double getLambda() {
    return lambda;
  }

  public void setLambda(double newLambda) {
    lambda = newLambda;
  }

  public double generateExpRandomNum() {
    return Math.log(1 - Math.random()) * (-1.0 / lambda);
  }
}
