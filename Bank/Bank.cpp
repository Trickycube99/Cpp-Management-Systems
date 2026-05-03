#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <ctime> // For transaction timestamps

class BankAccount {
private:
    int accountNumber;
    std::string accountHolder;
    double balance;

public:
    // Constructor
    BankAccount(int accNo = 0, std::string name = "", double bal = 0.0) 
        : accountNumber(accNo), accountHolder(name), balance(bal) {}

    // Destructor - Explicitly required by project goals
    ~BankAccount() {
        // In a real system, you might log the object destruction for security audits
    }

    // Getters
    int getAccountNumber() const { return accountNumber; }
    std::string getHolderName() const { return accountHolder; }
    double getBalance() const { return balance; }

    void deposit(double amount) {
        if (amount > 0) balance += amount;
    }

    bool withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            return true;
        }
        return false;
    }

    std::string toFileData() const {
        return std::to_string(accountNumber) + "|" + accountHolder + "|" + std::to_string(balance);
    }
};

class AccountManager {
private:
    std::string fileName = "bank_data.txt";
    std::string logFile = "transactions.txt";

    void clearBuffer() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Logs transactions to a separate file
    void logTransaction(int accNo, std::string type, double amount) {
        std::ofstream log(logFile, std::ios::app);
        time_t now = time(0);
        char* dt = ctime(&now);
        // Remove trailing newline from ctime
        std::string timestamp(dt);
        if (!timestamp.empty()) timestamp.pop_back();

        log << "[" << timestamp << "] Acc: " << accNo << " | " << type << ": $" << amount << "\n";
        log.close();
    }

    bool accountExists(int accNo) {
        std::ifstream file(fileName);
        if (!file) return false;
        int currentID;
        std::string line;
        while (file >> currentID) {
            if (currentID == accNo) return true;
            std::getline(file, line);
        }
        return false;
    }

public:
    // Destructor for the Manager
    ~AccountManager() {
        std::cout << "\n[System] Saving state and closing bank database safely..." << std::endl;
    }

    void createAccount() {
        int accNo;
        std::string name;
        double initialDeposit;

        std::cout << "Enter Account Number: ";
        if (!(std::cin >> accNo) || accountExists(accNo)) {
            std::cout << "Invalid or Duplicate Account Number!\n";
            std::cin.clear(); clearBuffer();
            return;
        }

        clearBuffer();
        std::cout << "Enter Account Holder Name: ";
        std::getline(std::cin, name);
        
        std::cout << "Enter Initial Deposit: ";
        if (!(std::cin >> initialDeposit) || initialDeposit < 0) {
            std::cout << "Invalid Amount!\n";
            std::cin.clear(); clearBuffer();
            return;
        }

        std::ofstream outFile(fileName, std::ios::app);
        outFile << accNo << "|" << name << "|" << initialDeposit << "\n";
        outFile.close();
        
        logTransaction(accNo, "Account Created", initialDeposit);
        std::cout << "Account created successfully!\n";
    }

    void balanceInquiry(int accNo) {
        std::ifstream inFile(fileName);
        int id;
        std::string name;
        double bal;
        bool found = false;

        while (inFile >> id) {
            inFile.ignore(1, '|');
            std::getline(inFile, name, '|');
            inFile >> bal;
            if (id == accNo) {
                std::cout << "\n--- Account Details ---\n";
                std::cout << "Holder: " << name << "\nBalance: $" << std::fixed << std::setprecision(2) << bal << "\n";
                found = true;
                break;
            }
        }
        if (!found) std::cout << "Account not found!\n";
    }

    void performTransaction(int accNo, int type) {
        std::ifstream inFile(fileName);
        if (!inFile) return;

        std::vector<BankAccount> accounts;
        int id;
        std::string name;
        double bal;
        bool found = false;
        double transAmount = 0;

        while (inFile >> id) {
            inFile.ignore(1, '|');
            std::getline(inFile, name, '|');
            inFile >> bal;
            
            BankAccount acc(id, name, bal);
            if (id == accNo) {
                found = true;
                if (type == 1) { // Deposit
                    std::cout << "Enter Deposit Amount: ";
                    std::cin >> transAmount;
                    acc.deposit(transAmount);
                    logTransaction(accNo, "Deposit", transAmount);
                } else if (type == 2) { // Withdrawal
                    std::cout << "Enter Withdrawal Amount: ";
                    std::cin >> transAmount;
                    if (acc.withdraw(transAmount)) {
                        logTransaction(accNo, "Withdrawal", transAmount);
                    } else {
                        std::cout << "Insufficient funds!\n";
                        transAmount = 0; // Prevent log update
                    }
                }
            }
            accounts.push_back(acc);
        }
        inFile.close();

        if (found && transAmount > 0) {
            std::ofstream outFile(fileName, std::ios::trunc);
            for (const auto& a : accounts) outFile << a.toFileData() << "\n";
            std::cout << "Transaction successful.\n";
        }
    }

    void deleteAccount(int accNo) {
        std::ifstream inFile(fileName);
        std::vector<BankAccount> accounts;
        int id;
        std::string name;
        double bal;
        bool found = false;

        while (inFile >> id) {
            inFile.ignore(1, '|');
            std::getline(inFile, name, '|');
            inFile >> bal;
            if (id == accNo) found = true;
            else accounts.push_back(BankAccount(id, name, bal));
        }
        inFile.close();

        if (found) {
            std::ofstream outFile(fileName, std::ios::trunc);
            for (const auto& a : accounts) outFile << a.toFileData() << "\n";
            logTransaction(accNo, "Account Deleted", 0);
            std::cout << "Account closed successfully.\n";
        } else {
            std::cout << "Account not found.\n";
        }
    }
};

int main() {
    AccountManager manager;
    int choice;

    while (true) {
        std::cout << "\n--- C++ Secure Banking System ---\n";
        std::cout << "1. Open Account\n2. Deposit\n3. Withdraw\n";
        std::cout << "4. Balance Inquiry\n5. Close Account\n6. Exit\n";
        std::cout << "Choice: ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (choice == 6) break;

        int acc;
        if (choice >= 2 && choice <= 5) {
            std::cout << "Enter Account Number: ";
            std::cin >> acc;
        }

        switch (choice) {
            case 1: manager.createAccount(); break;
            case 2: manager.performTransaction(acc, 1); break;
            case 3: manager.performTransaction(acc, 2); break;
            case 4: manager.balanceInquiry(acc); break;
            case 5: manager.deleteAccount(acc); break;
            default: std::cout << "Invalid choice.\n";
        }
    }
    return 0;
}