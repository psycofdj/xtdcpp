#ifndef COUNTERS_BASE_HH_
# define COUNTERS_BASE_HH_

# include <boost/thread/mutex.hpp>
# include <boost/noncopyable.hpp>
# include <types.hh> // libcommon
# include "Visitor.hh"


namespace xtd {
namespace counters {

class Base : boost::noncopyable
{
  friend class Composed;

public:
  typedef std::shared_ptr<Base> t_sptr;
  typedef map<string, string>     t_data;

public:
  Base(const string& p_name);
  virtual ~Base(void);

public:
  void          visit(Visitor& p_visitor) const;
  void          update(void);
  const string& getName(void) const;

protected:
  virtual void update_safe(void);
  virtual void visit_safe(Visitor& p_visitor) const = 0;

protected:
  string          m_name;
  mutable boost::mutex m_mutex;
};

}}


#endif // !COUNTERS_BASE_HH_
