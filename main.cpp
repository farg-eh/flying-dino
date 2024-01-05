#include<GL/glut.h>
#include<math.h>
#include<chrono>
#include<iostream>
#include<thread>
#include<string>
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

    // buttons
    int btn_width = 380;
    int btn_height = 75;
    double btns_gap = btn_height + 20;
    Button btns[4] = {
                        Button(Point(SW/2, SH/2 - btns_gap * 0), btn_width, btn_height, "PLAY"),
                        Button(Point(SW/2, SH/2 - btns_gap * 1), btn_width, btn_height, "LEADERBOARD"),
                        Button(Point(SW/2, SH/2 - btns_gap * 2), btn_width, btn_height, "SETTINGS"),
                        Button(Point(SW/2, SH/2 - btns_gap * 3), btn_width, btn_height, "QUIT"),
                    };
    // constructor
    Menu()
    {
        // assigning function pointers to our btns
        btns[0].func = []()->void{std::cout<<"play!\n";};
    }

    // particles effect
    ConnectingCells right_owo = ConnectingCells(Rect(Point(-20, -20), SW/4+20, SH+40, 'c'));
    ConnectingCells left_owo = ConnectingCells(Rect(Point(SW+20, -20), SW/4+20, SH+40, 'd'));


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
    }

    void update()
    {
        // update btns
        for(Button &btn : btns)
            btn.update(mouse);

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
