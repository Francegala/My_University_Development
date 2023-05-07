/**
 * @author: Francesco Galassi
 * @student No: 170492959
 * @date: 13/12/2017
 * @purpose:This class stores all the essential informations about a hotel,
 *               including a name and some room
 */
public class Hotel {

	// these private variables can only be used inside the class
	private String name; // where to store the name of the Hotel
	private boolean hasVacancies; // boolean to check if the hotel has a vacancy
	private int numRooms; // how many rooms the hotel has
	private Room[] roomArray; // the array containing all the rooms

	public void setHotel(String givenname, int howmanyrooms) { // the constructor to create the hotel
		setName(givenname);// the hotel must have a name
		setRooms(howmanyrooms);// the hotel must have at least a room
	}

	// with these getter/setter methods the program save and return the name propriety
	public void setName(String givenname) {// pass the name to the constructor in order to set it to the value of the parameter
		this.name = givenname;// set the parameter equal to the name passed (String)
	}

	public String getName() {// method to return the name of the hotel
		return this.name;
	}

	// with these getter/setter methods the program save and return the number of rooms it has to create
	public void setRooms(int howmanyrooms) {// pass the number of the rooms (int)
		this.numRooms = howmanyrooms;// set it as the parameter
	}

	public int getRooms() {// return the number of rooms
		return this.numRooms;
	}

	// with these getter/setter methods the program save and return the array containing the Room objects
	public void setRoomArray(Room[] roomArray) {// pass the array with all the rooms
		this.roomArray = roomArray;
	}

	public Room[] getRoomArray() {// return the array
		return this.roomArray;
	}

	// with occupancy method the program count the maximum occupancy of the hotel adding one to single rooms and two to double
	public int occupancy() {
		int o = 0;
		int roomCounter = 0;
		int bedCounter = 0;
		for (Room room : getRoomArray()) {// for loop for the rooms
			for (Bed bed : room.getaddBed()) {// for loop for the beds
				if (bed.getsizeBeds().equals("single")) {// if the room is "single" type
					o++;// counter +1
				} else {// if the room is double
					o += 2;// occupancy counter +2
				}
				bedCounter++;
			}

			roomCounter++;
		}
		return o; //return occupancy
	}

	// with this method the program check if at least a room is vacant
	public boolean hasVacancies() {
		for (Room room : getRoomArray()) {
			if (room.getroomVacancy() == true) {// if at least one room is vacant, the hotel has a vacancy and the default value of a primitive boolean is false so if no room is vacant the boolean is false
				this.hasVacancies = true;
			}
		}
		return this.hasVacancies;// return if the hotel has vacancies

	}

	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}

}
