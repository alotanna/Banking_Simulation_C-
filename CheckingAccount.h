#ifndef CHECKING_ACCOUNT_H
#define CHECKING_ACCOUNT_H

#include "BankAccount.h"
#include "TransactionLog.h"
#include <sstream>
#include <iomanip>

// CheckingAccount class extends BankAccount with overdraft protection
class CheckingAccount : public BankAccount {
private:
    double overdraft_limit;// this is the overdraft for checking accounts
    TransactionLog* logger; // I am adding a logger pointer here to be able to log the trasactions independently

public:
 // Default overdraft limit is $100
    CheckingAccount(double limit = 100.0) : overdraft_limit(limit), logger(nullptr) {}
    
    void set_logger(TransactionLog* log) {
        logger = log;
    }

void deposit(int amount, int customer_id) override {
        std::lock_guard<std::mutex> lock(mtx);
        balance += amount;
        total_transactions++;
        
        //this would run if the logger is available
        if (logger) {
            std::stringstream formatted_msg;
            formatted_msg << std::fixed << std::setprecision(2)
                        << "Deposit: Customer " << customer_id 
                        << ", Amount: $" << amount
                        << ", New Balance: $" << balance;
            logger->log(formatted_msg.str());
        }
        
        //this is to print the details alongside to confirm for any error
        std::lock_guard<std::mutex> cout_lock(cout_mutex);
        std::cout << "Transaction #" << std::setw(2) << total_transactions 
                 << ": Customer " << customer_id << " deposited $" << std::setw(3) << amount
                 << ". New balance: $" << std::setw(3) << balance << std::endl;
        
        cv.notify_all();
    }

//withdraw function to implement the overdraft 
void withdraw(int amount, int customer_id) override {
        std::unique_lock<std::mutex> lock(mtx);
        
        if (balance - amount < -overdraft_limit) {
            if (logger) {
                std::stringstream error_msg;
                error_msg << std::fixed << std::setprecision(2)
                         << "Checking Withdrawal Blocked: Customer " << customer_id 
                         << " attempted to withdraw $" << amount 
                         << ". Current balance: $" << balance 
                         << ", Overdraft limit: $" << overdraft_limit;
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
                       << "Checking Withdrawal: Customer " << customer_id 
                       << ", Amount: $" << amount 
                       << ", New Balance: $" << balance;
            logger->log(success_msg.str());
        }
        
        //displays details to the console as well
        std::lock_guard<std::mutex> cout_lock(cout_mutex);
        std::cout << "Transaction #" << std::setw(2) << total_transactions 
                 << ": Customer " << customer_id << " withdrew $" << std::setw(3) << amount
                 << ". New balance: $" << std::setw(3) << balance << std::endl;
        
        cv.notify_all();
    }
};

#endif 