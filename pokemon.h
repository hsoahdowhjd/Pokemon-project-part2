#pragma once
#include <string>
#include <vector>
#include "move.h"

class Pokemon{
    private:
        int index;
        std::string name;
        int hp;
        int maxHp;
        int attack;
        int defense;
        int speed;
        int special;
        std::string typeOne;
        std::string typeTwo;
        std::string nickName;
        std::vector<Move> moves;
    public:
        Pokemon();
        Pokemon(int,std::string,int,int,int,int,int);
        void setNickName(std::string);
        void setHp(int);
        void setMaxHp(int);
        void setAttack(int);
        void setDefense(int);
        void setSpeed(int);
        void setSpecial(int);
        void setTypeOne(std::string);
        void setTypeTwo(std::string);
        void setMoves(std::vector<Move>);
        std::string getNickName();
        int getIndex();
        std::string getName();
        int getHp();
        int getMaxHp();
        int getAttack();
        int getDefense();
        int getSpeed();
        int getSpecial();
        std::string getTypeOne();
        std::string getTypeTwo();
        std::vector<Move> getMoves();
};
