#ifndef NETWORK_HTTP_MESSAGE_HH_
# define NETWORK_HTTP_MESSAGE_HH_
# include "types.hh" //libcore
# include "http/http_types.hh"

namespace xtd {
namespace network {
namespace http {

class Message
{
public:
  typedef map<string, string> t_header_map;

public:
  Message(void);
  virtual ~Message(void);

public:
  bool                existsHeader(const string& p_name) const;
  bool                getHeader(const string& p_name, string& p_value) const;
  const t_header_map& getHeaders(void) const;
  void                addHeader(const string& p_name, const string& p_value);
  void                setVersion(version p_version);
  version             getVersion(void) const;
  status              getDataSize(size_t& p_length) const;
  const string&       getData(void) const;

public:
  status read(std::istream& p_request);
  status readHead(std::istream& p_request);
  void   write(std::ostream& p_stream) const;

protected:
  virtual status readVersion(const string& p_version);
  virtual status readHeaders(std::istream& p_stream);
  virtual status readInitial(std::istream& p_stream);
  virtual status readData(std::istream& p_stream);
  virtual void   writeInitial(std::ostream& p_stream) const;
  virtual void   writeData(std::ostream& p_stream) const;
  virtual void   writeHeaders(std::ostream& p_stream) const;


protected:
  version      m_version;
  t_header_map m_headers;
  string       m_data;
};


}}}

#endif // !NETWORK_HTTP_MESSAGE_HH_
