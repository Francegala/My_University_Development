/**
 * @author: Francesco Galassi
 * @student No: 170492959
 * @date: 13/12/2017
 * @purpose:This class stores the size of a bed (Single or Double)
 */
public class Bed {

	// the purpose of this class is to store the size of each bed
	private String size;// stored in private variable

	public Bed(String Sizeofabed) {// this is the constructor of the Bed class
		setsizeBed(Sizeofabed);// pass the String ("single") or ("double")

	}

	// getter and setter methods to store the size of the beds
	public void setsizeBed(String Sizeofabed) {
		this.size = Sizeofabed;
	}

	public String getsizeBeds() {
		return size;// and then return the Size
	}

	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}

}
