public class main {
  public static void main(String[] args) {
    int j = 1;
    for (int i = 0; i < 250; i++) {
      int k = i + j;
      j = k;
    }
    System.out.println(j);
  }
}
