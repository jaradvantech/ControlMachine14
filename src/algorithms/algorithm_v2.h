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
		Brick(short argType, int argPosition, short argAssignedManipulator, short argDNI);

		static short size;
		short Type;
		int Position;
		short AssignedManipulator;
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

void CancelOrder(std::deque<Brick>* mBrickList, std::vector<std::deque<Order>> * mOrder_List);
void CancelOrder(Brick * mBrick, std::vector<std::deque<Order>> * mOrder_List);
void CancelOrder(int index, std::vector<std::deque<Order>> * mOrder_List);
void SetNumberOfManipulators(short NumberOfManipulators);
int  ConvertToAbsolute(int mPosition);
int  Calculate_Advance(long &);
void update_list(std::deque<Brick>*, int, std::deque<int>*);


#endif /* ALGORITHMS_ALGORITHM_V2_H_ */
