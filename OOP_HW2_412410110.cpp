#include <iostream>
#include <map>
#include <queue>
#include <utility>
#include <climits>
#include <functional>
#include <iomanip>
#include <stack>

using namespace std;

#define SET(func_name, type, var_name, _var_name) \
    void func_name(type _var_name) { var_name = _var_name; }
#define GET(func_name, type, var_name) \
    type func_name() const { return var_name; }

typedef unsigned int uint;

class header;
class payload;
class packet;
class node;
class event;
class link; // new

// for simplicity, we use a const int to simulate the delay
// if you want to simulate the more details, you should revise it to be a class
const uint ONE_HOP_DELAY = 10;
const uint BROCAST_ID = UINT_MAX;

// BROCAST_ID means that all neighbors are receivers; UINT_MAX is the maximum value of uint

class header
{
public:
    virtual ~header() {}

    SET(setSrcID, uint, srcID, _srcID);
    SET(setDstID, uint, dstID, _dstID);
    SET(setPreID, uint, preID, _preID);
    SET(setNexID, uint, nexID, _nexID);
    GET(getSrcID, uint, srcID);
    GET(getDstID, uint, dstID);
    GET(getPreID, uint, preID);
    GET(getNexID, uint, nexID);

    virtual string type() = 0;

    // factory concept: generate a header
    class header_generator
    {
        // lock the copy constructor
        header_generator(header_generator &) {}
        // store all possible types of header
        static map<string, header_generator *> prototypes;

    protected:
        // allow derived class to use it
        header_generator() {}
        // after you create a new header type, please register the factory of this header type by this function
        void register_header_type(header_generator *h) { prototypes[h->type()] = h; }
        // you have to implement your own generate() to generate your header
        virtual header *generate() = 0;

    public:
        // you have to implement your own type() to return your header type
        virtual string type() = 0;
        // this function is used to generate any type of header derived
        static header *generate(string type)
        {
            if (prototypes.find(type) != prototypes.end())
            {                                        // if this type derived exists
                return prototypes[type]->generate(); // generate it!!
            }
            std::cerr << "no such header type" << std::endl; // otherwise
            return nullptr;
        }
        static void print()
        {
            cout << "registered header types: " << endl;
            for (map<string, header::header_generator *>::iterator it = prototypes.begin(); it != prototypes.end(); it++)
                cout << it->second->type() << endl;
        }
        virtual ~header_generator() {};
    };

protected:
    header() : srcID(BROCAST_ID), dstID(BROCAST_ID), preID(BROCAST_ID), nexID(BROCAST_ID) {} // this constructor cannot be directly called by users

private:
    uint srcID;
    uint dstID;
    uint preID;
    uint nexID;
    header(header &) {} // this constructor cannot be directly called by users
};
map<string, header::header_generator *> header::header_generator::prototypes;

class TRA_data_header : public header
{
    TRA_data_header(TRA_data_header &s) : labels(s.labels) {} // cannot be called by users

    uint used_labels = 0;
    stack<uint> labels; // you can use the stack to store labels
    map<uint, bool> hi;

protected:
    TRA_data_header() {} // this constructor cannot be directly called by users

public:
    ~TRA_data_header() {}
    string type() { return "TRA_data_header"; }

    void push_label(uint _id)
    {
        if (_id == getSrcID() || _id == get_label() || getNexID() == get_label())
            used_labels += 1;
        else
            used_labels += 2;
        labels.push(_id);
    }
    void pop_label() { labels.pop(); }
    uint get_num_labels() { return labels.size(); } // although the original returned value's type size_t, we change it for simplicity
    uint get_label() { return labels.size() ? labels.top() : 0; }
    uint get_used_labels() { return used_labels; }

    void visit(uint node_id) { hi.insert({node_id, true}); }
    bool visited(uint node_id) { return !(hi.find(node_id) == hi.end()); }

    class TRA_data_header_generator;
    friend class TRA_data_header_generator;
    // TRA_data_header_generator is derived from header_generator to generate a header
    class TRA_data_header_generator : public header_generator
    {
        static TRA_data_header_generator sample;
        // this constructor is only for sample to register this header type
        TRA_data_header_generator() { /*cout << "TRA_data_header registered" << endl;*/ register_header_type(&sample); }

    protected:
        virtual header *generate()
        {
            // cout << "TRA_data_header generated" << endl;
            return new TRA_data_header;
        }

    public:
        virtual string type() { return "TRA_data_header"; }
        ~TRA_data_header_generator() {}
    };
};
TRA_data_header::TRA_data_header_generator TRA_data_header::TRA_data_header_generator::sample;

class TRA_ctrl_header : public header
{
    TRA_ctrl_header(TRA_ctrl_header &) {} // cannot be called by users

protected:
    TRA_ctrl_header() {} // this constructor cannot be directly called by users

public:
    ~TRA_ctrl_header() {}
    string type() { return "TRA_ctrl_header"; }

    class TRA_ctrl_header_generator;
    friend class TRA_ctrl_header_generator;
    // TRA_ctrl_header_generator is derived from header_generator to generate a header
    class TRA_ctrl_header_generator : public header_generator
    {
        static TRA_ctrl_header_generator sample;
        // this constructor is only for sample to register this header type
        TRA_ctrl_header_generator() { /*cout << "TRA_ctrl_header registered" << endl;*/ register_header_type(&sample); }

    protected:
        virtual header *generate()
        {
            // cout << "TRA_ctrl_header generated" << endl;
            return new TRA_ctrl_header;
        }

    public:
        virtual string type() { return "TRA_ctrl_header"; }
        ~TRA_ctrl_header_generator() {}
    };
};
TRA_ctrl_header::TRA_ctrl_header_generator TRA_ctrl_header::TRA_ctrl_header_generator::sample;

class payload
{
    payload(payload &) {} // this constructor cannot be directly called by users

    string msg;

protected:
    payload() {}

public:
    virtual ~payload() {}
    virtual string type() = 0;

    SET(setMsg, string, msg, _msg);
    GET(getMsg, string, msg);

    class payload_generator
    {
        // lock the copy constructor
        payload_generator(payload_generator &) {}
        // store all possible types of header
        static map<string, payload_generator *> prototypes;

    protected:
        // allow derived class to use it
        payload_generator() {}
        // after you create a new payload type, please register the factory of this payload type by this function
        void register_payload_type(payload_generator *h) { prototypes[h->type()] = h; }
        // you have to implement your own generate() to generate your payload
        virtual payload *generate() = 0;

    public:
        // you have to implement your own type() to return your header type
        virtual string type() = 0;
        // this function is used to generate any type of header derived
        static payload *generate(string type)
        {
            if (prototypes.find(type) != prototypes.end())
            {                                        // if this type derived exists
                return prototypes[type]->generate(); // generate it!!
            }
            std::cerr << "no such payload type" << std::endl; // otherwise
            return nullptr;
        }
        static void print()
        {
            cout << "registered payload types: " << endl;
            for (map<string, payload::payload_generator *>::iterator it = prototypes.begin(); it != prototypes.end(); it++)
                cout << it->second->type() << endl;
        }
        virtual ~payload_generator() {};
    };
};
map<string, payload::payload_generator *> payload::payload_generator::prototypes;

class TRA_data_payload : public payload
{
    TRA_data_payload(TRA_data_payload &) {}

protected:
    TRA_data_payload() {} // this constructor cannot be directly called by users
public:
    ~TRA_data_payload() {}

    string type() { return "TRA_data_payload"; }

    class TRA_data_payload_generator;
    friend class TRA_data_payload_generator;
    // TRA_data_payload is derived from payload_generator to generate a payload
    class TRA_data_payload_generator : public payload_generator
    {
        static TRA_data_payload_generator sample;
        // this constructor is only for sample to register this payload type
        TRA_data_payload_generator() { /*cout << "TRA_data_payload registered" << endl;*/ register_payload_type(&sample); }

    protected:
        virtual payload *generate()
        {
            // cout << "TRA_data_payload generated" << endl;
            return new TRA_data_payload;
        }

    public:
        virtual string type() { return "TRA_data_payload"; }
        ~TRA_data_payload_generator() {}
    };
};
TRA_data_payload::TRA_data_payload_generator TRA_data_payload::TRA_data_payload_generator::sample;

class TRA_ctrl_payload : public payload
{
    TRA_ctrl_payload(TRA_ctrl_payload &s) : n_id(s.n_id), netw_info(s.netw_info) {} // counter (s.counter) {}

    // uint counter ;
    unsigned n_id = 0; // default is zero
    map<uint, pair<double, double>> netw_info;
    // nodeID, <first: capacity, second: occupied>

protected:
    TRA_ctrl_payload() {} // : counter (0) {} // this constructor cannot be directly called by users
public:
    ~TRA_ctrl_payload() {}

