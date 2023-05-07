/**
 * @author: Francesco Galassi
 * @student No: 170492959
 * @date: 13/12/2017
 * @purpose:This class creates and tests the program showing also the maximum occupancy
 */
import java.util.List;

public class HotelTest {// this class creates an hotel and test the program

	public Room[] roomArray;// private variable to create the room objects
	public List<Bed> bedArray;// private variable to create the list of beds

	public static void main(String[] args) {// this main method demonstrate that it works
		Hotel hotelTest = new Hotel();// the program create an hotel
		String inputName;// the name of the hotel is in a String variable
		int inputRooms;// an integer to create the number of rooms

		inputName = "New Hotel Test";// New Hotel Test is the name of the hotel
		inputRooms = 3;// the hotel has 3 rooms
		hotelTest.setHotel(inputName, inputRooms);// the hotel is created with a constructor
		Room[] roomArray = new Room[inputRooms];// all the rooms are stored in an Array
		hotelTest.setRoomArray(roomArray);// the array is passed to the hotel class
		roomArray[0] = new Room(2);// the first room has 2 beds
		roomArray[0].setroomVacancy(false);// the room is not vacant
		roomArray[1] = new Room(1);// the second room has 1 bed
		roomArray[1].setroomVacancy(false);// the room is not vacant
		roomArray[2] = new Room(3);// the third room has 3 beds
		roomArray[2].setroomVacancy(false);// the room is not vacant
		roomArray[0].addBed(new Bed("single"));// the first bed of the first room is single
		roomArray[0].addBed(new Bed("double"));// the second bed of the first room is double

		roomArray[1].addBed(new Bed("double"));// the first bed of the first room is double

		roomArray[2].addBed(new Bed("double"));// the first bed of the first room is double
		roomArray[2].addBed(new Bed("double"));// the second bed of the first room is double
		roomArray[2].addBed(new Bed("double"));// the third bed of the first room is double

		HotelReport.report(hotelTest);// it prints the report of the hotel created

	}

}
