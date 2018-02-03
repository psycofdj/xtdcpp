#ifndef SERIALIZER_SERIALIZER_HXX_
# define SERIALIZER_SERIALIZER_HXX_

# include <sstream>
# include <string>
# include <boost/serialization/nvp.hpp>
# include <boost/range/iterator_range_core.hpp>      // IWYU pragma: keep
# include <boost/iostreams/filtering_stream.hpp>     // IWYU pragma: keep
# include <boost/iostreams/device/back_inserter.hpp> // IWYU pragma: keep

namespace xtd {
namespace serializer {

template<class Mode>
template<class T>
void
serializer<Mode>::save(std::ostream& p_stream,
                       const T&      p_object,
                       bool          p_debug,
                       option        p_opt)
{
  typename Mode::saver l_archive(p_stream, p_debug, p_opt.value());
  l_archive << boost::serialization::make_nvp("object", p_object);
}

template<class Mode>
template<class T>
void
serializer<Mode>::load(std::istream& p_stream, T& p_object, bool& p_debug, option p_opt)
{
  typename Mode::loader l_archive(p_stream, p_opt.value());

  l_archive >> boost::serialization::make_nvp("object", p_object);
  p_debug = l_archive.getDebugState();
}


template<class TSerializable>
std::string as_xml(const TSerializable& p_obj, bool p_debug, option p_opt)
{
  std::stringstream l_buffer;
  serializer<mode::xml>::save(l_buffer, p_obj, p_debug, p_opt);
  return l_buffer.str();
}

template<class TSerializable>
void as_bin(const TSerializable& p_obj,
            vector<char>&   p_data,
            bool                 p_debug,
            int                  p_opt)
{
  boost::iostreams::filtering_stream<boost::iostreams::output> l_fstr;
  l_fstr.push(boost::make_iterator_range(p_data));
  serializer<mode::bin>::save(l_fstr, p_obj, p_debug, p_opt);
}

template<class TSerializable>
vector<char> as_bin(const TSerializable& p_obj, bool p_debug, option p_opt)
{
  vector<char> l_data;
  boost::iostreams::filtering_stream<boost::iostreams::output> l_fstr;
  l_fstr.push(boost::iostreams::back_inserter(l_data));
  serializer<mode::bin>::save(l_fstr, p_obj, p_debug, p_opt);
  return l_data;
}


template<class T>
vector<char> as_text(const T& p_obj, bool p_debug, option p_opt)
{
  vector<char> l_data;
  boost::iostreams::filtering_stream<boost::iostreams::output> l_fstr;
  l_fstr.push(boost::iostreams::back_inserter(l_data));
  serializer<mode::text>::save(l_fstr, p_obj, p_debug, p_opt);
  return l_data;
}




template<class TSerializable>
void from_bin(const vector<char>& p_data,
              TSerializable&      p_obj,
              bool&               p_debug,
              option              p_opt)
{
  boost::iostreams::filtering_stream<boost::iostreams::input> l_fstr;
  l_fstr.push(boost::make_iterator_range(p_data));
  serializer<mode::bin>::load(l_fstr, p_obj, p_debug, p_opt);
}


template<class TSerializable>
void from_bin(const vector<char>& p_data,
              TSerializable&      p_obj,
              option              p_opt)
{
  bool l_debug;
  from_bin(p_data, p_obj, l_debug, p_opt);
}

template<class TSerializable>
void from_text(const vector<char>& p_data,
               TSerializable&      p_obj,
               bool&               p_debug,
               option              p_opt)
{
  boost::iostreams::filtering_stream<boost::iostreams::input> l_fstr;
  l_fstr.push(boost::make_iterator_range(p_data));
  serializer<mode::text>::load(l_fstr, p_obj, p_debug, p_opt);
}



}}

#endif // !SERIALIZER_SERIALIZER_HXX_