    // void increase() { counter ++; } // used to increase the counter
    // GET(getCounter,uint,counter); // used to get the value of counter
    SET(setnid, uint, n_id, _n_id);
    GET(getnid, uint, n_id);
    void addNetwInfo(uint nb_id, double link_capacity, double occupied)
    {
        //  if (netw_info.find(nb_id) == netw_info.end())
        netw_info[nb_id] = make_pair(link_capacity, occupied);
    }
    map<uint, pair<double, double>> getNetwInfo()
    {
        return netw_info;
    }

    string type() { return "TRA_ctrl_payload"; }

    class TRA_ctrl_payload_generator;
    friend class TRA_ctrl_payload_generator;
    // TRA_data_payload is derived from payload_generator to generate a payload
    class TRA_ctrl_payload_generator : public payload_generator
    {
        static TRA_ctrl_payload_generator sample;
        // this constructor is only for sample to register this payload type
        TRA_ctrl_payload_generator() { /*cout << "TRA_ctrl_payload registered" << endl;*/ register_payload_type(&sample); }

    protected:
        virtual payload *generate()
        {
            // cout << "TRA_ctrl_payload generated" << endl;
            return new TRA_ctrl_payload;
        }

    public:
        virtual string type() { return "TRA_ctrl_payload"; }
        ~TRA_ctrl_payload_generator() {}
    };
};
TRA_ctrl_payload::TRA_ctrl_payload_generator TRA_ctrl_payload::TRA_ctrl_payload_generator::sample;

class packet
{
    // a packet usually contains a header and a payload
    header *hdr;
    payload *pld;
    uint p_id;
    static uint last_packet_id;

    packet(packet &) {}
    static int live_packet_num;

protected:
    double size = 0; // packet size (flow size) is set to 0 by default (especially for control msg)

    // these constructors cannot be directly called by users
    packet() : hdr(nullptr), pld(nullptr)
    {
        p_id = last_packet_id++;
        live_packet_num++;
    }
    packet(string _hdr, string _pld, double _size = 0, bool rep = false, uint rep_id = 0)
    {
        if (!rep) // a duplicated packet does not have a new packet id
            p_id = last_packet_id++;
        else
            p_id = rep_id;
        hdr = header::header_generator::generate(_hdr);
        pld = payload::payload_generator::generate(_pld);
        size = _size;
        live_packet_num++;
    }

public:
    virtual ~packet()
    {
        // cout << "packet destructor begin" << endl;
        if (hdr != nullptr)
            delete hdr;
        if (pld != nullptr)
            delete pld;
        live_packet_num--;
        // cout << "packet destructor end" << endl;
    }

    SET(setHeader, header *, hdr, _hdr);
    GET(getHeader, header *, hdr);
    SET(setPayload, payload *, pld, _pld);
    GET(getPayload, payload *, pld);
    GET(getPacketID, uint, p_id);

    SET(setSize, uint, size, _size);
    GET(getSize, uint, size);

    static void discard(packet *&p)
    {
        // cout << "checking" << endl;
        if (p != nullptr)
        {
            // cout << "discarding" << endl;
            // cout << p->type() << endl;
            delete p;
            // cout << "discarded" << endl;
        }
        p = nullptr;
        // cout << "checked" << endl;
    }
    virtual string type() = 0;
    // you can define your own packet's addition_information
    // to print more information for recv_event and send_event
    virtual string addition_information() { return ""; }

    static int getLivePacketNum() { return live_packet_num; }

    class packet_generator;
    friend class packet_generator;
    class packet_generator
    {
        // lock the copy constructor
        packet_generator(packet_generator &) {}
        // store all possible types of packet
        static map<string, packet_generator *> prototypes;

    protected:
        // allow derived class to use it
        packet_generator() {}
        // after you create a new packet type, please register the factory of this payload type by this function
        void register_packet_type(packet_generator *h) { prototypes[h->type()] = h; }
        // you have to implement your own generate() to generate your payload
        virtual packet *generate(packet *p = nullptr) = 0;

    public:
        // you have to implement your own type() to return your packet type
        virtual string type() = 0;
        // this function is used to generate any type of packet derived
        static packet *generate(string type)
        {
            if (prototypes.find(type) != prototypes.end())
            {                                        // if this type derived exists
                return prototypes[type]->generate(); // generate it!!
            }
            std::cerr << "no such packet type" << std::endl; // otherwise
            return nullptr;
        }
        static packet *replicate(packet *p)
        {
            if (prototypes.find(p->type()) != prototypes.end())
            {                                              // if this type derived exists
                return prototypes[p->type()]->generate(p); // generate it!!
            }
            std::cerr << "no such packet type" << std::endl; // otherwise
            return nullptr;
        }
        static void print()
        {
            cout << "registered packet types: " << endl;
            for (map<string, packet::packet_generator *>::iterator it = prototypes.begin(); it != prototypes.end(); it++)
                cout << it->second->type() << endl;
        }
        virtual ~packet_generator() {};
    };
};
map<string, packet::packet_generator *> packet::packet_generator::prototypes;
uint packet::last_packet_id = 0;
int packet::live_packet_num = 0;

// this packet is used to tell the destination the msg
class TRA_data_packet : public packet
{
    TRA_data_packet(TRA_data_packet &) {}

protected:
    TRA_data_packet() {} // this constructor cannot be directly called by users
    TRA_data_packet(packet *p) : packet(p->getHeader()->type(), p->getPayload()->type(), p->getSize(), true, p->getPacketID())
    {
        *(dynamic_cast<TRA_data_header *>(this->getHeader())) = *(dynamic_cast<TRA_data_header *>(p->getHeader()));
        *(dynamic_cast<TRA_data_payload *>(this->getPayload())) = *(dynamic_cast<TRA_data_payload *>(p->getPayload()));
        // DFS_path = (dynamic_cast<TRA_data_header*>(p))->DFS_path;
        // isVisited = (dynamic_cast<TRA_data_header*>(p))->isVisited;
    } // for duplicate
    TRA_data_packet(string _h, string _p, double _size = 0) : packet(_h, _p, _size) {}

public:
    virtual ~TRA_data_packet() {}
    string type() { return "TRA_data_packet"; }
    virtual string addition_information()
    {
        double size = getSize();
        // cout << counter << endl;
        TRA_data_header *_hdr = dynamic_cast<TRA_data_header *>(getHeader());
        uint label = 0;
        if (_hdr && _hdr->get_num_labels())
            return " label " + to_string(_hdr->get_label()); // " size " + to_string(size);
        return " label x";
    }

    class TRA_data_packet_generator;
    friend class TRA_data_packet_generator;
    // TRA_data_packet is derived from packet_generator to generate a pub packet
    class TRA_data_packet_generator : public packet_generator
    {
        static TRA_data_packet_generator sample;
        // this constructor is only for sample to register this packet type
        TRA_data_packet_generator() { /*cout << "TRA_data_packet registered" << endl;*/ register_packet_type(&sample); }

    protected:
        virtual packet *generate(packet *p = nullptr)
        {
            // cout << "TRA_data_packet generated" << endl;
            if (nullptr == p)
                return new TRA_data_packet("TRA_data_header", "TRA_data_payload"); // generate a new packet
            else
                return new TRA_data_packet(p); // duplicate
        }

    public:
        virtual string type() { return "TRA_data_packet"; }
        ~TRA_data_packet_generator() {}
    };
};
TRA_data_packet::TRA_data_packet_generator TRA_data_packet::TRA_data_packet_generator::sample;

// this packet type is used to tell the node what should be installed in the routing table
class TRA_ctrl_packet : public packet
{
    TRA_ctrl_packet(TRA_ctrl_packet &) {}

protected:
    TRA_ctrl_packet() {} // this constructor cannot be directly called by users
    TRA_ctrl_packet(packet *p) : packet(p->getHeader()->type(), p->getPayload()->type(), p->getSize(), true, p->getPacketID())
    {
        *(dynamic_cast<TRA_ctrl_header *>(this->getHeader())) = *(dynamic_cast<TRA_ctrl_header *>(p->getHeader()));
        *(dynamic_cast<TRA_ctrl_payload *>(this->getPayload())) = *(dynamic_cast<TRA_ctrl_payload *>(p->getPayload()));
        // DFS_path = (dynamic_cast<TRA_ctrl_header*>(p))->DFS_path;
        // isVisited = (dynamic_cast<TRA_ctrl_header*>(p))->isVisited;
    } // for duplicate
    TRA_ctrl_packet(string _h, string _p, double _size = 0) : packet(_h, _p, _size) {}

public:
    virtual ~TRA_ctrl_packet() {}
    string type() { return "TRA_ctrl_packet"; }
    virtual string addition_information()
    {
        // uint counter = (dynamic_cast<TRA_ctrl_payload*>(this->getPayload()))->getCounter();
        uint id = (dynamic_cast<TRA_ctrl_payload *>(this->getPayload()))->getnid();
        // cout << counter << endl;
        return " from " + to_string(id);
    }

