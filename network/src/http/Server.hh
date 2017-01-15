#ifndef NETWORK_HTTP_SERVER_HH_
# define NETWORK_HTTP_SERVER_HH_

# include "base/Server.hh"
# include "http/Template.hh"

namespace xtd {
namespace network {
namespace http {

class Request;
class Response;
template <typename Domain> class Connection;
class Template;


std::function<bool(const Request& p_req)>
operator&&(std::function<bool(const Request& p_req)> p_fn1,
           std::function<bool(const Request& p_req)> p_fn2);


std::function<bool(const Request& p_req)>
operator||(std::function<bool(const Request& p_req)> p_fn1,
           std::function<bool(const Request& p_req)> p_fn2);


/**
 ** @param Domain mode de connexion, utils::af_inet ou utils::af_unix
 ** @details
 ** Serveur générique http. Gère la réception de requête HTTP, les transforme en
 ** objet Request, et envoie des réponses HTTP à partir d'objet Response.
 **
 ** En interne, cet objet gère une liste de "handlers", capables de transformer
 ** un objet Request en un objet Response. Il gère également un mécanisme
 ** d'enregistrement et de routage des requête ces différents handlers.
 **
 ** <br>
 ** <h2> Le routage </h2>
 **
 ** Le routage est extrêmement simple. On pacourt la liste des handlers enregistrés,
 ** et on exécute le premier vérifiant toutes les conditions. Si aucun handler est
 ** trouvé, on exécute le handler par défaut.
 **
 ** Pour être exécuté, un handler doit remplir deux critère :
 ** - l'url sur laquelle il à été enregistré correspond a la ressource demandée dans
 **   la requête (ce qui suit le GET ou le POST de la première ligne du header).
 **   On note le cas spécial où un handler peut être enregistré sur toutes les urls
 **   en même temps.
 **
 ** - si le handler a été enregistré avec un filtre, on vérifie la condition posée
 **   le filtre est vrai pour la requête.
 **
 **
 **
 **
 **
 **
 ** <br>
 ** <h2> Les handlers </h2>
 **
 ** Un handler doit être vu comme un pointeur sur fonction dont le prototype serait :
 ** @code
 **   status myhandler(uint32_t p_requestID, const Request& p_req, Response& p_res);
 ** @endcode
 **
 ** En réalité, cet objet demande à ce que les handlers soient construit à partir de ses classes internes,
 ** dont le raccourcis est "h". Exemple :
 ** @code
 **   // une fonction à moi que j'aime
 **   status MyServer::myhandler(uint32_t p_requestID, const Request& p_req, Response& p_res);
 **
 **   // enregistrement de ma fonction comme handler de la ressources /index
 **   bind("/index", h(&MyServer::myhandler, this));
 ** @endcode
 **
 ** La raison pour laquelle ces handlers ont été wrappé dans un objet interne est
 ** d'une part de ne pas demander à l'utilisateur de systématiquement binder 3
 ** placeholders _1, _2, _3.
 **
 **
 **
 **
 **
 ** <br>
 ** <h2> Les filtres </h2>
 **
 ** De la même façon, les filtres doivent être vus comme un pointeur sur fonction dont le prototype serait :
 ** @code
 **   bool filter(const Request&);
 ** @endcode
 **
 ** Ils se contruisent à partir de l'objet interne du server dont le raccourcis est "f". Exemple :
 ** @code
 **   // une fonction à moi que j'aime
 **   status MyServer::myhandler(uint32_t p_requestID, const Request& p_req, Response& p_res);
 **   bool  MyServer::myfilter(const Request& p_req);
 **
 **   // enregistrement de ma fonction comme handler de la ressources /index
 **   bind("/index", h(&MyServer::myhandler, this), f(&MyServer::myfilter, this));
 ** @endcode
 **
 ** Ils ont également une autre fonctionnalité, il peuvent se composer avec les
 ** opérateurs standards booléens ||, && et !. Exemple :
 **
 ** @code
 **   // une fonction à moi que j'aime
 **   status MyServer::myhandler(uint32_t p_requestID, const Request& p_req, Response& p_res);
 **   bool  MyServer::hasHeader(const string& p_headerName, const Request& p_req)
 **   {
 **     return p_req.existsHeader(p_headerName);
 **   }
 **
 **   // enregistrement de ma fonction comme handler de la ressources /index
 **   bind("/index",
 **        h(&MyServer::myhandler, this),
 **        f(&MyServer::hasHeader, this, "Content-type") &&
 **        f(&MyServer::hasHeader, this, "Content-Length"));
 ** @endcode
 **
 **
 **
 ** <br>
 ** <h2> L'enregistrement </h2>
 **
 ** Cet objet founi de nombreuses méthodes utilitaires pour faciliter l'enregistrement des handlers.
 ** Elles sont toutes préfixées par "bind".
 **
 ** - @code void bind(const string& p_url, handler p_handler, [filter p_filter]); @endcode
 **   @copydoc xtd::network::http::Server::bind <br><br>
 **
 ** - @code void bind_any(handler p_handler, [filter p_filter]); @endcode
 **   @copydoc xtd::network::http::Server::bind_any <br><br>
 **
 ** - @code void bind_default(handler p_handler); @endcode
 **   @copydoc xtd::network::http::Server::bind_default <br><br>
 **
 ** - @code void bind_redirect(const string& p_src, const string& p_dst, [filter p_filter]); @endcode
 **   @copydoc xtd::network::http::Server::bind_redirect <br><br>
 **
 ** - @code
 ** void bind_file(const string& p_path,
 **                const string& p_filePath,
 **                const string& p_contentType = "text/plain",
 **                [filter            p_filter]);
 **   @endcode
 **   @copydoc xtd::network::http::Server::bind_file <br><br>
 **
 ** - @code
 ** void bind_dir(const string& p_path,
 **               const string& p_filePath,
 **               const string& p_contentType = "text/plain",
 **               [filter            p_filter]);
 **   @endcode
 **   @copydoc xtd::network::http::Server::bind_dir <br><br>
 **
 **
 ** <h2> Les handlers prédéfinis </h2>
 **
 ** - @code h_redirect(const string& p_dst, const uint32_t p_requestId, const Request& p_request, Response& p_response); @endcode
 **   @copydoc xtd::network::http::Server::h_redirect
 **
 ** - @code h_raw(const string& p_data, const string& p_contentType, const uint32_t p_requestId, const Request& p_request, Response& p_response); @endcode
 **   @copydoc xtd::network::http::Server::h_raw
 **
 ** - @code h_file(const string& p_filePath, const string& p_contentType, const uint32_t p_requestId, const Request& p_request, Response& p_response); @endcode
 **   @copydoc xtd::network::http::Server::h_file
 **
 ** - @code h_dir(const string& p_dirPath, const string& p_contentType, const uint32_t p_requestId, const Request& p_request, Response& p_response); @endcode
 **   @copydoc xtd::network::http::Server::h_dir
 **
 ** - @code h_template_file(const Template& p_tmpl, const string& const uint32_t p_requestID, const Request& p_request, Response& p_response); @endcode
 **   @copydoc xtd::network::http::Server::h_template_file
 **
 ** - @code h_error_text(const string& p_message, const uint32_t p_requestId, const Request& p_request, Response& p_response); @endcode
 **   @copydoc xtd::network::http::Server::h_error_text
 **
 ** - @code h_error_html(const string& p_message, const uint32_t p_requestId, const Request& p_request, Response& p_response); @endcode
 **   @copydoc xtd::network::http::Server::h_error_html
 **
 ** <h2> Les filtres prédéfinis </h2>
 **
 **
 ** - @code static bool f_none(const Request& p_request); @endcode
 **   @copydoc xtd::network::http::Server::f_none <br><br>
 **
 ** - @code bool f_cgi_exist(const string& p_cgiName, const Request& p_request); @endcode
 **   @copydoc xtd::network::http::Server::f_cgi_exist <br><br>
 **
 ** - @code bool f_one_cgi_exist(const vector <string > & p_cgiName, const Request& p_req); @endcode
 **   @copydoc xtd::network::http::Server::f_one_cgi_exist <br><br>
 **
 ** - @code bool f_cgi_equal(const string& p_cgiName, const string& p_value, const Request& p_request); @endcode
 **   @copydoc xtd::network::http::Server::f_cgi_equal <br><br>
 **
 ** - @code bool f_cgi_match(const string& p_cgiName, const string& p_regex, const Request& p_request); @endcode
 **   @copydoc xtd::network::http::Server::f_cgi_match <br><br>
 **
 ** - @code bool f_post_exist(const string& p_cgiName, const Request& p_request); @endcode
 **   @copydoc xtd::network::http::Server::f_post_exist <br><br>
 **
 ** - @code bool f_post_equal(const string& p_cgiName, const string& p_value, const Request& p_request); @endcode
 **   @copydoc xtd::network::http::Server::f_post_equal <br><br>
 **
 ** - @code bool f_post_match(const string& p_cgiName, const string& p_regex, const Request& p_request); @endcode
 **   @copydoc xtd::network::http::Server::f_post_match <br><br>
 **
 ** - @code bool f_header_exist(const string& p_headerName, const Request& p_request); @endcode
 **   @copydoc xtd::network::http::Server::f_header_exist <br><br>
 **
 ** - @code bool f_header_equal(const string& p_headerName, const string& p_value, const Request& p_request); @endcode
 **   @copydoc xtd::network::http::Server::f_header_equal <br><br>
 **
 ** - @code bool f_header_match(const string& p_headerName, const string& p_value, const Request& p_request); @endcode
 **   @copydoc xtd::network::http::Server::f_header_match <br><br>
 **
 */
template<typename Domain = utils::af_inet>
class Server : public base::Server<Domain>
{
private:
  typedef base::Server<Domain>       TBase;
  typedef typename TBase::cnx_sptr_t cnx_sptr_t;

public:
  Server(void);
  virtual ~Server(void);

