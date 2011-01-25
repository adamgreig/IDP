/* Title:     robot_link.h
 * Purpose:   Declares the robot_link class.
 * Notes:     For use with robots based on the Balloon board
 *            system. Not suitable for use with IDP grey
 *            boxes.
 */

#ifndef __cplusplus
#error robot_link.h must be used from C++, not C code
#endif

#ifndef _ROBOT_LINK_INCLUDED
#define _ROBOT_LINK_INCLUDED

#include <string>

#include "robot_instr.h" // command/request opcode values

// Enumerated type: link_err
// =========================
// Lists the various error that can result as a result of
// calling methods of the robot_link class. 

enum link_err {
  LINKERR_NONE        =  0,      // no error
  LINKERR_COMMS       =  1,      // fatal communications error
  LINKERR_READ        =  2,      // read error
  LINKERR_WRITE       =  3,      // write error
  LINKERR_CHKSUM      =  4,      // checksum error
  LINKERR_NOTINIT     =  5,      // link not initialised
  LINKERR_BADCARD     =  6,      // invalid card number specified
  LINKERR_BADHOST     =  7,      // invalid hostname specified
  LINKERR_BADPORT     =  8,      // invalid port number specified
  LINKERR_SKT1        =  9,      // error creating socket
  LINKERR_SKT2        = 10,      // error connecting socket
  LINKERR_SKT3        = 11,      // error setting non-blocking IO
  LINKERR_NOHOST      = 12,      // host not found on network
  LINKERR_NOSERV      = 13,      // robot_link service not running on host
  LINKERR_OVERFLOW    = 14       // error buffer overflow
};


const int ERR_BUFLEN    = 16;
const int REQUEST_ERROR = -1;


// Class declaration: robot_link
// =============================
// Instances of the robot_link class represent a connection to a robot.

class robot_link
{
public:
  // Construction/Destruction
  robot_link();
  ~robot_link();

  // Link (re)initialisation
  bool initialise(int card);         // connect to wireless card
  bool initialise(const char *host); // connect to a specific host
  bool initialise();                 // connect to localhost
  bool reinitialise();               // reconnect link after error
 
  // Input and output
  bool command(command_instruction cmd, int arg);
  int  request(request_instruction);

  // Advanced I/O
  bool transact(unsigned char cmd, unsigned char &arg);
  robot_link& operator <<(const robot_command& cmd);
  robot_link& operator >>(robot_request &req);

  // Error handling
  void           clear_errs();
  link_err       get_err();
  link_err       lookup_err(int n) const;
  const char*    err_string() const;
  const char*    err_string(link_err code) const;
  void           print_errs(const char *prefix);
  void           print_errs();
  bool           any_errs() const { return m_errbufcnt > 0; }
  bool           fatal_err() const { return m_fatalerrflag; }

  int            send_errs;
  int            recv_errs;
  int            cmd_errs;

private:
  // Implementation:
  bool           connect_unix(const std::string& socket);
  bool           connect_inet(const std::string& host, unsigned short port);
  void           record_err(link_err);

  int            m_fd;
  link_err       m_errbuf[ERR_BUFLEN];
  int            m_errbufcnt;
  bool           m_fatalerrflag;
  std::string    m_host; // host (or socket pathname)
  unsigned short m_port; // inet port number
};

#endif /* _ROBOT_LINK_INCLUDED */

