

public class MarkCalculator {
	//static int[] marks; // not local int
	//static int[] studentArrayinteger; // not local int
	public static void main(String[] args) { // main method
		//studentArrayinteger = Summary.studentSummary();
		// call the other method
		//marks = computeMarks(studentArrayinteger);		Part 1 test
		//computeResult(studentArray);
	}
	public static int[] computeMarks(int[] studentArrayinteger) { // given an array of student exam and coursework marks, returns an array of returned module marks.
		double[] computed_module_mark = new double[6]; // create an array for each module
		int[] computed_module_mark_int = new int[6]; // create an array but integer
		int[] cw = { 50, 40, 100, 50, 20, 35 };// array with the weight
		int c = 0;// I need a counter to look into the array
		int h = 0;// another counter

		for (int a = 0; a < 6; a++) {//select right module
			if (a == 2) {//software engineering
				computed_module_mark[a] = (double) studentArrayinteger[c + 1]; // software engineering has only coursework
				// System.out.println(computed_module_mark[a]);
				c += 2;//increase counter
			} else {
				computed_module_mark[a] = ((((double)studentArrayinteger[c + 1] * (double)cw[a]) + (((double)100 - (double)cw[a]) * (double)studentArrayinteger[c])) / 100);// given formula
				// System.out.println(computed_module_mark[a]);
				c += 2;//increase counter
			}
		}
		//for (int gg=0;gg<6;gg++){
		//System.out.println(computed_module_mark[gg]);}
		for (int m = 0; m < 6; m++) {
			switch (m) { // attempt of using a switch
			case 2:// software engineering
				// System.out.println("m" + m);		does it work properly?
				// System.out.println("h" + h);
				// System.out.println("cmm" + computed_module_mark_int[m]);
				computed_module_mark_int[m] = (int) Math.round(computed_module_mark[m]); // rounded
				// to the nearest whole number integer
				// System.out.println(computed_module_mark_int[m]);
				// System.out.println(computed_module_mark[m]);
				h += 2;//increase counter
				// System.out.println("h" + h);
				continue;//go in the next case of the switch
			default://other modules
				// System.out.println("m" + m);

				if (studentArrayinteger[h + 1] >= 35 && studentArrayinteger[h] >= 35) {//exam and coursework marks >= 35
					// System.out.println("h1" + h);
					// System.out.println(computed_module_mark[m]);
					computed_module_mark_int[m] = (int) Math.round(computed_module_mark[m]);// rounded
					// to the nearest whole number integer
					// System.out.println(computed_module_mark_int[m]);
					// System.out.println(computed_module_mark[m]);
					h += 2;//increase counter
					// System.out.println("h" + h);
					continue;//go in the next case of the switch
				}

				if (studentArrayinteger[h + 1] < 35 || studentArrayinteger[h] < 35) {//exam or coursework marks >= 35
					// System.out.println("h2" + h);
					// System.out.println("cmm" + computed_module_mark[m]);
					computed_module_mark_int[m] = (int) Math.round(Math.min(35, computed_module_mark[m]));// rounded
					// to the nearest whole number integer
					// System.out.println(computed_module_mark_int[m]);
					// System.out.println(computed_module_mark[m]);
					h += 2;
					// System.out.println("h" + h);
					continue;//go in the next case of the switch
				}

			}
		}
		//for (int stn=0;stn<6;stn++){
		//System.out.println(computed_module_mark_int[stn]);//print all the marks }
		return computed_module_mark_int; //return the array
	}

	public static String[] computeResult(int[] studentArrayinteger) {//given an array of student exam and coursework marks, returns a Stage Result for that student.
		int[] performance = new int[6];// array with Pass, Compensatable or Fail
		int counter = 0;// counter for "no more than 2"
		//int[] computed_module_mark_int = Summary.studentArrayinteger;// marks from the previous method
		int[] computed_module_mark_int = MarkCalculator.computeMarks(Summary.studentArrayinteger);
		int averageI;// average of the marks
		boolean noFail = false;// boolean if no fail
		boolean Pass = false;// boolean if all are pass
		String[] passtable = new String[7];// new array to save if pass or not the module

		for (int i = 0; i < 6; i++) {

			if (computed_module_mark_int[i] >= 40) {//if the module mark is >= 40
				passtable[i] = "PASS"; // you pass the module if the grade is >=40
				performance[i] = 1;// 1= pass
			}
			if (computed_module_mark_int[i] < 40 && computed_module_mark_int[i] >= 35 && (i != 1)) {// exclude the second module that is a core module
				passtable[i] = "COMPENSATABLE FAIL";// compensatable if between 35 and 40
				performance[i] = 2;// 2=comp.
				counter++;// how many comp.
			}
			if (computed_module_mark_int[i] < 35 || ((computed_module_mark_int[i] < 40 && (i == 1)))) { // Programming 2 or pass or fail not Compensatable Fail
				passtable[i] = "FAIL";// fail if less then 35
				performance[i] = 3;// 3=fail

			}
		}

		averageI = (computed_module_mark_int[0] + computed_module_mark_int[1] + computed_module_mark_int[2] + computed_module_mark_int[3] + computed_module_mark_int[4] + computed_module_mark_int[5]) / 6;// average
		if (performance[0] != 3 && performance[1] != 3 && performance[2] != 3 && performance[3] != 3 && performance[4] != 3 && performance[5] != 3) 
		{
			noFail = true;
		} // there is no modules failed
		if (performance[0] == 1 && performance[1] == 1 && performance[2] == 1 && performance[3] == 1 && performance[4] == 1 && performance[5] == 1) 
		{
			Pass = true;
		} // all modules passed

		if (Pass == true) {
			passtable[6] = "STAGE 1 PERFORMANCE : PASS "+ '\n'+"YOUR AVERAGE MARK : "+ averageI;
		} // pass if all passed
		else {
			if (averageI > 40 && noFail == true && counter < 3) {
				passtable[6] = "STAGE 1 PERFORMANCE : PASS BY COMPENSATION "+ '\n'+"YOUR AVERAGE MARK : "+ averageI;
			} // average + no fail + credits <40 --> Pass by compensation
			else {
				passtable[6] = "STAGE 1 PERFORMANCE : FAIL "+ '\n'+"YOUR AVERAGE MARK : "+ averageI;
			} // otherwise failed
		}
		return passtable; //pass or not the modules and Stage
	}
}