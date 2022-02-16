
#include <iostream>
#include <sstream>
#include <simtime_t.h>

using namespace std;

class Operation {

    public:
        string op;
        // TODO may also contain other data like simtime_t

        Operation(simtime_t ts);
        Operation( const Operation& handle );
        Operation();
    private:
        simtime_t timestamp;

};
