#ifndef GUI_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define GUI_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include "IO/PipeListener.h"

namespace Musador
{
    /// @class GUIListener
    /// @brief A concrete PipeListener which yields a GUIConnection.
    class GUIListener : public PipeListener
    {
    public:

        /// @brief Constructor.
        GUIListener();

        /// @brief Factory method which creates a new GUIConnection.
        boost::shared_ptr<Connection> createConnection();

        /// @brief Close the listener and free any associated resources.
        void close() {}

    };
}

#endif