  /**
   ** @brief invoke base start method and print log
   */
  virtual void start(void);

  /**
   ** @brief invoke base stop method and print log
   */
  virtual void stop(void);

private:
  cnx_sptr_t createCnx(string p_hostname, uint32_t p_port);
  void       afterAccept(cnx_sptr_t p_conn);
  void       afterSend(cnx_sptr_t   p_conn);
  void       afterReceive(cnx_sptr_t p_conn, utils::sharedBuf_t p_inBuffer);
  void       processRequest(uint32_t p_processID, std::istream& p_request, Request& p_req, Response& p_res);
  void       onReceiveError(const boost::system::error_code p_error, cnx_sptr_t p_conn);
  void       onReceiveTimeout(const boost::system::error_code p_error, cnx_sptr_t p_conn);

protected:
  class Handler
  {
  public:
    typedef vector<Handler>                                                  t_listof;
    typedef std::function<status(const uint32_t, const Request&, Response&)> t_process;
    typedef std::function<bool(const Request&)>                              t_filter;

  public:
    static bool less(const Handler& p_obj1, const Handler& p_obj2);

  public:
    Handler(void);

  public:
    string    m_path;
    t_process m_process;
    t_filter  m_filter;
    bool      m_matchAny;
    string    m_descr;
  };

protected:
  typedef typename Handler::t_process handler;
  typedef typename Handler::t_filter  filter;

protected:
  template<typename T, typename... Arguments>
  handler h(T p_fn, Arguments&&... p_args);

