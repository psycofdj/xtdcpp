#ifndef SERVERS_APP_HTTPSERVER_HH_
# define SERVERS_APP_HTTPSERVER_HH_

# include <unistd.h>
# include <vector>
# include <map>
# include <iostream>
# include <ctime>
# include <memory>
# include <boost/lexical_cast.hpp>

# include <Application.hh>   // libcore
# include <network_fwd.hh>   // libnetwork
# include <http/Server.hh>   // libnetwork
# include <utils/Config.hh>  // libnetwork
# include <counters.hh>      // libcounters

using boost::format;
using boost::str;

namespace xtd {
namespace config  { class Parser; }
namespace servers {
namespace param { class Handler; }
namespace app {

class Action
{
public:
  Action(string p_name, string p_description) :
    m_name(p_name),
    m_description(p_description),
    m_log("no log"),
    m_timestamp(boost::posix_time::microsec_clock::local_time())
  {}

  ~Action(void) {}

public:
  string              m_name;
  string              m_description;
  string              m_log;
  boost::posix_time::ptime m_timestamp;
};


class HttpServer :
    public Application,
    public network::http::Server<network::utils::af_inet>
{

private:
  template <typename T>
  struct id_translator
  {
    typedef T internal_type;
    typedef T external_type;

    boost::optional<T> get_value(const T &v) { return  v.substr(1, v.size() - 2) ; }
    boost::optional<T> put_value(const T &v) { return '"' + v +'"'; }
  };

protected:
  typedef HttpServer                                               http_app;
  typedef network::http::Server<network::utils::af_inet>           http_net;
  typedef std::shared_ptr<servers::param::Handler>               t_param_handler;
  typedef counters::CounterManager::t_sptr                         t_prober;
  typedef std::shared_ptr<Action>                                t_action;
  typedef map<string, t_action>                                    t_actions;
  typedef std::shared_ptr<counters::Base>                        t_counter;

public:
  static const uint32_t mcs_defaultProbeDelay        = 30;
  static const uint32_t mcs_defaultTimeoutMs         = 5000;
  static const uint32_t mcs_defaultThresholdMs       = 0;
  static const char     mcs_defaultListenInterface[];

public:
  HttpServer(bool p_isDebug = false);
  virtual ~HttpServer(void);

protected:
  virtual string getCommonConfKey(void)   const;
  virtual string getSpecificConfKey(void) const;
  virtual string getSnmpPath(void)        const;
  virtual void   handleTERM(void);
  virtual int    process(void);
  virtual void   stop(void);
  virtual void   start(void);
  virtual void   parseConfig(void);
  virtual void   parseHttpPort(void);
  virtual void   checkOptions(void);
  virtual void   initialize(void);
  virtual status defineProbes(void);

  void handleUSR2(void);
  void modeProbeActivation(void);
  void setLastAdminLog(const string& p_message);

  void bind_action(const string& p_name,
                   const string& p_description,
                   handler       p_action);

  status h_runAction(const string&                 p_name,
                     handler                       p_action,
                     const uint32_t                p_requestId,
                     const network::http::Request& p_req,
                     network::http::Response&      p_res);


  status addProbe(t_counter     p_counter,
                  const string& p_path);

  template<typename TType>
  status addProbe(const TType&  p_value,
                  const string& p_path,
                  const string& p_name);

  template<typename T>
  status readConf(const std::string& p_key,
                  T&                 p_dst,
                  const T&           p_default);

  template<typename T>
  status readConf(const std::string& p_key, T& p_dst);

private:
  /**
   ** @details
   ** process_mem_usage(double &, double &) - takes two doubles by reference,
   ** attempts to read the system-dependent data for a process' virtual memory
   ** size and resident set size, and return the results in KB.
   **
   ** On failure, returns 0.0, 0.0
   */
  void process_mem_usage(uint32_t& p_vmUsage, uint32_t& p_residentSet);

  status h_actionList(const uint32_t                p_requestID,
                      const network::http::Request& p_req,
                      network::http::Response&      p_res);

  status h_ident(const uint32_t                     p_requestID,
                 const network::http::Request&      p_req,
                 network::http::Response&           p_res);

  status h_index(const uint32_t                     p_requestID,
                 const network::http::Request&      p_req,
                 network::http::Response&           p_res);

  status h_counter(const uint32_t                   p_requestID,
                   const network::http::Request&    p_req,
                   network::http::Response&         p_res);

  status h_admin(const uint32_t                     p_requestID,
                 const network::http::Request&      p_req,
                 network::http::Response&           p_res);


  status h_conf(const uint32_t                      p_requestID,
                const network::http::Request&       p_req,
                network::http::Response&            p_res);

  status h_log(const uint32_t                       p_requestID,
               const network::http::Request&        p_req,
               network::http::Response&             p_res);

  status h_probe(const uint32_t                     p_requestID,
                 const network::http::Request&      p_req,
                 network::http::Response&           p_res);

protected:
  // config
  shared_ptr<config::Parser> m_config;
  string                     m_configPath;
  size_t                     m_nbThread;
  size_t                     m_timeoutMs;
  bool                       m_isModeProbe;
  bool                       m_isDebug;

  // snmp
  string                              m_snmpPath;
  uint32_t                            m_probeDelay;
  t_prober                            m_prober;
  shared_ptr<counters::Value32>       m_ramCounter;
  shared_ptr<counters::AvgTimedValue> m_perfCounter;
  uint32_t                            m_thresholdMs;

  // http
  string                 m_httpHost;
  uint32_t               m_httpPort;
  network::utils::Config m_httpConfig;
  string                 m_httpConfigPath;

  // admin
  t_param_handler m_params;
  string          m_adminDir;
  t_actions       m_actions;
};

}}} //end namespaces

#include "HttpServer.hxx"

#endif // !SERVERS_APP_HTTPSERVER_HH_
