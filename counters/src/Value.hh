#ifndef COUNTERS_VALUE_HH_
# define COUNTERS_VALUE_HH_

# include <string>
# include "Base.hh"


namespace xtd {
namespace counters {

/** */

class Atomic
{
public:
  Atomic(void);

public:
  void inc(uint32_t& p_val);
  void dec(uint32_t& p_val);
  void write(uint32_t& p_val, const uint32_t& p_src);

  template<typename TType>
  void inc(TType& p_val);
  template<typename TType>
  void dec(TType& p_val);
  template<typename TType>
  void write(TType& p_val, const TType& p_src);

private:
  boost::mutex m_mutex;
};


template<typename TType>
class Value : public Base
{
  friend class Composed;

public:
  typedef boost::shared_ptr<Value<TType> > t_sptr;

public:
  Value(const string& p_name);
  virtual ~Value(void) {};

public:
  Value& NaN(void);
  bool   isNaN(void) const;
  Value& operator=(const TType& p_value);
  Value& operator++(void);
  Value& operator++(int);
  Value& operator--(void);
  Value& operator--(int);

public:
  const TType& getValue(void) const;

protected:
  void visit_safe(Visitor& p_visitor) const;

protected:
  bool   m_isNaN;
  TType  m_value;
  Atomic m_atom;
};

typedef Value<uint32_t> Value32;
typedef Value<uint64_t> Value64;

}}

#endif // !COUNTERS_VALUE_HH_
