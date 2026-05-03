#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <sstream> // Essential for stringstream line parsing

class Person {
public:
    int id;
    std::string name;
    Person(int i = 0, std::string n = "") : id(i), name(n) {}
};

class Patient : public Person {
public:
    int age;
    std::string ailment;
    Patient(int i = 0, std::string n = "", int a = 0, std::string al = "") 
        : Person(i, n), age(a), ailment(al) {}
};

class Doctor : public Person {
public:
    std::string specialty;
    double fees;
    Doctor(int i = 0, std::string n = "", std::string s = "", double f = 0.0) 
        : Person(i, n), specialty(s), fees(f) {}
};

class HospitalManager {
private:
    std::string patientFile = "patients.txt";
    std::string doctorFile = "doctors.txt";
    std::string appointmentFile = "appointments.txt";

    void clearBuffer() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

public:
    // --- Doctor Management ---
    void addDoctor() {
        int id;
        std::string name, spec;
        double fees;

        std::cout << "Enter Doctor ID: "; std::cin >> id;
        clearBuffer();
        std::cout << "Enter Name: Dr. "; std::getline(std::cin, name);
        std::cout << "Enter Specialty: "; std::getline(std::cin, spec);
        std::cout << "Enter Consultation Fees: "; std::cin >> fees;

        std::ofstream df(doctorFile, std::ios::app);
        df << id << "|" << name << "|" << spec << "|" << fees << "\n";
        df.close();
        std::cout << "Doctor record added!\n";
    }

    void viewDoctors() {
        std::ifstream df(doctorFile);
        if (!df) { std::cout << "No doctors registered.\n"; return; }
        
        std::string line;
        std::cout << "\n--- Registered Doctors ---\n";
        std::cout << std::left << std::setw(10) << "ID" << std::setw(20) << "Name" 
                  << std::setw(20) << "Specialty" << "Fees" << std::endl;
        
        while (std::getline(df, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string id, name, spec, fees;
            
            std::getline(ss, id, '|');
            std::getline(ss, name, '|');
            std::getline(ss, spec, '|');
            std::getline(ss, fees, '|');
            
            std::cout << std::left << std::setw(10) << id << std::setw(20) << name 
                      << std::setw(20) << spec << "$" << fees << std::endl;
        }
        df.close();
    }

    // --- Patient Management ---
    void registerPatient() {
        int id, age;
        std::string name, ailment;
        std::cout << "Enter Patient ID: "; std::cin >> id;
        clearBuffer();
        std::cout << "Enter Name: "; std::getline(std::cin, name);
        std::cout << "Enter Age: "; std::cin >> age;
        clearBuffer();
        std::cout << "Enter Ailment: "; std::getline(std::cin, ailment);

        std::ofstream pf(patientFile, std::ios::app);
        pf << id << "|" << name << "|" << age << "|" << ailment << "\n";
        pf.close();
        std::cout << "Patient registered successfully!\n";
    }

    // --- Appointment & Billing ---
    void bookAppointment() {
        int dId;
        std::string date, pId;
        std::string targetDName;
        double targetDFees = 0.0;
        bool docFound = false;

        viewDoctors();
        std::cout << "\nEnter Doctor ID to book with: "; std::cin >> dId;
        
        std::ifstream df(doctorFile);
        std::string line;
        while (std::getline(df, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string idStr, nameStr, specStr, feesStr;
            
            std::getline(ss, idStr, '|');
            std::getline(ss, nameStr, '|');
            std::getline(ss, specStr, '|');
            std::getline(ss, feesStr, '|');
            
            if (std::stoi(idStr) == dId) {
                targetDName = nameStr;
                targetDFees = std::stod(feesStr);
                docFound = true;
                break;
            }
        }
        df.close();

        if (!docFound) {
            std::cout << "Doctor not found!\n";
            return;
        }

        std::cout << "Enter Patient ID: "; std::cin >> pId;
        std::cout << "Enter Date (DD-MM-YYYY): "; std::cin >> date;

        double totalBill = targetDFees + 100.0; // Service Charge

        std::ofstream af(appointmentFile, std::ios::app);
        af << pId << "|" << targetDName << "|" << date << "|" << totalBill << "\n";
        af.close();

        std::cout << "\n--- Appointment Booked ---\n";
        std::cout << "Consultant: Dr. " << targetDName << "\nTotal Fees: $" << std::fixed << std::setprecision(2) << totalBill << "\n";
    }

    // --- Reports Generation ---
    void generateReport() {
        std::ifstream af(appointmentFile);
        if (!af) { 
            std::cout << "No data available for reports.\n"; 
            return; 
        }

        int count = 0;
        double totalRevenue = 0;
        std::string line;

        std::cout << "\n========== HOSPITAL SUMMARY REPORT ==========\n";
        std::cout << std::left << std::setw(15) << "Patient ID" 
                  << std::setw(20) << "Doctor" 
                  << std::setw(15) << "Date" 
                  << "Bill Amount" << std::endl;
        std::cout << "------------------------------------------------------------\n";

        while (std::getline(af, line)) {
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string pId, dName, date, billStr;

            // Extract each part separated by '|'
            std::getline(ss, pId, '|');
            std::getline(ss, dName, '|');
            std::getline(ss, date, '|');
            std::getline(ss, billStr, '|');

            if (!billStr.empty()) {
                try {
                    double billValue = std::stod(billStr); 
                    
                    std::cout << std::left << std::setw(15) << pId 
                              << std::setw(20) << dName 
                              << std::setw(15) << date 
                              << "$" << std::fixed << std::setprecision(2) << billValue << std::endl;
                    
                    totalRevenue += billValue;
                    count++;
                } catch (...) {
                    // Skip malformed conversion lines
                    continue;
                }
            }
        }
        af.close();

        std::cout << "------------------------------------------------------------\n";
        std::cout << "Total Appointments: " << count << std::endl;
        std::cout << "Total Revenue Generated: $" << std::fixed << std::setprecision(2) << totalRevenue << std::endl;
        std::cout << "=============================================\n";
    }
};

int main() {
    HospitalManager hospital;
    int choice;

    while (true) {
        std::cout << "\n--- Hospital Information System ---\n";
        std::cout << "1. Manage Doctors (Add/View)\n";
        std::cout << "2. Register New Patient\n";
        std::cout << "3. Book Appointment & Generate Bill\n";
        std::cout << "4. View Full Hospital Report\n";
        std::cout << "5. Exit\n";
        std::cout << "Selection: ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: {
                int subChoice;
                std::cout << "1. Add Doctor\n2. View Doctors\nChoice: ";
                std::cin >> subChoice;
                if (subChoice == 1) hospital.addDoctor();
                else hospital.viewDoctors();
                break;
            }
            case 2: hospital.registerPatient(); break;
            case 3: hospital.bookAppointment(); break;
            case 4: hospital.generateReport(); break;
            case 5: return 0;
            default: std::cout << "Invalid choice.\n";
        }
    }
    return 0;
}
