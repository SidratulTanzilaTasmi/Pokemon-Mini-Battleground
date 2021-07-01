#include <bits/stdc++.h>
#include <conio.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

std::string types[19]={"None", "Normal", "Grass", "Fire", "Water", "Electric", "Ice", "Ground", "Rock", "Steel", "Fighting", "Poison", "Bug", "Flying", "Psychic", "Ghost", "Dark", "Dragon", "Fairy"};
std::string movecategory[2]={"Physical", "Special"};

typedef struct move
{
    char name[25];
    int power, accuracy, pp, type, category;
    bool highcritical, multistrike;
    struct move *nxtmove;
} move;

typedef struct pokeprofile
{
    int dexid;
    char name[20];
    int baseHP, baseAttack, baseDefense, baseSpecialAttack, baseSpecialDefense, baseSpeed;
    int type1, type2;
    bool levitate;
    move *moves[4];
    char dexentry[500];
    struct pokeprofile *nxt;
} pokeprofile;

typedef struct inbattlePokemon
{
    pokeprofile *dex;
    int hp, maxhp;
    int stat[5]; //stat[0]=speed, stat[1]=attack, stat[2]=special attack, stat[3]=defense, stat[4]=special defense
    move *moveset[5];
    int pp[4];
    int form;
} pokemon;

typedef struct player
{
    char name[50];
    int gender;
    pokemon team[6];
    int photo;
} player;

pokeprofile* gotolast(void);
pokeprofile* gotoid(int id);
pokeprofile* gotoPokemon(std::string name);
move* gotomove(std::string name);
move* gotoend(void);

void addPokemon(int id, std::string name, int hp, int atk, int def, int spa, int spd, int spe, int type1, int type2, bool ability, std::string move0, std::string move1, std::string move2, std::string move3);
void addDexentry(std::string name, std::string entry);
void addMove(std::string name, int pwr, int acc, int pp, int type, int category, bool crit, bool strike);

void icon();
int menu(void);
void intro();

void initmoves(void);
void initPokemon(void);

void initsound(void);
void changetrack(int id);

void pokeinfo(pokeprofile *p);
void moveinfo(move *p);
void pokelist(void);
void cry(pokeprofile *p);

bool raised(pokeprofile *p);
bool hit(move *m);
bool stab(pokeprofile *att, move *m);
int typefx(pokeprofile *def, move *mv);
bool criticalhit(pokeprofile *att, move *m);

void teambuilder(bool pos);
void autobuilder(bool pos, int num);
void autonomen(bool pos);
void choosePokemon(bool pos);
void attack(bool pos);
bool lose(player *trainer);

void addBG(int bg);
void drawsprite(bool pos, int id);
void drawtrainer(bool pos);
void drawtext(void);
void puttext(std::string txt, int x, int y, int size);
void textdisplay(std::string str);
void drawoption(std::string str1, std::string str2, std::string str3, std::string str4, int highlight);
void typechart(void);
void teamdisplay(bool pos, int highlight);
void fulldisplay(void);

void labbg();
void labdisplay();
void dialoguebox(void);
void boxdisplay(std::string str);

void rotomdex(void);
void dexbeep(void);
void screenscanf(int x, int y, int limit);

pokeprofile *pokedex;
move *list;
player trainer[2];
int bg=0, active[2];
std::string msg="";

sf::RenderWindow window;
sf::Event event;
sf::Clock clk;
sf::Music music[2];
sf::Font arial, gameboy;

int main()
{
    window.setKeyRepeatEnabled(false);

    pokedex=(pokeprofile*)malloc(sizeof(pokeprofile));
    pokedex->nxt=0;

    list=(move*)malloc(sizeof(move));
    list->nxtmove=0;

    srand(time(0));
    window.create(sf::VideoMode(1280, 720), "Pokemon Mini BattleGround");
    icon();
    arial.loadFromFile("arial.ttf");
    gameboy.loadFromFile("GameBoy.ttf");
    initmoves();
    initPokemon();
    initsound();

    int choice;

    while(window.isOpen())
    {
        if(!bg)
        {
            intro();
            changetrack(1);
            bg=rand()%29;
        }
        if(!window.isOpen()) break;

        choice=menu();
        msg="";

        if(choice==1)
        {
            int truant[2]={};
            active[0]=6;
            active[1]=6;

            boxdisplay("Hello there!");
            boxdisplay("I am Professor Oak.");
            boxdisplay("Glad to meet you.");
            teambuilder(1);
            teambuilder(0);
            if(!window.isOpen()) window.close();

            if(trainer[1].photo==26 && trainer[0].photo==2) trainer[0].photo=4;
            if(trainer[1].photo==26 && trainer[0].photo==11) trainer[0].photo=12;
            else if(trainer[1].photo==19 && trainer[0].photo==5) trainer[0].photo=1;
            else if(trainer[1].photo==27 && trainer[0].photo==1) trainer[0].photo=5;

            addBG(bg);
            drawtrainer(1);
            drawtrainer(0);

            choosePokemon(0);
            choosePokemon(1);

            while(!lose(&trainer[1]) && !lose(&trainer[0]) && window.isOpen()) //While the entire match is not over
            {
                int turn=rand()&1, high=0;
                int activity;
                bool tie=0, quit=0;

                if(!trainer[turn].team[active[turn]].hp) choosePokemon(turn);
                if(!trainer[!turn].team[active[!turn]].hp) choosePokemon(!turn);
                if(!window.isOpen()) break;

                changetrack(0);
                turn=0;
                if((trainer[1].team[active[1]].stat[0])<(trainer[0].team[active[0]].stat[0])) ++turn;

                while(trainer[1].team[active[1]].hp && trainer[0].team[active[0]].hp && window.isOpen()) //While the Pokemon are not knocked out
                {
                    turn++;
                    activity=5;

                    if(truant[turn&1])
                    {
                        if(trainer[turn&1].team[active[turn&1]].dex==gotoPokemon("Slaking"))
                        {
                            msg="";
                            if(trainer[1].team[active[1]].dex->dexid==trainer[0].team[active[0]].dex->dexid)
                            {
                                std::string str(trainer[turn&1].name);
                                msg=str+"'s ";
                            }
                            textdisplay(msg+"Slaking is loafing around!");
                            msg="";
                            truant[turn&1]=0;
                            continue;
                        }

                        else truant[turn&1]=0;
                    }

                    while(1)
                    {
                        std::string str(trainer[turn&1].name);
                        textdisplay("What will you do, Trainer "+str+"?");

                        break;
                    }

                    fulldisplay();
                    drawoption("Fight", "Switch", "Pokedex", "Run", 0);

                    while(activity>4 && window.isOpen())
                    {
                        while(window.pollEvent(event))
                        {
                            if(event.type==sf::Event::Closed)
                            {
                                window.close();
                                break;
                            }

                            if(event.type==sf::Event::Resized)
                            {
                                fulldisplay();
                                drawoption("Fight", "Switch", "Pokedex", "Run", 0);
                            }

                            if(event.type==sf::Event::TextEntered && event.text.unicode>'0' && event.text.unicode<'5') activity=event.text.unicode-'0';

                            if(event.type==sf::Event::MouseMoved)
                            {

                                if(event.mouseMove.y*720/window.getSize().y>=600 && event.mouseMove.y*720/window.getSize().y<=700)
                                {
                                    high=5;
                                    if(event.mouseMove.x*1280/window.getSize().x>=120 && event.mouseMove.x*1280/window.getSize().x<=600) high=1;
                                    else if(event.mouseMove.x*1280/window.getSize().x>=760 && event.mouseMove.x*1280/window.getSize().x<=1240) high=2;
                                    high+=2*(event.mouseMove.y*720/window.getSize().y>=650);

                                    if(high<5)
                                    {
                                        fulldisplay();
                                        drawoption("Fight", "Switch", "Pokedex", "Run", high);
                                    }

                                    else
                                    {
                                        fulldisplay();
                                        drawoption("Fight", "Switch", "Pokedex", "Run", 0);
                                        high=0;
                                    }
                                }

                                else if(high)
                                {
                                    fulldisplay();
                                    drawoption("Fight", "Switch", "Pokedex", "Run", 0);
                                    high=0;
                                }
                            }

                            if(event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left && event.mouseButton.y*720/window.getSize().y>=600 && event.mouseButton.y*720/window.getSize().y<=700)
                            {
                                if(event.mouseButton.x*1280/window.getSize().x>=120 && event.mouseButton.x*1280/window.getSize().x<=600) activity=1;
                                else if(event.mouseButton.x*1280/window.getSize().x>=760 && event.mouseButton.x*1280/window.getSize().x<=1240) activity=2;
                                activity+=2*(event.mouseButton.y*720/window.getSize().y>=650);
                            }
                        }
                    }
                    if(!window.isOpen()) break;
                    msg="";

                    if(activity==1)
                    {
                        attack(turn&1);
                        if(trainer[turn&1].team[active[turn&1]].dex==gotoPokemon("Slaking")) truant[turn&1]=1;
                    }

                    else if(activity==2)
                    {
                        active[(turn&1)]=6;
                        choosePokemon(turn&1);

                        changetrack(0);
                        turn=0;
                        if((trainer[1].team[active[1]].stat[0])<(trainer[0].team[active[0]].stat[0])) ++turn;
                        continue;
                    }

                    else if(activity==3)
                    {
                        rotomdex();
                        if(!window.isOpen()) break;
                        turn--;
                        changetrack(0);
                    }

                    else
                    {
                        for(int i=0; i<6; ++i) trainer[turn&1].team[i].hp=0;
                        quit=1;
                    }
                }
                if(!window.isOpen()) break;

                if(!quit) //If Pokemon is knocked out
                {
                    std::string str="";

                    if(!trainer[1].team[active[1]].hp && !trainer[0].team[active[0]].hp)
                    {
                        if(trainer[1].team[active[1]].dex->dexid==trainer[0].team[active[0]].dex->dexid)
                        {
                            std::string temp(trainer[1].team[active[1]].dex->name);
                            str="Both "+temp;
                        }

                        else
                        {
                            std::string str1(trainer[1].team[active[1]].dex->name), str2(trainer[0].team[active[0]].dex->name);
                            str="Both "+str1+" and "+str2;
                        }

                        tie=1;
                    }

                    else if (trainer[0].team[active[0]].hp)
                    {
                        if(trainer[1].team[active[1]].dex->dexid==trainer[0].team[active[0]].dex->dexid)
                        {
                            std::string str1(trainer[1].name);
                            str=str1+"'s ";
                        }

                        std::string str2(trainer[1].team[active[1]].dex->name);
                        str=str+str2;
                    }

                    else
                    {
                        if(trainer[1].team[active[1]].dex->dexid==trainer[0].team[active[0]].dex->dexid)
                        {
                            std::string str1(trainer[0].name);
                            str=str+str1+"'s ";
                        }

                        std::string str2(trainer[0].team[active[0]].dex->name);
                        str=str+str2;
                    }

                    switch(rand()%3)
                    {
                    case 1:
                        tie? str=str+" are ": str=str+" is ";
                        str=str+"unable to battle!";
                        break;
                    case 2:
                        tie? str=str+" are ": str=str+" is ";
                        str=str+"knocked out!";
                        break;
                    default:
                        str=str+" fainted!";
                    }

                    textdisplay(str);
                    msg="";
                }
            }
            if(!window.isOpen()) break;

            if(!lose(&trainer[1]))
            {
                std::string str(trainer[1].name);
                textdisplay("Trainer "+str+" wins the match!");
            }
            else if(!lose(&trainer[0]))
            {
                std::string str(trainer[0].name);
                textdisplay("Trainer "+str+" wins the match!");
            }
            else
            {
                std::string str1(trainer[1].name), str2(trainer[0].name);
                textdisplay("The match between Trainer "+str1+" and Trainer "+str2+" ended in a tie!");
            }

            changetrack(1);
            fulldisplay();
            if(!lose(&trainer[1])) teamdisplay(1, 6);
            if(!lose(&trainer[0])) teamdisplay(0, 6);

            while(window.isOpen())
            {
                bool flag=0;

                while(window.pollEvent(event))
                {
                    if(event.type==sf::Event::Closed) window.close();
                    if(event.type==sf::Event::Resized) fulldisplay();
                    if(event.type==sf::Event::KeyPressed || (event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left)) flag=1;
                }

                if(flag) break;
            }
        }

        else if(choice==2)
        {
            rotomdex();
            if(!window.isOpen()) break;
        }

        else window.close();

        bg++;
    }

    return 0;
}

pokeprofile* gotolast(void)
{
    pokeprofile *p=pokedex;
    while(p->nxt) p=p->nxt;
    return p;
}

pokeprofile* gotoid(int id)
{
    pokeprofile *p;
    for(p=pokedex->nxt; p && p->dexid!=id; p=p->nxt);
    return p;
}

pokeprofile* gotoPokemon(std::string name)
{
    pokeprofile *p;
    for(p=pokedex->nxt; p && strcmp(p->name, name.c_str()); p=p->nxt);
    return p;
}

void addPokemon(int id, std::string name, int hp, int atk, int def, int spa, int spd, int spe, int type1, int type2, bool ability, std::string move0, std::string move1, std::string move2, std::string move3)
{
    pokeprofile *species;
    species=(pokeprofile*)malloc(sizeof(pokeprofile));
    species->dexid=id;
    strcpy(species->name, name.c_str());
    species->baseHP=hp;
    species->baseAttack=atk;
    species->baseDefense=def;
    species->baseSpecialAttack=spa;
    species->baseSpecialDefense=spd;
    species->baseSpeed=spe;
    species->type1=type1;
    species->type2=type2;
    species->levitate=ability;
    species->moves[0]=gotomove(move0);
    species->moves[1]=gotomove(move1);
    species->moves[2]=gotomove(move2);
    species->moves[3]=gotomove(move3);
    species->nxt=NULL;

    pokeprofile *p=gotolast();
    p->nxt=species;
    addDexentry(name, "");
}

void addDexentry(std::string name, std::string entry)
{
    pokeprofile *p=gotoPokemon(name);
    strcpy(p->dexentry, entry.c_str());
}

move* gotoend(void)
{
    move *p=list;
    while(p->nxtmove) p=p->nxtmove;
    return p;
}

move* gotomove(std::string name)
{
    move *p;
    for(p=list->nxtmove; p && strcmp(p->name, name.c_str()); p=p->nxtmove);
    return p;
}

void addMove(std::string name, int pwr, int acc, int pp, int type, int category, bool crit, bool strike)
{
    move *newmove;
    newmove=(move*)malloc(sizeof(move));
    strcpy(newmove->name, name.c_str());
    newmove->power=pwr;
    newmove->accuracy=acc;
    newmove->pp=pp;
    newmove->type=type;
    newmove->category=category;
    newmove->highcritical=crit;
    newmove->multistrike=strike;
    newmove->nxtmove=NULL;

    move *p=gotoend();
    p->nxtmove=newmove;
}

int menu(void)
{
    sf::Texture texture, t1, t2, t3;
    sf::Sprite sprite, s1, s2, s3;

    texture.loadFromFile("img/menu/bg.jpg");
    sprite.setTexture(texture);

    t1.loadFromFile("img/menu/menu1.jpg");
    s1.setTexture(t1);
    s1.setPosition(100, 60);

    t2.loadFromFile("img/menu/menu2.jpg");
    s2.setTexture(t2);
    s2.setPosition(100, 270);

    t3.loadFromFile("img/menu/menu3.jpg");
    s3.setTexture(t3);
    s3.setPosition(100, 480);

    int choice;
    while(window.isOpen())
    {
        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
            {
                window.close();
                return 0;
            }

            if(event.type==sf::Event::TextEntered && event.text.unicode>'0' && event.text.unicode<'4')
            {
                choice=event.text.unicode;
                return choice%3;
            }

            if(event.type==sf::Event::MouseMoved)
            {
                if(event.mouseMove.x*1280/window.getSize().x>=100 && event.mouseMove.x*1280/window.getSize().x<=750)
                {
                    if(event.mouseMove.y*720/window.getSize().y>=60 && event.mouseMove.y*720/window.getSize().y<=240)
                    {
                        s1.setPosition(150, 60);
                        s2.setPosition(100, 270);
                        s3.setPosition(100, 480);
                    }

                    else if(event.mouseMove.y*720/window.getSize().y>=270 && event.mouseMove.y*720/window.getSize().y<=450)
                    {
                        s1.setPosition(100, 60);
                        s2.setPosition(150, 270);
                        s3.setPosition(100, 480);
                    }

                    else if(event.mouseMove.y*720/window.getSize().y>=480 && event.mouseMove.y*720/window.getSize().y<=660)
                    {
                        s1.setPosition(100, 60);
                        s2.setPosition(100, 270);
                        s3.setPosition(150, 480);
                    }

                    else if(s1.getPosition().x>100 || s2.getPosition().x>100 || s3.getPosition().x>100)
                    {
                        s1.setPosition(100, 60);
                        s2.setPosition(100, 270);
                        s3.setPosition(100, 480);
                    }
                }

                else if(s1.getPosition().x>100 || s2.getPosition().x>100 || s3.getPosition().x>100)
                {
                    s1.setPosition(100, 60);
                    s2.setPosition(100, 270);
                    s3.setPosition(100, 480);
                }
            }

            if(event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left && event.mouseButton.x*1280/window.getSize().x>=100 && event.mouseButton.x*1280/window.getSize().x<=750)
            {
                if(event.mouseButton.y*720/window.getSize().y>=60 && event.mouseButton.y*720/window.getSize().y<=240) return 1;
                if(event.mouseButton.y*720/window.getSize().y>=270 && event.mouseButton.y*720/window.getSize().y<=450) return 2;
                if(event.mouseButton.y*720/window.getSize().y>=480 && event.mouseButton.y*720/window.getSize().y<=660) return 0;
            }
        }

        window.clear();
        window.draw(sprite);
        window.draw(s1);
        window.draw(s2);
        window.draw(s3);
        window.display();
    }
}

void icon(void)
{
    sf::Image logo;
    logo.loadFromFile("img/Pikachu_Shuffle_small.png");

    window.setIcon(logo.getSize().x, logo.getSize().y, logo.getPixelsPtr());
}

void intro(void)
{
    sf::Texture texture;
    sf::Sprite sprite;

    sf::Music pika;
    pika.openFromFile("audio/intro.ogg");
    pika.setLoop(0);

    for(int i=0, t; i<101; i+=t+1)
    {
        bool close=0;

        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
            {
                window.close();
                return;
            }

            if(event.type==sf::Event::KeyPressed || (event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left))
            {
                pika.stop();
                window.clear();
                close=1;
            }
        }

        if(close) break;

        clk.restart();
        if(!i) pika.play();
        std::string str="img/intro/frame ("+std::to_string(i+1)+").png";
        texture.loadFromFile(str);
        sprite.setTexture(texture);
        window.draw(sprite);
        window.display();
        window.clear();

        t=floor(clk.getElapsedTime().asMilliseconds()*0.02);
        while(clk.getElapsedTime().asMilliseconds()<50*(t+1));
    }

    window.display();
}

void rotomdex(void)
{
    dexbeep();
    changetrack(1);

    sf::Texture texture;
    texture.loadFromFile("img/menu/bg.jpg");
    sf::Sprite sprite;
    sprite.setTexture(texture);

    int choice=5;
    while(choice && window.isOpen())
    {
        choice=5;

        window.clear();
        window.draw(sprite);
        puttext("1. Pokemon Info", 120, 120, 30);
        puttext("2. Move Info", 120, 240, 30);
        puttext("3. Pokemon List", 120, 360, 30);
        puttext("4. Type Effectiveness", 120, 480, 30);
        puttext("5. Exit Pokedex", 120, 600, 30);
        window.display();

        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
            {
                window.close();
                return;
            }

            if(event.type==sf::Event::TextEntered && event.text.unicode>'0' && event.text.unicode<'6')
            {
                choice=event.text.unicode-'0';
                choice%=5;
            }

            /*if(event.type==sf::Event::MouseMoved)
            {
                if(event.mouseMove.x*1280/window.getSize().x>=100 && event.mouseMove.x*1280/window.getSize().x<=750)
                {
                    if(event.mouseMove.y*720/window.getSize().y>=60 && event.mouseMove.y*720/window.getSize().y<=240)
                    {
                        s1.setPosition(150, 60);
                        s2.setPosition(100, 270);
                        s3.setPosition(100, 480);
                    }

                    else if(event.mouseMove.y*720/window.getSize().y>=270 && event.mouseMove.y*720/window.getSize().y<=450)
                    {
                        s1.setPosition(100, 60);
                        s2.setPosition(150, 270);
                        s3.setPosition(100, 480);
                    }

                    else if(event.mouseMove.y*720/window.getSize().y>=480 && event.mouseMove.y*720/window.getSize().y<=660)
                    {
                        s1.setPosition(100, 60);
                        s2.setPosition(100, 270);
                        s3.setPosition(150, 480);
                    }

                    else if(s1.getPosition().x>100 || s2.getPosition().x>100 || s3.getPosition().x>100)
                    {
                        s1.setPosition(100, 60);
                        s2.setPosition(100, 270);
                        s3.setPosition(100, 480);
                    }
                }

                else if(s1.getPosition().x>100 || s2.getPosition().x>100 || s3.getPosition().x>100)
                {
                    s1.setPosition(100, 60);
                    s2.setPosition(100, 270);
                    s3.setPosition(100, 480);
                }
            }*/

            /*if(event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left && event.mouseButton.x*1280/window.getSize().x>=100 && event.mouseButton.x*1280/window.getSize().x<=750)
            {
                if(event.mouseButton.y*720/window.getSize().y>=60 && event.mouseButton.y*720/window.getSize().y<=240) return 1;
                if(event.mouseButton.y*720/window.getSize().y>=270 && event.mouseButton.y*720/window.getSize().y<=450) return 2;
                if(event.mouseButton.y*720/window.getSize().y>=480 && event.mouseButton.y*720/window.getSize().y<=660) return 0;
            }*/
        }

        if(choice==1)
        {
            int i, id=0;

            boxdisplay("Which Pokemon do you want to know about?\n");
            screenscanf(310, 630, 15);
            if(!window.isOpen()) return;

            if(msg[0]>='a' && msg[0]<='z') msg[0]-=32;
            for(i=1; i<msg.length(); i++) if(msg[i]>='A' && msg[i]<='Z') msg[i]+=32;

            if(gotoPokemon(msg)) id=gotoPokemon(msg)->dexid;
            else for(i=0; i<msg.length() && msg[i]>='0' && msg[i]<='9'; i++) id=id*10+msg[i]-'0';

            if(gotoid(id))
            {
                msg="";
                pokeinfo(gotoid(id));
            }

            else
            {
                std::string str=msg;

                boxdisplay("I am sorry.");
                boxdisplay("We could not find any Pokemon named "+str+" in our database.");
                msg="";
            }
        }

        else if(choice==2)
        {
            int i;

            boxdisplay("Which move do you want to know about?\n");
            screenscanf(310, 630, 20);
            if(!window.isOpen()) return;

            if(msg[0]>='a' && msg[0]<='z') msg[0]-=32;
            for(i=1; i<msg.length(); i++) if(msg[i]>='A' && msg[i]<='Z') msg[i]+=32;
            for(i=1; i<msg.length(); i++) if(msg[i]>='a' && msg[i]<='z' && msg[i-1]==' ') msg[i]-=32;

            if(gotomove(msg))
            {
                moveinfo(gotomove(msg));
                msg="";
            }

            else
            {
                boxdisplay("I am sorry.");
                boxdisplay("We could not find any move named "+msg+"in our database.");
                msg="";
            }
        }

        else if(choice==3) pokelist();
        else if(choice==4) typechart();
    }
}

