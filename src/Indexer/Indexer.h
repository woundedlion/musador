#ifndef INDEXER_A8965371_DE0E_4ff7_95A0_11B956535E12
#define INDEXER_A8965371_DE0E_4ff7_95A0_11B956535E12

#include <list>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/signal.hpp>
#include <boost/smart_ptr.hpp>

#include "Database/Database.h"

typedef std::list<boost::filesystem::wpath> Targets;
typedef std::list<boost::filesystem::wpath>::iterator TargetsIter;
typedef boost::mutex Mutex;
typedef boost::mutex::scoped_lock Guard;
typedef boost::condition Condition;

namespace fs = boost::filesystem;

namespace Musador
{

	class IndexerProgress
	{
	friend class Indexer;

	public:

		IndexerProgress();
				
		unsigned int numFiles;
		unsigned int numDirs;
		unsigned long long bytes;
		time_t duration;
		std::wstring lastPath;
		bool done;
	};

	class Indexer
	{
	public:

		boost::signal<void (const IndexerProgress& p)> sigDone;

		Indexer(std::wstring dbFilename);
		~Indexer();

		void clearRootTargets();
		bool addRootTarget(const std::wstring& target);
		void reindex();
		void cancel();
		void waitDone();
		void runIndexer();
		IndexerProgress progress() const;

	private:

		static enum {
			ERR_PARSE = 1,
			ERR_DB = 2,
		} IndexerError;

		bool initDB();
		unsigned long addDirectory(const fs::wdirectory_entry& dir);
		unsigned long addFile(const fs::wdirectory_entry& file, unsigned long parentId);

		static const int INVALID_ID = -1;

		std::wstring dbFilename;
		boost::shared_ptr<Database> db;

		Targets targets;
		Mutex targetsMutex;

		mutable Mutex progressMutex;
		IndexerProgress p;

		boost::thread * indexThread;
		Mutex indexThreadMutex;
		bool canceled;
	};
}

#endif