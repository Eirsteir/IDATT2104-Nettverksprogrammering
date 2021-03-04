#include <iostream>
#include <thread>
#include <functional>
#include <list> 
#include <mutex>

using namespace std;

class PrimeSolver {

    int start;
    int end;
    int numberOfThreads;
    mutex primeFoundLock;
    list<thread> threads;
    list<int> primes;

    public:
        PrimeSolver(int start, int end, int numberOfThreads) {
            this->start = start;
            this->end = end;
            this->numberOfThreads = numberOfThreads;
        }

        void solveForPrimes() {

            const int delta = end + 1 - start;
            const int chunk = delta / numberOfThreads;

            int startIndex = start;
            int endIndex;

            for (int i = 0; i < numberOfThreads; i++) {
                endIndex = startIndex + chunk;
                pair<int, int> range(startIndex, endIndex); 

                threads.emplace_back([this, range, i] {
                    for (int n = range.first; n <= range.second; n++) {
                        if (isPrime(n)) {
                            primeFoundLock.lock();
                            primes.emplace_back(n);
                            primeFoundLock.unlock();
                        }
                    }
                });

                startIndex = endIndex + 1;
            }


            for (auto &thread : threads)
                thread.join();

            threads.clear();
            primes.sort();    
        }

        bool isPrime(int n) {
            if (n == 0 || n == 1)
                return false;
            
            for (int i = 2; i <= n / 2; ++i)
                if (n % i == 0)
                    return false;
                
            return true;
        }

    list<int> getPrimes() {
        return primes;
    }

};


int main() {
    int start = 0;
    int end = 1003;
    int numberOfThreads = 14;
    
    PrimeSolver solver(start, end, 4);
    solver.solveForPrimes();

    for (auto &prime : solver.getPrimes())
        cout << prime << endl;
    
    cout << '\n' << endl;
}

