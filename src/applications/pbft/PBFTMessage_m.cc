//
// Generated file, do not edit! Created by opp_msgc 4.2 from applications/pbft/PBFTMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "PBFTMessage_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




EXECUTE_ON_STARTUP(
    cEnum *e = cEnum::find("MessageType");
    if (!e) enums.getInstance()->add(e = new cEnum("MessageType"));
    e->insert(REQUEST, "REQUEST");
    e->insert(PREPREPARE, "PREPREPARE");
    e->insert(PREPARE, "PREPARE");
    e->insert(COMMIT, "COMMIT");
    e->insert(REPLY, "REPLY");
    e->insert(CHECKPOINT, "CHECKPOINT");
);

Register_Class(PBFTMessage);

PBFTMessage::PBFTMessage(const char *name, int kind) : cPacket(name,kind)
{
    this->type_var = 0;
    this->retryNumber_var = 0;
}

PBFTMessage::PBFTMessage(const PBFTMessage& other) : cPacket(other)
{
    copy(other);
}

PBFTMessage::~PBFTMessage()
{
}

PBFTMessage& PBFTMessage::operator=(const PBFTMessage& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void PBFTMessage::copy(const PBFTMessage& other)
{
    this->type_var = other.type_var;
    this->retryNumber_var = other.retryNumber_var;
    this->creatorAddress_var = other.creatorAddress_var;
    this->creatorKey_var = other.creatorKey_var;
}

void PBFTMessage::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->type_var);
    doPacking(b,this->retryNumber_var);
    doPacking(b,this->creatorAddress_var);
    doPacking(b,this->creatorKey_var);
}

void PBFTMessage::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->type_var);
    doUnpacking(b,this->retryNumber_var);
    doUnpacking(b,this->creatorAddress_var);
    doUnpacking(b,this->creatorKey_var);
}

int PBFTMessage::getType() const
{
    return type_var;
}

void PBFTMessage::setType(int type)
{
    this->type_var = type;
}

int PBFTMessage::getRetryNumber() const
{
    return retryNumber_var;
}

void PBFTMessage::setRetryNumber(int retryNumber)
{
    this->retryNumber_var = retryNumber;
}

TransportAddress& PBFTMessage::getCreatorAddress()
{
    return creatorAddress_var;
}

void PBFTMessage::setCreatorAddress(const TransportAddress& creatorAddress)
{
    this->creatorAddress_var = creatorAddress;
}

OverlayKey& PBFTMessage::getCreatorKey()
{
    return creatorKey_var;
}

void PBFTMessage::setCreatorKey(const OverlayKey& creatorKey)
{
    this->creatorKey_var = creatorKey;
}

class PBFTMessageDescriptor : public cClassDescriptor
{
  public:
    PBFTMessageDescriptor();
    virtual ~PBFTMessageDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(PBFTMessageDescriptor);

PBFTMessageDescriptor::PBFTMessageDescriptor() : cClassDescriptor("PBFTMessage", "cPacket")
{
}

PBFTMessageDescriptor::~PBFTMessageDescriptor()
{
}

bool PBFTMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<PBFTMessage *>(obj)!=NULL;
}

const char *PBFTMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int PBFTMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int PBFTMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *PBFTMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "type",
        "retryNumber",
        "creatorAddress",
        "creatorKey",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int PBFTMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='t' && strcmp(fieldName, "type")==0) return base+0;
    if (fieldName[0]=='r' && strcmp(fieldName, "retryNumber")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "creatorAddress")==0) return base+2;
    if (fieldName[0]=='c' && strcmp(fieldName, "creatorKey")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *PBFTMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "TransportAddress",
        "OverlayKey",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *PBFTMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "MessageType";
            return NULL;
        default: return NULL;
    }
}

int PBFTMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    PBFTMessage *pp = (PBFTMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PBFTMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTMessage *pp = (PBFTMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getType());
        case 1: return long2string(pp->getRetryNumber());
        case 2: {std::stringstream out; out << pp->getCreatorAddress(); return out.str();}
        case 3: {std::stringstream out; out << pp->getCreatorKey(); return out.str();}
        default: return "";
    }
}

