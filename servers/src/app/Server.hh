#ifndef SERVERS_APP_SERVER_HH_
# define SERVERS_APP_SERVER_HH_

# include <unistd.h>
# include <map>
# include <zlib.h>
# include <bip/Server.hh> // libnetwork
# include <counters.hh> // libcounters
# include "app/HttpServer.hh"
# include "param/Base.hh"

namespace xtd {
namespace servers {
namespace app {

template<typename T>
struct Address
{
  static const char mcs_value[];
};


template<typename TReq, typename TRes, typename Domain = network::utils::af_inet>
class Server :
    public network::bip::Server<TReq, TRes, Domain>,
    public servers::app::HttpServer
{
protected:
  typedef Server<TReq, TRes, Domain>                bip_app;
  typedef network::bip::Server<TReq, TRes, Domain>  bip_net;
  typedef param::Base::t_sptr                       t_param_sptr;

private:
  enum class stopper_status : uint32_t {
    disabled = 1,
      waitting = 2,
      enabled  = 3
      };

public:
  static const uint32_t mcs_defaultProbeDelay = 30;
  static const uint32_t mcs_defaultTimeoutMs  = 5000;
  static const uint32_t mcs_httpNbThread      = 5;
  static const char     mcs_defaultListenInterface[];
  static const string   mcs_defaultStructGenDumpPath;

public:
  Server(void);
  ~Server(void);

protected:
  virtual void processRequest(uint32_t    p_requestID,
                              const TReq& p_request,
                              const bool  p_requestDebug,
                              TRes&       p_response,
                              bool&       p_responseDebug) = 0;

  virtual void   parseConfig(void);
  virtual void   parseBipPort(void);
  virtual void   checkOptions(void);
  virtual void   initialize(void);
  virtual status defineProbes(void);
  virtual bool   isResponseValid(const TRes& p_response);
  virtual void   stop(void);
  virtual void   start(void);
  bool           isStructgenMode(void) const;

  status h_query(const uint32_t                p_requestId,
                 const network::http::Request& p_req,
                 network::http::Response&      p_res);

  status h_ihm_query(const uint32_t                p_requestId,
                     const network::http::Request& p_req,
                     network::http::Response&      p_res);

  void setAssertRTT(bool p_assert);

private:
  int  process(void);
  void dumpRequest(const TReq& p_req);
  void processObjectRequest(uint32_t p_requestID,
                            const TReq&  p_request,
                            const bool   p_requestDebug,
                            TRes&        p_response,
                            bool&        p_responseDebug);



protected:
  // bip
  string                 m_bipHost;
  uint32_t               m_bipPort;
  uint32_t               m_bipNbThread;
  network::utils::Config m_bipConfig;
  bool                   m_useCompression;
  stopper_status         m_stopperStatus;
  TRes                   m_stopperResponse;
  boost::mutex           m_structgenMutex;
  string                 m_structgenFilename;

private:
  boost::shared_ptr<counters::AvgTimedValue> m_perfCounter;
  t_param_sptr                               m_assertRTT;
};

}}} //end namespaces

#endif // !SERVERS_APP_SERVER_HH_
