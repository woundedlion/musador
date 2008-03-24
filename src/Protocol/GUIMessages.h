#ifndef GUIMESSAGES_06E5C6BBB53C_4038_A5DC_BE634E8E0430
#define GUIMESSAGES_06E5C6BBB53C_4038_A5DC_BE634E8E0430

#include <boost/serialization/serialization.hpp>

namespace Musador
{
    enum GUIMsgType
    {
        GUI_MSG_ENABLE_REQ,
        GUI_MSG_DISABLE_REQ,
        GUI_MSG_ENABLED_NOTIFY,
        GUI_MSG_DISABLED_NOTIFY
    };

    class GUIMsg
    {
		friend class boost::serialization::access;

	public:

		GUIMsg() {};

        GUIMsg(GUIMsgType type) : type(type) {}

        inline GUIMsgType getType() { return type; }

	private: 

		template<class A>
		void serialize(A & ar, const unsigned int version)
		{
			ar & type;
		}

        GUIMsgType type;    
    };


    class GUIMsgEnableReq : public GUIMsg
    {
    public:
		GUIMsgEnableReq() : GUIMsg(GUI_MSG_ENABLE_REQ) {}
	};

    class GUIMsgDisableReq : public GUIMsg
    {
    public:
        GUIMsgDisableReq() : GUIMsg(GUI_MSG_DISABLE_REQ) {}
    };

    class GUIMsgEnabledNotify : public GUIMsg
    {
    public:
        GUIMsgEnabledNotify() : GUIMsg(GUI_MSG_ENABLED_NOTIFY) {}
    };

    class GUIMsgDisabledNotify : public GUIMsg
    {
    public:
        GUIMsgDisabledNotify() : GUIMsg(GUI_MSG_DISABLED_NOTIFY) {}
    };

	typedef boost::function1<void, boost::shared_ptr<GUIMsg> > GUIHandler;

}

#endif