#ifndef NETWORK_DOC_MAIN_HH_
# define NETWORK_DOC_MAIN_HH_



/**
 ** @mainpage
 ** La bibliothèque xtd::network est la base de toutes les communications réseaux utilisée dans le moteur.
 **
 ** Elle s'appuie sur la bibliothèque boost::asio pour fournir des interfaces de haut niveau de client/serveur
 ** http et bip (binary-protocol).
 **
 ** @section sec_main Sommaire
 ** <hr>
 ** <ol>
 **
 **   <li>
 **   @ref sec_boost
 **   <ol>
 **     <li>@ref ssec_boost_practice </li>
 **     <li>@ref ssec_boost_bug </li>
 **   </ol>
 **   </li>
 **
 **   <li>
 **   @ref sec_design
 **   <ol>
 **     <li>@ref ssec_design_obj1 </li>
 **     <li>@ref ssec_design_obj2 </li>
 **   </ol>
 **   </li>
 **
 **   <li>
 **   @ref sec_bip
 **   <ol>
 **     <li>@ref ssec_bip_cnx </li>
 **     <li>@ref ssec_bip_client </li>
 **     <li>@ref ssec_bip_server </li>
 **   </ol>
 **   </li>
 **
 **   <li>
 **   @ref sec_http
 **   <ol>
 **     <li>@ref ssec_http_cnx </li>
 **     <li>@ref ssec_http_server </li>
 **   </ol>
 **   </li>
 **
 **   <li>  @ref sec_usages </li>
 **
 **   <li>  @ref sec_tests </li>
 ** </ol>
 ** <br><br>
 **
 **
 **
 **
 **
 ** <hr>
 ** @section sec_boost Un mot sur boost::asio
 ** <hr>
 **
 ** Boost::asio (http://www.boost.org/doc/libs/1_48_0/doc/html/boost_asio.html) est une bibliothèque réseau et I/O
 ** écrite en c++ et utilisée dans de nombreux software.<br/>
 ** <b>Son utilisation n'est pas triviale</b>, aussi la première chose à faire est de bien lire la documentation
 ** et se familiariser avec la gestion des événements asynchrones et de l'utilisation du boost::asio::io_service.<br/>
 **
 ** <br/>
 ** @subsection ssec_boost_practice Les bonnes pratiques
 **
 ** Pour résumer, on retiendra de la documentation :
 **
 ** -# qu'il est fortement @b déconseillé de mélanger synchrone et asynchrone
 ** -# qu'il n'est pas possible d'utiliser des timeouts en synchrone. En réalité, on peut fermer la socket pendant l'éxécution
 **    d'un événement synchrone mais cette approche n'est pas "naturelle" pour le framework boost::asio est cela nous a posé
 **    beaucoup de problème en production.
 ** -# qu'il faut, en toutes circonstance, garantir la durée de vie des objets et données utilisée dans les
 **    callback des événements asynchrones. L'utilisation des shared_ptr sera ici très utile.
 ** -# qu'il est possible de débugguer la gestion des événements à l'aide de la macro @b -DBOOST_ASIO_ENABLE_HANDLER_TRACKING
 **    (http://www.boost.org/doc/libs/1_48_0/doc/html/boost_asio/overview/core/handler_tracking.html)
 **
 ** <br/>
 ** @subsection ssec_boost_bug Bug dans la version 1.48
 **
 ** Aujourd'hui (18-10-2013), on utilise boost::asio en compilant avec la macro @b -DBOOST_ASIO_DISABLE_EPOLL. Cette macro a pour
 ** effet de forcer l'utilisation de l'appel systeme "select" a la place de son jeune remplaçant "epoll".
 **
 ** Ce point nous a posé énormément de problèmes. Pendant plusieurs semaines, des simples "toy problem" ne marchaient pas
 ** dès que le serveur avait plus qu'une thread en écoute sur son boost::asio::io_service. De temps en temps, le client
 ** envoyait une requête, les données traversent le réseaux (wireshark) mais le serveur ne déclenche pas son événement de lecture
 ** de donnée sur la socket... rien ne se passe et la socket est perdue.
 **
 ** On a jamais trouvé l'explication finale de ce problème, ni de bug précis dans le tracker boost ni d'explication à la lecture
 ** du code. Toujours est-il qu'un faisceau d'indices nous a fait conclure à une faille de boost::asio dans sa gestion du epoll.
 **
 ** Les indices étants :
 ** - de nombreuse va-et-vient dans les changelog asio sur le epoll entre la version 1.48 (qu'on utilise) et la version 1.54 actuelle
 ** - la disparition de ces problèmes lorsqu'on passe en select
 ** - la disparition de ces problèmes lorsqu'on passe en boost::asio 1.52
 **
 **
 **
 **
 **
 ** <br/><hr>
 ** @section sec_design Le design
 ** <hr>
 **
 ** Le design de la bibliothèque xtd::network est composé de deux couches :
 ** - la couche haute xtd::network::protocol qui implémentent les clients serveurs pour les protocoles HTTP et BIP utilisés dans le moteur
 ** - la couche basse xtd::network::base qui fournie les primitives de bas niveau pour l'implémentation de ces protocoles.
 **
 **
 ** @subsection ssec_design_obj1 Objectif n°1 : mutualiser la gestion des sockets entre client et serveur
 **
 ** Pour y parvenir, on crée un objet "Connection" qui sera utilisé à la fois dans le client et dans le serveur. Cet objet fera
 ** office d'interface avec la socket en proposant des primitives simples d'ouverture, d'envoi et de réception de données.
 **
 ** De plus, cet objet va nous aider dans la gestion du multithreading en garantissant, en travaillant sur son propre boost::asio::strand,
 ** qu'une seule opération soit exécutée à la fois.
 **
 ** On se retrouve donc à définir 3 objets dans cette couche basse :
 ** - xtd::network::base::Connection : un objet s'occupant des interactions avec la socket
 ** - xtd::network::base::Client : un client
 ** - xtd::network::base::Server : un serveur
 **
 ** @image html interface.png
 **
 ** On parlera de triplet client/server/connection :
 ** @image html triplet.png
 **
 **
 ** @subsection ssec_design_obj2 Objectif n°2 : rester procotole agnostique
 **
 ** On définit un protocol comme étant :
 ** - le format du message échangé : quelle langue parlent les deux interlocuteurs et, en particulier, comment font-il
 **   pour savoir qu'un message est terminé (problème du morcelement des packets par le transport TCP/IP).
 ** - la séquence du dialogue : qui commence à parler ? qui commence à écouter ? En tout temps, il faut qu'un interlocuteur
 **   écoute lorsque l'autre parle et vis/versa.
 **
 ** Pour rester agnostique du protocol, notre triplet (client, server, connexion) ne doit pas faire de supposition
 ** ni le format du message ni sur l'enchaînement des envois et des réceptions.
 ** En ce qui concerne le format, cela implique que la xtd::network::base::Connection ne peut pas directement
 ** écrire et lire sur la socket. Dans certains cas, on voudra lire une donnée de taille fixe, dans d'autres cas, un
 ** header puis une data, bref, on il faut déléguer ces opérations à la couche xtd::network::protocol.
 ** En ce qui concerne l'enchainement des envois/receptions, cela implique que ces objets ne peuvent pas lancer une réception
 ** ou un envoi, ils ne peuvent que définir ces primitives mais sans jamais les appeler.
 **
 ** Au final, on se retrouve avec :
 **
 ** - xtd::network::base::Connection :
 **   - ownership de la socket
 **   - gestion des envois de message avec timeout (partie abstraite de création et mise sur le socket du message)
 **   - gestion des réceptions de message avec timeout (partie abstraite de décodage et lecture sur le socket du message)
 **   - gestion ouverture et fermeture de la socket
 ** - xtd::network::base::Client :
 **   - gère l'instanciation d'un connexion
 **   - la connexion vers un serveur (utilise xtd::network::base::Connection::connect)
 **   - l'envoie d'un message (utilise xtd::network::base::Connection::send)
 **   - la réception d'un message (utilise xtd::network::base::Connection::receive)
 **   - la fermeture de la connexion (utilise xtd::network::base::Connection::close)
 **   - les compteurs d'exploitation
 ** - xtd::network::base::Server :
 **   - l'initialisation des threads de traitement et de connexion
 **   - l'ouverture d'une connexion entrante (utilise xtd::network::base::Connection::accept)
 **   - réception d'un message (utilise xtd::network::base::Connection::receive)
 **   - envoie d'un message (utilise xtd::network::base::Connection::send)
 **   - fermeture des connexion (utilise xtd::network::base::Connection::close)
 **   - les compteurs d'exploitation
 **
 **
 ** Ensuite, chaque protocole n'a plus qu'a définir ses spécificités en créant
 ** son propre son triplet de client/server/connexion qui dérive du triplet de
 ** base.<br/>
 ** Au final ou abouti au modèle suivant :
 **
 ** @image html diagram.png
 **
 **
 **
 **
 **
 **
 ** <br/><hr>
 ** @section sec_bip Protocol Bip
 ** <hr>
 **
 ** <br/>
 ** @subsection ssec_bip_cnx Format et workflow
 ** @copydoc xtd::network::bip::Connection
 **
 ** <br/>
 ** @subsection ssec_bip_client Client bip
 ** @copydoc xtd::network::bip::Client
 **
 ** <br/>
 ** @subsection ssec_bip_server Serveur bip
 ** @copydoc xtd::network::bip::Server
 **
 **
 **
 **
 **
 ** <br/><hr>
 ** @section sec_http Protocol HTTP
 ** <hr>
 **
 ** <br/>
 ** @subsection ssec_http_cnx Format et workflow
 ** @copydoc xtd::network::http::Connection
 **
 ** <br/>
 ** @subsection ssec_http_server Serveur http
 ** @copydoc xtd::network::http::Server
 **
 **
 **
 **
 ** <hr>
 ** @section sec_usages Matrice d'utilisation
 ** <hr>
 **
 ** <ul>
 **   <li> matrices des utilisations dans le moteurs </li>
 ** </ul>
 **
 ** <hr>
 ** @section sec_tests Les tests
 ** <hr>
 **
 ** <ul>
 **   <li> cachier de tests </li>
 ** </ul>
 */


#endif // !KENETWORK_DOC_MAIN_HH_
