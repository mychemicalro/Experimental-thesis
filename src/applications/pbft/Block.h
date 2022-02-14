
#include <iostream>
#include <sstream>

using namespace std;

class Block {

    public:
        string hash;
        string prevBlockHash;

        Block();
        string computeHash() const;

    private:

};
