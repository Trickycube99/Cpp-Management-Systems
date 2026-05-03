#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>

class Student {
public:
    int id;
    std::string name;
    float marks;
    char grade;

    void calculateGrade() {
        if (marks >= 90) grade = 'A';
        else if (marks >= 80) grade = 'B';
        else if (marks >= 70) grade = 'C';
        else if (marks >= 60) grade = 'D';
        else grade = 'F';
    }

    static void displayHeader() {
        std::cout << "\n" << std::left << std::setw(10) << "ID" 
                  << std::setw(20) << "Name" 
                  << std::setw(10) << "Marks" 
                  << "Grade" << std::endl;
        std::cout << "---------------------------------------------------" << std::endl;
    }

    void displayRecord() const {
        std::cout << std::left << std::setw(10) << id 
                  << std::setw(20) << name 
                  << std::setw(10) << marks 
                  << grade << std::endl;
    }
};

class StudentManager {
private:
    std::string fileName = "students.txt";

    void clearBuffer() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    bool idExists(int targetId) {
        std::ifstream inFile(fileName);
        if (!inFile) return false;
        int id;
        std::string line;
        while (inFile >> id) {
            if (id == targetId) return true;
            std::getline(inFile, line);
        }
        return false;
    }

public:
    void addRecord() {
        Student s;
        std::cout << "Enter ID: ";
        if (!(std::cin >> s.id)) {
            std::cout << "Invalid ID format!\n";
            std::cin.clear(); clearBuffer();
            return;
        }

        if (idExists(s.id)) {
            std::cout << "Error: Student with ID " << s.id << " already exists!\n";
            return;
        }

        clearBuffer();
        std::cout << "Enter Name: "; std::getline(std::cin, s.name);
        
        std::cout << "Enter Marks (0-100): "; 
        if (!(std::cin >> s.marks) || s.marks < 0 || s.marks > 100) {
            std::cout << "Invalid marks! Add aborted.\n";
            std::cin.clear(); clearBuffer();
            return;
        }
        
        s.calculateGrade();
        std::ofstream outFile(fileName, std::ios::app);
        if (!outFile) {
            std::cout << "Error opening file for writing!\n";
            return;
        }
        outFile << s.id << "|" << s.name << "|" << s.marks << "|" << s.grade << "\n";
        outFile.close();
        std::cout << "Record saved successfully!\n";
    }

    void viewAllRecords() {
        std::ifstream inFile(fileName);
        if (!inFile) {
            std::cerr << "Database empty or file missing.\n";
            return;
        }

        Student s;
        Student::displayHeader();
        while (inFile >> s.id) {
            inFile.ignore(std::numeric_limits<std::streamsize>::max(), '|'); 
            std::getline(inFile, s.name, '|');
            inFile >> s.marks;
            inFile.ignore(std::numeric_limits<std::streamsize>::max(), '|');
            inFile >> s.grade;
            s.displayRecord();
        }
        inFile.close();
    }

    void updateRecord(int targetId) {
        std::ifstream inFile(fileName);
        if (!inFile) {
            std::cout << "Database not found!\n";
            return;
        }

        std::vector<Student> students;
        Student s;
        bool found = false;
        bool updateSuccessful = false;

        while (inFile >> s.id) {
            inFile.ignore(1, '|');
            std::getline(inFile, s.name, '|');
            inFile >> s.marks;
            inFile.ignore(1, '|');
            inFile >> s.grade;

            if (s.id == targetId) {
                found = true;
                std::cout << "Record Found! Current Name: " << s.name << "\n";
                std::cout << "Enter New Name: ";
                clearBuffer();
                std::string newName;
                std::getline(std::cin, newName);
                
                float newMarks;
                std::cout << "Enter New Marks (0-100): ";
                if (std::cin >> newMarks && newMarks >= 0 && newMarks <= 100) {
                    s.name = newName;
                    s.marks = newMarks;
                    s.calculateGrade();
                    updateSuccessful = true;
                } else {
                    std::cout << "Invalid marks. Update rejected.\n";
                    std::cin.clear(); clearBuffer();
                }
            }
            students.push_back(s);
        }
        inFile.close();

        if (updateSuccessful) {
            std::ofstream outFile(fileName, std::ios::trunc);
            for (const auto& st : students) {
                outFile << st.id << "|" << st.name << "|" << st.marks << "|" << st.grade << "\n";
            }
            std::cout << "File successfully updated.\n";
        } else if (!found) {
            std::cout << "Record ID " << targetId << " not found.\n";
        }
    }

    void deleteRecord(int targetId) {
        std::ifstream inFile(fileName);
        if (!inFile) {
            std::cout << "Database not found!\n";
            return;
        }

        std::vector<Student> students;
        Student s;
        bool found = false;

        while (inFile >> s.id) {
            inFile.ignore(1, '|');
            std::getline(inFile, s.name, '|');
            inFile >> s.marks;
            inFile.ignore(1, '|');
            inFile >> s.grade;
            
            if (s.id == targetId) found = true; 
            else students.push_back(s);
        }
        inFile.close();

        if (found) {
            std::ofstream outFile(fileName, std::ios::trunc); 
            for (const auto& st : students) {
                outFile << st.id << "|" << st.name << "|" << st.marks << "|" << st.grade << "\n";
            }
            std::cout << "Record deleted.\n";
        } else {
            std::cout << "Record ID " << targetId << " not found.\n";
        }
    }
};

int main() {
    StudentManager manager;
    int choice;

    while (true) {
        std::cout << "\n--- Student Management System (Refined) ---\n";
        std::cout << "1. Add Student\n2. View All\n3. Update\n4. Delete\n5. Exit\n";
        std::cout << "Enter choice: ";
        
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Enter a number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: manager.addRecord(); break;
            case 2: manager.viewAllRecords(); break;
            case 3: {
                int id; std::cout << "ID to update: "; std::cin >> id;
                manager.updateRecord(id);
                break;
            }
            case 4: {
                int id; std::cout << "ID to delete: "; std::cin >> id;
                manager.deleteRecord(id);
                break;
            }
            case 5: return 0;
            default: std::cout << "Invalid option.\n";
        }
    }
    return 0;
}