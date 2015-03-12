#ifndef COMMON_TYPES_HH_
# define COMMON_TYPES_HH_
# include <string>
# include <map>
# include <vector>
# include <sstream>
# include <fstream>

using std::string;
using std::map;
using std::vector;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::size_t;
using std::stringstream;
using std::ifstream;
using std::ofstream;
using std::istream;
using std::ostream;
using std::endl;

namespace xtd {


template<typename T>
typename std::underlying_type<T>::type valueof(T p_item)
{
  return static_cast<typename std::underlying_type<T>::type>(p_item);
}

enum class status : uint32_t { ok = 0, error = 1, timeout = 2, notfound = 3 };

}

#endif // !COMMON_TYPES_HH_
