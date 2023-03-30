#include <iostream>
#include "PrimeChecker.hpp"
//comment for A6 submission
//Comment for A6 from Erik
//Comment for A6 from Mijin
//Comment for A6 from Patricia

int main(int argc, char ** argv) {
    if(argc == 2) {
        int number = std::stoi(argv[1]);
        PrimeChecker pc;
        std::cout << "Harring, Erik; " << number << " is a prime number? " << pc.isPrime(number) << std::endl;
    }
    
    return 0;
}
