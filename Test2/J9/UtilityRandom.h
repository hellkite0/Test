// $Id: UtilityRandom.h 2378 2009-04-22 13:34:04Z hyojin.lee $

#ifndef __J9_UTILITY_RANDOM_H__
#define __J9_UTILITY_RANDOM_H__

namespace J9
{
	namespace RandomUtil
	{
		template<typename T>
		void	Shuffle(std::vector<T>& rVector, Random& rGenerator)
		{
			Size	iSize = rVector.size();
			for (Size i = 0; i < iSize; ++i)
			{
				Size	iTarget = rGenerator.GetU32() % iSize;
				if (i == iTarget) continue;
				std::swap(rVector[i], rVector[iTarget]);
			}
		}

		template<typename T>
		void	Shuffle(std::vector<T>& rVector)
		{
			Shuffle(rVector, Random::cGlobal);
		}
	}
}

#endif//__J9_UTILITY_RANDOM_H__