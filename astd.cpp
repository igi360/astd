#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include <iostream>  //<------------streams hihi
#include "MainMenu.h"
#include <stdexcept>
#include <fstream>

using namespace sf;
using namespace std;

const int W = 1200;
const int H = 800;

float deg = 0.017453f;//convrsion from radians coef

long int score = 0;
int previousScore;

class Animation
{
public:
    float Frame, speed;
    Sprite sprite;
    vector<IntRect> frames;

    Animation() {}

    Animation(Texture& t, int x, int y, int w, int h, int count, float Speed)
    {
        Frame = 0;
        speed = Speed;

        for (int i = 0; i < count; i++)
            frames.push_back(IntRect(x + i * w, y, w, h));

        sprite.setTexture(t);
        sprite.setOrigin(w / 2, h / 2);
        sprite.setTextureRect(frames[0]);
    }


    void update()
    {
        Frame += speed;
        int n = frames.size();
        if (Frame >= n) Frame -= n;
        if (n > 0) sprite.setTextureRect(frames[int(Frame)]);
    }

    bool isEnd()
    {
        return Frame + speed >= frames.size();
    }

};

//objects class
class Entity
{
public:
    float x, y, dx, dy, R, angle;
    bool life;
    string name;
    Animation anim;

    Entity()
    {
        life = 1;
    }

    void settings(Animation& a, int X, int Y, float Angle = 0, int radius = 1)
    {
        anim = a;
        x = X; y = Y;
        angle = Angle;
        R = radius;
    }

    virtual void update() {};
    //setting up hitboxes
    void draw(RenderWindow& app)
    {
        anim.sprite.setPosition(x, y);
        anim.sprite.setRotation(angle + 90);
        app.draw(anim.sprite);

        CircleShape circle(R);
        circle.setFillColor(Color(255, 0, 0, 170));
        circle.setPosition(x, y);
        circle.setOrigin(R, R);

        // to see hitboxes
        //      ^
        //      |
        //      |
        //app.draw(circle);
    }

    virtual ~Entity() {};
};
//movement of asteroids

class asteroid : public Entity                     //<----Using classes is called polimorphism
{
public:
    asteroid()
    {
        dx = rand() % 7 - 4;
        dy = rand() % 7 - 4;
        name = "asteroid";
    }

    void update()
    {
        x += dx;
        y += dy;

        if (x > W) x = 0;  if (x < 0) x = W;
        if (y > H) y = 0;  if (y < 0) y = H;
    }

};
double divide(int u1, int u2)                   //exeption part1
{
    if (u2 == 0)
    {
        throw range_error("Division by zero");
    }
    return u1 / u2;
}
//trajectory for bullet
class bullet : public Entity//, public MainMenu
{

private:
    int GameMode;
public:
    int value;
    void MainMenuPressed(){
        int o;
    }
    void setValue(int v) {
        GameMode = v;
    }
    
   
    bullet()
    {
        name = "bullet";
    }

    void update()
    {

       // value = g;
        dx = cos(angle * deg) * 6;
        dy = sin(angle * deg) * 6;

        //if (value == 1)
        //{
        //    angle += rand() % 7 - 3;  //for hard mode
        //    x += dx;
        //    y += dy;
        //    if (x > W || x<0 || y>H || y < 0) life = 0;
        //}
        //else
        //{
        x += dx;
        y += dy;

        if (x > W || x<0 || y>H || y < 0) life = 0;

    }

    

};

//trajectory of player
class player : public Entity
{
public:
    bool thrust;

    player()                                              //<--------------using constructors is called multiple inharitance
    {
        name = "player";
    }

    void update()
    {
        if (thrust)
        {
            dx += cos(angle * deg) * 0.2;
            dy += sin(angle * deg) * 0.2;
        }
        else
        {
            dx *= 0.99;
            dy *= 0.99;
        }

        int maxSpeed = 15;
        float speed = sqrt(dx * dx + dy * dy);
        if (speed > maxSpeed)
        {
            dx *= maxSpeed / speed;
            dy *= maxSpeed / speed;
        }

        x += dx;
        y += dy;

        if (x > W) x = 0; if (x < 0) x = W;
        if (y > H) y = 0; if (y < 0) y = H;
    }

};



