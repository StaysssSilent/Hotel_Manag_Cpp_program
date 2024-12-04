#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <iomanip>

using namespace std;

class Customer {
public:
    string name;
    string mobileNo;
    string paymentType;

    Customer(const string &name, const string &mobileNo, const string &paymentType)
        : name(name), mobileNo(mobileNo), paymentType(paymentType) {}
};

    // Helper function to capitalize the first letter of a string
    string capitalize(const string &input) {
        string result = input;
        if (!result.empty()) {
            result[0] = toupper(result[0]);
            for (size_t i = 1; i < result.size(); i++) {
                result[i] = tolower(result[i]);
            }
        }
        return result;
    }

class Room {
public:
    int roomNumber;
    string roomType;
    bool isAvailable;
    bool hasAC;
    bool isNonSmoking;
    double pricePerNight;

    Room(int roomNumber, const string &roomType, bool hasAC, bool isNonSmoking, double pricePerNight)
        : roomNumber(roomNumber), roomType(roomType), hasAC(hasAC), isNonSmoking(isNonSmoking),
          pricePerNight(pricePerNight), isAvailable(true) {}

    bool matchesPreferences(const string &roomType, bool hasACPref, bool isNonSmokingPref) {
        return (this->roomType == roomType && this->isAvailable &&
                this->hasAC == hasACPref && this->isNonSmoking == isNonSmokingPref);
    }

    void bookRoom() {
        isAvailable = false;
    }

    void display() const {
        cout << "Room " << roomNumber << " (" << roomType << ") - "
             << (hasAC ? "With AC" : "Without AC") << ", "
             << (isNonSmoking ? "Non-Smoking" : "Smoking") << " - "
             << (isAvailable ? "Available" : "Booked") << endl;
    }
};

class Reservation {
public:
    static int idCounter;
    int bookingID;
    Customer customer;
    Room *room;
    string checkInDate;
    string checkOutDate;
    int numberOfPersons;
    int stayDuration;
    double mealCost;

    Reservation(Customer &customer, Room *room, const string &checkInDate, const string &checkOutDate, int numberOfPersons, int stayDuration, double mealCost)
        : customer(customer), room(room), checkInDate(checkInDate), checkOutDate(checkOutDate), numberOfPersons(numberOfPersons), stayDuration(stayDuration), mealCost(mealCost) {
        bookingID = ++idCounter;
        room->bookRoom();
    }

    void display() const {
        cout << "\nBooking ID: " << bookingID << "\nCustomer: " << customer.name
             << "\nMobile No: " << customer.mobileNo
             << "\nPayment Type: " << customer.paymentType
             << "\nRoom: " << room->roomNumber << "\nCheck-in: " << checkInDate
             << "\nCheck-out: " << checkOutDate << endl;
        cout << "Meal Cost: " << mealCost << " rupees" << endl;
    }

    double getTotalAmount() const {
        return room->pricePerNight * stayDuration + mealCost;
    }

    double getAdvancePayment() const {
        return getTotalAmount() / 2; // Advance payment is 50%
    }

    double getRemainingAmount() const {
        return getTotalAmount() - getAdvancePayment();
    }
};

int Reservation::idCounter = 0;

class Hotel {
public:
    string name;
    string location;
    vector<Room> rooms;
    vector<Reservation> reservations;

    Hotel(const string &name, const string &location) : name(name), location(location) {}

    void addRoom(int roomNumber, const string &roomType, bool hasAC, bool isNonSmoking, double pricePerNight) {
        rooms.emplace_back(roomNumber, roomType, hasAC, isNonSmoking, pricePerNight);
    }

    void checkAvailability(const string &roomType, bool hasACPref, bool isNonSmokingPref, vector<int> &availableRoomNumbers) {
        cout << "Available " << roomType << " rooms with " << (hasACPref ? "AC" : "No AC") << " and "
             << (isNonSmokingPref ? "Non-Smoking" : "Smoking") << " preference:\n";

        for (auto &room : rooms) {
            if (room.matchesPreferences(roomType, hasACPref, isNonSmokingPref)) {
                room.display();
                availableRoomNumbers.push_back(room.roomNumber);
            }
        }

        if (availableRoomNumbers.empty()) {
            cout << "\nNo rooms available matching your preferences.\n";
        }
    }