bool PBFTMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    PBFTMessage *pp = (PBFTMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setType(string2long(value)); return true;
        case 1: pp->setRetryNumber(string2long(value)); return true;
        default: return false;
    }
}

const char *PBFTMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        "TransportAddress",
        "OverlayKey",
    };
    return (field>=0 && field<4) ? fieldStructNames[field] : NULL;
}

void *PBFTMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTMessage *pp = (PBFTMessage *)object; (void)pp;
    switch (field) {
        case 2: return (void *)(&pp->getCreatorAddress()); break;
        case 3: return (void *)(&pp->getCreatorKey()); break;
        default: return NULL;
    }
}

Register_Class(PBFTRequestMessage);

PBFTRequestMessage::PBFTRequestMessage(const char *name, int kind) : PBFTMessage(name,kind)
{
    this->setType(REQUEST);

}

PBFTRequestMessage::PBFTRequestMessage(const PBFTRequestMessage& other) : PBFTMessage(other)
{
    copy(other);
}

PBFTRequestMessage::~PBFTRequestMessage()
{
}

PBFTRequestMessage& PBFTRequestMessage::operator=(const PBFTRequestMessage& other)
{
    if (this==&other) return *this;
    PBFTMessage::operator=(other);
    copy(other);
    return *this;
}

void PBFTRequestMessage::copy(const PBFTRequestMessage& other)
{
    this->op_var = other.op_var;
}

void PBFTRequestMessage::parsimPack(cCommBuffer *b)
{
    PBFTMessage::parsimPack(b);
    doPacking(b,this->op_var);
}

void PBFTRequestMessage::parsimUnpack(cCommBuffer *b)
{
    PBFTMessage::parsimUnpack(b);
    doUnpacking(b,this->op_var);
}

Operation& PBFTRequestMessage::getOp()
{
    return op_var;
}

void PBFTRequestMessage::setOp(const Operation& op)
{
    this->op_var = op;
}

class PBFTRequestMessageDescriptor : public cClassDescriptor
{
  public:
    PBFTRequestMessageDescriptor();
    virtual ~PBFTRequestMessageDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(PBFTRequestMessageDescriptor);

PBFTRequestMessageDescriptor::PBFTRequestMessageDescriptor() : cClassDescriptor("PBFTRequestMessage", "PBFTMessage")
{
}

PBFTRequestMessageDescriptor::~PBFTRequestMessageDescriptor()
{
}

bool PBFTRequestMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<PBFTRequestMessage *>(obj)!=NULL;
}

const char *PBFTRequestMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int PBFTRequestMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount(object) : 1;
}

unsigned int PBFTRequestMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *PBFTRequestMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "op",
    };
    return (field>=0 && field<1) ? fieldNames[field] : NULL;
}

int PBFTRequestMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='o' && strcmp(fieldName, "op")==0) return base+0;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *PBFTRequestMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "Operation",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : NULL;
}

const char *PBFTRequestMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int PBFTRequestMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    PBFTRequestMessage *pp = (PBFTRequestMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PBFTRequestMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTRequestMessage *pp = (PBFTRequestMessage *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getOp(); return out.str();}
        default: return "";
    }
}

bool PBFTRequestMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    PBFTRequestMessage *pp = (PBFTRequestMessage *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *PBFTRequestMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        "Operation",
    };
    return (field>=0 && field<1) ? fieldStructNames[field] : NULL;
}

void *PBFTRequestMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTRequestMessage *pp = (PBFTRequestMessage *)object; (void)pp;
    switch (field) {
        case 0: return (void *)(&pp->getOp()); break;
        default: return NULL;
    }
}

Register_Class(PBFTPreprepareMessage);

PBFTPreprepareMessage::PBFTPreprepareMessage(const char *name, int kind) : PBFTMessage(name,kind)
{
    this->setType(PREPREPARE);

    this->view_var = 0;
    this->seqNumber_var = 0;
    this->digest_var = 0;
}