bool isCollide(Entity* a, Entity* b)
{
    return (b->x - a->x) * (b->x - a->x) +
        (b->y - a->y) * (b->y - a->y) <
        (a->R + b->R) * (a->R + b->R);
}


int main()
{
    int counter = 0;
    srand(time(0));
    //rendering main window
    RenderWindow MENU(VideoMode(1200, 800), "Main Menu", Style::Default);
    MainMenu mainMenu(MENU.getSize().x, MENU.getSize().y);



    //space for loading graphics
    Texture t1, t2, t3, t4, t5, t6, t7, t8;
    t1.loadFromFile("spaceship.png");
    t2.loadFromFile("background.jpg");
    t3.loadFromFile("explosions/type_C.png");
    t4.loadFromFile("rock.png");

    t5.loadFromFile("fire.png");
    t6.loadFromFile("rock_small.png");
    t7.loadFromFile("explosions/type_B.png");
    t8.loadFromFile("MainMenuBackground.jpg");
    Font fnt;
    fnt.loadFromFile("arial.ttf");
    t1.setSmooth(true);
    t2.setSmooth(true);
    
    Text HighScore;
    HighScore.setFont(fnt);
    HighScore.setString("High Score: " + to_string(previousScore));
    HighScore.setCharacterSize(50);
    HighScore.setPosition(400, 400);
    HighScore.setFillColor(Color::Red);

    Text Score;
    Score.setFont(fnt);
    //Score.setString("Your Score: " + to_string(score));
    Score.setCharacterSize(25);
    Score.setPosition(40, 40);
    Score.setFillColor(Color::Red);

    //Animations stuff
    Sprite background(t2);
    Sprite MenuBackground(t8);

    Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
    Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
    Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
    Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
    Animation sPlayer(t1, 64, 0, 64, 64, 1, 0);
    Animation sPlayer_go(t1, 0, 0, 64, 64, 1, 0);
    Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);


    list<Entity*> entities;

    for (int i = 0; i < 15; i++)
    {
        asteroid* a = new asteroid();
        a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
        entities.push_back(a);
    }
    int u1 = 1, u2 = 1;
    try                                                //<----------------eception catch
    {
       cout << divide(u1, u2) << std::endl;
    }
    catch (const range_error& e)
    {
        cerr << "Error: " << e.what() << std::endl;
    }
    player* p = new player();
    p->settings(sPlayer, 600, 400, 0, 20);
    entities.push_back(p);
    bullet mode;
    int newValue;

    /////main loop/////
    while (MENU.isOpen()) {
        Event event;
        while (MENU.pollEvent(event)) {
            if (event.type == Event::Closed) {
                MENU.close();
            }
            if (event.type == Event::KeyReleased) {

                if (event.key.code == Keyboard::Up) {
                    mainMenu.MoveUp();
                    break;
                }

                if (event.key.code == Keyboard::Down) {
                    mainMenu.MoveDown();
                    break;
                }
                if (event.key.code == Keyboard::Return) {
                    RenderWindow Play(VideoMode(1200, 800), "Asteroids");
                    Play.setFramerateLimit(60);
                    RenderWindow Options(VideoMode(1200, 800), "Instructions");
                    RenderWindow About(VideoMode(1200, 800), "High Score");

                    int x = mainMenu.MainMenuPressed();

                    if (x == 0) {

                        // bullet.update *g = 1;
                        // g ->update();
                           // g=inputValue

                         /*if (x == 0)
                         {
                             mode.setValue(0);
                         }*/
                        while (Play.isOpen())
                        {
                            Event aevent;
                            while (Play.pollEvent(aevent))
                            {
                                if (aevent.type == Event::Closed) {
                                    Play.close();
                                }
                                counter++;
                                if (aevent.type == Event::KeyPressed)
                                {
                                    if (aevent.key.code == Keyboard::Escape)
                                    {
                                        
                                        Play.close();
                                    }

                                    if (aevent.key.code == Keyboard::Space)
                                    {
                                        bullet* b = new bullet();
                                        b->settings(sBullet, p->x, p->y, p->angle, 10);
                                        entities.push_back(b);
                                    }
                                }
                            }
                            
                            if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 3;
                            if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle -= 3;
                            if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust = true;
                            else p->thrust = false;

                            Score.setString("Your Score: " + to_string(score));
                            for (auto a : entities)
                                for (auto b : entities)
                                {
                                    if (a->name == "asteroid" && b->name == "bullet")
                                        if (isCollide(a, b))
                                        {

                                            
                                            a->life = false;
                                            b->life = false;

                                            Entity* e = new Entity();
                                            e->settings(sExplosion, a->x, a->y);
                                            e->name = "explosion";
                                            entities.push_back(e);


                                            for (int i = 0; i < 2; i++)
                                            {
                                                if (a->R == 15) continue;
                                                Entity* e = new asteroid();
                                                e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
                                                entities.push_back(e);
                                            }
                                            score = score + 50;                              //<----------High score
                                            ifstream input_file("score.txt");

                                            if (input_file.is_open())
                                            {
                                                input_file >> previousScore;

                                                input_file.close();
                                            }




                                            if (score > previousScore)
                                            {
                                                ofstream output_file("score.txt", ofstream::trunc);

                                                if (output_file.is_open())
                                                {
                                                    output_file << score;
                                                    output_file.close();
                                                }
                                                
                                            }
                                        }

                                    if (a->name == "player" && b->name == "asteroid")
                                        if (isCollide(a, b))
                                        {
                                            b->life = false;

                                            Entity* e = new Entity();
                                            e->settings(sExplosion_ship, a->x, a->y);
                                            e->name = "explosion";
                                            entities.push_back(e);

                                            p->settings(sPlayer, W / 2, H / 2, 0, 20);
                                            p->dx = 0; p->dy = 0;
                                        }
                                }


                            if (p->thrust)  p->anim = sPlayer_go;
                            else   p->anim = sPlayer;


                            for (auto e : entities)
                                if (e->name == "explosion")
                                    if (e->anim.isEnd()) e->life = 0;

                            if (rand() % 150 == 0)
                            {
                                asteroid* a = new asteroid();
                                a->settings(sRock, 0, rand() % H, rand() % 360, 25);
                                entities.push_back(a);
                            }

                            for (auto i = entities.begin(); i != entities.end();)
                            {
                                Entity* e = *i;

                                e->update();
                                e->anim.update();

                                if (e->life == false) { i = entities.erase(i); delete e; }
                                else i++;
                            }




                            //draw

                            Play.draw(background);
                            Play.draw(Score);
                            for (auto i : entities) i->draw(Play);
                            Play.display();
                            Options.close();
                            About.close();
                            Play.clear();
                        }
                    }

                    if (x == 1) {
                        //int GameMode = 0;
                        while (Options.isOpen())
                        {
                            Event aevent;
                            while (Options.pollEvent(aevent))
                            {
                                if (aevent.type == Event::Closed) {
                                    Options.close();
                                }
                                if (aevent.type == Event::KeyPressed)
                                {
                                    if (aevent.key.code == Keyboard::Escape)
                                    {
                                        Options.close();
                                    }
                                }
                            }
                            Options.clear();
                            About.close();
                            Play.close();
                            Options.draw(MenuBackground);
                            Options.display();

                        }

                        break;
                    }

                    if (x == 2) {
                        ifstream input_file("score.txt");
                        if (input_file.is_open())
                        {
                            input_file >> previousScore;

                            input_file.close();
                        }
                        cout << previousScore;
                        HighScore.setString("High Score: " + to_string(previousScore));
                        while (About.isOpen())
                        {
                            Event aevent;
                            while (About.pollEvent(aevent))
                            {
                                if (aevent.type == Event::Closed) {
                                    About.close();
                                }
                                if (aevent.type == Event::KeyPressed)
                                {
                                    if (aevent.key.code == Keyboard::Escape)
                                    {
                                        About.close();
                                    }
                                }
                                

                                
                            }
                            
                           
                            Play.close();
                            Options.clear();
                            About.clear();
                            About.draw(MenuBackground);
                            About.draw(HighScore);
                            //input_file.close();
                            About.display();
                        }
                    }
                    if (x == 3) {
                        MENU.close();
                        break;
                    }
                }
            }

        }
            MENU.clear();
            MENU.draw(MenuBackground);
            mainMenu.draw(MENU);
            MENU.display();
        
    }

    return 0;

}
