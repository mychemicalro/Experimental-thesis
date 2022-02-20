
/*
 * Operation.cc
 *
 *  Created on: 16/feb/2021
 *      Author: DinuFC
 */

#include "Operation.h"
#include "sha256.h"

Operation::Operation(OverlayKey ok, IPvXAddress ip, simtime_t ts){
    stringstream ss;
    ss << "WRITE;" << ok.toString() << ";" << ts.str();

    op = ss.str();

    timestamp = ts;
    originator = make_pair(ok, ip);
    hash = cHash();
    EV << "op: " << op << " hash: " << hash << endl;
}

//copy constructor
Operation::Operation( const Operation& operation ) {
    op = operation.op;
    timestamp = operation.timestamp;
    originator = operation.originator;
    hash = operation.hash;
}

Operation::Operation(){

}

/*
// TODO Adjust the hash computation
const char* Operation::computeHash() {
    stringstream ss;
    ss << op << originator.first.toString() << originator.second.str();
    const char* c = sha256(ss.str()).c_str();
    return c;
}
*/

Operation& Operation::getOp(){
    return *this;
}

string Operation::cHash() const {
    stringstream ss;
    ss << op << originator.first.toString() << originator.second.str();

    return sha256(ss.str());
}

