#ifndef GUI_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD
#define GUI_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD

#include "IO/PipeConnection.h"
#include "GUIMessages.h"

namespace Musador
{
    /// @brief The filename of the named pipe used for Librarain<->LibrarianGUI communication.
    static const wchar_t * GUI_PIPE_NAME = L"\\\\.\\pipe\\LibrarianGUI";

    /// @class GUIConnection
    /// @brief A concrete PipeConnection implementing the protocol for communications between 
    /// the Librarian service and the LibrarianGUI process.
    class GUIConnection : public PipeConnection
    {

    public:

        /// @brief Constructor.
        GUIConnection();

        /// @brief Handler invoked when an asynchronous read operation completes.
        /// @param[in] msg A message indicating the result of the asynchronous read operation.
        /// @param[in] tag User-defined value passed in when the original asynchronous read operation was initiated.
        void onReadComplete(boost::shared_ptr<IOMsg> msg, boost::any tag = NULL);

        /// @brief Handler invoked when an asynchronous write operation completes.
        /// @param[in] msg A message indicating the result of the asynchronous write operation.
        /// @param[in] tag User-defined value passed in when the original asynchronous write operation was initiated.
        void onWriteComplete(boost::shared_ptr<IOMsg> msg, boost::any tag = NULL);

        /// @brief Handler invoked when an asynchronous accept operation completes.
        /// @param[in] msg A message indicating the result of the asynchronous accept operation.
        /// @param[in] tag User-defined value passed in when the original asynchronous accept operation was initiated.
        void onAcceptComplete(boost::shared_ptr<IOMsg> msg, boost::any tag = NULL); 

        /// @brief Handler invoked when an asynchronous connect operation completes.
        /// @param[in] msg A message indicating the result of the asynchronous connect operation.
        /// @param[in] tag User-defined value passed in when the original asynchronous connect operation was initiated.
        void onConnectComplete(boost::shared_ptr<IOMsg>, boost::any tag = NULL);

        /// @brief Register a function to process GUI Messages received over the connection.
        /// @param[in] handler A GUIHandler fucntion which will be invoked whenever a GUIMsg is received over the connection.
        void setHandler(const GUIHandler& handler);

    private:

        GUIHandler handler;

        int connRetries;
    };

}

#endif