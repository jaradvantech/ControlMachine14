/*
 * SynchronizationPoints.h
 *
 *  Created on: Mar 23, 2018
 *      Author: Andres Grau
 */

#ifndef SYNCHRONIZATIONPOINTS_H_
#define SYNCHRONIZATIONPOINTS_H_
#include <vector>
#include <thread>
#include <mutex>


class Synchro{
private:
	static std::vector<int> SynchronizationPoints_List;
	static std::mutex SynchronizationPoints_mutex;
public:
	static int  GetSynchronizationPointValue(int IndexOfSynchronizationPoint);
	static int  GetSynchronizationPointNextFreeIndex();
	static void IncreaseSynchronizationPointValue(int IndexOfSynchronizationPoint);
	static void DecreaseSynchronizationPointValue(int IndexOfSynchronizationPoint);
};



#endif /* SYNCHRONIZATIONPOINTS_H_ */
