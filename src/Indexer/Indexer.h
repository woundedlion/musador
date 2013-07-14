#pragma once

#include <atomic>
#include <thread>
#include <mutex>
#include <boost/filesystem.hpp>
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
		void indexTarget(const boost::filesystem::path& path,
			storm::sqlite::Transaction& txn);
		storm::sqlite::id_t indexDirectory(const boost::filesystem::directory_entry& dir,
			storm::sqlite::Transaction& txn);
		storm::sqlite::id_t indexFile(const boost::filesystem::directory_entry& file,
			storm::sqlite::id_t dir,
			storm::sqlite::Transaction& txn);
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
