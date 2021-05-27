public class SingleLoopWithHotSideExit {
  public static void main(String[] args) {
    foo();
  }
  public static void foo() {
    int i = 0;
    for (int j = 0; j < 10000000; j++) {
      if (j % 100 == 0) {
        i += 2;
      } else if (j % 50 == 0) {
        i += 1;
      } else {
        i += 3;
      }
    }
  }
}