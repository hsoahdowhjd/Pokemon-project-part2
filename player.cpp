#include "player.h"
#include "pokemon.h"
#include "move.h"
#include <string>
#include <vector>
#include <cstdlib>
#include "item.h"

using namespace std;

Player::Player(int y,int x){
    this->y=y;
    this->x=x;
}
void Player::add_pokemon(Pokemon add, const vector<Move> &moves){
    vector<Move> more;
    for(int i = 0;i<4;i++){
        more.push_back(moves.at(rand()%moves.size()));
    }
    add.setMoves(more);
    party.push_back(add);
}
int Player::get_y(){
    return y;
}
int Player::get_x(){
    return x;
}
vector<Pokemon> Player::get_party(){
    return party;
}
void Player::set_party(vector<Pokemon> party){
    this->party=party;
}
void Player::set_y(int y){
    this->y=y;
}
void Player::set_x(int x){
    this->x=x;
}
void Player::set_items(vector<Item> items){
    this->items=items;
}
vector<Item> Player::get_items(){
    return items;
}
int Player::get_money(){
    return money;
}
void Player::set_money(int money){
    this->money=money;
}
void Player::set_badges(int idx){
    badges.at(idx)=true;
}
vector<bool> Player::get_badges(){
    return badges;
}