  template<typename T, typename... Arguments>
  filter f(T p_fn, Arguments&&... p_args);

protected:
  // filters
  /**
   ** @brief Toujours vrai
   */
  static bool f_none(const Request& p_request);

  /**
   ** @brief Vrai si la requête contient un paramètre GET nommé p_cgiName
   */
  bool f_cgi_exist(const string&  p_cgiName,
                   const Request& p_request);


  /**
   ** @details
   ** Vrai si la requête contient un paramètre GET dont le nom
   ** correspond à l'un des éléments du tableau p_cgiName
   */
  bool f_one_cgi_exist(const vector<string>& p_cgiName,
                       const Request&        p_req);

  /**
   ** @details
   ** Vrai si la requête contient un paramètre GET nommé p_cgiName
   ** et dont la valeur est égale à p_value
   */
  bool f_cgi_equal(const string&  p_cgiName,
                   const string&  p_value,
                   const Request& p_request);

  /**
   ** @details
   ** Vrai si la requête contient un paramètre GET nommé p_cgiName
   ** et dont la valeur match la regexp p_regex
   */
  bool f_cgi_match(const string&  p_cgiName,
                   const string&  p_regex,
                   const Request& p_request);

  /**
   ** @brief Vrai si la requête contient un paramètre POST nommé p_cgiName
   */
  bool f_post_exist(const string&  p_cgiName,
                    const Request& p_request);

