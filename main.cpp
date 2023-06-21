#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "pokemon.h"
#include "move.h"
#include "map.h"
#include "player.h"
#include <ncurses.h>
#include <string>
#include "item.h"

using namespace std;

vector<Pokemon> load_pokemon(const string &filename);
void load_types(const string &filename, vector<Pokemon> &pokemon);
vector<vector<float>> load_type_system(const string &filename);
vector<Move> load_moves(const string &filename);
void pokemon_debug(vector<Pokemon> pokemon, vector<Move> moves);
string winput(WINDOW *window,int y,int x, int max_char);
vector<Item> load_items();

int main() {
    vector<Pokemon> pokemon = load_pokemon("data/pokemon.txt");
    load_types("data/types.txt",pokemon);
    vector<Move> moves = load_moves("data/moves.txt");  
    vector<vector<float>> type_system= load_type_system("data/type_system.txt"); 

    initscr();
    noecho();
    cbreak();
    curs_set(0);
    start_color();

    init_pair(2,COLOR_GREEN,COLOR_BLACK);
    init_pair(4,COLOR_BLUE,COLOR_BLACK);
    init_pair(6,COLOR_CYAN,COLOR_BLACK);
    init_pair(3,COLOR_YELLOW,COLOR_BLACK);
    init_pair(5,COLOR_MAGENTA,COLOR_BLACK);

    int y_max, x_max;
    getmaxyx(stdscr,y_max,x_max);
    Map outside(y_max-12,x_max-32,2,15);
    printw("%i , %i",y_max,x_max);
    outside.build_random_map();

    Player *player= new Player(outside.get_player_y(),outside.get_player_x());
    Pokemon deck;
    player->set_items(load_items());

    WINDOW *starter = newwin(6,x_max-12,y_max-8,5);
    box(starter, 0, 0);
    refresh();
    wrefresh(starter);

    keypad(starter, true);
    
    string starters[3] = {"Bulbasaur", "Charmander", "Squirtle"};
    int starter_choice;
    int starter_highlight = 0;

    //starter choice loop
    while (true){
        for (int i =0; i<3; i++) {
            if(i==starter_highlight) wattron(starter,A_REVERSE);
            mvwprintw(starter, i+1, 1, starters[i].c_str());
            wattroff(starter, A_REVERSE);
        }
        starter_choice = wgetch(starter);

        switch(starter_choice) {
            case KEY_UP:
                starter_highlight--;
                if(starter_highlight<0) starter_highlight=2;
                break;
            case KEY_DOWN:
                starter_highlight++;
                if(starter_highlight>2) starter_highlight=0;
                break;
            default:
                break;
        }
        
        //if enter is ckicked the current value of the highliget will be chosen
        if (starter_choice == 10) break;
    }
    
    keypad(starter, false);
    mvwprintw(starter,1,1,"What is %s name going to be?", starters[starter_highlight].c_str());
    mvwprintw(starter,2,1,"          ");
    mvwprintw(starter,3,1,"        ");
    wrefresh(starter);
    deck = pokemon.at(starter_highlight*3);

    string starter_name = winput(starter,2,1,20);
    if(starter_name==""){
        starter_name=starters[starter_highlight];
    }
    deck.setNickName(starter_name);
    /*deck.setMaxHp(1000000);
    deck.setHp(10000000);
    Move testt;
    for(Move m : moves) if(m.getName()=="Self-Destruct") testt=m;
    player->add_pokemon(deck,{testt});*/
    player->add_pokemon(deck,moves);
    mvwprintw(starter,1,1,"%s has been sent to your deck!       ",starter_name.c_str());
    mvwprintw(starter,2,1,"                    ");
    wrefresh(starter);
    


    getch();
    mvwprintw(starter,1,1,"                                                ");
    wrefresh(starter);

    outside.display();
    refresh();
    keypad(stdscr, true);

    while(true){
        int y=outside.get_player_y();
        int x=outside.get_player_x();
        int move_direction=wgetch(stdscr);
        int x_change=0;
        int y_change=0;

        switch(move_direction){
            case KEY_UP:
            case 'w':
                y_change--;
                break;
            case KEY_RIGHT:
            case 'd':
                x_change++;
                break;
            case KEY_LEFT:
            case 'a':
                x_change--;
                break;
            case KEY_DOWN:
            case 's':
                y_change++;
                break;
            default:
                break;
        }

        outside.move_player(y+y_change,x+x_change,player,pokemon,type_system,moves); 
        outside.display();
        wrefresh(starter);
        refresh();
    }

    getch();
    endwin();
}

string winput(WINDOW *window, int y , int x, int max_char){
    string result="";
    char value;
    while(true){
        value = wgetch(window);
        if(value==(char) 10){
            return result;
        }else if(value== '\b' && result!=""){
            //!WARNING! backspace dose not work for now
            mvwprintw(window,y,x-1," ");
            x--;
            result.pop_back();
        }else if(result.size()<max_char){
            string input(1,value);
            mvwprintw(window,y,x,input.c_str());
            x++;
            result+=input;
        }
        wrefresh(window);

    }
}

