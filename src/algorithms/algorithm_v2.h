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
#include "algorithm_v2_types.h"
namespace Algorithm {
	namespace Set {
		void enable_WhetherOrNotPutTheTileTo_16(bool set_to);
		void force_input(bool set_to);
		void force_output(bool set_to);
		void forced_pallet(int set_to);
		void order(Brick brick);
		void CurrentPackagingColor(int PackagingColor);
		void CurrentPackagingGrade(int PackagingGrade);
		void ManipulatorOperationTime(int mManipulatorOperationTime);
		void ManipulatorModes(std::vector<int> mModes);
		void ManipulatorFixedPosition(std::vector<int> mManipulator_Fixed_Position);
	}
	namespace Get{
		std::deque<int> Available_DNI_List();
		std::vector<std::deque<Order>> Manipulator_Order_List();
		std::deque<Brick> Bricks_Before_The_Line();
		std::deque<Brick> Bricks_On_The_Line();

		std::vector<int> Bricks_Ready_For_Output();
		std::vector<int> Manipulator_Fixed_Position();
		std::vector<int> Manipulator_Modes();
		std::vector<int> Pallet_LowSpeedPulse_Height_List();
		std::vector<Brick> Manipulator_TakenBrick();
		std::vector<int> IndexesOfBricksOnLine(std::deque<Brick> Bricks_On_The_Line);
	}


	/*It shouldn't be needed anymore*/

//void CancelOrder(std::deque<Brick>*, std::vector<std::deque<Order>> *);

//void CancelOrder(Brick *, std::vector<std::deque<Order>> *);

//void CancelOrder(int, std::vector<std::deque<Order>> *);

//int  ConvertToAbsolute(int);

//void FillDINs(std::deque<int>*);

//int  CheckForSurfaceCoincidence(int, int, int);

//int  CheckForFirstMatch(int, int, int);

//int  CheckQuantityContained(int, int, int);


}

void * AlgorithmV2(void *Arg);

#endif /* ALGORITHMS_ALGORITHM_V2_H_ */
