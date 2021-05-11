public class FloatFactorial {
  public static void main(String[] args) {
    float d = factorial(6f);
    System.out.println(d);
  }
  public static float factorial(float n) {
    if (n <= 1f)
      return 1f;
    else
      return n * factorial(n - 1f);
  }
}