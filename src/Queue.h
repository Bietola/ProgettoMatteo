#include <iterator> // per std::iterator.
#include <iostream> // per integrazione della stampa di una coda.

// Eccezione lanciata quando un nodo e' nullo.
class null_node_exception : public std::runtime_error {
    public:
        null_node_exception(const char* msg):
            std::runtime_error(msg) {}
};

// Eccezione lanciata quando la coda e' vuota.
class empty_queue_exception : public std::runtime_error {
    public:
        empty_queue_exception(const char* msg):
            std::runtime_error(msg) {}
};

// Forward declaration di Queue (per l'amicizia con Node).
template <typename T> class Queue;

// Generico nodo di una doppia lista puntata.
// Usato nell'implementazione della classe Qeueue.
template <typename T>
class Node {
    public:
        friend Queue<T>;

        // Costruttore per varie inizializzazioni.
        Node(const T& var, Node<T>* next, Node<T>* prev):
            _data(var),
            _next(next),
            _prev(prev) {}
        Node(const T& var):
            _data(var) {}

        // Costruttore di copia.
        Node(const Node& other):
            _data(other._data),
            _next(other._next),
            _prev(other._prev) {}

        // Operatori di comparazione.
        bool operator==(const Node<T>& other) {
            return _data == other._data && _next == other._next && _prev == other._prev;
        }
        bool operator!=(const Node<T>& other) {
            return !(*this == other);
        }

    private:
        T _data;
        Node<T>* _next = NULL;
        Node<T>* _prev = NULL;
};
    
// generica coda
//  -) implementata come doppia lista puntata.
//  -) rispetta la politica FIFO (First In First Out), ovvero il primo inserito
//     equivale al primo elemento rimosso.
//  -) supporta le operazioni di:
//      -) inserimento del primo (push).
//      -) rimozione del primo inserito.
//  -) implementata come doppia lista puntata
//  -) supporta due tipi di iteratori:
//      -) input/output principalmente per modifica.
//      -) costante (esclusivamente di output) per operazione di interrogazione.
template <typename T>
class Queue{
    public:
        // iteratore di input/output (bidirezionale)
        class iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
            public:
                // costruttore per inizializzazione.
                explicit iterator(Node<T>* currentNode):
                    _currentNode(currentNode) {}

                // costruttore di copia.
                iterator(const iterator& other):
                    _currentNode(other._currentNode) {}
            
                // operatori per scorrimento coda.
                // N.B. L'operatore "++" "avanza" nell'anzianita' degli elementi.
                //      Ovvero un'espressione del tipo "++itr" restituisce un 
                //      iteratore piu' vecchio di "itr".
                iterator& operator++() {
                    if(_currentNode == NULL)
                        throw null_node_exception("ERROR: trying to increment iterator containing null node.");
                    _currentNode = _currentNode->_next;
                    return *this;
                }
                iterator operator++(int) { // versione postfissa.
                    iterator tmp = *this; 
                    ++(*this);
                    return tmp;
                }
                iterator& operator--() {
                    if (_currentNode == NULL)
                        throw null_node_exception("ERROR: trying to decrement iterator containing null node.");
                    _currentNode = _currentNode->_prev;
                    return *this;
                }
                iterator operator--(int) { // versione postfissa.
                    iterator tmp = *this;
                    --(*this);
                    return tmp;
                }

                // dereferenziamento (con possibile modifica).
                T& operator*() {
                    if (_currentNode == NULL)
                        throw null_node_exception("ERROR: trying to dereference iterator containing null node.");
                    return _currentNode->_data;
                }

                // assegnamento.
                iterator& operator=(const iterator& other) {
                    _currentNode = other._currentNode;
                    return *this;
                }

                // operatori di comparazione.
                bool operator==(const iterator& other) const {
                    return _currentNode == other._currentNode;
                }
                bool operator!=(const iterator& other) const {
                    return !(*this == other); //implementato in termini di ==
                }
                
