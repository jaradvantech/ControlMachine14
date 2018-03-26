/*
 * SynchronizationPoints.cpp
 *
 *  Created on: Mar 23, 2018
 *      Author: Andres Grau
 */



#include <SynchronizationPoints.h>
#include <vector>
#include <thread>
#include <mutex>



std::vector<int> Synchro::SynchronizationPoints_List;
std::mutex Synchro::SynchronizationPoints_mutex;

int Synchro::GetSynchronizationPointValue(int IndexOfSynchronizationPoint) {
	std::lock_guard < std::mutex > lockGuard(SynchronizationPoints_mutex);
	if (IndexOfSynchronizationPoint < SynchronizationPoints_List.size()
			&& IndexOfSynchronizationPoint >= 0)
		return SynchronizationPoints_List[IndexOfSynchronizationPoint];
	else
		return -1;
}
int Synchro::GetSynchronizationPointNextFreeIndex() {
	std::lock_guard < std::mutex > lockGuard(SynchronizationPoints_mutex);
	return SynchronizationPoints_List.size();
}
void Synchro::IncreaseSynchronizationPointValue(
		int IndexOfSynchronizationPoint) {
	std::lock_guard < std::mutex > lockGuard(SynchronizationPoints_mutex);
	if (IndexOfSynchronizationPoint == SynchronizationPoints_List.size()) {
		SynchronizationPoints_List.push_back(1);
	} else if (IndexOfSynchronizationPoint < SynchronizationPoints_List.size()
			&& IndexOfSynchronizationPoint >= 0) {
		SynchronizationPoints_List[IndexOfSynchronizationPoint] += 1;
	}
}

	void Synchro::DecreaseSynchronizationPointValue(int IndexOfSynchronizationPoint)
	{
		std::lock_guard<std::mutex> lockGuard(SynchronizationPoints_mutex);
		if(IndexOfSynchronizationPoint<SynchronizationPoints_List.size() && IndexOfSynchronizationPoint>=0)
		{
			if (SynchronizationPoints_List[IndexOfSynchronizationPoint] > 0) SynchronizationPoints_List[IndexOfSynchronizationPoint]-=1;
		}
	}




