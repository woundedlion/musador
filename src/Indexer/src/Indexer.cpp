#include <time.h>
#include <sstream>
#include <vector>

#include "Indexer.h"
#include "EntityFile.h"
#include "EntityDir.h"
#include "taglib/FileRef.h"
#include "taglib/Tag.h"
#include "Utilities/MIMEResolver.h"
#include "boost/bind.hpp"

#include "Logger/Logger.h"
#define LOG_SENDER L"Indexer"

using namespace Musador;

Indexer::Indexer(std::wstring dbFilename) :
indexThread(NULL),
canceled(false),
dbFilename(dbFilename)
{
	MIMEResolver::init();
}

Indexer::~Indexer()
{
}

void Indexer::clearRootTargets()
{
	LOG(Info) <<  "Clearing " << this->targets.size() <<  " Root Targets";
	Guard targetsGuard(this->targetsMutex);
	this->targets.clear();
}

bool Indexer::addRootTarget(const std::wstring& target)
{
	Guard targetsGuard(this->targetsMutex);
	this->targets.push_back(target);
	
	LOG(Info) <<  "Added Root Target: " << target;

	return true;
}

void Indexer::runIndexer()
{
	LOG(Info) << "Reindexing " << this->targets.size() << " Targets";

	try 
	{
		this->db.reset(new DatabaseSqlite(this->dbFilename));
	}
	catch (DatabaseException e)
	{
		LOG(Critical) << "Error acquiring database " << this->dbFilename << ": " << e.what();
	}

	this->initDB();

	time_t startTime;
	::time(&startTime);

	this->db->txnBegin();

	this->canceled = false;
	for (TargetsIter targIter = this->targets.begin(); targIter != this->targets.end(); targIter++)
	{
		if (canceled)
		{
			break;
		}

		unsigned long parentId = NULL;
		unsigned long newId = Indexer::INVALID_ID;

		LOG(Info) << "Indexing Target: " << targIter->string();
		// Add top-level target directory to database
		fs::wdirectory_entry dir(*targIter);
		if (Indexer::INVALID_ID == (newId = this->addDirectory(dir)))
		{
			LOG(Error) << "Failed to index " << targIter->string() << ". Skipping...";
			continue;
		}
		else
		{
			parentId = newId;
		}

		fs::wrecursive_directory_iterator end_iter;
		for (fs::wrecursive_directory_iterator iter(*targIter); iter != end_iter; iter++)
		{
			if (fs::is_directory(*iter))
			{
				// Add directory to database
				if (Indexer::INVALID_ID == (newId = this->addDirectory(*iter)))
				{
					continue;
				}
				else
				{
					parentId = newId;
				}
			}
			else if (fs::is_regular(*iter))
			{
				if (!MIMEResolver::valid(iter->path().leaf()))
					continue;
				// Add file to database
				if (Indexer::INVALID_ID == this->addFile(*iter,parentId))
					continue;

			}
		}			
	}
	this->db->txnCommit();

	{
		// Finalize progress counts
		Guard lock(this->progressMutex);
		this->p.duration = static_cast<time_t>(::difftime(::time(NULL),startTime));
		this->p.done = true;
	}

	this->db.reset();

	this->sigDone(this->p);
}

void Indexer::reindex()
{
	this->cancel();
	this->waitDone();

	{
		Guard lock(this->progressMutex);
		this->p.numDirs = static_cast<unsigned int>(this->targets.size());
		this->p.numFiles = 0;
		this->p.bytes = 0;
	}

	Guard lock(this->indexThreadMutex);
	this->indexThread = new boost::thread(boost::bind(&Indexer::runIndexer,this));
}

void Indexer::waitDone()
{
	Guard lock(this->indexThreadMutex);
	if (NULL != this->indexThread)
	{
		this->indexThread->join();
		delete this->indexThread;
		this->indexThread = NULL;
	}
}

