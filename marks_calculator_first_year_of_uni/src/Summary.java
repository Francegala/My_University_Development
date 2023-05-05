import java.util.Scanner;

public class Summary {

	static int[] studentArrayinteger;// I need to create a variable not local to
	// put the array with marks

	public static int[] studentSummary() {
		int[] studentArrayinteger = new int[12];// create un array of 12 elements for exams and courseworks marks
		Scanner s1 = new Scanner(System.in); // ask for an input

		System.out.println("Please for each module write marks and press Enter:"+'\n'+"All marks are integers from 0 to 100"+'\n');// for the user
		for (int i = 0; i < 12; i++) {// for loop asking marks
			if (i == 0) {
				System.out.println("Module 1 CSC1021 " + "Exam mark: ");
			} // 1' exam mark
			if (i == 1) {
				System.out.println("Coursework mark: ");
			} // 1' coursework mark
			if (i == 2) {
				System.out.println("Module 2 CSC1022 " + "Exam mark: ");
			} // 2' exam mark
			if (i == 3) {
				System.out.println("Coursework mark: ");
			} // 2' coursework mark
			if (i == 4) {
				studentArrayinteger[i] = 0;// 3' exam mark
				continue;
			} // jump to the next one
			if (i == 5) {
				System.out.println("Module 3 CSC1023 Coursework mark: ");
			} // 3' coursework mark
			if (i == 6) {
				System.out.println("Module 4 CSC1024 " + "Exam mark: ");
			} // 4' exam mark
			if (i == 7) {
				System.out.println("Coursework mark: ");
			} // 4' coursework mark
			if (i == 8) {
				System.out.println("Module 5 CSC1025 " + "Exam mark: ");
			} // 5' exam mark
			if (i == 9) {
				System.out.println("Coursework mark: ");
			} // 5' coursework mark
			if (i == 10) {
				System.out.println("Module 6 CSC1026 " + "Exam mark: ");
			} // 6' exam mark
			if (i == 11) {
				System.out.println("Coursework mark: ");
			} // 6' coursework mark
			studentArrayinteger[i] = s1.nextInt(); // create an array with what the user type
			if (studentArrayinteger[i] < 0 || studentArrayinteger[i] > 100) {
				System.out.println("The mark should be positive and lower or equal to 100. Please restart the program");//check
				System.exit(0);//stop the program
			}
		}
		s1.close();//stop asking input

		return studentArrayinteger;

	}

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		studentArrayinteger = studentSummary();
		// MarkCalculator.computeMarks(studentArrayinteger);
		StudentChart studentchart = new StudentChart(studentArrayinteger);
		studentchart.draw(MarkCalculator.computeMarks(studentArrayinteger));
		studentchart.printSummary(MarkCalculator.computeMarks(studentArrayinteger));
	}

}