    int calculateDays(const string &checkInDate, const string &checkOutDate) {
        int yearIn, monthIn, dayIn, yearOut, monthOut, dayOut;
        sscanf(checkInDate.c_str(), "%d-%d-%d", &yearIn, &monthIn, &dayIn);
        sscanf(checkOutDate.c_str(), "%d-%d-%d", &yearOut, &monthOut, &dayOut);

        struct tm checkIn = {0, 0, 0, dayIn, monthIn - 1, yearIn - 1900};
        struct tm checkOut = {0, 0, 0, dayOut, monthOut - 1, yearOut - 1900};

        time_t checkInTime = mktime(&checkIn);
        time_t checkOutTime = mktime(&checkOut);

        double seconds = difftime(checkOutTime, checkInTime);
        return seconds / (60 * 60 * 24);
    }

    void makeReservation(Customer &customer, int roomNumber, const string &checkInDate, const string &checkOutDate, int numberOfPersons, double mealCost) {
        int stayDuration = calculateDays(checkInDate, checkOutDate);
        
        for (auto &room : rooms) {
            if (room.roomNumber == roomNumber && room.isAvailable) {
                reservations.emplace_back(customer, &room, checkInDate, checkOutDate, numberOfPersons, stayDuration, mealCost);
                cout << "\n\nReservation successful! Booking ID: " << reservations.back().bookingID << endl;
                cout << "Total Price for Stay: " << std::fixed << std::setprecision(2) << reservations.back().getTotalAmount() << " rupees" << std::endl;
                cout << "Advance Payment (50%): " << std::fixed << std::setprecision(2) << reservations.back().getAdvancePayment() << " rupees" << std::endl;
                cout << "Please pay the advance now.\n";
                return;
            }
        }
        cout << "Selected room is not available.\n";
    }

    void viewReservations() const {
        if (reservations.empty()) {
            cout << "No reservations available.\n";
        } else {
            cout << "\n\nAll Reservations:";
            for (const auto &reservation : reservations) {
                reservation.display();
            }
        }
    }

    bool isValidRoomNumber(int roomNumber, const vector<int> &availableRoomNumbers) {
        return find(availableRoomNumbers.begin(), availableRoomNumbers.end(), roomNumber) != availableRoomNumbers.end();
    }

    bool isValidDate(const string &date) {
        if (date.size() != 10 || date[4] != '-' || date[7] != '-') return false;
        for (int i = 0; i < date.size(); ++i) {
            if ((i != 4 && i != 7) && !isdigit(date[i])) return false;
        }
        return true;
    }

    bool isDateValidOrder(const string &checkInDate, const string &checkOutDate) {
        return calculateDays(checkInDate, checkOutDate) > 0;
    }

    void getValidDate(string &date) {
        while (true) {
            cout << "Enter date (YYYY-MM-DD): ";
            cin >> date;
            if (isValidDate(date)) {
                break;
            } else {
                cout << "Invalid date format! Please try again.\n";
            }
        }
    }

    // Function to ask the customer about meal preferences
    double askMeals() {
        int choice;
        double totalMealCost = 0;

        cout << "\nWould you like any meals during your stay? (1 for Yes, 0 for No): ";
        cin >> choice;

        if (choice == 1) {
            cout << "Meal Options:\n";
            cout << "1. Breakfast (Veg - 150, Non-Veg - 200)\n";
            cout << "2. Lunch (Veg - 300, Non-Veg - 400)\n";
            cout << "3. Dinner (Veg - 350, Non-Veg - 450)\n";

            string mealTypes[] = {"Breakfast", "Lunch", "Dinner"};
            double vegPrices[] = {150, 300, 350};
            double nonVegPrices[] = {200, 400, 450};

            for (int i = 0; i < 3; i++) {
                cout << "Would you like " << mealTypes[i] << "? (1 for Veg, 2 for Non-Veg, 0 for No): ";
                int choice;
                cin >> choice;
                if (choice == 1) {
                    totalMealCost += vegPrices[i];
                } else if (choice == 2) {
                    totalMealCost += nonVegPrices[i];
                }
            }
        }

        return totalMealCost;
    }

