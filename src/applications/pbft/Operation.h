#include <iostream>
#include <sstream>
#include <simtime_t.h>
#include <OverlayKey.h>
#include <IPvXAddress.h>

using namespace std;

class Operation {

    public:
        Operation(OverlayKey ok, IPvXAddress ip, simtime_t ts);
        Operation( const Operation& operation );
        Operation();

        // Getters
        simtime_t getTimestamp(){return timestamp;}
        string getOp(){return op;}
        OverlayKey getOriginatorKey(){return originator.first;}
        IPvXAddress getIp(){return originator.second;}

        // And setters
        // TODO

    private:
        string op;
        simtime_t timestamp;
        pair<OverlayKey, IPvXAddress> originator;

};
