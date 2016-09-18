#include "types.hh"

namespace xtd {

ostream& operator<<(ostream& p_buf, const xtd::status& p_status)
{
  switch (p_status) {
  case xtd::status::ok:       p_buf << "status::ok"       ; break;
  case xtd::status::error:    p_buf << "status::error"    ; break;
  case xtd::status::timeout:  p_buf << "status::timeout"  ; break;
  case xtd::status::notfound: p_buf << "status::notfound" ; break;
  case xtd::status::next:     p_buf << "status::next"     ; break;
  }
  return p_buf;
}

}
