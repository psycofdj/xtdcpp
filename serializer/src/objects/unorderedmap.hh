/*
 * boost_serialization_unorderedmap.hh
 *
 *  Created on: 30 janv. 2013
 *      Author: cdelattre
 */

#ifndef BOOST_SERIALIZATION_UNORDEREDMAP_HH_
#define BOOST_SERIALIZATION_UNORDEREDMAP_HH_

/**
 ** warning: operation on '((boost::unordered::iterator_detail::c_iterator<const
 ** boost::unordered::detail::ptr_node<std::pair<const unsigned int, float> >*,
 ** boost::unordered::detail::ptr_node<std::pair<const unsigned int, float> >*,
 ** std::pair<const unsigned int, float> >*)this)->boost::unordered::iterator_detail::
 ** c_iterator<const boost::unordered::detail::ptr_node<std::pair<const unsigned int,
 ** float> >*, boost::unordered::detail::ptr_node<std::pair<const unsigned int, float> >*,
 ** std::pair<const unsigned int, float> >::node_' may be undefined [-Wsequence-point]
 */
#pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wsequence-point"
# include <boost/unordered_map.hpp>
# include <boost/config.hpp>
# include <boost/serialization/utility.hpp>
# include <boost/serialization/collections_save_imp.hpp>
# include <boost/serialization/collections_load_imp.hpp>
# include <boost/serialization/split_free.hpp>
#pragma GCC diagnostic pop


namespace boost {
namespace serialization {

template<class Archive, class Type, class Key, class Hash, class Compare, class Allocator >
inline void save(
                 Archive & ar,
                 const boost::unordered_map<Key, Type, Hash, Compare, Allocator> &t,
                 const unsigned int /* file_version */
                 ){
  boost::serialization::stl::save_collection<
    Archive,
    boost::unordered_map<Key, Type, Hash, Compare, Allocator>
    >(ar, t);
}

  template<class Archive, class Type, class Key, class Hash, class Compare, class Allocator >
  inline void load(
                   Archive & ar,
                   boost::unordered_map<Key, Type, Hash, Compare, Allocator> &t,
                   const unsigned int /* file_version */
                   ){
    boost::serialization::stl::load_collection<
      Archive,
      boost::unordered_map<Key, Type, Hash, Compare, Allocator>,
      boost::serialization::stl::archive_input_map<
        Archive, boost::unordered_map<Key, Type, Hash, Compare, Allocator> >,
      boost::serialization::stl::no_reserve_imp<boost::unordered_map<
        Key, Type, Hash, Compare, Allocator
        >
                                                >
      >(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Type, class Key, class Hash, class Compare, class Allocator >
inline void serialize(
                      Archive & ar,
                      boost::unordered_map<Key, Type, Hash, Compare, Allocator> &t,
                      const unsigned int file_version
                      ){
  boost::serialization::split_free(ar, t, file_version);
}

} // serialization
} // namespace boost



#endif /* BOOST_SERIALIZATION_UNORDEREDMAP_HH_ */
