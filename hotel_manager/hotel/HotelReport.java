/**
 * @author: Francesco Galassi
 * @student No: 170492959
 * @date: 13/12/2017
 * @purpose:Given a Hotel object it produces a short textual report describing
 *                the name of the hotel, the number of rooms and, for each room,
 *                lists the number and size of the beds
 */
public class HotelReport {

	public static void main(String[] args) {
	}

	public static void report(Hotel h) {// given an hotel, this report method print a neatly written up report

		System.out.println('\n' + "-------------------------------------------------");
		System.out.println('\n' + "Hotel Report:");
		System.out.printf("%1s %2s \n%10s %11s \n", "Hotel Name:", h.getName(), "Number of rooms:", h.getRooms());// it prints the name of the hotel and the number of rooms
		int roomCounter = 0;
		for (Room room : h.getRoomArray()) {
			System.out.println('\n');
			System.out.printf("%1s %6d %9s %10s %-11s \n", "Room number ", (roomCounter + 1), " has: ", room.getBeds(), // it prints the number of beds for each room
					" bed(s) ");
			int bedCounter = 0;
			for (Bed bed : room.getaddBed()) {
				System.out.printf("%1s %2s %5d %8s %-9s \n", " ", "Bed number ", (bedCounter + 1), " is: ",
						bed.getsizeBeds());// it prints the size of the bed

				bedCounter++;// the program uses some counters
			}

			roomCounter++;
		}
		System.out.println('\n');
		if (h.hasVacancies() == true) {// if the hotel has a vacancy
			System.out.println("The hotel has a vacancy");// it prints a positive message
		}
		if (h.hasVacancies() == false) {// if the hotel does not have a vacancy
			System.out.println("The hotel does not have a vacancy");// it prints a negative message
		}
		System.out.println("The maximum occupancy of " + h.getName() + " is:" + " " + h.occupancy());// it prints the hotel occupancy
		System.out.println('\n' + "-------------------------------------------------");

	}

}