#include "CWrap.hh"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>

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

CWrap::hook_state::hook_state(const string& p_name, const t_hook& p_hook) :
  m_name(p_name),
  m_base(CWrap::get().getHooks())
{
  CWrap::addHook(m_name, p_hook);
}

CWrap::hook_state::~hook_state(void)
{
  CWrap::setHooks(m_base);
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

CWrap::t_hooks
CWrap::getHooks(void)
{
  return m_hooks;
}


void
CWrap::runHooks(const string& p_name, const boost::any& p_args)
{
  CWrap& l_this = get();
  std::cout << "running hooks for " << p_name << std::endl;

  t_hooks::const_iterator c_begin = l_this.m_hooks.lower_bound(p_name);
  t_hooks::const_iterator c_end   = l_this.m_hooks.upper_bound(p_name);

  while (c_begin != c_end)
  {
    std::cout << "running hook" << std::endl;
    c_begin->second(p_args);
    c_begin++;
  }
}

void
CWrap::addHook(const string& p_name, const t_hook& p_hook)
{
  CWrap& l_this = get();
  l_this.m_hooks.insert(std::make_pair(p_name, p_hook));
}

void
CWrap::setHooks(const t_hooks& p_hooks)
{
  CWrap& l_this = get();
  l_this.m_hooks = p_hooks;
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


void
openlog(const char *ident, int option, int facility)
{
  typedef std::tuple<const char*, int, int> t_args;
  typedef void (*orig_t)(const char*, int, int);

  t_args     l_args   = std::make_tuple(ident, option, facility);
  orig_t     l_orig   = (orig_t)dlsym(RTLD_NEXT, "openlog");
  boost::any l_params = l_args;

  xtd::tests::CWrap::runHooks("openlog", l_params);
  return l_orig(ident, option, facility);
}


void
closelog(void)
{
  typedef void (*orig_t)(void);

  orig_t     l_orig   = (orig_t)dlsym(RTLD_NEXT, "closelog");
  boost::any l_params;
  xtd::tests::CWrap::runHooks("closelog", l_params);
  return l_orig();
}


void
vsyslog (int priority, const char *msg_format, va_list ap)
{
  typedef std::tuple<int, const char*> t_args;
  typedef void (*orig_t)(int, const char *, va_list);

  t_args     l_args   = std::make_tuple(priority, msg_format);
  orig_t     l_orig   = (orig_t)dlsym(RTLD_NEXT, "vsyslog");
  boost::any l_params = l_args;

  xtd::tests::CWrap::runHooks("vsyslog", l_params);
  return l_orig(priority, msg_format, ap);
}

void
syslog(int priority, const char *msg_format, ...)
{
  va_list ap;

  va_start (ap, msg_format);
  vsyslog(priority, msg_format, ap);
  va_end (ap);
}