  /**
   ** @brief Vrai si la requête contient un paramètre POST ou GET nommé p_cgiName
   */
  bool f_param_exist(const string&  p_cgiName,
                     const Request& p_request);

  /**
   ** @details
   ** Vrai si la requête contient un paramètre POST nommé p_cgiName
   ** et dont la valeur est égale à p_value
   */
  bool f_post_equal(const string&  p_cgiName,
                    const string&  p_value,
                    const Request& p_request);


  /**
   ** @details
   ** Vrai si la requête contient un paramètre POST nommé p_cgiName
   ** et dont la valeur match la regexp p_regex
   */
  bool f_post_match(const string&  p_cgiName,
                    const string&  p_regex,
                    const Request& p_request);

  /**
   ** @brief Vrai si p_request contient le header p_headerName
   */
  bool f_header_exist(const string&  p_headerName,
                      const Request& p_request);

  /**
   ** @details
   ** Vrai si p_request contient le header p_headerName dont la valeur est egale
   ** a p_value
   */
  bool f_header_equal(const string&  p_headerName,
                      const string&  p_value,
                      const Request& p_request);

  /**
   ** @details
   ** Vrai si p_request contient le header p_headerName dont la valeur match la
   ** regexp p_value
   */
  bool f_header_match(const string&  p_headerName,
                      const string&  p_value,
                      const Request& p_request);

  // handlers
  /**
   ** @brief Handler de redirection
   ** @param p_dst destination de la redirection HTTP
   ** @param p_requestId identifiant de requête
   ** @param p_request @ref xtd::network::http::Request "requête"
   ** @param p_response @ref xtd::network::http::Response "réponse"
   ** @details
   ** Créer une redirection HTTP Response::STATUS_302 "code 302" redirigeant sur p_dst
   */
  status h_redirect(const string&  p_dst,
                    const uint32_t p_requestId,
                    const Request& p_request,
                    Response&      p_response);

  /**
   ** @brief Handler de contenu
   ** @param p_data donnée à insérer dans la réponse
   ** @param p_contentType type MIME de la donnée
   ** @param p_response @ref xtd::network::http::Response "réponse"
   ** @details
   ** Créer une réponse HTTP (Response::STATUS_200 contenant la
   ** donnée p_data et le header @b Content-Type p_contentType
   */
  status h_raw(const string&     p_data,
               const string&     p_contentType,
               const uint32_t /* p_requestId*/,
               const Request& /* p_request */,
               Response&         p_response);

