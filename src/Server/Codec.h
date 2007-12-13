#ifndef CODEC_8ED0622B_6E7D_4e64_8E93_9C86C91D309B
#define CODEC_8ED0622B_6E7D_4e64_8E93_9C86C91D309B

namespace Musador
{
	template <typename T>
	class Codec
	{
	public:

		Codec();
		
		~Codec();

//		virtual T& operator<<(boost::shared_array<char> buf, unsigned int len) = 0;

	};
}
#endif