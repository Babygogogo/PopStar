#ifndef __PROCESS_INVOKER__
#define __PROCESS_INVOKER__

#include <memory>
#include <chrono>

class BaseProcess;

/*!
 * \class ProcessRunner
 *
 * \brief Manages the processes attached here and runs them.
 *
 * \details
 *	The runner doesn't run the processes automatically.
 *	You must call updateAllProcesses() (probably on every game loop) to run the processes.
 *
 * \author Babygogogo
 * \date 2015.7
 */
class ProcessRunner
{
public:
	~ProcessRunner();

	//Create an instance.
	static std::unique_ptr<ProcessRunner> create();

	//Attach a new process.
	void attachProcess(std::unique_ptr<BaseProcess> && process);

	//Call update() on all processes attached with the eclapsed time.
	void updateAllProcess(const std::chrono::milliseconds & deltaTimeMs);

	//Abort all processes attached and detach them.
	void abortAllProcess();

private:
	ProcessRunner();

	//Implementation stuff.
	struct ProcessRunnerImpl;
	std::unique_ptr<ProcessRunnerImpl> pimpl;
};

#endif // !__PROCESS_INVOKER__
