#ifndef GUIMESSAGES_06E5C6BBB53C_4038_A5DC_BE634E8E0430
#define GUIMESSAGES_06E5C6BBB53C_4038_A5DC_BE634E8E0430

#include <boost/serialization/serialization.hpp>

namespace Musador
{

    /// @brief Types of the various messages which compose the Librarian Protocol.
    enum GUIMsgType
    {
        GUI_MSG_ENABLE_REQ,
        GUI_MSG_DISABLE_REQ,
        GUI_MSG_ENABLED_NOTIFY,
        GUI_MSG_DISABLED_NOTIFY
    };

    /// @class GUIMsg
    /// @brief Base class for all Librarian Protocol messages.
    class GUIMsg
    {
        friend class boost::serialization::access;

    public:

        /// @brief Default Constructor. 
        /// Creates a message with no type.
        GUIMsg() {};

        /// @brief Constructor.
        /// @param[in] type A member of the GUIMsgType enumeration indicating the type of message to create.
        GUIMsg(GUIMsgType type) : type(type) {}

        /// @brief Get the type of this message.
        /// @returns A member of the GUIMsgType enumeration indicating the type of the message.
        inline GUIMsgType getType() { return type; }

    private: 

        template<class A>
        void serialize(A & ar, const unsigned int version)
        {
            ar & type;
        }

        GUIMsgType type;    
    };

    /// @class GUIMsgEnableReq
    /// @brief Message requesting that the Librarian Service be enabled.
    /// @note Message flow: LibrarianGUI ---> Librarian
    class GUIMsgEnableReq : public GUIMsg
    {
    public:

        /// @brief Constructor.
        GUIMsgEnableReq() : GUIMsg(GUI_MSG_ENABLE_REQ) {}
    };

    /// @class GUIMsgDisableReq
    /// @brief Message requesting that the Librarian Service be disabled. 
    /// @note Message flow: LibrarianGUI ---> Librarian
    class GUIMsgDisableReq : public GUIMsg
    {
    public:

        /// @brief Constructor.
        GUIMsgDisableReq() : GUIMsg(GUI_MSG_DISABLE_REQ) {}
    };

    /// @class GUIMsgEnabledNotify
    /// @brief Message indicating that the LibrarianService has been enabled. 
    /// @note Message flow: Librarian ---> LibrarianGUI
    class GUIMsgEnabledNotify : public GUIMsg
    {
    public:

        /// @brief Constructor.
        GUIMsgEnabledNotify() : GUIMsg(GUI_MSG_ENABLED_NOTIFY) {}
    };

    /// @class GUIMsgDisabledNotify
    /// @brief Message indicating that the LibrarianService has been disabled. 
    /// @note Message flow: Librarian ---> LibrarianGUI
    class GUIMsgDisabledNotify : public GUIMsg
    {
    public:

        /// @brief Constructor.
        GUIMsgDisabledNotify() : GUIMsg(GUI_MSG_DISABLED_NOTIFY) {}
    };

    /// @brief Functor type for callbacks to handle a GUIMsg when it is received from the pipe.
    typedef boost::function1<void, boost::shared_ptr<GUIMsg> > GUIHandler;

}

#endif