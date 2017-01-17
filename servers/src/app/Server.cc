#include <objects/Doc.hh>        // libserializer
#include "app/Server.hh"
#include "app/Server.hxx"

/**
 * permet de générer à la compilation les objets réellement utilisés dans le métier
 * et de gagner beaucoup de temps à la compilation (via la réalisation des templates)
 */

namespace xtd {
namespace servers {
namespace app {

template class Server<serializer::Doc, serializer::Doc, network::af_unix>;
template class Server<serializer::Doc, serializer::Doc, network::af_inet>;

}}} //end namespaces
