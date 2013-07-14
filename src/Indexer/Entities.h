#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace Musador
{
	class Directory
	{
	public:

		Directory() :
			id(0),
			mtime(0)
		{}

		template <class Archive>
		void serialize(Archive& ar, unsigned int)
		{
			using namespace boost::serialization;
			ar * BOOST_SERIALIZATION_NVP(id);
			ar & BOOST_SERIALIZATION_NVP(path);
			ar & BOOST_SERIALIZATION_NVP(mtime);
		}

		static const wchar_t *table() { return L"dirs"; }

		int64_t id;
		std::wstring path;
		time_t mtime;
	};

	class File
	{
	public:

		File() :
			id(0),
			dir_id(0),
			mtime(0),
			size(0),
			track(0),
			length(0),
			bitrate(0)
		{}

		template <class Archive>
		void serialize(Archive& ar, unsigned int)
		{
			using namespace boost::serialization;
			ar * BOOST_SERIALIZATION_NVP(id);
			ar & BOOST_SERIALIZATION_NVP(dir_id);
			ar & BOOST_SERIALIZATION_NVP(path);
			ar & BOOST_SERIALIZATION_NVP(mtime);
			ar & BOOST_SERIALIZATION_NVP(size);
			ar & BOOST_SERIALIZATION_NVP(artist);
			ar & BOOST_SERIALIZATION_NVP(album);
			ar & BOOST_SERIALIZATION_NVP(title);
			ar & BOOST_SERIALIZATION_NVP(genre);
			ar & BOOST_SERIALIZATION_NVP(track);
			ar & BOOST_SERIALIZATION_NVP(length);
			ar & BOOST_SERIALIZATION_NVP(bitrate);
		}

		static const wchar_t *table() { return L"files"; }

		int64_t id;
		int64_t dir_id;
		std::wstring path;
        time_t mtime;
        size_t size;
		std::wstring artist;
        std::wstring album;
		std::wstring title;
		std::wstring genre;
		uint32_t track;
		int length;
		int bitrate;
	};

}
