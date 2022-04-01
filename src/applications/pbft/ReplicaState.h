/*
 * ReplicaState.h
 *
 *  Created on: 16/feb/2021
 *      Author: DinuFC
 */

#ifndef REPLICASTATE_H_
#define REPLICASTATE_H_

#include <NodeVector.h>
#include <omnetpp.h>
#include <InitStages.h>
#include <vector>
#include "PBFTMessage_m.h"

/**
 * Class for storing the replica state and the message log, that will have to contain
 * most of the client requests and lots of other messages.
 * Initially, the replica is not primary.
 *
 * It maintains:
 *      - a requests vector
 *      - a preprepares vector
 *      - a prepares vector
 *      - a commits vector
 *      - a replies vector
 *
 */

class Checkpoint{
    public:
        Checkpoint(int sn, string d);
        bool proof;
        int sn;
        string digest;
};

class ReplicaState : public cSimpleModule {

public:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage* msg);

    // don't know what it is
    virtual int numInitStages() const {
        return MAX_STAGE_OVERLAY + 1;
    }

    // initialize parameters and data structure
    virtual void initializeState(const OverlayKey* ok);

    // Getters
    int getCurrentView(){ return currentView; }
    bool getPrimary() { return primary; }
    int getNodesNumber() { return nodesNumber; }

    // and setters
    void setCurrentView(int cv){ currentView = cv; }
    void setPrimary(bool b){ primary = b; }

    /**
     * Add message to requests log
     */
    void addToRequestsLog(PBFTRequestMessage* msg);

    /**
     * Check if a given message digest is in requests
     */
    bool digestInRequestsLog(const char* digest);

    /**
     * Add message to preprepares log
     */
    void addToPrepreparesLog(PBFTPreprepareMessage* msg);

    /**
     * Add message to prepares log
     */
    void addToPreparesLog(PBFTPrepareMessage* msg);

    void addToCommitsLog(PBFTCommitMessage* msg);

    void addToRepliesLog(PBFTReplyMessage* msg);

    void addToCheckpointsLog(PBFTCheckpointMessage* msg);

    /**
     * Returns true if the message is already present in the log
     */
    bool seenMessage(cMessage* msg);

    /**
     * Check if there is some PreparedCertificate for some message m.
     * The messages should be unique in the preprepares vector.
     * So a simple count should be enough,
     * where digest, seqNum and v are the same as in the request.
     */
    bool searchPreparedCertificate(PBFTPrepareMessage* m);

    /**
     * I need to have also my commit message.
     */
    bool searchCommittedCertificate(PBFTCommitMessage* m);

    bool searchReplyCertificate(PBFTReplyMessage* m);

    /**
     * Check if the replica hash accepted another prepare with same seqNum and view but with different digest
     */
    bool otherPreprepareAccepted(PBFTPreprepareMessage* msg);

    /**
     * Procedure that check if a replica can send a PREPARE message.
     * Checks all the existing preprepares it received, and if the block can be prepared
     * and the block is not among the candidates, the block becomes a candidate
     */
    bool checkIfCanPrepare(PBFTRequestMessage* msg);

    void addCandidateBlock(PBFTPreprepareMessage* preprep);


    bool isPresentCandidateBlock(string dig);


    map<string,Block> getCandidateBlocks();

    Block& getBlock(const char* digest);

    PBFTPreprepareMessage& getPreprepareMessage(Operation& op);

    void addTimestamp(PBFTPreprepareMessage* preprep);

    double getTimestamp(string digest);

    /**
     * Returns true if the operation was already included in a PREPREPARE block.
     */
    bool operationPrepPrepared(Operation& op);

    vector<PBFTCommitMessage> getCommitMessages(int sn);

    void clearDataStructures();

    /**
     * Given the sequence number, delete all old preprepares, prepares and commits, and also old checkpoints.
     * For now, leave replies and requests.
     */
    void throwGarbage(int sn);

    /**
     * Checks if for this sequence number the certificate is stable.
     * If yes, then deletes the previous checkpoints.
     */
    void checkpointProcedure(int sn);

    /**
     * Removes a request from the clientRequests based on an operation in input
     */
    void deleteRequestFromClientRequests(Operation& op);

    /**
     * Returns the vector of client requests.
     */
    vector<PBFTRequestMessage> getClientRequests();

    /**
     * Add request to the client requests vector.
     */
    void addClientRequest(PBFTRequestMessage* req);

    int getClientRequestSize();

protected:

private:
    // Class variables
    int currentView;
    bool primary; /* this replica is primary*/
    int f;
    int nodesNumber;

    std::vector<PBFTRequestMessage> requests;
    std::vector<PBFTPreprepareMessage> preprepares;
    std::vector<PBFTPrepareMessage> prepares;
    std::vector<PBFTCommitMessage> commits;
    std::vector<PBFTReplyMessage> replies;
    std::vector<PBFTCheckpointMessage> checkpoints;

    // These are copies of the actual messages sent
    std::vector<PBFTRequestMessage> clientRequests;

    const OverlayKey* overlayk;

    map<string,Block> candidateBlocks;
    map<string,double> timestamps;

    map<int, Checkpoint> checkpoints_data;



};


#endif /* REPLICASTATE_H_ */