PBFTPreprepareMessage::PBFTPreprepareMessage(const PBFTPreprepareMessage& other) : PBFTMessage(other)
{
    copy(other);
}

PBFTPreprepareMessage::~PBFTPreprepareMessage()
{
}

PBFTPreprepareMessage& PBFTPreprepareMessage::operator=(const PBFTPreprepareMessage& other)
{
    if (this==&other) return *this;
    PBFTMessage::operator=(other);
    copy(other);
    return *this;
}

void PBFTPreprepareMessage::copy(const PBFTPreprepareMessage& other)
{
    this->view_var = other.view_var;
    this->seqNumber_var = other.seqNumber_var;
    this->digest_var = other.digest_var;
    this->block_var = other.block_var;
}

void PBFTPreprepareMessage::parsimPack(cCommBuffer *b)
{
    PBFTMessage::parsimPack(b);
    doPacking(b,this->view_var);
    doPacking(b,this->seqNumber_var);
    doPacking(b,this->digest_var);
    doPacking(b,this->block_var);
}

void PBFTPreprepareMessage::parsimUnpack(cCommBuffer *b)
{
    PBFTMessage::parsimUnpack(b);
    doUnpacking(b,this->view_var);
    doUnpacking(b,this->seqNumber_var);
    doUnpacking(b,this->digest_var);
    doUnpacking(b,this->block_var);
}

int PBFTPreprepareMessage::getView() const
{
    return view_var;
}

void PBFTPreprepareMessage::setView(int view)
{
    this->view_var = view;
}

int PBFTPreprepareMessage::getSeqNumber() const
{
    return seqNumber_var;
}

void PBFTPreprepareMessage::setSeqNumber(int seqNumber)
{
    this->seqNumber_var = seqNumber;
}

const char * PBFTPreprepareMessage::getDigest() const
{
    return digest_var.c_str();
}

void PBFTPreprepareMessage::setDigest(const char * digest)
{
    this->digest_var = digest;
}

Block& PBFTPreprepareMessage::getBlock()
{
    return block_var;
}

void PBFTPreprepareMessage::setBlock(const Block& block)
{
    this->block_var = block;
}

class PBFTPreprepareMessageDescriptor : public cClassDescriptor
{
  public:
    PBFTPreprepareMessageDescriptor();
    virtual ~PBFTPreprepareMessageDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(PBFTPreprepareMessageDescriptor);

PBFTPreprepareMessageDescriptor::PBFTPreprepareMessageDescriptor() : cClassDescriptor("PBFTPreprepareMessage", "PBFTMessage")
{
}

PBFTPreprepareMessageDescriptor::~PBFTPreprepareMessageDescriptor()
{
}

bool PBFTPreprepareMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<PBFTPreprepareMessage *>(obj)!=NULL;
}

const char *PBFTPreprepareMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int PBFTPreprepareMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int PBFTPreprepareMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *PBFTPreprepareMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "view",
        "seqNumber",
        "digest",
        "block",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int PBFTPreprepareMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='v' && strcmp(fieldName, "view")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "seqNumber")==0) return base+1;
    if (fieldName[0]=='d' && strcmp(fieldName, "digest")==0) return base+2;
    if (fieldName[0]=='b' && strcmp(fieldName, "block")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *PBFTPreprepareMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "string",
        "Block",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *PBFTPreprepareMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int PBFTPreprepareMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    PBFTPreprepareMessage *pp = (PBFTPreprepareMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PBFTPreprepareMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTPreprepareMessage *pp = (PBFTPreprepareMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getView());
        case 1: return long2string(pp->getSeqNumber());
        case 2: return oppstring2string(pp->getDigest());
        case 3: {std::stringstream out; out << pp->getBlock(); return out.str();}
        default: return "";
    }
}