void Indexer::cancel()
{
	this->canceled = true;
}

unsigned long Indexer::addDirectory(const fs::wdirectory_entry& dir)
{
	EntityDir d(this->db);
	d.path = dir.path().directory_string();
	d.mtime = fs::last_write_time(dir);
	try
	{
		if (!d.save())
		{
			LOG(Error) << "Save failed for directory: " << static_cast<std::wstring>(d.path);
			return Indexer::INVALID_ID;
		}
	}
	catch (DatabaseException e)
	{
		LOG(Error) << "Database Error " << e.what();
		return Indexer::INVALID_ID;
	}

	{
		// Update progress counts
		Guard lock(this->progressMutex);
		++this->p.numDirs;
	}

	return d.getId();
}

unsigned long Indexer::addFile(const fs::wdirectory_entry& file, unsigned long parentId)
{
	EntityFile f(this->db);

	// Gather file info
	f.filename = file.path().leaf();
	f.size = static_cast<long>(fs::file_size(file));
	f.mtime = fs::last_write_time(file);
	f.parentID = parentId;

	// Parse audio metadata
	try {
		TagLib::FileRef fr(Util::unicodeToUtf8(file.path().file_string()).c_str(),true,TagLib::AudioProperties::Average);
		if (!fr.isNull())
		{
			TagLib::Tag * t = fr.tag();
			f.artist = t->artist().to32Bit();
			f.title = t->title().to32Bit();
			f.album = t->album().to32Bit();
			f.track = t->track();
			f.genre = t->genre().to32Bit();
			TagLib::AudioProperties * audio = fr.audioProperties();
			if (audio)
			{
				f.length = audio->length();
				f.bitrate = audio->bitrate();
			}
		}
	} 
	catch (std::runtime_error e) 
	{
		f.status_id = Indexer::ERR_PARSE;
	}

	// Add file to database
	try
	{
		if (!f.save())
		{
			LOG(Error) << "Save failed for file: " << static_cast<std::wstring>(f.filename);
			return Indexer::INVALID_ID;
		}
	}
	catch (DatabaseException e)
	{
		LOG(Error) << "Database Error " << e.what();
		return Indexer::INVALID_ID;
	}

	{
		// Update progress counts
		Guard progressGuard(this->progressMutex);
		++this->p.numFiles;
		this->p.bytes += f.size;
	}

	return f.getId();
}

bool Indexer::initDB()
{
	try
	{
		this->db->execute(L"DROP TABLE IF EXISTS files");
		this->db->execute(L"CREATE TABLE files (\
						   id INTEGER PRIMARY KEY AUTOINCREMENT,\
						   parent_id INTEGER,\
						   filename TEXT,\
						   size INTEGER,\
						   mtime INTEGER,\
						   artist TEXT,\
						   title TEXT,\
						   album TEXT,\
						   track INTEGER,\
						   genre TEXT,\
						   length INTEGER,\
						   bitrate INTEGER,\
						   status_id INTEGER\
						   )");
		this->db->execute(L"DROP TABLE IF EXISTS dirs");
		this->db->execute(L"CREATE TABLE dirs (\
						   id INTEGER PRIMARY KEY AUTOINCREMENT,\
						   path TEXT,\
						   mtime INTEGER\
						   )");
	}
	catch (DatabaseException e)
	{
		LOG(Critical) << "Unable to initialize database tables: Error " << e.what();
		return false;
	}

	LOG(Info) << "Initialized database tables";
	return true;
}

IndexerProgress Indexer::progress() const
{
	Guard progressGuard(this->progressMutex);
	IndexerProgress r(this->p);
	return r;
}

///////////////////////////////////////////////////////////////////////
// IndexerProgress
///////////////////////////////////////////////////////////////////////

IndexerProgress::IndexerProgress() :
numFiles(0),
numDirs(0),
bytes(0),
duration(0),
lastPath(L""),
done(false)
{
}