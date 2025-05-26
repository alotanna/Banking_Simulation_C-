# Banking System Simulation

A multi-threaded C++ banking system simulation that demonstrates concurrent account management, transaction logging, and different account types with various features.

## Features

### Account Types
- **Bank Account (Base Class)**
  - Basic deposit and withdrawal functionality
  - Thread-safe balance management
  - Transaction counting
  - Protected with mutexes for thread safety

- **Savings Account**
  - Minimum balance requirement
  - Interest rate calculation
  - Monthly interest application
  - Automatic logging of transactions

- **Checking Account**
  - Overdraft protection
  - Transaction logging
  - No minimum balance requirement

### Transaction Logging
- Timestamp-based logging
- Thread-safe logging mechanism using shared mutex
- Session tracking with start/end markers
- Detailed transaction information including customer IDs

### Concurrency Features
- Multi-threaded customer simulation
- Thread-safe account operations
- Separate mutex for console output
- Condition variables for synchronization
- Atomic flags for thread control

## System Requirements

- C++17 compatible compiler
- POSIX-compliant system (for pthread support)
- Standard C++ libraries

## Compilation and Execution

To compile the program, use:
```bash
g++ -std=c++17 main.cpp -o banksimulation -pthread
```

To run the simulation:
```bash
./banksimulation
```

## Program Structure

- `BankAccount.h`: Base class with core banking functionality
- `SavingsAccount.h`: Implements savings account with interest and minimum balance
- `CheckingAccount.h`: Implements checking account with overdraft protection
- `TransactionLog.h`: Handles thread-safe logging of all transactions
- `main.cpp`: Contains the simulation logic and thread management

## Simulation Details

The program simulates:
1. Multiple customers performing concurrent transactions
2. Automatic interest calculations for savings accounts
3. Overdraft protection for checking accounts
4. Transaction logging with timestamps
5. Both successful and failed transaction handling

## Output

The simulation provides two types of output:
1. Console output showing real-time transactions
2. Detailed logs in `transactions.txt` file with:
   - Transaction timestamps
   - Customer IDs
   - Transaction types and amounts
   - Balance updates
   - Error messages for failed transactions

## Error Handling

- Insufficient funds protection
- Minimum balance enforcement for savings accounts
- Overdraft limit enforcement for checking accounts
- Thread-safe error logging

## Notes

- The simulation runs with 5 customers for each account type
- Each customer performs 5 random transactions
- Interest is calculated every 300ms for savings accounts
- Random transaction amounts range from $1 to $50
- Default minimum balance for savings: $100
- Default overdraft limit for checking: $100
- Default interest rate: 5%