void initPokemon(void)
{
    addPokemon(3, "Venusaur", 80, 82, 83, 100, 100, 80, 2, 11, 0, "Razor Leaf", "Solar Beam", "Sludge Bomb", "Earth Power");
    addDexentry("Venusaur", "Venusaur, the Seed Pokemon. It is the evolved form of Ivysaur.\nThere is a large flower on Venusaur's back. The flower is said to take on vivid colors when it gets plenty of nutrition and sunlight. The flower's aroma soothes the emotions of people.");

    addPokemon(6, "Charizard", 78, 84, 78, 109, 85, 100, 3, 13, 0, "Flamethrower", "Air Slash", "Dragon Claw", "Seismic Toss");
    addDexentry("Charizard", "Charizard, the Flame Pokemon. It is the evolved form of Charmeleon.\nCharizard flies around the sky in search of powerful opponents. It breathes fire of such great heat that it melts almost anything. However, it never turns its fiery breath on any opponent weaker than itself. It is said that Charizard's fire burns hotter if it has experienced harsh battles.");

    addPokemon(9, "Blastoise", 79, 83, 100, 85, 105, 78, 4, 0, 0, "Bite", "Hydro Pump", "Ice Beam", "Dragon Pulse");
    addDexentry("Blastoise", "Blastoise, the Shellfish Pokemon. It is the evolved form of Wartortle.\nIt is a brutal Pokemon with pressurized water jets on its shell. Once it takes aim at its enemy, it blasts out water with even more force than a fire hose. It firmly plants its feet on the ground before shooting in order to withstand the recoil.");

    addPokemon(12, "Butterfree", 60, 45, 50, 90, 80, 70, 12, 13, 0, "Bug Buzz", "Acrobatics", "Hyper Beam", "Solar Beam");

    addPokemon(15, "Beedrill", 65, 90, 40, 45, 80, 75, 12, 11, 1, "Poison Jab", "Pin Missile", "Giga Impact", "Brick Break");

    addPokemon(18, "Pidgeot", 83, 80, 75, 70, 70, 101, 1, 13, 0, "Wing Attack", "Double Edge", "Steel Wing", "Hurricane");

    addPokemon(20, "Raticate", 55, 81, 60, 50, 70, 97, 1, 0, 0, "Bite", "Hyper Fang", "Shadow Ball", "Ice Beam");

    addPokemon(22, "Fearow", 65, 90, 65, 61, 61, 100, 1, 13, 0, "Drill Peck", "Steel Wing", "Drill Run", "Hyper Beam");

    addPokemon(24, "Arbok", 60, 95, 69, 65, 79, 80, 11, 0, 0, "Bite", "Iron Tail", "Dragon Tail", "Sludge Bomb");

    addPokemon(25, "Pikachu", 45, 80, 50, 75, 60, 120, 5, 0, 0, "Thunderbolt", "Iron Tail", "Volt Tackle", "Surf");
    addDexentry("Pikachu", "Pikachu, the Mouse Pokemon.\nIt has small electric sacs on both its cheeks. If threatened, it looses electric charges from the sacs. This intelligent Pokemon raises its tail to check its surroundings. When multiple Pikachu meet, they'll touch their tails together and exchange electricity through them as a form of greeting. Forests where nests of Pikachu live are dangerous, since the trees are so often struck by lightning.");

    addPokemon(26, "Raichu", 60, 90, 55, 90, 80, 110, 5, 0, 0, "Thunderbolt", "Thunder", "Wild Charge", "Play Rough");

    addPokemon(28, "Sandslash", 75, 100, 110, 45, 55, 65, 7, 0, 0, "Metal Claw", "Rock Tomb", "Pin Missile", "Bulldoze");

    addPokemon(31, "Nidoqueen", 90, 92, 87, 75, 85, 76, 11, 7, 0, "Double Edge", "Earth Power", "Sludge Wave", "Brick Break");

    addPokemon(34, "Nidoking", 81, 102, 77, 85, 75, 85, 11, 7, 0, "Poison Jab", "Drill Run", "Iron Tail", "Shadow Ball");

    addPokemon(36, "Clefable", 95, 70, 73, 95, 90, 60, 18, 0, 0, "Moonblast", "Meteor Mash", "Magical Leaf", "Fire Punch");

    addPokemon(38, "Ninetales", 73, 76, 75, 81, 100, 100, 3, 0, 0, "Fire Blast", "Solar Beam", "Extrasensory", "Flare Blitz");

    addPokemon(40, "Wigglytuff", 140, 70, 45, 85, 50, 45, 1, 18, 0, "Pound", "Play Rough", "Psychic", "Disarming Voice");

    addPokemon(42, "Golbat", 75, 80, 70, 65, 75, 90, 11, 13, 0, "Bite", "Poison Fang", "Air Slash", "Hyper Beam");

    addPokemon(45, "Vileplume", 75, 80, 85, 110, 90, 50, 2, 11, 0, "Bullet Seed", "Sludge Wave", "Energy Ball", "Moonblast");

    addPokemon(47, "Parasect", 60, 95, 80, 60, 80, 30, 12, 2, 0, "X-Scissor", "Brick Break", "Energy Ball", "Sludge Bomb");

    addPokemon(49, "Venomoth", 70, 65, 60, 90, 75, 90, 12, 11, 1, "Bug Buzz", "Sludge Bomb", "Poison Fang", "Hyper Beam");

    addPokemon(51, "Dugtrio", 35, 100, 50, 50, 70, 120, 7, 0, 0, "Bulldoze", "Earth Power", "Rock Tomb", "Sludge Wave");

    addPokemon(52, "Meowth", 40, 45, 35, 40, 40, 90, 1, 0, 0, "Scratch", "Fury Swipes", "Bite", "Swift");

    addPokemon(53, "Persian", 65, 70, 60, 65, 65, 115, 1, 0, 0, "Feint Attack", "Play Rough", "Shadow Ball", "Giga Impact");

    addPokemon(54, "Psyduck", 50, 52, 48, 65, 50, 55, 4, 0, 0, "Scratch", "Water Gun", "Water Pulse", "Aqua Tail");

    addPokemon(55, "Golduck", 80, 82, 78, 95, 80, 85, 4, 0, 0, "Ice Punch", "Hydro Pump", "Psychic", "Brick Break");

    addPokemon(57, "Primeape", 65, 105, 60, 60, 70, 95, 10, 0, 0, "Submission", "Thunder Punch", "Overheat", "Seismic Toss");

    addPokemon(59, "Arcanine", 90, 110, 80, 100, 80, 95, 3, 0, 0, "Flare Blitz", "Overheat", "Wild Charge", "Solar Beam");

    addPokemon(62, "Poliwrath", 90, 95, 95, 70, 90, 70, 4, 10, 0, "Bubble Beam", "Hydro Pump", "Submission", "Poison Jab");

    addPokemon(65, "Alakazam", 55, 50, 45, 135, 95, 120, 14, 0, 0, "Psychic", "Focus Punch", "Dazzling Gleam", "Thunder Punch");

    addPokemon(68, "Machamp", 90, 130, 80, 65, 85, 55, 10, 0, 0, "Vital Throw", "Dynamic Punch", "Seismic Toss", "Focus Blast");

    addPokemon(71, "Victreebel", 80, 105, 65, 100, 70, 70, 2, 11, 0, "Poison Jab", "Sludge Bomb", "Leaf Blade", "Power Whip");

    addPokemon(73, "Tentacruel", 80, 70, 65, 80, 120, 100, 4, 11, 0, "Cross Poison", "Sludge Wave", "Hydro Pump", "Blizzard");

    addPokemon(76, "Golem", 80, 120, 130, 55, 65, 45, 8, 7, 0, "Rock Slide", "Earthquake", "Fire Blast", "Thunder Punch");

    addPokemon(78, "Rapidash", 65, 100, 70, 80, 80, 105, 3, 0, 0, "Flamethrower", "Megahorn", "Flare Blitz", "Solar Beam");

    addPokemon(80, "Slowbro", 95, 75, 110, 100, 80, 30, 4, 14, 0, "Surf", "Zen Headbutt", "Psychic", "Shadow Ball");

    addPokemon(82, "Magneton", 50, 60, 95, 120, 70, 70, 5, 9, 1, "Spark", "Thunderbolt", "Zap Cannon", "Flash Cannon");

    addPokemon(83, "Farfetch'd", 52, 90, 55, 58, 62, 60, 1, 13, 0, "Double Edge", "Air Slash", "Leaf Blade", "Night Slash");

    addPokemon(85, "Dodrio", 60, 110, 70, 60, 60, 110, 1, 13, 0, "Headbutt", "Air Cutter", "Brave Bird", "Giga Impact");

    addPokemon(87, "Dewgong", 90, 70, 80, 70, 95, 70, 4, 6, 0, "Ice Shard", "Ice Beam", "Surf", "Drill Run");

    addPokemon(89, "Muk", 105, 105, 75, 65, 100, 50, 11, 0, 0, "Poison Jab", "Sludge Wave", "Ice Punch", "Dark Pulse");

    addPokemon(91, "Cloyster", 50, 95, 180, 85, 45, 70, 4, 6, 0, "Ice Shard", "Razor Shell", "Pin Missile", "Earth Power");

    addPokemon(94, "Gengar", 60, 65, 60, 130, 75, 110, 15, 11, 0, "Shadow Ball", "Shadow Claw", "Energy Ball", "Sludge Wave");

    addPokemon(95, "Onix", 35, 45, 160, 30, 45, 70, 8, 7, 0, "Dragon Breath", "Rock Blast", "Bulldoze", "Body Slam");

    addPokemon(97, "Hypno", 85, 73, 70, 73, 115, 67, 14, 0, 0, "Confusion", "Psychic", "Fire Punch", "Shadow Ball");

    addPokemon(99, "Kingler", 55, 130, 115, 50, 50, 75, 4, 0, 0, "Bubble Beam", "Metal Claw", "Hammer Arm", "Crabhammer");

    addPokemon(101, "Electrode", 60, 50, 70, 80, 80, 150, 5, 0, 0, "Thunderbolt", "Thunder", "Wild Charge", "Explosion");

    addPokemon(103, "Exeggutor", 95, 95, 85, 125, 75, 55, 2, 14, 0, "Bullet Seed", "Solar Beam", "Hyper Beam", "Psychic");

    addPokemon(105, "Marowak", 60, 80, 110, 50, 80, 45, 7, 0, 0, "Bone Rush", "Fire Punch", "Thunder Punch", "Earth Power");

    addPokemon(106, "Hitmonlee", 50, 120, 53, 35, 110, 87, 10, 0, 0, "Mega Kick", "Blaze Kick", "Low Sweep", "Brick Break");

    addPokemon(107, "Hitmonchan", 50, 105, 79, 35, 110, 76, 10, 0, 0, "Fire Punch", "Ice Punch", "Thunder Punch", "Focus Punch");

    addPokemon(108, "Lickitung", 90, 55, 75, 60, 75, 30, 1, 0, 0, "Lick", "Power Whip", "Flamethrower", "Hyper Beam");

    addPokemon(110, "Weezing", 65, 90, 120, 85, 70, 60, 11, 0, 1, "Sludge Bomb", "Sludge Wave", "Shadow Ball", "Giga Impact");

    addPokemon(112, "Rhydon", 105, 130, 120, 45, 45, 40, 7, 8, 0, "Rock Slide", "Earth Power", "Drill Run", "Horn Attack");

    addPokemon(113, "Chansey", 250, 5, 5, 35, 105, 50, 1, 0, 0, "Pound", "Take Down", "Disarming Voice", "Double Edge");

    addPokemon(114, "Tangela", 65, 55, 115, 100, 40, 60, 2, 0, 0, "Vine Whip", "Power Whip", "Ancient Power", "Sludge Bomb");

    addPokemon(115, "Kangaskhan", 105, 95, 80, 40, 80, 90, 1, 0, 0, "Crunch", "Brick Break", "Ice Beam", "Giga Impact");

    addPokemon(117, "Seadra", 55, 65, 95, 95, 45, 85, 4, 0, 0, "Dragon Breath", "Bubble Beam", "Waterfall", "Flash Cannon");

    addPokemon(119, "Seaking", 80, 92, 65, 65, 80, 68, 4, 0, 0, "Water Pulse", "Aqua Tail", "Megahorn", "Earth Power");

    addPokemon(121, "Starmie", 60, 75, 85, 100, 85, 115, 4, 14, 0, "Tackle", "Hydro Pump", "Psychic", "Power Gem");

    addPokemon(122, "Mr. Mime", 40, 45, 65, 100, 120, 90, 14, 18, 0, "Pound", "Confusion", "Dazzling Gleam", "Sucker Punch");

    addPokemon(123, "Scyther", 70, 110, 80, 55, 80, 105, 12, 13, 0, "X-Scissor", "Air Slash", "Brick Break", "Night Slash");

    addPokemon(124, "Jynx", 65, 50, 35, 115, 95, 95, 6, 14, 0, "Lick", "Confusion", "Pound", "Blizzard");

    addPokemon(125, "Electabuzz", 65, 83, 57, 95, 85, 105, 5, 0, 0, "Spark", "Thunderbolt", "Fire Punch", "Ice Punch");

    addPokemon(126, "Magmar", 65, 95, 57, 100, 85, 93, 3, 0, 0, "Fire Punch", "Flamethrower", "Karate Chop", "Fire Blast");

    addPokemon(127, "Pinsir", 65, 125, 100, 55, 70, 85, 12, 0, 0, "Bug Bite", "Submission", "Hyper Beam", "Rock Blast");

    addPokemon(128, "Tauros", 75, 100, 95, 40, 70, 110, 1, 0, 0, "Horn Attack", "Double Edge", "Close Combat", "Hyper Beam");

    addPokemon(129, "Magikarp", 20, 10, 55, 15, 20, 80, 4, 0, 0, "Splash", "Splash", "Splash", "Splash");
    addDexentry("Magikarp", "Magikarp, the Fish Pokemon.\nMagikarp is virtually useless in battle as it can only Splash around. As a result, it is considered to be the weakest and most pathetic Pokemon. However, it is actually a very hardy Pokemon that can survive in any body of water no matter how polluted it is.");

    addPokemon(130, "Gyarados", 95, 125, 79, 60, 100, 81, 4, 13, 0, "Aqua Tail", "Crunch", "Hyper Beam", "Hurricane");

    addPokemon(131, "Lapras", 130, 85, 80, 85, 95, 60, 4, 6, 0, "Surf", "Blizzard", "Zen Headbutt", "Drill Run");

    addPokemon(133, "Eevee", 65, 75, 70, 65, 85, 75, 1, 0, 0, "Tackle", "Swift", "Shadow Ball", "Double Edge");

    addPokemon(134, "Vaporeon", 130, 65, 60, 110, 95, 65, 4, 0, 0, "Swift", "Aqua Tail", "Ice Beam", "Hydro Pump");

    addPokemon(135, "Jolteon", 65, 65, 60, 110, 95, 130, 5, 0, 0, "Swift", "Thunder Fang", "Thunder", "Shadow Ball");

    addPokemon(136, "Flareon", 65, 130, 60, 95, 110, 65, 3, 0, 0, "Swift", "Fire Fang", "Overheat", "Shadow Ball");

    addPokemon(139, "Omastar", 70, 60, 125, 115, 70, 55, 8, 4, 0, "Rock Blast", "Rock Slide", "Hydro Pump", "Blizzard");

    addPokemon(141, "Kabutops", 60, 115, 105, 65, 70, 80, 8, 4, 0, "Razor Shell", "Stone Edge", "Night Slash", "Ice Beam");

    addPokemon(142, "Aerodactyl", 80, 105, 65, 60, 75, 130, 8, 13, 0, "Wing Attack", "Stone Edge", "Dragon Pulse", "Thunder Fang");

    addPokemon(143, "Snorlax", 160, 110, 65, 65, 110, 30, 1, 0, 0, "Zen Headbutt", "Hammer Arm", "Hyper Beam", "Body Slam");

    addPokemon(149, "Dragonite", 91, 134, 95, 100, 100, 80, 17, 13, 0, "Dragon Rush", "Hurricane", "Brick Break", "Steel Wing");

    addPokemon(154, "Meganium", 80, 82, 100, 83, 100, 80, 2, 0, 0, "Vine Whip", "Solar Beam", "Body Slam", "Earthquake");

    addPokemon(157, "Typhlosion", 78, 84, 78, 109, 85, 100, 3, 0, 0, "Flame Wheel", "Shadow Claw", "Solar Beam", "Blast Burn");

    addPokemon(160, "Feraligatr", 85, 105, 100, 79, 83, 78, 4, 0, 0, "Aqua Tail", "Ice Fang", "Dragon Claw", "Hydro Cannon");

    addPokemon(162, "Furret", 85, 76, 64, 45, 55, 90, 1, 0, 0, "Fury Swipes", "Ice Beam", "Sucker Punch", "Giga Impact");

    addPokemon(164, "Noctowl", 100, 50, 50, 86, 96, 70, 1, 13, 0, "Wing Attack", "Air Slash", "Extrasensory", "Hyper Beam");

    addPokemon(166, "Ledian", 55, 35, 50, 55, 110, 85, 12, 13, 0, "Bug Buzz", "Air Slash", "Double Edge", "Focus Blast");

    addPokemon(168, "Ariados", 70, 90, 70, 60, 70, 40, 12, 11, 0, "Bug Bite", "Cross Poison", "Night Slash", "Hyper Beam");

    addPokemon(169, "Crobat", 85, 90, 80, 70, 80, 130, 11, 13, 0, "Cross Poison", "Crunch", "Acrobatics", "Air Cutter");

    addPokemon(171, "Lanturn", 125, 58, 58, 76, 76, 67, 4, 5, 0, "Waterfall", "Hydro Pump", "Ice Beam", "Discharge");

    addPokemon(176, "Togetic", 55, 40, 85, 80, 105, 40, 18, 13, 0, "Dazzling Gleam", "Aerial Ace", "Extrasensory", "Brick Break");

    addPokemon(178, "Xatu", 65, 75, 70, 95, 70, 95, 14, 13, 0, "Psychic", "Aerial Ace", "Hyper Beam", "Solar Beam");

    addPokemon(181, "Ampharos", 90, 75, 85, 115, 90, 55, 5, 0, 0, "Dragon Pulse", "Thunder", "Iron Tail", "Seismic Toss");

    addPokemon(182, "Bellossom", 75, 80, 95, 90, 100, 50, 2, 0, 0, "Sludge Bomb", "Magical Leaf", "Play Rough", "Moonblast");

    addPokemon(184, "Azumarill", 100, 50, 80, 60, 80, 50, 4, 18, 0, "Bubble Beam", "Aqua Tail", "Double Edge", "Play Rough");

    addPokemon(185, "Sudowoodo", 70, 100, 115, 30, 65, 30, 8, 0, 0, "Wood Hammer", "Hammer Arm", "Head Smash", "Earth Power");

    addPokemon(186, "Politoed", 90, 75, 75, 90, 100, 70, 4, 0, 0, "Hydro Pump", "Earth Power", "Body Slam", "Ice Punch");

    addPokemon(189, "Jumpluff", 75, 55, 70, 55, 95, 110, 2, 13, 0, "Bullet Seed", "Acrobatics", "Solar Beam", "Dazzling Gleam");

    addPokemon(190, "Aipom", 55, 70, 55, 40, 55, 85, 1, 0, 0, "Swift", "Acrobatics", "Shadow Claw", "Focus Punch");

    addPokemon(192, "Sunflora", 75, 75, 55, 105, 85, 30, 2, 0, 0, "Pound", "Razor Leaf", "Double Edge", "Sludge Bomb");

    addPokemon(195, "Quagsire", 95, 85, 85, 65, 65, 35, 4, 7, 0, "Aqua Tail", "Hydro Pump", "Brick Break", "Earthquake");

    addPokemon(196, "Espeon", 65, 65, 60, 130, 95, 110, 14, 0, 0, "Swift", "Psychic", "Double Edge", "Dazzling Gleam");

    addPokemon(197, "Umbreon", 95, 65, 110, 60, 130, 65, 16, 0, 0, "Dark Pulse", "Feint Attack", "Swift", "Giga Impact");

    addPokemon(198, "Murkrow", 60, 85, 42, 85, 42, 91, 16, 13, 0, "Dark Pulse", "Drill Peck", "Air Cutter", "Steel Wing");

    addPokemon(199, "Slowking", 95, 75, 80, 100, 110, 30, 4, 14, 0, "Water Pulse", "Power Gem", "Psychic", "Blizzard");

    addPokemon(203, "Girafarig", 70, 80, 65, 90, 65, 85, 1, 14, 0, "Zen Headbutt", "Psychic", "Hyper Beam", "Dazzling Gleam");

    addPokemon(205, "Forretress", 75, 90, 140, 60, 60, 40, 12, 9, 0, "Bug Bite", "Flash Cannon", "Earthquake", "Explosion");

    addPokemon(206, "Dunsparce", 100, 70, 70, 65, 65, 45, 1, 0, 0, "Take Down", "Ancient Power", "Dragon Rush", "Iron Tail");

    addPokemon(207, "Gligar", 65, 75, 105, 35, 65, 85, 7, 13, 0, "Aerial Ace", "Bulldoze", "Rock Tomb", "Sludge Bomb");

    addPokemon(208, "Steelix", 75, 85, 200, 55, 65, 30, 9, 7, 0, "Iron Tail", "Dragon Pulse", "Earthquake", "Stone Edge");

    addPokemon(210, "Granbull", 90, 120, 75, 60, 60, 45, 18, 0, 0, "Fire Fang", "Thunder Fang", "Play Rough", "Dazzling Gleam");

    addPokemon(211, "Qwilfish", 65, 95, 85, 55, 55, 85, 4, 11, 0, "Aqua Tail", "Sludge Bomb", "Pin Missile", "Ice Beam");

    addPokemon(212, "Scizor", 70, 130, 100, 55, 80, 65, 12, 9, 0, "Metal Claw", "X-Scissor", "Brick Break", "Flash Cannon");

    addPokemon(213, "Shuckle", 20, 10, 230, 10, 230, 5, 12, 8, 0, "Bug Bite", "Rock Slide", "Body Slam", "Earth Power");

    addPokemon(214, "Heracross", 80, 125, 75, 40, 95, 85, 12, 10, 0, "Pin Missile", "Close Combat", "Stone Edge", "Megahorn");

    addPokemon(215, "Sneasel", 55, 95, 55, 35, 75, 115, 16, 6, 0, "Ice Shard", "Dark Pulse", "Psycho Cut", "Low Sweep");

    addPokemon(217, "Ursaring", 90, 130, 75, 75, 75, 55, 1, 0, 0, "Hyper Beam", "Hammer Arm", "Shadow Claw", "Play Rough");

    addPokemon(219, "Magcargo", 60, 50, 120, 90, 80, 30, 3, 8, 0, "Flamethrower", "Rock Slide", "Overheat", "Solar Beam");

    addPokemon(221, "Piloswine", 100, 100, 80, 60, 60, 50, 6, 7, 0, "Take Down", "Blizzard", "Ancient Power", "Bulldoze");

    addPokemon(222, "Corsola", 65, 55, 95, 65, 95, 35, 4, 8, 0, "Bubble Beam", "Bulldoze", "Power Gem", "Ice Beam");

    addPokemon(224, "Octillery", 75, 105, 75, 105, 75, 45, 4, 0, 0, "Bubble Beam", "Ice Beam", "Gunk Shot", "Rock Blast");

    addPokemon(226, "Mantine", 85, 40, 70, 80, 140, 70, 4, 13, 0, "Surf", "Air Slash", "Bulldoze", "Hurricane");

    addPokemon(227, "Skarmory", 65, 80, 140, 40, 70, 70, 9, 13, 0, "Steel Wing", "Brave Bird", "Dark Pulse", "Rock Tomb");

    addPokemon(229, "Houndoom", 75, 90, 50, 110, 80, 95, 16, 3, 0, "Flamethrower", "Fire Fang", "Crunch", "Solar Beam");

    addPokemon(230, "Kingdra", 75, 95, 95, 95, 95, 85, 4, 17, 0, "Iron Head", "Hydro Pump", "Draco Meteor", "Hurricane");

    addPokemon(232, "Donphan", 90, 120, 120, 60, 60, 50, 7, 0, 0, "Ice Shard", "Earthquake", "Ancient Power", "Giga Impact");

    addPokemon(233, "Porygon2", 85, 80, 90, 105, 95, 60, 1, 0, 0, "Zap Cannon", "Zen Headbutt", "Hyper Beam", "Energy Ball");

    addPokemon(234, "Stantler", 73, 95, 62, 85, 65, 85, 1, 0, 0, "Take Down", "Double Edge", "Zen Headbutt", "Shadow Ball");

    addPokemon(237, "Hitmontop", 50, 95, 95, 35, 110, 70, 10, 0, 0, "Low Sweep", "Focus Blast", "Sucker Punch", "Bulldoze");

    addPokemon(241, "Miltank", 95, 80, 105, 40, 70, 100, 1, 0, 0, "Giga Impact", "Play Rough", "Shadow Ball", "Milk Drink");

    addPokemon(242, "Blissey", 255, 10, 10, 75, 135, 55, 1, 0, 0, "Pound", "Zen Headbutt", "Double Edge", "Dazzling Gleam");

    addPokemon(248, "Tyranitar", 100, 134, 110, 95, 100, 61, 8, 16, 0, "Flamethrower", "Stone Edge", "Giga Impact", "Crunch");

    addPokemon(254, "Sceptile", 70, 105, 65, 85, 85, 120, 2, 0, 0, "Leaf Blade", "Dragon Claw", "Stone Edge", "Frenzy Plant");

    addPokemon(257, "Blaziken", 80, 120, 70, 110, 70, 80, 3, 10, 0, "Blaze Kick", "Focus Blast", "Brave Bird", "Thunder Punch");

    addPokemon(260, "Swampert", 100, 110, 90, 85, 90, 60, 4, 7, 0, "Surf", "Earthquake", "Ice Beam", "Hydro Cannon");

    addPokemon(262, "Mightyena", 70, 90, 70, 60, 60, 70, 16, 0, 0, "Bite", "Dark Pulse", "Poison Fang", "Thunder Fang");

    addPokemon(264, "Linoone", 78, 70, 61, 50, 61, 100, 1, 0, 0, "Fury Swipes", "Pin Missile", "Ice Beam", "Shadow Claw");

    addPokemon(267, "Beautifly", 60, 70, 50, 100, 50, 65, 12, 13, 0, "Bug Buzz", "Acrobatics", "Shadow Ball", "Hyper Beam");

    addPokemon(269, "Dustox", 60, 50, 70, 50, 90, 65, 12, 11, 1, "Bug Bite", "Signal Beam", "Sludge Bomb", "Energy Ball");

    addPokemon(272, "Ludicolo", 80, 70, 70, 90, 100, 70, 4, 2, 0, "Zen Headbutt", "Thunder Punch", "Leaf Storm", "Hydro Pump");

    addPokemon(275, "Shiftry", 90, 100, 60, 90, 60, 80, 2, 16, 0, "Rock Tomb", "Hurricane", "Night Slash", "Leaf Storm");

    addPokemon(277, "Swellow", 60, 85, 60, 75, 50, 125, 1, 13, 0, "Air Cutter", "Steel Wing", "Aerial Ace", "Giga Impact");

    addPokemon(279, "Pelipper", 60, 50, 100, 95, 70, 65, 4, 13, 0, "Wing Attack", "Air Slash", "Hydro Pump", "Gunk Shot");

    addPokemon(282, "Gardevoir", 68, 65, 65, 125, 115, 80, 14, 18, 0, "Psychic", "Moonblast", "Shadow Ball", "Hyper Beam");

    addPokemon(284, "Masquerain", 70, 60, 62, 100, 82, 80, 12, 13, 0, "Bug Buzz", "Aerial Ace", "Bubble Beam", "Ice Beam");

    addPokemon(286, "Breloom", 60, 130, 80, 60, 60, 70, 2, 10, 0, "Bullet Seed", "Brick Break", "Sludge Bomb", "Rock Tomb");

    addPokemon(289, "Slaking", 150, 160, 100, 95, 65, 100, 1, 0, 0, "Hyper Beam", "Earthquake", "Hammer Arm", "Play Rough");
    addDexentry("Slaking", "Slaking, the Lazy Pokemon. It is the evolved form of Vigoroth.\nSlaking lies down most of the time. It gets up and moves only when there is no more grass close by to eat.");

    addPokemon(291, "Ninjask", 61, 90, 45, 50, 50, 160, 12, 13, 0, "Air Slash", "X-Scissor", "Night Slash", "Double Edge");

    addPokemon(295, "Exploud", 104, 91, 63, 91, 73, 68, 1, 0, 0, "Boomburst", "Crunch", "Extrasensory", "Brick Break");

    addPokemon(297, "Hariyama", 144, 120, 60, 40, 60, 50, 10, 0, 0, "Poison Jab", "Vital Throw", "Earthquake", "Seismic Toss");

    addPokemon(299, "Nosepass", 30, 45, 135, 45, 90, 30, 8, 0, 0, "Tackle", "Rock Slide", "Power Gem", "Zap Cannon");

    addPokemon(301, "Delcatty", 70, 65, 65, 55, 55, 90, 1, 0, 0, "Thunderbolt", "Iron Tail", "Zen Headbutt", "Double Edge");

    addPokemon(302, "Sableye", 50, 75, 75, 65, 65, 50, 16, 15, 0, "Scratch", "Shadow Claw", "Feint Attack", "Power Gem");

    addPokemon(303, "Mawile", 50, 85, 85, 55, 55, 50, 9, 18, 0, "Iron Head", "Sludge Bomb", "Play Rough", "Crunch");

    addPokemon(306, "Aggron", 70, 110, 180, 60, 60, 50, 9, 8, 0, "Iron Tail", "Dragon Claw", "Thunder Punch", "Stone Edge");

    addPokemon(308, "Medicham", 60, 60, 75, 60, 75, 80, 10, 14, 0, "Focus Punch", "Psychic", "Poison Jab", "Thunder Punch");

    addPokemon(310, "Manectric", 70, 75, 60, 105, 60, 105, 5, 0, 0, "Shock Wave", "Thunder", "Ice Fang", "Fire Blast");

    addPokemon(311, "Plusle", 60, 50, 40, 85, 75, 95, 5, 0, 0, "Swift", "Thunderbolt", "Signal Beam", "Iron Tail");

    addPokemon(312, "Minun", 60, 40, 50, 75, 85, 95, 5, 0, 0, "Swift", "Discharge", "Signal Beam", "Iron Tail");

    addPokemon(313, "Volbeat", 65, 73, 75, 47, 85, 85, 12, 0, 0, "Signal Beam", "Double Edge", "Brick Break", "Play Rough");

    addPokemon(314, "Illumise", 65, 47, 75, 73, 85, 85, 12, 0, 0, "Bug Buzz", "Shadow Ball", "Thunderbolt", "Dynamic Punch");

    addPokemon(315, "Roselia", 50, 60, 45, 100, 80, 65, 2, 11, 0, "Sludge Bomb", "Pin Missile", "Power Gem", "Leaf Storm");

    addPokemon(317, "Swalot", 100, 73, 83, 73, 83, 55, 11, 0, 0, "Sludge Bomb", "Ice Beam", "Shadow Ball", "Gunk Shot");

    addPokemon(319, "Sharpedo", 70, 120, 40, 95, 40, 95, 4, 16, 0, "Surf", "Crunch", "Ice Fang", "Poison Fang");

    addPokemon(321, "Wailord", 170, 90, 45, 90, 45, 60, 4, 0, 0, "Waterfall", "Avalanche", "Earthquake", "Hydro Pump");

    addPokemon(323, "Camerupt", 70, 100, 70, 105, 75, 40, 3, 7, 0, "Earthquake", "Overheat", "Solar Beam", "Stone Edge");

    addPokemon(324, "Torkoal", 70, 85, 140, 85, 70, 20, 3, 0, 0, "Flame Wheel", "Overheat", "Earth Power", "Body Slam");

    addPokemon(326, "Grumpig", 80, 45, 65, 90, 110, 80, 14, 0, 0, "Dynamic Punch", "Confusion", "Shadow Ball", "Seismic Toss");

    addPokemon(327, "Spinda", 60, 60, 60, 60, 60, 60, 1, 0, 0, "Rock Slide", "Shadow Ball", "Ice Punch", "Double Edge");

    addPokemon(330, "Flygon", 80, 100, 80, 80, 80, 100, 7, 17, 1, "Steel Wing", "Earthquake", "Draco Meteor", "Stone Edge");

    addPokemon(332, "Cacturne", 70, 115, 60, 115, 60, 55, 2, 16, 0, "Needle Arm", "Pin Missile", "Focus Blast", "Dark Pulse");

    addPokemon(334, "Altaria", 75, 70, 90, 70, 105, 80, 17, 13, 0, "Dragon Pulse", "Steel Wing", "Fire Blast", "Dazzling Gleam");

    addPokemon(335, "Zangoose", 73, 115, 60, 60, 60, 90, 1, 0, 0, "Shadow Claw", "Shadow Ball", "Fury Swipes", "Brick Break");

    addPokemon(336, "Seviper", 73, 100, 60, 100, 60, 65, 11, 0, 0, "Poison Tail", "Flamethrower", "Sludge Wave", "Crunch");

    addPokemon(337, "Lunatone", 90, 55, 65, 95, 85, 70, 8, 14, 1, "Psychic", "Moonblast", "Power Gem", "Explosion");

    addPokemon(338, "Solrock", 90, 95, 85, 55, 65, 70, 8, 14, 1, "Solar Beam", "Rock Blast", "Zen Headbutt", "Explosion");

    addPokemon(340, "Whiscash", 110, 78, 73, 76, 71, 60, 4, 7, 0, "Waterfall", "Earth Power", "Bulldoze", "Hydro Pump");

    addPokemon(342, "Crawdaunt", 63, 120, 85, 90, 55, 55, 4, 16, 0, "Hydro Pump", "Night Slash", "Crabhammer", "Close Combat");

    addPokemon(344, "Claydol", 60, 70, 105, 70, 120, 75, 7, 14, 1, "Drill Run", "Shadow Ball", "Extrasensory", "Rock Tomb");

    addPokemon(346, "Cradily", 86, 81, 97, 81, 107, 43, 8, 2, 0, "Earth Power", "Bullet Seed", "Stone Edge", "Sludge Wave");

    addPokemon(348, "Armaldo", 75, 125, 100, 70, 80, 45, 8, 12, 0, "X-Scissor", "Stone Edge", "Earthquake", "Energy Ball");

    addPokemon(350, "Milotic", 95, 60, 79, 100, 125, 81, 4, 0, 0, "Surf", "Ice Beam", "Dragon Tail", "Disarming Voice");

    addPokemon(354, "Banette", 64, 115, 65, 83, 63, 65, 15, 0, 0, "Sucker Punch", "Shadow Claw", "Thunderbolt", "Dazzling Gleam");

    addPokemon(356, "Dusclops", 40, 70, 130, 60, 130, 25, 15, 0, 0, "Shadow Punch", "Fire Punch", "Brick Break", "Shadow Ball");

    addPokemon(357, "Tropius", 99, 68, 83, 72, 87, 51, 2, 13, 0, "Leaf Storm", "Earthquake", "Solar Beam", "Aerial Ace");

    addPokemon(358, "Chimecho", 75, 50, 80, 95, 90, 65, 14, 0, 1, "Psychic", "Shadow Ball", "Zen Headbutt", "Dazzling Gleam");

    addPokemon(359, "Absol", 65, 130, 60, 75, 60, 75, 16, 0, 0, "Night Slash", "Stone Edge", "Air Slash", "Giga Impact");

    addPokemon(362, "Glalie", 80, 80, 80, 80, 80, 80, 6, 0, 1, "Ice Beam", "Crunch", "Earthquake", "Iron Head");

    addPokemon(365, "Walrein", 110, 80, 90, 95, 90, 65, 6, 4, 0, "Surf", "Avalanche", "Earthquake", "Body Slam");

    addPokemon(367, "Huntail", 55, 104, 105, 94, 75, 52, 4, 0, 0, "Waterfall", "Ice Beam", "Crunch", "Iron Tail");

    addPokemon(368, "Gorebyss", 55, 84, 105, 114, 75, 52, 4, 0, 0, "Aqua Tail", "Hydro Pump", "Ice Beam", "Psychic");

    addPokemon(369, "Relicanth", 100, 90, 130, 45, 65, 55, 4, 8, 0, "Head Smash", "Aqua Tail", "Double Edge", "Earth Power");

    addPokemon(373, "Salamence", 95, 135, 80, 110, 80, 100, 17, 13, 0, "Flamethrower", "Air Slash", "Steel Wing", "Dragon Claw");

    addPokemon(376, "Metagross", 80, 135, 130, 95, 90, 70, 9, 14, 1, "Meteor Mash", "Hammer Arm", "Psychic", "Earthquake");

    addPokemon(389, "Torterra", 95, 109, 105, 75, 85, 56, 2, 7, 0, "Wood Hammer", "Frenzy Plant", "Earthquake", "Stone Edge");

    addPokemon(392, "Infernape", 76, 104, 71, 104, 71, 108, 3, 10, 0, "Flamethrower", "Close Combat", "Flare Blitz", "Solar Beam");

    addPokemon(395, "Empoleon", 84, 86, 88, 111, 101, 60, 4, 9, 0, "Drill Peck", "Steel Wing", "Hydro Pump", "Bulldoze");

    addPokemon(398, "Staraptor", 85, 120, 70, 50, 60, 100, 1, 13, 0, "Brave Bird", "Air Cutter", "Close Combat", "Giga Impact");

    addPokemon(400, "Bibarel", 79, 85, 60, 55, 60, 71, 1, 4, 0, "Water Gun", "Crunch", "Ice Beam", "Hyper Fang");

    addPokemon(402, "Kricketune", 77, 85, 51, 55, 51, 65, 12, 0, 0, "Bug Bite", "Night Slash", "Brick Break", "Bug Buzz");

    addPokemon(405, "Luxray", 80, 120, 79, 95, 79, 70, 5, 0, 0, "Wild Charge", "Thunder", "Night Slash", "Ice Fang");

    addPokemon(407, "Roserade", 60, 70, 65, 125, 105, 90, 2, 11, 0, "Poison Jab", "Solar Beam", "Shadow Ball", "Power Gem");

    addPokemon(409, "Rampardos", 97, 165, 60, 65, 50, 58, 8, 0, 0, "Headbutt", "Zen Headbutt", "Earthquake", "Head Smash");

    addPokemon(411, "Bastiodon", 60, 52, 168, 47, 138, 30, 8, 9, 0, "Iron Head", "Thunder", "Earthquake", "Stone Edge");

    addPokemon(414, "Mothim", 70, 94, 50, 94, 50, 66, 12, 13, 0, "Bug Bite", "Air Slash", "Energy Ball", "Confusion");

    addPokemon(416, "Vespiquen", 70, 80, 102, 80, 102, 40, 12, 13, 0, "Air Slash", "Attack Order", "Sludge Wave", "Power Gem");

    addPokemon(417, "Pachirisu", 60, 45, 70, 45, 90, 95, 5, 0, 0, "Spark", "Discharge", "Iron Tail", "Hyper Fang");

    addPokemon(419, "Floatzel", 85, 105, 55, 85, 50, 115, 4, 0, 0, "Aqua Tail", "Hydro Pump", "Ice Punch", "Brick Break");

    addPokemon(423, "Gastrodon", 111, 83, 68, 92, 82, 39, 4, 7, 0, "Water Pulse", "Body Slam", "Ice Beam", "Earth Power");

    addPokemon(424, "Ambipom", 75, 100, 66, 60, 66, 115, 1, 0, 0, "Swift", "Acrobatics", "Shadow Claw", "Focus Punch");

    addPokemon(426, "Drifblim", 150, 80, 44, 90, 54, 80, 15, 13, 0, "Shadow Ball", "Hyper Beam", "Air Cutter", "Giga Impact");

    addPokemon(428, "Lopunny", 65, 76, 84, 54, 96, 105, 1, 0, 0, "Mega Kick", "Focus Blast", "Shadow Ball", "Play Rough");

    addPokemon(429, "Mismagius", 60, 60, 60, 105, 105, 105, 15, 0, 1, "Shadow Ball", "Aerial Ace", "Magical Leaf", "Dazzling Gleam");

    addPokemon(430, "Honchkrow", 100, 125, 52, 105, 52, 71, 16, 13, 0, "Brave Bird", "Night Slash", "Dark Pulse", "Giga Impact");

    addPokemon(432, "Purugly", 71, 82, 64, 64, 59, 112, 1, 0, 0, "Fury Swipes", "Hyper Beam", "Body Slam", "Play Rough");

    addPokemon(435, "Skuntank", 103, 93, 67, 71, 61, 84, 11, 16, 0, "Sludge Bomb", "Crunch", "Flamethrower", "Play Rough");

    addPokemon(437, "Bronzong", 67, 89, 116, 79, 116, 33, 9, 14, 1, "Iron Head", "Psychic", "Zen Headbutt", "Earthquake");

    addPokemon(441, "Chatot", 76, 65, 45, 92, 42, 91, 1, 13, 0, "Air Cutter", "Steel Wing", "Boomburst", "Aerial Ace");

    addPokemon(442, "Spiritomb", 50, 92, 108, 92, 108, 35, 15, 16, 1, "Shadow Ball", "Feint Attack", "Rock Tomb", "Hyper Beam");

    addPokemon(445, "Garchomp", 108, 130, 95, 80, 85, 102, 17, 7, 0, "Poison Jab", "Earthquake", "Brick Break", "Dragon Rush");

    addPokemon(448, "Lucario", 70, 110, 70, 115, 70, 90, 10, 9, 0, "Aura Sphere", "Flash Cannon", "Ice Punch", "Stone Edge");

    addPokemon(450, "Hippowdon", 108, 112, 118, 68, 72, 47, 7, 0, 0, "Earthquake", "Stone Edge", "Crunch", "Thunder Fang");

    addPokemon(452, "Drapion", 70, 90, 110, 60, 75, 95, 11, 16, 0, "Pin Missile", "Cross Poison", "Dark Pulse", "Ice Fang");

    addPokemon(454, "Toxicroak", 83, 106, 65, 86, 65, 85, 11, 10, 0, "Gunk Shot", "Close Combat", "Dark Pulse", "Ice Punch");

    addPokemon(455, "Carnivine", 74, 100, 72, 90, 72, 46, 2, 0, 1, "Crunch", "Bullet Seed", "Vine Whip", "Sludge Bomb");

    addPokemon(457, "Lumineon", 69, 69, 76, 69, 86, 91, 4, 0, 0, "Ice Beam", "Signal Beam", "Waterfall", "Air Cutter");

    addPokemon(460, "Abomasnow", 90, 92, 75, 92, 85, 60, 2, 6, 0, "Wood Hammer", "Ice Punch", "Rock Slide", "Blizzard");

    addPokemon(461, "Weavile", 70, 120, 65, 45, 85, 125, 16, 6, 0, "Icicle Spear", "Metal Claw", "Focus Blast", "Night Slash");

    addPokemon(462, "Magnezone", 70, 70, 115, 130, 90, 60, 5, 9, 1, "Thunderbolt", "Flash Cannon", "Extrasensory", "Earthquake");

    addPokemon(463, "Lickilicky", 110, 85, 95, 80, 95, 50, 1, 0, 0, "Lick", "Power Whip", "Focus Blast", "Hyper Beam");

    addPokemon(464, "Rhyperior", 115, 140, 130, 55, 55, 40, 7, 8, 0, "Rock Wrecker", "Hammer Arm", "Earthquake", "Megahorn");

    addPokemon(465, "Tangrowth", 100, 100, 125, 110, 50, 50, 2, 0, 0, "Vine Whip", "Solar Beam", "Ancient Power", "Brick Break");

    addPokemon(466, "Electivire", 75, 123, 67, 95, 85, 95, 5, 0, 0, "Thunder Punch", "Thunder", "Giga Impact", "Brick Break");

    addPokemon(467, "Magmortar", 75, 95, 67, 125, 95, 83, 3, 0, 0, "Flamethrower", "Overheat", "Rock Tomb", "Solar Beam");

    addPokemon(468, "Togekiss", 85, 50, 95, 120, 115, 80, 18, 13, 0, "Aura Sphere", "Aerial Ace", "Dazzling Gleam", "Shadow Ball");

    addPokemon(469, "Yanmega", 86, 76, 86, 116, 56, 95, 12, 13, 0, "Air Cutter", "Bug Buzz", "Ancient Power", "Dragon Rush");

    addPokemon(470, "Leafeon", 65, 110, 130, 60, 65, 95, 2, 0, 0, "Razor Leaf", "X-Scissor", "Iron Tail", "Leaf Storm");

    addPokemon(471, "Glaceon", 65, 60, 110, 130, 95, 65, 6, 0, 0, "Ice Fang", "Blizzard", "Shadow Ball", "Aqua Tail");

    addPokemon(472, "Gliscor", 75, 95, 125, 45, 75, 95, 7, 13, 0, "Acrobatics", "Stone Edge", "Earthquake", "Night Slash");

    addPokemon(473, "Mamoswine", 110, 130, 80, 70, 60, 80, 6, 7, 0, "Avalanche", "Ancient Power", "Earthquake", "Iron Head");

    addPokemon(474, "Porygon-Z", 85, 80, 70, 135, 75, 90, 1, 0, 1, "Zap Cannon", "Zen Headbutt", "Hyper Beam", "Blizzard");

    addPokemon(475, "Gallade", 68, 125, 65, 65, 115, 80, 14, 10, 0, "Psycho Cut", "Close Combat", "Night Slash", "Stone Edge");

    addPokemon(476, "Probopass", 60, 55, 145, 75, 150, 40, 8, 9, 0, "Power Gem", "Flash Cannon", "Discharge", "Giga Impact");

    addPokemon(477, "Dusknoir", 45, 100, 135, 65, 135, 45, 15, 0, 1, "Shadow Punch", "Focus Blast", "Shadow Ball", "Rock Tomb");

    addPokemon(478, "Froslass", 70, 80, 70, 80, 70, 110, 6, 15, 1, "Blizzard", "Shadow Ball", "Icicle Spear", "Thunder");
    addDexentry("Froslass", "Froslass, the Snow Land Pokemon. It is the evolved form of Snorunt.\nLegends in snowy regions say that after a woman lost on a snowy mountain met her end, her soul possessed an icicle and her regrets lingered on. From them, this Pokemon was born. Now when it finds humans or Pokemon it likes, it freezes them and takes them to its chilly den, where they become decorations. The food it most relishes is the souls of men.");

    addPokemon(497, "Serperior", 75, 75, 95, 75, 95, 113, 2, 0, 0, "Vine Whip", "Leaf Storm", "Dragon Tail", "Aerial Ace");

    addPokemon(500, "Emboar", 110, 123, 65, 100, 65, 65, 3, 10, 0, "Head Smash", "Wild Charge", "Hammer Arm", "Fire Blast");

    addPokemon(503, "Samurott", 95, 100, 85, 108, 70, 70, 4, 0, 0, "Razor Shell", "Ice Beam", "Dragon Tail", "Megahorn");

    addPokemon(505, "Watchog", 60, 85, 69, 60, 69, 77, 1, 0, 0, "Bite", "Hyper Fang", "Bullet Seed", "Flamethrower");

    addPokemon(508, "Stoutland", 85, 110, 90, 45, 90, 80, 1, 0, 0, "Bite", "Ice Fang", "Play Rough", "Hyper Beam");

    addPokemon(510, "Liepard", 64, 88, 50, 88, 50, 106, 16, 0, 0, "Dark Pulse", "Shadow Claw", "Play Rough", "Aerial Ace");

    addPokemon(512, "Simisage", 75, 98, 63, 98, 63, 101, 2, 0, 0, "Bullet Seed", "Energy Ball", "Rock Slide", "Iron Tail");

    addPokemon(514, "Simisear", 75, 98, 63, 98, 63, 101, 3, 0, 0, "Flamethrower", "Focus Blast", "Fire Punch", "Gunk Shot");

    addPokemon(516, "Simipour", 75, 98, 63, 98, 63, 101, 4, 0, 0, "Water Gun", "Aqua Tail", "Ice Beam", "Rock Slide");

    addPokemon(518, "Musharna", 116, 55, 85, 107, 95, 29, 14, 0, 1, "Psychic", "Zen Headbutt", "Moonblast", "Signal Beam");

    addPokemon(521, "Unfezant", 80, 115, 80, 65, 55, 93, 1, 13, 0, "Wing Attack", "Air Slash", "Double Edge", "Steel Wing");

    addPokemon(523, "Zebstrika", 75, 100, 63, 80, 63, 116, 5, 0, 0, "Thunderbolt", "Overheat", "Double Edge", "Low Sweep");

    addPokemon(526, "Gigalith", 85, 135, 130, 60, 80, 25, 8, 0, 0, "Stone Edge", "Earthquake", "Power Gem", "Solar Beam");

    addPokemon(528, "Swoobat", 67, 57, 55, 77, 55, 114, 14, 13, 0, "Psycho Cut", "Air Cutter", "Confusion", "Shadow Ball");

    addPokemon(530, "Excadrill", 110, 135, 60, 50, 65, 88, 7, 9, 0, "Iron Head", "Drill Run", "Sludge Bomb", "Shadow Claw");

    addPokemon(531, "Audino", 103, 60, 86, 60, 86, 50, 1, 0, 0, "Zen Headbutt", "Dazzling Gleam", "Pound", "Double Edge");

    addPokemon(534, "Conkeldurr", 105, 140, 95, 55, 65, 45, 10, 0, 0, "Hammer Arm", "Earthquake", "Ice Punch", "Rock Blast");

    addPokemon(537, "Seismitoad", 105, 95, 75, 85, 75, 74, 4, 7, 0, "Bubble Beam", "Earth Power", "Boomburst", "Power Whip");

    addPokemon(538, "Throh", 120, 100, 85, 30, 85, 45, 10, 0, 0, "Vital Throw", "Ice Punch", "Rock Slide", "Seismic Toss");

    addPokemon(539, "Sawk", 75, 125, 75, 30, 75, 85, 10, 0, 0, "Karate Chop", "Poison Jab", "Rock Slide", "Low Sweep");

    addPokemon(542, "Leavanny", 75, 103, 80, 70, 80, 92, 12, 2, 0, "Leaf Blade", "Shadow Claw", "X-Scissor", "Bug Buzz");

    addPokemon(545, "Scolipede", 60, 100, 89, 55, 69, 112, 12, 11, 0, "Poison Tail", "Rock Slide", "Sludge Bomb", "Megahorn");

    addPokemon(547, "Whimsicott", 60, 67, 85, 77, 75, 116, 2, 18, 0, "Razor Leaf", "Energy Ball", "Play Rough", "Dazzling Gleam");

    addPokemon(549, "Lilligant", 70, 60, 75, 110, 75, 90, 2, 0, 0, "Energy Ball", "Hyper Beam", "Magical Leaf", "Giga Impact");

    addPokemon(550, "Basculin", 70, 92, 65, 80, 55, 98, 4, 0, 0, "Bite", "Water Gun", "Headbutt", "Double Edge");

    addPokemon(553, "Krookodile", 95, 117, 80, 65, 70, 92, 7, 16, 0, "Earthquake", "Dark Pulse", "Dragon Claw", "Sludge Bomb");

    addPokemon(556, "Maractus", 75, 86, 67, 106, 67, 60, 2, 0, 0, "Needle Arm", "Energy Ball", "Poison Jab", "Pin Missile");

    addPokemon(558, "Crustle", 70, 105, 125, 65, 75, 45, 12, 8, 0, "Rock Blast", "X-Scissor", "Earthquake", "Hyper Beam");

    addPokemon(560, "Scrafty", 65, 90, 115, 45, 115, 58, 16, 10, 0, "Head Smash", "Brick Break", "Sucker Punch", "Sludge Bomb");

    addPokemon(561, "Sigilyph", 72, 58, 80, 103, 80, 97, 14, 13, 0, "Air Slash", "Ice Beam", "Confusion", "Shadow Ball");

    addPokemon(563, "Cofagrigus", 58, 50, 145, 95, 105, 30, 15, 0, 0, "Shadow Ball", "Energy Ball", "Confusion", "Giga Impact");

    addPokemon(565, "Carracosta", 74, 108, 133, 83, 65, 32, 4, 8, 0, "Bite", "Razor Shell", "Rock Slide", "Earth Power");

    addPokemon(569, "Garbodor", 80, 95, 82, 60, 82, 75, 11, 0, 0, "Pound", "Sludge Bomb", "Gunk Shot", "Dark Pulse");

    addPokemon(571, "Zoroark", 60, 105, 60, 120, 60, 105, 16, 0, 0, "Night Slash", "Dark Pulse", "Extrasensory", "Sludge Bomb");

    addPokemon(573, "Cinccino", 75, 95, 60, 65, 60, 115, 1, 0, 0, "Pound", "Rock Blast", "Aqua Tail", "Thunderbolt");

    addPokemon(576, "Gothitelle", 70, 55, 95, 95, 110, 65, 14, 0, 0, "Thunderbolt", "Shadow Ball", "Rock Slide", "Confusion");

    addPokemon(579, "Reuniclus", 110, 65, 75, 125, 85, 30, 14, 0, 1, "Confusion", "Fire Punch", "Thunder", "Shadow Ball");

    addPokemon(581, "Swanna", 75, 87, 63, 87, 63, 98, 4, 13, 0, "Wing Attack", "Air Cutter", "Bubble Beam", "Ice Beam");

    addPokemon(584, "Vanilluxe", 71, 95, 85, 110, 95, 79, 6, 0, 1, "Ice Shard", "Flash Cannon", "Signal Beam", "Blizzard");

    addPokemon(587, "Emolga", 55, 75, 60, 75, 60, 103, 5, 13, 0, "Spark", "Aerial Ace", "Discharge", "Signal Beam");

    addPokemon(589, "Escavalier", 70, 135, 105, 60, 105, 20, 12, 9, 1, "Iron Head", "Double Edge", "Aerial Ace", "Megahorn");

    addPokemon(591, "Amoonguss", 114, 85, 70, 85, 80, 30, 2, 11, 0, "Body Slam", "Feint Attack", "Sludge Bomb", "Energy Ball");

    addPokemon(593, "Jellicent", 100, 60, 70, 85, 105, 60, 4, 15, 0, "Waterfall", "Sludge Bomb", "Shadow Ball", "Ice Beam");

    addPokemon(594, "Alomomola", 165, 75, 80, 40, 45, 65, 4, 0, 0, "Waterfall", "Ice Beam", "Shadow Ball", "Hydro Pump");

    addPokemon(596, "Galvantula", 70, 77, 60, 97, 60, 108, 12, 5, 0, "Bug Buzz", "Thunder", "Energy Ball", "X-Scissor");

    addPokemon(598, "Ferrothorn", 74, 94, 131, 54, 116, 20, 2, 9, 0, "Power Whip", "Metal Claw", "Pin Missile", "Shadow Claw");

    addPokemon(601, "Klinklang", 60, 100, 115, 70, 85, 90, 9, 0, 1, "Gear Grind", "Zap Cannon", "Flash Cannon", "Signal Beam");

    addPokemon(604, "Eelektross", 85, 115, 80, 105, 80, 50, 5, 0, 1, "Thunderbolt", "Zap Cannon", "Crunch", "Dragon Claw");

    addPokemon(606, "Beheeyem", 75, 75, 75, 125, 95, 40, 14, 0, 0, "Confusion", "Shadow Ball", "Rock Tomb", "Headbutt");

    addPokemon(609, "Chandelure", 60, 55, 90, 145, 90, 80, 15, 3, 1, "Shadow Ball", "Solar Beam", "Overheat", "Hyper Beam");

    addPokemon(612, "Haxorus", 76, 147, 90, 60, 70, 97, 17, 0, 0, "Dragon Pulse", "Close Combat", "Earthquake", "Aqua Tail");

    addPokemon(614, "Beartic", 95, 130, 80, 70, 80, 50, 6, 0, 0, "Icicle Spear", "Blizzard", "Brick Break", "Bulldoze");

    addPokemon(615, "Cryogonal", 80, 50, 50, 95, 135, 105, 6, 0, 1, "Ice Shard", "Ancient Power", "Signal Beam", "Blizzard");

    addPokemon(617, "Accelgor", 80, 70, 40, 100, 60, 145, 12, 0, 0, "Bug Buzz", "Water Shuriken", "Sludge Bomb", "Focus Blast");

    addPokemon(618, "Stunfisk", 109, 66, 84, 81, 99, 32, 7, 5, 0, "Aqua Tail", "Discharge", "Earth Power", "Sludge Bomb");

    addPokemon(620, "Mienshao", 65, 125, 60, 95, 60, 105, 10, 0, 0, "Poison Jab", "Aura Sphere", "Stone Edge", "Acrobatics");

    addPokemon(621, "Druddigon", 77, 120, 90, 60, 90, 48, 17, 0, 0, "Thunder Fang", "Dragon Claw", "Poison Tail", "Dark Pulse");

    addPokemon(623, "Golurk", 89, 124, 80, 55, 80, 55, 7, 15, 0, "Shadow Punch", "Dynamic Punch", "Earthquake", "Hyper Beam");

    addPokemon(625, "Bisharp", 65, 125, 100, 60, 70, 70, 16, 9, 0, "Iron Head", "Night Slash", "Brick Break", "Stone Edge");

    addPokemon(626, "Bouffalant", 95, 110, 95, 40, 95, 55, 1, 0, 0, "Horn Attack", "Bulldoze", "Megahorn", "Wild Charge");

    addPokemon(628, "Braviary", 100, 123, 75, 57, 75, 80, 1, 13, 0, "Brave Bird", "Air Slash", "Close Combat", "Giga Impact");

    addPokemon(630, "Mandibuzz", 110, 65, 105, 55, 95, 80, 16, 13, 0, "Bone Rush", "Dark Pulse", "Air Slash", "Steel Wing");

    addPokemon(631, "Heatmor", 85, 97, 66, 105, 66, 65, 3, 0, 0, "Lick", "Flamethrower", "Bug Bite", "Body Slam");

    addPokemon(632, "Durant", 58, 109, 112, 48, 48, 109, 12, 9, 0, "Bug Bite", "Rock Tomb", "Flash Cannon", "Crunch");

    addPokemon(635, "Hydreigon", 92, 105, 90, 125, 90, 98, 16, 17, 1, "Bite", "Dragon Pulse", "Acrobatics", "Fire Blast");

    addPokemon(637, "Volcarona", 85, 60, 65, 135, 105, 100, 12, 3, 1, "Bug Buzz", "Flame Wheel", "Double Edge", "Hurricane");

    addPokemon(652, "Chesnaught", 88, 107, 122, 74, 75, 64, 2, 10, 0, "Wood Hammer", "Focus Blast", "Pin Missile", "Rock Tomb");

    addPokemon(655, "Delphox", 75, 69, 72, 114, 100, 104, 3, 14, 0, "Fire Punch", "Overheat", "Psychic", "Shadow Ball");

    addPokemon(658, "Greninja", 72, 95, 67, 103, 71, 122, 4, 16, 0, "Water Shuriken", "Night Slash", "Hydro Cannon", "Gunk Shot");

    addPokemon(660, "Diggersby", 85, 56, 77, 50, 77, 78, 1, 7, 0, "Take Down", "Bulldoze", "Hammer Arm", "Hyper Beam");

    addPokemon(663, "Talonflame", 78, 81, 71, 74, 69, 126, 3, 13, 0, "Brave Bird", "Steel Wing", "Flare Blitz", "Solar Beam");

    addPokemon(666, "Vivilion", 80, 52, 50, 90, 50, 89, 12, 13, 0, "Bug Bite", "Signal Beam", "Aerial Ace", "Energy Ball");

    addPokemon(668, "Pyroar", 86, 68, 72, 109, 66, 106, 3, 1, 0, "Fire Fang", "Overheat", "Wild Charge", "Hyper Beam");

    addPokemon(671, "Florges", 78, 65, 68, 112, 154, 75, 18, 0, 0, "Magical Leaf", "Moonblast", "Solar Beam", "Psychic");

    addPokemon(673, "Gogoat", 123, 100, 62, 97, 81, 68, 2, 0, 0, "Solar Beam", "Rock Slide", "Bulldoze", "Wild Charge");

    addPokemon(675, "Pangoro", 95, 124, 78, 69, 71, 58, 10, 16, 0, "Karate Chop", "Zen Headbutt", "Sludge Bomb", "Night Slash");

    addPokemon(676, "Furfrou", 75, 80, 60, 65, 90, 102, 1, 0, 0, "Tackle", "Bite", "Wild Charge", "Dark Pulse");

    addPokemon(683, "Aromatisse", 101, 72, 72, 99, 89, 29, 18, 0, 0, "Disarming Voice", "Confusion", "Dazzling Gleam", "Energy Ball");

    addPokemon(685, "Slurpuff", 82, 80, 86, 85, 75, 72, 18, 0, 0, "Play Rough", "Dazzling Gleam", "Flamethrower", "Confusion");

    addPokemon(687, "Malamar", 86, 92, 88, 68, 75, 73, 16, 14, 0, "Psycho Cut", "Night Slash", "Flamethrower", "Dark Pulse");

    addPokemon(689, "Barbaracle", 72, 105, 115, 54, 86, 68, 8, 4, 0, "Water Gun", "Razor Shell", "Stone Edge", "Dragon Claw");

    addPokemon(691, "Dragalge", 65, 75, 90, 97, 123, 44, 11, 17, 0, "Sludge Bomb", "Hydro Pump", "Dragon Tail", "Shadow Ball");

    addPokemon(693, "Clawitzer", 71, 73, 88, 120, 89, 59, 4, 0, 0, "Hydro Pump", "Dragon Pulse", "Crabhammer", "Ice Beam");

    addPokemon(695, "Heliolisk", 62, 55, 52, 109, 94, 109, 5, 1, 0, "Thunderbolt", "Dragon Pulse", "Giga Impact", "Energy Ball");

    addPokemon(697, "Tyrantrum", 82, 121, 119, 69, 59, 71, 8, 17, 0, "Crunch", "Stone Edge", "Earthquake", "Draco Meteor");

    addPokemon(699, "Aurorus", 123, 77, 72, 99, 92, 58, 8, 6, 0, "Rock Blast", "Blizzard", "Psychic", "Hyper Beam");

    addPokemon(700, "Sylveon", 95, 65, 65, 110, 130, 60, 18, 0, 0, "Disarming Voice", "Double Edge", "Moonblast", "Shadow Ball");

    addPokemon(701, "Hawlucha", 78, 92, 75, 74, 63, 118, 10, 13, 0, "Karate Chop", "Aerial Ace", "X-Scissor", "Flying Press");

    addPokemon(702, "Dedenne", 67, 58, 57, 81, 67, 101, 5, 18, 0, "Discharge", "Dazzling Gleam", "Signal Beam", "Play Rough");

    addPokemon(703, "Carbink", 50, 50, 150, 50, 150, 50, 8, 18, 0, "Power Gem", "Moonblast", "Rock Slide", "Confusion");

    addPokemon(706, "Goodra", 90, 100, 70, 110, 150, 80, 17, 0, 0, "Dragon Tail", "Flamethrower", "Sludge Wave", "Earthquake");

    addPokemon(707, "Klefki", 57, 80, 91, 80, 87, 75, 9, 18, 1, "Flash Cannon", "Play Rough", "Dazzling Gleam", "Feint Attack");

    addPokemon(709, "Trevenant", 85, 110, 76, 65, 82, 56, 15, 2, 0, "Wood Hammer", "Shadow Claw", "Rock Slide", "Leaf Storm");

    addPokemon(713, "Avalugg", 95, 117, 184, 44, 46, 28, 6, 0, 0, "Icicle Spear", "Avalanche", "Earthquake", "Flash Cannon");

    addPokemon(715, "Noivern", 85, 70, 80, 97, 80, 123, 13, 17, 0, "Boomburst", "Acrobatics", "Focus Blast", "Dragon Claw");

    addPokemon(724, "Decidueye", 78, 107, 75, 100, 100, 70, 2, 15, 1, "Razor Leaf", "Brave Bird", "Shadow Claw", "Leaf Storm");

    addPokemon(727, "Incineroar", 95, 115, 90, 80, 90, 60, 3, 16, 0, "Fire Blast", "Poison Jab", "Close Combat", "Crunch");

    addPokemon(730, "Primarina", 80, 74, 74, 126, 116, 60, 4, 18, 0, "Bubble Beam", "Iron Tail", "Moonblast", "Hydro Cannon");

    addPokemon(733, "Toucannon", 80, 120, 75, 75, 75, 60, 1, 13, 0, "Drill Peck", "Bullet Seed", "Brick Break", "Overheat");

    addPokemon(735, "Gumshoos", 88, 110, 60, 55, 60, 45, 1, 0, 0, "Bite", "Hyper Fang", "Shock Wave", "Ice Fang");

    addPokemon(738, "Vikavolt", 77, 70, 90, 145, 75, 43, 12, 5, 1, "Bug Bite", "Thunderbolt", "Zap Cannon", "Energy Ball");

    addPokemon(740, "Crabominable", 97, 132, 77, 62, 67, 43, 10, 6, 0, "Bubble Beam", "Ice Punch", "Dynamic Punch", "Avalanche");

    addPokemon(743, "Ribombee", 60, 55, 60, 95, 70, 124, 12, 18, 1, "Bug Buzz", "Dazzling Gleam", "Magical Leaf", "Psychic");

    addPokemon(748, "Toxapex", 50, 63, 152, 53, 142, 35, 11, 4, 0, "Waterfall", "Ice Beam", "Gunk Shot", "Pin Missile");

    addPokemon(750, "Mudsdale", 100, 125, 100, 55, 85, 35, 7, 0, 0, "Earthquake", "Close Combat", "Earth Power", "Giga Impact");

    addPokemon(752, "Araquanid", 68, 70, 92, 50, 132, 42, 4, 12, 0, "Bug Bite", "Bubble Beam", "Headbutt", "Crunch");

    addPokemon(754, "Lurantis", 70, 105, 90, 80, 90, 45, 2, 0, 0, "Leaf Blade", "Solar Beam", "Psycho Cut", "Brick Break");

    addPokemon(756, "Shiinotic", 60, 45, 80, 90, 100, 30, 2, 18, 0, "Energy Ball", "Moonblast", "Sludge Bomb", "Shadow Claw");

    addPokemon(758, "Salazzle", 68, 64, 60, 111, 60, 117, 11, 3, 0, "Sludge Wave", "Fire Blast", "Dragon Pulse", "Gunk Shot");

    addPokemon(760, "Bewear", 120, 125, 80, 55, 60, 60, 1, 10, 0, "Hammer Arm", "Double Edge", "Focus Blast", "Dragon Claw");

    addPokemon(763, "Tsareena", 72, 120, 98, 50, 98, 72, 2, 0, 0, "Razor Leaf", "Magical Leaf", "Leaf Storm", "Mega Kick");

    addPokemon(764, "Comfey", 51, 52, 90, 82, 110, 100, 18, 0, 1, "Acrobatics", "Dazzling Gleam", "Play Rough", "Magical Leaf");

    addPokemon(765, "Oranguru", 90, 60, 80, 90, 110, 60, 1, 14, 0, "Zen Headbutt", "Confusion", "Rock Slide", "Hyper Beam");

    addPokemon(766, "Passimian", 100, 120, 90, 40, 60, 80, 10, 0, 0, "Double Edge", "Low Sweep", "Focus Blast", "Acrobatics");

    addPokemon(768, "Golisopod", 75, 125, 140, 60, 90, 40, 12, 4, 0, "Poison Jab", "X-Scissor", "Razor Shell", "Ice Beam");

    addPokemon(770, "Palossand", 85, 75, 110, 100, 75, 35, 15, 7, 0, "Earth Power", "Shadow Ball", "Bulldoze", "Sludge Bomb");

    addPokemon(776, "Turtonator", 60, 78, 135, 91, 85, 36, 3, 17, 0, "Dragon Pulse", "Overheat", "Stone Edge", "Explosion");

    addPokemon(777, "Togedemaru", 65, 98, 63, 40, 73, 96, 9, 0, 0, "Iron Head", "Pin Missile", "Spark", "Disarming Voice");

    addPokemon(779, "Bruxish", 68, 105, 70, 70, 70, 92, 4, 14, 0, "Waterfall", "Aqua Tail", "Crunch", "Confusion");

    addPokemon(780, "Drampa", 78, 60, 85, 135, 91, 36, 1, 17, 0, "Dragon Breath", "Hyper Beam", "Extrasensory", "Bulldoze");

    addPokemon(781, "Dhelmise", 70, 131, 100, 86, 90, 40, 15, 2, 0, "Energy Ball", "Shadow Claw", "Brick Break", "Rock Slide");

    addPokemon(784, "Kommo-o", 75, 110, 125, 100, 105, 85, 17, 10, 0, "Dragon Tail", "Close Combat", "Boomburst", "Shadow Claw");
}

