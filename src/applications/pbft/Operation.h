#ifndef OPERATION_H_
#define OPERATION_H_

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
        OverlayKey getOriginatorKey() const {return originator.first;}
        IPvXAddress getIp(){return originator.second;}
        string getHash() const {return hash;}


        // And setters

        // Other methods

        /**
         * Computes the hash of the operation,
         * concatenating "WRITE;<OVERLAYKEY>;<timestamp>;<IP>"
         */
        string cHash() const;

        /**
         * TODO Implement == operator for comparing two operations
         */

    private:
        string op;
        simtime_t timestamp;
        pair<OverlayKey, IPvXAddress> originator;
        string hash;
};

#endif
