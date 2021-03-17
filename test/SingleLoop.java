public class SingleLoop {
  public static void main(String[] args) {
    int j = 1;
    for (int i = 0; i < 10000; i++) {
      int k = i + j;
      j = k;
    }
    System.out.println(j);
  }
}