    class TRA_ctrl_packet_generator;
    friend class TRA_ctrl_packet_generator;
    // TRA_ctrl_packet is derived from packet_generator to generate a pub packet
    class TRA_ctrl_packet_generator : public packet_generator
    {
        static TRA_ctrl_packet_generator sample;
        // this constructor is only for sample to register this packet type
        TRA_ctrl_packet_generator() { /*cout << "TRA_ctrl_packet registered" << endl;*/ register_packet_type(&sample); }

    protected:
        virtual packet *generate(packet *p = nullptr)
        {
            // cout << "TRA_ctrl_packet generated" << endl;
            if (nullptr == p)
                return new TRA_ctrl_packet("TRA_ctrl_header", "TRA_ctrl_payload"); // generate a new packet
            else
                return new TRA_ctrl_packet(p); // duplicate
        }

    public:
        virtual string type() { return "TRA_ctrl_packet"; }
        ~TRA_ctrl_packet_generator() {}
    };
};
TRA_ctrl_packet::TRA_ctrl_packet_generator TRA_ctrl_packet::TRA_ctrl_packet_generator::sample;

class node
{
    // all nodes created in the program
    static map<uint, node *> id_node_table;
    map<uint, bool> phy_neighbors;
    uint id;
    uint num_of_label;

protected:
    node(node &) {} // this constructor should not be used
    node() {}       // this constructor should not be used
    node(uint _id) : id(_id) { id_node_table[_id] = this; }
    virtual void print_to(ostream &os) const { os << id << endl; }

public:
    virtual ~node()
    { // erase the node
        id_node_table.erase(id);
    }
    friend ostream &operator<<(ostream &os, const node &n);

    virtual string type() = 0; // please define it in your derived node class

    void add_phy_neighbor(uint _id, string link_type = "simple_link", map<string, double> link_args = {}); // we only add a directed link from id to _id
    void del_phy_neighbor(uint _id);                                                                       // we only delete a directed link from id to _id

    // you can use the function to get the node's neigbhors at this time
    const map<uint, bool> &getPhyNeighbors()
    {
        return phy_neighbors;
    }

    link *getLink(uint nb_id);

    void recv(packet *p)
    {
        packet *tp = p;
        recv_handler(tp);
        packet::discard(p);
    } // the packet will be directly deleted after the handler
    void send(packet *p);

    // receive the packet and do something; this is a pure virtual function
    virtual void recv_handler(packet *p) = 0;
    void send_handler(packet *P);

    static node *id_to_node(uint _id) { return ((id_node_table.find(_id) != id_node_table.end()) ? id_node_table[_id] : nullptr); }
    GET(getNodeID, uint, id);
    GET(getNumOfLabel, uint, num_of_label);
    SET(setNumOfLabel, uint, num_of_label, _num_of_label);

    static void del_node(uint _id)
    {
        if (id_node_table.find(_id) != id_node_table.end())
            id_node_table.erase(_id);
    }
    static uint getNodeNum() { return id_node_table.size(); }

    class node_generator
    {
        // lock the copy constructor
        node_generator(node_generator &) {}
        // store all possible types of node
        static map<string, node_generator *> prototypes;

    protected:
        // allow derived class to use it
        node_generator() {}
        // after you create a new node type, please register the factory of this node type by this function
        void register_node_type(node_generator *h) { prototypes[h->type()] = h; }
        // you have to implement your own generate() to generate your node
        virtual node *generate(uint _id) = 0;

    public:
        // you have to implement your own type() to return your node type
        virtual string type() = 0;
        // this function is used to generate any type of node derived
        static node *generate(string type, uint _id)
        {
            if (id_node_table.find(_id) != id_node_table.end())
            {
                std::cerr << "duplicate node id" << std::endl; // node id is duplicated
                return nullptr;
            }
            else if (BROCAST_ID == _id)
            {
                cerr << "BROCAST_ID cannot be used" << endl;
                return nullptr;
            }
            else if (prototypes.find(type) != prototypes.end())
            { // if this type derived exists
                node *created_node = prototypes[type]->generate(_id);
                return created_node; // generate it!!
            }
            std::cerr << "no such node type" << std::endl; // otherwise
            return nullptr;
        }
        static void print()
        {
            cout << "registered node types: " << endl;
            for (map<string, node::node_generator *>::iterator it = prototypes.begin(); it != prototypes.end(); it++)
                cout << it->second->type() << endl;
        }
        virtual ~node_generator() {};
    };
};
map<string, node::node_generator *> node::node_generator::prototypes;
map<uint, node *> node::id_node_table;

class TRA_switch : public node
{
    // you can extract the data structure for storing nodes and links here from hw1
    // note the data stored here should be local and cannot be accessed by the other nodes directly
    map<uint, uint> last_p_id_from_node;
    map<uint, vector<uint>> entry_table;
    map<pair<uint, uint>, pair<double, double>> network; // {(src, nb), (capacity, occupied)}

protected:
    TRA_switch() {}                     // it should not be used
    TRA_switch(TRA_switch &) {}         // it should not be used
    TRA_switch(uint _id) : node(_id) {} // this constructor cannot be directly called by users
    virtual void print_to(ostream &os) const override;

public:
    ~TRA_switch() {}
    string type() { return "TRA_switch"; }

    // please define recv_handler function to deal with the incoming packet
    bool isNewPacket(packet *p);
    virtual void recv_handler(packet *p);

    double getCapacity(uint nb_id);
    double getOccupied(uint nb_id);

    // void add_one_hop_neighbor (uint n_id) { one_hop_neighbors[n_id] = true; }
    // uint get_one_hop_neighbor_num () { return one_hop_neighbors.size(); }

    class TRA_switch_generator;
    friend class TRA_switch_generator;
    // TRA_switch is derived from node_generator to generate a node
    class TRA_switch_generator : public node_generator
    {
        static TRA_switch_generator sample;
        // this constructor is only for sample to register this node type
        TRA_switch_generator() { /*cout << "TRA_switch registered" << endl;*/ register_node_type(&sample); }

    protected:
        virtual node *generate(uint _id) { /*cout << "TRA_switch generated" << endl;*/ return new TRA_switch(_id); }

    public:
        virtual string type() { return "TRA_switch"; }
        ~TRA_switch_generator() {}
    };
};
TRA_switch::TRA_switch_generator TRA_switch::TRA_switch_generator::sample;

class mycomp
{
    bool reverse;

public:
    mycomp(const bool &revparam = false) { reverse = revparam; }
    bool operator()(const event *lhs, const event *rhs) const;
};

class event
{
    event(event *&) {} // this constructor cannot be directly called by users
    static priority_queue<event *, vector<event *>, mycomp> events;
    static uint cur_time; // timer
    static uint end_time;

    // get the next event
    static event *get_next_event();
    static void add_event(event *e) { events.push(e); }
    static hash<string> event_seq;

protected:
    uint trigger_time;

    event() {} // it should not be used
    event(uint _trigger_time) : trigger_time(_trigger_time) {}

public:
    virtual void trigger() = 0;
    virtual ~event() {}

    virtual uint event_priority() const = 0;
    uint get_hash_value(string string_for_hash) const
    {
        uint priority = event_seq(string_for_hash);
        return priority;
    }

    static void flush_events(); // only for debug

    GET(getTriggerTime, uint, trigger_time);

    static void start_simulate(uint _end_time); // the function is used to start the simulation

    static uint getCurTime() { return cur_time; }
    static void getCurTime(uint _cur_time) { cur_time = _cur_time; }
    // static uint getEndTime() { return end_time ; }
    // static void getEndTime(uint _end_time) { end_time = _end_time; }

    virtual void print() const = 0; // the function is used to print the event information

    class event_generator
    {
        // lock the copy constructor
        event_generator(event_generator &) {}
        // store all possible types of event
        static map<string, event_generator *> prototypes;

    protected:
        // allow derived class to use it
        event_generator() {}
        // after you create a new event type, please register the factory of this event type by this function
        void register_event_type(event_generator *h) { prototypes[h->type()] = h; }
        // you have to implement your own generate() to generate your event
        virtual event *generate(uint _trigger_time, void *data) = 0;

    public:
        // you have to implement your own type() to return your event type
        virtual string type() = 0;
        // this function is used to generate any type of event derived
        static event *generate(string type, uint _trigger_time, void *data)
        {
            if (prototypes.find(type) != prototypes.end())
            { // if this type derived exists
                event *e = prototypes[type]->generate(_trigger_time, data);
                add_event(e);
                return e; // generate it!!
            }
            std::cerr << "no such event type" << std::endl; // otherwise
            return nullptr;
        }
        static void print()
        {
            cout << "registered event types: " << endl;
            for (map<string, event::event_generator *>::iterator it = prototypes.begin(); it != prototypes.end(); it++)
                cout << it->second->type() << endl;
        }
        virtual ~event_generator() {}
    };
};
map<string, event::event_generator *> event::event_generator::prototypes;
priority_queue<event *, vector<event *>, mycomp> event::events;
hash<string> event::event_seq;

