#include "libc_wrapper.hh"
#include <dlfcn.h>
#include <stdio.h>

std::shared_ptr<LibCWrapper> LibCWrapper::ms_instance;

LibCWrapper::LibCWrapper(void)
{
}


void
LibCWrapper::initialize(void)
{
  set("fread", {
      {"success", 1},
      {"error",   0}
    });

  for (const auto& cc_item : m_data)
    m_origin.insert(cc_item);
}


void
LibCWrapper::reset(void)
{
  if (nullptr == ms_instance.get())
  {
    ms_instance.reset(new LibCWrapper());
    ms_instance->initialize();
  }

  ms_instance->m_data.clear();
  for (const auto& cc_item : ms_instance->m_origin)
    ms_instance->m_data.insert(cc_item);
}

std::string
LibCWrapper::get(const std::string& p_funcName)
{
  if (nullptr == ms_instance.get())
  {
    ms_instance.reset(new LibCWrapper());
    ms_instance->initialize();
  }
  return ms_instance->generate(p_funcName);
}

std::string
LibCWrapper::generate(const std::string& p_funcName)
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
LibCWrapper::set(const std::string& p_name, const t_config& p_config)
{
  if (nullptr == ms_instance.get())
  {
    ms_instance.reset(new LibCWrapper());
    ms_instance->initialize();
  }
  ms_instance->add(p_name, p_config);
}

void
LibCWrapper::add(const std::string& p_name, const t_config& p_config)
{
  m_data[p_name] = p_config;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  typedef size_t (*orig_t)(void *ptr, size_t size, size_t nmemb, FILE *stream);
  orig_t l_orig = (orig_t)dlsym(RTLD_NEXT, "fread");

  const std::string& l_state = LibCWrapper::get("fread");
  if ("success" == l_state)
    return l_orig(ptr, size, nmemb, stream);
  return 0;
}