void initmoves(void)
{
    addMove("Fury Swipes", 18, 100, 15, 1, 0, 0, 1);
    addMove("Pound", 40, 100, 35, 1, 0, 0, 0);
    addMove("Scratch", 40, 100, 35, 1, 0, 0, 0);
    addMove("Tackle", 40, 100, 35, 1, 0, 0, 0);
    addMove("Struggle", 50, 105, 1, 1, 0, 0, 0);
    addMove("Horn Attack", 65, 100, 25, 1, 0, 1, 0);
    addMove("Headbutt", 70, 100, 15, 1, 0, 0, 0);
    addMove("Hyper Fang", 80, 90, 15, 1, 0, 0, 0);
    addMove("Body Slam", 85, 100, 15, 1, 0, 0, 0);
    addMove("Take Down", 90, 85, 20, 1, 0, 0, 0);
    addMove("Double Edge", 105, 100, 15, 1, 0, 0, 0);
    addMove("Mega Kick", 120, 75, 10, 1, 0, 0, 0);
    addMove("Giga Impact", 150, 90, 5, 1, 0, 0, 0);
    addMove("Explosion", 250, 100, 5, 1, 0, 0, 0);
    addMove("Milk Drink", 0, 0, 10, 1, 1, 0, 0);
    addMove("Swift", 60, 105, 20, 1, 1, 0, 0);
    addMove("Boomburst", 140, 105, 10, 1, 1, 0, 0);
    addMove("Hyper Beam", 150, 90, 5, 1, 1, 0, 0);
    addMove("Bullet Seed", 25, 100, 30, 2, 0, 0, 1);
    addMove("Razor Leaf", 55, 95, 25, 2, 0, 1, 0);
    addMove("Vine Whip", 65, 100, 25, 2, 0, 0, 0);
    addMove("Needle Arm", 70, 100, 15, 2, 0, 0, 0);
    addMove("Leaf Blade", 90, 100, 15, 2, 0, 1, 0);
    addMove("Power Whip", 120, 85, 10, 2, 0, 0, 0);
    addMove("Wood Hammer", 120, 100, 15, 2, 0, 0, 0);
    addMove("Magical Leaf", 60, 105, 20, 2, 1, 0, 0);
    addMove("Energy Ball", 90, 105, 10, 2, 1, 0, 0);
    addMove("Solar Beam", 125, 100, 5, 2, 1, 0, 0);
    addMove("Leaf Storm", 130, 90, 5, 2, 1, 0, 0);
    addMove("Frenzy Plant", 150, 90, 5, 2, 1, 0, 0);
    addMove("Flame Wheel", 60, 100, 25, 3, 0, 0, 0);
    addMove("Fire Punch", 75, 100, 15, 3, 0, 0, 0);
    addMove("Fire Fang", 80, 95, 15, 3, 0, 0, 0);
    addMove("Blaze Kick", 85, 90, 10, 3, 0, 1, 0);
    addMove("Flare Blitz", 120, 100, 15, 3, 0, 0, 0);
    addMove("Flamethrower", 90, 100, 15, 3, 1, 0, 0);
    addMove("Fire Blast", 110, 85, 5, 3, 1, 0, 0);
    addMove("Overheat", 130, 90, 5, 3, 1, 0, 0);
    addMove("Blast Burn", 150, 90, 5, 3, 1, 0, 0);
    addMove("Splash", 0, 0, 40, 4, 0, 0, 0);
    addMove("Water Shuriken", 20, 100, 20, 4, 0, 1, 1);
    addMove("Waterfall", 80, 100, 15, 4, 0, 0, 0);
    addMove("Aqua Tail", 90, 90, 10, 4, 0, 0, 0);
    addMove("Razor Shell", 95, 95, 10, 4, 0, 0, 0);
    addMove("Crabhammer", 100, 90, 10, 4, 0, 1, 0);
    addMove("Water Gun", 50, 100, 25, 4, 1, 0, 0);
    addMove("Bubble Beam", 65, 100, 20, 4, 1, 0, 0);
    addMove("Water Pulse", 70, 100, 20, 4, 1, 0, 0);
    addMove("Surf", 80, 100, 15, 4, 1, 0, 0);
    addMove("Hydro Pump", 120, 80, 5, 4, 1, 0, 0);
    addMove("Hydro Cannon", 150, 90, 5, 4, 1, 0, 0);
    addMove("Spark", 65, 100, 20, 5, 0, 0, 0);
    addMove("Thunder Punch", 75, 100, 15, 5, 0, 0, 0);
    addMove("Thunder Fang", 80, 95, 15, 5, 0, 0, 0);
    addMove("Wild Charge", 90, 100, 15, 5, 0, 0, 0);
    addMove("Volt Tackle", 120, 100, 15, 5, 0, 0, 0);
    addMove("Shock Wave", 60, 105, 20, 5, 1, 0, 0);
    addMove("Discharge", 80, 100, 15, 5, 1, 0, 0);
    addMove("Thunderbolt", 90, 100, 15, 5, 1, 0, 0);
    addMove("Thunder", 110, 70, 10, 5, 1, 0, 0);
    addMove("Zap Cannon", 140, 50, 5, 5, 1, 0, 0);
    addMove("Icicle Spear", 25, 100, 30, 6, 0, 0, 1);
    addMove("Ice Shard", 50, 100, 30, 6, 0, 0, 0);
    addMove("Ice Punch", 75, 100, 15, 6, 0, 0, 0);
    addMove("Avalanche", 80, 100, 10, 6, 0, 0, 0);
    addMove("Ice Fang", 80, 95, 15, 6, 0, 0, 0);
    addMove("Ice Beam", 90, 100, 10, 6, 1, 0, 0);
    addMove("Blizzard", 110, 70, 5, 6, 1, 0, 0);
    addMove("Bone Rush", 25, 90, 10, 7, 0, 0, 1);
    addMove("Bulldoze", 70, 100, 20, 7, 0, 0, 0);
    addMove("Drill Run", 80, 95, 10, 7, 0, 1, 0);
    addMove("Earthquake", 100, 100, 10, 7, 0, 0, 0);
    addMove("Earth Power", 90, 100, 10, 7, 1, 0, 0);
    addMove("Rock Blast", 25, 90, 10, 8, 0, 0, 1);
    addMove("Rock Tomb", 60, 95, 15, 8, 0, 0, 0);
    addMove("Rock Slide", 75, 90, 10, 8, 0, 0, 0);
    addMove("Stone Edge", 100, 80, 5, 8, 0, 1, 0);
    addMove("Rock Wrecker", 150, 90, 5, 8, 0, 0, 0);
    addMove("Head Smash", 160, 80, 5, 8, 0, 0, 0);
    addMove("Ancient Power", 60, 100, 5, 8, 1, 0, 0);
    addMove("Power Gem", 80, 100, 20, 8, 1, 0, 0);
    addMove("Metal Claw", 60, 95, 35, 9, 0, 0, 0);
    addMove("Steel Wing", 70, 90, 25, 9, 0, 0, 0);
    addMove("Iron Head", 80, 100, 15, 9, 0, 0, 0);
    addMove("Meteor Mash", 90, 100, 10, 9, 0, 0, 0);
    addMove("Gear Grind", 100, 85, 15, 9, 0, 0, 0);
    addMove("Iron Tail", 110, 75, 15, 9, 0, 0, 0);
    addMove("Flash Cannon", 90, 100, 10, 9, 1, 0, 0);
    addMove("Karate Chop", 50, 100, 25, 10, 0, 1, 0);
    addMove("Low Sweep", 65, 100, 20, 10, 0, 0, 0);
    addMove("Vital Throw", 70, 105, 10, 10, 0, 0, 0);
    addMove("Brick Break", 75, 100, 15, 10, 0, 0, 0);
    addMove("Submission", 80, 80, 20, 10, 0, 0, 0);
    addMove("Flying Press", 100, 100, 10, 10, 0, 0, 0);
    addMove("Hammer Arm", 100, 90, 10, 10, 0, 0, 0);
    addMove("Seismic Toss", 100, 105, 20, 10, 0, 0, 0);
    addMove("Close Combat", 120, 100, 5, 10, 0, 0, 0);
    addMove("Dynamic Punch", 130, 50, 5, 10, 0, 0, 0);
    addMove("Focus Punch", 150, 100, 5, 10, 0, 0, 0);
    addMove("Aura Sphere", 80, 105, 20, 10, 1, 0, 0);
    addMove("Focus Blast", 120, 70, 5, 10, 1, 0, 0);
    addMove("Poison Tail", 60, 100, 25, 11, 0, 1, 0);
    addMove("Cross Poison", 70, 100, 20, 11, 0, 1, 0);
    addMove("Poison Jab", 80, 100, 20, 11, 0, 0, 0);
    addMove("Poison Fang", 85, 95, 15, 11, 0, 0, 0);
    addMove("Gunk Shot", 120, 80, 15, 11, 0, 0, 0);
    addMove("Sludge Bomb", 90, 100, 10, 11, 1, 0, 0);
    addMove("Sludge Wave", 95, 100, 10, 11, 1, 0, 0);
    addMove("Pin Missile", 25, 95, 20, 12, 0, 0, 1);
    addMove("Bug Bite", 60, 100, 20, 12, 0, 0, 0);
    addMove("X-Scissor", 80, 100, 15, 12, 0, 0, 0);
    addMove("Attack Order", 90, 100, 15, 12, 0, 1, 0);
    addMove("Megahorn", 120, 85, 10, 12, 0, 0, 0);
    addMove("Signal Beam", 75, 100, 15, 12, 1, 0, 0);
    addMove("Bug Buzz", 90, 100, 10, 12, 1, 0, 0);
    addMove("Wing Attack", 60, 100, 35, 13, 0, 0, 0);
    addMove("Drill Peck", 80, 100, 20, 13, 0, 0, 0);
    addMove("Aerial Ace", 90, 105, 15, 13, 0, 0, 0);
    addMove("Acrobatics", 110, 100, 15, 13, 0, 0, 0);
    addMove("Brave Bird", 120, 100, 15, 13, 0, 0, 0);
    addMove("Air Cutter", 60, 95, 25, 13, 1, 1, 0);
    addMove("Air Slash", 75, 95, 15, 13, 1, 0, 0);
    addMove("Hurricane", 110, 70, 10, 13, 1, 0, 0);
    addMove("Psycho Cut", 70, 100, 20, 14, 0, 1, 0);
    addMove("Zen Headbutt", 80, 90, 15, 14, 0, 0, 0);
    addMove("Confusion", 60, 100, 25, 14, 1, 0, 0);
    addMove("Extrasensory", 80, 100, 20, 14, 1, 0, 0);
    addMove("Psychic", 90, 100, 10, 14, 1, 0, 0);
    addMove("Lick", 50, 100, 30, 15, 0, 0, 0);
    addMove("Shadow Punch", 60, 105, 20, 15, 0, 0, 0);
    addMove("Shadow Claw", 70, 100, 15, 15, 0, 1, 0);
    addMove("Shadow Ball", 80, 100, 15, 15, 1, 0, 0);
    addMove("Bite", 60, 100, 25, 16, 0, 0, 0);
    addMove("Feint Attack", 60, 105, 20, 16, 0, 0, 0);
    addMove("Night Slash", 70, 100, 15, 16, 0, 1, 0);
    addMove("Crunch", 80, 100, 15, 16, 0, 0, 0);
    addMove("Sucker Punch", 90, 100, 5, 16, 0, 0, 0);
    addMove("Dark Pulse", 80, 100, 15, 16, 1, 0, 0);
    addMove("Dragon Tail", 60, 90, 20, 17, 0, 0, 0);
    addMove("Dragon Claw", 80, 100, 15, 17, 0, 0, 0);
    addMove("Dragon Rush", 100, 75, 10, 17, 0, 0, 0);
    addMove("Dragon Breath", 60, 100, 20, 17, 1, 0, 0);
    addMove("Dragon Pulse", 85, 100, 10, 17, 1, 0, 0);
    addMove("Draco Meteor", 130, 90, 5, 17, 1, 0, 0);
    addMove("Play Rough", 90, 90, 10, 18, 0, 0, 0);
    addMove("Disarming Voice", 50, 105, 15, 18, 1, 0, 0);
    addMove("Dazzling Gleam", 80, 100, 10, 18, 1, 0, 0);
    addMove("Moonblast", 95, 100, 15, 18, 1, 0, 0);
}

