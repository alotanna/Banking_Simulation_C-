#ifndef SAVINGS_ACCOUNT_H
#define SAVINGS_ACCOUNT_H

#include "BankAccount.h"
#include "TransactionLog.h"
#include <iomanip>
#include <sstream>

class SavingsAccount : public BankAccount {
private:
    double interest_rate;
    double minimum_balance;
    TransactionLog* logger;  // Add logger pointer to help me log the details from the account 

public:
// Default values of  5% interest rate, $100 minimum balance
    SavingsAccount(double rate = 0.05, double min_balance = 100.0) 
        : interest_rate(rate), minimum_balance(min_balance), logger(nullptr) {}
    
    void set_logger(TransactionLog* log) {
        logger = log;
    }

    // Override withdraw method to add minimum balance requirement
   void withdraw(int amount, int customer_id) override {
        std::unique_lock<std::mutex> lock(mtx);
        
        if (balance - amount < minimum_balance) {
            if (logger) {
                std::stringstream error_msg;
                error_msg << std::fixed << std::setprecision(2)
                         << "Savings Withdrawal Blocked: Customer " << customer_id 
                         << " attempted to withdraw $" << amount 
                         << ". Current balance: $" << balance 
                         << ", Minimum balance requirement: $" << minimum_balance;
                logger->log(error_msg.str());
            }
            
            std::lock_guard<std::mutex> cout_lock(cout_mutex);
            std::cout << "Withdrawal blocked: Insufficient funds for Customer " 
                     << customer_id << std::endl;
            return;
        }
        
        balance -= amount;
        total_transactions++;
        
        if (logger) {
            std::stringstream success_msg;
            success_msg << std::fixed << std::setprecision(2)
                       << "Savings Withdrawal: Customer " << customer_id 
                       << ", Amount: $" << amount 
                       << ", New Balance: $" << balance;
            logger->log(success_msg.str());
        }
        
         // Display transaction details to the console
        std::lock_guard<std::mutex> cout_lock(cout_mutex);
        std::cout << "Transaction #" << std::setw(2) << total_transactions 
                 << ": Customer " << customer_id << " withdrew $" << std::setw(3) << amount
                 << ". New balance: $" << std::setw(3) << balance << std::endl;
        
        cv.notify_all();
    }

void deposit(int amount, int customer_id) override {
        std::lock_guard<std::mutex> lock(mtx);
        balance += amount;
        total_transactions++;
        
        if (logger) {
            std::stringstream formatted_msg;
            formatted_msg << std::fixed << std::setprecision(2)
                        << "Deposit: Customer " << customer_id 
                        << ", Amount: $" << amount
                        << ", New Balance: $" << balance;
            logger->log(formatted_msg.str());
        }
        
        std::lock_guard<std::mutex> cout_lock(cout_mutex);
        std::cout << "Transaction #" << std::setw(2) << total_transactions 
                 << ": Customer " << customer_id << " deposited $" << std::setw(3) << amount
                 << ". New balance: $" << std::setw(3) << balance << std::endl;
        
        cv.notify_all();
    }

  // Apply monthly interest to the account if minimum balance is maintained
    double apply_monthly_interest(int customer_id) {
        std::lock_guard<std::mutex> lock(mtx);
        
        double interest = 0.0;
        if (balance >= minimum_balance) {
            interest = balance * interest_rate;
            balance += interest;
            total_transactions++;
            
            std::lock_guard<std::mutex> cout_lock(cout_mutex);
            std::cout << "Interest Applied: $" << std::fixed << std::setprecision(2) << interest 
                      << " to account by Customer " << customer_id 
                      << ". New balance: $" << balance << std::endl;
        }
        
        return interest;
    }

    double get_interest_rate() const {
        return interest_rate;
    }

    double get_minimum_balance() const {
        return minimum_balance;
    }
};

#endif