uint event::cur_time = 0;
uint event::end_time = 0;

void event::flush_events()
{
    cout << "**flush begin" << endl;
    while (!events.empty())
    {
        cout << setw(11) << events.top()->trigger_time << ": " << setw(11) << events.top()->event_priority() << endl;
        delete events.top();
        events.pop();
    }
    cout << "**flush end" << endl;
}
event *event::get_next_event()
{
    if (events.empty())
        return nullptr;
    event *e = events.top();
    events.pop();
    // cout << events.size() << " events remains" << endl;
    return e;
}
void event::start_simulate(uint _end_time)
{
    if (_end_time < 0)
    {
        cerr << "you should give a possitive value of _end_time" << endl;
        return;
    }
    end_time = _end_time;
    event *e;
    e = event::get_next_event();
    while (e != nullptr && e->trigger_time <= end_time)
    {
        if (cur_time <= e->trigger_time)
            cur_time = e->trigger_time;
        else
        {
            cerr << "cur_time = " << cur_time << ", event trigger_time = " << e->trigger_time << endl;
            break;
        }

        // cout << "event trigger_time = " << e->trigger_time << endl;
        e->print(); // for log
        // cout << " event begin" << endl;
        e->trigger();
        // cout << " event end" << endl;
        delete e;
        e = event::get_next_event();
    }
    // cout << "no more event" << endl;
}

bool mycomp::operator()(const event *lhs, const event *rhs) const
{
    // cout << lhs->getTriggerTime() << ", " << rhs->getTriggerTime() << endl;
    // cout << lhs->type() << ", " << rhs->type() << endl;
    uint lhs_pri = lhs->event_priority();
    uint rhs_pri = rhs->event_priority();
    // cout << "lhs hash = " << lhs_pri << endl;
    // cout << "rhs hash = " << rhs_pri << endl;

    if (reverse)
        return ((lhs->getTriggerTime()) == (rhs->getTriggerTime())) ? (lhs_pri < rhs_pri) : ((lhs->getTriggerTime()) < (rhs->getTriggerTime()));
    else
        return ((lhs->getTriggerTime()) == (rhs->getTriggerTime())) ? (lhs_pri > rhs_pri) : ((lhs->getTriggerTime()) > (rhs->getTriggerTime()));
}

class recv_event : public event
{
public:
    class recv_data; // forward declaration

private:
    recv_event(recv_event &) {} // this constructor cannot be used
    recv_event() {}             // we don't allow users to new a recv_event by themselv
    uint senderID;              // the sender
    uint receiverID;            // the receiver; the packet will be given to the receiver
    packet *pkt;                // the packet

protected:
    // this constructor cannot be directly called by users; only by generator
    recv_event(uint _trigger_time, void *data) : event(_trigger_time), senderID(BROCAST_ID), receiverID(BROCAST_ID), pkt(nullptr)
    {
        recv_data *data_ptr = (recv_data *)data;
        senderID = data_ptr->s_id;
        receiverID = data_ptr->r_id; // the packet will be given to the receiver
        pkt = data_ptr->_pkt;
    }

public:
    virtual ~recv_event() {}
    // recv_event will trigger the recv function
    virtual void trigger();

    uint event_priority() const;

    class recv_event_generator;
    friend class recv_event_generator;
    // recv_event is derived from event_generator to generate a event
    class recv_event_generator : public event_generator
    {
        static recv_event_generator sample;
        // this constructor is only for sample to register this event type
        recv_event_generator() { /*cout << "recv_event registered" << endl;*/ register_event_type(&sample); }

    protected:
        virtual event *generate(uint _trigger_time, void *data)
        {
            // cout << "recv_event generated" << endl;
            return new recv_event(_trigger_time, data);
        }

    public:
        virtual string type() { return "recv_event"; }
        ~recv_event_generator() {}
    };
    // this class is used to initialize the recv_event
    class recv_data
    {
    public:
        uint s_id;
        uint r_id;
        packet *_pkt;
    };

    void print() const;
};
recv_event::recv_event_generator recv_event::recv_event_generator::sample;

void recv_event::trigger()
{
    if (pkt == nullptr)
    {
        cerr << "recv_event error: no pkt!" << endl;
        return;
    }
    else if (node::id_to_node(receiverID) == nullptr)
    {
        cerr << "recv_event error: no node " << receiverID << "!" << endl;
        delete pkt;
        return;
    }
    node::id_to_node(receiverID)->recv(pkt);
}
uint recv_event::event_priority() const
{
    string string_for_hash;
    string_for_hash = to_string(getTriggerTime()) + to_string(senderID) + to_string(receiverID) + to_string(pkt->getPacketID());
    return get_hash_value(string_for_hash);
}
// the recv_event::print() function is used for log file
void recv_event::print() const
{
    cout << "time " << setw(11) << event::getCurTime()
         << "   recID" << setw(11) << receiverID
         << "   pktID" << setw(11) << pkt->getPacketID()
         << "   srcID" << setw(11) << pkt->getHeader()->getSrcID()
         << "   dstID" << setw(11) << pkt->getHeader()->getDstID()
         << "   preID" << setw(11) << pkt->getHeader()->getPreID()
         << "   nexID" << setw(11) << pkt->getHeader()->getNexID()
         << "   " << pkt->type()
         << pkt->addition_information();
    //  if ( pkt->type() == "TRA_ctrl_packet" ) cout << "   " << ((TRA_ctrl_payload*)pkt->getPayload())->getCounter();
    cout << endl;
    // cout << pkt->type()
    //      << "   time "       << setw(11) << event::getCurTime()
    //      << "   recID "      << setw(11) << receiverID
    //      << "   pktID"       << setw(11) << pkt->getPacketID()
    //      << "   srcID "      << setw(11) << pkt->getHeader()->getSrcID()
    //      << "   dstID"       << setw(11) << pkt->getHeader()->getDstID()
    //      << "   preID"       << setw(11) << pkt->getHeader()->getPreID()
    //      << "   nexID"       << setw(11) << pkt->getHeader()->getNexID()
    //      << endl;
}

class send_event : public event
{
public:
    class send_data; // forward declaration

private:
    send_event(send_event &) {}
    send_event() {} // we don't allow users to new a recv_event by themselves
    // this constructor cannot be directly called by users; only by generator
    uint senderID;   // the sender
    uint receiverID; // the receiver
    packet *pkt;     // the packet

protected:
    send_event(uint _trigger_time, void *data) : event(_trigger_time), senderID(BROCAST_ID), receiverID(BROCAST_ID), pkt(nullptr)
    {
        send_data *data_ptr = (send_data *)data;
        senderID = data_ptr->s_id;
        receiverID = data_ptr->r_id;
        pkt = data_ptr->_pkt;
    }

public:
    virtual ~send_event() {}
    // send_event will trigger the send function
    virtual void trigger();

    uint event_priority() const;

    class send_event_generator;
    friend class send_event_generator;
    // send_event is derived from event_generator to generate a event
    class send_event_generator : public event_generator
    {
        static send_event_generator sample;
        // this constructor is only for sample to register this event type
        send_event_generator() { /*cout << "send_event registered" << endl;*/ register_event_type(&sample); }

    protected:
        virtual event *generate(uint _trigger_time, void *data)
        {
            // cout << "send_event generated" << endl;
            return new send_event(_trigger_time, data);
        }

    public:
        virtual string type() { return "send_event"; }
        ~send_event_generator() {}
    };
    // this class is used to initialize the send_event
    class send_data
    {
    public:
        uint s_id;
        uint r_id;
        packet *_pkt;
        uint t;
    };

    void print() const;
};
send_event::send_event_generator send_event::send_event_generator::sample;

void send_event::trigger()
{
    if (pkt == nullptr)
    {
        cerr << "send_event error: no pkt!" << endl;
        return;
    }
    else if (node::id_to_node(senderID) == nullptr)
    {
        cerr << "send_event error: no node " << senderID << "!" << endl;
        delete pkt;
        return;
    }
    node::id_to_node(senderID)->send(pkt);
}
uint send_event::event_priority() const
{
    string string_for_hash;
    string_for_hash = to_string(getTriggerTime()) + to_string(senderID) + to_string(receiverID) + to_string(pkt->getPacketID());
    return get_hash_value(string_for_hash);
}
// the send_event::print() function is used for log file
void send_event::print() const
{
    cout << "time " << setw(11) << event::getCurTime()
         << "   senID" << setw(11) << senderID
         << "   pktID" << setw(11) << pkt->getPacketID()
         << "   srcID" << setw(11) << pkt->getHeader()->getSrcID()
         << "   dstID" << setw(11) << pkt->getHeader()->getDstID()
         << "   preID" << setw(11) << pkt->getHeader()->getPreID()
         << "   nexID" << setw(11) << pkt->getHeader()->getNexID()
         << "   " << pkt->type()
         << pkt->addition_information()
         //<< "   msg"         << setw(11) << dynamic_cast<TRA_data_payload*>(pkt->getPayload())->getMsg()
         << endl;
    // cout << pkt->type()
    //      << "   time "       << setw(11) << event::getCurTime()
    //      << "   senID "      << setw(11) << senderID
    //      << "   pktID"       << setw(11) << pkt->getPacketID()
    //      << "   srcID "      << setw(11) << pkt->getHeader()->getSrcID()
    //      << "   dstID"       << setw(11) << pkt->getHeader()->getDstID()
    //      << "   preID"       << setw(11) << pkt->getHeader()->getPreID()
    //      << "   nexID"       << setw(11) << pkt->getHeader()->getNexID()
    //      << endl;
}

