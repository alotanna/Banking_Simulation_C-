#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include <atomic>
#include <sstream>

#include "BankAccount.h"
#include "SavingsAccount.h"
#include "CheckingAccount.h"
#include "TransactionLog.h"

// Global flag to control threads
std::atomic<bool> keep_running(true);
template<typename AccountType>//Had to use this to show which tyoe of account is being creatd

void customer(AccountType& account, int customer_id, TransactionLog& logger) {
    std::random_device rd;
    std::mt19937 gen(rd());//generates random numbers
    std::uniform_int_distribution<> dis_type(0, 1);    // 0 or 1 for deposit/withdraw
    std::uniform_int_distribution<> dis_amount(1, 50); // amount between 1 and 50

    for (int i = 0; i < 5 && keep_running; ++i) {
        // Generate random amount and transaction type
        bool is_deposit = (dis_type(gen) == 0);
        double amount = dis_amount(gen);

        //Had to take out the methods to deposit and withdraw from here to ensure that everything logs from  the account.
        if (is_deposit) {
            account.deposit(amount, customer_id);
        } else {
            account.withdraw(amount, customer_id);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void interest_calculator(SavingsAccount& savings_account, TransactionLog& logger) {
    while (keep_running) {
        // Wait for specified interval before calculating interest whicn is 300ms
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        
        // Apply interest and log the result
        double interest = savings_account.apply_monthly_interest(0); // 0 because no customer does it
        
        //logs interest calculation if any interest was applied
        if (interest > 0) {
            std::stringstream interest_log;
            interest_log << "System Interest Calculation: Amount $" 
                        << std::fixed << std::setprecision(2) << interest 
                        << ", New Balance: $" << savings_account.get_balance()
                        << ", Interest Rate: " << savings_account.get_interest_rate() * 100 << "%";
            logger.log(interest_log.str());
        }
    }
}

int main() {
    //this is fo thr console
    std::cout << "Starting Banking Simulation in the console...\n" << std::endl;
    
    TransactionLog logger;
    
    SavingsAccount savings_account;
    savings_account.set_logger(&logger);  //set the logger for savings account
    
    CheckingAccount checking_account;
    checking_account.set_logger(&logger);  //set the logger for checking account

    //SavingsAccount savings_account;
    std::vector<std::thread> savings_threads;
    const int NUM_SAVINGS_CUSTOMERS = 5;

    // Create interest calculation thread
    std::thread interest_thread(interest_calculator, std::ref(savings_account), std::ref(logger));

    // Create customer threads for Savings
    for (int i = 1; i <= NUM_SAVINGS_CUSTOMERS; ++i) {
        savings_threads.push_back(std::thread(customer<SavingsAccount>, std::ref(savings_account), i, std::ref(logger)));
    }

    //waiting for all savings threads to complete
    for (auto& thread : savings_threads) {
        thread.join();
    }

    // Stop interest calculation
    keep_running = false;
    interest_thread.join();

    // Log final savings account summary
    std::stringstream savings_summary;
    savings_summary << "\nSavings Account Final Summary:\n "
                    << "Total Transactions: " << savings_account.get_total_transactions() 
                    << ", Final Balance: $" << std::fixed << std::setprecision(2) << savings_account.get_balance();
    logger.log(savings_summary.str());

    std::cout << "\n=== Savings Account Simulation Complete ===\n";
    std::cout << "Total Transactions: " << savings_account.get_total_transactions() << std::endl;
    std::cout << "Final Balance: $" << savings_account.get_balance() << std::endl;

    // Now do the same for Checking Account
    //CheckingAccount checking_account;
    std::vector<std::thread> checking_threads;
    const int NUM_CHECKING_CUSTOMERS = 5;

    // Reset running flag for the curent account
    keep_running = true;

    // Create customer threads for Checking
    for (int i = 1; i <= NUM_CHECKING_CUSTOMERS; ++i) {
        checking_threads.push_back(std::thread(customer<CheckingAccount>, std::ref(checking_account), i, std::ref(logger)));
    }

    //wait for all checking threads to complete
    for (auto& thread : checking_threads) {
        thread.join();
    }

    // Log final checking account summary
    std::stringstream checking_summary;
    checking_summary << "\nChecking Account Final Summary: \n"
                     << "Total Transactions: " << checking_account.get_total_transactions() 
                     << ", Final Balance: $" << std::fixed << std::setprecision(2) << checking_account.get_balance();
    logger.log(checking_summary.str());

 //display checking account results
    std::cout << "\n=== Checking Account Simulation Complete ===\n";
    std::cout << "Total Transactions: " << checking_account.get_total_transactions() << std::endl;
    std::cout << "Final Balance: $" << checking_account.get_balance() << std::endl;
    
    return 0;
}