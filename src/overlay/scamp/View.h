/*
 * View.h
 *
 *  Created on: 30/nov/2021
 *      Author: DinuFC
 */

#ifndef VIEW_H_
#define VIEW_H_

#include <NodeVector.h>
#include <omnetpp.h>
#include <map>
#include <InitStages.h>

enum ViewType{
    INVIEW,
    PARTIALVIEW,
    SEENVIEW
};

class BaseOverlay;

class Scamp;

struct NodeEntry{
    NodeHandle nodeHandle;
    double lastHeartbeatArrival;
};
std::ostream& operator<<(std::ostream& os, const NodeEntry& e);


/**
 * This class must store the InView or PartialView of the nodes.
 * The view will have some size, with a getter.
 * The only difference between the two is the size. For InView, the size is unbounded,
 * for the PartialView, it must be static.
 *
 */

class View : public cSimpleModule {

public:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage* msg);

    // don't know what it is
    virtual int numInitStages() const {
        return MAX_STAGE_OVERLAY + 1;
    }

    // initialize parameters and data structure
    virtual void initializeView(const NodeHandle& owner, Scamp* overlay);

    /**
     * Get members size
     */
    virtual uint32_t getSize();

    /**
     * Get members' max size allowed
     */
    virtual uint32_t getMaxSize();

    /**
     * Return true if there is room for eventual members
     */
    virtual bool room();

    // add a member to the data structure
    virtual bool addMember(const NodeHandle& node, bool force = false);

    // get a member in some position
    virtual const NodeHandle& getMember(uint32_t pos);

    // remove a node -> position needed?
    virtual void removeMember(const NodeHandle& node);

    /**
     * Handle the failure of some node
     */
    bool handleFailedNode(const TransportAddress& failed);

    /**
     * clear members
     */
    virtual void clearView();

    /**
     * Check if node is already part of members
     */
    virtual bool isPresent(const NodeHandle& node);

    /**
     * Get random nodes from partialView with an exception, otherwise empty vector
     */
    virtual std::vector<NodeHandle> getRandomNodesExcept(const NodeHandle& exception, int howMany=1);

    /**
     * Get random nodes from partialView
     */
    virtual std::vector<NodeHandle> getRandomNodes(int howMany);

    /**
     * Check if view is empty
     */
    virtual bool isEmpty();

    /**
     * Set simTime of the last heartbeat arrived
     */
    virtual void setHeartbeatArrival(const NodeHandle& node, double arrival);

    /**
     * Delete nodes from members with an arrival time too great.
     * Returnes the number of deleted nodes
     */
    virtual std::vector<NodeHandle> prune(double now, double aliveTimeout);

    virtual std::vector<NodeHandle> getNodeHandles();

    virtual ViewType getType();
    virtual NodeHandle& getOwnerNode();

    virtual Scamp& getOverlay();


protected:
    int maxSize;
    NodeHandle thisNode; /**< own node handle */
    ViewType t;


private:
    std::map<OverlayKey, NodeEntry> members; /**< the members structure */
    Scamp* overlay; /**< pointer to the main Scamp module */
};




#endif /* VIEW_H_ */
