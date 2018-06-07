/*
 * algorithm_v2_types.cpp
 *
 *  Created on: Jun 5, 2018
 *      Author: andres
 */


#include <algorithm_v2_types.h>



OrderManager::OrderManager(int numberOfArms){
	for(int i=0;i<numberOfArms;i++) _Manipulator_OrderList.push_back(Manipulators ());
}

Order* OrderManager::Manipulators::getOrder_byIndex(int _index){
	//constant time complexity

#define allowedtothrow 0

#if allowedtothrow
	//this will throw an out of range
	return _OrderList.at(_index);
#else
	//this will not throw, we can check against nullptr
	if(_OrderList.size()<_index && _OrderList.size()>=0) return _OrderList[_index];
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

void AddOrder(){


}
//-------------------------------------------------------------------------------


OrderManager::Manipulators& OrderManager::atManipulator(int _index){
	return _Manipulator_OrderList.at(_index);
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

	orders.atManipulator(3).getOrder_byIndex(3)->When;

	orders.atManipulator(3).getOrder_afterPosition(2000)->When;


	Order desiredOrder;
	if(OrderManager::CheckIfOrderExists(orders.atManipulator(3).getOrder_afterPosition(2000),&desiredOrder)){




	}
	orders.atManipulator(3).getOrder_beforePosition(2000)->What;
}