////////////////////////////////////////////////////////////////////////////////

class TRA_data_pkt_gen_event : public event
{
public:
    class gen_data; // forward declaration

private:
    TRA_data_pkt_gen_event(TRA_data_pkt_gen_event &) {}
    TRA_data_pkt_gen_event() {} // we don't allow users to new a recv_event by themselves
    // this constructor cannot be directly called by users; only by generator
    uint src; // the src
    uint dst; // the dst
    double size;
    // packet *pkt; // the packet
    string msg;

protected:
    TRA_data_pkt_gen_event(uint _trigger_time, void *data) : event(_trigger_time), src(BROCAST_ID), dst(BROCAST_ID)
    {
        pkt_gen_data *data_ptr = (pkt_gen_data *)data;
        src = data_ptr->src_id;
        dst = data_ptr->dst_id;
        size = data_ptr->_size;
        // pkt = data_ptr->_pkt;
        msg = data_ptr->msg;
    }

public:
    virtual ~TRA_data_pkt_gen_event() {}
    // TRA_data_pkt_gen_event will trigger the packet gen function
    virtual void trigger();

    uint event_priority() const;

    class TRA_data_pkt_gen_event_generator;
    friend class TRA_data_pkt_gen_event_generator;
    // TRA_data_pkt_gen_event_generator is derived from event_generator to generate an event
    class TRA_data_pkt_gen_event_generator : public event_generator
    {
        static TRA_data_pkt_gen_event_generator sample;
        // this constructor is only for sample to register this event type
        TRA_data_pkt_gen_event_generator() { /*cout << "send_event registered" << endl;*/ register_event_type(&sample); }

    protected:
        virtual event *generate(uint _trigger_time, void *data)
        {
            // cout << "send_event generated" << endl;
            return new TRA_data_pkt_gen_event(_trigger_time, data);
        }

    public:
        virtual string type() { return "TRA_data_pkt_gen_event"; }
        ~TRA_data_pkt_gen_event_generator() {}
    };
    // this class is used to initialize the TRA_data_pkt_gen_event
    class pkt_gen_data
    {
    public:
        uint src_id;
        uint dst_id;
        double _size;
        string msg;
        // packet *_pkt;
    };

    void print() const;
};
TRA_data_pkt_gen_event::TRA_data_pkt_gen_event_generator TRA_data_pkt_gen_event::TRA_data_pkt_gen_event_generator::sample;

void TRA_data_pkt_gen_event::trigger()
{
    if (node::id_to_node(src) == nullptr)
    {
        cerr << "TRA_data_pkt_gen_event error: no node " << src << "!" << endl;
        return;
    }
    else if (dst != BROCAST_ID && node::id_to_node(dst) == nullptr)
    {
        cerr << "TRA_data_pkt_gen_event error: no node " << dst << "!" << endl;
        return;
    }

    TRA_data_packet *pkt = dynamic_cast<TRA_data_packet *>(packet::packet_generator::generate("TRA_data_packet"));
    if (pkt == nullptr)
    {
        cerr << "packet type is incorrect" << endl;
        return;
    }
    TRA_data_header *hdr = dynamic_cast<TRA_data_header *>(pkt->getHeader());
    TRA_data_payload *pld = dynamic_cast<TRA_data_payload *>(pkt->getPayload());

    if (hdr == nullptr)
    {
        cerr << "header type is incorrect" << endl;
        return;
    }
    if (pld == nullptr)
    {
        cerr << "payload type is incorrect" << endl;
        return;
    }

    hdr->setSrcID(src);
    hdr->setDstID(dst);
    hdr->setPreID(src); // this column is not important when the packet is first received by the src (i.e., just generated)
    hdr->setNexID(src); // this column is not important when the packet is first received by the src (i.e., just generated)

    pld->setMsg(msg);

    pkt->setSize(size);

    // cout << "**size = " << pkt->getSize() << "**" << endl ;

    recv_event::recv_data e_data;
    e_data.s_id = src;
    e_data.r_id = src; // to make the packet start from the src
    e_data._pkt = pkt;

    recv_event *e = dynamic_cast<recv_event *>(event::event_generator::generate("recv_event", trigger_time, (void *)&e_data));
}
uint TRA_data_pkt_gen_event::event_priority() const
{
    string string_for_hash;
    string_for_hash = to_string(getTriggerTime()) + to_string(src) + to_string(dst); // to_string (pkt->getPacketID());
    return get_hash_value(string_for_hash);
}
// the TRA_data_pkt_gen_event::print() function is used for log file
void TRA_data_pkt_gen_event::print() const
{
    cout << "time " << setw(11) << event::getCurTime()
         << "        " << setw(11) << " "
         << "        " << setw(11) << " "
         << "   srcID" << setw(11) << src
         << "   dstID" << setw(11) << dst
         << "        " << setw(11) << " "
         << "        " << setw(11) << " "
         << "   TRA_data_packet generating" // << setw(5) << size
         << endl;
}

class TRA_ctrl_pkt_gen_event : public event
{
public:
    class gen_data; // forward declaration

private:
    TRA_ctrl_pkt_gen_event(TRA_ctrl_pkt_gen_event &) {}
    TRA_ctrl_pkt_gen_event() {} // we don't allow users to new a recv_event by themselves
    // this constructor cannot be directly called by users; only by generator
    uint src; // the src
    uint dst; // the dst
    double size;
    // uint mat;
    // uint act;
    // packet *pkt; // the packet
    string msg;
    // double per; // percentage

protected:
    TRA_ctrl_pkt_gen_event(uint _trigger_time, void *data) : event(_trigger_time), src(BROCAST_ID), dst(BROCAST_ID)
    {
        pkt_gen_data *data_ptr = (pkt_gen_data *)data;
        src = data_ptr->src_id;
        dst = data_ptr->dst_id;
        size = data_ptr->_size;
        // pkt = data_ptr->_pkt;
        // mat = data_ptr->mat_id;
        // act = data_ptr->act_id;
        msg = data_ptr->msg;
        // per = data_ptr->per;
    }

public:
    virtual ~TRA_ctrl_pkt_gen_event() {}
    // TRA_ctrl_pkt_gen_event will trigger the packet gen function
    virtual void trigger();

    uint event_priority() const;

    class TRA_ctrl_pkt_gen_event_generator;
    friend class TRA_ctrl_pkt_gen_event_generator;
    // TRA_ctrl_pkt_gen_event_generator is derived from event_generator to generate an event
    class TRA_ctrl_pkt_gen_event_generator : public event_generator
    {
        static TRA_ctrl_pkt_gen_event_generator sample;
        // this constructor is only for sample to register this event type
        TRA_ctrl_pkt_gen_event_generator() { /*cout << "send_event registered" << endl;*/ register_event_type(&sample); }

    protected:
        virtual event *generate(uint _trigger_time, void *data)
        {
            // cout << "send_event generated" << endl;
            return new TRA_ctrl_pkt_gen_event(_trigger_time, data);
        }

    public:
        virtual string type() { return "TRA_ctrl_pkt_gen_event"; }
        ~TRA_ctrl_pkt_gen_event_generator() {}
    };
    // this class is used to initialize the TRA_ctrl_pkt_gen_event
    class pkt_gen_data
    {
    public:
        uint src_id;
        uint dst_id;
        double _size;
        // uint mat_id; // the target of the rule
        // uint act_id; // the next hop toward the target recorded in the rule
        string msg;
        // double per; // the percentage
        // packet *_pkt;
    };

    void print() const;
};
TRA_ctrl_pkt_gen_event::TRA_ctrl_pkt_gen_event_generator TRA_ctrl_pkt_gen_event::TRA_ctrl_pkt_gen_event_generator::sample;

