#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

// -------------------- Car (Base Class) --------------------
class Car {
protected:
    int id;
    string brand;
    string model;
    double dailyRate;
    bool available;

public:
    static int totalCars;

    Car(int i, string b, string m, double r) {
        id = i;
        brand = b;
        model = m;
        dailyRate = r;
        available = true;
        totalCars++;
    }

    virtual double calculateRent(int days) { return days * dailyRate; }
    virtual string getType() { return "Car"; }

    int getID() { return id; }
    string getBrand() { return brand; }
    string getModel() { return model; }
    double getRate() { return dailyRate; }
    bool isAvailable() { return available; }

    void setAvailable(bool a) { available = a; }

    void display() {
        cout << id << " | " << brand << " " << model
             << " | Type: " << getType()
             << " | Rate: $" << dailyRate
             << " | " << (available ? "Available" : "Rented") << endl;
    }
};
int Car::totalCars = 0;

// -------------------- Derived Car Types --------------------
class SportsCar : public Car {
public:
    SportsCar(int i, string b, string m, double r) : Car(i, b, m, r) {}
    double calculateRent(int days) { return Car::calculateRent(days) * 1.15; }
    string getType() { return "Sports"; }
};

class LuxuryCar : public Car {
public:
    LuxuryCar(int i, string b, string m, double r) : Car(i, b, m, r) {}
    double calculateRent(int days) { return Car::calculateRent(days) * 1.10; }
    string getType() { return "Luxury"; }
};

class StandardCar : public Car {
public:
    StandardCar(int i, string b, string m, double r) : Car(i, b, m, r) {}
    string getType() { return "Standard"; }
};

class BaseCar : public Car {
public:
    BaseCar(int i, string b, string m, double r) : Car(i, b, m, r) {}
    string getType() { return "Base"; }
};

// -------------------- Customer --------------------
class Customer {
    string name;
    string cnic;
public:
    Customer(string n, string id) {
        name = n;
        cnic = id;
    }
    string getName() { return name; }
    string getCNIC() { return cnic; }
};

// -------------------- Booking --------------------
class Booking {
public:
    int carID;
    string customerName;
    string customerCNIC;
    int days;
    double amount;
};

// -------------------- Rental System --------------------
class RentalSystem {
    vector<Car*> cars;
    vector<Booking> bookings;

public:
    void addCar(Car* c) { cars.push_back(c); }

    void addNewCar() {
        int id = cars.size() + 1;
        string brand, model, type;
        double rate;

        cout << "\nEnter Car Type (Sports/Luxury/Standard/Base): ";
        cin >> type;
        cout << "Enter Car Brand: ";
        cin >> brand;
        cout << "Enter Car Model: ";
        cin.ignore();
        getline(cin, model);
        cout << "Enter Daily Rate ($): ";
        cin >> rate;

        if (type == "Sports" || type == "sports")
            cars.push_back(new SportsCar(id, brand, model, rate));
        else if (type == "Luxury" || type == "luxury")
            cars.push_back(new LuxuryCar(id, brand, model, rate));
        else if (type == "Standard" || type == "standard")
            cars.push_back(new StandardCar(id, brand, model, rate));
        else
            cars.push_back(new BaseCar(id, brand, model, rate));

        cout << "✅ New car added successfully!\n";
    }

    void viewCars() {
        cout << "\n--- Available Cars ---\n";
        for (auto c : cars)
            c->display();
    }

    bool isCarRentedByCustomer(string cnic) {
        for (auto &b : bookings)
            if (b.customerCNIC == cnic)
                return true;
        return false;
    }

    void rentCar(string customerName, string customerCNIC) {
        if (isCarRentedByCustomer(customerCNIC)) {
            cout << "⚠ You already have a rented car. Please return it or change it.\n";
            return;
        }

        int id, days;
        cout << "Enter Car ID to rent: ";
        cin >> id;

        Car* chosen = NULL;
        for (auto c : cars) {
            if (c->getID() == id) {
                chosen = c;
                break;
            }
        }

        if (chosen == NULL) {
            cout << "❌ Car not found!\n";
            return;
        }
        if (!chosen->isAvailable()) {
            cout << "❌ Sorry, this car is already rented.\n";
            return;
        }

        cout << "Enter number of days: ";
        cin >> days;
        if (days <= 0) {
            cout << "❌ Invalid number of days!\n";
            return;
        }

        double total = chosen->calculateRent(days);
        chosen->setAvailable(false);

        Booking b;
        b.carID = id;
        b.customerName = customerName;
        b.customerCNIC = customerCNIC;
        b.days = days;
        b.amount = total;
        bookings.push_back(b);

        ofstream file("C:\\Users\\LTC\\OneDrive\\Desktop\\BSCS\\demo\\my programs\\rental_log.txt", ios::app);
        file << "Customer: " << customerName
             << " (CNIC: " << customerCNIC << ")"
             << ", Car: " << chosen->getBrand() << " " << chosen->getModel()
             << ", Car ID: " << id
             << ", Days: " << days
             << ", Amount: $" << total << endl;
        file.close();

        cout << "✅ Car rented successfully! Total rent = $" << total << "\n";
    }

    void changeCar(string customerCNIC) {
        int newID;
        bool found = false;

        // Find current booking
        for (auto &b : bookings) {
            if (b.customerCNIC == customerCNIC) {
                found = true;

                // Make old car available
                for (auto c : cars)
                    if (c->getID() == b.carID)
                        c->setAvailable(true);

                cout << "Enter new Car ID to rent: ";
                cin >> newID;

                Car* newCar = NULL;
                for (auto c : cars)
                    if (c->getID() == newID)
                        newCar = c;

                if (newCar == NULL) {
                    cout << "❌ New car not found!\n";
                    return;
                }
                if (!newCar->isAvailable()) {
                    cout << "❌ That car is already rented.\n";
                    return;
                }

                int newDays;
                cout << "Enter number of days: ";
                cin >> newDays;

                b.carID = newID;
                b.days = newDays;
                b.amount = newCar->calculateRent(newDays);

                newCar->setAvailable(false);
                cout << "✅ Car changed successfully! New total: $" << b.amount << "\n";
                return;
            }
        }
        if (!found) cout << "⚠ No active booking found for your CNIC.\n";
    }

