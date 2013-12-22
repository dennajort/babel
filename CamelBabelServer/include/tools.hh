#ifndef __TOOLS_HH__
#define __TOOLS_HH__

#include	<mongo/client/dbclient.h>
#include	<string>

namespace tools
{
  std::string	lower(const std::string &);
  std::string	sha1(const std::string &);
  mongo::OID	uintToOID(uint16_t);
  uint16_t	OIDToUint(const mongo::OID &);

}

#endif