bool PBFTPreprepareMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    PBFTPreprepareMessage *pp = (PBFTPreprepareMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setView(string2long(value)); return true;
        case 1: pp->setSeqNumber(string2long(value)); return true;
        case 2: pp->setDigest((value)); return true;
        default: return false;
    }
}

const char *PBFTPreprepareMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
        "Block",
    };
    return (field>=0 && field<4) ? fieldStructNames[field] : NULL;
}

void *PBFTPreprepareMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTPreprepareMessage *pp = (PBFTPreprepareMessage *)object; (void)pp;
    switch (field) {
        case 3: return (void *)(&pp->getBlock()); break;
        default: return NULL;
    }
}

Register_Class(PBFTPrepareMessage);

PBFTPrepareMessage::PBFTPrepareMessage(const char *name, int kind) : PBFTMessage(name,kind)
{
    this->setType(PREPARE);

    this->view_var = 0;
    this->seqNumber_var = 0;
    this->digest_var = 0;
}

PBFTPrepareMessage::PBFTPrepareMessage(const PBFTPrepareMessage& other) : PBFTMessage(other)
{
    copy(other);
}

PBFTPrepareMessage::~PBFTPrepareMessage()
{
}

PBFTPrepareMessage& PBFTPrepareMessage::operator=(const PBFTPrepareMessage& other)
{
    if (this==&other) return *this;
    PBFTMessage::operator=(other);
    copy(other);
    return *this;
}

void PBFTPrepareMessage::copy(const PBFTPrepareMessage& other)
{
    this->view_var = other.view_var;
    this->seqNumber_var = other.seqNumber_var;
    this->digest_var = other.digest_var;
}

void PBFTPrepareMessage::parsimPack(cCommBuffer *b)
{
    PBFTMessage::parsimPack(b);
    doPacking(b,this->view_var);
    doPacking(b,this->seqNumber_var);
    doPacking(b,this->digest_var);
}

void PBFTPrepareMessage::parsimUnpack(cCommBuffer *b)
{
    PBFTMessage::parsimUnpack(b);
    doUnpacking(b,this->view_var);
    doUnpacking(b,this->seqNumber_var);
    doUnpacking(b,this->digest_var);
}

int PBFTPrepareMessage::getView() const
{
    return view_var;
}

void PBFTPrepareMessage::setView(int view)
{
    this->view_var = view;
}

int PBFTPrepareMessage::getSeqNumber() const
{
    return seqNumber_var;
}

void PBFTPrepareMessage::setSeqNumber(int seqNumber)
{
    this->seqNumber_var = seqNumber;
}

const char * PBFTPrepareMessage::getDigest() const
{
    return digest_var.c_str();
}

void PBFTPrepareMessage::setDigest(const char * digest)
{
    this->digest_var = digest;
}

class PBFTPrepareMessageDescriptor : public cClassDescriptor
{
  public:
    PBFTPrepareMessageDescriptor();
    virtual ~PBFTPrepareMessageDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(PBFTPrepareMessageDescriptor);

PBFTPrepareMessageDescriptor::PBFTPrepareMessageDescriptor() : cClassDescriptor("PBFTPrepareMessage", "PBFTMessage")
{
}

PBFTPrepareMessageDescriptor::~PBFTPrepareMessageDescriptor()
{
}

bool PBFTPrepareMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<PBFTPrepareMessage *>(obj)!=NULL;
}

const char *PBFTPrepareMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int PBFTPrepareMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int PBFTPrepareMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *PBFTPrepareMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "view",
        "seqNumber",
        "digest",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int PBFTPrepareMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='v' && strcmp(fieldName, "view")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "seqNumber")==0) return base+1;
    if (fieldName[0]=='d' && strcmp(fieldName, "digest")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *PBFTPrepareMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "string",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *PBFTPrepareMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int PBFTPrepareMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    PBFTPrepareMessage *pp = (PBFTPrepareMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PBFTPrepareMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTPrepareMessage *pp = (PBFTPrepareMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getView());
        case 1: return long2string(pp->getSeqNumber());
        case 2: return oppstring2string(pp->getDigest());
        default: return "";
    }
}

