#ifndef STATS_BLOCK_H_704610B2_D420_4d67_B986_04D8FF6EBD4E
#define STATS_BLOCK_H_704610B2_D420_4d67_B986_04D8FF6EBD4E

#include <map>
namespace Musador
{

    /// @class StatsBlock
    /// @brief A serializable class containing arbitrary stats name-value pairs
    class StatsBlock
    {
    public:

        /// @brief Collection type for stats data
        typedef std::map<std::wstring,int> StatsCollection;

        /// Serialize this StatsBlock.
        /// @param[in] ar Destination archive for the serialized data.
        /// @param[in] version The version of the archive. Used by boost::serialization version tracking.
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_NVP(id);
            ar & BOOST_SERIALIZATION_NVP(displayName);
            ar & BOOST_SERIALIZATION_NVP(data);
        }

        /// @brief The numeric id of this StatsBlock.
        int id;

        /// @brief The friendly name for this stats block.
        std::wstring displayName;

        /// @brief The stats data contained within this block.
        /// This member consists of name-value pairs representing individual stats.
        StatsCollection data;

    };
}
#endif