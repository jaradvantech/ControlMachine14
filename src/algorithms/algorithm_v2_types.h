/*
 * algorithm_v2_types.h
 *
 *  Created on: Jun 5, 2018
 *      Author: andres
 */

#ifndef ALGORITHMS_ALGORITHM_V2_TYPES_H_
#define ALGORITHMS_ALGORITHM_V2_TYPES_H_

#include <deque>
#include <vector>

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


class OrderManager {
	public:
		class Manipulators{
			private:
			std::deque<Order> _OrderList;
			public:
			void AddOrder();
			void DeleteOrder();

			Order* getOrder_byIndex(int _index);
			Order* getOrder_afterPosition(int _position);
			Order* getOrder_beforePosition(int _position);

			//Piece of advice: check against nullptr to see if it exists before dereferencing.

			//Given the position on relative displacement from the origin, tells how much distance lasts until the tile reaches the manipulator
		};

	private:

	std::vector<Manipulators> _Manipulator_OrderList;

	public:
	OrderManager(int numberOfArms);
	Manipulators& atManipulator(int _index);
	static bool CheckIfOrderExists(Order* _orderTocheck, Order* _outputOrder);
	static bool CheckIfOrderExists(Order* _orderTocheck);

};


#endif /* ALGORITHMS_ALGORITHM_V2_TYPES_H_ */
