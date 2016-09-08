/*
 * ThreadTask.h
 *
 *  Created on: 2015年8月12日
 *      Author: dengxu
 */

#ifndef BASE_SRC_THREADTASK_H_
#define BASE_SRC_THREADTASK_H_

template<class TASK>
class CThreadTask
{
public:
	CThreadTask()
	{

	}
	virtual ~CThreadTask()
	{

	}
public:
	inline TASK & Instance() const
	{
		return this->m_task;
	}

	static void *task_proc(void *task)
	{
		CThreadTask *pThis = (CThreadTask *)task;
		if(pThis)
		{
			pThis->m_task.RunTask(&pThis->m_task);
		}
		return (void *)0;
	}
protected:
	mutable TASK m_task;
};

#endif /* BASE_SRC_THREADTASK_H_ */