  /**
   ** @brief Handler de fichier
   ** @param p_filePath chemin vers le fichier à insérer dans la réponse
   ** @param p_contentType type MIME du fichier
   ** @param p_cachable La reponse peut elle mettre en mis en cache par le navigateur ?
   ** @param p_requestId identifiant de requête
   ** @param p_request @ref xtd::network::http::Request "requête"
   ** @param p_response @ref xtd::network::http::Response "réponse"
   ** @details
   ** Créer une réponse HTTP Response::STATUS_200 embarquant le
   ** contenu du fichier pointé par p_filePath et le header @b Content-Type p_contentType.
   ** Si p_filePath n'éxiste pas, la réponse sera générée par @ref xtd::network::http::Server::h_error_text.
   */
  status h_file(const string&  p_filePath,
                const string&  p_contentType,
                bool           p_cachable,
                const uint32_t p_requestId,
                const Request& p_request,
                Response&      p_response);

  /**
   ** @brief Handler de répertoire
   ** @param p_dirPath chemin vers le répertoire à servir
   ** @param p_contentType type MIME des fichiers du répertoire
   ** @param p_cachable La reponse peut elle mettre en mis en cache par le navigateur ?
   ** @param p_requestId identifiant de requête
   ** @param p_request @ref xtd::network::http::Request "requête"
   ** @param p_response @ref xtd::network::http::Response "réponse"
   ** @details
   ** Comme @ref xtd::network::http::Server::h_file mais trouve automatiquement quel
   ** fichier de p_dirPath à servir en fonction de la ressource demandé dans p_request.
   */
  status h_dir(const string&  p_dirPath,
               const string&  p_contentType,
               bool           p_cachable,
               const uint32_t p_requestId,
               const Request& p_request,
               Response&      p_response);


  /**
   ** @brief Handler de texte templaté a partir d'un fichier
   ** @param p_tmpl objet @ref xtd::network::http::Template "template"
   ** @param p_filePath fichier a partir duquel initialiser le template
   ** @param p_requestID identifiant de requête
   ** @param p_req @ref xtd::network::http::Request "requête"
   ** @param p_res @ref xtd::network::http::Response "réponse"
   ** @details
   ** Génère une réponse HTTP pré-formatée par l'objet Template p_tmpl Response::STATUS_200.
   ** Le header @b Content-Type est également donné par p_tmpl.
   ** Si la lecture du fichier p_filePath ou si la résolution des variable
   ** du template échouent, alors la réponse sera générée par
   ** @ref xtd::network::http::Server::h_error_text.
   */
  status h_template_file(Template&      p_tmpl,
                         const string&  p_filePath,
                         const uint32_t p_requestID,
                         const Request& p_req,
                         Response&      p_res);


  /**
   ** @brief Handler de generation de texte
   ** @param p_gen objet @ref xtd::network::http::Generator "generateur"
   ** @param p_requestId identifiant de requête
   ** @param p_request @ref xtd::network::http::Request "requête"
   ** @param p_response @ref xtd::network::http::Response "réponse"
   ** @details
   ** Génère une réponse HTTP pré-formatée par l'objet Template p_tmpl Response::STATUS_200.
   ** Le header @b Content-Type est également donné par p_tmpl.
   ** Si la résolution des variable du template échoue, alors la réponse sera générée
   ** par @ref xtd::network::http::Server::h_error_text.
   */
  status h_gen(Generator&     p_gen,
               const uint32_t p_requestId,
               const Request& p_request,
               Response&      p_response);


  /**
   ** @brief Handler de génération de message d'erreur en texte
   ** @param p_message contenu du message d'erreur
   ** @param p_requestId identifiant de requête
   ** @param p_request @ref xtd::network::http::Request "requête"
   ** @param p_response @ref xtd::network::http::Response "réponse"
   ** @details
   ** Génère une réponse HTTP d'erreur Response::STATUS_500 de type
   ** @b Content-Type "text/plain" contenant le message p_message.
   */
  status h_error_text(const string&  p_message,
                      const uint32_t p_requestId,
                      const Request& p_request,
                      Response&      p_response);