void initsound(void)
{
    music[1].openFromFile("audio/menu.ogg");
    music[1].setVolume(50);
    music[1].setLoop(true);
    music[1].setPlayingOffset(sf::seconds(music[1].getDuration().asSeconds()-1));

    music[0].openFromFile("audio/battle.ogg");
    music[0].setVolume(50);
    music[0].setLoop(true);
    music[0].setPlayingOffset(sf::seconds(music[0].getDuration().asSeconds()-1));
}

void changetrack(int id)
{
    music[!id].pause();
    music[id].pause();

    if(id)
    {
        music[0].setPlayingOffset(sf::seconds(music[0].getDuration().asSeconds()-1));

        if(music[1].getPlayingOffset().asMilliseconds()<119173) music[1].setPlayingOffset(sf::milliseconds(119173));
        else if(music[1].getPlayingOffset().asMilliseconds()<245188) music[1].setPlayingOffset(sf::milliseconds(245188));
        else if(music[1].getPlayingOffset().asMilliseconds()<336256) music[1].setPlayingOffset(sf::milliseconds(336225));
        else if(music[1].getPlayingOffset().asMilliseconds()<507256) music[1].setPlayingOffset(sf::milliseconds(507225));
        else if(music[1].getPlayingOffset().asMilliseconds()<691367) music[1].setPlayingOffset(sf::milliseconds(691336));
        else music[1].setPlayingOffset(sf::milliseconds(0));
    }

    else
    {
        if(music[0].getPlayingOffset().asMilliseconds()<106068) music[0].setPlayingOffset(sf::milliseconds(106068));
        else if(music[0].getPlayingOffset().asMilliseconds()<201907) music[0].setPlayingOffset(sf::milliseconds(201907));
        else if(music[0].getPlayingOffset().asMilliseconds()<300912) music[0].setPlayingOffset(sf::milliseconds(300912));
        else if(music[0].getPlayingOffset().asMilliseconds()<394588) music[0].setPlayingOffset(sf::milliseconds(394588));
        else if(music[0].getPlayingOffset().asMilliseconds()<453752) music[0].setPlayingOffset(sf::milliseconds(453752));
        else if(music[0].getPlayingOffset().asMilliseconds()<562282) music[0].setPlayingOffset(sf::milliseconds(562282));
        else if(music[0].getPlayingOffset().asMilliseconds()<683405) music[0].setPlayingOffset(sf::milliseconds(683405));
        else if(music[0].getPlayingOffset().asMilliseconds()<798099) music[0].setPlayingOffset(sf::milliseconds(798099));
        else if(music[0].getPlayingOffset().asMilliseconds()<894276) music[0].setPlayingOffset(sf::milliseconds(894276));
        else if(music[0].getPlayingOffset().asMilliseconds()<1009286) music[0].setPlayingOffset(sf::milliseconds(1009286));
        else if(music[0].getPlayingOffset().asMilliseconds()<1140084) music[0].setPlayingOffset(sf::milliseconds(1140084));
        else if(music[0].getPlayingOffset().asMilliseconds()<1290224) music[0].setPlayingOffset(sf::milliseconds(1290224));
        else if(music[0].getPlayingOffset().asMilliseconds()<1391393) music[0].setPlayingOffset(sf::milliseconds(1391393));
        else if(music[0].getPlayingOffset().asMilliseconds()<1496496) music[0].setPlayingOffset(sf::milliseconds(1496496));
        else if(music[0].getPlayingOffset().asMilliseconds()<1614307) music[0].setPlayingOffset(sf::milliseconds(1614307));
        else if(music[0].getPlayingOffset().asMilliseconds()<1750933) music[0].setPlayingOffset(sf::milliseconds(1750933));
        else music[0].setPlayingOffset(sf::milliseconds(0));
    }

    music[id].play();

    return;
}