void TRA_ctrl_pkt_gen_event::trigger()
{

    TRA_ctrl_packet *pkt = dynamic_cast<TRA_ctrl_packet *>(packet::packet_generator::generate("TRA_ctrl_packet"));
    if (pkt == nullptr)
    {
        cerr << "packet type is incorrect" << endl;
        return;
    }
    TRA_ctrl_header *hdr = dynamic_cast<TRA_ctrl_header *>(pkt->getHeader());
    TRA_ctrl_payload *pld = dynamic_cast<TRA_ctrl_payload *>(pkt->getPayload());

    if (hdr == nullptr)
    {
        cerr << "header type is incorrect" << endl;
        return;
    }
    if (pld == nullptr)
    {
        cerr << "payload type is incorrect" << endl;
        return;
    }

    hdr->setSrcID(src);
    hdr->setDstID(dst);
    hdr->setPreID(src);
    // hdr->setNexID(dst); // in hw3, you should set NexID to src
    hdr->setNexID(src);

    // payload
    pld->setMsg(msg);
    pld->setnid(src); // add this line
    // pld->setMatID(mat);
    // pld->setActID(act);
    // pld->setPer(per);
    pkt->setSize(size);

    recv_event::recv_data e_data;
    e_data.s_id = src;
    // e_data.r_id = dst; // in hw3, you should set r_id it src
    e_data.r_id = src;
    e_data._pkt = pkt;

    recv_event *e = dynamic_cast<recv_event *>(event::event_generator::generate("recv_event", trigger_time, (void *)&e_data));
}
uint TRA_ctrl_pkt_gen_event::event_priority() const
{
    string string_for_hash;
    // string_for_hash = to_string(getTriggerTime()) + to_string(src) + to_string(dst) + to_string(mat) + to_string(act); //to_string (pkt->getPacketID());
    string_for_hash = to_string(getTriggerTime()) + to_string(src) + to_string(dst); // to_string (pkt->getPacketID());
    return get_hash_value(string_for_hash);
}
// the TRA_ctrl_pkt_gen_event::print() function is used for log file
void TRA_ctrl_pkt_gen_event::print() const
{
    cout << "time " << setw(11) << event::getCurTime()
         << "        " << setw(11) << " "
         << "        " << setw(11) << " "
         << "   srcID" << setw(11) << src
         << "   dstID" << setw(11) << dst
         << "        " << setw(11) << " "
         << "        " << setw(11) << " "
         //  << "   matID"       << setw(11) << mat
         //  << "   actID"       << setw(11) << act
         << "   TRA_ctrl_packet generating"
         << endl;
}

////////////////////////////////////////////////////////////////////////////////

class link
{
    // all links created in the program
    static map<pair<uint, uint>, link *> id_id_link_table;

    uint id1; // from
    uint id2; // to

protected:
    link(link &) {} // this constructor should not be used
    link() {}       // this constructor should not be used
    link(uint _id1, uint _id2) : id1(_id1), id2(_id2) { id_id_link_table[pair<uint, uint>(id1, id2)] = this; }

public:
    virtual ~link()
    {
        id_id_link_table.erase(pair<uint, uint>(id1, id2)); // erase the link
    }

    static link *id_id_to_link(uint _id1, uint _id2)
    {
        return ((id_id_link_table.find(pair<uint, uint>(_id1, _id2)) != id_id_link_table.end()) ? id_id_link_table[pair<unsigned, unsigned>(_id1, _id2)] : nullptr);
    }

    virtual uint getLatency() = 0; // you must implement your own latency

    static void del_link(uint _id1, uint _id2)
    {
        pair<uint, uint> temp;
        if (id_id_link_table.find(temp) != id_id_link_table.end())
            id_id_link_table.erase(temp);
    }

    static uint getLinkNum() { return id_id_link_table.size(); }

    virtual bool canTransmit(map<string, double> args = {}) = 0;
    virtual void reserve(map<string, double> args = {}) = 0;

    class link_generator
    {
        // lock the copy constructor
        link_generator(link_generator &) {}
        // store all possible types of link
        static map<string, link_generator *> prototypes;

    protected:
        // allow derived class to use it
        link_generator() {}
        // after you create a new link type, please register the factory of this link type by this function
        void register_link_type(link_generator *h) { prototypes[h->type()] = h; }
        // you have to implement your own generate() to generate your link
        virtual link *generate(uint _id1, uint _id2, map<string, double> args = {}) = 0;

    public:
        // you have to implement your own type() to return your link type
        virtual string type() = 0;
        // this function is used to generate any type of link derived
        static link *generate(string type, uint _id1, uint _id2, map<string, double> args = {})
        {
            if (id_id_link_table.find(pair<uint, uint>(_id1, _id2)) != id_id_link_table.end())
            {
                std::cerr << "duplicate link id" << std::endl; // link id is duplicated
                return nullptr;
            }
            else if (BROCAST_ID == _id1 || BROCAST_ID == _id2)
            {
                cerr << "BROCAST_ID cannot be used" << endl;
                return nullptr;
            }
            else if (prototypes.find(type) != prototypes.end())
            { // if this type derived exists
                link *created_link = prototypes[type]->generate(_id1, _id2, args);
                return created_link; // generate it!!
            }
            std::cerr << "no such link type" << std::endl; // otherwise
            return nullptr;
        }
        static void print()
        {
            cout << "registered link types: " << endl;
            for (map<string, link::link_generator *>::iterator it = prototypes.begin(); it != prototypes.end(); it++)
                cout << it->second->type() << endl;
        }
        virtual ~link_generator() {};
    };
};
map<string, link::link_generator *> link::link_generator::prototypes;
map<pair<uint, uint>, link *> link::id_id_link_table;

void node::add_phy_neighbor(uint _id, string link_type, map<string, double> link_args)
{
    if (id == _id)
        return; // if the two nodes are the same...
    if (id_node_table.find(_id) == id_node_table.end())
        return; // if this node does not exist
    if (phy_neighbors.find(_id) != phy_neighbors.end())
        return; // if this neighbor has been added
    phy_neighbors[_id] = true;

    link::link_generator::generate(link_type, id, _id, link_args);
}

void node::del_phy_neighbor(uint _id)
{
    phy_neighbors.erase(_id);
}

class simple_link : public link
{
    double capacity = 100; // default capacity is set to 100; please set your link's capacity manually by add_phy_neighbor
    double occupied = 0;   // default occupied is set to 0

    uint latency = ONE_HOP_DELAY; // default latency

protected:
    simple_link() {}                                        // it should not be used outside the class
    simple_link(simple_link &) {}                           // it should not be used
    simple_link(uint _id1, uint _id2) : link(_id1, _id2) {} // this constructor cannot be directly called by users

public:
    virtual ~simple_link() {}
    virtual uint getLatency() { return latency; } // you can implement your own latency

    SET(setCapacity, uint, capacity, _capacity);
    GET(getCapacity, double, capacity);
    GET(getOccupied, double, occupied);

    SET(setLatency, uint, latency, _latency);

    bool canTransmit(map<string, double> args = {})
    {
        if (args.find("pkt_size") != args.end())
            return (occupied + args["pkt_size"] <= capacity);
        return false;
    }
    void reserve(map<string, double> args = {})
    {
        if (args.find("pkt_size") != args.end())
            occupied += args["pkt_size"];
    }

    class simple_link_generator;
    friend class simple_link_generator;
    // simple_link is derived from link_generator to generate a link
    class simple_link_generator : public link_generator
    {
        static simple_link_generator sample;
        // this constructor is only for sample to register this link type
        simple_link_generator() { /*cout << "simple_link registered" << endl;*/ register_link_type(&sample); }

    protected:
        virtual link *generate(uint _id1, uint _id2, map<string, double> args = {})
        { /*cout << "simple_link generated" << endl;*/
            simple_link *l = new simple_link(_id1, _id2);
            if (args.find("capacity") != args.end())
                l->setCapacity(static_cast<uint>(args["capacity"]));
            if (args.find("latency") != args.end())
                l->setLatency(static_cast<uint>(args["latency"]));
            return l;
        }

    public:
        virtual string type() { return "simple_link"; }
        ~simple_link_generator() {}
    };
};

simple_link::simple_link_generator simple_link::simple_link_generator::sample;

// the data_packet_event function is used to add an initial event
void data_packet_event(uint src, uint dst, double f_size, uint t = 0, string msg = "default")
{
    if (node::id_to_node(src) == nullptr || (dst != BROCAST_ID && node::id_to_node(dst) == nullptr))
    {
        cerr << "src or dst is incorrect" << endl;
        return;
        return;
    }

    TRA_data_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = dst;
    e_data._size = f_size;
    e_data.msg = msg;

    // recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event",t, (void *)&e_data) );
    TRA_data_pkt_gen_event *e = dynamic_cast<TRA_data_pkt_gen_event *>(event::event_generator::generate("TRA_data_pkt_gen_event", t, (void *)&e_data));
    if (e == nullptr)
        cerr << "event type is incorrect" << endl;
}

// the TRA_ctrl_packet_event function is used to add an initial event

