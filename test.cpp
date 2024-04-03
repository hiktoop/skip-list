#include <iostream>
#include "./skiplist.hpp"
using namespace std;

const string line = "=============================================================================";
int main() {
    SkipList<int, int> list(10);
    // == Insert Test =====================================================================
    cout << "== Insert Test " + line << endl;
    for (int i = 1; i <= 10; i++) {
        list.insert(i, 10 - i);
    }
    cout << list.print() << endl;

    // == Search Test =====================================================================
    cout << "== Search Test " + line << endl;
    for (int i = 1; i <= 10; i += 2) {
        cout << "find " << i << " get " << list.search(i) << endl;
    }
    cout << endl;

    // == Remove Test =====================================================================
    cout << "== Remove Test " << line << endl;
    for (int i = 2; i <= 10; i += 2) {
        cout << "remove " << i << endl;
        list.remove(i);
    }
    cout << list.print() << endl;
  
    // == Change Test =====================================================================
    // cout << "== Change Test " << line << endl;
  
    // == Concurrency Test ================================================================
    // cout << "== Concurrency Test " << line << endl;
 
    return 0;
}
