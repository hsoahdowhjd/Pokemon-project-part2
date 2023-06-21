#pragma once
#include <string>
#include <vector>
#include "pokemon.h"
#include "item.h"
#include "move.h"

class Player{
    private:
        std::vector<Pokemon> party;
        std::vector<Item> items;
        std::vector<bool> badges={false,false,false,false};
        int y;
        int x;
        int money=0;
    public:
        Player(int,int);
        void add_pokemon(Pokemon,const std::vector<Move> &moves);
        int get_y();
        int get_x();
        std::vector<Pokemon> get_party();
        void set_party(std::vector<Pokemon>);
        void set_y(int);
        void set_x(int);
        void set_items(std::vector<Item>);
        std::vector<Item> get_items();
        int get_money();
        void set_money(int);
        void set_badges(int);
        std::vector<bool> get_badges();
};


