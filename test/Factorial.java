public class Factorial {
  public static void main(String[] args) {
    int f = factorial(5);
    System.out.println(f);
  }

  public static int factorial(int n) {
    if (n <= 1)
      return 1;
    else
      return n * factorial(n - 1);
  }
}