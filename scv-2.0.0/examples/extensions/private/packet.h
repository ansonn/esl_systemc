#include "scv.h"

#include <iomanip>

#ifndef PACKET_H
#define PACKET_H

// Define a packet with an integer payload


class packetT1 {
  public:
    packetT1 () {}
    virtual ~packetT1() {}

    // Packet's data
 private:
    sc_uint<16> src;
    sc_uint<16> dest;
 public:
    sc_uint< 4> priority;
    sc_uint<16> payload;
    // nbcode "fields" end

    // nbcode "access" start
    // Field access methods
    sc_uint<16> get_src()                   {return src;  }
    sc_uint<16> get_dest()                  {return dest; }
    void        set_src(sc_uint<16> _src)   {src  = _src; }
    void        set_dest(sc_uint<16> _dest) {dest = _dest;}
    // nbcode "access" end

    // nbcode "friend" start
    // Declare extensions class to be a friend
    friend class scv_extensions<packetT1>;

    // Define ostream method to print the packet
    friend ostream& operator<< (ostream& os, const packetT1& p) {
      os << "  src: "       << std::setw(5) << p.src
         << "  dest: "      << std::setw(5) << p.dest
         << "  priority: "  << std::setw(3) << p.priority 
         << "  payload: "   << std::setw(5) << p.payload;
      return os;
    }
};
// nbcode "friend" end

// nbcode "fields" start
class packetT2 {
  public:
    packetT2 () {}
    virtual ~packetT2() {}

    // Packet's data
 private:
    sc_uint<16> src;
    sc_uint<16> dest;
 public:
    sc_uint< 4> priority;
    sc_uint<16> payload;
    // nbcode "fields" end

    // Field access methods
    sc_uint<16> get_src()                   {return src;  }
    sc_uint<16> get_dest()                  {return dest; }
    void        set_src(sc_uint<16> _src)   {src  = _src; }
    void        set_dest(sc_uint<16> _dest) {dest = _dest;}

    // Define ostream method to print the packet
    friend ostream& operator<< (ostream& os, const packetT2& p) {
      os << "  src: "       << std::setw(5) << p.src
         << "  dest: "      << std::setw(5) << p.dest
         << "  priority: "  << std::setw(3) << p.priority 
         << "  payload: "   << std::setw(5) << p.payload;
      return os;
    }
};
// nbcode "out" end

// nbcode "ext1" start
// Extensions to packetT1
template<>
class scv_extensions<packetT1> : public scv_extensions_base<packetT1> {
public:
  scv_extensions< sc_uint<16> > src;
  scv_extensions< sc_uint<16> > dest;
  scv_extensions< sc_uint< 4> > priority;
  scv_extensions< sc_uint<16> > payload;

  SCV_EXTENSIONS_CTOR(packetT1) {
    //must be in order
    SCV_FIELD(src);
    SCV_FIELD(dest);
    SCV_FIELD(priority);
    SCV_FIELD(payload);
  }
};
// nbcode "ext1" end

// nbcode "ext2" start
// Extensions to packetT2
template<>
class scv_extensions<packetT2> : public scv_extensions_base<packetT2> {
public:
  scv_extensions< sc_uint< 4> > priority;
  scv_extensions< sc_uint<16> > payload;

  SCV_EXTENSIONS_CTOR(packetT2) {
    //must be in order
    SCV_FIELD(priority);
    SCV_FIELD(payload);
  }
};
// nbcode "ext2" end

#endif //PACKET_H
