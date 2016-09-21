#ifndef COUNTERS_CACHE_HH_
# define COUNTERS_CACHE_HH_

# include <vector>

# include "Composed.hh"
# include "Value.hh"
# include "Freq.hh"
# include "InstantFreq.hh"


namespace xtd {
namespace counters {

/**
 ** @details
 ** On choisi l'heritage plutot que la composition pour garder une
 ** coherence sur la visibilite des methodes internes collect_safe
 ** update_safe (qu'on serait onbligé de mettre en public ou en friend
 ** pour faire de la composition).
 */
class Cache : public Composed
{
  friend class Composed;

public:
  typedef std::shared_ptr<Cache> t_sptr;

public:
  Cache(const string& p_name,
        uint32_t       p_deltaTimeMs = 10000);

public:
  void hit(void);
  void miss(void);
  void invalid(void);
  void valid(void);
  void filtered(void);

private:
  Value32     m_nb;
  Value32     m_nbFiltered;
  Value32     m_nbAccepted;
  Value32     m_nbAcceptedMiss;
  Value32     m_nbAcceptedHit;
  Value32     m_nbAcceptedHitInvalid;
  Value32     m_nbAcceptedHitValid;
  Freq        m_freqHit;
  Freq        m_freqMiss;
  InstantFreq m_iFreqHit;
  InstantFreq m_iFreqMiss;
};

}}


#endif // !COUNTERS_CACHE_HH_
