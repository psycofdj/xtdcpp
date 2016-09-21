#ifndef SERIALIZER_OBJECTS_DOC_HH_
# define SERIALIZER_OBJECTS_DOC_HH_

# include <types.hh>   // libcore
# include "serializer.hh"

namespace xtd {
namespace serializer {

class Doc
{
  friend class boost::serialization::access;

public:
  typedef vector<Doc> t_listof;

public:
  Doc(void);

private:
  template<class TArchive>
  void serialize(TArchive& p_archive, const unsigned int p_version);

public:
  uint32_t  m_score;
  // debug
  uint32_t  m_debugScore;
};

}}

BOOST_CLASS_VERSION(xtd::serializer::Doc, 0)

# include "objects/Doc.hxx"

#endif // !SERIALIZER_OBJECTS_DOC_HH_