void TRA_ctrl_packet_event(uint src, uint t = event::getCurTime(),
                           string msg = "default")
{
    // 1st parameter: the source; the destination that want to broadcast a msg with counter 0 (i.e., match ID)
    // 2nd parameter: time (optional)
    // 3rd parameter: msg for debug information (optional)
    if (node::id_to_node(src) == nullptr)
    {
        cerr << "id is incorrect" << endl;
        return;
    }

    // uint src = con_id;
    TRA_ctrl_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = BROCAST_ID;
    // e_data.mat_id = mat;
    // e_data.act_id = act;
    e_data._size = 0; // we assume ctrl msg size is zero
    e_data.msg = msg;
    // e_data.per = per;

    TRA_ctrl_pkt_gen_event *e = dynamic_cast<TRA_ctrl_pkt_gen_event *>(event::event_generator::generate("TRA_ctrl_pkt_gen_event", t, (void *)&e_data));
    if (e == nullptr)
        cerr << "event type is incorrect" << endl;
}

link *node::getLink(uint nb_id)
{
    return link::id_id_to_link(getNodeID(), nb_id);
}

// send_handler function is used to transmit packet p based on the information in the header
// Note that the packet p will not be discard after send_handler ()
void node::send_handler(packet *p)
{
    packet *_p = packet::packet_generator::replicate(p);
    send_event::send_data e_data;
    e_data.s_id = _p->getHeader()->getPreID();
    e_data.r_id = _p->getHeader()->getNexID();
    e_data._pkt = _p;
    send_event *e = dynamic_cast<send_event *>(event::event_generator::generate("send_event", event::getCurTime(), (void *)&e_data));
    if (e == nullptr)
        cerr << "event type is incorrect" << endl;
}

void node::send(packet *p)
{ // this function is called by event; not for the user; you cannot change the send function
    if (p == nullptr)
        return;

    uint _nexID = p->getHeader()->getNexID();
    for (map<uint, bool>::iterator it = phy_neighbors.begin(); it != phy_neighbors.end(); it++)
    {
        uint nb_id = it->first; // neighbor id

        if (nb_id != _nexID && BROCAST_ID != _nexID)
            continue; // this neighbor will not receive the packet

        link *l = link::id_id_to_link(id, nb_id);
        // if ( !l ) continue; // no such a link

        double pkt_size = p->getSize();
        if (!(l->canTransmit({{"pkt_size", pkt_size}})))
            continue; // if the capacity is insufficient, drop the packet
        l->reserve({{"pkt_size", pkt_size}});

        uint trigger_time = event::getCurTime() + link::id_id_to_link(id, nb_id)->getLatency(); // we simply assume that the delay is fixed
        // cout << "node " << id << " send to node " << nb_id << endl;
        recv_event::recv_data e_data;
        e_data.s_id = id;    // set the sender   (i.e., preID)
        e_data.r_id = nb_id; // set the receiver (i.e., nexID)

        packet *p2 = packet::packet_generator::replicate(p);
        e_data._pkt = p2;

        recv_event *e = dynamic_cast<recv_event *>(event::event_generator::generate("recv_event", trigger_time, (void *)&e_data)); // send the packet to the neighbor
        if (e == nullptr)
            cerr << "event type is incorrect" << endl;
    }
    packet::discard(p);
}

double TRA_switch::getCapacity(uint nb_id)
{
    simple_link *l = dynamic_cast<simple_link *>(getLink(nb_id));
    if (l)
        return l->getCapacity();
    return 0;
}
double TRA_switch::getOccupied(uint nb_id)
{
    simple_link *l = dynamic_cast<simple_link *>(getLink(nb_id));
    if (l)
        return l->getOccupied();
    return 0;
}

void TRA_switch::print_to(ostream &os) const
{
    node::print_to(os);
    for (auto it = entry_table.begin(); it != entry_table.end(); it++)
        os << it->first << ' ' << it->second[0] << endl;
}

ostream &operator<<(ostream &os, const node &n)
{
    n.print_to(os);
    return os;
}

bool TRA_switch::isNewPacket(packet *p)
{
    uint src = p->getHeader()->getSrcID();
    uint id = p->getPacketID();

    auto it = last_p_id_from_node.find(src);
    if (it == last_p_id_from_node.end())
        return true;
    else
    {
        if (id > it->second)
            return true;
        else
            return false;
    }
}

// you have to write the code in recv_handler of TRA_switch
// each node follows this function to handle the received packet
void TRA_switch::recv_handler(packet *p)
{
    // in this function, you are "not" allowed to use node::id_to_node(id) and id_id_to_link(id1, id2) !!!!!!!!
    // moreover, you cannot use any static variable or static functions in this function

    // this is a simple example
    // node 0 broadcasts its message to every node and every node relays the packet "only once"
    // the variable hi is used to examine whether the packet has been received by this node before
    // you can remove the variable hi and create your own routing table in class TRA_switch
    if (p == nullptr)
        return;

    if (p->type() == "TRA_ctrl_packet")
    { // the switch receives a packet from the controller
        TRA_ctrl_packet *p3 = nullptr;
        p3 = dynamic_cast<TRA_ctrl_packet *>(p);
        TRA_ctrl_payload *l3 = nullptr;
        l3 = dynamic_cast<TRA_ctrl_payload *>(p3->getPayload());

        if (p3->getHeader()->getSrcID() == getNodeID()) // if this packet is sent from this node
        {
            uint p_id = p3->getPacketID();
            auto last_packet = last_p_id_from_node.find(getNodeID());
            if (last_packet != last_p_id_from_node.end() && p_id <= last_packet->second) // receive packet sent itself
                return;

            // put the node's neighbors' information into the netw_info
            l3->setnid(getNodeID()); // use this line to set the node ID
            map<uint, bool> nblist = getPhyNeighbors();
            for (map<uint, bool>::iterator it = nblist.begin(); it != nblist.end(); it++)
            {
                unsigned nb_id = it->first; // nb id
                // use this line to add one more entry (one more neighbor) in the payload
                l3->addNetwInfo(nb_id, getCapacity(nb_id), getOccupied(nb_id));
                network[{getNodeID(), nb_id}] = {getCapacity(nb_id), getOccupied(nb_id)};
            }
            last_p_id_from_node[getNodeID()] = p_id;
        }
        else // if this packet is sent from the other node
        {
            // you have to design your data structure in TRA_switch to store the network information extracted from the packet

            // you have to design a variable in TRA_switch for identifying whether the packet was received before
            // you can check the pktID to see whether the packet was received before
            // if the packet was, then don't update the information and don't send the packet again

            // otherwise, update the node's stored network information from the packet
            uint src_id = l3->getnid();
            uint pre_id = p3->getHeader()->getPreID();
            map<uint, pair<double, double>> src_nbs = l3->getNetwInfo();

            auto last_packet = last_p_id_from_node.find(src_id);
            if (last_packet == last_p_id_from_node.end()) // first packet from src node
            {
                uint pre_id = p3->getHeader()->getPreID();
                vector<uint> entries;
                entries.push_back(pre_id);
                entry_table.insert({src_id, entries});
                last_p_id_from_node.insert({src_id, p->getPacketID()});
                for (auto nb = src_nbs.begin(); nb != src_nbs.end(); nb++)
                    network.insert({{src_id, nb->first}, {nb->second}});
            }
            else if (p->getPacketID() > last_packet->second) // new packet, update info
            {
                // update data
                for (auto nb = src_nbs.begin(); nb != src_nbs.end(); nb++)
                    network[{src_id, nb->first}] = {nb->second};
                last_packet->second = p->getPacketID();
            }
            else
            {
                for (auto it = entry_table.at(src_id).begin(); it != entry_table.at(src_id).end(); it++)
                    if (*it == pre_id)
                        return;
                entry_table.at(src_id).push_back(pre_id);
                return; // discard packets received before
            }

            // you can use the following loop to get every link's information
            // cout << "cur_id = " << getNodeID() << ", src_id = " << src_id << endl;
            // for (map<uint, pair<double, double>>::iterator it = src_nbs.begin(); it != src_nbs.end(); it++)
            // {
            //     cout << "nb_id = " << it->first << ", capacity = " << it->second.first << ", occupied = " << it->second.second << endl;
            //     do something to update the node's stored network information from the packet
            // }
        }

        // update header
        p3->getHeader()->setPreID(getNodeID());
        p3->getHeader()->setNexID(BROCAST_ID);
        p3->getHeader()->setDstID(BROCAST_ID);

        // l3->increase();
        send_handler(p3);
        // unsigned mat = l3->getMatID();
        // unsigned act = l3->getActID();
        // string msg = l3->getMsg(); // get the msg
    }
    else if (p->type() == "TRA_data_packet")
    {
        // the switch receives a packet
        // cout << "node " << getNodeID() << " send the packet" << endl;
        TRA_data_packet *p4 = nullptr;
        p4 = dynamic_cast<TRA_data_packet *>(p);
        TRA_data_payload *l4 = nullptr;
        l4 = dynamic_cast<TRA_data_payload *>(p4->getPayload());

        // you have to implement the menchanisms of routing table and segment routing here

        // if the node is the source, then insert the labels here
        // use void push_label(uint _id) to push each label into the header
        // i.e., p4->getHeader()->push_label(_id);

        // otherwise, read the label and route the packet toward the label
        // use uint get_label() to get the current label
        // i.e., p4->getHeader()->get_label()

        // once arriving at the intermediate node:
        // before removing a label, check whther a label can be removed
        // use uint get_num_labels()
        // if so, remove a label from the header
        // use void pop_label() to remove one label from the header
        // i.e., p4->getHeader()->pop_label()

        // the following code is a naive way to send the packet sent from 4 to 2 and from 2 to 0
        // you should not write the code in this way; this is for simplicity to demonstrate how the recv_handler works
        TRA_data_header *h4 = dynamic_cast<TRA_data_header *>(p4->getHeader());
        uint src_id = h4->getSrcID();
        uint dst_id = h4->getDstID();
        uint pre_id = h4->getPreID();
        uint cur_id = getNodeID();

        if (entry_table.size() < getNodeNum() - 1) // entry table not prepared
            return;
        if (cur_id == dst_id) // packet arrive
            return;
        else if (cur_id == src_id) // prepare packet to send
        {
            h4->push_label(dst_id); // dst
            // cout << "the current top label = " << next << endl;
            h4->setDstID(dst_id);
        }
        else if (cur_id == h4->get_label())
            h4->pop_label();
        // cout << "the current top label = " << next << endl;

        uint top_id = h4->get_label(); // get top label from the header
        uint next_id = entry_table.find(top_id)->second[0];

        pair<double, double> edge = network.at({cur_id, next_id});
        if (edge.first - edge.second < p4->getSize()) // capacity not enough
        {
            // find another path
            if (h4->get_used_labels() < getNumOfLabel() - 1) // check #labels
            {
                // add label
                vector entries = entry_table.at(dst_id);
                uint label = entries[0];
                double capacity;
                for (uint i = 0; i <= entries.size() / 2; i++)
                {
                    auto nb = entries[i];
                    // find nb with enough capacity
                    pair nb_edge = network.at({cur_id, nb});
                    capacity = nb_edge.first - nb_edge.second;
                    if (!h4->visited(nb) && capacity > p4->getSize())
                    {
                        label = nb;
                        break;
                    }
                }

                if (label != entries[0])
                {
                    h4->push_label(label);
                    next_id = label;
                }
            }
            else
                return;
        }

        if (h4->visited(next_id))
            return;

        // update header
        h4->visit(cur_id);
        h4->setPreID(cur_id);
        h4->setNexID(next_id);
        send_handler(p);
    }

    // you should implement the OSPF algorithm in recv_handler
    // getNodeID() returns the id of the current node

    // The current node's neighbors are already stored in the following variable
    // map<uint,bool> node::phy_neighbors
    // however, this variable is private in the class node
    // You have to use node::getPhyNeighbors to get the variable
    // for example, if you want to print all the neighbors of this node
    // const map<uint,bool> &nblist = getPhyNeighbors();
    // cout << "node " << getNodeID() << "'s nblist: ";
    // for (map<uint,bool>::const_iterator it = nblist.begin(); it != nblist.end(); it ++) {
    //     cout << it->first << ", " ;
    // }
    // cout << endl;

    // you can use p->getHeader()->setSrcID() or getSrcID()
    //             p->getHeader()->setDstID() or getDstID()
    //             p->getHeader()->setPreID() or getPreID()
    //             p->getHeader()->setNexID() or getNexID() to change or read the packet header

    // In addition, you can get the packet, header, and payload with the correct type (GR)
    // in fact, this is downcasting
    // TRA_data_packet * pkt = dynamic_cast<TRA_data_packet*> (p);
    // TRA_data_header * hdr = dynamic_cast<TRA_data_header*> (p->getHeader());
    // TRA_data_payload * pld = dynamic_cast<TRA_data_payload*> (p->getPayload());

    // you can also change the TRA_data_payload setting
    // pld->setMsg(string): to set the message transmitted to the destination

    // Besides, you can use packet::packet_generator::generate() to generate a new packet; note that you should fill the header and payload in the packet
    // moreover, you can use "packet *p2 = packet::packet_generator::replicate(p)" to make a clone p2 of packet p
    // note that if the packet is generated or replicated manually, you must delete it by packet::discard() manually before recv_handler finishes

    // "IMPORTANT":
    // You have to "carefully" fill the correct information (e.g., srcID, dstID, ...) in the packet before you send it
    // Note that if you want to transmit a packet to only one next node (i.e., unicast), then you fill the ID of a specific node to "nexID" in the header
    // Otherwise, i.e., you want to broadcasts, then you fill "BROCAST_ID" to "nexID" in the header
    // after that, you can use send_hander() to transmit the packet
    // usage: send_handler (p);

    // note that packet p will be discarded (deleted) after recv_handler(); you don't need to manually delete it
}

