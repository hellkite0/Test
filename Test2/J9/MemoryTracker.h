// $Id$

#ifndef __J9_MEMORY_TRACKER_H__
#define __J9_MEMORY_TRACKER_H__

// 메모리 누수 탐지시에
// 디버깅에 도움이 될 수 있도록 클래스 헤더를 설정해 넣는 기능
// 메모리 누수를 탐지하지 않을때는 메모리와 성능이 낭비되지 않도록
// 꺼두도록 하자
#ifdef DETECT_MEMORY_LEAK
	//#define ENABLE_CLASS_HEADER
#endif

namespace J9
{
	// 메모리 누수 체크에 도움을 받기 위해 클래스의 맨 앞에 헤더를 박아넣는 유틸리티
	// 클래스의 맨 앞에 SET_CLASS_HEADER('head') 와 같이 사용하면
	// ENABLE_CLASS_HEADER 가 정의되어 있는 경우
	// 클래스의 맨 앞에 'head'라고 헤더를 박아 넣는다.
	// 클래스마다 식별할 수 있는 헤더를 정의하고 박아넣으면
	// 메모리 누수가 보고될 때 헤더를 참고하여 클래스 종류를 판단할 수 있다.
	// 주의할 점은 'head'라고 헤더를 선언할 경우 보고되는 헤더는 'daeh'라는 것이다
	// 실제 사용 예는 Buffer클래스를 참고하자.
#ifdef ENABLE_CLASS_HEADER
	template<int T>
	class ClassHeader
	{
	public:
		ClassHeader() : iHeader_(Int2Type<T>::Value) {}
		~ClassHeader() {}

	private:
		int	iHeader_;
	};

	#define SET_CLASS_HEADER(x)		J9::ClassHeader<x>	cClassHeader_;
#else
	#define SET_CLASS_HEADER(x)
#endif

	// 메모리 누수를 탐지하기 위하여 가장 먼저 생성되는 클래스
	// 이 클래스가 생성되면서 메모리 누수 체크기능을 켠다.
	// 사용자는 이 클래스 생성 삭제에 신경 쓸 필요가 없다
	class MemoryTracker
	{
		STATIC_SINGLETON(MemoryTracker);

		MemoryTracker();
		~MemoryTracker();
	};
}

#endif//__J9_MEMORY_TRACKER_H__