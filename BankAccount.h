#ifndef BANK_ACCOUNT_H
#define BANK_ACCOUNT_H

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <iomanip>

class BankAccount {
protected:
    double balance;
    std::mutex mtx; //this is my primary mutex for protecting the account data
    std::condition_variable cv; // for signaling
    int total_transactions;
    std::mutex cout_mutex; // Separate mutex for cout operations

public:
    BankAccount() : balance(0), total_transactions(0) {}

    virtual void deposit(int amount, int customer_id) {
        //this lock is for potential waiting conditions and to protect the account data
        std::lock_guard<std::mutex> lock(mtx);
        balance += amount;
        total_transactions++;
        
        // Using a separate mutex for cout to prevent interleaved output
        std::lock_guard<std::mutex> cout_lock(cout_mutex);
        std::cout << "Transaction #" << std::setw(2) << total_transactions 
                 << ": Customer " << customer_id << " deposited $" << std::setw(3) << amount
                 << ". New balance: $" << std::setw(3) << balance << std::endl;
        cv.notify_all();
    }

    virtual void withdraw(int amount, int customer_id) {
        std::unique_lock<std::mutex> lock(mtx);
        
        balance -= amount;
        total_transactions++;
        
        // Use a separate mutex for cout to prevent interleaved output
        std::lock_guard<std::mutex> cout_lock(cout_mutex);
        std::cout << "Transaction #" << std::setw(2) << total_transactions 
                 << ": Customer " << customer_id << " withdrew $" << std::setw(3) << amount
                 << ". New balance: $" << std::setw(3) << balance << std::endl;
    }

    double get_balance(){
        // method to retrieve current balance
        std::lock_guard<std::mutex> lock(mtx);
        return balance;
    }

    int get_total_transactions() {
        // method to retrieve current balance
        std::lock_guard<std::mutex> lock(mtx);
        return total_transactions;
    }
};

#endif