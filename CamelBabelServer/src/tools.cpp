#include	<boost/foreach.hpp>
#include	<boost/uuid/sha1.hpp>
#include	<sstream>
#include	<iomanip>
#include	"tools.hh"

std::string	tools::lower(const std::string &in)
{
  std::string	out;

  BOOST_FOREACH(const char c, in)
    {
      if (c >= 'A' && c <= 'Z')
	out += (c + 32);
      else
	out += c;
    }
  return out;
}

std::string	tools::sha1(const std::string &data)
{
  boost::uuids::detail::sha1	sha;
  unsigned int			digest[5];
  std::ostringstream		out;

  sha.process_bytes(data.c_str(), data.length());
  sha.get_digest(digest);
  out << std::hex << std::setfill('0');
  BOOST_FOREACH(unsigned int nb, digest)
    {
      out << std::setw(8) << nb;
    }
  return lower(out.str());
}

mongo::OID	tools::uintToOID(uint16_t id)
{
  unsigned char	tmp[12];

  for (int n = 0; n < 12; n++)
    tmp[n] = (unsigned char)(id & (1 << (11 - n)));
  return mongo::OID(tmp);
}

uint16_t	tools::OIDToUint(const mongo::OID &oid)
{
  uint16_t	id = 0;

  for (int n = 0; n < 12; n++)
    id += ((const uint16_t)(oid.getData()[n])) << (11 - n);
  return id;
}
