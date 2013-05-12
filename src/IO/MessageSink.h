#if 0
#ifndef MESSAGE_SINK_C2446D88_0202_481d_8FCF_49CC1258BB26
#define MESSAGE_SINK_C2446D88_0202_481d_8FCF_49CC1258BB26

#include <map>
#include "IO/Proactor.h"

namespace Musador
{
    namespace IO
    {
        class MessageSink
        {
        public:
   
            MessageSink();

            virtual ~MessageSink();

            void notify(boost::shared_ptr<IO::Msg> msg, boost::any tag = NULL);

            void onNotifyComplete(boost::shared_ptr<IO::MsgNotify> msg, boost::any tag);

            virtual void onNotify(boost::shared_ptr<IO::MsgNotify> msg, boost::any tag) = 0;

        };
    }
}

#endif
#endif
