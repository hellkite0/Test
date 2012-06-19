// $Id$

#ifndef __J9_VERSION_H__
#define __J9_VERSION_H__

namespace J9
{
	class Version
	{
	public:
		Version();
		Version(const Version& r);

		bool operator==(const Version& r) const
		{
			for (Size i = 0; i < 4; ++i)
			{
				if (Comp(i_[i], r.i_[i]) != 0)
					return false;
			}

			return true;
		}

		bool operator!=(const Version& r) const
		{
			return !(*this == r);
		}

		bool operator<(const Version& r) const
		{
			for (Size i = 0; i < 4; ++i)
			{
				S32 iResult = Comp(i_[i], r.i_[i]);
				if (iResult < 0)
					return true;
				else if (iResult > 0)
					return false;
			}

			return false;
		}

		J9::StringW		GetVersion() const;
		U16				GetVersion(U8 i) const { return i_[i]; }
		U16				GetMajorVersion() const { return i_[0]; }
		U16				GetMinorVersion() const { return i_[1]; }
		U16				GetProductVersion() const { return i_[2]; }
		U16				GetBulidVersion() const { return i_[3]; }

		void			SetVersion(const J9::StringW& rVersion);
		void			SetVersion(U8 iIndex, U16 i) { i_[iIndex] = i; }
		void			SetMajorVersion(U16 i) { i_[0] = i; }
		void			SetMinorVersion(U16 i) { i_[1] = i; }
		void			SetProductVersion(U16 i) { i_[2] = i; }
		void			SetBulidVersion(U16 i) { i_[3] = i; }

	private:
		S32				Comp(U16 ilhs, U16 irhs) const
		{
			if (ilhs == irhs) return 0;
			else if (ilhs > irhs) return 1;
			else return -1;
		}

	private:
		U16		i_[4];
	};
}

#endif//__J9_VERSION_H__
