#include "map.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <ncurses.h>
#include <iostream>
#include "player.h"
#include "pokemon.h"
#include "move.h"
#include "item.h"
#include <fstream>

using namespace std;


Map::Map(int y_max,int x_max,int start_y,int start_x) {
    this->map_window=newwin(y_max+2,x_max+2,start_y-1,start_x-1);
    box(map_window,0,0);
    this->y_max=y_max;
    this->x_max=x_max;
    player_y=y_max/2;
    player_x=x_max/2;

    map_data.resize(y_max,vector<char>(x_max,' '));
}


void Map::build_random_map() {
    srand(time(NULL));
    for(int y=0; y< y_max;y++){
        for(int x=0; x<x_max;x++){
            int value=(rand()%100)+1;
            char char_value;
            if(value<=8 || y==0 || x==0 || y==y_max-1 || x==x_max-1){
                char_value=TREE;
            }else if(value<=38){
                char_value=TALL_GRASS;
            } else if(value<=45){
                char_value=WATER;
            } else if(value<=52){
                char_value=SHOP;
            } else if(value<=56){
                char_value=NPC;
            } else if(value<=60){
                char_value=GYM;
            } else{
                char_value=EMPTY;
            }
            map_data.at(y).at(x)=char_value;
        }
    }
    map_data.at(player_y).at(player_x)=PLAYER;
}

void Map::display(){
    for(int y=0;y<y_max;y++){
        for(int x=0;x<x_max;x++){
            char tile=map_data[y][x];
            string s(1,map_data[y][x]);
            int color_value=0;
            if(tile == TREE || tile == TALL_GRASS){
                wattron(map_window,COLOR_PAIR(2));
                color_value=2;
            }else if (tile == WATER){
                wattron(map_window,COLOR_PAIR(4));
                color_value=4;
            } else if (tile == SHOP){
                wattron(map_window,COLOR_PAIR(6));
                color_value=6;
            } else if (tile == NPC){
                wattron(map_window,COLOR_PAIR(3));
                color_value=3;
            } else if (tile == GYM){
                wattron(map_window,COLOR_PAIR(5));
                color_value=5;
            }
            mvwprintw(map_window,y+1,x+1,s.c_str());
            if(color_value>0){
                wattroff(map_window,COLOR_PAIR(color_value));
            }
        }
    }
    refresh();
    wrefresh(map_window);
    refresh();
}

bool Map::is_walkable(int y,int x,Player *player){
    if((map_data[y][x]!=TREE && map_data[y][x]!=WATER)||(surf_check(player) && map_data[y][x]==WATER)) return true;
    return false;
}

