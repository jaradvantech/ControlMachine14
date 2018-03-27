/*
 * algorithm_v2.h
 *
 *  Created on: Mar 26, 2018
 *      Author: baseconfig
 */

#ifndef ALGORITHMS_ALGORITHM_V2_H_
#define ALGORITHMS_ALGORITHM_V2_H_

#include <string>
#include <vector>
#include <deque>

class Brick
{
	public:
		Brick(short argType, int argPosition, short argAssignedPallet, short argDNI);

		static short size;
		short Type;
		int Position;
		short AssignedPallet;
		short DNI;
};

class Order
{
	public:
		Order(int When, bool Where, bool What);

		int When;
		bool Where;
		bool What;
};

//void CancelOrder(std::deque<Brick>*, std::vector<std::deque<Order>> *);

//void CancelOrder(Brick *, std::vector<std::deque<Order>> *);

void CancelOrder(int, std::vector<std::deque<Order>> *);

void SetNumberOfManipulators(short);

int  ConvertToAbsolute(int);

void FillDINs(std::deque<int>*);

int  CheckForSurfaceCoincidence(int, int, int);

int  CheckForFirstMatch(int, int, int);

int  CheckQuantityContained(int, int, int);


std::deque<Brick> GetListOfBricksOnTheLine(void);

void Algorithm_SetCurrentPackagingColor(int);

void Algorithm_SetCurrentPackagingGrade(int);

void Algorithm_SetManipulatorOperationTime(int);

void Algorithm_SetManipulatorModes(std::vector<int>);

void * AlgorithmV2(void *Arg);

#endif /* ALGORITHMS_ALGORITHM_V2_H_ */
