#include <assert.h>
#include <time.h>
#include <sstream>
#include <vector>

#include "boost/bind.hpp"

#define WIN32_LEAN_AND_MEAN
#include "taglib/FileRef.h"
#include "taglib/Tag.h"

#include "Utilities/MIMEResolver.h"
#include "Logger/Logger.h"
#include "IO/Proactor.h"
#include "Indexer.h"

#define LOG_SENDER L"Indexer"

using namespace Musador;
using namespace storm;

Indexer::Indexer(const std::wstring& dbName) :
canceled(false),
running(false),
dbName(dbName)
{
}

Indexer::~Indexer()
{
	assert(!isRunning());
}

void Indexer::clearTargets()
{
    LOG(Info) <<  "Clearing " << targets.size() <<  " Root Targets";
    targets.clear();
}

void Indexer::addTarget(const std::wstring& target)
{
    targets.push_back(target);
    LOG(Info) <<  "Added Root Target: " << target;
}

bool
Indexer::isRunning()
{
	return running;
}

void Indexer::reindex()
{
    if (isRunning()) {
        LOG(Error) << "Indexer is already running.";
        return;
    }

	running = true;
	canceled = false;
    setProgress(IndexerProgress());
	IO::Proactor::instance()->beginInvoke(boost::bind(&Indexer::run, this));
}

void Indexer::cancel()
{
    this->canceled = true;
}

IndexerProgress Indexer::getProgress() const
{
    std::unique_lock<std::mutex> lock(progressMutex);
    IndexerProgress r(progress);
    r.curTime = std::clock();
    return r;
}

void Indexer::run()
{
    LOG(Info) << "Reindexing " << this->targets.size() << " Targets";

	try {
		initDB();
		indexTargets();
		running = false;
	} catch (const std::exception& e) {
		LOG(Error) << "Index failed: " << e.what();
		running = false;
		std::unique_lock<std::mutex> lock(progressMutex);
		progress.done = true;
	}
}

void Indexer::initDB()
{
	sqlite::Database db(dbName);
	sqlite::Transaction txn(db);
	txn << sql::drop<Directory>()
		<< sql::drop<File>()
		<< sql::create<Directory>()
		<< sql::create<File>();
	txn.commit();
}

void Indexer::indexTargets()
{
	sqlite::Database db(dbName);
	for (auto target : targets) {
		sqlite::Transaction txn(db);
		indexTarget(target, txn);
		txn.commit();
	}

	std::unique_lock<std::mutex> lock(progressMutex);
	progress.done = true;
}

void Indexer::indexTarget(const boost::filesystem::path& path,
						  sqlite::Transaction& txn)
{
	LOG(Info) << "Indexing Target: " << path.string();
	fs::recursive_directory_iterator iter(path);
	fs::recursive_directory_iterator endIter;
	storm::sqlite::id_t currentDir = 0;
	for (; iter != endIter; ++iter) {
		if (canceled) {
			break;
		}
		if (fs::is_directory(*iter)) {
			currentDir = indexDirectory(*iter, txn);
		}
		else if (fs::is_regular(*iter)) {
			if (Util::MIMEResolver::valid(iter->path().leaf().wstring())) {
				(void) indexFile(*iter, currentDir, txn);
			}
		}
	}
}

storm::sqlite::id_t Indexer::indexDirectory(const boost::filesystem::directory_entry& dir,
							 sqlite::Transaction& txn)
{
	Directory d;
    d.path = dir.path().wstring();
    d.mtime = fs::last_write_time(dir);
	txn << d;

	std::unique_lock<std::mutex> lock(progressMutex);
	++progress.numDirs;
	progress.lastPath = d.path;

	return d.id;
}

storm::sqlite::id_t Indexer::indexFile(const boost::filesystem::directory_entry& file,
						sqlite::id_t dir,
						sqlite::Transaction& txn)
{
	File f;
	f.path = file.path().wstring();
    f.size = fs::file_size(file);
    f.mtime = fs::last_write_time(file);
    f.dir_id = dir;

	try {
		parseTags(f);
		txn << f;
	} catch (const std::exception& e) {
		LOG(Warning) << "Tag parsing failed: " << e.what();
	}

	std::unique_lock<std::mutex> lock(progressMutex);
	++progress.numFiles;
	progress.lastPath = f.path;

	return f.id;
}

void Indexer::parseTags(File& f)
{
	TagLib::FileRef fr(Util::unicodeToUtf8(f.path.c_str()).c_str());

	if (!fr.isNull()) {
		TagLib::Tag *t = fr.tag();
		f.artist = t->artist().toWString();
		f.title = t->title().toWString();
		f.album = t->album().toWString();
		f.track = t->track();
		f.genre = t->genre().toWString();
		TagLib::AudioProperties * audio = fr.audioProperties();

		if (audio) {
			f.length = audio->length();
			f.bitrate = audio->bitrate();
		}
	} else {
		LOG(Warning) << "Failed to open " << f.path;
	}
}


void Indexer::setProgress(const IndexerProgress& p)
{
	std::unique_lock<std::mutex> lock(progressMutex);
	progress = p;
}

///////////////////////////////////////////////////////////////////////
// IndexerProgress
///////////////////////////////////////////////////////////////////////

IndexerProgress::IndexerProgress() :
numFiles(0),
numDirs(0),
bytes(0),
startTime(0),
curTime(0),
lastPath(L""),
done(false),
canceled(false)
{
}
