#include<GL/glut.h>
#include<math.h>
#include<chrono>
#include<iostream>
#include<thread>
#include<string>
#include<functional>
#include<curl/curl.h> // for http request | NOTE: if you are on windows look up how to install libcurl | NOTE: comment this line and set the below variable to false if you dont want the leaderboard to work
bool curl_included = true;
#include"tools.h"

// objects
Mouse mouse;
Clock CLOCK = Clock();

bool pause = false;

class Menu
{
public:
    // title
    std::string title =  "Flying Dino";
    Point title_pos = Point(SW/2, SH - SH * 0.2);
    Color title_color = Color(245,174,45);
    double title_scale = 1.0;
    int title_thickness = 10;

    // particles effect
    ConnectingCells right_owo = ConnectingCells(Rect(Point(-20, -20), SW/4+20, SH+40, 'c'));
    ConnectingCells left_owo = ConnectingCells(Rect(Point(SW+20, -20), SW/4+20, SH+40, 'd'));


    // buttons
    int btn_width = 380;
    int btn_height = 75;
    double btns_gap = btn_height + 20;

    // creating the buttons array
    Button btns[4] = {
                        Button(Point(SW/2, SH/2 - btns_gap * 0), btn_width, btn_height, "PLAY"),
                        Button(Point(SW/2, SH/2 - btns_gap * 1), btn_width, btn_height, "SETTINGS"),
                        Button(Point(SW/2, SH/2 - btns_gap * 2), btn_width, btn_height, "LEADERBOARD"),
                        Button(Point(SW/2, SH/2 - btns_gap * 3), btn_width, btn_height, "QUIT"),
                    };
    // buttons pointers
    Button* play = &btns[0];
    Button* settings = &btns[1];
    Button* leaderboard = &btns[2];
    Button* quit = &btns[3];

    // variables for managing the layout ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  ( not a very good approach too much repeated code i should have used a lambda )
    // play
    Point play_ipos = play->center; // play button inital position
    Point play_fpos = settings->center; // play button final position
    // settings
    Point settings_size = Point(settings->width, settings->height);
    Point settings_menu_size = Point(settings->width * 1.1, SH * 0.72);
    bool settings_activate;  // if true means the settings menu is activating
    bool settings_menu;      // if true means the settings menu is fully activated
    bool settings_can_retrieve; // if true means the settings menu can retrieve | if settings menu is false and this is true the menu will retrive back to being a button
    ExitMark settings_x = ExitMark(Point(SW*0.30, SH*0.68), [&](){settings_menu = false;});

    // quit
    Point quit_ipos = quit->center; // quit button inital position
    Point quit_fpos = leaderboard->center; // quit button final position
    // leaderboard
    Point leaderboard_size = Point(leaderboard->width, leaderboard->height);
    Point leaderboard_menu_size = settings_menu_size;
    bool leaderboard_activate;  // if true means the leaderboard menu is activating
    bool leaderboard_menu;      // if true means the leaderboard menu is fully activated
    bool leaderboard_can_retrieve; // if true means the leaderboard menu can retrieve | if leaderboard menu is false and this is true the menu will retrive back to being a button
    ExitMark leaderboard_x = ExitMark(Point(SW - SW*0.30, SH*0.68), [&](){leaderboard_menu = false;});
    bool stop_going_right;

    // constructor
    Menu()
    {
        // assigning functions (lambdas) to our buttons
        btns[0].func = [&](){std::cout<<"play!"<<"\n";};
        btns[1].func = [&](){std::cout<<"settings!"<<"\n"; settings_activate = true; settings->clickable = false;};
        btns[2].func = [&](){std::cout<<"leaderboard!"<<"\n"; leaderboard_activate = true; leaderboard->clickable = false;};
        btns[3].func = [&](){std::cout<<"quit!"<<"\n"; exit(0);};
    }


