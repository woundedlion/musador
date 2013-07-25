#pragma once

#include <atomic>
#include <thread>
#include <mutex>
#include <boost/filesystem.hpp>
#include <boost/serialization/nvp.hpp>
#include "Database/DatabaseSqlite.h"
#include "Entities.h"

namespace fs = boost::filesystem;

namespace Musador
{

    class IndexerProgress
    {
    public:

        IndexerProgress();

        unsigned int numFiles;
        unsigned int numDirs;
        unsigned long long bytes;
        clock_t startTime;
        clock_t curTime;
        std::wstring lastPath;
        bool done;
        bool canceled;

		template <typename Archive>
		void serialize(Archive& ar, unsigned int)
		{
			ar & BOOST_SERIALIZATION_NVP(numFiles);
			ar & BOOST_SERIALIZATION_NVP(numDirs);
			ar & BOOST_SERIALIZATION_NVP(bytes);
			ar & BOOST_SERIALIZATION_NVP(startTime);
			ar & BOOST_SERIALIZATION_NVP(curTime);
			ar & BOOST_SERIALIZATION_NVP(lastPath);
			ar & BOOST_SERIALIZATION_NVP(done);
			ar & BOOST_SERIALIZATION_NVP(canceled);
		}
    };

    class Indexer
    {
    public:

        Indexer(const std::wstring& dbName);
        ~Indexer();

        void clearTargets();
        void addTarget(const std::wstring& target);
        void reindex();
        void cancel();
        IndexerProgress getProgress() const;
		bool isRunning();

    private:

		void run();
		void initDB();
		void indexTargets();
		void indexTarget(const fs::path& path,
			storm::sqlite::Transaction& txn);
		storm::sqlite::id_t indexDirectory(const fs::directory_entry& dir, storm::sqlite::Transaction& txn);
		storm::sqlite::id_t indexFile(const fs::directory_entry& file,
			storm::sqlite::id_t dir,
			storm::sqlite::Transaction& txn);
		bool parseable(const fs::path& path);
		void parseTags(File& f);
		void setProgress(const IndexerProgress& p);

		bool canceled;
		bool running;
		std::wstring dbName;
		std::vector<boost::filesystem::path> targets;
		mutable std::mutex progressMutex;
		IndexerProgress progress;
    };
}
