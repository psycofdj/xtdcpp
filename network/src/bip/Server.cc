#include "bip/Server.hh"
#include "bip/Server.hxx"
#include <objects/Doc.hh> // lib serializer

/**
 * permet de générer à la compilation les objets réellement utilisés dans le métier
 * et de gagner beaucoup de temps à la compilation (via la réalisation des templates)
 */
namespace xtd {
namespace network {
namespace bip {

template class Server<serializer::Doc, serializer::Doc, utils::af_inet>;
template class Server<serializer::Doc, serializer::Doc, utils::af_unix>;

}}}
