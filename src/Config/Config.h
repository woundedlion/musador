#ifndef CONFIG_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45
#define CONFIG_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45

#include "Server/HTTP.h"
#include "Utilities/Singleton.h"
#include "boost/serialization/nvp.hpp"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

namespace Musador
{

    /// @class Contains configuration data specific to an HTTP site
    class HTTPConfig
    {
    public:
        
        /// Constructor
        HTTPConfig() :
            documentRoot(),
            addr("0.0.0.0"),
            port(5152),
            requireAuth(false),
            realm(L"")
        {}
    
        /// Add an HTTP user to the configuration.
        /// @param[in] user User information for the new user to add.
        void addUser(const HTTP::User& user);
        
        /// Serialize this configuration.
        /// @param[in] ar Destination archive for the serialized data.
        /// @param[in] version The version of the archive. Used by boost::serailization version tracking.
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_NVP(documentRoot);
            ar & BOOST_SERIALIZATION_NVP(addr);
            ar & BOOST_SERIALIZATION_NVP(port);
            ar & BOOST_SERIALIZATION_NVP(requireAuth);
            ar & BOOST_SERIALIZATION_NVP(users);
            ar & BOOST_SERIALIZATION_NVP(realm);
        }

        /// The root directory for the site, available at http://addr:port/
        std::wstring documentRoot;

        /// The IP Address in dot notation on which to listen for requests to this site.
        std::string addr;

        /// The port number on which to listen for requests to this site.
        unsigned short port;

        /// Specifies whether HTTP Digest authentication is required to access this site.
        bool requireAuth;

        /// Collection of users with credentials to access the site.
        HTTP::UserCollection users;

        /// Realm name used for HTTP Authentication
        std::wstring realm;

    };

    class Controller;

    /// @class Contains configuration data specific to an HTTP server.
    class ServerConfig
    {
    public:
        
        /// Collection type for HTTPConfig instances.
        typedef std::vector<HTTPConfig> HTTPSiteCollection;

        /// Serialize this configuration.
        /// @param[in] ar Destination archive for the serialized data.
        /// @param[in] version The version of the archive. Used by boost::serailization version tracking.
       template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_NVP(sites);
        }

        /// Collection of the sites defined for this server.
        HTTPSiteCollection sites;

        /// Used inernally. NOT SERIALIZED.
        /// Pointer to the controller instance which implements application-specific logic
        Controller * controller;
    };

    /// @class Top-level configuration container. Contains the configuration for the entire system.
    class Config : public Singleton<Config>
    {
    public:

        /// Load the configuration data contained in the specified file.
        /// @param[in] The path to the config file
        bool load(const std::wstring& path);

        /// Save the configuration data to specified file.
        /// @param[in] The path to the destination file.
        bool save(const std::wstring& path);

        /// Serialize this configuration.
        /// @param[in] ar Destination archive for the serialized data.
        /// @param[in] version The version of the archive. Used by boost::serailization version tracking.
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_NVP(server);
        }
        
        /// The Server configuration for this application
        ServerConfig server;
    };


}

#endif