void Map::move_player(int y, int x,Player *player,const vector<Pokemon> &pokemon,const vector<vector<float>> &type_system, const vector<Move> &moves){
    if(!is_walkable(y,x,player)){
        return;
    }
    map_data[player_y][player_x]=prev_tile;
    prev_tile=map_data[y][x];
    map_data[y][x]=PLAYER;
    player_y=y;
    player_x=x;
    player->set_y(y);
    player->set_x(x);
    move(1,0);
    switch(prev_tile){
        case '#':
            on_tile_grass(player,pokemon,type_system,moves);
            break;
        case '?':
            store(player);
            break;
        case '!':
            NPC_batttle(player,pokemon,type_system,moves);
            break;
        case '&':
            GYM_battle(player,pokemon,type_system,moves);
            break; //battle
        default:
            break;
    }
}
int Map::get_player_y(){
    return player_y;
}
int Map::get_player_x(){
    return player_x;
}
int type_mod_value(string type){
    string types[17]={"Normal","Fighting","Poison","Ground","Rock","Bug","Ghost","Steel","Fire","Water","Grass","Electric","Psychic","Ice","Dragon","Dark","Fairy"};
    for(int i = 0;i<17;i++) if(types[i]==type) return i;
    return 1;
}
void Map::on_tile_grass(Player *player,const vector<Pokemon> &pokemon, const vector<vector<float>> &type_system, const vector<Move> &moves){
    int chance=1;
    grass_walked++;
    chance=grass_walked*9;
    srand(time(NULL));
    if((rand()%100)+1<=chance){
        grass_walked=0;
        display();
        WINDOW *battle = newwin(25,80,y_max/2-10,x_max/2-40);
        box(battle,0,0);
        wrefresh(battle);
        refresh();

        WINDOW *choice = newwin(10,78,y_max/2+4,x_max/2-39);
        box(choice,0,0);

        mvwprintw(choice,1,1,"Which pokemon from your deck would you like to use:");
        vector<Pokemon> party=player->get_party();

        for(size_t i=0;i<party.size();i++){
            int y = 2*((i/3)+1)+1;
            int x = (25*i)%75+5;
            mvwprintw(choice,y,x,party.at(i).getNickName().c_str());
        }

        Pokemon to_use;
        int int_choice;
        int light_up=0;
        keypad(choice,true);

        while(true){
            for(int i=0;i<party.size();i++){
                int y = 2*((i/3)+1)+1;
                int x = (25*i)%75+5;
                if(light_up==i) wattron(choice,A_REVERSE);
                mvwprintw(choice,y,x,party.at(i).getNickName().c_str());
                wattroff(choice,A_REVERSE);
            }
            refresh();
            wrefresh(choice);
            int_choice=wgetch(choice);

            switch(int_choice){
                case KEY_UP:
                    if(light_up>2){
                        light_up-=3;
                    }
                    break;
                case KEY_DOWN:
                    if(light_up+3>party.size()-1) break;
                    if(light_up<3){
                        light_up+=3;
                    }
                    break;
                case KEY_LEFT:
                    if(light_up-1<0) break;
                    if(light_up != 0 || light_up !=3){
                        light_up--;
                    }
                    break;
                case KEY_RIGHT:
                    if(light_up+1>party.size()-1) break;
                    if(light_up!=2 || light_up!=5){
                        light_up++;
                    }
                    break;
                default:
                    break;
            }

            if(int_choice==10)break;
        }
        Pokemon chosen=party.at(light_up);
        Pokemon enemy=pokemon.at(rand()%pokemon.size());

        mvwprintw(battle,2,10,enemy.getName().c_str());
        mvwprintw(battle,3,10,"Hp: %i",enemy.getHp());

        vector<Move> enemy_moves;
        for(int i = 0;i < 4;i++){
            enemy_moves.push_back(moves.at(rand()%moves.size()));
        }
        enemy.setMoves(enemy_moves);

        mvwprintw(battle,11,50,chosen.getNickName().c_str());
        mvwprintw(battle,12,50,"Hp: %i",chosen.getHp());

        wrefresh(battle);
        refresh();

        if(enemy.getSpeed()>chosen.getSpeed() || (rand()%2==1 && enemy.getSpeed()==chosen.getSpeed())){
            Move rand_move=enemy_moves.at(rand()%enemy_moves.size());
            if(rand()%100<rand_move.getAccuracy() && rand_move.getPP()!=0){
                rand_move.setPP(rand_move.getPP()-1);
                float stab=1.0;
                float type_mod=1;
                if(rand_move.getType()==enemy.getTypeOne()||rand_move.getType()==enemy.getTypeTwo()) stab+=0.5;
                type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(chosen.getTypeOne()));
                if(chosen.getTypeTwo()!="") type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(chosen.getTypeTwo()));
                chosen.setHp(chosen.getHp()-((rand_move.getPower()*enemy.getAttack())/chosen.getDefense()*stab*type_mod));
                if(chosen.getHp()<0) chosen.setHp(0);
                mvwprintw(battle,11,50,chosen.getNickName().c_str());
                mvwprintw(battle,12,50,"Hp: %i  ",chosen.getHp());
                wrefresh(battle);
                refresh();
            }
        }
        if(chosen.getHp()==0){
            wclear(choice);
            box(choice,0,0);
            mvwprintw(choice,1,1,"You lost, :(");
            chosen.setHp(10000000);
            party.at(light_up)=chosen;
            player->set_party(party);
            wrefresh(choice);
            refresh();
            getch();
            return ;
        }

        while(true){
            vector<int> value = battle_menu(player,light_up,choice);
            switch(value.at(0)){
                case 0:{
                           Move rand_move= chosen.getMoves()[value.at(1)];
                           if(rand()%100<rand_move.getAccuracy() && rand_move.getPP()!=0){
                               rand_move.setPP(rand_move.getPP()-1);
                               float stab=1.0;
                               float type_mod=1;
                               if(rand_move.getType()==chosen.getTypeOne()||rand_move.getType()==chosen.getTypeTwo()) stab+=0.5;
                               type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(enemy.getTypeOne()));
                               if(enemy.getTypeTwo()!="") type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(enemy.getTypeTwo()));
                               enemy.setHp(enemy.getHp()-((rand_move.getPower()*chosen.getAttack())/enemy.getDefense()*stab*type_mod));
                               if(enemy.getHp()<0) enemy.setHp(0);
                               mvwprintw(battle,2,10,enemy.getNickName().c_str());
                               mvwprintw(battle,3,10,"Hp: %i   ",enemy.getHp());
                               wrefresh(battle);
                               refresh();
                           }else{
                               wclear(choice);
                               box(choice,0,0);
                               mvwprintw(choice,1,1,"Move Faild!!");
                               wrefresh(choice);
                               refresh();
                               getch();
                           }
                           break;
                       }
                case 1:{
                           vector<Item> items=player->get_items();
                           items.at(value.at(1)).decrement_quantity();
                           player->set_items(items);
                           chosen.setHp(chosen.getHp()+items.at(value.at(1)).get_heal_amount());
                           wclear(battle);
                           box(battle,0,0);
                           mvwprintw(battle,11,50,chosen.getNickName().c_str());
                           mvwprintw(battle,12,50,"Hp: %i   ",chosen.getHp());
                           mvwprintw(battle,2,10,enemy.getName().c_str());
                           mvwprintw(battle,3,10,"Hp: %i  ",enemy.getHp());
                           wrefresh(battle);
                           refresh();
                           break;
                       }
                case 2:{
                           light_up=value.at(1);
                           chosen=party.at(light_up);
                           wclear(battle);
                           box(battle,0,0);
                           mvwprintw(battle,11,50,"%s              ",chosen.getNickName().c_str());
                           mvwprintw(battle,12,50,"Hp: %i   ",chosen.getHp());
                           mvwprintw(battle,2,10,enemy.getName().c_str());
                           mvwprintw(battle,3,10,"Hp: %i  ",enemy.getHp());
                           wrefresh(battle);
                           refresh();
                           break;
                       }
                default:
                       break;
            }
            if(value.at(0)==3) break;
            if(enemy.getHp()==0){
                wclear(choice);
                box(choice,0,0);
                int reward_money=rand()%500+300;
                player->set_money(player->get_money()+reward_money);
                if(party.size()!=6){
                    mvwprintw(choice,1,1,"You won!! %s was added to you deck",enemy.getName().c_str());
                    mvwprintw(choice,2,1,"You got %i reward money!!",reward_money);
                    enemy.setHp(10000000);
                    party.push_back(enemy);
                    player->set_party(party);
                }else{
                    mvwprintw(choice,1,1,"You won!!");
                    mvwprintw(choice,2,1,"You got %i reward money!!",reward_money);
                }
                wrefresh(choice);
                refresh();
                getch();
                break;
            }
            Move rand_move=enemy_moves.at(rand()%enemy_moves.size());
            if(rand()%100<rand_move.getAccuracy() && rand_move.getPP()!=0){
                rand_move.setPP(rand_move.getPP()-1);
                float stab=1.0;
                float type_mod=1;
                if(rand_move.getType()==enemy.getTypeOne()||rand_move.getType()==enemy.getTypeTwo()) stab+=0.5;
                type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(chosen.getTypeOne()));
                if(chosen.getTypeTwo()!="") type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(chosen.getTypeTwo()));
                chosen.setHp(chosen.getHp()-((rand_move.getPower()*enemy.getAttack())/chosen.getDefense()*stab*type_mod));
                if(chosen.getHp()<0) chosen.setHp(0);
                mvwprintw(battle,11,50,chosen.getNickName().c_str());
                mvwprintw(battle,12,50,"Hp: %i  ",chosen.getHp());
                wrefresh(battle);
                refresh();
            }
            if(chosen.getHp()==0){
                wclear(choice);
                box(choice,0,0);
                mvwprintw(choice,1,1,"You lost, :(");
                chosen.setHp(10000000);
                party.at(light_up)=chosen;
                player->set_party(party);
                wrefresh(choice);
                refresh();
                getch();
                break;
            }

        }
    }
}