bool PBFTPrepareMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    PBFTPrepareMessage *pp = (PBFTPrepareMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setView(string2long(value)); return true;
        case 1: pp->setSeqNumber(string2long(value)); return true;
        case 2: pp->setDigest((value)); return true;
        default: return false;
    }
}

const char *PBFTPrepareMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<3) ? fieldStructNames[field] : NULL;
}

void *PBFTPrepareMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTPrepareMessage *pp = (PBFTPrepareMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(PBFTCommitMessage);

PBFTCommitMessage::PBFTCommitMessage(const char *name, int kind) : PBFTMessage(name,kind)
{
    this->setType(COMMIT);

    this->view_var = 0;
    this->seqNumber_var = 0;
    this->digest_var = 0;
}

PBFTCommitMessage::PBFTCommitMessage(const PBFTCommitMessage& other) : PBFTMessage(other)
{
    copy(other);
}

PBFTCommitMessage::~PBFTCommitMessage()
{
}

PBFTCommitMessage& PBFTCommitMessage::operator=(const PBFTCommitMessage& other)
{
    if (this==&other) return *this;
    PBFTMessage::operator=(other);
    copy(other);
    return *this;
}

void PBFTCommitMessage::copy(const PBFTCommitMessage& other)
{
    this->view_var = other.view_var;
    this->seqNumber_var = other.seqNumber_var;
    this->digest_var = other.digest_var;
}

void PBFTCommitMessage::parsimPack(cCommBuffer *b)
{
    PBFTMessage::parsimPack(b);
    doPacking(b,this->view_var);
    doPacking(b,this->seqNumber_var);
    doPacking(b,this->digest_var);
}

void PBFTCommitMessage::parsimUnpack(cCommBuffer *b)
{
    PBFTMessage::parsimUnpack(b);
    doUnpacking(b,this->view_var);
    doUnpacking(b,this->seqNumber_var);
    doUnpacking(b,this->digest_var);
}

int PBFTCommitMessage::getView() const
{
    return view_var;
}

void PBFTCommitMessage::setView(int view)
{
    this->view_var = view;
}

int PBFTCommitMessage::getSeqNumber() const
{
    return seqNumber_var;
}

void PBFTCommitMessage::setSeqNumber(int seqNumber)
{
    this->seqNumber_var = seqNumber;
}

const char * PBFTCommitMessage::getDigest() const
{
    return digest_var.c_str();
}

void PBFTCommitMessage::setDigest(const char * digest)
{
    this->digest_var = digest;
}

class PBFTCommitMessageDescriptor : public cClassDescriptor
{
  public:
    PBFTCommitMessageDescriptor();
    virtual ~PBFTCommitMessageDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(PBFTCommitMessageDescriptor);

PBFTCommitMessageDescriptor::PBFTCommitMessageDescriptor() : cClassDescriptor("PBFTCommitMessage", "PBFTMessage")
{
}

PBFTCommitMessageDescriptor::~PBFTCommitMessageDescriptor()
{
}

bool PBFTCommitMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<PBFTCommitMessage *>(obj)!=NULL;
}

const char *PBFTCommitMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int PBFTCommitMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int PBFTCommitMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *PBFTCommitMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "view",
        "seqNumber",
        "digest",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int PBFTCommitMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='v' && strcmp(fieldName, "view")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "seqNumber")==0) return base+1;
    if (fieldName[0]=='d' && strcmp(fieldName, "digest")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *PBFTCommitMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "string",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *PBFTCommitMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int PBFTCommitMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    PBFTCommitMessage *pp = (PBFTCommitMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PBFTCommitMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTCommitMessage *pp = (PBFTCommitMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getView());
        case 1: return long2string(pp->getSeqNumber());
        case 2: return oppstring2string(pp->getDigest());
        default: return "";
    }
}

