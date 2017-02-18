#include <MainTestApplication.hh> // libtests
#include <TestFixture.hh>         // libtests
#include <http/Server.hh>         // libnetwork
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <cstdlib>
#include <sys/wait.h>
#include <http/Request.hh>
#include <http/Response.hh>
#include <http/Client.hh>
#include <base/ThreadManager.hh>

using namespace xtd;
using namespace xtd::network::http;


class TestHttpServer;

class TestServer : public Server<xtd::network::af_inet>
{
  friend class TestHttpServer;
public:
  virtual ~TestServer(void) {}
};

class TestHttpServer : public tests::TestFixture
{
  CPPUNIT_TEST_SUITE(TestHttpServer);
  CPPUNIT_TEST(unhandledRequest);
  CPPUNIT_TEST(malformedRequest);
  CPPUNIT_TEST(simpleHandler);
  CPPUNIT_TEST(filteredHandler);
  CPPUNIT_TEST(anyHandler);
  CPPUNIT_TEST(expiredConnection);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp(void);
  void tearDown(void);

private:
  void unhandledRequest(void);
  void malformedRequest(void);
  void simpleHandler(void);
  void filteredHandler(void);
  void anyHandler(void);
  void expiredConnection(void);

private:
  sptr<TestServer> m_server;
  size_t           m_port;
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestHttpServer);


void
TestHttpServer::setUp(void)
{
  network::base::ThreadManager::get();
  m_server.reset(new TestServer());
  m_server->initialize("127.0.0.1", 0, 10);
  m_server->start();
  m_port = m_server->getEndPoint().port();
}

void
TestHttpServer::tearDown(void)
{
  m_server->stop();
  m_server->join();
  network::base::ThreadManager::destroy();
}

void
TestHttpServer::unhandledRequest(void)
{
  string l_cmd    = format::vargs("wget -q -O/dev/null http://localhost:%d >/dev/null 2>&1", m_port);
  exec(l_cmd, 8, __FILE__, __LINE__);
}

void
TestHttpServer::malformedRequest(void)
{
  string l_cmd = format::vargs("echo 'GET / HTTP/1.0\r\nContent-Length:1\r\nContent-type:test\r\n\r\na' | nc localhost %d >/dev/null", m_port);
  exec(l_cmd, 0, __FILE__, __LINE__);
}


void
TestHttpServer::simpleHandler(void)
{
  TestServer::handler l_handler = [](uint32_t, const Request&, Response& p_res)  {
    p_res.setStatus(code::ok);
    return status::ok;
  };

  m_server->bind("/test", l_handler);
  string l_cmd = format::vargs("wget -q -O/dev/null http://localhost:%d/test", m_port);
  exec(l_cmd, 0, __FILE__, __LINE__);
}


void
TestHttpServer::filteredHandler(void)
{
  TestServer::handler l_200 = [](uint32_t, const Request&, Response& p_res)  {
    p_res.setStatus(code::ok);
    return status::ok;
  };
  TestServer::handler l_404 = [](uint32_t, const Request&, Response& p_res)  {
    p_res.setStatus(code::not_found);
    return status::ok;
  };

  m_server->bind("/test", l_200, [](const Request& p_req) {
      string l_val;
      return (true == p_req.getCgi("200", l_val));
    });
  m_server->bind("/test", l_404);

  string l_cmd = format::vargs("wget -q -O/dev/null http://localhost:%d/test?200=1", m_port);
  exec(l_cmd, 0, __FILE__, __LINE__);
  l_cmd = format::vargs("wget -q -O/dev/null http://localhost:%d/test", m_port);
  exec(l_cmd, 8, __FILE__, __LINE__);
}

void
TestHttpServer::anyHandler(void)
{
  TestServer::handler l_200 = [](uint32_t, const Request&, Response& p_res)  {
    p_res.setStatus(code::ok);
    return status::ok;
  };
  TestServer::handler l_404 = [](uint32_t, const Request&, Response& p_res)  {
    p_res.setStatus(code::not_found);
    return status::ok;
  };

  m_server->bind_any(l_200);
  m_server->bind("/test", l_404);

  {
    string l_cmd = format::vargs("wget -q -O/dev/null http://localhost:%d/", m_port);
    exec(l_cmd, 0, __FILE__, __LINE__);
  }

  {
    string l_cmd = format::vargs("wget -q -O/dev/null http://localhost:%d/foo", m_port);
    exec(l_cmd, 0, __FILE__, __LINE__);
  }

  {
    string l_cmd = format::vargs("wget -q -O/dev/null http://localhost:%d/bar", m_port);
    exec(l_cmd, 0, __FILE__, __LINE__);
  }

  {
    // specific handlers have precedence on wildcar handlers
    string l_cmd = format::vargs("wget -q -O/dev/null http://localhost:%d/test", m_port);
    exec(l_cmd, 8, __FILE__, __LINE__);
  }
}

void
TestHttpServer::expiredConnection(void)
{
  Client<network::af_inet> l_client;
  string   l_value;

  Request  l_req;
  Response l_res;
  l_req.setPath("/");
  l_req.setMethod(method::get);
  l_req.setVersion(version::v1_0);

  // simple 1.0 request, no server keep alive
  CPPUNIT_ASSERT_EQUAL(status::ok, l_client.connect("127.0.0.1", m_port));
  CPPUNIT_ASSERT_EQUAL(status::ok, l_client.send(l_req));
  CPPUNIT_ASSERT_EQUAL(status::ok, l_client.receive(l_res));
  CPPUNIT_ASSERT_EQUAL(false,           l_res.getHeader("Keep-Alive", l_value));
  l_client.close();

  l_res = Response();
  l_req.setVersion(version::v1_1);
  CPPUNIT_ASSERT_EQUAL(status::ok, l_client.connect("127.0.0.1", m_port));
  CPPUNIT_ASSERT_EQUAL(status::ok, l_client.send(l_req));
  CPPUNIT_ASSERT_EQUAL(status::ok, l_client.receive(l_res));
  // this time server let the cnx open
  CPPUNIT_ASSERT_EQUAL(true,            l_res.getHeader("Keep-Alive", l_value));
  CPPUNIT_ASSERT_EQUAL(string("timeout=1, max=100"), l_value);

  l_res = Response();
  // we can resend a request on same cnx
  CPPUNIT_ASSERT_EQUAL(status::ok, l_client.send(l_req));
  CPPUNIT_ASSERT_EQUAL(status::ok, l_client.receive(l_res));
  CPPUNIT_ASSERT_EQUAL(string("timeout=1, max=100"), l_value);

  l_res = Response();
  // delay is expired, server should have closed the cnx
  boost::this_thread::sleep(boost::posix_time::milliseconds(3000));
  CPPUNIT_ASSERT_EQUAL(status::ok,    l_client.send(l_req));
  CPPUNIT_ASSERT_EQUAL(status::error, l_client.receive(l_res));
}

XTD_TEST_MAIN();


// Local Variables:
// ispell-local-dictionary: "en"
// End:
