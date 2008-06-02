#ifndef STATS_BLOCK_H_704610B2_D420_4d67_B986_04D8FF6EBD4E
#define STATS_BLOCK_H_704610B2_D420_4d67_B986_04D8FF6EBD4E

namespace Musador
{
    class StatsBlock
    {
    public:

        typedef std::map<std::wstring,int> StatsCollection;

        /// Serialize this StatsBlock.
        /// @param[in] ar Destination archive for the serialized data.
        /// @param[in] version The version of the archive. Used by boost::serailization version tracking.
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_NVP(id);
            ar & BOOST_SERIALIZATION_NVP(displayName);
            ar & BOOST_SERIALIZATION_NVP(data);
        }

        int id;
        std::wstring displayName;
        StatsCollection data;

    };
}
#endif