    // Checkout process
    void checkout(Reservation &reservation) {
        cout << "\nThank you for staying with us! Your booking ID: " << reservation.bookingID << endl;
        cout << "Total Amount: " << std::fixed << std::setprecision(2) << reservation.getTotalAmount() << " rupees" << std::endl;
        cout << "Advance Payment is done via " << std::fixed << std::setprecision(2) << reservation.customer.paymentType << " : " << reservation.getAdvancePayment() << " rupees\n"<< std::endl;
        cout << "Remaining Amount: " << std::fixed << std::setprecision(2) << reservation.getRemainingAmount() << " rupees" << std::endl;
        cout << "Please pay the remaining amount at the time of checkout.";
    }
};

// Your classes and methods remain the same...

int main() {
    Hotel hotel("MAHARAJA HOTEL", "MUMBAI");

    hotel.addRoom(1, "Single", true, true, 4000.0);  // Single, AC, Non-Smoking
    hotel.addRoom(2, "Single", true, true, 4000.0);  // Single, AC, Non-Smoking
    hotel.addRoom(3, "Single", true, true, 4000.0);  // Single, AC, Non-Smoking
    hotel.addRoom(4, "Single", true, false, 4500.0); // Single, AC, Smoking
    hotel.addRoom(5, "Single", true, false, 4500.0); // Single, AC, Smoking
    hotel.addRoom(6, "Single", false, true, 2500.0); // Single, No AC, Non-Smoking
    hotel.addRoom(7, "Single", false, true, 2500.0); // Single, No AC, Non-Smoking

    hotel.addRoom(101, "Double", true, true, 7000.0);  // Double, AC, Non-Smoking
    hotel.addRoom(102, "Double", true, true, 7000.0);  // Double, AC, Non-Smoking
    hotel.addRoom(104, "Double", true, true, 7000.0);  // Double, AC, Non-Smoking
    hotel.addRoom(105, "Double", true, true, 7000.0);  // Double, AC, Non-Smoking
    hotel.addRoom(106, "Double", false, false, 6000.0); // Double, AC, Smoking
    hotel.addRoom(107, "Double", false, false, 6000.0); // Double, AC, Smoking
    hotel.addRoom(108, "Double", false, false, 6000.0); // Double, AC, Smoking
    hotel.addRoom(109, "Double", false, true, 3900.0); // Double, No AC, Non-Smoking
    hotel.addRoom(110, "Double", false, true, 3900.0); // Double, No AC, Non-Smoking
    hotel.addRoom(111, "Double", false, false, 4500.0); // Double, No AC, Smoking

    hotel.addRoom(201, "Suite", true, true, 10000.0);   // Suite, AC, Non-Smoking
    hotel.addRoom(202, "Suite", true, false, 8500.0); // Suite, AC, Smoking
    hotel.addRoom(203, "Suite", true, false, 8500.0); // Suite, AC, Smoking
    hotel.addRoom(204, "Suite", false, true, 6500.0); // Suite, No AC, Non-Smoking
    hotel.addRoom(205, "Suite", false, true, 6500.0); // Suite, No AC, Non-Smoking
    hotel.addRoom(206, "Suite", false, true, 6500.0); // Suite, No AC, Non-Smoking
    hotel.addRoom(207, "Suite", false, true, 6500.0); // Suite, No AC, Non-Smoking
    hotel.addRoom(208, "Suite", false, false, 7000.0); // Suite, No AC, Smoking
    hotel.addRoom(209, "Suite", false, false, 7000.0); // Suite, No AC, Smoking
    hotel.addRoom(210, "Suite", false, false, 7000.0); // Suite, No AC, Smoking
    hotel.addRoom(211, "Suite", false, false, 7000.0); // Suite, No AC, Smoking
    hotel.addRoom(212, "Suite", false, false, 7000.0); // Suite, No AC, Smoking


    string name, mobileNo, paymentType;
    vector<string> validPaymentTypes = {"Credit Card", "UPI", "AmazonPay", "Bank Transfer"};

    // Input customer details
    cout << "Enter your name: ";
    getline(cin, name);  // First input as getline
    cout << "Enter your mobile number: +91 ";
    cin >> mobileNo;
    cin.ignore();  // Clear the newline character left in the buffer
    // cout << "Enter your payment type (Credit Card, UPI, AmazonPay, Bank Transfer): ";
    // getline(cin, paymentType);  // getline after cin.ignore

    bool validPayment = false;
    while (!validPayment) {
        cout << "Enter your payment type (Credit Card, UPI, AmazonPay, Bank Transfer): ";
        getline(cin, paymentType);
        if (find(validPaymentTypes.begin(), validPaymentTypes.end(), paymentType) != validPaymentTypes.end()) {
            validPayment = true;
        } else {
            cout << "Invalid payment type! Please enter a valid payment type.\n";
        }
    }

    Customer customer(name, mobileNo, paymentType);

    string roomType;
    bool hasAC, isNonSmoking;

    cout << "\nWelcome to " << hotel.name << " in " << hotel.location << endl;
    
    // Displays room charger per night 
    cout << "\n\nPlease read the room charges below carefully before check-in " << endl;
    cout << "\nAC & Non-Smoking - \nSingle : Rs 4,000 only per night \nDouble : Rs 7,000 only per night \nSuite  : Rs 10,000 only per night \n" << endl;
    cout << "AC & Smoking - \nSingle : Rs 4,500 only per night \nDouble : Rs 6,000 only per night \nSuite  : Rs 8,500 only per night \n" << endl;
    cout << "Non-AC & Non-Smoking - \nSingle : Rs 2,500 only per night \nDouble : Rs 3,900 only per night \nSuite  : Rs 6,500 only per night \n" << endl;
    cout << "Non-AC & Smoking - \nSingle : Rs 2,000 only per night \nDouble : Rs 4,500 only per night \nSuite  : Rs 7,000 only per night \n\n" << endl;
    
    cout << "Enter your preferences: Room Type (Single/Double/Suite): ";
    cin >> roomType;
    roomType = capitalize(roomType);

    cout << "AC? (1 for Yes, 0 for No): ";
    cin >> hasAC;
    cout << "Non-Smoking? (1 for Yes, 0 for No): ";
    cin >> isNonSmoking;

    vector<int> availableRoomNumbers;
    hotel.checkAvailability(roomType, hasAC, isNonSmoking, availableRoomNumbers);

    int selectedRoom;
    if (!availableRoomNumbers.empty()) {
        cout << "Select your room by entering the room number: ";
    while (true) {
        cin >> selectedRoom;

        // Validate if input is numeric
        if (cin.fail()) {
            cin.clear(); // Clear error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            cout << "Invalid input! Please enter a numeric room number: ";
            continue;
        }

        if (hotel.isValidRoomNumber(selectedRoom, availableRoomNumbers)) {
            break; // Valid room number
        } else {
            cout << "Invalid room number! Available rooms: ";
            for (int room : availableRoomNumbers) {
                cout << room << " ";
            }
            cout << "\nPlease select a valid room number: ";
        }
    }

    string checkInDate, checkOutDate;
    cout << "\nEnter check-in date (YYYY-MM-DD): ";
    hotel.getValidDate(checkInDate);
    cout << "Enter check-out date (YYYY-MM-DD): ";
    hotel.getValidDate(checkOutDate);

    while (!hotel.isDateValidOrder(checkInDate, checkOutDate)) {
        cout << "Check-out date must be after check-in date! Please enter valid dates.\n";
        cout << "Enter check-in date (YYYY-MM-DD): ";
        hotel.getValidDate(checkInDate);
        cout << "Enter check-out date (YYYY-MM-DD): ";
        hotel.getValidDate(checkOutDate);
    }

    double mealCost = hotel.askMeals();
    hotel.makeReservation(customer, selectedRoom, checkInDate, checkOutDate, 2, mealCost);
    hotel.viewReservations();

    // Checkout process for the first reservation
    hotel.checkout(hotel.reservations[0]);
} else {
    cout << "\nNo available rooms matching your preferences." << endl;
}


    return 0;
}