    void returnCar(string cnic) {
        bool found = false;
        for (size_t i = 0; i < bookings.size(); i++) {
            if (bookings[i].customerCNIC == cnic) {
                for (auto c : cars)
                    if (c->getID() == bookings[i].carID)
                        c->setAvailable(true);

                bookings.erase(bookings.begin() + i);
                cout << "✅ Car returned successfully!\n";
                found = true;
                break;
            }
        }
        if (!found) cout << "❌ No booking found under your CNIC.\n";
    }

    void viewBookings() {
        cout << "\n--- All Bookings ---\n";
        for (auto &b : bookings)
            cout << "Customer: " << b.customerName
                 << " | CNIC: " << b.customerCNIC
                 << " | Car ID: " << b.carID
                 << " | Days: " << b.days
                 << " | Amount: $" << b.amount << endl;
    }
};

// -------------------- Main --------------------
int main() {
    RentalSystem system;

    // -------------------- Preloaded Cars --------------------
    system.addCar(new SportsCar(1, "Ferrari", "488 GTB", 1200));
    system.addCar(new SportsCar(2, "Lamborghini", "Huracan EVO", 1500));
    system.addCar(new SportsCar(3, "Porsche", "911 Turbo S", 1000));

    // Hyper Cars
    system.addCar(new SportsCar(4, "Bugatti", "Chiron Super Sport", 5000));
    system.addCar(new SportsCar(5, "McLaren", "Speedtail", 4500));
    system.addCar(new SportsCar(6, "Koenigsegg ", "Gemera", 4500));

    // Luxury Cars
    system.addCar(new LuxuryCar(7, "Mercedes-Benz", "S-Class", 800));
    system.addCar(new LuxuryCar(8, "BMW", "7 Series", 750));
    system.addCar(new LuxuryCar(9, "Rolls-Royce", "Ghost", 2500));
    system.addCar(new LuxuryCar(10, "Bentley", "Continental GT", 2000));

    // Mid-Range Cars
    system.addCar(new StandardCar(11, "Toyota", "Camry", 150));
    system.addCar(new StandardCar(12, "Honda", "Accor", 140));
    system.addCar(new StandardCar(13, "Hyundai", "Sonata", 130));

    // SUVs
    system.addCar(new StandardCar(14, "Toyota", "Land Cruiser", 200));
    system.addCar(new StandardCar(15, "Range Rover", "Autobiography", 600));
    system.addCar(new StandardCar(16, "Kia", "Sportage", 180));
    system.addCar(new LuxuryCar(17, "Mercedes-Benz", "G wagon", 700));

    // Pickup Trucks
    system.addCar(new BaseCar(18, "Ford", "F-150 Raptor", 250));
    system.addCar(new BaseCar(19, "Toyota", "Hilux", 220));
    system.addCar(new BaseCar(20, "RAM", "1500", 300));

    // Normal Cars (Economy)
    system.addCar(new BaseCar(21, "Suzuki", "Alto", 80));
    system.addCar(new BaseCar(22, "Toyota", "Yaris", 100));
    system.addCar(new BaseCar(23, "Honda", "civic", 110));
    system.addCar(new BaseCar(24, "Toyota", "Raize", 90));
    // -------------------- Program Menu --------------------
    int roleChoice;
    cout << "===== Welcome to Car Rental System =====\n";
    cout << "1. Admin Login\n";
    cout << "2. Customer Login\n";
    cout << "Choose your role: ";
    cin >> roleChoice;

    if (roleChoice == 1) {
        string user, pass;
        cout << "Enter Admin Username: ";
        cin >> user;
        cout << "Enter Admin Password: ";
        cin >> pass;

        if (user == "admin" && pass == "1234") {
            int choice;
            do {
                cout << "\n===== Admin Panel =====\n";
                cout << "1. View All Cars\n";
                cout << "2. Add New Car\n";
                cout << "3. View All Bookings\n";
                cout << "4. Exit\n";
                cout << "Choice: ";
                cin >> choice;

                if (choice == 1) system.viewCars();
                else if (choice == 2) system.addNewCar();
                else if (choice == 3) system.viewBookings();
            } while (choice != 4);
        } else {
            cout << "❌ Invalid login credentials.\n";
        }
    } 
    else if (roleChoice == 2) {
        cin.ignore();
        string name, cnic;
        cout << "Enter your name: ";
        getline(cin, name);
        cout << "Enter your CNIC: ";
        getline(cin, cnic);

        int choice;
        do {
            cout << "\n===== Customer Panel =====\n";
            cout << "1. View Cars\n";
            cout << "2. Rent a Car\n";
            cout << "3. Return a Car\n";
            cout << "4. Change Car\n";
            cout << "5. View Bookings\n";
            cout << "6. Exit\n";
            cout << "Choice: ";
            cin >> choice;

            if (choice == 1) system.viewCars();
            else if (choice == 2) system.rentCar(name, cnic);
            else if (choice == 3) system.returnCar(cnic);
            else if (choice == 4) system.changeCar(cnic);
            else if (choice == 5) system.viewBookings();
        } while (choice != 6);
    }

   cout<<"Existing system.GOODBYE\n";
    return 0;
}
