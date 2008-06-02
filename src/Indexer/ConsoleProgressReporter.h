#ifndef CONSOLE_PROGRESS_REPORTER_H_HJKSHK_DSA
#define CONSOLE_PROGRESS_REPORTER_H_HJKSHK_DSA

namespace Musador
{
    class Indexer;
    class IndexerProgress;

    /// @class ConsoleProgressReporter
    /// @brief Monitors an Indexer and displays progress to the Console.
    class ConsoleProgressReporter
    {

    public:

        /// @brief Constructor
        /// @param[in] indexer The Indexer whose progress should be monitored.
        ConsoleProgressReporter(const Indexer& indexer);

        /// @brief Execute the main loop of the ConsoleProgressReporter.
        /// @remarks Will not return until the Indexer is done.
        void run();

    private:

        const Indexer & indexer;

    };
}


#endif