# Order Book Trading System

## Overview

The Order Book Trading System is a C++ project designed to manage and execute orders in financial markets. It provides functionalities for adding, modifying, canceling orders, and matching them to execute trades. This system is a core component of trading platforms, aiming to ensure efficient and accurate order processing, reduce manual intervention, and optimize transaction speeds.

### Features

- **Order Management**: Add, modify, and cancel orders.
- **Order Matching**: Match buy and sell orders efficiently.
- **Real-Time Updates**: Maintain and update order book data.
- **Testing**: Comprehensive unit and integration tests using GoogleTest.

## Prerequisites

- **C++20**: Requires a C++20 compatible compiler (e.g., GCC 10+, Clang 11+).
- **GoogleTest**: For unit testing.

## Installation

### Clone the Repository

Clone the repository to your local machine:
```bash
git clone https://github.com/Yage0110/OrderbookProj.git
cd OrderbookProj
```

Install Dependencies
GoogleTest

macOS:
```bash
brew install googletest
```

Ubuntu

```bash
sudo apt-get install libgtest-dev
sudo apt-get install cmake # if not already installed
```

For other systems, follow instructions on the [GoogleTest GitHub page](https://github.com/google/googletest)

##Build the Project
  1. Create Build Directory:

  ```bash
    mkdir build
    cd build
  ```

  2. Configure with CMake:
  ```bash
   cmake ..
  ```
  3. Compiling
  ```bash
     make 
  ```

## Running Tests

After building the project, run the unit tests with:

```bash

g++ -std=c++20 -o test_include test_include.cpp Orderbook.cpp -I/usr/local/include -L/usr/local/lib -lgtest -lgmock -pthread
./test_include
```

## Running the Main Program

To compile and run the main program:

  1. Compile the Main Program:
```bash
g++ -std=c++20 -o main main.cpp Orderbook.cpp -I/usr/local/include -L/usr/local/lib -lgtest -lgmock -pthread
```

  or
    
  ```bash
  g++ -std=c++20 -o main main.cpp Orderbook.cpp 
  ```
2. Run the Program
   ```bash
   ./main
   ```

   Then copy the path of the text file you want to execute and paste it to the terminal.

## Structuring Your Input File

When running main with your own input file, use the following structure:

Add Order: A <side> <order_id> <price> <quantity> <order_type>
Modify Order: M <order_id> <new_price> <new_quantity>
Cancel Order: C <order_id>
Result Line: R <result_code>

Example Input File Structure: 
```plaintext
A B GoodTillCancel 100 10 1  # Add buy order of type GoodTillCancel with ID 100, price 10, quantity 1
A B FillAndKill 200 15 2  # Add order of type FillAndKill with ID 200, price 15, quantity 2
M 100 12 1  # Modify order with ID 100, new price 12, new quantity 1
C 200        # Cancel order with ID 200
R 0 0 0     # Result line indicating end of commands
```
<Side>: Indicates whether the order is a Buy (B) or Sell (S).
<OrderId>: Unique identifier for the order.
<OrderType>: Type of the order. Can be GoodTillCancel, FillAndKill, FillOrKill, etc.
<Price>: Price of the order.
<Quantity>: Quantity of the order.

R: Result line indicating expected bid, ask, and trade counts.


## System Design Document

For a detailed explanation of the system design, including architecture, data structures, and testing, please refer to the [Software Design Document](https://docs.google.com/document/d/1kf8LL_O-1KlqD-nWsoXOeVqfwp0ifyrbbhAPxCQ-emE/edit?usp=sharing).

