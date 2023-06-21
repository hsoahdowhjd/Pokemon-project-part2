#include "move.h"
#include <string>

Move::Move(){
    index=-1;
    name="";
    type="";
    category="";
    PP=-1;
    power=-1;
    accuracy=-1;
};
Move::Move(int index,std::string name,std::string type,std::string category,int PP,int power,int accuracy){
    this->index=index;
    this->name=name;
    this->type=type;
    this->category=category;
    this->PP=PP;
    this->maxPP=PP;
    this->power=power;
    this->accuracy=accuracy;
}
void Move::setPP(int PP){
    this->PP=PP;
}
void Move::setMaxPP(int maxPP){
    this->maxPP=maxPP;
}
void Move::setPower(int power){
    this->power=power;
}
void Move::setAccuracy(int accuracy){
    this->accuracy=accuracy;
}
int Move::getIndex(){
    return index;
}
std::string Move::getName(){
    return name;
}
std::string Move::getType(){
    return type;
}
std::string Move::getCategory(){
    return category;
}
int Move::getPP(){
    return PP;
}
int Move::getMaxPP(){
    return maxPP;
}
int Move::getPower(){
    return power;
}
int Move::getAccuracy(){
    return accuracy;
}
