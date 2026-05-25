/*
    ============================================
    MINI BANKING SYSTEM - C++ OOP Project
    Made by: [Omkar Sharma]
    Course: BCA
    ============================================
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

// ─── Transaction Class ───────────────────────────────────────────────────────
class Transaction {
public:
    string type;    // "Credit" or "Debit"
    double amount;
    string note;

    Transaction(string t, double a, string n) {
        type = t;
        amount = a;
        note = n;
    }

    void display() const {
        cout << "  [" << type << "] Rs." << fixed << setprecision(2) << amount
             << "  (" << note << ")" << endl;
    }
};

// ─── Account Base Class ───────────────────────────────────────────────────────
class Account {
protected:
    int accountNumber;
    string holderName;
    double balance;
    string pin;
    vector<Transaction> history;

public:
    Account(int accNo, string name, double initBalance, string userPin) {
        accountNumber = accNo;
        holderName    = name;
        balance       = initBalance;
        pin           = userPin;
        history.push_back(Transaction("Credit", initBalance, "Opening Balance"));
    }

    virtual string getType() const = 0;  // Pure virtual - child class define karegi

    int getAccountNumber() const { return accountNumber; }
    string getName()       const { return holderName; }
    double getBalance()    const { return balance; }
    bool checkPin(string p) const { return pin == p; }

    void deposit(double amount) {
        if (amount <= 0) {
            cout << "  Error: Invalid amount!\n";
            return;
        }
        balance += amount;
        history.push_back(Transaction("Credit", amount, "Deposit"));
        cout << "  Rs." << fixed << setprecision(2) << amount << " deposited successfully!\n";
    }

    virtual bool withdraw(double amount) {
        if (amount <= 0) {
            cout << "  Error: Invalid amount!\n";
            return false;
        }
        if (amount > balance) {
            cout << "  Error: Insufficient balance!\n";
            return false;
        }
        balance -= amount;
        history.push_back(Transaction("Debit", amount, "Withdrawal"));
        cout << "  Rs." << fixed << setprecision(2) << amount << " withdrawn successfully!\n";
        return true;
    }

    void showDetails() const {
        cout << "\n  +---------------------------------+\n";
        cout << "  | Account Number : " << accountNumber << endl;
        cout << "  | Name           : " << holderName << endl;
        cout << "  | Account Type   : " << getType() << endl;
        cout << "  | Balance        : Rs." << fixed << setprecision(2) << balance << endl;
        cout << "  +---------------------------------+\n";
    }

    void showHistory() const {
        cout << "\n  --- Transaction History ---\n";
        if (history.empty()) {
            cout << "  No transactions yet.\n";
            return;
        }
        for (const Transaction& t : history) {
            t.display();
        }
    }

    // File mein save karne ke liye
    void saveToFile() const {
        ofstream file("account_" + to_string(accountNumber) + ".txt");
        file << accountNumber << "\n";
        file << holderName    << "\n";
        file << getType()     << "\n";
        file << balance       << "\n";
        file << pin           << "\n";
        file << "--- Transactions ---\n";
        for (const Transaction& t : history) {
            file << t.type << " | " << t.amount << " | " << t.note << "\n";
        }
        file.close();
        cout << "  Account data saved!\n";
    }
};

// ─── Savings Account (inherits Account) ──────────────────────────────────────
class SavingsAccount : public Account {
private:
    double interestRate;

public:
    SavingsAccount(int accNo, string name, double initBal, string pin)
        : Account(accNo, name, initBal, pin) {
        interestRate = 0.04;  // 4% annual interest
    }

    string getType() const override { return "Savings"; }

    void addInterest() {
        double interest = balance * interestRate;
        balance += interest;
        history.push_back(Transaction("Credit", interest, "Annual Interest (4%)"));
        cout << "  Interest of Rs." << fixed << setprecision(2) << interest << " added!\n";
    }
};

// ─── Current Account (inherits Account) ──────────────────────────────────────
class CurrentAccount : public Account {
private:
    double overdraftLimit;

public:
    CurrentAccount(int accNo, string name, double initBal, string pin)
        : Account(accNo, name, initBal, pin) {
        overdraftLimit = 5000.0;  // Rs. 5000 overdraft allowed
    }

    string getType() const override { return "Current"; }

    // Overdraft allowed - withdraw() override karte hain
    bool withdraw(double amount) override {
        if (amount <= 0) {
            cout << "  Error: Invalid amount!\n";
            return false;
        }
        if (amount > balance + overdraftLimit) {
            cout << "  Error: Exceeds overdraft limit!\n";
            return false;
        }
        balance -= amount;
        history.push_back(Transaction("Debit", amount, "Withdrawal"));
        cout << "  Rs." << fixed << setprecision(2) << amount << " withdrawn.\n";
        if (balance < 0)
            cout << "  Warning: Overdraft used! Current balance: Rs." << balance << "\n";
        return true;
    }
};

// ─── Bank Class (manages all accounts) ───────────────────────────────────────
class Bank {
private:
    vector<Account*> accounts;
    int nextAccNumber;

    Account* findAccount(int accNo) {
        for (Account* acc : accounts) {
            if (acc->getAccountNumber() == accNo)
                return acc;
        }
        return nullptr;
    }

public:
    Bank() { nextAccNumber = 1001; }

    ~Bank() {
        for (Account* acc : accounts)
            delete acc;
    }

    void createSavingsAccount() {
        string name, pin;
        double initBal;
        cout << "\n  Enter your name     : "; cin.ignore(); getline(cin, name);
        cout << "  Set a 4-digit PIN   : "; cin >> pin;
        cout << "  Opening balance (Rs.): "; cin >> initBal;
        if (initBal < 500) {
            cout << "  Error: Minimum opening balance is Rs.500\n";
            return;
        }
        SavingsAccount* acc = new SavingsAccount(nextAccNumber, name, initBal, pin);
        accounts.push_back(acc);
        cout << "  Account created! Your Account Number: " << nextAccNumber << "\n";
        nextAccNumber++;
    }

    void createCurrentAccount() {
        string name, pin;
        double initBal;
        cout << "\n  Enter your name      : "; cin.ignore(); getline(cin, name);
        cout << "  Set a 4-digit PIN    : "; cin >> pin;
        cout << "  Opening balance (Rs.): "; cin >> initBal;
        if (initBal < 1000) {
            cout << "  Error: Minimum opening balance is Rs.1000\n";
            return;
        }
        CurrentAccount* acc = new CurrentAccount(nextAccNumber, name, initBal, pin);
        accounts.push_back(acc);
        cout << "  Account created! Your Account Number: " << nextAccNumber << "\n";
        nextAccNumber++;
    }

    Account* login() {
        int accNo; string pin;
        cout << "\n  Enter Account Number : "; cin >> accNo;
        cout << "  Enter PIN            : "; cin >> pin;
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "  Error: Account not found!\n"; return nullptr; }
        if (!acc->checkPin(pin)) { cout << "  Error: Wrong PIN!\n"; return nullptr; }
        cout << "  Login successful! Welcome, " << acc->getName() << "\n";
        return acc;
    }
};

// ─── Account Menu ─────────────────────────────────────────────────────────────
void accountMenu(Account* acc) {
    int choice;
    do {
        cout << "\n  === ACCOUNT MENU ===\n";
        cout << "  1. View Details\n";
        cout << "  2. Deposit\n";
        cout << "  3. Withdraw\n";
        cout << "  4. Transaction History\n";
        cout << "  5. Save to File\n";
        cout << "  0. Logout\n";
        cout << "  Choice: "; cin >> choice;

        switch (choice) {
        case 1: acc->showDetails(); break;
        case 2: {
            double amt;
            cout << "  Enter amount to deposit: Rs."; cin >> amt;
            acc->deposit(amt);
            break;
        }
        case 3: {
            double amt;
            cout << "  Enter amount to withdraw: Rs."; cin >> amt;
            acc->withdraw(amt);
            break;
        }
        case 4: acc->showHistory(); break;
        case 5: acc->saveToFile(); break;
        case 0: cout << "  Logged out. Goodbye!\n"; break;
        default: cout << "  Invalid choice!\n";
        }
    } while (choice != 0);
}

// ─── Main Function ────────────────────────────────────────────────────────────
int main() {
    Bank bank;
    int choice;

    cout << "\n  =============================================\n";
    cout << "       WELCOME TO MINI BANKING SYSTEM\n";
    cout << "  =============================================\n";

    do {
        cout << "\n  === MAIN MENU ===\n";
        cout << "  1. Open Savings Account\n";
        cout << "  2. Open Current Account\n";
        cout << "  3. Login to My Account\n";
        cout << "  0. Exit\n";
        cout << "  Choice: "; cin >> choice;

        switch (choice) {
        case 1: bank.createSavingsAccount(); break;
        case 2: bank.createCurrentAccount(); break;
        case 3: {
            Account* acc = bank.login();
            if (acc) accountMenu(acc);
            break;
        }
        case 0: cout << "\n  Thank you for using Mini Banking System!\n\n"; break;
        default: cout << "  Invalid choice!\n";
        }
    } while (choice != 0);

    return 0;
}
