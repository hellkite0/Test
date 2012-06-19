// $Id$
#ifndef __OBJECT_H__
#define __OBJECT_H__

namespace J9
{
#ifdef _SERVER
	class RecycleObject;
	typedef J9::Ptr< J9::Que< J9::Ptr< J9::RecycleObject> > > RecycledObjectQue;

	#define DECL_RECYCLEOBJECT(sTypeName) \
	friend class J9::Ptr<sTypeName>; \
	public: static J9::Ptr<##sTypeName##> Create() { \
	J9::RecycledObjectQue pQue = J9::RecycleObject::cRecycleBins.Get(_W(#sTypeName)); \
	J9::Ptr<sTypeName> pRet; \
	if (pQue && (pRet = pQue->Pop(), pRet)) { /*LOG(_W("recycled %s\r\n"), _W(#sTypeName));*/ pRet->Initialize(); return pRet; } \
	pRet = J9::Ptr<sTypeName>(ALLOC); /*LOG(_W("created %s\r\n"), _W(#sTypeName));*/\
	pRet->Initialize(); \
	return pRet; } \
	virtual void Destroy() { \
	J9::RecycledObjectQue pQue = J9::RecycleObject::cRecycleBins.Get(_W(#sTypeName)); \
	if (!pQue) { pQue.New(); J9::RecycleObject::cRecycleBins.Put(_W(#sTypeName), pQue); } \
	this->Finalize(); \
	pQue->Push(this); } \
	protected: sTypeName() { Dummy(); Initialize(); };

	class RecycleObject
	{
		friend class J9::Ptr<RecycleObject>;
	public:
		virtual void Destroy() = 0;
		virtual void Initialize() = 0;
		virtual void Finalize() = 0;
		virtual void Dummy() {};

	protected:
		RecycleObject() {}
		virtual ~RecycleObject() { }

	public:
		static J9::NamedPool<J9::RecycledObjectQue> cRecycleBins;
	};

#endif
};

#endif // __OBJECT_H__