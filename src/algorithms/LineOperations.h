/*
 * LineOperations.h
 *
 *  Created on: Jul 2, 2018
 *      Author: baseconfig
 */

#ifndef ALGORITHMS_LINEOPERATIONS_H_
#define ALGORITHMS_LINEOPERATIONS_H_

#include <deque>
#include <algorithm>
#include "algorithms/algorithm_v2_types.h"

int Calculate_Advance(long* PreviousValueOfTheLineEncoder);
void CheckPhotoSensor1(std::deque<Brick>* _BricksBeforeTheLine);
bool CheckPhotoSensor4(std::deque<Brick>* _BricksBeforeTheLine, std::deque<Brick>* _BricksOnTheLine, std::deque<int>* Available_DNI_List);


#endif /* ALGORITHMS_LINEOPERATIONS_H_ */
