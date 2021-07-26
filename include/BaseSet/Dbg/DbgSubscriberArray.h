
#ifndef B_SETCDBGSUBSCRIBERARRAY_H
#define B_SETCDBGSUBSCRIBERARRAY_H

#include "../BaseObj/BaseObject.h"
#include "../BaseObj/Subscription.h"

namespace b_set {

class CDbgSubscriber : virtual public CBaseObj
{
public:
	virtual void OnDbgMessage( LPCTSTR lpObjName, DWORD dwMsgType, LPCTSTR lpText ) = 0;
};

//!/////////////////////////////////////////////////////////////////

class CDbgSubscriberArray : public CSubscriberArray<CDbgSubscriber>
{
public:
CDbgSubscriberArray(LPCTSTR lpObjName = NULL){
	if (lpObjName)
		SetObjectName(lpObjName);
};
virtual ~CDbgSubscriberArray(){};

};

//CSubscriberArray<CDbgSubscriber> g_arrDbgSubscribers(_T("DbgSubscribers"));

}

#endif //! B_SETCDBGSUBSCRIBERARRAY_H