            private:
                // modo in cui l'iteratore tiene traccia della sua
                // posizione all'interno della coda. nel dettaglio
                // viene utilizzato un puntatore ad un oggetto Nodo<T>,
                // da cui si puo derivare il valore puntato [dall'iteratore]
                // e le informazioni posizionali circostanti (l'elemento prima
                // e quello dopo; per questo l'iteratore e' bidirezionale).
                Node<T>* _currentNode = NULL;
        };

        // variante costante dell'iteratore sopra.
        //  -) N.B. ridondanza di codice. Alcune operazioni non saranno descritte
        //          in maniera approfondita (riferirsi alla classe "iterator")
        class const_iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
            public:
                // costruttore.
                explicit const_iterator(Node<T>* currentNode):
                    _currentNode(currentNode) {}

                // costruttore di copia.
                const_iterator(const const_iterator& other):
                    _currentNode(other._currentNode) {}

                // scorrimento.
                const_iterator& operator++() {
                    if (_currentNode == NULL)
                        throw null_node_exception("ERROR: trying to increment iterator containing null node.");
                    _currentNode = _currentNode->_next;
                    return *this;
                }
                const_iterator operator++(int) { // versione postfissa.
                    const_iterator tmp = *this; 
                    ++(*this);
                    return tmp;
                }
                const_iterator& operator--() {
                    if (_currentNode == NULL)
                        throw null_node_exception("ERROR: trying to decrement iterator containing null node.");
                    _currentNode = _currentNode->_prev;
                    return *this;
                }
                const_iterator operator--(int) { // version postfissa.
                    const_iterator tmp = *this;
                    --(*this);
                    return tmp;
                }
                // dereferenziamento (non e' permessa la modifica della reference).
                const T& operator*() const { 
                    if (_currentNode == NULL)
                        throw null_node_exception("ERROR: trying to dereference iterator containing null node.");
                    return _currentNode->_data;
                }
                // assegnamento.
                const_iterator& operator=(const const_iterator& other) {
                    _currentNode = other._currentNode;
                    return *this;
                }

                // operatori di comparazione.
                bool operator==(const const_iterator& other) const {
                    return _currentNode == other._currentNode;
                }
                bool operator!=(const const_iterator& other) const {
                    return !(*this == other);
                }
                
            private:
                // rappresentazione interna dell'elemento puntato dall'iteratore e
                // della sua posizione (tutte info contenute nella classe Node<T>).
                const Node<T>* _currentNode = NULL;
        };

        // inserimento del primo elemento. Metodo fondamentale
        // per l'implementazione della politica FIFO (First In First Out).
        // Sara' infatti il primo elemento ad essere inserito tramite
        // "push" ad essere rimosso tramite "pop".
        // IMPLEMENTAZIONE SOTTOSTANTE: inserimento in testa ad una doppia lista puntata.
        void push(const T& var){
            // gestione size
            ++_size;

            // si crea un nuova testa e si collega il suo "next" alla vecchia testa
            Node<T>* newHead = new Node<T>(var, _head, NULL);

            // si collega prev della vecchia testa alla nuova testa.
            if (_head != NULL) {
                _head->_prev = newHead;
            }

            // si tiene a mente che la testa e' la nuova testa (e non piu' la vecchia).
            _head = newHead;

            // quando la lista è vuota; la coda corrisponde con la prima testa creata.
            if (_tail == NULL) {
                _tail = _head;
            }
        }

        // rimozione primo elemento inserito. Con "push" implementa
        // la politica FIFO (First In First Out), rimuovendo sempre
        // il primo elemento inserito con "push".
        // IMPLEMENTAZIONE SOTTOSTANTE: rimozione della coda ad una doppia lista puntata.
        void pop() {
            // eccezione: se la coda e' vuota non c'e' modo di rimuovere elementi.
            if (_size == 0) {
                throw empty_queue_exception("ERROR: trying to pop element from an empty queue.");
            }

            // gestione size
            --_size;

            // la nuova coda sara' l'elemento prima della coda corrente.
            Node<T>* newTail = _tail->_prev;

            // si cancella la vecchia coda (deallocandone la memoria).
            delete _tail;

            // la nuova coda non ha piu' nulla dopo (per definizione). 
            newTail->_next = NULL;

            // si tiene a mente che la coda e' la nuova coda (e non piu' la vecchia).
            _tail = newTail;
        }

        // inserisce nella coda un range di elementi identificato da due iteratori
        // di output. L'ordine di inserimento determina l'anzianita' dell'elemento inserito.
        template <typename OutputIterator>
        void insert(OutputIterator inBegin, OutputIterator inEnd) {
            for (OutputIterator inItr = inBegin; inItr != inEnd ; ++inItr) {
                push(*inItr);
            }
        }

        // numero di elementi nella coda.
        size_t size() const {
            return _size;
        }
        
        // getter per accesso al primo e ultimo elemento.
        T front() const {
            if (_tail == NULL)
                throw null_node_exception("ERROR: encountered null node while accessing first element.");
            return _tail->_data;
        }
        T back() const {
            if (_head == NULL)
                throw null_node_exception("ERROR: encountered null node while accessing last element.");
            return  _head->_data;
        }
        
        // setter per modifica del primo e utlimo elemento.
        void setFront(const T& first) {
            if (_tail == NULL)
                throw null_node_exception("ERROR: encountered null node while accessing first element.");
             _tail->_data = first;
        }
        void setBack(const T& last) {
            if (_tail == NULL)
                throw null_node_exception("ERROR: encountered null node while accessing last element.");
             _head->_data = last;
        }
        
        // interrogazione: presenza di uno specificato elemento nella coda.
        bool contains(const T& ele) const {
            // scorrimento coda con iteratori (const perche' non serve la modifica).
            for (const_iterator itr = cbegin(); itr != cend(); ++itr) {
                if (*itr == ele)
                    return true;
            }
            // scorrimento lista terminata senza aver trovato l'elemento.
            return false;
        }
        
        // iteratori di fine ed'inizio.
        iterator begin() {
            return iterator(_head);
        }
        iterator end() {
            return iterator(NULL);
        }
        // equivalenti costanti (solo output).
        const_iterator cbegin() const {
            return const_iterator(_head);
        }
        const_iterator cend() const {
            return const_iterator(NULL);
        }

    private: 
        Node<T>* _head = NULL;
        Node<T>* _tail = NULL;
        size_t _size = 0;
};

// generica funzione per l'applicazione di un predicato (generico)
// ad ogni elemento di una specficata lista che rispetta uno
// specificato (e generico) operatore.
template <typename T, typename Pred, typename Oper>
void transformif(Queue<T>& queue, const Pred& pred, const Oper& oper) {
    typedef typename Queue<T>::iterator iterator;
    for(iterator itr = queue.begin(); itr != queue.end(); ++itr) {
        if(pred(*itr) == true){
            *itr = oper(*itr);
        }
    }
}

// stampa di una coda dalle standard streams.
template <class T>
std::ostream& operator<<(std::ostream& ostream, const Queue<T>& queue) {
    // alias per iterator constante a elemento queue.
    typedef typename Queue<T>::const_iterator cqitr_t;

    // coda vuota e' [].
    if (queue.cbegin() == queue.cend()) {
        ostream << "[]";
    }

    // coda non vuota.
    else {
        ostream << "[" << *(queue.cbegin());
        for (cqitr_t itr = std::next(queue.cbegin()); itr != queue.cend(); ++itr) {
            ostream << ", " << *itr;
        }
        ostream << "]";
    }

    //
    return ostream;
}
