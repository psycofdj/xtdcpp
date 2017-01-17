#include "http/Server.hh"
#include "http/Server.hxx"
#include "network_types.hh"

namespace xtd {
namespace network {
namespace http {

std::function<bool(const Request& p_req)>
operator&&(std::function<bool(const Request& p_req)> p_fn1,
           std::function<bool(const Request& p_req)> p_fn2)
{
  return [&p_fn1,&p_fn2](const Request& p_req) -> bool {
    return p_fn1(p_req) && p_fn2(p_req);
  };
}


std::function<bool(const Request& p_req)>
operator||(std::function<bool(const Request& p_req)> p_fn1,
           std::function<bool(const Request& p_req)> p_fn2)
{
  return [&p_fn1,&p_fn2](const Request& p_req) -> bool {
    return p_fn1(p_req) || p_fn2(p_req);
  };
}

template class Server<af_unix>;
template class Server<af_inet>;

}}}
