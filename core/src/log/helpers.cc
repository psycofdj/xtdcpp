#include "log/helpers.hh"
#include "log/Logger.hh"

namespace xtd {
namespace log {

namespace {

sptr<RootLogger> m_instance;

static const map<string, level> lcs_labels({
    {"emerg",   level::emerg},
    {"alert",   level::alert},
    {"crit",    level::crit},
    {"err",     level::err},
    {"warning", level::warning},
    {"notice",  level::notice},
    {"info",    level::info},
    {"debug",   level::debug}
  });

static const map<level, string> lcs_levels({
    {level::emerg,   "emerg"   },
    {level::alert,   "alert"   },
    {level::crit,    "crit"    },
    {level::err,     "err"     },
    {level::warning, "warning" },
    {level::notice,  "notice"  },
    {level::info,    "info"    },
    {level::debug,   "debug"   }
  });
}

level from(const std::uint32_t& p_level)
{
  if (false == is_valid(p_level))
    return level::crit;
  return to_enum<level>(p_level);
}

level from(const std::string& p_level)
{
  auto c_val = lcs_labels.find(p_level);
  if (lcs_labels.end() != c_val)
    return c_val->second;
  return level::crit;
}

std::uint32_t to_value(level p_level)
{
  return valueof(p_level);
}

std::string to_string(level p_level)
{
  auto c_val = lcs_levels.find(p_level);
  if (lcs_levels.end() != c_val)
    return c_val->second;
  return "crit";
}

bool is_valid(const std::string& p_level)
{
  return (0 != lcs_labels.count(p_level));
}

bool is_valid(const std::uint32_t& p_level)
{
  typename std::underlying_type<level>::type l_val = p_level;
  return
    ((l_val >= valueof(level::emerg)) &&
     (l_val <= valueof(level::debug)));
}

RootLogger& getRoot(void)
{
  if (0 == m_instance.get())
    m_instance.reset(new RootLogger());
  return *m_instance;
}

/**
 ** @brief
 ** @param p_buff
 ** @param p_level
 ** @return
 ** @details
 **
 */
ostream& operator<<(ostream& p_buff, level p_level) {
  return p_buff << to_string(p_level);
}


}}