int main()
{
    // header::header_generator::print();   // print all registered headers
    // payload::payload_generator::print(); // print all registered payloads
    // packet::packet_generator::print();   // print all registered packets
    // node::node_generator::print();       // print all registered nodes
    // event::event_generator::print();     // print all registered events
    // link::link_generator::print();       // print all registered links
    // #define test
    uint nSwitch = 5, nLink, nPair, nLabel, period, simulate_time;
// #define test
#ifndef test
    {
        cin >> nSwitch >> nLink >> nPair >> nLabel >> period >> simulate_time;
        // read the input and generate switch nodes
        for (uint id = 0; id < nSwitch; id++)
        {
            node::node_generator::generate("TRA_switch", id);
            node::id_to_node(id)->setNumOfLabel(nLabel);
            for (uint time = 0; time <= simulate_time; time += period)
                TRA_ctrl_packet_event(id, time);
        }

        // set switches' neighbors
        for (uint id = 0; id < nLink; id++)
        {
            uint src, dst;
            double link_capacity;
            cin >> id >> src >> dst >> link_capacity;
            map<string, double> entry = {{"capacity", link_capacity}};
            node::id_to_node(src)->add_phy_neighbor(dst, "simple_link", entry);
            node::id_to_node(dst)->add_phy_neighbor(src, "simple_link", entry);
        }

        for (uint id = 0; id < nPair; id++)
        {
            uint src, dst, time;
            double f_size;
            cin >> id >> src >> dst >> f_size >> time;
            data_packet_event(src, dst, f_size, time);
        }
        event::start_simulate(simulate_time);
    }
#endif

#ifdef test
    {
        for (uint id = 0; id < 5; id++)
            node::node_generator::generate("TRA_switch", id);

        double link_capacity = 5;
        map<string, double> entry = {{"capacity", link_capacity}}; // similar to JSON
        // set switches' neighbors
        node::id_to_node(0)->add_phy_neighbor(1, "simple_link", entry);
        node::id_to_node(1)->add_phy_neighbor(0, "simple_link", entry);
        node::id_to_node(0)->add_phy_neighbor(2, "simple_link", entry);
        node::id_to_node(2)->add_phy_neighbor(0, "simple_link", entry);
        node::id_to_node(1)->add_phy_neighbor(2, "simple_link", entry);
        node::id_to_node(2)->add_phy_neighbor(1, "simple_link", entry);
        node::id_to_node(1)->add_phy_neighbor(3, "simple_link", entry);
        node::id_to_node(3)->add_phy_neighbor(1, "simple_link", entry);
        node::id_to_node(2)->add_phy_neighbor(4, "simple_link", entry);
        node::id_to_node(4)->add_phy_neighbor(2, "simple_link", entry);

        // node 0 broadcasts its neighbor information at time 100
        for (int i = 0; i < 5; i++)
            TRA_ctrl_packet_event(i, 100);

        // 1st parameter: the source; the destination that want to broadcast its neighobr information
        // 2nd parameter: time (optional)
        // 3rd parameter: msg for debug information (optional)
        // you can iteratively call TRA_ctrl_packet_event(0, time) with multiple different ``time points''
        // (e.g., time = 100, 120, 140, ....) to make node 0 braodcast periodically
        // you need to implement how to update each node's stored network informatino from the periodic broadcasting packets

        // node 4 sends a packet to node 0 with a size of 5 at time 200 --> you need to implement routing tables for TRA_switches

        data_packet_event(4, 0, 5, 200); // data_packet is type TRA_data_packet
        data_packet_event(4, 0, 5, 250); // data_packet is type TRA_data_packet

        // 1st parameter: the source node, src
        // 2nd parameter: the destination node, dst
        // 3rd parameter: the packet size, s
        // 4th parameter: time, t
        // 5th parameter: msg for debug (optional)
        // you can invoke data_packet_event to generate a flow sent from src to dst with a size s at time t

        // start simulation!!
        event::start_simulate(300);
        // event::flush_events() ;
        // cout << packet::getLivePacketNum() << endl;
    }
#endif

    for (uint id = 0; id < nSwitch; id++)
        cout << *(node::id_to_node(id));

    return 0;
}
