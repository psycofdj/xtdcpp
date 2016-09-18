#ifndef COMMON_TYPES_HH_
# define COMMON_TYPES_HH_
# include <string>
# include <map>
# include <vector>
# include <sstream>
# include <fstream>
# include <memory>

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

template<class T> using sptr = std::shared_ptr<T>;
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
