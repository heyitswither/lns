public class javatest{
	public static void main(String[] args) {
		long start = System.currentTimeMillis();
		int max = 100000;
		for(int i = 0;i<max;i++){}
		System.out.println("Done in " + (System.currentTimeMillis() - start) + " milliseconds.");
	}
}
