#ifndef CONSOLE_PROGRESS_REPORTER_H_HJKSHK_DSA
#define CONSOLE_PROGRESS_REPORTER_H_HJKSHK_DSA


namespace Musador
{

	class Indexer;
	class IndexerProgress;

	class ConsoleProgressReporter
	{

	public:
		
		ConsoleProgressReporter(const Indexer& indexer);

		void run();

	private:

		const Indexer & indexer;

	};
}


#endif