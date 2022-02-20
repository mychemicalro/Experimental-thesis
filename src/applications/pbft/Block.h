
#ifndef BLOCK_H_
#define BLOCK_H_

#include <iostream>
#include <sstream>
#include <vector>

#include "Operation.h"

using namespace std;

/**
 * This class must represent a block in the blockchain.
 * A block has a max capacity, that is the number of operations contained.
 * It must have a hash, a prevhash, and a list of operations.
 * Also, the block must refer to a view number and a seqNum.
 *
 * It must be able to compute its hash.
 */

class Block {

    public:


        /**
         * Constructor gets the capacity of the block.
         */
        Block(int c);

        //copy constructor
        Block( const Block& block );
        Block();

        string computeHash();

        // Getters
        int getCapacity(){return capacity;}
        string getHash(){return hash;}

        //Setters
        void addOperation(Operation& op);
        void setSeqNumber(int sn){seqNumber = sn;}
        void setViewNumber(int vn){viewNumber = vn;}

        // Others
        bool isFull();
        vector<Operation> getOperations() const;

    private:
        string hash; // the block's hash
        string prevBlockHash; // the hash of the previous block appended in the blockchain

        int capacity; // the block's max capacity
        int seqNumber; // the sequence number assigned to this block by the primary
        int viewNumber; // view number

        // The block must contain a list of capacity operations
        vector<Operation> operations;


};

#endif
