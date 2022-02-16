
/*
 * Block.cc
 *
 *  Created on: 14/feb/2021
 *      Author: DinuFC
 */

#include "Block.h"

Block::Block(){
    hash = computeHash();
}


string Block::computeHash() const {
    cout << "Call in computeHash()" << endl;
    return "ciao";
}
