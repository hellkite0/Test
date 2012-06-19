// $Id$

#ifndef __J9_MEMORY_TRACKER_H__
#define __J9_MEMORY_TRACKER_H__

// �޸� ���� Ž���ÿ�
// ����뿡 ������ �� �� �ֵ��� Ŭ���� ����� ������ �ִ� ���
// �޸� ������ Ž������ �������� �޸𸮿� ������ ������� �ʵ���
// ���ε��� ����
#ifdef DETECT_MEMORY_LEAK
	//#define ENABLE_CLASS_HEADER
#endif

namespace J9
{
	// �޸� ���� üũ�� ������ �ޱ� ���� Ŭ������ �� �տ� ����� �ھƳִ� ��ƿ��Ƽ
	// Ŭ������ �� �տ� SET_CLASS_HEADER('head') �� ���� ����ϸ�
	// ENABLE_CLASS_HEADER �� ���ǵǾ� �ִ� ���
	// Ŭ������ �� �տ� 'head'��� ����� �ھ� �ִ´�.
	// Ŭ�������� �ĺ��� �� �ִ� ����� �����ϰ� �ھƳ�����
	// �޸� ������ ����� �� ����� �����Ͽ� Ŭ���� ������ �Ǵ��� �� �ִ�.
	// ������ ���� 'head'��� ����� ������ ��� ����Ǵ� ����� 'daeh'��� ���̴�
	// ���� ��� ���� BufferŬ������ ��������.
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

	// �޸� ������ Ž���ϱ� ���Ͽ� ���� ���� �����Ǵ� Ŭ����
	// �� Ŭ������ �����Ǹ鼭 �޸� ���� üũ����� �Ҵ�.
	// ����ڴ� �� Ŭ���� ���� ������ �Ű� �� �ʿ䰡 ����
	class MemoryTracker
	{
		STATIC_SINGLETON(MemoryTracker);

		MemoryTracker();
		~MemoryTracker();
	};
}

#endif//__J9_MEMORY_TRACKER_H__