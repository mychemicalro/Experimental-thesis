//
// Generated file, do not edit! Created by opp_msgc 4.2 from applications/pbft/PBFTMessage.msg.
//

#ifndef _PBFTMESSAGE_M_H_
#define _PBFTMESSAGE_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0402
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif

// cplusplus {{
#include <TransportAddress.h>
#include <OverlayKey.h>
#include <Operation.h>
#include <Block.h>



static const int MESSAGETYPE_L = 3;
static const int OPERATION_L = 208; 
static const int VIEW_L = 8;
static const int SEQNUMBER_L = 8;
static const int DIGEST_L = 64; 
static const int OPRESULT_L = 1;
static const int REPLICANUMBER_L = 32;
static const int CREATORADDRESS_L = 32; 
static const int CREATORKEY_L = 32;
static const int BLOCK_L = 88; 
static const int RETRYNUMBER_L = 8;


#define PBFTMESSAGE_L(msg) (BASECALL_L(msg) + MESSAGETYPE_L + RETRYNUMBER_L + TRANSPORTADDRESS_L + KEY_L)
#define PBFTREQUEST(msg) (PBFTMESSAGE_L(msg) + OPERATION_L)
#define PBFTPREPREPARE(msg) (PBFTMESSAGE_L(msg) + VIEW_L + SEQNUMBER_L + DIGEST_L + OPERATION_L * msg->getBlock().getBlockOpsNumber())
#define PBFTPREPARE(msg) (PBFTMESSAGE_L(msg) + VIEW_L + SEQNUMBER_L + DIGEST_L)
#define PBFTCOMMIT(msg) (PBFTMESSAGE_L(msg) + VIEW_L + SEQNUMBER_L + DIGEST_L)
#define PBFTREPLY(msg) (PBFTMESSAGE_L(msg) + VIEW_L + OPRESULT_L + OPERATION_L + KEY_L)
#define PBFTCHECKPOINT(msg) (PBFTMESSAGE_L(msg) + DIGEST_L + SEQNUMBER_L)
// }}



/**
 * Enum generated from <tt>applications/pbft/PBFTMessage.msg</tt> by opp_msgc.
 * <pre>
 * enum MessageType {
 *     REQUEST = 1;
 * 	PREPREPARE = 2;
 * 	PREPARE = 3;
 * 	COMMIT = 4;
 * 	REPLY = 5;
 * 	CHECKPOINT = 6;
 * }
 * </pre>
 */
enum MessageType {
    REQUEST = 1,
    PREPREPARE = 2,
    PREPARE = 3,
    COMMIT = 4,
    REPLY = 5,
    CHECKPOINT = 6
};

/**
 * Class generated from <tt>applications/pbft/PBFTMessage.msg</tt> by opp_msgc.
 * <pre>
 * packet PBFTMessage {
 *     int type enum (MessageType);         
 *     int retryNumber = 0;				 
 *   
 * 	TransportAddress creatorAddress;      
 *     OverlayKey creatorKey;				 
 * 
 * }
 * </pre>
 */
class PBFTMessage : public ::cPacket
{
  protected:
    int type_var;
    int retryNumber_var;
    TransportAddress creatorAddress_var;
    OverlayKey creatorKey_var;

  private:
    void copy(const PBFTMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const PBFTMessage&);

