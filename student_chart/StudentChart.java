
public class StudentChart {//new class StudentChart to produce the thin-bar chart

	int [] studentArrayinteger;//I need to create a variable not local to put the array with marks

	//Constructor Outlines the blueprints for an object (A StudentChart Object)
	public StudentChart(int[] studentArrayinteger){// this is the constructor with as parameter, the array of integers specifying exam and the coursework marks
		this.studentArrayinteger = studentArrayinteger;//this. reference to the current object to connect the arrays
	}

	public void draw (int[] computed_module_mark_int){ //draws thin bars based on the values in that array using the MarkCalculator class you produced in Part 1
		/*for (int i=0; i<studentArrayinteger.length; i++)
		{
		//int [] aaa = new int [0];
		//aaa[0]=studentArrayinteger[0];					//1' attempt to test if it worked
		System.out.println(studentArrayinteger[i]);
		}*/

		// now let's create bars x- and y- axes
		Bar bary = new Bar();
		bary.makeVisible(); // make every bar visible
		bary.changeSize(10, 200); //decide dimension
		bary.changeColour(Colour.BLACK);//colour of the bar
		bary.moveHorizontal(-10);//decide the position

		Bar barx = new Bar();
		barx.makeVisible();
		barx.changeSize(250, 10);
		barx.changeColour(Colour.BLACK);
		barx.moveVertical(200);

		//let's create all the bars for each modules
		Bar bar0 = new Bar();
		bar0.makeVisible();
		bar0.changeSize(20, 2*computed_module_mark_int[0]); //the Width for each bar is fixed as 20 and the Height is 2*computed module mark
		bar0.moveVertical(200-(2*computed_module_mark_int[0])); //position of the bar 200 (it is the beginning of the axes) - the length 
		if(computed_module_mark_int[0]>=70){bar0.changeColour(Colour.MAGENTA);}     //color magenta if >70
		if(computed_module_mark_int[0]>=40 && computed_module_mark_int[0]<70){bar0.changeColour(Colour.GREEN);}//color green if PASS but <70
		if(computed_module_mark_int[0]<40 && computed_module_mark_int[0]>=35){bar0.changeColour(Colour.YELLOW);}//color yellow if compensation
		if(computed_module_mark_int[0]<35){bar0.changeColour(Colour.RED);}//color red if failed

		Bar bar1 = new Bar();
		bar1.makeVisible();
		bar1.changeSize(20, 2*computed_module_mark_int[1]);
		bar1.moveHorizontal(35);
		bar1.moveVertical(200-(2*computed_module_mark_int[1]));
		if(computed_module_mark_int[1]>=70){bar1.changeColour(Colour.MAGENTA);}     
		if(computed_module_mark_int[1]>=40 && computed_module_mark_int[1]<70){bar1.changeColour(Colour.GREEN);}
		if(computed_module_mark_int[1]<40){bar1.changeColour(Colour.RED);}

		Bar bar2 = new Bar();
		bar2.makeVisible();
		bar2.changeSize(20, 2*computed_module_mark_int[2]);
		bar2.moveHorizontal(70);
		bar2.moveVertical(200-(2*computed_module_mark_int[2]));
		if(computed_module_mark_int[2]>=70){bar2.changeColour(Colour.MAGENTA);}        
		if(computed_module_mark_int[2]>=40 && computed_module_mark_int[2]<70){bar2.changeColour(Colour.GREEN);}
		if(computed_module_mark_int[2]<40 && computed_module_mark_int[2]>=35){bar2.changeColour(Colour.YELLOW);}
		if(computed_module_mark_int[2]<35){bar2.changeColour(Colour.RED);}

		Bar bar3 = new Bar();
		bar3.makeVisible();
		bar3.changeSize(20, 2*computed_module_mark_int[3]);
		bar3.moveHorizontal(110);
		bar3.moveVertical(200-(2*computed_module_mark_int[3]));
		if(computed_module_mark_int[3]>=70){bar3.changeColour(Colour.MAGENTA);}        
		if(computed_module_mark_int[3]>=40 && computed_module_mark_int[3]<70){bar3.changeColour(Colour.GREEN);}
		if(computed_module_mark_int[3]<40 && computed_module_mark_int[3]>=35){bar3.changeColour(Colour.YELLOW);}
		if(computed_module_mark_int[3]<35){bar3.changeColour(Colour.RED);}

		Bar bar4 = new Bar();
		bar4.makeVisible();
		bar4.changeSize(20, 2*computed_module_mark_int[4]);
		bar4.moveHorizontal(145);
		bar4.moveVertical(200-(2*computed_module_mark_int[4]));
		if(computed_module_mark_int[4]>=70){bar4.changeColour(Colour.MAGENTA);}        
		if(computed_module_mark_int[4]>=40 && computed_module_mark_int[4]<70){bar4.changeColour(Colour.GREEN);}
		if(computed_module_mark_int[4]<40 && computed_module_mark_int[4]>=35){bar4.changeColour(Colour.YELLOW);}
		if(computed_module_mark_int[4]<35){bar4.changeColour(Colour.RED);}

		Bar bar5 = new Bar();
		bar5.makeVisible();
		bar5.changeSize(20, 2*computed_module_mark_int[5]);
		bar5.moveHorizontal(180);
		bar5.moveVertical(200-(2*computed_module_mark_int[5]));
		if(computed_module_mark_int[5]>=70){bar5.changeColour(Colour.MAGENTA);}        
		if(computed_module_mark_int[5]>=40 && computed_module_mark_int[5]<70){bar5.changeColour(Colour.GREEN);}
		if(computed_module_mark_int[5]<40 && computed_module_mark_int[5]>=35){bar5.changeColour(Colour.YELLOW);}
		if(computed_module_mark_int[5]<35){bar5.changeColour(Colour.RED);}
	}

	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}

	public void printSummary(int[] computed_module_mark_int){// it neatly prints a table of returned marks corresponding to your chart.

		String[] passtable = new String[7];//
		passtable = MarkCalculator.computeResult(Summary.studentArrayinteger);//call for the table if you pass/compensatable/fail
		String string1 = new String ("Module");// string for each module
		System.out.printf( "%-15s %24s %26s %48s %50s %n", ("Modules:"), ("Exam Mark"), ("Coursework Mark"), ("Module Result"), ("Module Mark") );//formatting print first line
		int kj=0;//a counter
		for(int a=0; a<6;a++){//loop for each module 
			if(a==0){string1="Module 1 CSC1021";}
			if(a==1){string1="Module 2 CSC1022";}
			if(a==2){string1="Module 3 CSC1023";}			//print all the modules
			if(a==3){string1="Module 4 CSC1024";}
			if(a==4){string1="Module 5 CSC1025";}
			if(a==5){string1="Module 6 CSC1026";}
			if(a==2){System.out.printf( "%-15s %20s %23s %47s %54s %n", string1,"",studentArrayinteger[kj+1], computed_module_mark_int[a],passtable[a]);//print a formatted table for software engineering
			kj=kj+2;//increase counter
			}
			else{
				System.out.printf( "%-15s %20s %23s %47s %54s %n", string1,studentArrayinteger[kj],studentArrayinteger[kj+1], computed_module_mark_int[a],passtable[a] );//print a formatted table
				kj=kj+2;//increase counter
			}

		}
		System.out.printf('\n' + passtable[6]+'\n');//STAGE 1 + overall mark

	}
}
