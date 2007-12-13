#ifndef INDEXER_A8965371_DE0E_4ff7_95A0_11B956535E12
#define INDEXER_A8965371_DE0E_4ff7_95A0_11B956535E12

#include <list>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/signal.hpp>
#include <boost/smart_ptr.hpp>

#include "Database/Database.h"

typedef std::list<boost::filesystem::wpath> Targets;
typedef std::list<boost::filesystem::wpath>::iterator TargetsIter;
typedef boost::mutex Mutex;
typedef boost::mutex::scoped_lock Guard;
namespace fs = boost::filesystem;

namespace Musador
{


	class IndexerProgress
	{
	friend class Indexer;

	public:

		IndexerProgress();
		IndexerProgress(const IndexerProgress& p);
		
		unsigned int numFiles() const;
		unsigned int numDirs() const;
		long long bytes() const;
		time_t duration() const;
		std::wstring lastPath() const;

		void setNumFiles(unsigned int v);
		void setNumDirs(unsigned int v);
		void setBytes(long long v);
		void setDuration(time_t v);
		void setLastPath(const std::wstring& v);

	private:
		
		unsigned int m_numFiles;
		unsigned int m_numDirs;
		unsigned long long m_bytes;
		time_t m_duration;
		std::wstring m_lastPath;
	};

	class Indexer
	{
	public:

		boost::signal<void (const IndexerProgress& p)> sigDone;

		Indexer(std::wstring databaseName);
		~Indexer();

		void clearRootTargets();
		bool addRootTarget(const std::wstring& target);
		bool reindex();
		bool initDB();
		bool progress(IndexerProgress * p) const;

	private:

		static const int INVALID_ID = -1;
		static enum {
			ERR_PARSE = 1,
			ERR_DB = 2,
		} IndexerError;

		boost::shared_ptr<Database> db;
		bool done;
		Targets targets;
		Mutex targetsMutex;
		IndexerProgress p;
		mutable Mutex progressMutex;

		unsigned long addDirectory(const fs::wdirectory_entry& dir);
		unsigned long addFile(const fs::wdirectory_entry& file, unsigned long parentId);

	};
}

#endif