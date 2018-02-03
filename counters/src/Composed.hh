#ifndef COUNTERS_COMPOSED_HH_
# define COUNTERS_COMPOSED_HH_
# include "Base.hh"
# include "types.hh"

namespace xtd { namespace counters { class Visitor; } }
namespace xtd {
namespace counters {

class Composed : public Base
{
public:
  Composed(const string& p_name);
  virtual ~Composed(void) {}

protected:
  void addItem(Base& p_item);

protected:
  virtual void update_safe(void);
  virtual void visit_safe(Visitor& p_visitor) const;


private:
  vector<Base*> m_items;
};


}}

#endif // !COUNTERS_COMPOSED_HH_
