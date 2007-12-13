#include <time.h>
#include <sstream>
#include <vector>

#include "Indexer.h"
#include "EntityFile.h"
#include "EntityDir.h"
#include "taglib/FileRef.h"
#include "taglib/Tag.h"
#include "Utilities/MIMEResolver.h"

#include "Logger/Logger.h"
#define LOG(LVL) LoggerConsole::instance()->log(LVL,"Indexer")

using namespace Musador;

Indexer::Indexer(std::wstring databaseName) :
done(false)
{
	MIMEResolver::init();
	try {
		this->db.reset(new DatabaseSqlite(databaseName));
	}
	catch (DatabaseException e)
	{
		LOG(Critical) << "Error acquiring database " << databaseName << ": " << e.what();
	}
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

bool Indexer::reindex()
{
	LOG(Info) << "Reindexing " << this->targets.size() << " Targets";

	this->done = false;

	time_t startTime;
	::time(&startTime);

	{
		Guard progressGuard(this->progressMutex);
		this->p.setNumDirs(static_cast<unsigned int>(this->targets.size()));
		this->p.setNumFiles(0);
		this->p.setBytes(0);
	}

	this->db->txnBegin();

	for (TargetsIter targIter = this->targets.begin(); targIter != this->targets.end(); targIter++)
	{
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
		Guard progressGuard(this->progressMutex);
		this->p.setDuration(static_cast<time_t>(::difftime(::time(NULL),startTime)));
		this->done = true;
	}

	this->sigDone(this->p);
	return true;
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
			LOG(Error) << "Save failed for directory: " << d.path;
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
		this->p.setNumDirs(this->p.numDirs() + 1);
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
			LOG(Error) << "Save failed for file: " << f.filename;
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
		this->p.setNumFiles(this->p.numFiles() + 1);
		this->p.setBytes(this->p.bytes() + f.size);
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

bool Indexer::progress(IndexerProgress * p) const
{
	Guard progressGuard(this->progressMutex);
	*p = this->p;
	return !this->done;
}

///////////////////////////////////////////////////////////////////////
// IndexerProgress
///////////////////////////////////////////////////////////////////////

IndexerProgress::IndexerProgress() :
m_numFiles(0),
m_numDirs(0),
m_bytes(0),
m_duration(0),
m_lastPath(L"")
{
}

IndexerProgress::IndexerProgress(const IndexerProgress& p) :
m_numFiles(p.m_numFiles),
m_numDirs(p.m_numDirs),
m_bytes(p.m_bytes),
m_duration(p.m_duration),
m_lastPath(p.m_lastPath)
{
}

unsigned int IndexerProgress::numFiles() const
{
	return this->m_numFiles;
}

unsigned int IndexerProgress::numDirs() const
{
	return this->m_numDirs;
}

long long IndexerProgress::bytes() const
{
	return this->m_bytes;
}

time_t IndexerProgress::duration() const
{
	return this->m_duration;
}

std::wstring IndexerProgress::lastPath() const
{
	return this->m_lastPath;
}

void IndexerProgress::setNumFiles(unsigned int v)
{
	this->m_numFiles = v;
}

void IndexerProgress::setNumDirs(unsigned int v)
{
	this->m_numDirs = v;
}

void IndexerProgress::setBytes(long long v)
{
	this->m_bytes = v;
}

void IndexerProgress::setDuration(time_t v)
{
	this->m_duration = v;
}

void IndexerProgress::setLastPath(const std::wstring& v)
{
	this->m_lastPath = v;
}
