#include "item.h"
#include <string>

using namespace std;

Item::Item(string name,int quantity, int heal_amount){
    this->name=name;
    this->quantity=quantity;
    this->heal_amount=heal_amount;
}

void Item::set_name(string name){
    this->name=name;
}
void Item::set_quantity(int quantity){
    this->quantity=quantity;
}
void Item::set_heal_amount(int heal_amount){
    this->heal_amount=heal_amount;
}
void Item::increment_quantity(){
    quantity++;
}
void Item::decrement_quantity(){
    this->quantity--;
}
string Item::get_name(){
    return name;
}
int Item::get_quantity(){
    return quantity;
}
int Item::get_heal_amount(){
    return heal_amount;
}
