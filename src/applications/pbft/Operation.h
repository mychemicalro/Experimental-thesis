// #include <cstdint>
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
        Operation& getOp();
        OverlayKey getOriginatorKey(){return originator.first;}
        IPvXAddress getIp(){return originator.second;}
        string getHash(){return hash;}


        // And setters
        // TODO

        // Other methods
        const char* computeHash();

        string cHash() const;

    private:
        string op;
        simtime_t timestamp;
        pair<OverlayKey, IPvXAddress> originator;
        string hash;

};
