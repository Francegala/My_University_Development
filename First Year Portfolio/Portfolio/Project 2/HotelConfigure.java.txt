/**
 * @author: Francesco Galassi
 * @student No: 170492959
 * @date: 13/12/2017
 * @purpose:In this class the user can choose the properties of the Hotel
 */
import java.util.List;
import java.util.Scanner;

public class HotelConfigure {

	public Room[] roomArray;
	public List<Bed> bedArray;

	public static void main(String[] args) {// this main method demonstrate that it works
		Hotel newHotel = new Hotel();// the hotel object created
		Scanner s = new Scanner(System.in);// the user decides the features of the hotel
		String inputName;// where it will be stored the name of the hotel
		int inputRooms;// the number of rooms in the hotel
		int inputBeds;// how many beds in each room
		String sizeofabed;// a string for the bed size
		String vacancy;// a string to store the word Yes or Not
		int counterBeds = 0;// a counter

		System.out.println("Please, create a New Hotel... write the name");
		inputName = s.nextLine();// input for the name
		System.out.println("How many room does the hotel have?");
		inputRooms = s.nextInt();// input for the number of rooms
		if (inputRooms <= 0) {// the number must be greater than zero
			System.out.println("Please write a number greater than zero");
			inputRooms = s.nextInt();
			while (inputRooms <= 0) {
				System.out.println("Please write a number greater than zero");
				inputRooms = s.nextInt();
			}
		}
		newHotel.setHotel(inputName, inputRooms);// create the hotel with a
													// constructor
		Room[] roomArray = new Room[inputRooms];// create an array to store the rooms

		newHotel.setRoomArray(roomArray);// set the array to the hotel

		for (int counterRoom = 0; counterRoom < inputRooms; counterRoom++) {
			System.out.println("How many beds does the room number " + (counterRoom + 1) + " have?"); // ask how many beds for each room
			inputBeds = s.nextInt();// save the input
			if (inputBeds <= 0) {
				System.out.println("Please write a number greater than zero?");// error
				// checker
				inputBeds = s.nextInt();
				while (inputBeds <= 0) {
					System.out.println("Please write a number greater than zero");
					inputBeds = s.nextInt();
				}
			}

			roomArray[counterRoom] = new Room(inputBeds);// create the Rooms
			for (counterBeds = 0; counterBeds < inputBeds; counterBeds++) {
				System.out.println("Does the bed number " + (counterBeds + 1) + " is Single or Double?");
				System.out.println("Please write \"Single\" or \"Double\"");
				sizeofabed = s.next().toLowerCase();// save it to lower case to avoid errors like "SiNgLe"
				if (sizeofabed.equals("single") || sizeofabed.equals("double")) {
					roomArray[counterRoom].addBed(new Bed(sizeofabed));// add the beds to the room and set it as single or double
				} else {// if the user misspell the word
					while ((!sizeofabed.equals("single") && !sizeofabed.equals("double"))) {
						System.out.println("Please write \"Single\" or \"Double\"");
						sizeofabed = s.next().toLowerCase();
						if (sizeofabed.equals("single") || sizeofabed.equals("double")) {
							roomArray[counterRoom].addBed(new Bed(sizeofabed));
							break;
						}

					}
				}
			}
			System.out.println("Is this room vacant?");
			System.out.println("Please write \"Y\" (for yes) or \"N\" (for not)");
			vacancy = s.next().toLowerCase();// save the vacancy it does not matter if it is upper case or lower case
			if (vacancy.equals("y")) {// yes
				roomArray[counterRoom].setroomVacancy(true);

			}
			if (vacancy.equals("n")) {// not
				roomArray[counterRoom].setroomVacancy(false);
			} else {// if the user write another letter
				while (!vacancy.equals("y") && !vacancy.equals("n")) {
					System.out.println("Please write \"Y\" (for yes) or \"N\" (for not)");
					vacancy = s.next().toLowerCase();
					if (vacancy.equals("y")) {
						roomArray[counterRoom].setroomVacancy(true);
						break;
					}
					if (vacancy.equals("n")) {
						roomArray[counterRoom].setroomVacancy(false);
						break;
					}

				}
			}

		}

		s.close();
		HotelReport.report(newHotel);// the report of the hotel

	}
}
