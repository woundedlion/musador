#ifndef CONFIG_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45
#define CONFIG_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45

#include <map>
#include "Server/HTTP.h"
#include "Utilities/Singleton.h"
#include "Utilities/Property.h"
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
            documentRoot(L""),
            addr("0.0.0.0"),
            port(5152),
            requireAuth(false),
            realm(L"")
        {}
            
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
        Property<std::wstring> documentRoot;

        /// The IP Address in dot notation on which to listen for requests to this site.
        Property<std::string> addr;

        /// The port number on which to listen for requests to this site.
        Property<unsigned short> port;

        /// Specifies whether HTTP Digest authentication is required to access this site.
        Property<bool> requireAuth;

        /// Collection of users with credentials to access the site.
        Property<HTTP::UserCollection> users;

        /// Realm name used for HTTP Authentication
        Property<std::wstring> realm;

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
        Property<HTTPSiteCollection> sites;

        /// Used inernally. NOT SERIALIZED.
        /// Pointer to the controller instance which implements application-specific logic
        Property<Controller *> controller;
    };

    /// @class Contains configuration data specific to a music library
    class LibraryConfig
    {
    public:

        /// Serialize the configuration of this Library.
        /// @param[in] ar Destination archive for the serialized data.
        /// @param[in] version The version of the archive. Used by boost::serailization version tracking.
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_NVP(id);
            ar & BOOST_SERIALIZATION_NVP(nickname);
            ar & BOOST_SERIALIZATION_NVP(dataFile);
            ar & BOOST_SERIALIZATION_NVP(targets);
        }

        /// The numeric Library ID.
        Property<int> id;

        /// User-defined nickname for this Library        
        Property<std::wstring> nickname;
        
        /// path to the data file for this Library
        Property<std::wstring> dataFile;

        /// List of root target directories whose contents are indexed in this Library
        Property<std::vector<std::wstring> > targets;

    };
    /// @class Contains the configuration for the Librarian

    class LibrarianConfig
    {
    public:

        typedef std::map<int,LibraryConfig> LibraryCollection;
        
        /// Serialize this configuration.
        /// @param[in] ar Destination archive for the serialized data.
        /// @param[in] version The version of the archive. Used by boost::serailization version tracking.
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_NVP(dataDir);
            ar & BOOST_SERIALIZATION_NVP(libraries);
        }

        /// The path to the application-specific data directory
        Property<std::wstring> dataDir;

        /// The music libraries known by the system, mapped by ID
        Property<LibraryCollection> libraries;

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
            ar & BOOST_SERIALIZATION_NVP(librarian);
        }
        
        /// The Server configuration for this application
        ServerConfig server;

        /// The Librarian configuration for this application
        LibrarianConfig librarian;

    };


}

#endif