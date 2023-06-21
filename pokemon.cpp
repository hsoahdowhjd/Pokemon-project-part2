#include "pokemon.h"
#include "move.h"
#include <string>
#include <vector>

Pokemon::Pokemon(){
    this->index=-1;
    this->name="";
    this->hp=-1;
    this->attack=-1;
    this->defense=-1;
    this->speed=-1;
    this->special=-1;
    this->typeOne="";
    this->typeTwo="";
}
Pokemon::Pokemon(int index,std::string name,int hp,int attack,int defense,int speed,int special){
    this->index=index;
    this->name=name;
    nickName=name;
    this->hp=hp;
    maxHp=hp;
    this->attack=attack;
    this->defense=defense;
    this->speed=speed;
    this->special=special;
    this->typeOne="";
    this->typeTwo="";
}
void Pokemon::setNickName(std::string nickName){
    this->nickName=nickName;
}
void Pokemon::setHp(int hp){
    if(hp>maxHp) hp=maxHp;
    this->hp=hp;
}
void Pokemon::setMaxHp(int maxHp){
    this->maxHp=maxHp;
}
void Pokemon::setAttack(int attack){
    this->attack=attack;
}
void Pokemon::setDefense(int defense){
    this->defense=defense;
}
void Pokemon::setSpeed(int speed){
    this->speed=speed;
}
void Pokemon::setSpecial(int special){
    this->special=special;
}
void Pokemon::setTypeOne(std::string typeOne){
    this->typeOne=typeOne;
}
void Pokemon::setTypeTwo(std::string typeTwo){
    this->typeTwo=typeTwo;
}
void Pokemon::setMoves(std::vector<Move> moves){
    this->moves=moves;
}
std::string Pokemon::getNickName(){
    return nickName;
}

int Pokemon::getIndex(){
    return index;
}
std::string Pokemon::getName(){
    return name;
}
int Pokemon::getHp(){
    return hp;
}
int Pokemon::getMaxHp(){
    return maxHp;
}
int Pokemon::getAttack(){
    return attack;
}
int Pokemon::getDefense(){
    return defense;
}
int Pokemon::getSpeed(){
    return speed;
}
int Pokemon::getSpecial(){
    return special;
}
std::string Pokemon::getTypeOne(){
    return typeOne;
}
std::string Pokemon::getTypeTwo(){
    return typeTwo;
}
std::vector<Move> Pokemon::getMoves(){
    return moves;
}