    void manage_layout()
    {
        // settings and play button management ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if(settings_activate)
        {
            // moving the settings button
            if(settings->center.morphTo(Point(SW/6,settings->center.y), 1600))
            {
                settings->rect = Rect(settings->center, settings->width, settings->height, 'e');
            }
            else
            {
                // moving the player button
                if (play->center.morphTo(play_fpos, 450))
                    play->rect = Rect(play->center, play->width, play->height, 'e');
                // increasing the size of the settings button to become the settings menu plate
                if (settings_size.morphTo(settings_menu_size, 1500))
                    settings->rect = Rect(settings->center, settings_size.x, settings_size.y, 'e');
                else // if its fully increased
                {
                    settings_menu = true;
                    settings_can_retrieve = true;
                    settings_activate = false;
                }
            }
        }

        if(settings_can_retrieve && !settings_menu)
        {
            if(settings_size.morphTo(Point(settings->width, settings->height), 1200))
                settings->rect = Rect(settings->center, settings_size.x, settings_size.y, 'e');
            else // if it fully shrinks
            {
                if (play->center.morphTo(play_ipos, 450))
                        play->rect = Rect(play->center, play->width, play->height, 'e');
                else
                    if(settings->center.morphTo(Point(SW/2,settings->center.y), 1600))
                        settings->rect = Rect(settings->center, settings->width, settings->height, 'e');
                    else
                        {
                            settings_menu = false;
                            settings_can_retrieve = false;
                            settings_activate = false;
                            settings->clickable = true;

                        }
            }
        }

        // leaderboard and quit button management ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if(leaderboard_activate)
        {
            // moving the leaderboard button
            if(!stop_going_right && leaderboard->center.morphTo(Point(SW - SW/6,leaderboard->center.y), 1600))
            {
                leaderboard->rect = Rect(leaderboard->center, leaderboard->width, leaderboard->height, 'e');
            }
            else // if it has reached the final position
            {
                stop_going_right = true;
                // moving the leaderboard a pos 1 button up
                if(leaderboard->center.morphTo(Point(leaderboard->center.x, play_fpos.y), 900))  // finally pos of play is the inital position of the settings button
                        leaderboard->rect = Rect(leaderboard->center, leaderboard_size.x, leaderboard_size.y, 'e');

                // moving the quit button
                if (quit->center.morphTo(quit_fpos, 450))
                    quit->rect = Rect(quit->center, quit->width, quit->height, 'e');

                else // increasing the size of the leaderboard button to become the leaderboard menu plate
                    if (leaderboard_size.morphTo(leaderboard_menu_size, 1500))
                        leaderboard->rect = Rect(leaderboard->center, leaderboard_size.x, leaderboard_size.y, 'e');
                    else // if its fully increased
                    {
                        leaderboard_menu = true;
                        leaderboard_can_retrieve = true;
                        leaderboard_activate = false;
                    }
            }
        }

        if(leaderboard_can_retrieve && !leaderboard_menu)
        {
            if(leaderboard_size.morphTo(Point(leaderboard->width, leaderboard->height), 1200))
                leaderboard->rect = Rect(leaderboard->center, leaderboard_size.x, leaderboard_size.y, 'e');
            else // if it fully shrinks
            {
                // moving the leaderboard a pos 1 button down
                if(leaderboard->center.morphTo(Point(leaderboard->center.x, quit_fpos.y), 450))  // finall pos of quit is the inital position of the leaderboard button
                        leaderboard->rect = Rect(leaderboard->center, leaderboard_size.x, leaderboard_size.y, 'e');
                //else
                if (quit->center.morphTo(quit_ipos, 450))
                    quit->rect = Rect(quit->center, quit->width, quit->height, 'e');
                else
                    if(leaderboard->center.morphTo(Point(SW/2,leaderboard->center.y), 1600))
                        leaderboard->rect = Rect(leaderboard->center, leaderboard->width, leaderboard->height, 'e');
                    else
                        {
                            leaderboard_menu = false;
                            leaderboard_can_retrieve = false;
                            leaderboard_activate = false;
                            leaderboard->clickable = true;
                            stop_going_right = false;  // the leaderboard button will be able to move right
                        }
            }
        }



    } // manage_layout end

    void draw()
    {
        // draw title
        Text(title, title_pos, title_color, title_scale, title_thickness).draw();

        // draw btns
        for(Button &btn : btns)
            btn.draw();

        // draw particles
        right_owo.draw();
        left_owo.draw();

        // draw the x buttons
        if (settings_menu) settings_x.draw();
        if (leaderboard_menu) leaderboard_x.draw();

    }

    void update()
    {
        // update btns
        for(Button &btn : btns)
            btn.update(mouse);
        // manage the buttons layout animations
        manage_layout();
        // update x buttons
        if(settings_menu) settings_x.update(mouse);
        if(leaderboard_menu) leaderboard_x.update(mouse);
        // update particles
        right_owo.update();
        left_owo.update();
    }

}; // menu end
Menu menu;

Color SC(15,12,19);  // screen color
void display()
{
    glClearColor(SC.r/255, SC.g/255, SC.b/255, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw
    menu.draw();

    glutSwapBuffers();

}

void idle()
{
    // update delta time
    CLOCK.tick();
    // update menu
    if (!pause)
        menu.update();


    // reset mouse
    mouse.reset();
    // redraw
    glutPostRedisplay();
}

void mouse_passive_motion(int x, int y)
{
    // update mouse position
    mouse.pos.x = x;
    mouse.pos.y = SH - y;
}

void mouse_motion(int x, int y)
{
    mouse_passive_motion(x, y);
}

void mouse_btns(int btn, int state, int x, int y)
{
    mouse.take_input(btn, state);
}

void keyboard(unsigned char c, int x, int y)
{
    if(char(c) == ' ')
        pause = !pause;
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(SW, SH);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutCreateWindow("owo");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    gluOrtho2D(0, SW, 0, SH);

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutPassiveMotionFunc(mouse_passive_motion);
    glutMotionFunc(mouse_motion);
    glutMouseFunc(mouse_btns);
    glutKeyboardFunc(keyboard);


    glutMainLoop();

}
