#ifndef	BABELEXCEPTION_HPP
#define	BABELEXCEPTION_HPP

#include <exception>
#include <string>

class BabelException : public std::exception
{
public:
  BabelException(const std::string &msg) : _msg(msg) {}
  virtual ~BabelException() throw() {}
  virtual const char *what() const throw() {return (_msg.c_str());}

private:
  std::string _msg;
};



#endif // BABELEXCEPTION_HPP