  /**
   ** @brief Handler de génération de message d'erreur en html
   ** @param p_message contenu du message d'erreur
   ** @param p_requestId identifiant de requête
   ** @param p_request @ref xtd::network::http::Request "requête"
   ** @param p_response @ref xtd::network::http::Response "réponse"
   ** @details
   ** Même chose que @ref xtd::network::http::Server::h_error_text mais le message
   ** généré est de type "text/html".
   */
  status h_error_html(const string&  p_message,
                      const uint32_t p_requestId,
                      const Request& p_request,
                      Response&      p_response);



  // binders
  /**
   ** @param p_src ressource sur laquelle déclencher la redirection
   ** @param p_dst destination de la redirection
   ** @param p_filter filter "filtre" optionnel
   ** @details
   ** Enregistrement d'un handler de redirection. Créer une réponse http qui contient
   ** le header "Location : p_dst" et le code HTTP Response::STATUS_302.
   */
  void bind_redirect(const string& p_src,
                     const string& p_dst,
                     filter        p_filter = &Server::f_none);


  void bind_xml(const string& p_url,
                const string& p_xmlPath,
                const string& p_xsltPath,
                filter        p_filter = &Server::f_none);

  /**
   ** @details
   ** Voir @ref xtd::network::http::Server::h_file
   */
  void bind_file(const string& p_path,
                 const string& p_filePath,
                 const string& p_contentType,
                 bool          p_cachable,
                 filter        p_filter = &Server::f_none);

  /**
   ** @details
   ** Voir @ref xtd::network::http::Server::h_dir
   */
  void bind_dir(const string& p_url,
                const string& p_filePath,
                const string& p_contentType,
                bool          p_cachable,
                filter        p_filter = &Server::f_none);

  /**
   ** @param p_url ressource à enregistrer
   ** @param p_handler handler à enregistrer
   ** @param p_filter filter "filtre" optionnel
   ** @param p_descr description du handler (optionnel)
   ** @details
   ** Enregistrement générique de la ressource p_url sous condition optionnelle p_filter
   ** sur le handler p_handler.
   */
  void bind(const string& p_url,
            handler       p_handler,
            filter        p_filter = &Server::f_none,
            const string& p_descr  = string());


  /**
   ** @param p_url ressource à enregistrer
   ** @param p_handler handler à enregistrer
   ** @param p_descr description du handler
   ** @details
   ** Enregistrement générique de la ressource p_url sur le handler p_handler.
   ** Cette ressource sera automatique mise a disposition dans
   ** la création de l'index automatique.
   */
  void bind_public(const string& p_url,
                   handler       p_handler,
                   const string& p_descr);


  /**
   ** @param p_handler handler à enregistrer
   ** @param p_filter filter "filtre" optionnel
   ** @details
   ** Comme @ref xtd::network::http::Server::bind mais se déclenche quelque soit la
   ** ressource demandée.
   */
  void bind_any(handler p_handler,
                filter  p_filter = &Server::f_none);

  /**
   ** @param p_handler handler à enregistrer
   ** @details
   ** Enregistrement du handler à exécuter lorsqu'aucun handler valide n'a été trouvé.
   ** A la construction, le handler par défaut est Server::h_error_template.
   */
  void bind_default(handler p_handler);


  /**
   ** @details Cree la liste des handler accessibles sans parametres
   */
  void getMatchingHandlers(const Request&              p_req,
                           typename Handler::t_listof& p_res) const;

private:
  bool     isUrlBinded(const string& p_urlPath) const;
  handler& findHandler(const Request& p_req);

private:
  handler                    m_defaultHandler;
  typename Handler::t_listof m_handlerList;
};

}}} //end namespaces

#endif // !NETWORK_HTTP_SERVER_HH_
