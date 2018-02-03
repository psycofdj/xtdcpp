#include "Cache.hh"

namespace xtd {
namespace counters {


Cache::Cache(const string&  p_name,
             uint32_t        p_deltaTimeMs) :
  Composed(p_name),
  m_nb                   ("requests"),
  m_nbFiltered           ("requests.filtered"),
  m_nbAccepted           ("requests.accepted"),
  m_nbAcceptedMiss       ("requests.accepted.miss"),
  m_nbAcceptedHit        ("requests.accepted.hit"),
  m_nbAcceptedHitInvalid ("requests.accepted.hit.invalid"),
  m_nbAcceptedHitValid   ("requests.accepted.hit.valid"),
  m_freqHit              ("perf.hit"),
  m_freqMiss             ("perf.miss"),
  m_iFreqHit             ("perf.instant.hit", p_deltaTimeMs),
  m_iFreqMiss            ("perf.instant.miss", p_deltaTimeMs)
{
  m_nb                   = 0;
  m_nbFiltered           = 0;
  m_nbAccepted           = 0;
  m_nbAcceptedMiss       = 0;
  m_nbAcceptedHit        = 0;
  m_nbAcceptedHitInvalid = 0;
  m_nbAcceptedHitValid   = 0;

  m_freqMiss    = 0;
  m_freqHit     = 0;
  m_iFreqMiss   = 0;
  m_iFreqHit    = 0;

  addItem(m_nb);
  addItem(m_nbFiltered);
  addItem(m_nbAccepted);
  addItem(m_nbAcceptedMiss);
  addItem(m_nbAcceptedHit);
  addItem(m_nbAcceptedHitInvalid);
  addItem(m_nbAcceptedHitValid);
  addItem(m_freqMiss);
  addItem(m_freqHit);
  addItem(m_iFreqMiss);
  addItem(m_iFreqHit);
}

void
Cache::hit(void)
{
  m_nb++;
  m_nbAccepted++;
  m_nbAcceptedHit++;
  m_freqHit.tick();
  m_iFreqHit.tick();
}

void
Cache::miss(void)
{
  m_nb++;
  m_nbAccepted++;
  m_nbAcceptedMiss++;
  m_freqMiss.tick();
  m_iFreqMiss.tick();
}

void
Cache::invalid(void)
{
  m_nbAcceptedHitInvalid++;
}

void
Cache::valid(void)
{
  m_nbAcceptedHitValid++;
}

void
Cache::filtered(void)
{
  m_nb++;
  m_nbFiltered++;
}


}}
