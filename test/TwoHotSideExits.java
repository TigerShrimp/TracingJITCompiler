public class TwoHotSideExits {
  public static void main(String[] args) {
    int i = 0;
    for (int j = 0; j < 100000; j++) {
      if (j < 33333) {
        i += 1;
      } else if (j < 66666) {
        i += 2;
      } else {
        i += 3;
      }
    }
    System.out.println(i);
  }
}