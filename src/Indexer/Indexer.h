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

    /// @class IndexerProgress
    /// @brief Container for statistics on the current progress of an Indexer.
    class IndexerProgress
    {
        friend class Indexer;

    public:

        /// @brief Constructor.
        IndexerProgress();

        /// @brief The number of files which have so far been indexed.
        unsigned int numFiles;

        /// @brief The number of directories which have so far been indexed.
        unsigned int numDirs;

        /// @brief The total size in bytes if all the files which have so far been indexed.
        unsigned long long bytes;

        /// @brief The start time of the indexing job.
        clock_t startTime;

        /// @brief The current time of the indexing job.
        clock_t curTime;

        /// @brief The last full path processed by the Indexer.
        std::wstring lastPath;

        /// @brief True if the index is complete, false otherwise.
        bool done;
    };

    /// @class Indexer
    /// @brief Recursively processes a directory tree, indexing all directories and files contained therein.
    /// When run, the Indexer creates a database file and populates it with the collected data.
    /// In addition to file system data, ID3 meta-data are also parsed and added to the Database.
    class Indexer
    {
    public:

        /// @brief Constructor.
        /// @param[in] dbFilename The path to the output database file for indices create by this Indexer.
        Indexer(std::wstring dbFilename);

        /// @brief Destructor.
        ~Indexer();

        /// @brief Clear the collection of paths targetted byt his Indexer.
        void clearRootTargets();

        /// @brief Add a target diretory to this Indexer.
        /// @param[in] target The path to a directory to be recursively indexed by this Indexer.
        /// @returns true if the path was added, false otherwise
        bool addRootTarget(const std::wstring& target);

        /// @brief Asynchronously regenerate the full index of this Indexer's targets.
        /// @remarks Spawns a thread to do the actual indexing and returns immediately.
        void reindex();

        /// @brief Cancel the current indexing job, if one is in progess.
        void cancel();

        /// @brief Wait until the current indexing job is done.
        void waitDone();

        /// @brief Synchronously run an indexing job.
        /// @remarks Returns when the indexing job is done.
        void runIndexer();

        /// Get the progress stats for th currently executing indexing job.
        IndexerProgress progress() const;

        /// boost::signal used by interested parties to connect and receive notification whenever an indexing job completes.
        boost::signal<void (const IndexerProgress& p)> sigDone;

    private:

        static enum {
            ERR_PARSE = 1,
            ERR_DB = 2,
        } IndexerError;

        bool initDB();
        bool indexDB();
        unsigned long addDirectory(const fs::wdirectory_entry& dir);
        unsigned long addFile(const fs::wdirectory_entry& file, unsigned long parentId);

        static const int INVALID_ID = -1;

        std::wstring dbFilename;
        boost::shared_ptr<Database::IDatabase> db;

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