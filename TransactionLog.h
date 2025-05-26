#ifndef TRANSACTION_LOG_H
#define TRANSACTION_LOG_H

#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <string>

class TransactionLog {
private:
    std::ofstream log_file;// output file
    std::mutex log_mutex; //mutex for thread-safe logging of the accout details

public:
    TransactionLog(const std::string& filename = "transactions.txt") {
        log_file.open(filename, std::ios::app);
        log_session_start();//write session start marker at the header to differentiate the beginning
        // of a new session

    }

    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);// this also ensure thread safe logging

        //get current timestamp

        auto now = std::chrono::system_clock::now();
        auto time_now = std::chrono::system_clock::to_time_t(now);
         
        log_file << std::put_time(std::localtime(&time_now), "%Y-%m-%d %H:%M:%S") 
                 << " | " << message << std::endl;

        log_file.flush(); // Ensure immediate writing to the logs, corrected the issue of logs not entering as soon as they happpen
    }

    void log_session_start() {
        log("\n"); // Adding a new line to make my work neat
        log("-------------- NEW BANKING SESSION STARTED ---------------");
    }

// Destructor ensures proper cleanup and session end logging
// wanted to see the end of a session to seperate it from anithe
    ~TransactionLog() {
        if (log_file.is_open()) {
            log("---------- BANKING SESSION ENDED ------------------ ");
            log_file.close();
        }
    }
};

#endif