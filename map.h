#pragma once
#include <vector>
#include <ncurses.h>
#include "player.h"
#include "pokemon.h"
#include "move.h"

class Map{
    private:
        WINDOW *map_window;
        int x_max, y_max,player_y,player_x;
        std::vector<std::vector<char>> map_data;
        int grass_walked = 0;
        char prev_tile = ' ';
        const char TALL_GRASS = '#';//30% green
        const char PLAYER = '@';
        const char TREE = '$';//8% green
        const char WATER = '~';//7% blue
        const char SHOP = '?';//7% cyan
        const char NPC = '!';//4% yellow
        const char GYM = '&';//4% majenta
        const char EMPTY = ' ';//40%
    public:
        Map(int,int,int,int);
        void build_random_map();
        void display();
        int get_player_y();
        int get_player_x();
        bool is_walkable(int,int,Player *player);
        void move_player(int y,int x,Player *player,const std::vector<Pokemon> &pokemon, const std::vector<std::vector<float>> &type_system,const std::vector<Move> &moves);
        void on_tile_grass(Player *player,const std::vector<Pokemon> &pokemon,const std::vector<std::vector<float>> &type_system,const std::vector<Move> &moves);
        std::vector<int> battle_menu(Player *player, int idx, WINDOW *choice);
        void store(Player *player);
        bool surf_check(Player *player);
        void NPC_batttle(Player *player,const std::vector<Pokemon> &pokemon,const std::vector<std::vector<float>> &type_system,const std::vector<Move> &moves);
        void GYM_battle(Player *player,const std::vector<Pokemon> &pokemon,const std::vector<std::vector<float>> &type_system,const std::vector<Move> &moves);

};