void pokeinfo(pokeprofile *p)
{
    if(!window.isOpen()) return;
    dexbeep();

    std::string name(p->name), type=types[p->type1], dex(p->dexentry);
    std::string hp=std::to_string(p->baseHP), att=std::to_string(p->baseAttack), def=std::to_string(p->baseDefense), spa=std::to_string(p->baseSpecialAttack), spd=std::to_string(p->baseSpecialDefense), spe=std::to_string(p->baseSpeed);

    sf::Texture bgt, texture;
    sf::Sprite bgs, sprite;

    if(p->type1<2 && p->type2) bgt.loadFromFile("img/bg/"+types[p->type2]+".png");
    else bgt.loadFromFile("img/bg/"+type+".png");
    bgs.setTexture(bgt);
    if(p->type2) type=type+"-"+types[p->type2];

    texture.loadFromFile("img/sprites/"+std::to_string(p->dexid)+".png");
    sprite.setTexture(texture);
    sprite.setOrigin(120, 120);
    sprite.scale(2, 2);
    sprite.setPosition(300, 300);

    int flag=1;
    while(window.isOpen() && flag)
    {
        window.clear();
        window.draw(bgs);
        window.draw(sprite);
        puttext(name, 180, 20, 48);
        puttext("Type: "+type, 720, 50, 24);
        puttext("HP: ", 800, 150, 30);
        puttext("Attack: ", 800, 200, 30);
        puttext("Defense: ", 800, 250, 30);
        puttext("Special Attack: ", 800, 300, 30);
        puttext("Special Defense: ", 800, 350, 30);
        puttext("Speed: ", 800, 400, 30);
        puttext(hp, 1100, 150, 30);
        puttext(att, 1100, 200, 30);
        puttext(def, 1100, 250, 30);
        puttext(spa, 1100, 300, 30);
        puttext(spd, 1100, 350, 30);
        puttext(spe, 1100, 400, 30);
        puttext(dex, 120, 480, 33);
        window.display();

        if(flag==1) cry(p);
        flag++;

        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed) window.close();
            if(event.type==sf::Event::KeyPressed) flag=0;

            if(event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left)
            {
                if(event.mouseButton.x*1280/window.getSize().x>=120 && event.mouseButton.x*1280/window.getSize().x<=480 && event.mouseButton.y*720/window.getSize().y>=120 && event.mouseButton.y*720/window.getSize().y<=420) cry(p);
                else flag=0;
            }
        }
    }
}

