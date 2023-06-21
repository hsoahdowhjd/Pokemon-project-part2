#pragma once
#include <string>

class Move{
    private:
        int index;
        std::string name;
        std::string type;
        std::string category;
        int maxPP;
        int PP;
        int power;
        int accuracy;
    public:
        Move();
        Move(int,std::string,std::string,std::string,int,int,int);
        void setPP(int);
        void setMaxPP(int);
        void setPower(int);
        void setAccuracy(int);
        int getIndex();
        std::string getName();
        std::string getType();
        std::string getCategory();
        int getPP();
        int getMaxPP();
        int getPower();
        int getAccuracy();
};