vector<Pokemon> load_pokemon(const string &filename) {
    ifstream file;
    file.open(filename);
    vector<Pokemon> pokemon;
    string line;
    string value;
    while(file.good()){
        Pokemon current;
        int index;
        string name;
        int hp;
        int attack;
        int defense;
        int speed;
        int special;

        getline(file,line);
        if(line=="") break;

        stringstream s(line);
        getline(s,value,'\t');
        index = stoi(value);
        getline(s,value,'\t');
        name = value;
        getline(s,value,'\t');
        hp = stoi(value);
        getline(s,value,'\t');
        attack = stoi(value);
        getline(s,value,'\t');
        defense = stoi(value);
        getline(s,value,'\t');
        speed = stoi(value);
        getline(s,value,'\t');
        special = stoi(value);
        pokemon.push_back({index,name,hp,attack,defense,speed,special});
    }
    return pokemon;
}


void load_types(const string &filename, vector<Pokemon> &pokemon) {
    ifstream file;
    file.open(filename);
    string line;
    string value;
    int i=0;
    while(file.good()){
        string typeTwo;
        
        getline(file,line);
        if(line=="") break;

        stringstream s(line);
        getline(s,value,'\t');
        getline(s,value,'\t');
        getline(s,value,'\t');
        pokemon.at(i).setTypeOne(value);
        getline(s,typeTwo);
        if(typeTwo=="") typeTwo="";
        pokemon.at(i).setTypeTwo(typeTwo);
        i++;
    }
    return;
}

vector<vector<float>> load_type_system(const string &filename){
    ifstream file;
    file.open(filename);
    string line;
    string value;
    vector<vector<float>> type_system;
    while(file.good()){
        vector<float> line_data;
        
        getline(file,line);
        if(line=="") break;

        stringstream s(line);
        for(int i=0;i<17;i++){
            getline(s,value,'\t');
            line_data.push_back(stof(value));
        }
        type_system.push_back(line_data);
    }
    return type_system;
}


vector<Move> load_moves(const string &filename) {
    ifstream file;
    file.open(filename);
    vector<Move> moves;
    string line;
    string value;
    while(file.good()){
        int index;
        string name;
        string type;
        string category;
        int PP;
        int power;
        int accuracy;

        getline(file,line);
        if(line=="") break;

        stringstream s(line);
        getline(s,value,'\t');
        if(value=="—") continue;
        index = stoi(value);
        getline(s,value,'\t');
        if(value=="—") continue;
        name = value;
        getline(s,value,'\t');
        if(value=="—") continue;
        type = value;
        getline(s,value,'\t');
        if(value=="—" || value=="Status") continue;
        category = value;
        getline(s,value,'\t');
        if(value=="—") continue;
        PP = stoi(value);
        getline(s,value,'\t');
        if(value=="—") continue;
        power = stoi(value);
        getline(s,value,'\t');
        if(value=="—") continue;
        value.pop_back();
        accuracy = stoi(value);
        moves.push_back({index,name,type,category,PP,power,accuracy});
    }
    return moves;
    
}


void pokemon_debug(vector<Pokemon> pokemon, vector<Move> moves){
    while(true){
        cout<<"=== Debug Menu ==="<<endl<<endl;
        cout<<"1.\tPokemon Data"<<endl;
        cout<<"2.\tMove Data"<<endl;
        cout<<"3.\tQuit"<<endl<<endl;
        cout<<"-> ";
        int output;
        cin>>output;
        if(!cin){
            cout<<"Error"<<endl;
            break;
        }
        cout<<endl;
        if(output==1){
            cout<<"Pokedex Number: ";
            int number;
            cin>>number;
            cout<<endl;
            if(!cin || number<1){
                continue;
            }
            Pokemon neww;
            for(Pokemon i:pokemon){
                if(i.getIndex()==number){
                    neww=i;
                    break;
                }
            }
            cout<<"Pokedex Entry "<<number<<": "<<neww.getName()<<endl;
            cout<<"HP: "<<neww.getHp()<<endl;
            cout<<"Attack: "<<neww.getAttack()<<endl;
            cout<<"Defense: "<<neww.getDefense()<<endl;
            cout<<"Speed: "<<neww.getSpeed()<<endl;
            cout<<"Special: "<<neww.getSpecial()<<endl;
            cout<<"Type 1: "<<neww.getTypeOne()<<endl;
            cout<<"Type 2: "<<neww.getTypeTwo()<<endl;
        }else if(output==2){
            cout<<"Move Index Number: ";
            int number;
            cin>>number;
            cout<<endl;
            if(!cin || number<1){
                continue;
            }
            Move neww;
            bool flag=false;
            for(Move i:moves){
                if(i.getIndex()==number){
                    neww=i;
                    flag=true;
                    break;
                }
            }
            if(!flag){
                cout<<endl;
                continue;
            } 

            cout<<"Move Index "<<number<<": "<<neww.getName()<<endl;
            cout<<"Type: "<<neww.getType()<<endl;
            cout<<"Category: "<<neww.getCategory()<<endl;
            cout<<"PP: "<<neww.getPP()<<endl;
            cout<<"Power: "<<neww.getPower()<<endl;
            cout<<"Accuracy: "<<neww.getAccuracy()<<"%"<<endl<<endl;


        }else if(output==3){
            break;
        }else{
            cout<<"Invalid Input!"<<endl;
        }
    }
}
vector<Item> load_items(){
    vector<Item> items;
    string names[4] = {"Potion","Super Potion","Hyper Potion","Max Potion"};
    int heal[4] = {20,50,200,1000000};
    for(int i = 0;i<4;i++){
        Item temp(names[i],0,heal[i]);
        items.push_back(temp);
    }
    return items;
}
