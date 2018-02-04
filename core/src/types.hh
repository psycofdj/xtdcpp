#ifndef COMMON_TYPES_HH_
# define COMMON_TYPES_HH_

# include <sys/types.h> // IWYU pragma: export
# include <cstdint>     // IWYU pragma: export
# include <cstddef>     // IWYU pragma: export
# include <fstream>     // IWYU pragma: export
# include <functional>  // IWYU pragma: export
# include <map>         // IWYU pragma: export
# include <memory>      // IWYU pragma: export
# include <string>      // IWYU pragma: export
# include <type_traits> // IWYU pragma: export
# include <utility>     // IWYU pragma: export
# include <vector>      // IWYU pragma: export
# include <stdint.h>    // for uint16_t, uint32_t, uint64_t, uint8_t
# include <iosfwd>      // for ostream, ifstream, istream, ofstream, stringstream
# include <ostream>     // for endl

# include <bits/c++config.h>
# include <bits/shared_ptr.h>
# include <bits/stl_map.h>
# include <bits/stl_pair.h>
# include <bits/stl_vector.h>
# include <bits/stringfwd.h>


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
using std::pair;
using std::shared_ptr;

/**
 ** @brief Alias to std::shared_ptr<T>
 */
template<class T> using sptr = std::shared_ptr<T>;

/**
 ** @brief Alias to std::function<T>
 */
template<class T> using fn   = std::function<T>;

namespace xtd {

/**
 ** @brief Returns internal value of an enum class
 ** @param p_item Enum class label
 ** @return Value of p_item
 */
template<typename T>
typename std::underlying_type<T>::type valueof(T p_item)
{
  return static_cast<typename std::underlying_type<T>::type>(p_item);
}


/**
 ** @brief Converts underlying type to its corresponding class enum
 ** @brief
 ** @param value underlying enum value
 */
template<typename E, typename T>
constexpr typename std::enable_if<std::is_enum<E>::value && std::is_integral<T>::value, E>::type
  to_enum(T value) noexcept
{
  return static_cast<E>(value);
}

/**
 ** @brief Generic status object
 ** @details
 ** Meaning of labels may differ from a function to another, the following tag
 ** descriptions are a general guide line. Please refer to the functions documentation
 ** for a more precise meaning of different statuses.
 */
enum class status : uint32_t
{

  ok       = 0, ///< everything went fine
  error    = 1, ///< encountered unrecoverable error
  timeout  = 2, ///< process timeout, may be retried
  notfound = 3, ///< searched item not found
  next     = 4  ///< everything wen't fine, more data is available
 };


/**
 ** @brief Dumps @ref status value to ostream
 */
ostream& operator<<(ostream& p_buf, const xtd::status& p_status);

}

#endif // !COMMON_TYPES_HH_

// Local Variables:
// ispell-local-dictionary: "american"
// End:
