#include <cmath>
#include <cstring>
#include <mutex>
#include <string>

std::mutex mtx;     // mutex for critical section

// Node
//
// Public member variable:
// - node_level: high level of this node
// - forward: point to next node
// =====================================================================================
template<typename K, typename V> 
class Node {
public:
    Node(int l): _key(), _value(), node_level(l), forward(new Node<K, V>*[l + 1]) {
        memset(forward, 0, sizeof(Node<K, V>*) * (l + 1));
    } 

    Node(const K k, const V v, int l): _key(k), _value(v), node_level(l), forward(new Node<K, V>*[l + 1]) {
        memset(forward, 0, sizeof(Node<K, V>*) * (l + 1));
    }

    ~Node() { delete [] forward; }

    K get_key() const { return _key; }
    V get_value() const { return _value; }
    std::string get_pair() const { return std::string("(") + std::to_string(_key) + ", " + std::to_string(_value) + ")"; }
    void set_value(V value) { this->_value = value; }
    
    int node_level;         // node level
    Node<K, V> **forward;   // array point to next node

private:
    // Inner data
    K _key;
    V _value;
};
// ======================================================================================

// SkipList
// ======================================================================================
template <typename K, typename V> 
class SkipList {
public: 
    // Constructe
    SkipList(int max_level) {
        this->_max_level = max_level;
        this->_skip_list_level = 0;
        this->_element_count = 0;
        this->_header = new Node<K, V>(max_level);
    }

    // Destructe
    ~SkipList() { delete _header; }

    // Get random level less than max_level
    int get_random_level() {
        int k = 1;
        while (rand() % 2) {
            k++;
            if (k >= _max_level) return _max_level;
        }
        return k;
    }

    // Insert element
    int insert(const K k, const V v) {
        mtx.lock();
        Node<K, V> *cur = this->_header;
        
        // use update to store each node less than k
        Node<K, V> *update[_max_level + 1];
        memset(update, 0, sizeof(Node<K, V>*) * (_max_level + 1));

        for (int i = _skip_list_level; i >= 0; i--) {
            while (cur->forward[i] != nullptr && cur->forward[i]->get_key() < k) {
                cur = cur->forward[i];
            }
            if (cur->forward[i] && cur->forward[i]->get_key() == k) {
                mtx.unlock();
                return 1;
            }
            update[i] = cur;
        }

        // fill with _header when rand level is more than skip list level
        int rand_level = get_random_level();
        if (rand_level > _skip_list_level) {
            for (int i = _skip_list_level + 1; i <= rand_level; i++) {
                update[i] = _header;
            }
            _skip_list_level = rand_level;
        }

        // update new node to their position
        Node<K, V> *new_node = new Node<K, V>(k, v, rand_level);
        for (int i = rand_level; i >= 0; i--) {
            new_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = new_node;
        }

        mtx.unlock();
        return 0;
    }

    // Search a exist key
    V search(K key) {
        mtx.lock();
        Node<K, V> *cur = _header;
        for (int i = _skip_list_level; i >= 0; i--) {
            while (cur->forward[i] && cur->forward[i]->get_key() < key) {
                cur = cur->forward[i];
            }
            // find the key
            if(cur->forward[i] && cur->forward[i]->get_key() == key) {
                mtx.unlock();
                return cur->forward[i]->get_value();
            }
        }
        mtx.unlock();
        return { };
    }

    // Remove the node key if there is
    void remove(K k) {
        mtx.lock();
        Node<K, V> *cur = this->_header;
        
        // use update to store each node less than k
        Node<K, V> *update[_max_level + 1];
        memset(update, 0, sizeof(Node<K, V>*) * (_max_level + 1));

        Node<K, V> *to_remove;
        for (int i = _skip_list_level; i >= 0; i--) {
            while (cur->forward[i] != nullptr && cur->forward[i]->get_key() < k) {
                cur = cur->forward[i];
            }
            // find the key to remove
            if (cur->forward[i] && cur->forward[i]->get_key() == k) {
                to_remove = cur->forward[i];
                cur->forward[i] = to_remove->forward[i];
            }
        }
        delete to_remove;

        mtx.unlock();
    }

    std::string print() {
        Node<K, V> *cur;
        std::string res;

        res += "==================================================================\n";
        res += "$ max level: " + std::to_string(_max_level) + ", ";
        res += "level: " + std::to_string(_skip_list_level) + ", ";
        res += "element count: " + std::to_string(_element_count) + "\n";
        for (int i = _skip_list_level; i >= 0; i--) {
            cur = _header;
            while (cur->forward[i]) {
                cur = cur->forward[i];
                res += "<||-->" + cur->get_pair() + "-->";
            }
            res += "nullptr\n";
        }
        res += "==================================================================\n";
        return res;
    }

    int size() { return _element_count; };

private:    
    int _max_level;         // max level can be
    int _skip_list_level;   // current level 
    int _element_count;     // node number
    Node<K, V> *_header;    // head node with no element
};
// ======================================================================================
