
/*
 * Operation.cc
 *
 *  Created on: 16/feb/2021
 *      Author: DinuFC
 */

#include "Operation.h"
#include "sha256.h"

Operation::Operation(OverlayKey ok, IPvXAddress ip, simtime_t ts){
    op = "Ciao";
    timestamp = ts;
    originator = make_pair(ok, ip);
}

//copy constructor
Operation::Operation( const Operation& operation ) {
    op = operation.op;
    timestamp = operation.timestamp;
}

Operation::Operation(){

}

const char* Operation::computeHash() {
    stringstream ss;
    ss << op << timestamp;
    EV << "DIGEST: " << ss << endl;
    const char* c = sha256(ss.str()).c_str();
    return c;
}

Operation& Operation::getOp(){
    return *this;
}



/*
std::ostream& operator<<(std::ostream& os, const Operation& operation) {
    // os << operation.getOp() << " at" << operation.getOriginator() << " with timestamp: " << operation.getTimestamp().;
    os << "Some operation";
    return os;
};
*/