void moveinfo(move *p)
{
    if(!window.isOpen()) return;
    dexbeep();

    std::string name(p->name), type=types[p->type], cat=movecategory[p->category];

    sf::Texture texture;
    texture.loadFromFile("img/bg/"+type+".png");
    sf::Sprite sprite;
    sprite.setTexture(texture);

    bool flag=1;
    while(window.isOpen() && flag)
    {
        window.clear();
        window.draw(sprite);
        puttext("Move: "+name, 120, 120, 48);
        puttext("Type: "+type, 120, 200, 30);
        puttext("Category: "+cat, 120, 250, 30);
        puttext("Power: "+std::to_string(p->power), 760, 120, 48);
        puttext("Accuracy: "+std::to_string(p->accuracy>100? 100: p->accuracy), 760, 200, 30);
        puttext("PP: "+std::to_string(p->pp), 760, 250, 30);

        int i=420;
        if(p->multistrike) puttext("This is a multi-strike move.", 120, i+=60, 30);
        if(p->highcritical) puttext("This move has a high critical hit ratio.", 120, i+=60, 30);
        if(p==gotomove("Flying Press")) puttext("This move is Fighting and Flying type simultaneously.", 120, i+=60, 30);

        window.display();

        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed) window.close();
            if(event.type==sf::Event::KeyPressed || (event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left)) flag=0;
        }
    }
}

void pokelist(void)
{
    pokeprofile *p;

    for(p=pokedex->nxt; p; p=p->nxt)
    {
        printf("%d. %s\t", p->dexid, p->name);
        if(strlen(p->name)<4 || strlen(p->name)>10) putchar('\t');
    }

    putchar('\n');
}

void cry(pokeprofile *p)
{
    bool track=(music[1].getStatus()==sf::Music::Playing);
    music[track].pause();

    sf::Music cry;
    cry.openFromFile("audio/cry/"+std::to_string(p->dexid)+".ogg");
    cry.setLoop(0);
    cry.play();

    while(cry.getStatus()==sf::Music::Playing)
    {
        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
            {
                window.close();
                return;
            }

            if(event.type==sf::Event::KeyPressed || (event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left)) cry.stop();
        }
    }

    music[track].play();
}

bool raised(pokeprofile *p)
{
    switch(p->dexid)
    {
    case 83:
    case 85:
    case 123:
    case 130:
    case 198:
    case 344:
    case 441:
    case 455:
    case 701:
    case 724:
    case 733:
        return 0;
    }

    return !typefx(p, gotomove("Earthquake"));
}

bool hit(move *m)
{
    return (rand()%105)<(m->accuracy);
}

bool stab(pokeprofile *att, move *m)
{
    return att->type1==m->type||att->type2==m->type;
}

int typefx(pokeprofile *def, move *mv)
{
    int m=4, a=mv->type, d1=def->type1, d2=def->type2;

    if(a==1) //Normal Type Move
    {
        if(d1==8 || d2==8) m/=2; //Not very effective against Rock
        if(d1==9 || d2==9) m/=2; //Not very effective against Steel
        if(d1==15 || d2==15) return 0; //Ineffective against Ghost
    }

    else if(a==2) //Grass Type Move
    {
        if(d1==4 || d2==4) m*=2; //Super effective against Water
        if(d1==7 || d2==7) m*=2; //Super effective against Ground
        if(d1==8 || d2==8) m*=2; //Super effective against Rock
        if(d1==2 || d2==2) m/=2; //Not very effective against Grass
        if(d1==3 || d2==3) m/=2; //Not very effective against Fire
        if(d1==9 || d2==9) m/=2; //Not very effective against Steel
        if(d1==11 || d2==11) m/=2; //Not very effective against Poison
        if(d1==12 || d2==12) m/=2; //Not very effective against Bug
        if(d1==13 || d2==13) m/=2; //Not very effective against Flying
        if(d1==17 || d2==17) m/=2; //Not very effective against Dragon
    }

    else if(a==3) //Fire Type Move
    {
        if(d1==2 || d2==2) m*=2; //Super effective against Grass
        if(d1==6 || d2==6) m*=2; //Super effective against Ice
        if(d1==9 || d2==9) m*=2; //Super effective against Steel
        if(d1==12 || d2==12) m*=2; //Super effective against Bug
        if(d1==3 || d2==3) m/=2; //Not very effective against Fire
        if(d1==4 || d2==4) m/=2; //Not very effective against Water
        if(d1==8 || d2==8) m/=2; //Not very effective against Rock
        if(d1==17 || d2==17) m/=2; //Not very effective against Dragon
    }

    else if(a==4) //Water Type Move
    {
        if(d1==3 || d2==3) m*=2; //Super effective against Fire
        if(d1==7 || d2==7) m*=2; //Super effective against Ground
        if(d1==8 || d2==8) m*=2; //Super effective against Rock
        if(d1==2 || d2==2) m/=2; //Not very effective against Grass
        if(d1==4 || d2==4) m/=2; //Not very effective against Water
        if(d1==17 || d2==17) m/=2; //Not very effective against Dragon
    }

    else if(a==5) //Electric Type Move
    {
        if(d1==4 || d2==4) m*=2;  //Super effective against Water
        if(d1==13 || d2==13) m*=2; //Super effective against Flying
        if(d1==2 || d2==2) m/=2; //Not very effective against Grass
        if(d1==5 || d2==5) m/=2; //Not very effective against Electric
        if(d1==17 || d2==17) m/=2; //Not very effective against Dragon
        if(d1==7 || d2==7) return 0; //Ineffective against Ground
    }

    else if(a==6) //Ice Type Move
    {
        if(d1==2 || d2==2) m*=2; //Super effective against Grass
        if(d1==7 || d2==7) m*=2; //Super effective against Ground
        if(d1==13 || d2==13) m*=2; //Super effective against Flying
        if(d1==17 || d2==17) m*=2; //Super effective against Dragon
        if(d1==3 || d2==3) m/=2; //Not very effective against Fire
        if(d1==4 || d2==4) m/=2; //Not very effective against Water
        if(d1==6 || d2==6) m/=2; //Not very effective against Ice
        if(d1==9 || d2==9) m/=2; //Not very effective against Steel
    }

    else if(a==7) //Ground Type Move
    {
        if(d1==3 || d2==3) m*=2; //Super effective against Fire
        if(d1==5 || d2==5) m*=2; //Super effective against Electric
        if(d1==8 || d2==8) m*=2; //Super effective against Rock
        if(d1==9 || d2==9) m*=2; //Super effective against Steel
        if(d1==11 || d2==11) m*=2; //Super effective against Poison
        if(d1==2 || d2==2) m/=2; //Not very effective against Grass
        if(d1==12 || d2==12) m/=2; //Not very effective against Bug
        if(d1==13 || d2==13 || def->levitate) return 0; //Ineffective against Flying and Pokemon with Levitate ability
    }

    else if(a==8) //Rock Type Move
    {
        if(d1==3 || d2==3) m*=2; //Super effective against Fire
        if(d1==6 || d2==6) m*=2; //Super effective against Ice
        if(d1==12 || d2==12) m*=2; //Super effective against Bug
        if(d1==13 || d2==13) m*=2; //Super effective against Flying
        if(d1==7 || d2==7) m/=2; //Not very effective against Ground
        if(d1==9 || d2==9) m/=2; //Not very effective against Steel
        if(d1==10 || d2==10) m/=2; //Not very effective against Fighting
    }

    else if(a==9) //Steel Type Move
    {
        if(d1==6 || d2==6) m*=2; //Super effective against Ice
        if(d1==8 || d2==8) m*=2; //Super effective against Rock
        if(d1==18 || d2==18) m*=2; //Super effective against Fairy
        if(d1==3 || d2==3) m/=2; //Not very effective against Fire
        if(d1==4 || d2==4) m/=2; //Not very effective against Water
        if(d1==5 || d2==5) m/=2; //Not very effective against Electric
        if(d1==9 || d2==9) m/=2; //Not very effective against Steel
    }

    else if(a==10) //Fighting Type Move
    {
        if(d1==1 || d2==1) m*=2; //Super effective against Normal
        if(d1==6 || d2==6) m*=2; //Super effective against Ice
        if(d1==8 || d2==8) m*=2; //Super effective against Rock
        if(d1==9 || d2==9) m*=2; //Super effective against Steel
        if(d1==16 || d2==16) m*=2; //Super effective against Dark
        if(d1==11 || d2==11) m/=2; //Not very effective against Poison
        if(d1==12 || d2==12) m/=2; //Not very effective against Bug
        if(d1==13 || d2==13) m/=2; //Not very effective against Flying
        if(d1==14 || d2==14) m/=2; //Not very effective against Psychic
        if(d1==18 || d2==18) m/=2; //Not very effective against Fairy
    }

    else if(a==11) //Poison Type Move
    {
        if(d1==2 || d2==2) m*=2; //Super effective against Grass
        if(d1==18 || d2==18) m*=2; //Super effective against Fairy
        if(d1==7 || d2==7) m/=2; //Not very effective against Ground
        if(d1==8 || d2==8) m/=2; //Not very effective against Rock
        if(d1==11 || d2==11) m/=2; //Not very effective against Poison
        if(d1==15 || d2==15) m/=2; //Not very effective against Ghost
        if(d1==9 || d2==9) return 0; //Ineffective against Steel
    }

    else if(a==12) //Bug Type Move
    {
        if(d1==2 || d2==2) m*=2; //Super effective against Grass
        if(d1==14 || d2==14) m*=2; //Super effective against Psychic
        if(d1==16 || d2==16) m*=2; //Super effective against Dark
        if(d1==3 || d2==3) m/=2; //Not very effective against Fire
        if(d1==9 || d2==9) m/=2; //Not very effective against Steel
        if(d1==10 || d2==10) m/=2; //Not very effective against Fighting
        if(d1==11 || d2==11) m/=2; //Not very effective against Poison
        if(d1==13 || d2==13) m/=2; //Not very effective against Flying
        if(d1==15 || d2==15) m/=2; //Not very effective against Ghost
        if(d1==18 || d2==18) m/=2; //Not very effective against Fairy
    }

    else if(a==13) //Flying Type Move
    {
        if(d1==2 || d2==2) m*=2; //Super effective against Grass
        if(d1==10 || d2==10) m*=2; //Super effective against Fighting
        if(d1==12 || d2==12) m*=2; //Super effective against Bug
        if(d1==5 || d2==5) m/=2; //Not very effective against Electric
        if(d1==8 || d2==8) m/=2; //Not very effective against Rock
        if(d1==9 || d2==9) m/=2; //Not very effective against Steel
    }

    else if(a==14) //Psychic Type Move
    {
        if(d1==10 || d2==10) m*=2; //Super effective against Fighting
        if(d1==11 || d2==11) m*=2; //Super effective against Poison
        if(d1==9 || d2==9) m/=2; //Not very effective against Steel
        if(d1==14 || d2==14) m/=2; //Not very effective against Psychic
        if(d1==16 || d2==16) return 0; //Ineffective against Dark
    }

    else if(a==15) //Ghost Type Move
    {
        if(d1==14 || d2==14) m*=2; //Super effective against Psychic
        if(d1==15 || d2==15) m*=2; //Super effective against Ghost
        if(d1==16 || d2==16) m/=2; //Not very effective against Dark
        if(d1==1 || d2==1) return 0; //Ineffective against Normal
    }

    else if(a==16) //Dark Type Move
    {
        if(d1==14 || d2==14) m*=2; //Super effective against Psychic
        if(d1==15 || d2==15) m*=2; //Super effective against Ghost
        if(d1==10 || d2==10) m/=2; //Not very effective against Fighting
        if(d1==16 || d2==16) m/=2; //Not very effective against Dark
        if(d1==18 || d2==18) m/=2; //Not very effective against Fairy
    }

    else if(a==17) //Dragon Type Move
    {
        if(d1==17 || d2==17) m*=2; //Super effective against Dragon
        if(d1==9 || d2==9) m/=2; //Not very effective against Steel
        if(d1==18 || d2==18) return 0; //Ineffective against Fairy
    }

    else if(a==18) //Fairy Type Move
    {
        if(d1==10 || d2==10) m*=2; //Super effective against Fighting
        if(d1==16 || d2==16) m*=2; //Super effective against Dark
        if(d1==17 || d2==17) m*=2; //Super effective against Dragon
        if(d1==3 || d2==3) m/=2; //Not very effective against Fire
        if(d1==9 || d2==9) m/=2; //Not very effective against Steel
        if(d1==11 || d2==11) m/=2; //Not very effective against Poison
    }

    return m;
}

bool criticalhit(pokeprofile *att, move *m)
{
    int p=att->baseSpeed+att->baseSpeed*m->highcritical*2;
    if(p>511) p=511;

    return (rand()%512)<p;
}