bool PBFTCommitMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    PBFTCommitMessage *pp = (PBFTCommitMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setView(string2long(value)); return true;
        case 1: pp->setSeqNumber(string2long(value)); return true;
        case 2: pp->setDigest((value)); return true;
        default: return false;
    }
}

const char *PBFTCommitMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<3) ? fieldStructNames[field] : NULL;
}

void *PBFTCommitMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTCommitMessage *pp = (PBFTCommitMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(PBFTReplyMessage);

PBFTReplyMessage::PBFTReplyMessage(const char *name, int kind) : PBFTMessage(name,kind)
{
    this->setType(REPLY);

    this->view_var = 0;
}

PBFTReplyMessage::PBFTReplyMessage(const PBFTReplyMessage& other) : PBFTMessage(other)
{
    copy(other);
}

PBFTReplyMessage::~PBFTReplyMessage()
{
}

PBFTReplyMessage& PBFTReplyMessage::operator=(const PBFTReplyMessage& other)
{
    if (this==&other) return *this;
    PBFTMessage::operator=(other);
    copy(other);
    return *this;
}

void PBFTReplyMessage::copy(const PBFTReplyMessage& other)
{
    this->view_var = other.view_var;
    this->replicaNumber_var = other.replicaNumber_var;
    this->block_var = other.block_var;
}

void PBFTReplyMessage::parsimPack(cCommBuffer *b)
{
    PBFTMessage::parsimPack(b);
    doPacking(b,this->view_var);
    doPacking(b,this->replicaNumber_var);
    doPacking(b,this->block_var);
}

void PBFTReplyMessage::parsimUnpack(cCommBuffer *b)
{
    PBFTMessage::parsimUnpack(b);
    doUnpacking(b,this->view_var);
    doUnpacking(b,this->replicaNumber_var);
    doUnpacking(b,this->block_var);
}

int PBFTReplyMessage::getView() const
{
    return view_var;
}

void PBFTReplyMessage::setView(int view)
{
    this->view_var = view;
}

OverlayKey& PBFTReplyMessage::getReplicaNumber()
{
    return replicaNumber_var;
}

void PBFTReplyMessage::setReplicaNumber(const OverlayKey& replicaNumber)
{
    this->replicaNumber_var = replicaNumber;
}

Block& PBFTReplyMessage::getBlock()
{
    return block_var;
}

void PBFTReplyMessage::setBlock(const Block& block)
{
    this->block_var = block;
}

class PBFTReplyMessageDescriptor : public cClassDescriptor
{
  public:
    PBFTReplyMessageDescriptor();
    virtual ~PBFTReplyMessageDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(PBFTReplyMessageDescriptor);

PBFTReplyMessageDescriptor::PBFTReplyMessageDescriptor() : cClassDescriptor("PBFTReplyMessage", "PBFTMessage")
{
}

PBFTReplyMessageDescriptor::~PBFTReplyMessageDescriptor()
{
}

bool PBFTReplyMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<PBFTReplyMessage *>(obj)!=NULL;
}

const char *PBFTReplyMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int PBFTReplyMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int PBFTReplyMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *PBFTReplyMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "view",
        "replicaNumber",
        "block",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int PBFTReplyMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='v' && strcmp(fieldName, "view")==0) return base+0;
    if (fieldName[0]=='r' && strcmp(fieldName, "replicaNumber")==0) return base+1;
    if (fieldName[0]=='b' && strcmp(fieldName, "block")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *PBFTReplyMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "OverlayKey",
        "Block",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *PBFTReplyMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int PBFTReplyMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    PBFTReplyMessage *pp = (PBFTReplyMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PBFTReplyMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTReplyMessage *pp = (PBFTReplyMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getView());
        case 1: {std::stringstream out; out << pp->getReplicaNumber(); return out.str();}
        case 2: {std::stringstream out; out << pp->getBlock(); return out.str();}
        default: return "";
    }
}

bool PBFTReplyMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    PBFTReplyMessage *pp = (PBFTReplyMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setView(string2long(value)); return true;
        default: return false;
    }
}

