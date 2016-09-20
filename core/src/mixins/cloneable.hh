#ifndef CORE_MIXINS_CLONEABLE_HH_
# define CORE_MIXINS_CLONEABLE_HH_

namespace xtd {
namespace mixins {

template<class Base, class Derived>
class Cloneable : public Base
{
public:
  typedef Cloneable<Base, Derived> Parent;

public:
  template<typename... Args>
  Cloneable(Args&&... p_args) :
    Base(p_args...)
  {
  }

  virtual ~Cloneable(void) { }

public:
  virtual Base* clone(void) const override
  {
    return new Derived(dynamic_cast<const Derived&>(*this));
  }
};


template<class Base>
class PureCloneableBase
{
public:
  typedef PureCloneableBase<Base> Parent;

public:
  virtual Base* clone(void) const = 0;
};

template<class Base>
class CloneableBase
{
public:
  typedef CloneableBase<Base> Parent;

public:
  virtual Base* clone(void) const
  {
    return new Base(dynamic_cast<const Base&>(*this));
  }
};

}}

#endif // !CORE_MIXINS_CLONEABLE_HH_