void teambuilder(bool pos)
{
    if(!window.isOpen()) return;

    if(pos) boxdisplay("You must be Trainer 1.");
    else boxdisplay("And, You are Trainer 2.");

    boxdisplay("Are you a Boy or a Girl?\n1. Boy\n2. Girl");
    int choice=0;
    while(!choice && window.isOpen())
    {
        labdisplay();

        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
            {
                window.close();
                return;
            }

            if(event.type==sf::Event::TextEntered && event.text.unicode>'0' && event.text.unicode<'3') choice=event.text.unicode-'0';
            if(event.type==sf::Event::KeyPressed && event.key.code==sf::Keyboard::Enter) choice=1;

            if(event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left && event.mouseButton.x*1280/window.getSize().x>=310 && event.mouseButton.x*1280/window.getSize().x<=470)
            {
                if(event.mouseButton.y*720/window.getSize().y>=590 && event.mouseButton.y*720/window.getSize().y<=620) choice=1;
                if(event.mouseButton.y*720/window.getSize().y>=630 && event.mouseButton.y*720/window.getSize().y<=660) choice=2;
            }
        }
    }
    msg="";
    trainer[pos].gender=choice&1;

    boxdisplay("Alright. What's your name?\n");
    screenscanf(310, 600, 25);
    if(!window.isOpen()) return;

    if(msg[0]>='a' && msg[0]<='z') msg[0]-=32;
    while(msg[0]==' ') msg=msg.substr(1, msg.length()); while(msg[msg.length()-1]==' ') msg=msg.substr(0, msg.length()-1);
    std::string name=msg;

    if(!pos) while(!strcmp(name.c_str(), trainer[1].name))
    {
        boxdisplay("I'm sorry.");
        boxdisplay("What's your name again?\n");
        screenscanf(310, 600, 25);
        if(!window.isOpen()) return;

        if(msg[0]>='a' && msg[0]<='z') msg[0]-=32;
        while(msg[0]==' ') msg=msg.substr(1, msg.length()); while(msg[msg.length()-1]==' ') msg=msg.substr(0, msg.length()-1);
        name=msg;
    }

    for(int j=1; j<msg.length(); j++) if(msg[j]>='A' && msg[j]<='Z') msg[j]+=32;
    strcpy(trainer[pos].name, msg.c_str());

    if(!pos && trainer[0].gender*trainer[1].gender)
    {
        std::string opp(trainer[1].name);
        for(int j=1; j<opp.length(); j++) if(opp[j]>='A' && opp[j]<='Z') opp[j]+=32;

        if(!strncmp(trainer[0].name, "Red", 3) && !strncmp(opp.c_str(), "Red", 3)) while(!strncmp(trainer[0].name, "Red", 3))
        {
            boxdisplay("I'm sorry.");
            boxdisplay("What's your name again?\n");
            screenscanf(310, 600, 25);
            if(!window.isOpen()) return;

            if(msg[0]>='a' && msg[0]<='z') msg[0]-=32;
            while(msg[0]==' ') msg=msg.substr(1, msg.length()); while(msg[msg.length()-1]==' ') msg=msg.substr(0, msg.length()-1);
            name=msg;
            for(int j=1; j<msg.length(); j++) if(msg[j]>='A' && msg[j]<='Z') msg[j]+=32;
            strcpy(trainer[pos].name, msg.c_str());
        }

        else if(!strncmp(trainer[0].name, "Blue", 4) && !strncmp(opp.c_str(), "Blue", 4)) while(!strncmp(trainer[0].name, "Blue", 4) || !strncmp(trainer[0].name, "Green", 5))
        {
            boxdisplay("I'm sorry.");
            boxdisplay("What's your name again?\n");
            screenscanf(310, 600, 25);
            if(!window.isOpen()) return;

            if(msg[0]>='a' && msg[0]<='z') msg[0]-=32;
            while(msg[0]==' ') msg=msg.substr(1, msg.length()); while(msg[msg.length()-1]==' ') msg=msg.substr(0, msg.length()-1);
            name=msg;
            for(int j=1; j<msg.length(); j++) if(msg[j]>='A' && msg[j]<='Z') msg[j]+=32;
            strcpy(trainer[pos].name, msg.c_str());
        }

        else if(!strncmp(trainer[0].name, "Green", 5) && !strncmp(opp.c_str(), "Green", 5)) while(!strncmp(trainer[0].name, "Blue", 4) || !strncmp(trainer[0].name, "Green", 5))
        {
            boxdisplay("I'm sorry.");
            boxdisplay("What's your name again?\n");
            screenscanf(310, 600, 25);
            if(!window.isOpen()) return;

            if(msg[0]>='a' && msg[0]<='z') msg[0]-=32;
            while(msg[0]==' ') msg=msg.substr(1, msg.length()); while(msg[msg.length()-1]==' ') msg=msg.substr(0, msg.length()-1);
            name=msg;
            for(int j=1; j<msg.length(); j++) if(msg[j]>='A' && msg[j]<='Z') msg[j]+=32;
            strcpy(trainer[pos].name, msg.c_str());
        }

        else if(!strncmp(trainer[0].name, "Blue", 4) && !strncmp(opp.c_str(), "Green", 5)) while(!strncmp(trainer[0].name, "Blue", 4) || !strncmp(trainer[0].name, "Green", 5))
        {
            boxdisplay("I'm sorry.");
            boxdisplay("What's your name again?\n");
            screenscanf(310, 600, 25);
            if(!window.isOpen()) return;

            if(msg[0]>='a' && msg[0]<='z') msg[0]-=32;
            while(msg[0]==' ') msg=msg.substr(1, msg.length()); while(msg[msg.length()-1]==' ') msg=msg.substr(0, msg.length()-1);
            name=msg;
            for(int j=1; j<msg.length(); j++) if(msg[j]>='A' && msg[j]<='Z') msg[j]+=32;
            strcpy(trainer[pos].name, msg.c_str());
        }

        else if(!strncmp(trainer[0].name, "Green", 5) && !strncmp(opp.c_str(), "Blue", 4)) while(!strncmp(trainer[0].name, "Blue", 4) || !strncmp(trainer[0].name, "Green", 5))
        {
            boxdisplay("I'm sorry.");
            boxdisplay("What's your name again?\n");
            screenscanf(310, 600, 25);
            if(!window.isOpen()) return;

            if(msg[0]>='a' && msg[0]<='z') msg[0]-=32;
            while(msg[0]==' ') msg=msg.substr(1, msg.length()); while(msg[msg.length()-1]==' ') msg=msg.substr(0, msg.length()-1);
            name=msg;
            for(int j=1; j<msg.length(); j++) if(msg[j]>='A' && msg[j]<='Z') msg[j]+=32;
            strcpy(trainer[pos].name, msg.c_str());
        }
    }

    if(!strlen(trainer[pos].name) || !strncmp(trainer[pos].name, "Auto", 4) || !strncmp(trainer[pos].name, "Random", 6))
    {
        autobuilder(pos, 0);
        return;
    }

    if(trainer[pos].gender)
    {
        if(pos)
        {
            if(!strncmp(trainer[pos].name, "Victor", 6) || !strncmp(trainer[pos].name, "Masaru", 6)) trainer[pos].photo=19+8*!(rand()&1);
            else if(!strncmp(trainer[pos].name, "Brendan", 7) || !strncmp(trainer[pos].name, "Yuki", 4)) trainer[pos].photo=20;
            else if(!strncmp(trainer[pos].name, "Lucas", 5) || !strncmp(trainer[pos].name, "Koki", 4)) trainer[pos].photo=21+rand()&1;
            else if(!strncmp(trainer[pos].name, "Nate", 4) || !strncmp(trainer[pos].name, "Kyohei", 6)) trainer[pos].photo=23;
            else if(!strncmp(trainer[pos].name, "Red", 3)) trainer[pos].photo=24+!(rand()&3);
            else if(!strncmp(trainer[pos].name, "Blue", 4) || !strncmp(trainer[pos].name, "Green", 5)) trainer[pos].photo=26;
            else trainer[pos].photo=19+rand()%5;
        }

        else
        {
            if(!strncmp(trainer[pos].name, "Hilbert", 7) || !strncmp(trainer[pos].name, "Touya", 5)) trainer[pos].photo=6;
            else if(!strncmp(trainer[pos].name, "Elio", 6) || !strncmp(trainer[pos].name, "Yo", 2)) trainer[pos].photo=7+rand()&1;
            else if(!strncmp(trainer[pos].name, "Michael", 7)) trainer[pos].photo=9;
            else if(!strncmp(trainer[pos].name, "Zac", 3) || !strncmp(trainer[pos].name, "Lumiose", 7)) trainer[pos].photo=10;
            else if(!strncmp(trainer[pos].name, "Red", 3)) trainer[pos].photo=11+!(rand()&3);
            else if(!strncmp(trainer[pos].name, "Blue", 4) || !strncmp(trainer[pos].name, "Green", 5)) trainer[pos].photo=13;
            else trainer[pos].photo=6+rand()%5;
        }
    }

    else
    {
        if(pos)
        {
            if(!strncmp(trainer[pos].name, "Hilda", 5) || !strncmp(trainer[pos].name, "Touko", 5)) trainer[pos].photo=17-3*!(rand()%3);
            else if(!strncmp(trainer[pos].name, "Lyra", 4) || !strncmp(trainer[pos].name, "Kotone", 6)) trainer[pos].photo=16;
            else if(!strncmp(trainer[pos].name, "Selene", 6) || !strncmp(trainer[pos].name, "Mizuki", 6)) trainer[pos].photo=15+3*!(rand()%3);
            else trainer[pos].photo=14+rand()%5;
        }

        else
        {
            if(!strncmp(trainer[pos].name, "Rosa", 4) || !strncmp(trainer[pos].name, "Mei", 3)) trainer[pos].photo=3*!(rand()%3);
            else if(!strncmp(trainer[pos].name, "Gloria", 6) || !strncmp(trainer[pos].name, "Yuuri", 5)) trainer[pos].photo=1+4*!(rand()&1);
            else if(!strncmp(trainer[pos].name, "Leaf", 4)) trainer[pos].photo=2*(1+rand()&1);
            else trainer[pos].photo=rand()&3;
        }
    }
    strcpy(trainer[pos].name, name.c_str());
    boxdisplay("Now, it's time to build your team, Trainer "+name);
    boxdisplay("Remember, ...");
    boxdisplay("You have to press the Space key to see the list of available Pokemon species.");

    int i, j, id=0, diff;
    for(i=0; i<6; ++i, id=0)
    {
        boxdisplay(std::to_string(i+1)+". Choose A Pokemon\nTrainer "+name+"-\n");

        std::string temp=msg;
        sf::Text text;
        text.setFont(gameboy);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::Black);

        text.setPosition(310, 600);
        bool flag=1;
        while(flag && window.isOpen())
        {
            while(window.pollEvent(event))
            {
                if(event.type==sf::Event::Closed) window.close();

                else if(event.type==sf::Event::TextEntered && ((event.text.unicode>='a' && event.text.unicode<='z') || (event.text.unicode>='A' && event.text.unicode<='Z') || (event.text.unicode>='0' && event.text.unicode<='9')))
                {
                    std::string str="0";
                    str[0]=event.text.unicode;

                    if(msg.length()-temp.length()<15)
                    {
                        msg=msg+str;
                        text.setString(msg);
                    }

                    labdisplay();
                }

                else if(event.type==sf::Event::KeyPressed && event.key.code==sf::Keyboard::Enter)
                {
                    msg=msg.substr(temp.length(), msg.length());
                    flag=0;
                }

                else if(event.type==sf::Event::KeyPressed)
                {
                    if(event.key.code==sf::Keyboard::Backspace && msg.length()>temp.length()) msg=msg.substr(0, msg.length()-1);
                    else if(event.key.code==sf::Keyboard::Escape) msg=temp;
                    else if(event.key.code==sf::Keyboard::Space) pokelist();

                    labdisplay();
                }
            }
        }
        if(!window.isOpen()) return;

        if(msg[0]>='a' && msg[0]<='z') msg[0]-=32;
        for(j=1; j<msg.length(); j++) if(msg[j]>='A' && msg[j]<='Z') msg[j]+=32;

        if(msg.substr(0, 4)=="Auto" || !msg.length())
        {
            autobuilder(pos, i);
            return;
        }
        else if(gotoPokemon(msg)) id=gotoPokemon(msg)->dexid;
        else for(j=0; j<msg.length() && msg[j]>='0' && msg[j]<='9'; j++) id=id*10+msg[j]-'0';

        if(!gotoid(id))
        {
            i--;
            continue;
        }

        trainer[pos].team[i].dex=gotoid(id);

        trainer[pos].team[i].maxhp=2*(gotoid(id)->baseHP)+110+rand()%95;
        trainer[pos].team[i].hp=trainer[pos].team[i].maxhp;

        trainer[pos].team[i].stat[1]=floor((2*(gotoid(id)->baseAttack)+5)*0.9);
        diff=1+floor((2*(gotoid(id)->baseAttack)+99)*1.1)-trainer[pos].team[i].stat[1];
        trainer[pos].team[i].stat[1]+=rand()%diff;

        trainer[pos].team[i].stat[2]=floor((2*(gotoid(id)->baseSpecialAttack)+5)*0.9);
        diff=1+floor((2*(gotoid(id)->baseSpecialAttack)+99)*1.1)-trainer[pos].team[i].stat[2];
        trainer[pos].team[i].stat[2]+=rand()%diff;

        trainer[pos].team[i].stat[3]=floor((2*(gotoid(id)->baseDefense)+5)*0.9);
        diff=1+floor((2*(gotoid(id)->baseDefense)+99)*1.1)-trainer[pos].team[i].stat[3];
        trainer[pos].team[i].stat[3]+=rand()%diff;

        trainer[pos].team[i].stat[4]=floor((2*(gotoid(id)->baseSpecialDefense)+5)*0.9);
        diff=1+floor((2*(gotoid(id)->baseSpecialDefense)+99)*1.1)-trainer[pos].team[i].stat[4];
        trainer[pos].team[i].stat[4]+=rand()%diff;

        trainer[pos].team[i].stat[0]=floor((2*(gotoid(id)->baseSpeed)+5)*0.9);
        diff=1+floor((2*(gotoid(id)->baseSpeed)+99)*1.1)-trainer[pos].team[i].stat[0];
        trainer[pos].team[i].stat[0]+=rand()%diff;

        for(j=0; j<4; j++)
        {
            trainer[pos].team[i].moveset[j]=gotoid(id)->moves[j];
            if(trainer[pos].team[i].dex==gotoPokemon("Garchomp") && j==3 && !(rand()&3)) trainer[pos].team[i].moveset[j]=gotomove("Dragon Pulse");

            if(trainer[pos].team[i].dex==gotoPokemon("Magikarp"))
            {
                trainer[pos].team[i].pp[j]=gotoid(id)->moves[j]->pp+rand()%(gotoid(id)->moves[j]->pp/5*3+1);
                continue;
            }

            trainer[pos].team[i].pp[j]=rand()%(trainer[pos].team[i].moveset[j]->pp/5*8+1);
            diff=gotoid(id)->moves[j]->pp/5;
            if(trainer[pos].team[i].pp[j]<=4*diff) trainer[pos].team[i].pp[j]+=diff;
            else if (trainer[pos].team[i].pp[j]>=5*diff)
            {
                trainer[pos].team[i].pp[j]-=rand()%(++diff);
                trainer[pos].team[i].pp[j]-=rand()%diff;
                trainer[pos].team[i].pp[j]-=rand()%diff;
            }
        }

        trainer[pos].team[i].moveset[4]=gotomove("Struggle");

        switch(id)
        {
        case 3:
            trainer[pos].team[i].form=!(rand()&7);
            break;
        case 12:
        case 25:
        case 45:
        case 65:
        case 85:
        case 123:
        case 129:
        case 130:
        case 190:
        case 198:
        case 212:
        case 214:
        case 215:
        case 217:
        case 267:
        case 272:
        case 323:
        case 327:
        case 332:
        case 350:
        case 405:
        case 419:
        case 423:
        case 424:
        case 445:
        case 450:
        case 465:
        case 521:
        case 550:
        case 593:
            trainer[pos].team[i].form=rand()&1;
            break;
        case 666:
            trainer[pos].team[i].form=rand()%20;
            break;
        case 668:
            trainer[pos].team[i].form=!!(rand()&7);
            break;
        case 671:
            trainer[pos].team[i].form=rand()%5;
            break;
        default:
            trainer[pos].team[i].form=0;
        }
    }
    msg="";
}


void autonomen(bool pos)
{
    if(trainer[pos].gender)
    {
        if(pos)
        {
            trainer[pos].photo=19+(rand()&7);

            switch(trainer[pos].photo)
            {
            case 24:
                trainer[pos].photo=27;
            case 19:
                strcpy(trainer[pos].name, "Masaru");
                if(rand()%5) strcpy(trainer[pos].name, "Victor");
                break;
            case 20:
            case 25:
                trainer[pos].photo=20;
                strcpy(trainer[pos].name, "Yuki");
                if(rand()%5) strcpy(trainer[pos].name, "Brendan");
                break;
            case 21:
            case 22:
                strcpy(trainer[pos].name, "Koki");
                if(rand()%5) strcpy(trainer[pos].name, "Lucas");
                break;
            case 23:
            case 26:
                trainer[pos].photo=23;
                strcpy(trainer[pos].name, "Kyohei");
                if(rand()%5) strcpy(trainer[pos].name, "Nate");
            }
        }

        else
        {
            trainer[pos].photo=6+(rand()&7);

            switch(trainer[pos].photo)
            {
            case 6:
                strcpy(trainer[pos].name, "Touya");
                if(rand()%5) strcpy(trainer[pos].name, "Hilbert");
                break;
            case 7:
            case 8:
                strcpy(trainer[pos].name, "Yo");
                if(rand()%5) strcpy(trainer[pos].name, "Elio");
                break;
            case 9:
                strcpy(trainer[pos].name, "Michael");
                break;
            case 10:
                strcpy(trainer[pos].name, "Zac");
                break;
            case 12:
                trainer[pos].photo--;
            case 11:
                strcpy(trainer[pos].name, "Red");
                break;
            case 13:
                strcpy(trainer[pos].name, "Blue");
            }
        }
    }

    else
    {
        if(pos)
        {
            trainer[pos].photo=14+rand()%5;

            switch(trainer[pos].photo)
            {
            case 14:
            case 17:
                strcpy(trainer[pos].name, "Touko");
                if(rand()%5) strcpy(trainer[pos].name, "Hilda");
                break;
            case 15:
            case 18:
                strcpy(trainer[pos].name, "Mizuki");
                if(rand()%5) strcpy(trainer[pos].name, "Selene");
                break;
            case 16:
                strcpy(trainer[pos].name, "Kotone");
                if(rand()%5) strcpy(trainer[pos].name, "Lyra");
            }
        }

        else
        {
            trainer[pos].photo=rand()%6;

            switch(trainer[pos].photo)
            {
            case 0:
            case 3:
                strcpy(trainer[pos].name, "Mei");
                if(rand()%5) strcpy(trainer[pos].name, "Rosa");
                break;
            case 1:
            case 5:
                strcpy(trainer[pos].name, "Yuuri");
                if(rand()%5) strcpy(trainer[pos].name, "Gloria");
                break;
            case 2:
            case 4:
                strcpy(trainer[pos].name, "Leaf");
            }
        }
    }
}

void autobuilder(bool pos, int num)
{
    if(!strlen(trainer[pos].name) || !strncmp(trainer[pos].name, "Auto", 4) || !strncmp(trainer[pos].name, "Random", 6))
    {
        autonomen(pos);

        while(!pos)
        {
            if(!strcmp(trainer[0].name, trainer[1].name)) autonomen(0);
            else if(trainer[0].gender*trainer[1].gender && !strncmp(trainer[0].name, "Red", 3) && !strncmp(trainer[1].name, "Red", 3)) while(!strncmp(trainer[0].name, "Red", 3)) autonomen(0);
            else if(trainer[0].gender*trainer[1].gender && !strncmp(trainer[0].name, "Blue", 4) && !strncmp(trainer[1].name, "Blue", 4)) while(!strncmp(trainer[0].name, "Blue", 4) || !strncmp(trainer[0].name, "Green", 5)) autonomen(0);
            else if(trainer[0].gender*trainer[1].gender && !strncmp(trainer[0].name, "Green", 5) && !strncmp(trainer[1].name, "Green", 5)) while(!strncmp(trainer[0].name, "Blue", 4) || !strncmp(trainer[0].name, "Green", 5)) autonomen(0);
            else if(trainer[0].gender*trainer[1].gender && !strncmp(trainer[0].name, "Blue", 4) && !strncmp(trainer[1].name, "Green", 5)) while(!strncmp(trainer[0].name, "Blue", 4) || !strncmp(trainer[0].name, "Green", 5)) autonomen(0);
            else if(trainer[0].gender*trainer[1].gender && !strncmp(trainer[0].name, "Green", 5) && !strncmp(trainer[1].name, "Blue", 4)) while(!strncmp(trainer[0].name, "Blue", 4) || !strncmp(trainer[0].name, "Green", 5)) autonomen(0);
            else break;
        }
    }

    int i, j, id=rand()%783, diff;
    for(i=num; i<6; i++, id=rand()%783)
    {
        while(!gotoid(++id));

        trainer[pos].team[i].dex=gotoid(id);

        trainer[pos].team[i].maxhp=2*(gotoid(id)->baseHP)+110+rand()%95;
        trainer[pos].team[i].hp=trainer[pos].team[i].maxhp;

        trainer[pos].team[i].stat[1]=floor((2*(gotoid(id)->baseAttack)+5)*0.9);
        diff=1+floor((2*(gotoid(id)->baseAttack)+99)*1.1)-trainer[pos].team[i].stat[1];
        trainer[pos].team[i].stat[1]+=rand()%diff;

        trainer[pos].team[i].stat[2]=floor((2*(gotoid(id)->baseSpecialAttack)+5)*0.9);
        diff=1+floor((2*(gotoid(id)->baseSpecialAttack)+99)*1.1)-trainer[pos].team[i].stat[2];
        trainer[pos].team[i].stat[2]+=rand()%diff;

        trainer[pos].team[i].stat[3]=floor((2*(gotoid(id)->baseDefense)+5)*0.9);
        diff=1+floor((2*(gotoid(id)->baseDefense)+99)*1.1)-trainer[pos].team[i].stat[3];
        trainer[pos].team[i].stat[3]+=rand()%diff;

        trainer[pos].team[i].stat[4]=floor((2*(gotoid(id)->baseSpecialDefense)+5)*0.9);
        diff=1+floor((2*(gotoid(id)->baseSpecialDefense)+99)*1.1)-trainer[pos].team[i].stat[4];
        trainer[pos].team[i].stat[4]+=rand()%diff;

        trainer[pos].team[i].stat[0]=floor((2*(gotoid(id)->baseSpeed)+5)*0.9);
        diff=1+floor((2*(gotoid(id)->baseSpeed)+99)*1.1)-trainer[pos].team[i].stat[0];
        trainer[pos].team[i].stat[0]+=rand()%diff;

        for(j=0; j<4; j++)
        {
            trainer[pos].team[i].moveset[j]=gotoid(id)->moves[j];
            if(trainer[pos].team[i].dex==gotoPokemon("Garchomp") && j==3 && (rand()&3)) trainer[pos].team[i].moveset[j]=gotomove("Dragon Pulse");

            if(trainer[pos].team[i].dex==gotoPokemon("Magikarp"))
            {
                trainer[pos].team[i].pp[j]=gotoid(id)->moves[j]->pp+rand()%(gotoid(id)->moves[j]->pp/5*3+1);
                continue;
            }

            trainer[pos].team[i].pp[j]=rand()%(trainer[pos].team[i].moveset[j]->pp/5*8+1);
            diff=gotoid(id)->moves[j]->pp/5;
            if(trainer[pos].team[i].pp[j]<=4*diff) trainer[pos].team[i].pp[j]+=diff;
            else if (trainer[pos].team[i].pp[j]>=5*diff)
            {
                trainer[pos].team[i].pp[j]-=rand()%(++diff);
                trainer[pos].team[i].pp[j]-=rand()%diff;
                trainer[pos].team[i].pp[j]-=rand()%diff;
            }
        }

        trainer[pos].team[i].moveset[4]=gotomove("Struggle");

        switch(id)
        {
        case 3:
            trainer[pos].team[i].form=!(rand()&7);
            break;
        case 12:
        case 25:
        case 45:
        case 65:
        case 85:
        case 123:
        case 129:
        case 130:
        case 190:
        case 198:
        case 212:
        case 214:
        case 215:
        case 217:
        case 267:
        case 272:
        case 323:
        case 327:
        case 332:
        case 350:
        case 405:
        case 419:
        case 423:
        case 424:
        case 445:
        case 450:
        case 465:
        case 521:
        case 550:
        case 593:
            trainer[pos].team[i].form=rand()&1;
            break;
        case 666:
            trainer[pos].team[i].form=rand()%20;
            break;
        case 668:
            trainer[pos].team[i].form=!!(rand()&7);
            break;
        case 671:
            trainer[pos].team[i].form=rand()%5;
            break;
        default:
            trainer[pos].team[i].form=0;
        }
    }
}

void choosePokemon(bool pos)
{
    if(!window.isOpen()) return;

    int i, id, temp=0;
    sf::Sprite sprite;

    while(1)
    {
        std::string str(trainer[pos].name);
        textdisplay("Choose Your Active Pokemon, "+str+"-");

        break;
    }

    fulldisplay();
    teamdisplay(pos, 6);

    int high=6;
    while(!temp)
    {
        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
            {
                window.close();
                return;
            }

            if(event.type==sf::Event::Resized)
            {
                fulldisplay();
                teamdisplay(pos, 6);
            }

            if(event.type==sf::Event::TextEntered && event.text.unicode>'0' && event.text.unicode<'7')
            {
                id=event.text.unicode-'1';
                temp=trainer[pos].team[id].hp;
            }

            if(event.type==sf::Event::MouseMoved)
            {
                if(event.mouseMove.y*720/window.getSize().y>=600 && event.mouseMove.x*1280/window.getSize().x>=40 && event.mouseMove.x*1280/window.getSize().x<1240)
                {
                    high=(event.mouseMove.x*1280/window.getSize().x-40)/200;
                    if(trainer[pos].team[high].hp)
                    {
                        fulldisplay();
                        teamdisplay(pos, high);
                    }

                    else
                    {
                        fulldisplay();
                        teamdisplay(pos, 6);
                        high=6;
                    }
                }

                else if(high-6)
                {
                    fulldisplay();
                    teamdisplay(pos, 6);
                    high=6;
                }
            }

            if(event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left && event.mouseButton.y*720/window.getSize().y>=600)
            {
                if(event.mouseButton.x*1280/window.getSize().x>=40 && event.mouseButton.x*1280/window.getSize().x<1240) id=(event.mouseButton.x*1280/window.getSize().x-40)/200;
                temp=trainer[pos].team[id].hp;
            }
        }
    }
    msg="";

    active[pos]=id;
    fulldisplay();
    cry(trainer[pos].team[id].dex);
    while(1)
    {
        music[0].pause();
        std::string str1(trainer[pos].name), str2(trainer[pos].team[id].dex->name);
        textdisplay("Trainer "+str1+" sent out "+str2+"!");

        break;
    }
    msg="";

    if(trainer[pos].team[id].dex==gotoPokemon("Greninja") && music[1].getStatus()!=sf::Music::Playing && !(rand()&7))
    {
        music[0].pause();
        music[0].setPlayingOffset(sf::milliseconds(1390393));
    }
}


