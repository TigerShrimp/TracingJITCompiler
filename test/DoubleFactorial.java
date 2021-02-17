public class DoubleFactorial {
  public static void main(String[] args) {
    double d = factorial(6);
    System.out.println(d);
  }
  public static double factorial(double n) {
    if (n <= 1.0)
      return 1.0;
    else
      return n * factorial(n - 1.2);
  }
}