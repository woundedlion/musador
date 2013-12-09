#include <assert.h>
#include <time.h>
#include <sstream>
#include <vector>
#include "boost/assign.hpp"

#include "boost/bind.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "taglib/FileRef.h"
#include "taglib/Tag.h"

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
    clearProgress();
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

	updateProgressDone();
}

void Indexer::indexTarget(const fs::path& path,
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
			(void) indexFile(*iter, currentDir, txn);
		}
	}
}

storm::sqlite::id_t Indexer::indexDirectory(const fs::directory_entry& dir,
							 sqlite::Transaction& txn)
{
	Directory d;
    d.path = dir.path().wstring();
    d.mtime = fs::last_write_time(dir);
	txn << d;

	updateProgress(d);

	return d.id;
}

storm::sqlite::id_t Indexer::indexFile(const fs::directory_entry& file,
						sqlite::id_t dir,
						sqlite::Transaction& txn)
{
	File f;
	f.path = file.path().wstring();
    f.size = fs::file_size(file);
    f.mtime = fs::last_write_time(file);
    f.dir_id = dir;

	try {
		if (parseable(file.path())) {
			parseTags(f);
		}
		txn << f;
	} catch (const std::exception& e) {
		LOG(Error) << "Tag parsing failed: " << e.what();
	}

	updateProgress(f);

	return f.id;
}

bool Indexer::parseable(const fs::path& path)
{
	const std::vector<std::wstring> supportedFileTypes = boost::assign::list_of
		(L".mp3")
		(L".ogg")
		;

	return std::find(
		supportedFileTypes.begin(), 
		supportedFileTypes.end(), 
		path.leaf().extension()) != supportedFileTypes.end();
}

void Indexer::parseTags(File& f)
{
	TagLib::FileRef fr(f.path.c_str());

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
		LOG(Warning) << "Unsupported file type:" << f.path;
	}
}

void Indexer::clearProgress()
{
	std::unique_lock<std::mutex> lock(progressMutex);
	progress = IndexerProgress();
}
void Indexer::updateProgress(const Directory& d)
{
	std::unique_lock<std::mutex> lock(progressMutex);
	++progress.numDirs;
	progress.lastPath = d.path;
}

void Indexer::updateProgress(const File& f)
{
	std::unique_lock<std::mutex> lock(progressMutex);
	++progress.numFiles;
	progress.bytes += f.size;
	progress.lastPath = f.path;
}

void Indexer::updateProgressDone()
{
	std::unique_lock<std::mutex> lock(progressMutex);
	progress.done = true;
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
