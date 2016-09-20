#include "CWrap.hh"
#include <dlfcn.h>
#include <stdio.h>


namespace xtd {
namespace tests {

CWrap::state::state(const string& p_name, const t_config& p_conf) :
  m_name(p_name),
  m_base(CWrap::get().getConfig(m_name))
{
  CWrap::set(m_name, p_conf);
}

CWrap::state::~state(void)
{
  CWrap::set(m_name, m_base);
}

CWrap::CWrap(void)
{
  m_data.insert(std::make_pair("fread", t_config({
          { t_state({"success", 1}) },
          { t_state({"error",   0}) }
        })));
  for (const auto& cc_item : m_data)
    m_origin.insert(cc_item);
}


void
CWrap::reset(void)
{
  CWrap& l_obj = get();
  l_obj.m_data.clear();
  for (const auto& cc_item : l_obj.m_origin)
    l_obj.m_data.insert(cc_item);
}

std::string
CWrap::gen(const std::string& p_funcName)
{
  return get().generate(p_funcName);
}

std::string
CWrap::generate(const std::string& p_funcName)
{
  t_data::const_iterator cc_func = m_data.find(p_funcName);
  if (m_data.end() == cc_func)
    return "unknown";

  std::uint32_t l_sum   = 0;

  for (const auto& cc_state : cc_func->second)
    l_sum += cc_state.second;

  std::uint64_t l_rand  = random();
  std::uint32_t l_chunk = RAND_MAX / l_sum;

  for (const auto& cc_state : cc_func->second)
    if (l_rand < (l_chunk * cc_state.second))
      return cc_state.first;
  return "unknown";
}


void
CWrap::set(const std::string& p_name, const t_config& p_config)
{
  get().add(p_name, p_config);
}

CWrap::t_config
CWrap::getConfig(const string& p_name)
{
  return m_data[p_name];
}

void
CWrap::add(const std::string& p_name, const t_config& p_config)
{
  m_data[p_name] = p_config;
}

}}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  typedef size_t (*orig_t)(void *ptr, size_t size, size_t nmemb, FILE *stream);
  orig_t l_orig = (orig_t)dlsym(RTLD_NEXT, "fread");

  const std::string& l_state = xtd::tests::CWrap::gen("fread");
  if ("success" == l_state)
    return l_orig(ptr, size, nmemb, stream);
  return 0;
}
