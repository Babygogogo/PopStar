#include <cassert>

#include "BaseProcess.h"
#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//Enum of possible process states.
//////////////////////////////////////////////////////////////////////////
enum class ProcessState{
	UnRun,			//Created but not run
	Removed,		//Registered to IProcessExecutor, then unregistered before finishing running

	Running,		//Running
	Paused,			//Paused before finishing running

	Succeeded,		//Ended in success
	Failed,			//Ended in failure
	Aborted			//Ended because of internal issue; may never run at all!
};

//////////////////////////////////////////////////////////////////////////
//Definition of BaseProcess::BaseProcessImpl.
//////////////////////////////////////////////////////////////////////////
struct BaseProcess::BaseProcessImpl
{
public:
	BaseProcessImpl();
	~BaseProcessImpl();

	//Checks if the process is ended, no matter how it ends.
	bool isEnded() const;

	//Change the state of the process. Only be called from inside BaseProcess::update().
	void init(BaseProcess * process);	//Be called on the first call to update(). Calls vOnInit().
	void abort(BaseProcess * process);	//Be called from inside update if some internal issue occurs. Calls vOnAbort();    

	//Current state of the process.
	ProcessState m_State{ ProcessState::UnRun };

	//Child process. May be empty.
	std::unique_ptr<BaseProcess> m_Child;
};

BaseProcess::BaseProcessImpl::BaseProcessImpl()
{

}

BaseProcess::BaseProcessImpl::~BaseProcessImpl()
{

}

bool BaseProcess::BaseProcessImpl::isEnded() const
{
	return m_State == ProcessState::Succeeded || m_State == ProcessState::Failed || m_State == ProcessState::Aborted;
}

void BaseProcess::BaseProcessImpl::init(BaseProcess * process)
{
	assert(m_State == ProcessState::UnRun && process);
	process->vOnInit();
}

void BaseProcess::BaseProcessImpl::abort(BaseProcess * process)
{
	assert(!isEnded() && process);
	process->vOnAbort();
}

//////////////////////////////////////////////////////////////////////////
//Implementation of BaseProcess.
//////////////////////////////////////////////////////////////////////////
BaseProcess::BaseProcess() : pimpl(new BaseProcessImpl())
{

}

BaseProcess::~BaseProcess()
{

}

void BaseProcess::update(const std::chrono::milliseconds & deltaTimeMs)
{

}

void BaseProcess::succeed()
{
	assert(!pimpl->isEnded());
	vOnSucceed();
}

void BaseProcess::fail()
{
	assert(!pimpl->isEnded());
	vOnFail();
}

void BaseProcess::pause()
{
	assert(pimpl->m_State == ProcessState::Running);
	pimpl->m_State = ProcessState::Paused;
}

void BaseProcess::unPause()
{
	assert(pimpl->m_State == ProcessState::Paused);
	pimpl->m_State = ProcessState::Running;
}

bool BaseProcess::isSucceeded() const
{
	return pimpl->m_State == ProcessState::Succeeded;
}

bool BaseProcess::isFailed() const
{
	return pimpl->m_State == ProcessState::Failed;
}

bool BaseProcess::isAborted() const
{
	return pimpl->m_State == ProcessState::Aborted;
}

void BaseProcess::attachChild(std::unique_ptr<BaseProcess> && child)
{
	if (pimpl->m_Child)
		cocos2d::log("BaseProcess::attachChild overwritting an existing child process.");

	pimpl->m_Child = std::move(child);
}

std::unique_ptr<BaseProcess> BaseProcess::detachChild()
{
	return std::move(pimpl->m_Child);
}

const std::unique_ptr<BaseProcess> & BaseProcess::getChild() const
{
	return pimpl->m_Child;
}

void BaseProcess::vOnInit()
{

}

void BaseProcess::vOnSucceed()
{

}

void BaseProcess::vOnFail()
{

}

void BaseProcess::vOnAbort()
{

}
