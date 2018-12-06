#include <iostream>
#include <vector>

#include "Queue.h"

// predicato usato nel test dedicato a "transformif".  
template <class T>
class ParityChecker {
    public:
        bool operator()(const T& ele) const {
            return ele % 2 == 0;
        }
};

// operatore usato nel test dedicato a "transformif".  
template <class T>
class Multiplier {
    public:
        Multiplier(const T& bonus):
            _bonus(bonus) {}

        T operator()(const T& ele) const {
            return ele * _bonus;
        }

    private:
        T _bonus;
};

// MAIN
int main()
{
    // TEST: creazione coda di interi vuota.
    std::cout << "TEST1: creazione coda interi" << std::endl;
    Queue<int> queue;
    std::cout << "stampa coda: " << queue << std::endl;

    // TEST: inserimento elemento per elemento.
    std::cout << "TEST2: inserimento 5, 4, 3, 2, 1 elemento per elemento" << std::endl; 
    for (int j = 5; j > 0; --j) {
        queue.push(j);
    }
    std::cout << "stampa coda: " << queue << std::endl;

    // TEST: controllo lunghezza coda.
    std::cout << "TEST3: la coda contiene " << queue.size() << " elementi." << std::endl;
    std::cout << "stampa coda: " << queue << std::endl;

    // TEST: rimozione ultimo.
    std::cout << "TEST4: rimozione 5, 4, 3 elemento per elemento" << std::endl;
    for (int j = 0; j < 3; ++j) {
        queue.pop();
    }
    std::cout << "stampa coda: " << queue << std::endl;

    // TEST: accesso e scrittura primo e ultimo.
    std::cout << "TEST5: accesso a primo e utlimo: " << queue.front() << ", " << queue.back() << std::endl;
    std::cout << "TEST6: scrittura primo a 1 e utlimo a 2" << std::endl;
    queue.setFront(1);
    queue.setBack(2);
    std::cout << "stampa coda: " << queue << std::endl;

    // TEST: riemptimento lista da range delimitato da iteratori.
    std::cout << "TEST7: inserimento di std::vector{3...10} dentro la coda attraverso l'uso di "
              << "iteratori." << std::endl;
    std::vector<int> range = { 3, 4, 5, 6, 7, 8, 9, 10 };
    queue.insert(range.begin(), range.end());
    std::cout << "stampa coda: " << queue << std::endl;

    // TEST: controllo della presenza di un determinato elemento nella coda.
    std::cout << "TEST8: l'elemento 7 "
        << (queue.contains(7) ? "" : "non ")
        << "si trova nella coda"
        << std::endl;
    std::cout << "stampa coda: " << queue << std::endl;
    std::cout << "TEST9: l'elemento 64 "
        << (queue.contains(64) ? "" : "non ")
        << "si trova nella coda"
        << std::endl;
    std::cout << "stampa coda: " << queue << std::endl;
    
    // TEST: esempio di utilizzo di "transformif":
    //       raddoppiamento numeri pari nella coda.
    std::cout << "TEST10: raddoppiamento numeri pari con transformif." << std::endl;
    transformif(queue, ParityChecker<int>(), Multiplier<int>(2));
    std::cout << "stampa coda: " << queue << std::endl;
    
    return 0;
}