  public:
    PBFTMessage(const char *name=NULL, int kind=0);
    PBFTMessage(const PBFTMessage& other);
    virtual ~PBFTMessage();
    PBFTMessage& operator=(const PBFTMessage& other);
    virtual PBFTMessage *dup() const {return new PBFTMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getType() const;
    virtual void setType(int type);
    virtual int getRetryNumber() const;
    virtual void setRetryNumber(int retryNumber);
    virtual TransportAddress& getCreatorAddress();
    virtual const TransportAddress& getCreatorAddress() const {return const_cast<PBFTMessage*>(this)->getCreatorAddress();}
    virtual void setCreatorAddress(const TransportAddress& creatorAddress);
    virtual OverlayKey& getCreatorKey();
    virtual const OverlayKey& getCreatorKey() const {return const_cast<PBFTMessage*>(this)->getCreatorKey();}
    virtual void setCreatorKey(const OverlayKey& creatorKey);
};

inline void doPacking(cCommBuffer *b, PBFTMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, PBFTMessage& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>applications/pbft/PBFTMessage.msg</tt> by opp_msgc.
 * <pre>
 * packet PBFTRequestMessage extends PBFTMessage {
 * 	type = REQUEST;
 * 	Operation op;
 * }
 * </pre>
 */
class PBFTRequestMessage : public ::PBFTMessage
{
  protected:
    Operation op_var;

  private:
    void copy(const PBFTRequestMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const PBFTRequestMessage&);

  public:
    PBFTRequestMessage(const char *name=NULL, int kind=0);
    PBFTRequestMessage(const PBFTRequestMessage& other);
    virtual ~PBFTRequestMessage();
    PBFTRequestMessage& operator=(const PBFTRequestMessage& other);
    virtual PBFTRequestMessage *dup() const {return new PBFTRequestMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual Operation& getOp();
    virtual const Operation& getOp() const {return const_cast<PBFTRequestMessage*>(this)->getOp();}
    virtual void setOp(const Operation& op);
};

inline void doPacking(cCommBuffer *b, PBFTRequestMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, PBFTRequestMessage& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>applications/pbft/PBFTMessage.msg</tt> by opp_msgc.
 * <pre>
 * packet PBFTPreprepareMessage extends PBFTMessage {
 * 	type = PREPREPARE;
 * 	int view;
 * 	int seqNumber;
 * 	string digest; 
 * 	Block block; 
 * }
 * </pre>
 */
class PBFTPreprepareMessage : public ::PBFTMessage
{
  protected:
    int view_var;
    int seqNumber_var;
    opp_string digest_var;
    Block block_var;

  private:
    void copy(const PBFTPreprepareMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const PBFTPreprepareMessage&);

  public:
    PBFTPreprepareMessage(const char *name=NULL, int kind=0);
    PBFTPreprepareMessage(const PBFTPreprepareMessage& other);
    virtual ~PBFTPreprepareMessage();
    PBFTPreprepareMessage& operator=(const PBFTPreprepareMessage& other);
    virtual PBFTPreprepareMessage *dup() const {return new PBFTPreprepareMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getView() const;
    virtual void setView(int view);
    virtual int getSeqNumber() const;
    virtual void setSeqNumber(int seqNumber);
    virtual const char * getDigest() const;
    virtual void setDigest(const char * digest);
    virtual Block& getBlock();
    virtual const Block& getBlock() const {return const_cast<PBFTPreprepareMessage*>(this)->getBlock();}
    virtual void setBlock(const Block& block);
};

inline void doPacking(cCommBuffer *b, PBFTPreprepareMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, PBFTPreprepareMessage& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>applications/pbft/PBFTMessage.msg</tt> by opp_msgc.
 * <pre>
 * packet PBFTPrepareMessage extends PBFTMessage {
 * 	type = PREPARE;
 * 	int view;
 * 	int seqNumber;
 * 	string digest;
 * }
 * </pre>
 */
class PBFTPrepareMessage : public ::PBFTMessage
{
  protected:
    int view_var;
    int seqNumber_var;
    opp_string digest_var;

  private:
    void copy(const PBFTPrepareMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const PBFTPrepareMessage&);

  public:
    PBFTPrepareMessage(const char *name=NULL, int kind=0);
    PBFTPrepareMessage(const PBFTPrepareMessage& other);
    virtual ~PBFTPrepareMessage();
    PBFTPrepareMessage& operator=(const PBFTPrepareMessage& other);
    virtual PBFTPrepareMessage *dup() const {return new PBFTPrepareMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getView() const;
    virtual void setView(int view);
    virtual int getSeqNumber() const;
    virtual void setSeqNumber(int seqNumber);
    virtual const char * getDigest() const;
    virtual void setDigest(const char * digest);
};

inline void doPacking(cCommBuffer *b, PBFTPrepareMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, PBFTPrepareMessage& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>applications/pbft/PBFTMessage.msg</tt> by opp_msgc.
 * <pre>
 * packet PBFTCommitMessage extends PBFTMessage {
 * 	type = COMMIT;
 * 	int view;
 * 	int seqNumber;
 * 	string digest;
 * }
 * </pre>
 */
class PBFTCommitMessage : public ::PBFTMessage
{
  protected:
    int view_var;
    int seqNumber_var;
    opp_string digest_var;

  private:
    void copy(const PBFTCommitMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const PBFTCommitMessage&);

  public:
    PBFTCommitMessage(const char *name=NULL, int kind=0);
    PBFTCommitMessage(const PBFTCommitMessage& other);
    virtual ~PBFTCommitMessage();
    PBFTCommitMessage& operator=(const PBFTCommitMessage& other);
    virtual PBFTCommitMessage *dup() const {return new PBFTCommitMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getView() const;
    virtual void setView(int view);
    virtual int getSeqNumber() const;
    virtual void setSeqNumber(int seqNumber);
    virtual const char * getDigest() const;
    virtual void setDigest(const char * digest);
};

inline void doPacking(cCommBuffer *b, PBFTCommitMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, PBFTCommitMessage& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>applications/pbft/PBFTMessage.msg</tt> by opp_msgc.
 * <pre>
 * packet PBFTReplyMessage extends PBFTMessage {
 * 	type = REPLY;
 * 	int view;
 * 	OverlayKey replicaNumber; 							
 * 	
 * 	
 * 	Block block;
 * }
 * </pre>
 */
class PBFTReplyMessage : public ::PBFTMessage
{
  protected:
    int view_var;
    OverlayKey replicaNumber_var;
    Block block_var;

  private:
    void copy(const PBFTReplyMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const PBFTReplyMessage&);

  public:
    PBFTReplyMessage(const char *name=NULL, int kind=0);
    PBFTReplyMessage(const PBFTReplyMessage& other);
    virtual ~PBFTReplyMessage();
    PBFTReplyMessage& operator=(const PBFTReplyMessage& other);
    virtual PBFTReplyMessage *dup() const {return new PBFTReplyMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getView() const;
    virtual void setView(int view);
    virtual OverlayKey& getReplicaNumber();
    virtual const OverlayKey& getReplicaNumber() const {return const_cast<PBFTReplyMessage*>(this)->getReplicaNumber();}
    virtual void setReplicaNumber(const OverlayKey& replicaNumber);
    virtual Block& getBlock();
    virtual const Block& getBlock() const {return const_cast<PBFTReplyMessage*>(this)->getBlock();}
    virtual void setBlock(const Block& block);
};

inline void doPacking(cCommBuffer *b, PBFTReplyMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, PBFTReplyMessage& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>applications/pbft/PBFTMessage.msg</tt> by opp_msgc.
 * <pre>
 * packet PBFTCheckpointMessage extends PBFTMessage {
 *     type = CHECKPOINT;
 *     int seqNumber;
 *     string digest;
 * }
 * </pre>
 */
class PBFTCheckpointMessage : public ::PBFTMessage
{
  protected:
    int seqNumber_var;
    opp_string digest_var;

  private:
    void copy(const PBFTCheckpointMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const PBFTCheckpointMessage&);

  public:
    PBFTCheckpointMessage(const char *name=NULL, int kind=0);
    PBFTCheckpointMessage(const PBFTCheckpointMessage& other);
    virtual ~PBFTCheckpointMessage();
    PBFTCheckpointMessage& operator=(const PBFTCheckpointMessage& other);
    virtual PBFTCheckpointMessage *dup() const {return new PBFTCheckpointMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getSeqNumber() const;
    virtual void setSeqNumber(int seqNumber);
    virtual const char * getDigest() const;
    virtual void setDigest(const char * digest);
};

inline void doPacking(cCommBuffer *b, PBFTCheckpointMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, PBFTCheckpointMessage& obj) {obj.parsimUnpack(b);}


#endif // _PBFTMESSAGE_M_H_
