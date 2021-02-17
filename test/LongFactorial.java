public class LongFactorial {
  public static void main(String[] args) {
    long l = factorial(10);
    System.out.println(l);
  }
  public static long factorial(long n) {
    if (n <= 1)
      return 1;
    else
      return n * factorial(n - 1);
  }
}