#ifndef MESSAGE_SINK_C2446D88_0202_481d_8FCF_49CC1258BB26
#define MESSAGE_SINK_C2446D88_0202_481d_8FCF_49CC1258BB26

#include "boost/bind.hpp"
#include "IO/IOMessages.h"

namespace Musador
{
    namespace IO
    {
        template <typename T>
        class MessageSink
        {
        public:
   
            virtual void post(boost::shared_ptr<IO::Msg> msg, boost::any tag = NULL);

            virtual void onNotify(boost::shared_ptr<IO::MsgNotify, boost::any tag) = 0;
        };

        void MessageSink<T>::post(boost::shared_ptr<IO::MsgNotify> msg, boost::any tag /* = NULL */)
        {
            IO::Proactor::instance()->beginNotify(boost::bind(&MessageSink::onNotify, this, _1, _2), msg, tag);
        }
    }
}

#endif

