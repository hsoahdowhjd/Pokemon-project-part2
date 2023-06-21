#pragma once
#include <string>

class Item{
    private:
        std::string name;
        int quantity;
        int heal_amount;
    public:
        Item(std::string,int,int);
        void set_name(std::string);
        void set_quantity(int);
        void set_heal_amount(int);
        void increment_quantity();
        void decrement_quantity();
        std::string get_name();
        int get_quantity();
        int get_heal_amount();
};
