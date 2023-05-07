/**
 * @author: Francesco Galassi
 * @student No: 170492959
 * @date: 13/12/2017
 * @purpose:This class stores the number of beds in a room
 */
import java.util.ArrayList;
import java.util.List;

public class Room {

	private List<Bed> bedArray;// this class is saving the List of beds inside the room
	private boolean isVacant;// this boolean checks if the room is vacant
	private int numBeds;// this class save the number of beds inside too

	public Room(int howmanybeds) {// the program pass the number of beds to the constructor in order to create for each room the right number of beds
		setBeds(howmanybeds);
		bedArray = new ArrayList<Bed>();// the ArrayList of Beds
	}

	// with these getter/setter methods the constructor define the number of beds
	public void setBeds(int howmanybeds) {
		this.numBeds = howmanybeds;
	}

	
	public int getBeds() {
		return this.numBeds; // and then return it
	}

	// with these methods a Bed object is added to the Bed list
	public void addBed(Bed b) {
		this.bedArray.add(b);
	}

	public List<Bed> getaddBed() {
		return bedArray; // and then return it
	}

	// with these getter/setter methods the program check if the room is vacant
	public void setroomVacancy(boolean vac) {
		this.isVacant = vac;
	}

	public boolean getroomVacancy() {
		return this.isVacant;// return the boolean value
	}

	public static void main(String[] args) {

	}

}