void attack(bool pos)
{
    pokemon *att, *def;
    att=&trainer[pos].team[active[pos]];
    def=&trainer[!pos].team[active[!pos]];

    int i, damage=0, a=1, d=3, high=0;

    std::string mv1(att->moveset[0]->name), mv2(att->moveset[1]->name), mv3(att->moveset[2]->name), mv4(att->moveset[3]->name);
    mv1=mv1+"\tPP: "+std::to_string(att->pp[0]); if(!att->pp[0]) mv1="Struggle\tPP: 0";
    mv2=mv2+"\tPP: "+std::to_string(att->pp[1]); if(!att->pp[1]) mv2="Struggle\tPP: 0";
    mv3=mv3+"\tPP: "+std::to_string(att->pp[2]); if(!att->pp[2]) mv3="Struggle\tPP: 0";
    mv4=mv4+"\tPP: "+std::to_string(att->pp[3]); if(!att->pp[3]) mv4="Struggle\tPP: 0";

    while(1)
    {
        std::string str1(att->dex->name), str2(trainer[pos].name);
        textdisplay("What will "+str1+" do, Trainer "+str2+"?");

        break;
    }

    fulldisplay();
    drawoption(mv1, mv2, mv3, mv4, 0);

    while(i>3 && window.isOpen())
    {
        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
            {
                window.close();
                return;
            }

            if(event.type==sf::Event::Resized)
            {
                fulldisplay();
                drawoption(mv1, mv2, mv3, mv4, 0);
            }

            if(event.type==sf::Event::TextEntered && event.text.unicode>'0' && event.text.unicode<'5') i=event.text.unicode-'1';

            if(event.type==sf::Event::MouseMoved)
            {
                if(event.mouseMove.y*720/window.getSize().y>=600 && event.mouseMove.y*720/window.getSize().y<=700)
                {
                    high=5;
                    if(event.mouseMove.x*1280/window.getSize().x>=120 && event.mouseMove.x*1280/window.getSize().x<=600) high=1;
                    else if(event.mouseMove.x*1280/window.getSize().x>=760 && event.mouseMove.x*1280/window.getSize().x<=1240) high=2;
                    high+=2*(event.mouseMove.y*720/window.getSize().y>=650);

                    if(high<5)
                    {
                        fulldisplay();
                        drawoption(mv1, mv2, mv3, mv4, high);
                    }

                    else
                    {
                        fulldisplay();
                        drawoption(mv1, mv2, mv3, mv4, 0);
                        high=0;
                    }
                }

                else if(high)
                {
                    fulldisplay();
                    drawoption(mv1, mv2, mv3, mv4, 0);
                    high=0;
                }
            }

            if(event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left && event.mouseButton.y*720/window.getSize().y>=600 && event.mouseButton.y*720/window.getSize().y<=700)
            {
                if(event.mouseButton.x*1280/window.getSize().x>=120 && event.mouseButton.x*1280/window.getSize().x<=600) i=0;
                else if(event.mouseButton.x*1280/window.getSize().x>=760 && event.mouseButton.x*1280/window.getSize().x<=1240) i=1;
                i+=2*(event.mouseButton.y*720/window.getSize().y>=650);
            }
        }
    }
    msg="";

    if(!att->pp[i]) i=4;
    else att->pp[i]--;

    while(1)
    {
        std::string str1(att->dex->name), str2(att->moveset[i]->name);
        textdisplay(str1+" used "+str2+"!");

        break;
    }
    msg="";

    if(att->moveset[i]==gotomove("Splash"))
    {
        textdisplay("Nothing happened!");
        msg="";
        return;
    }

    if(att->moveset[i]==gotomove("Milk Drink"))
    {
        int heal=att->maxhp/2;
        if(att->maxhp-att->hp<heal) heal=att->maxhp-att->hp;

        if(heal)
        {
            std::string str(att->dex->name);
            textdisplay(str+" regained its health!");
            msg="";
        }

        while(heal--)
        {
            att->hp++;

            while(window.pollEvent(event))
            {
                if(event.type==sf::Event::Closed)
                {
                    window.close();
                    return;
                }

                if(event.type==sf::Event::KeyPressed || (event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left))
                {
                    att->hp+=heal;
                    heal=0;
                }
            }

            fulldisplay();

            clk.restart();
            while(clk.getElapsedTime().asMilliseconds()<20);
        }

        return;
    }

    if(att->moveset[i]==gotomove("Explosion")) att->hp=0;

    if(att->moveset[i]->category)
    {
        a++;
        d++;
    }

    if(hit(att->moveset[i]))
    {
        int effect=typefx(def->dex, att->moveset[i]), crit=criticalhit(att->dex, att->moveset[i])*(!!effect), rnd=217+rand()%39;
        if(att->moveset[i]==gotomove("Flying Press"))
        {
            effect*=typefx(def->dex, gotomove("Aerial Ace"));
            effect/=4;
        }

        if(crit && !att->moveset[i]->multistrike)
        {
            textdisplay("It was a critical hit!");
            msg="";
        }

        if(!effect)
        {
            textdisplay("It had no effect!");
            msg="";
        }
        else if(effect>4)
        {
            textdisplay("It was super-effective!");
            msg="";
        }
        else if(effect<4)
        {
            textdisplay("It was not very effective!");
            msg="";
        }

        //printf("Power: %d\nAttack: %d\nDefense: %d\nRandom: %d", att->moveset[i]->power, att->stat[a], def->stat[d], rnd); //This is for checking calculations

        damage=floor((0.84*(att->moveset[i]->power)*(att->stat[a])/(def->stat[d])+2)*effect*(1+0.5*stab(att->dex, att->moveset[i]))*(1+crit)*rnd/1020);
        if(att->moveset[i]==gotomove("Seismic Toss")) damage=100*(!!effect)*(1+crit);

        if(att->moveset[i]->multistrike)
        {
            int nhit=2, ncrit=crit, temp;

            switch(rand()%8)
            {
            case 0:
                nhit++; //12.5% chance of 5 hits
            case 1:
                nhit++; //12.5% chance of 4 hits
            case 2:
            case 3:
            case 4:
                nhit++; //37.5% chance of 3 hits
                // 37.5% chance of 2 hits
            }

            temp=nhit-1;

            while(temp--)
            {
                if(damage>def->hp)
                {
                    damage=def->hp;
                    nhit-=temp+1;
                    break;
                }

                crit=criticalhit(att->dex, att->moveset[i])*(!!effect);
                ncrit+=crit;
                rnd=217+rand()%39;

                //printf("Power: %d\nAttack: %d\nDefense: %d\nRandom: %d", att->moveset[i]->power, att->stat[a], def->stat[d], rnd); //This is for checking calculations

                damage+=floor((0.84*(att->moveset[i]->power)*(att->stat[a])/(def->stat[d])+2)*effect*(1+0.5*stab(att->dex, att->moveset[i]))*(1+crit)*rnd/1020);
            }

            if(nhit>1)
            {
                std::string str(att->moveset[i]->name);
                textdisplay(str+" hit "+std::to_string(nhit)+" times!");
                msg="";
            }

            else
            {
                std::string str(att->moveset[i]->name);
                textdisplay(str+" hit 1 time!");
                msg="";
            }

            if(ncrit)
            {
                std::string str=std::to_string(ncrit)+" critical hit";
                if(ncrit>1) str=str+"s";
                textdisplay(str+"!");
                msg="";
            }
        }
    }

    else
    {
        textdisplay("The attack missed!");
        msg="";
    }

    if(damage>def->hp) damage=def->hp;
    int frame=damage;

    while(frame--)
    {
        def->hp--;

        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
            {
                window.close();
                return;
            }

            if(event.type==sf::Event::KeyPressed || (event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left))
            {
                def->hp-=frame;
                frame=0;
            }
        }

        fulldisplay();

        clk.restart();
        while(clk.getElapsedTime().asMilliseconds()<20);
    }

    if(i==4) damage=att->maxhp/4;
    else if(att->moveset[i]==gotomove("Take Down") || att->moveset[i]==gotomove("Wild Charge") || att->moveset[i]==gotomove("Submission")) damage/=4;
    else if(att->moveset[i]==gotomove("Double Edge") || att->moveset[i]==gotomove("Wood Hammer") || att->moveset[i]==gotomove("Flare Blitz") || att->moveset[i]==gotomove("Volt Tackle") || att->moveset[i]==gotomove("Brave Bird")) damage/=3;
    else if(att->moveset[i]==gotomove("Head Smash")) damage/=2;
    else damage=0;

    if(damage)
    {
        std::string str(att->dex->name);
        textdisplay(str+" took recoil damage!");
        msg="";
    }

    if(damage>att->hp) damage=att->hp;
    frame=damage;

    while(frame--)
    {
        att->hp--;

        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
            {
                window.close();
                return;
            }

            if(event.type==sf::Event::KeyPressed || (event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left))
            {
                att->hp-=frame;
                frame=0;
            }
        }

        fulldisplay();

        clk.restart();
        while(clk.getElapsedTime().asMilliseconds()<20);
    }
}

bool lose(player *trainer)
{
    int trh=0; //TotalRemainingHP of all Pokemon

    for(int i=0; i<6; i++) trh+=trainer->team[i].hp;

    return !trh;
}

void addBG(int bg)
{
    if(!window.isOpen()) return;
    window.clear();

    sf::Texture texture;
    texture.loadFromFile("img/bg/"+std::to_string(bg%29)+".jpg");
    sf::Sprite sprite;
    sprite.setTexture(texture);

    window.draw(sprite);
    window.display();
}

void labbg(void)
{
    if(!window.isOpen()) return;
    window.clear();

    sf::Texture texture;
    texture.loadFromFile("img/prof.jpg");
    sf::Sprite sprite;
    sprite.setTexture(texture);

    window.draw(sprite);
}

void drawsprite(bool pos, int id)
{
    if(!window.isOpen()) return;

    pokeprofile *species;
    species=trainer[pos].team[id].dex;

    sf::Text text;
    text.setFont(arial);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    text.setOutlineThickness(2);
    text.setOutlineColor(sf::Color::Black);
    text.setStyle(sf::Text::Bold);

    std::string str(trainer[pos].team[id].dex->name);
    text.setString(str+"\nHP: "+std::to_string(trainer[pos].team[id].hp)+"/"+std::to_string(trainer[pos].team[id].maxhp));

    str="img/sprites/"+std::to_string(species->dexid);
    if(trainer[pos].team[id].form) str=str+"a";
    if(trainer[pos].team[id].form>1) str=str+std::to_string(trainer[pos].team[id].form);
    if(pos) str=str+"b";

    sf::Texture texture;
    texture.loadFromFile(str+".png");
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setOrigin(120, 120);
    sprite.scale(2, 2);

    text.setPosition(800, 250);
    sprite.setPosition(840, 450);

    if(pos)
    {
        text.move(-400, 0);
        sprite.move(-400, 0);
    }

    if(raised(species))
    {
        text.move(0, -200);
        sprite.move(0, -200);
    }

    if(species==gotoPokemon("Mantine") || species==gotoPokemon("Metagross") || species==gotoPokemon("Escavalier") || species==gotoPokemon("Eelektross"))
    {
        sprite.move(0, 100);
        text.move(0, 100);
    }
    else if(species==gotoPokemon("Stunfisk")) sprite.move(0, 80);

    window.display();
    window.draw(sprite);
    window.draw(text);
    window.display();
}

void drawtrainer(bool pos)
{
    if(!window.isOpen()) return;

    sf::Text text;
    text.setFont(arial);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    text.setOutlineThickness(2);
    text.setOutlineColor(sf::Color::Black);
    text.setStyle(sf::Text::Bold);

    std::string str(trainer[pos].name);
    if(str.length()>10)
    {
        str[9]='.'; str[10]='.'; str[11]=0;
    }
    text.setString(str);

    sf::Texture texture;
    texture.loadFromFile("img/trainer/"+std::to_string(trainer[pos].photo)+".png");
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setOrigin(0.5*texture.getSize().x, texture.getSize().y);

    text.setPosition(1080, 220);
    sprite.setPosition(1120, 520);

    if(pos)
    {
        text.move(-960, 0);
        sprite.move(-960, 0);
    }

    window.display();
    window.draw(sprite);
    window.draw(text);
    window.display();
}

void drawtext(void)
{
    if(!window.isOpen()) return;

    sf::Text text;
    text.setFont(arial);
    text.setString(msg);

    text.setCharacterSize(30);
    text.setFillColor(sf::Color::White);
    text.setOutlineThickness(2);
    text.setOutlineColor(sf::Color::Black);
    text.setStyle(sf::Text::Bold);
    text.setPosition(120, 560);

    window.display();
    window.draw(text);
    window.display();
}

void dialoguebox(void)
{
    if(!window.isOpen()) return;

    sf::Text text;
    text.setFont(gameboy);
    text.setString(msg);

    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);
    text.setPosition(310, 560);
    text.setLineSpacing(1.5);

    for(int i=0; i<msg.length(); i++)
    {
        if(text.findCharacterPos(i).x>940)
        {
            while(i && msg[i]-' ') i--;
            if(i)
            {
                msg=msg.substr(0, i)+"\n"+msg.substr(i+1, msg.length());
                text.setString(msg);
            }

            else break;
        }
    }

    window.draw(text);
}

void puttext(std::string txt, int x, int y, int size)
{
    if(!window.isOpen()) return;

    sf::Text text;
    text.setString(txt);
    text.setFont(arial);
    text.setCharacterSize(size);
    text.setFillColor(sf::Color::White);
    text.setOutlineThickness(size/12);
    text.setOutlineColor(sf::Color::Black);
    text.setStyle(sf::Text::Bold);
    text.setPosition(x, y);

    std::string str=txt;
    bool flag=0;

    for(int i=0; i<str.length(); i++)
    {
        if(str[i]=='\n') flag=1;

        if(text.findCharacterPos(i).x>1160)
        {
            if(!flag)
            {
                puttext(txt, x, y, size-1);
                return;
            }

            while(i && str[i]-' ') i--;

            if(i)
            {
                str=str.substr(0, i)+"\n"+str.substr(i+1, str.length());
                text.setString(str);
            }
            else break;
        }

        if(text.findCharacterPos(i).y>600)
        {
            puttext(txt, x, y, size-1);
            return;
        }
    }

    window.draw(text);
}

void textdisplay(std::string str)
{
    msg="";
    clk.restart();

    for(int i=1; i<=str.length(); i++, clk.restart())
    {
        msg=str.substr(0, i);
        while(clk.getElapsedTime().asMilliseconds()<20);

        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
            {
                window.close();
                return;
            }

            if(event.type==sf::Event::KeyPressed || (event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left))
            {
                msg=str;
                i=str.length();
            }
        }

        fulldisplay();

        if(i==str.length())
        {
            clk.restart();
            while(clk.getElapsedTime().asSeconds()<0.5)
            {
                while(window.pollEvent(event))
                {
                    if(event.type==sf::Event::Closed)
                    {
                        window.close();
                        return;
                    }

                    if(event.type==sf::Event::Resized) fulldisplay();
                    if(event.type==sf::Event::KeyPressed || (event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left)) return;
                }
            }
        }
    }
}

void boxdisplay(std::string str)
{
    if(!window.isOpen()) return;
    msg="";
    clk.restart();

    for(int i=1; i<=str.length(); i++, clk.restart())
    {
        msg=str.substr(0, i);
        while(clk.getElapsedTime().asMilliseconds()<20);

        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
            {
                window.close();
                return;
            }

            if(event.type==sf::Event::KeyPressed || (event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left))
            {
                msg=str;
                i=str.length();
            }
        }

        labdisplay();

        if(i==str.length())
        {
            clk.restart();
            while(clk.getElapsedTime().asSeconds()<0.5)
            {
                while(window.pollEvent(event))
                {
                    if(event.type==sf::Event::Closed)
                    {
                        window.close();
                        return;
                    }

                    if(event.type==sf::Event::Resized) labdisplay();
                    if(event.type==sf::Event::KeyPressed || (event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left)) return;
                }
            }
        }
    }
}

void drawoption(std::string str1, std::string str2, std::string str3, std::string str4, int highlight)
{
    if(!window.isOpen()) return;

    sf::Text op1, op2, op3, op4;
    op1.setFont(arial); op2.setFont(arial); op3.setFont(arial); op4.setFont(arial);
    op1.setString("1. "+str1); op2.setString("2. "+str2); op3.setString("3. "+str3); op4.setString("4. "+str4);

    op1.setCharacterSize(30); op2.setCharacterSize(30); op3.setCharacterSize(30); op4.setCharacterSize(30);
    if(highlight==1) op1.setCharacterSize(36);
    else if(highlight==2) op2.setCharacterSize(36);
    else if(highlight==3) op3.setCharacterSize(36);
    else if(highlight==4) op4.setCharacterSize(36);

    op1.setFillColor(sf::Color::White); op2.setFillColor(sf::Color::White); op3.setFillColor(sf::Color::White); op4.setFillColor(sf::Color::White);
    op1.setOutlineThickness(2); op2.setOutlineThickness(2); op3.setOutlineThickness(2); op4.setOutlineThickness(2);
    op1.setOutlineColor(sf::Color::Black); op2.setOutlineColor(sf::Color::Black); op3.setOutlineColor(sf::Color::Black); op4.setOutlineColor(sf::Color::Black);
    op1.setStyle(sf::Text::Bold); op2.setStyle(sf::Text::Bold); op3.setStyle(sf::Text::Bold); op4.setStyle(sf::Text::Bold);
    op1.setPosition(120, 600); op2.setPosition(760, 600); op3.setPosition(120, 650); op4.setPosition(760, 650);

    window.display();
    window.draw(op1); window.draw(op2); window.draw(op3); window.draw(op4);
    window.display();
}

void typechart(void)
{
    if(!window.isOpen()) return;

    sf::Texture texture;
    sf::Sprite sprite;

    texture.loadFromFile("img/TypeChart.png");
    sprite.setTexture(texture);
    sprite.move(280, 0);

    window.clear(sf::Color::White);
    window.draw(sprite);
    window.display();

    bool flag=1;
    clk.restart();
    while(clk.getElapsedTime().asSeconds()<10 && flag && window.isOpen())
    {
        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed) window.close();
            if(event.type==sf::Event::KeyPressed || (event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left)) flag=0;
        }
    }
}

void teamdisplay(bool pos, int highlight)
{
    if(!window.isOpen()) return;

    for(int i=0; i<6; i++)
    {
        std::string str;
        str="img/sprites/"+std::to_string(trainer[pos].team[i].dex->dexid);
        if(trainer[pos].team[i].form) str=str+"a";
        if(trainer[pos].team[i].form>1) str=str+std::to_string(trainer[pos].team[i].form);

        sf::Texture texture;
        texture.loadFromFile(str+".png");
        sf::Sprite sprite;
        sprite.setTexture(texture);
        sprite.setOrigin(120, 120);
        if(i==highlight) sprite.scale(1.25, 1.25);
        sprite.setPosition(140+200*i, 650);
        if(!trainer[pos].team[i].hp) sprite.setColor(sf::Color::Black);

        window.display();
        window.draw(sprite);
        window.display();
    }
}

void fulldisplay(void)
{
    if(!window.isOpen()) return;

    addBG(bg);
    drawtrainer(1);
    drawtrainer(0);
    if(active[1]<6) if(trainer[1].team[active[1]].hp) drawsprite(1, active[1]);
    if(active[0]<6) if(trainer[0].team[active[0]].hp) drawsprite(0, active[0]);
    drawtext();
}

void labdisplay(void)
{
    if(!window.isOpen()) return;

    labbg();
    dialoguebox();

    window.display();
}

void screenscanf(int x, int y, int limit)
{
    if(!window.isOpen()) return;
    std::string temp=msg;

    sf::Text text;
    text.setFont(gameboy);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);

    text.setPosition(x, y);
    bool flag=1;
    while(flag && window.isOpen())
    {
        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed) window.close();

            else if(event.type==sf::Event::Resized) labdisplay();

            else if(event.type==sf::Event::TextEntered && ((event.text.unicode>='a' && event.text.unicode<='z') || (event.text.unicode>='A' && event.text.unicode<='Z') || (event.text.unicode>='0' && event.text.unicode<='9') || event.text.unicode==' '))
            {
                std::string str="0";
                str[0]=event.text.unicode;

                if(msg.length()-temp.length()<limit)
                {
                    msg=msg+str;
                    text.setString(msg);
                }

                labdisplay();
            }

            else if(event.type==sf::Event::KeyPressed && event.key.code==sf::Keyboard::Enter)
            {
                msg=msg.substr(temp.length(), msg.length());
                flag=0;
            }

            else if(event.type==sf::Event::KeyPressed)
            {
                if(event.key.code==sf::Keyboard::Backspace && msg.length()>temp.length()) msg=msg.substr(0, msg.length()-1);
                else if(event.key.code==sf::Keyboard::Escape) msg=temp;

                labdisplay();
            }
        }
    }
}

void dexbeep(void)
{
    if(!window.isOpen()) return;

    sf::Texture texture;
    texture.loadFromFile("img/dex.jpg");
    sf::Sprite sprite;
    sprite.setTexture(texture);

    bool track=(music[1].getStatus()==sf::Music::Playing);
    music[track].pause();

    sf::Music dex;
    dex.openFromFile("audio/cry/dex.ogg");
    dex.setLoop(0);
    dex.play();

    while(dex.getStatus()==sf::Music::Playing)
    {
        window.clear();
        window.draw(sprite);
        window.display();

        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
            {
                window.close();
                return;
            }

            if(event.type==sf::Event::KeyPressed || (event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left)) dex.stop();
        }
    }

    music[track].play();
}
