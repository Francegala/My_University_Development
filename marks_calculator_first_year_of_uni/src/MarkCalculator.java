import java.util.Arrays;
import java.util.Scanner;

public class MarkCalculator {
	static int[] marks;

	public static void main(String[] args) {
		double[] studentArray = new double[12];
		for (int i = 0; i < 12; i++) {
			if (i == 0) {
				System.out.println("Module 1 CSC1021 " + "Exam mark");
			}
			if (i == 1) {
				System.out.println("Coursework mark");
			}
			if (i == 2) {
				System.out.println("Module 2 CSC1022 " + "Exam mark");
			}
			if (i == 3) {
				System.out.println("Coursework mark");
			}
			if (i == 4) {
				studentArray[i] = 0;
				continue;
			}
			if (i == 5) {
				System.out.println("Module 3 CSC1023 Coursework mark");
			}
			if (i == 6) {
				System.out.println("Module 4 CSC1024 " + "Exam mark");
			}
			if (i == 7) {
				System.out.println("Coursework mark");
			}
			if (i == 8) {
				System.out.println("Module 5 CSC1025 " + "Exam mark");
			}
			if (i == 9) {
				System.out.println("Coursework mark");
			}
			if (i == 10) {
				System.out.println("Module 6 CSC1026 " + "Exam mark");
			}
			if (i == 11) {
				System.out.println("Coursework mark");
			}
			Scanner s1 = new Scanner(System.in);
			studentArray[i] = s1.nextDouble();

		}
		/*
		 * for (int h=0;h<12;h++){ System.out.println(studentArray[h]); }
		 */
		marks = computeMarks(studentArray);
		computeResult(studentArray);
	}

	public static int[] computeMarks(double[] studentArray) {

		double[] computed_module_mark = new double[6];
		int[] computed_module_mark_int = new int[6];
		int[] cw = { 50, 40, 100, 50, 20, 35 };
		int c = 0;
		int h = 0;

		for (int a = 0; a < 6; a++) {
			if (a == 2) {
				computed_module_mark[a] = studentArray[c + 1];
				// System.out.println(computed_module_mark[a]);
				c += 2;
			} else {
				computed_module_mark[a] = (((studentArray[c + 1] * cw[a]) + ((100 - cw[a]) * studentArray[c])) / 100);
				// System.out.println(computed_module_mark[a]);
				c += 2;
			}
		}
		/*
		 * for (int gg=0;gg<6;gg++){ System.out.println(computed_module_mark[gg]);}
		 */

		for (int m = 0; m < 6; m++) {
			switch (m) {
			case 2:
				// System.out.println("m" + m);
				// System.out.println("h" + h);
				// System.out.println("cmm" + computed_module_mark_int[m]);
				computed_module_mark_int[m] = (int) Math.round(computed_module_mark[m]);
				// System.out.println(computed_module_mark_int[m]);
				// System.out.println(computed_module_mark[m]);
				h += 2;
				// System.out.println("h" + h);
				continue;
			default:
				// System.out.println("m" + m);

				if (studentArray[h + 1] >= 35 && studentArray[h] >= 35) {
					// System.out.println("h1" + h);
					// System.out.println(computed_module_mark[m]);
					computed_module_mark_int[m] = (int) Math.round(computed_module_mark[m]);
					// System.out.println(computed_module_mark_int[m]);
					// System.out.println(computed_module_mark[m]);
					h += 2;
					// System.out.println("h" + h);
					continue;
				}

				if (studentArray[h + 1] < 35 || studentArray[h] < 35) {
					// System.out.println("h2" + h);
					// System.out.println("cmm" + computed_module_mark[m]);
					computed_module_mark_int[m] = (int) Math.round(Math.min(35, computed_module_mark[m]));
					// System.out.println(computed_module_mark_int[m]);
					// System.out.println(computed_module_mark[m]);
					h += 2;
					// System.out.println("h" + h);
					continue;
				}

			}
		}
		for (int stn = 0; stn < 6; stn++) {
			System.out.println(computed_module_mark_int[stn]);
		}

		return computed_module_mark_int;
	}

	public static void computeResult(double[] studentArray) {
		int[] performance = new int[6];
		int counter = 0;
		int[] computed_module_mark_int = marks;
		int averageI;
		boolean noFail = false;

		for (int i = 0; i < 6; i++) {

			if (computed_module_mark_int[i] >= 40) {
				System.out.println("PASS");
				performance[i] = 1;
			}
			if (computed_module_mark_int[i] < 40 && computed_module_mark_int[i] >= 35 && (i != 1)) {
				System.out.println("COMPENSATABLE FAIL");
				performance[i] = 2;
				counter++;
			}
			if (computed_module_mark_int[i] < 35 || (computed_module_mark_int[i] <= 40 && (i == 1))) { // Programming 2
																										// or pass or
																										// fail? not
																										// Compensatable
																										// Fail
				System.out.println("FAIL");
				performance[i] = 3;

			}
		}

		averageI = (computed_module_mark_int[0] + computed_module_mark_int[1] + computed_module_mark_int[2]
				+ computed_module_mark_int[3] + computed_module_mark_int[4] + computed_module_mark_int[5]) / 6;
		if (performance[0] != 3 && performance[1] != 3 && performance[2] != 3 && performance[3] != 3
				&& performance[4] != 3 && performance[5] != 3) {
			noFail = true;
		}

		if (performance[0] == 1 && performance[1] == 1 && performance[2] == 1 && performance[3] == 1
				&& performance[4] == 1 && performance[5] == 1) {
			System.out.println("STAGE 1 PASS");
		} else {
			if (averageI > 40 && noFail == true && counter < 3) {
				System.out.println("STAGE 1 PASS BY COMPENSATION");
			} // credits
			else {
				System.out.println("FAIL");
			}
		}
	}

}