vector<int> Map::battle_menu(Player *player,int idx,WINDOW *choice){
    wclear(choice);
    box(choice,0,0);
    vector<Pokemon> bag=player->get_party();

    mvwprintw(choice,1,1,"What will %s do:",bag.at(idx).getNickName().c_str());

    refresh();
    wrefresh(choice);

    vector<string> choices={"Fight","Item","Party","Run"};

    int light_up=0;
    int int_choice;

    while(true){
        for(int i=0;i<4;i++){
            int y = 2*(i/2)+3;
            int x = (35*i)%70+10;
            if(light_up==i) wattron(choice,A_REVERSE);
            mvwprintw(choice,y,x,choices.at(i).c_str());
            wattroff(choice,A_REVERSE);
        }
        refresh();
        wrefresh(choice);
        int_choice=wgetch(choice);

        switch(int_choice){
            case KEY_UP:
                if(light_up>1){
                    light_up-=2;
                }
                break;
            case KEY_DOWN:
                if(light_up<2){
                    light_up+=2;
                }
                break;
            case KEY_LEFT:
                if(light_up-1<0) break;
                if(light_up != 0 || light_up !=2){
                    light_up--;
                }
                break;
            case KEY_RIGHT:
                if(light_up+1>3) break;
                if(light_up!=1 || light_up!=3){
                    light_up++;
                }
                break;
            default:
                break;
        }

        if(int_choice==10)break;
    }
    switch(light_up){
        case 0:{
                   //fight
                   wclear(choice);
                   box(choice,0,0);

                   mvwprintw(choice,1,1,"What move will %s do:",bag.at(idx).getNickName().c_str());
                   vector<Move> moves=bag.at(idx).getMoves();
                   refresh();
                   wrefresh(choice);

                   vector<string> choices = {"back"};
                   for(Move i:moves){
                       choices.push_back(i.getName());
                   }

                   light_up=0;

                   while(true){
                       for(int i=-1;i<4;i++){
                           int y = 2*(i/2)+5;
                           int x = (35*i)%70+10;
                           if(i==-1){
                               y=3;
                               x=10;
                           }
                           if(light_up==i) wattron(choice,A_REVERSE);
                           mvwprintw(choice,y,x,choices.at(i+1).c_str());
                           wattroff(choice,A_REVERSE);
                       }
                       refresh();
                       wrefresh(choice);
                       int_choice=wgetch(choice);

                       switch(int_choice){
                           case KEY_UP:
                               if(light_up==0){
                                   light_up--;
                               }
                               if(light_up>1){
                                   light_up-=2;
                               }
                               break;
                           case KEY_DOWN:
                               if(light_up==-1){
                                   light_up++;
                               }else if(light_up<2){
                                   light_up+=2;
                               }
                               break;
                           case KEY_LEFT:
                               if(light_up-1<0) break;
                               if(light_up != 0 || light_up !=2){
                                   light_up--;
                               }
                               break;
                           case KEY_RIGHT:
                               if(light_up+1>3) break;
                               if(light_up!=1 || light_up!=3){
                                   light_up++;
                               }
                               break;
                           default:
                               break;
                       }

                       if(int_choice==10){
                           if(light_up!=-1)return {0,light_up};
                           else return battle_menu(player,idx,choice);
                       }
                   }
                   break;
               }
        case 1:{
                   wclear(choice);
                   box(choice,0,0);

                   vector<Item> items=player->get_items();
                   if(items[0].get_quantity()==0 && items[1].get_quantity()==0 && items[2].get_quantity()==0 && items[3].get_quantity()==0){
                       mvwprintw(choice,1,1,"You don't have any items, silly!");
                       refresh();
                       wrefresh(choice);
                       getch();
                       return battle_menu(player,idx,choice); 
                   }
                   light_up=0;
                   mvwprintw(choice,1,1,"Which item would you like to use:");
                   while(true){
                       for(int i=0;i<5;i++){
                           if(light_up==i-1) wattron(choice,A_REVERSE);
                           if(i==0) mvwprintw(choice,3,5,"back");
                           else{
                               int x=40*((i-1)%2)+5;
                               int y=2*((i-1)/2)+5;
                               mvwprintw(choice,y,x,"%sx%i",items.at(i-1).get_name().c_str(),items.at(i-1).get_quantity());
                           } 
                           wattroff(choice,A_REVERSE);
                       }
                       refresh();
                       wrefresh(choice);
                       int_choice=wgetch(choice);

                       switch(int_choice){
                           case KEY_UP:
                               if(light_up==0){
                                   light_up--;
                               }else if(light_up>1){
                                   light_up-=2;
                               }
                               break;
                           case KEY_DOWN:
                               if(light_up==-1){
                                   light_up++;
                               }else if(light_up<2){
                                   light_up+=2;
                               }
                               break;
                           case KEY_LEFT:
                               if(light_up-1<0) break;
                               if(light_up!=0 || light_up!=2){
                                   light_up--;
                               }
                               break;
                           case KEY_RIGHT:
                               if(light_up+1>3) break;
                               if(light_up!=1 || light_up!=3){
                                   light_up++;
                               }
                               break;
                           default:
                               break;
                       }
                       if(int_choice==10){
                           if(light_up==-1) return battle_menu(player,idx,choice);
                           if(items.at(light_up).get_quantity()!=0)return {1,light_up};
                           else{
                               wclear(choice);
                               box(choice,0,0);
                               mvwprintw(choice,1,1,"You don't have any of that item, silly!");
                               refresh();
                               wrefresh(choice);
                               getch();
                               return battle_menu(player,idx,choice); 

                           }
                       }

                   }
               }
        case 2:{
                   wclear(choice);
                   box(choice,0,0);
                   light_up=0;
                   mvwprintw(choice,1,1,"Which pokemon from your deck would you like to use:");
                   //for(int i=0;i<5;i++) mvwprintw(choice,i+3,1,"%i",i);
                   while(true){
                       for(size_t i=0;i<bag.size()+1;i++){
                           //mvwprintw(choice,5,1,to_string(bag.size()).c_str());
                           if(light_up==i-1) wattron(choice,A_REVERSE);
                           if(i==0) mvwprintw(choice,3,5,"back");
                           else{
                               int y = 2*((i-1)/3)+5;
                               int x = (25*(i-1))%75+5;
                               mvwprintw(choice,y,x,bag.at(i-1).getNickName().c_str());
                           }
                           wattroff(choice,A_REVERSE);
                       }
                       refresh();
                       wrefresh(choice);
                       int_choice=wgetch(choice);

                       switch(int_choice){
                           case KEY_UP:
                               if(light_up>2){
                                   light_up-=3;
                               }else if(light_up==0){
                                   light_up--;
                               }
                               break;
                           case KEY_DOWN:
                               if(light_up+3>bag.size()-1) break;
                               if( light_up==-1 ){
                                   light_up++;
                               }else if(light_up<3){
                                   light_up+=3;
                               }
                               break;
                           case KEY_LEFT:
                               if(light_up-1<0) break;
                               if(light_up != 0 || light_up !=3){
                                   light_up--;
                               }
                               break;
                           case KEY_RIGHT:
                               if(light_up+1>bag.size()-1) break;
                               if(light_up!=2 || light_up!=5){
                                   light_up++;
                               }
                               break;
                           default:
                               break;
                       }

                       if(int_choice==10){
                           if(light_up!=-1)return {2,light_up};
                           else return battle_menu(player,idx,choice);
                       }

                   }
               }
        case 3:{
                   return {3};
               }
        default:
               break;

    }
}
void Map::store(Player *player){
        WINDOW *stor = newwin(25,80,y_max/2-10,x_max/2-40);
        box(stor,0,0);
        wrefresh(stor);
        refresh();
        keypad(stor,true);

        int int_choice;
        int light_up=0;
        string items[4][2]={{"Potion","200"},{"Super Potion","400"},{"Hyper Potion","1200"},{"Max Potion","2500"}};

        while (true){
            for(int i = 0;i<5;i++){
                if(light_up==i-1) wattron(stor,A_REVERSE);
                if(i==0) mvwprintw(stor,3,4,"leave");
                else{
                    int y = 5*i+2;
                    mvwprintw(stor,y,4,items[i-1][0].c_str());
                    mvwprintw(stor,y,75,items[i-1][1].c_str());
                }
                wattroff(stor,A_REVERSE);
            }
            refresh();
            wrefresh(stor);
            int_choice=wgetch(stor);
            
            mvprintw(10,0,"%i",int_choice);

            switch(int_choice){
                case KEY_UP:
                    if(light_up!=-1) light_up--;
                    break;
                case KEY_DOWN:
                    if(light_up!=4) light_up++;
                    break;
                default:
                    break;
            }
            if(int_choice==10){
                if(light_up==-1) break;
                if(player->get_money()-stoi(items[light_up][1])<0){
                    wclear(stor);
                    box(stor,0,0);
                    mvwprintw(stor,1,1,"not enough money!");
                    refresh();
                    wrefresh(stor);
                    getch();
                    store(player);
                    break;
                }else{
                    player->set_money(player->get_money()-stoi(items[light_up][1]));    
                    vector<Item> items=player->get_items();
                    items.at(light_up).increment_quantity();
                    player->set_items(items);
                    wclear(stor);
                    box(stor,0,0);
                    mvwprintw(stor,1,1,"purchase succsesfull! %i money left.",player->get_money());
                    refresh();
                    wrefresh(stor);
                    getch();
                    store(player);
                    break;
                }
            }
        }
}
bool Map::surf_check(Player *player){
    for(Pokemon p : player->get_party()){
        for(Move m : p.getMoves()){
            if(m.getName()=="Surf") return true;
        }
    }
    return false;
}
void Map::NPC_batttle(Player *player,const std::vector<Pokemon> &pokemon,const std::vector<std::vector<float>> &type_system,const std::vector<Move> &moves){
        display();
        WINDOW *battle = newwin(25,80,y_max/2-10,x_max/2-40);
        box(battle,0,0);
        wrefresh(battle);
        refresh();

        WINDOW *choice = newwin(10,78,y_max/2+4,x_max/2-39);
        box(choice,0,0);

        mvwprintw(choice,1,1,"Which pokemon from your deck would you like to use:");
        vector<Pokemon> party=player->get_party();

        for(size_t i=0;i<party.size();i++){
            int y = 2*((i/3)+1)+1;
            int x = (25*i)%75+5;
            mvwprintw(choice,y,x,party.at(i).getNickName().c_str());
        }

        Pokemon to_use;
        int int_choice;
        int light_up=0;
        keypad(choice,true);

        while(true){
            for(int i=0;i<party.size();i++){
                int y = 2*((i/3)+1)+1;
                int x = (25*i)%75+5;
                if(light_up==i) wattron(choice,A_REVERSE);
                mvwprintw(choice,y,x,party.at(i).getNickName().c_str());
                wattroff(choice,A_REVERSE);
            }
            refresh();
            wrefresh(choice);
            int_choice=wgetch(choice);

            switch(int_choice){
                case KEY_UP:
                    if(light_up>2){
                        light_up-=3;
                    }
                    break;
                case KEY_DOWN:
                    if(light_up+3>party.size()-1) break;
                    if(light_up<3){
                        light_up+=3;
                    }
                    break;
                case KEY_LEFT:
                    if(light_up-1<0) break;
                    if(light_up != 0 || light_up !=3){
                        light_up--;
                    }
                    break;
                case KEY_RIGHT:
                    if(light_up+1>party.size()-1) break;
                    if(light_up!=2 || light_up!=5){
                        light_up++;
                    }
                    break;
                default:
                    break;
            }

            if(int_choice==10)break;
        }
        Pokemon chosen=party.at(light_up);
        vector<Pokemon> enemys;
        for(int i = 0;i<3;i++) enemys.push_back(pokemon.at(rand()%pokemon.size()));
        Pokemon enemy=enemys.at(0);
        mvwprintw(battle,2,10,enemy.getName().c_str());
        mvwprintw(battle,3,10,"Hp: %i",enemy.getHp());
        
        for(size_t en = 0; en<enemys.size();en++){
            vector<Move> enemy_moves;
            for(int i = 0;i < 4;i++){
                enemy_moves.push_back(moves.at(rand()%moves.size()));
            }
            enemys.at(en).setMoves(enemy_moves);
        }
        enemy=enemys.at(0);

        mvwprintw(battle,11,50,chosen.getNickName().c_str());
        mvwprintw(battle,12,50,"Hp: %i",chosen.getHp());

        wrefresh(battle);
        refresh();

        if(enemy.getSpeed()>chosen.getSpeed() || (rand()%2==1 && enemy.getSpeed()==chosen.getSpeed())){
            Move rand_move=enemy.getMoves().at(rand()%enemy.getMoves().size());
            if(rand()%100<rand_move.getAccuracy() && rand_move.getPP()!=0){
                rand_move.setPP(rand_move.getPP()-1);
                float stab=1.0;
                float type_mod=1;
                if(rand_move.getType()==enemy.getTypeOne()||rand_move.getType()==enemy.getTypeTwo()) stab+=0.5;
                type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(chosen.getTypeOne()));
                if(chosen.getTypeTwo()!="") type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(chosen.getTypeTwo()));
                chosen.setHp(chosen.getHp()-((rand_move.getPower()*enemy.getAttack())/chosen.getDefense()*stab*type_mod));
                if(chosen.getHp()<0) chosen.setHp(0);
                mvwprintw(battle,11,50,chosen.getNickName().c_str());
                mvwprintw(battle,12,50,"Hp: %i  ",chosen.getHp());
                wrefresh(battle);
                refresh();
            }
        }
        if(chosen.getHp()==0){
            wclear(choice);
            box(choice,0,0);
            mvwprintw(choice,1,1,"You lost, :(");
            chosen.setHp(10000000);
            party.at(light_up)=chosen;
            player->set_party(party);
            wrefresh(choice);
            refresh();
            getch();
            return ;
        }

        while(true){
            vector<int> value = battle_menu(player,light_up,choice);
            switch(value.at(0)){
                case 0:{
                           Move rand_move= chosen.getMoves()[value.at(1)];
                           if(rand()%100<rand_move.getAccuracy() && rand_move.getPP()!=0){
                               rand_move.setPP(rand_move.getPP()-1);
                               float stab=1.0;
                               float type_mod=1;
                               if(rand_move.getType()==chosen.getTypeOne()||rand_move.getType()==chosen.getTypeTwo()) stab+=0.5;
                               type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(enemy.getTypeOne()));
                               if(enemy.getTypeTwo()!="") type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(enemy.getTypeTwo()));
                               enemy.setHp(enemy.getHp()-((rand_move.getPower()*chosen.getAttack())/enemy.getDefense()*stab*type_mod));
                               if(enemy.getHp()<0) enemy.setHp(0);
                               mvwprintw(battle,2,10,enemy.getNickName().c_str());
                               mvwprintw(battle,3,10,"Hp: %i   ",enemy.getHp());
                               wrefresh(battle);
                               refresh();
                           }else{
                               wclear(choice);
                               box(choice,0,0);
                               mvwprintw(choice,1,1,"Move Faild!!");
                               wrefresh(choice);
                               refresh();
                               getch();
                           }
                           break;
                       }
                case 1:{
                           vector<Item> items=player->get_items();
                           items.at(value.at(1)).decrement_quantity();
                           player->set_items(items);
                           chosen.setHp(chosen.getHp()+items.at(value.at(1)).get_heal_amount());
                           wclear(battle);
                           box(battle,0,0);
                           mvwprintw(battle,11,50,chosen.getNickName().c_str());
                           mvwprintw(battle,12,50,"Hp: %i   ",chosen.getHp());
                           mvwprintw(battle,2,10,enemy.getName().c_str());
                           mvwprintw(battle,3,10,"Hp: %i  ",enemy.getHp());
                           wrefresh(battle);
                           refresh();
                           break;
                       }
                case 2:{
                           light_up=value.at(1);
                           chosen=party.at(light_up);
                           wclear(battle);
                           box(battle,0,0);
                           mvwprintw(battle,11,50,"%s              ",chosen.getNickName().c_str());
                           mvwprintw(battle,12,50,"Hp: %i   ",chosen.getHp());
                           mvwprintw(battle,2,10,enemy.getName().c_str());
                           mvwprintw(battle,3,10,"Hp: %i  ",enemy.getHp());
                           wrefresh(battle);
                           refresh();
                           break;
                       }
                default:
                       break;
            }
            if(value.at(0)==3) break;
            if(enemy.getHp()==0){
                enemys.erase(enemys.begin());
                if(enemys.size()==0){
                    wclear(choice);
                    box(choice,0,0);
                    int reward_money=rand()%1000+600;
                    player->set_money(player->get_money()+reward_money);
                    mvwprintw(choice,1,1,"You won!!");
                    mvwprintw(choice,2,1,"You got %i reward money!!",reward_money);
                    wrefresh(choice);
                    refresh();
                    getch();
                    break;
                }else{
                    enemy=enemys.at(0);
                           wclear(battle);
                           box(battle,0,0);
                           mvwprintw(battle,11,50,"%s              ",chosen.getNickName().c_str());
                           mvwprintw(battle,12,50,"Hp: %i   ",chosen.getHp());
                           mvwprintw(battle,2,10,enemy.getName().c_str());
                           mvwprintw(battle,3,10,"Hp: %i  ",enemy.getHp());
                           wrefresh(battle);
                           refresh();
                }
            }
            Move rand_move=enemy.getMoves().at(rand()%enemy.getMoves().size());
            if(rand()%100<rand_move.getAccuracy() && rand_move.getPP()!=0){
                rand_move.setPP(rand_move.getPP()-1);
                float stab=1.0;
                float type_mod=1;
                if(rand_move.getType()==enemy.getTypeOne()||rand_move.getType()==enemy.getTypeTwo()) stab+=0.5;
                type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(chosen.getTypeOne()));
                if(chosen.getTypeTwo()!="") type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(chosen.getTypeTwo()));
                chosen.setHp(chosen.getHp()-((rand_move.getPower()*enemy.getAttack())/chosen.getDefense()*stab*type_mod));
                if(chosen.getHp()<0) chosen.setHp(0);
                mvwprintw(battle,11,50,chosen.getNickName().c_str());
                mvwprintw(battle,12,50,"Hp: %i  ",chosen.getHp());
                wrefresh(battle);
                refresh();
            }
            if(chosen.getHp()==0){
                wclear(choice);
                box(choice,0,0);
                mvwprintw(choice,1,1,"You lost, :(");
                chosen.setHp(10000000);
                party.at(light_up)=chosen;
                player->set_party(party);
                wrefresh(choice);
                refresh();
                getch();
                break;
            }

    }
}
void Map::GYM_battle(Player *player,const std::vector<Pokemon> &pokemon,const std::vector<std::vector<float>> &type_system,const std::vector<Move> &moves){
        display();
        WINDOW *battle = newwin(25,80,y_max/2-10,x_max/2-40);
        box(battle,0,0);
        refresh();
        wrefresh(battle);

        WINDOW *choice = newwin(10,78,y_max/2+4,x_max/2-39);
        box(choice,0,0);

        mvwprintw(choice,1,1,"Which pokemon from your deck would you like to use:");
        vector<Pokemon> party=player->get_party();

        for(size_t i=0;i<party.size();i++){
            int y = 2*((i/3)+1)+1;
            int x = (25*i)%75+5;
            mvwprintw(choice,y,x,party.at(i).getNickName().c_str());
        }

        Pokemon to_use;
        int int_choice;
        int light_up=0;
        keypad(choice,true);

        while(true){
            for(int i=0;i<party.size();i++){
                int y = 2*((i/3)+1)+1;
                int x = (25*i)%75+5;
                if(light_up==i) wattron(choice,A_REVERSE);
                mvwprintw(choice,y,x,party.at(i).getNickName().c_str());
                wattroff(choice,A_REVERSE);
            }
            refresh();
            wrefresh(choice);
            int_choice=wgetch(choice);

            switch(int_choice){
                case KEY_UP:
                    if(light_up>2){
                        light_up-=3;
                    }
                    break;
                case KEY_DOWN:
                    if(light_up+3>party.size()-1) break;
                    if(light_up<3){
                        light_up+=3;
                    }
                    break;
                case KEY_LEFT:
                    if(light_up-1<0) break;
                    if(light_up != 0 || light_up !=3){
                        light_up--;
                    }
                    break;
                case KEY_RIGHT:
                    if(light_up+1>party.size()-1) break;
                    if(light_up!=2 || light_up!=5){
                        light_up++;
                    }
                    break;
                default:
                    break;
            }

            if(int_choice==10)break;
        }
        Pokemon chosen=party.at(light_up);
        vector<bool> badges=player->get_badges();
        int idx=0;
        if(badges.at(0)&&badges.at(1)&&badges.at(2)&&badges.at(3)) idx=rand()%4;
        else{
            idx=rand()%4;
            while(badges.at(idx)) idx=rand()%4;
        }
        vector<Pokemon> enemys;
        switch(idx){
            case 0: 
                enemys={pokemon.at(73),pokemon.at(94)};
                //73=geodude 94=onix
                break;
            case 1:
                enemys={pokemon.at(119),pokemon.at(120)};
                //119=staryu 120=starmie
                break;
            case 2:
                enemys={pokemon.at(99),pokemon.at(24),pokemon.at(25)};
                //99=Voltorb 24=pikachu 25=raichu
                break;
            case 3:
                enemys={pokemon.at(70),pokemon.at(113),pokemon.at(44)};
                //70=victreebel 113=tangela 44=vileplume
                break;
        }
        Pokemon enemy=enemys.at(0);
        mvwprintw(battle,2,10,enemy.getName().c_str());
        mvwprintw(battle,3,10,"Hp: %i",enemy.getHp());
        
        for(size_t en = 0; en<enemys.size();en++){
            vector<Move> enemy_moves;
            for(int i = 0;i < 4;i++){
                enemy_moves.push_back(moves.at(rand()%moves.size()));
            }
            enemys.at(en).setMoves(enemy_moves);
        }
        enemy=enemys.at(0);

        mvwprintw(battle,11,50,chosen.getNickName().c_str());
        mvwprintw(battle,12,50,"Hp: %i",chosen.getHp());

        wrefresh(battle);
        refresh();

        if(enemy.getSpeed()>chosen.getSpeed() || (rand()%2==1 && enemy.getSpeed()==chosen.getSpeed())){
            Move rand_move=enemy.getMoves().at(rand()%enemy.getMoves().size());
            if(rand()%100<rand_move.getAccuracy() && rand_move.getPP()!=0){
                rand_move.setPP(rand_move.getPP()-1);
                float stab=1.0;
                float type_mod=1;
                if(rand_move.getType()==enemy.getTypeOne()||rand_move.getType()==enemy.getTypeTwo()) stab+=0.5;
                type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(chosen.getTypeOne()));
                if(chosen.getTypeTwo()!="") type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(chosen.getTypeTwo()));
                chosen.setHp(chosen.getHp()-((rand_move.getPower()*enemy.getAttack())/chosen.getDefense()*stab*type_mod));
                if(chosen.getHp()<0) chosen.setHp(0);
                mvwprintw(battle,11,50,chosen.getNickName().c_str());
                mvwprintw(battle,12,50,"Hp: %i  ",chosen.getHp());
                wrefresh(battle);
                refresh();
            }
        }
        if(chosen.getHp()==0){
            wclear(choice);
            box(choice,0,0);
            mvwprintw(choice,1,1,"You lost, :(");
            chosen.setHp(10000000);
            party.at(light_up)=chosen;
            player->set_party(party);
            wrefresh(choice);
            refresh();
            getch();
            return ;
        }

        while(true){
            vector<int> value = battle_menu(player,light_up,choice);
            switch(value.at(0)){
                case 0:{
                           Move rand_move= chosen.getMoves()[value.at(1)];
                           if(rand()%100<rand_move.getAccuracy() && rand_move.getPP()!=0){
                               rand_move.setPP(rand_move.getPP()-1);
                               float stab=1.0;
                               float type_mod=1;
                               if(rand_move.getType()==chosen.getTypeOne()||rand_move.getType()==chosen.getTypeTwo()) stab+=0.5;
                               type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(enemy.getTypeOne()));
                               if(enemy.getTypeTwo()!="") type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(enemy.getTypeTwo()));
                               enemy.setHp(enemy.getHp()-((rand_move.getPower()*chosen.getAttack())/enemy.getDefense()*stab*type_mod));
                               if(enemy.getHp()<0) enemy.setHp(0);
                               mvwprintw(battle,2,10,enemy.getNickName().c_str());
                               mvwprintw(battle,3,10,"Hp: %i   ",enemy.getHp());
                               wrefresh(battle);
                               refresh();
                           }else{
                               wclear(choice);
                               box(choice,0,0);
                               mvwprintw(choice,1,1,"Move Faild!!");
                               wrefresh(choice);
                               refresh();
                               getch();
                           }
                           break;
                       }
                case 1:{
                           vector<Item> items=player->get_items();
                           items.at(value.at(1)).decrement_quantity();
                           player->set_items(items);
                           chosen.setHp(chosen.getHp()+items.at(value.at(1)).get_heal_amount());
                           wclear(battle);
                           box(battle,0,0);
                           mvwprintw(battle,11,50,chosen.getNickName().c_str());
                           mvwprintw(battle,12,50,"Hp: %i   ",chosen.getHp());
                           mvwprintw(battle,2,10,enemy.getName().c_str());
                           mvwprintw(battle,3,10,"Hp: %i  ",enemy.getHp());
                           wrefresh(battle);
                           refresh();
                           break;
                       }
                case 2:{
                           light_up=value.at(1);
                           chosen=party.at(light_up);
                           wclear(battle);
                           box(battle,0,0);
                           mvwprintw(battle,11,50,"%s              ",chosen.getNickName().c_str());
                           mvwprintw(battle,12,50,"Hp: %i   ",chosen.getHp());
                           mvwprintw(battle,2,10,enemy.getName().c_str());
                           mvwprintw(battle,3,10,"Hp: %i  ",enemy.getHp());
                           wrefresh(battle);
                           refresh();
                           break;
                       }
                default:
                       break;
            }
            if(value.at(0)==3) break;
            if(enemy.getHp()==0){
                enemys.erase(enemys.begin());
                if(enemys.size()==0){
                    wclear(choice);
                    box(choice,0,0);
                    int reward_money=rand()%1000+600;
                    player->set_money(player->get_money()+reward_money);
                    player->set_badges(idx);
                    mvwprintw(choice,1,1,"You won!! and got gym badge %i!!",idx+1);
                    mvwprintw(choice,2,1,"You got %i reward money!!",reward_money);
                    wrefresh(choice);
                    refresh();
                    getch();
                    badges=player->get_badges();
                    if(badges.at(0)&&badges.at(1)&&badges.at(2)&&badges.at(3)){
                        WINDOW *win = newwin(5,57,y_max/2-5,x_max/2-29);
                        ifstream file;
                        file.open("./YOU_WIN.txt");
                        int line_idx=0;
                        string line;
                        while(file.good()){
                            getline(file,line);
                            if(line=="") break;

                            mvwprintw(win,line_idx,0,line.c_str());
                            line_idx++;
                        }
                        refresh();
                        wrefresh(win);
                        getch();
                        wclear(win);
                        refresh();
                        wrefresh(win);
                    }
                    break;
                }else{
                    enemy=enemys.at(0);
                           wclear(battle);
                           box(battle,0,0);
                           mvwprintw(battle,11,50,"%s              ",chosen.getNickName().c_str());
                           mvwprintw(battle,12,50,"Hp: %i   ",chosen.getHp());
                           mvwprintw(battle,2,10,enemy.getName().c_str());
                           mvwprintw(battle,3,10,"Hp: %i               ",enemy.getHp());
                           wrefresh(battle);
                           refresh();
                }
            }
            Move rand_move=enemy.getMoves().at(rand()%enemy.getMoves().size());
            if(rand()%100<rand_move.getAccuracy() && rand_move.getPP()!=0){
                rand_move.setPP(rand_move.getPP()-1);
                float stab=1.0;
                float type_mod=1;
                if(rand_move.getType()==enemy.getTypeOne()||rand_move.getType()==enemy.getTypeTwo()) stab+=0.5;
                type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(chosen.getTypeOne()));
                if(chosen.getTypeTwo()!="") type_mod*=type_system.at(type_mod_value(rand_move.getType())).at(type_mod_value(chosen.getTypeTwo()));
                chosen.setHp(chosen.getHp()-((rand_move.getPower()*enemy.getAttack())/chosen.getDefense()*stab*type_mod));
                if(chosen.getHp()<0) chosen.setHp(0);
                mvwprintw(battle,11,50,chosen.getNickName().c_str());
                mvwprintw(battle,12,50,"Hp: %i  ",chosen.getHp());
                wrefresh(battle);
                refresh();
            }
            if(chosen.getHp()==0){
                wclear(choice);
                box(choice,0,0);
                mvwprintw(choice,1,1,"You lost, :(");
                chosen.setHp(10000000);
                party.at(light_up)=chosen;
                player->set_party(party);
                wrefresh(choice);
                refresh();
                getch();
                break;
            }

    }
}
