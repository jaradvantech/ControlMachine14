/*
 * algorithm_v2_types.cpp
 *
 *  Created on: Jun 5, 2018
 *      Author: andres
 */


#include "algorithms/algorithm_v2_types.h"


OrderManager::OrderManager(){

}
OrderManager::OrderManager(int numberOfArms){
	for(int i=0;i<numberOfArms;i++) _Manipulator_OrderList.push_back(Manipulators ());
}
void OrderManager::SetNumberOfArms(int numberOfArms){
	for(int i=0;i<numberOfArms;i++) _Manipulator_OrderList.push_back(Manipulators ());
}

Order* OrderManager::Manipulators::getOrder_byIndex(int _index){
	//constant time complexity

#define allowedtothrow 1

#if allowedtothrow
	//this will throw an out of range
	return &_OrderList.at(_index);
#else
	//this will not throw, we can check against nullptr
	if(_OrderList.size()<_index && _OrderList.size()>=0) return &_OrderList[_index];
	return nullptr;
#endif


}

//_position is given in relative encoder units with origin at the manipulator
Order* OrderManager::Manipulators::getOrder_afterPosition(int _position){
	//linear time complexity
	for(int i=0;i<_OrderList.size();i++){
		if(_OrderList[i].When>_position) return &_OrderList[i];
	}
	return nullptr;
}

//_position is given in relative encoder units with origin at the manipulator
Order* OrderManager::Manipulators::getOrder_beforePosition(int _position){
	//linear time complexity
	for(int i=_OrderList.size()-1;i<0;i--){
		if(_OrderList[i].When<_position) return &_OrderList[i];
	}
	return nullptr;
}

void OrderManager::AddOrder(const Brick& mBrick, const std::vector<int>& _Manipulator_Fixed_Position)
{
	//What stands for what order 1=catch and pick down 0=retrieve from the storage and put on the line
	//Where is at which side, 0=left 1=right

	int mWho = (mBrick.AssignedPallet-1)/2;	//Who is which manipulator. From 0 to NMANIPULATORS-1

	Order OrderToPlace(0,0,0);
	OrderToPlace.What = mBrick.Type==0;//If has a Type, must be picked down, what = 0. If it's a hole, must retrieve. what = 1
	OrderToPlace.Where = mBrick.AssignedPallet%2 == 1; //0=left 1=right
	OrderToPlace.When = _Manipulator_Fixed_Position.at(mWho)-mBrick.Position; //When is the distance in encoder units that is left to reach the assigned manipulator
	this->atManipulator(mWho)->InsertOrder(OrderToPlace);
}

void OrderManager::Manipulators::InsertOrder(Order OrderToPlace)
{
	//Here we have the order. But where to place it?
	//Orders must be ordered by its position.
	unsigned int i=0;
	while(i<_OrderList.size() && _OrderList.at(i).When < OrderToPlace.When){
				i++;
	}
	_OrderList.insert(_OrderList.begin()+i,OrderToPlace);
}
void OrderManager::Manipulators::RemoveFirstOrder(){
	_OrderList.pop_front();
}
//-------------------------------------------------------------------------------




OrderManager::Manipulators* OrderManager::atManipulator(int _index){
	return &_Manipulator_OrderList.at(_index);
}

bool OrderManager::CheckIfOrderExists(Order* _orderTocheck, Order* _outputOrder){
	if(_orderTocheck==nullptr) return false;
	_outputOrder = _orderTocheck;
	return true;
}

bool CheckIfOrderExists(Order* _orderTocheck){
	if(_orderTocheck==nullptr) return false;
	return true;
}



OrderManager orders(5);

void Foo(){
	//old
	//_Manipulator_Order_List->at(i).at(j).When
	//new

	orders.atManipulator(3)->getOrder_byIndex(3)->When;

	orders.atManipulator(3)->getOrder_afterPosition(2000)->When;


	Order* desiredOrder=nullptr;
	if(OrderManager::CheckIfOrderExists(orders.atManipulator(3)->getOrder_afterPosition(2000),desiredOrder)){

	}
	orders.atManipulator(3)->getOrder_beforePosition(2000)->What;
}