const char *PBFTReplyMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        "OverlayKey",
        "Block",
    };
    return (field>=0 && field<3) ? fieldStructNames[field] : NULL;
}

void *PBFTReplyMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTReplyMessage *pp = (PBFTReplyMessage *)object; (void)pp;
    switch (field) {
        case 1: return (void *)(&pp->getReplicaNumber()); break;
        case 2: return (void *)(&pp->getBlock()); break;
        default: return NULL;
    }
}

Register_Class(PBFTCheckpointMessage);

PBFTCheckpointMessage::PBFTCheckpointMessage(const char *name, int kind) : PBFTMessage(name,kind)
{
    this->setType(CHECKPOINT);

    this->seqNumber_var = 0;
    this->digest_var = 0;
}

PBFTCheckpointMessage::PBFTCheckpointMessage(const PBFTCheckpointMessage& other) : PBFTMessage(other)
{
    copy(other);
}

PBFTCheckpointMessage::~PBFTCheckpointMessage()
{
}

PBFTCheckpointMessage& PBFTCheckpointMessage::operator=(const PBFTCheckpointMessage& other)
{
    if (this==&other) return *this;
    PBFTMessage::operator=(other);
    copy(other);
    return *this;
}

void PBFTCheckpointMessage::copy(const PBFTCheckpointMessage& other)
{
    this->seqNumber_var = other.seqNumber_var;
    this->digest_var = other.digest_var;
}

void PBFTCheckpointMessage::parsimPack(cCommBuffer *b)
{
    PBFTMessage::parsimPack(b);
    doPacking(b,this->seqNumber_var);
    doPacking(b,this->digest_var);
}

void PBFTCheckpointMessage::parsimUnpack(cCommBuffer *b)
{
    PBFTMessage::parsimUnpack(b);
    doUnpacking(b,this->seqNumber_var);
    doUnpacking(b,this->digest_var);
}

int PBFTCheckpointMessage::getSeqNumber() const
{
    return seqNumber_var;
}

void PBFTCheckpointMessage::setSeqNumber(int seqNumber)
{
    this->seqNumber_var = seqNumber;
}

const char * PBFTCheckpointMessage::getDigest() const
{
    return digest_var.c_str();
}

void PBFTCheckpointMessage::setDigest(const char * digest)
{
    this->digest_var = digest;
}

class PBFTCheckpointMessageDescriptor : public cClassDescriptor
{
  public:
    PBFTCheckpointMessageDescriptor();
    virtual ~PBFTCheckpointMessageDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(PBFTCheckpointMessageDescriptor);

PBFTCheckpointMessageDescriptor::PBFTCheckpointMessageDescriptor() : cClassDescriptor("PBFTCheckpointMessage", "PBFTMessage")
{
}

PBFTCheckpointMessageDescriptor::~PBFTCheckpointMessageDescriptor()
{
}

bool PBFTCheckpointMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<PBFTCheckpointMessage *>(obj)!=NULL;
}

const char *PBFTCheckpointMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int PBFTCheckpointMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int PBFTCheckpointMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *PBFTCheckpointMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "seqNumber",
        "digest",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int PBFTCheckpointMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "seqNumber")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "digest")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *PBFTCheckpointMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "string",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *PBFTCheckpointMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int PBFTCheckpointMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    PBFTCheckpointMessage *pp = (PBFTCheckpointMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PBFTCheckpointMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTCheckpointMessage *pp = (PBFTCheckpointMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSeqNumber());
        case 1: return oppstring2string(pp->getDigest());
        default: return "";
    }
}

bool PBFTCheckpointMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    PBFTCheckpointMessage *pp = (PBFTCheckpointMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setSeqNumber(string2long(value)); return true;
        case 1: pp->setDigest((value)); return true;
        default: return false;
    }
}

const char *PBFTCheckpointMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
    };
    return (field>=0 && field<2) ? fieldStructNames[field] : NULL;
}

void *PBFTCheckpointMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTCheckpointMessage *pp = (PBFTCheckpointMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


