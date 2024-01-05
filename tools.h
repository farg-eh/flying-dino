
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ varaibles ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const double pi = 3.14159265358979323846;
int SW = 1280; // screen width
int  SH = 720; // screen height
double dt = 0; // delta time for frame rate independnece


// ~~~~~~~~~~~~~~~~~~~~~~ simple functions ~~~~~~~~~~~~~~~~~~~~~~~~~
int Max(int arr[], int Size)
{
    int Max = arr[0];

    for(int v = 0; v < Size; v++)
        if (arr[v] > Max)
            Max = arr[v];

    return Max;
}

double Max(double arr[], int Size)
{
    double Max = arr[0];

    for(int v = 0; v < Size; v++)
        if (arr[v] > Max)
            Max = arr[v];

    return Max;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ structs ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct Point //  a point to represent x and y coordiantes
{
    double x, y;
    Point(double x_pos = 0, double y_pos = 0) : x(x_pos), y(y_pos){}

    double distance(Point other) // finds the distance between our point and another point
    {
        return sqrt(pow(other.x - x, 2) + pow(other.y - y, 2));
    }

    double angle_between(Point other)  // measures the angle between our points and another point
    {
        int dx = other.x - x;
        int dy = other.y - y;
        double angle = atan2(dy, dx);
        return angle * 180/pi;  // returns in degrees
    }

    void draw()
    {
        glBegin(GL_POINTS);
        glVertex2d(x, y);
        glEnd();
    }

    void draw_line(Point other, bool arrow=false, double t = 0.5)
    {
        glBegin(GL_LINES);
        glVertex2d(x, y);
        glVertex2d(other.x, other.y);
        glEnd();

        if (!arrow)
            return;
        // draws an arrow
        double r = distance(other);
        double X = x + r * t * cos(pi/180 * (angle_between(other)));
        double Y = y + r * t * sin(pi/180 *(angle_between(other)));
        double r2 = 5;
        double X2 = X + r2 * cos(pi/180 *(angle_between(other) + 130));
        double Y2 = Y + r2 * sin(pi/180 *(angle_between(other) + 130));
        double X3 = X + r2 * cos(pi/180 *(angle_between(other) - 130));
        double Y3 = Y + r2 * sin(pi/180 *(angle_between(other) - 130));
        glBegin(GL_POLYGON);
        glVertex2d(X, Y);
        glVertex2d(X2, Y2);
        glVertex2d(X3, Y3);
        glEnd();
    }

    std::string str()  // returns a string object representing the points value
    {
        return std::string("(" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }

    // operators
    bool operator==(const Point& other)
    {
        return (x == other.x && y == other.y);
    }

}; // Point end


struct Color     // to represent RBG colors
{
    double r, g, b;

    // constructors
    Color(){r=0; g=0; b=0;}
    Color(double r, double g, double b) : r(r), g(g), b(b){}

    void set(double alpha=255)
    {
        glColor4ub(r, g, b, alpha);
    }



    int morphTo(Color x, int speed = 25) // a method that transfers the current color to another color | WARINING THIS METHOD NEED WORKS BUT NEEDS TO BE REMADE
    {
        double diff_arr[3] = {abs(r-x.r), abs(g-x.g), abs(b-x.b)}; // array of difference between color values
        double max_diff = Max(diff_arr, 3); // arr, size
        if (int(max_diff) == 0) return -1;  // if there is no difference this means the colors are equal

        // max diff/ diff to get difference ratios (steps) | (r > x.r)? -1:1 -> the step directions
        double steps[3] = {(max_diff/diff_arr[0])*(r > x.r)? -1:1,
                            (max_diff/diff_arr[1])*(g > x.g)? -1:1,
                            (max_diff/diff_arr[2])*(b > x.b)? -1:1};
        // move
        if(int(diff_arr[0]) > abs(steps[0] * speed * dt)) r += steps[0] * speed * dt;
        else r = x.r;
        if(int(diff_arr[1]) > abs(steps[1] * speed * dt)) g += steps[1] * speed * dt;
        else g = x.g;
        if(int(diff_arr[2]) > abs(steps[2] * speed * dt)) b += steps[2] * speed * dt;
        else b = x.b;
//      debugging
//      std::cout<<"R:"<<r<<" | step = " << steps[0] * speed * dt<<std::endl;
//      std::cout<<"G:"<<g<<" | step = " << steps[1] * speed * dt<<std::endl;
//      std::cout<<"B:"<<b<<" | step = " << steps[2] * speed * dt<<std::endl;

    } // morphTo  method end

}; // Color end


struct Rect
{
    Point topleft, topright, bottomleft, bottomright;
    double width , height;

    Rect(){width = 0; height = 0;}
    Rect(Point p, double width, double height, char point_address='a') : width(width), height(height)
    {
        switch(point_address)
        {
            case 'a': // topleft
                topleft = p;
                topright = Point(p.x + width, p.y);
                bottomleft = Point(p.x, p.y - height);
                bottomright = Point(p.x + width, p.y - height);
            break;

            case 'b': //topright
                topleft = Point(p.x - width, p.y);
                topright = p;
                bottomleft = Point(p.x - width, p.y-height);
                bottomright = Point(p.x, p.y - height);
            break;

            case 'c': // bottomleft
                topleft = Point(p.x, p.y + height);
                topright = Point(p.x + width, p.y + height);
                bottomleft = p;
                bottomright = Point(p.x + width, p.y);
            break;

            case 'd': // bottomright
                topleft = Point(p.x - width, p.y + height);
                topright = Point(p.x, p.y + height);
                bottomleft = Point(p.x - width, p.y);
                bottomright = p;
            break;

            case 'e': // center
                topleft = Point(p.x - width/2, p.y + height/2);
                topright = Point(topleft.x + width, topleft.y);
                bottomright = Point(topleft.x + width, topleft.y-height);
                bottomleft = Point(topleft.x, topleft.y-height);
            break;

            case 'f': // midtop
                topleft = Point(p.x - width/2, p.y);
                topright = Point(topleft.x + width, topleft.y);
                bottomright = Point(topleft.x + width, topleft.y-height);
                bottomleft = Point(topleft.x, topleft.y-height);
            break;

            case 'g': // midbottom
                topleft = Point(p.x - width/2, p.y + height);
                topright = Point(topleft.x + width, topleft.y);
                bottomright = Point(topleft.x + width, topleft.y-height);
                bottomleft = Point(topleft.x, topleft.y-height);
            break;


            default:
            throw std::runtime_error("Invalid point address for Rect please consider choosing one of the following: \n (a , b , c, d) \na: topleft\nb: topright\nc: bottomright\nd: bottomleft\n");
        }
    } // constructor end

    void draw(bool Fill=false, int width = 1)
    {
        glLineWidth(width);
        if(Fill)
            glBegin(GL_POLYGON);
        else
            glBegin(GL_LINE_LOOP);
        glVertex2d(topleft.x, topleft.y);
        glVertex2d(topright.x, topright.y);
        glVertex2d(bottomright.x, bottomright.y);
        glVertex2d(bottomleft.x, bottomleft.y);
        glEnd();
    }

    bool collide_point(Point p) // checks if a point collides with our rect
    {
        if (p.x >= topleft.x && p.x <= topright.x && p.y >= bottomleft.y && p.y <= topleft.y)
            return true;
        return false;
    }

    bool collide_rect(Rect rect2) // checks if any point of a given rect collides with our rect (my implementation of AABB collision algorithm)
    {
        //if(collide_point(r.bottomleft) || collide_point(r.bottomright) || collide_point(r.topleft) || collide_point(r.topright))
            //return true;  // this didnt work
        if(topleft.x <= rect2.topright.x && topright.x >= rect2.topleft.x && bottomleft.y<=rect2.topleft.y && topleft.y >= rect2.bottomleft.y)
            return true;
        return false;
    }

    // operators
    bool operator==(const Rect &other)
    {
        return (topleft==other.topleft && width == other.width && height == other.height);
    }

}; // Rect end


struct Clock
{
    std::chrono::high_resolution_clock::time_point pTime = std::chrono::high_resolution_clock::now(),cTime; // previous time and current time declartions
    void tick(int fps_limit=0, bool print_fps = false)
    {
        // calculating delta time
        cTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = cTime - pTime;
        dt = duration.count();
        pTime = cTime;
        if(print_fps)
            std::cout<<"fps:" << 1/dt <<"\n";  // prints fps
        if(fps_limit != 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(1000/fps_limit)); // slow down the code a little bit | this is limting the frame rate
    }
}; // Clock end


struct Mouse
{
    // attributes
    Point pos;
    bool up_click;
    bool down_click;
    bool is_down;
    bool is_up;

    // for the mouse wheel scrolling
    int wheel_direction = 0;

    // for the button press
    Rect press_rect;


    // methods
    void take_input(int btn, int state)
    {
        if(btn == GLUT_LEFT_BUTTON)
        {
            if(state == GLUT_DOWN)
            {
                //std::cout<<"LEFT DOWN\n";
                is_down = true;
                is_up = false;

                down_click = true;
            }

            if(state == GLUT_UP)
            {
                //std::cout<<"LEFT UP\n";
                is_down = false;
                is_up = true;

                up_click = true;
            }

        }

        // scroll wheel  | 4 = up and 3 = down
        if(btn == 3)
        {
            //std::cout<<"Scrolling down\n";
            wheel_direction = -1;
        }

        if (btn == 4)
        {
            //std::cout<<"scrolling up\n";
            wheel_direction = 1;
        }


    } // take_input end

    bool is_pushing(Rect button_rect) // if the mouse is hovering on the button rect and the left mouse button is being holded  returns true else false
    {
        if (button_rect.collide_point(pos) && is_down)
            return true;
        return false;
    }

    bool button_press(Rect button_rect) // no hovering
    {

        if (button_rect.collide_point(pos) && down_click)  // if we click down while hovering on the button rect
        {
            press_rect = button_rect; // we save button rect in our press rect

        }

        if(button_rect == press_rect && !button_rect.collide_point(pos)) // if we are not hovering on the rect we delete the saved version the press rect
            press_rect = Rect();

        if (press_rect == button_rect && up_click)
        {
            press_rect = Rect(); // erase the saved rect and return true
            return true;

        }

        return false;
    }

    void reset() // call it after updating everything in you program
    {
        up_click = false;
        down_click = false;
        wheel_direction = 0;
    }
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ the toolbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class Tools  // a tool box for utility and graphics methods
{
public:

    Tools()
    {
        // setup
        srand(time(0));  // change the random seed
        glEnable(GL_POINT_SMOOTH);  // enable smooth points
    }

    // ~~~~~~~~~~~~~~~~~~~~~~ random methods ~~~~~~~~~~~~~~~~~~~~~~~~~
    int randbetween(int start, int end)  // returns a random number between start and end
    {
        return start  + rand()%(start - end + 1);
    }

    int rand_sign() // returns a random positive or negative sign
    {
        int sign = (rand()%2 == 0)? 1 : -1;
        return sign;
    }



    // ~~~~~~~~~~~~~~~~~~~~~~ measuring methods ~~~~~~~~~~~~~~~~~~~~~~~~~
    double angle_between(int x1, int y1, int x2, int y2)  // measures the angle between 2 points and returns it in degrees
    {
        int dx = x2 - x1;
        int dy = y2 - y1;

        double angle = atan2(dy, dx);
        return angle * 180/pi;

    }

    double distance(double x1, double y1, double x2, double y2)
    {
        return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    }

    double rad(double degrees)  // takes an angle in degrees and returns it in radians
    {
        return degrees * pi/180;
    }

    double deg(double radians) // takes an angle in radians and returns it in degrees
    {
        return radians * 180/pi;
    }


// ~~~~~~~~~~~~~~~~~~~~~~ modifying methods ~~~~~~~~~~~~~~~~~~~~~~~~~

    void resetWindow()
    {
        glViewport(0, 0, SW, SH);
        glLoadIdentity();
        gluOrtho2D(0, SW, 0, SH);
    }

    void setWindow(double left, double right, double bottom, double top)
    {
        // glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluOrtho2D(left, right, bottom, top);
    }

    void Rotate(double angle, double x, double y)
    {
        glTranslated(x , y , 0.0);
        glRotated(angle, 0, 0, 1);
        glTranslated(-x , -y, 0.0);
    }


    // ~~~~~~~~~~~~~~~~~~~~~~ drawing methods ~~~~~~~~~~~~~~~~~~~~~~~~~
    void draw_rect(double x, double y, int w, int h, bool outline=false) // draws a rect using topleft x , y and width, height
    {
        if (outline)
            glBegin(GL_LINE_LOOP);
        else
            glBegin(GL_POLYGON);

        glVertex2i(x, y); // 1
        glVertex2i(x, y-h); // 2
        glVertex2i(x+w, y-h); // 3
        glVertex2i(x+w, y); // 4

        glEnd();
    }

    void draw_round_rect(Point center, double width, double height , double radius = 0, bool outline = false)
    {
        auto draw_arc = [&](Point center, double radius, double start_angle, double end_angle) // lambda
        {
            for(double angle = start_angle ; angle > end_angle ; angle--)  // start angle must be > end angle cus we are rotating in reverse
            {
                double x = center.x + radius * cos(rad(angle));
                double y = center.y + radius * sin(rad(angle));
                glVertex2d(x, y);
            }
        };
        if(outline)
            glBegin(GL_LINE_LOOP);
        else
            glBegin(GL_POLYGON);
        glVertex2d(center.x - width/2 + radius, center.y + height/2); // topleft
        glVertex2d(center.x + width/2 - radius, center.y + height/2); // topright
        draw_arc(Point(center.x + width/2 - radius, center.y + height/2 - radius), radius, 90, 0); // topright arc
        glVertex2d(center.x + width/2, center.y - height/2 + radius); // bottomright
        draw_arc(Point(center.x + width/2 - radius, center.y - height/2 + radius), radius, 360, 270); // bottomright arc
        glVertex2d(center.x - width/2 + radius, center.y - height/2); // bottomleft
        draw_arc(Point(center.x - width/2 + radius, center.y - height/2 + radius), radius, 270, 180); // bottomleft arc
        draw_arc(Point(center.x - width/2 + radius, center.y + height/2 - radius), radius, 180, 90); // bottomleft arc
        glEnd();

    }

    // ~~~~~~~~~~~~~~~~~~~~~~ text methods ~~~~~~~~~~~~~~~~~~~~~~~~~
    void renderText(const std::string &text, int x, int y)
    {
        glRasterPos2d(x, y+3);
        for(char c : text)
        {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
        }

    }

    void renderText(const std::string &text, Point pos)  // overloading same function with different parameteres
    {
        renderText(text, pos.x, pos.y);
    }

    void strokeText(const std::string &text, int thickness)
    {
        glPointSize(thickness);
        glLineWidth(thickness);
        for(char c : text)
            glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    }


}; // tools end
Tools tools;





// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ complex classes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class Text
{
public:
    // inputed attributes
    std::string text;
    Point pos;
    Color color;
    double scale;
    int thickness;

    // other attributes
    int chars_num = text.length();  // number of characters in the text
    double translation = 39; // this it used to translate the stroke text because it gets stuck under the view window

    double width = calcWidth();
    double height = 110 + translation;  // i just picked 110 after trial and error

    // rectangle for calculating positions correctly
    Rect rect;
    bool show_text_box = false;


    // constructors
    Text(const std::string &text, Point pos, Color color=Color(0, 0, 0), double scale =0.25, int thickness=1) : text(text), pos(pos), color(color), scale(scale), thickness(thickness)
    {
        rect = Rect(pos, width*scale, height*scale, 'e');
    }


    // metohds
    void draw()
    {
        // setting a viewport for the text so we can scale it
        glViewport(rect.bottomleft.x, rect.bottomleft.y - translation/6, width*scale, height*scale);
        tools.setWindow(0, width, 0, height);
        glTranslated(0, translation, 0);  // offesting everything abit from the bottom

        // drawing the text
        color.set();
        tools.strokeText(text, thickness);  // draws the text
        tools.resetWindow();

        // drawing a box around the text if show_text_box is true
        if(show_text_box)
        {
            Color(255, 0, 0).set();
            rect.draw();
        }

    } // draw end

    void update()
    {
        // empty for now
    }

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ private methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
private:
    int calcWidth()  // calculates the overall width of the text
    {
        int w = 0;
        for(char c : text)
            w += glutStrokeWidth(GLUT_STROKE_ROMAN, c);
        return w;
    }

};

class Button
{
public:
    // attributes
    Point center;
    double width;
    double height;
    std::string name;

    // rect
    Rect rect = Rect(center, width, height, 'e');

    // colors
    Color btn_color = Color(182,63,67);
    Color outline_color = Color(215,107,114);

    Color text_color = outline_color;
    Color text_hover_color = Color(255,194,65); // yellow      // Color(255,147,154);
    Color curr_text_color = text_color; // current text color

    // text
    double text_scale = 0.4;
    int text_width = 3;
    double text_color_speed = 135;

    // other settings
    double corner_radius = 23;
    int outline_width = 5;

    // function pointer
    void(*func)() = [](){std::cout<<"no func\n";};


    // constructors
    Button(Point center, double width, double height, const std::string &name) : center(center),  width(width), height(height), name(name){}


    // methods

    void draw()
    {
        // button body
        btn_color.set();
        tools.draw_round_rect(center, width, height, corner_radius);

        // button outline
        curr_text_color.set();
        glLineWidth(outline_width);
        tools.draw_round_rect(center, width, height, corner_radius, true);

        // draw text
        Text(name, center, curr_text_color, text_scale, text_width).draw();

    }

    void update(Mouse &mouse)
    {
        // update text color
        if(rect.collide_point(mouse.pos))  // if the mouse if hovering on the buttons rect
            curr_text_color.morphTo(text_hover_color, text_color_speed);
        else
            curr_text_color.morphTo(text_color, text_color_speed);

        // check clicks

        if (mouse.button_press(rect))
        {
            func();
        }

    }

};

int id = 1; // delete lates
class ConnectingCells
{
public:
    static const int cellsNum = 14;
    Rect area;

    class Cell
    {
    public:
            // debug
            int ID = id;
            // settings
            double speed = 70, DirSpeed = 0.3, ConnDist = 75; // movement speed, change direction speed and the distance in which cells connect connecting
            // sizes
            double cell_size = 6;
            double line_width = 1;
            // colors
            Color cell_color = Color(204, 73, 81);
            Color line_color = Color(255, 177, 196);
            Color tri_color = Color(250, 127, 136);
            double cell_alpha = 0.65;

            // attributes
            Point pos, currDir, nxtDir; // position, current direction and next direction
            // pointers
            Cell* female=nullptr; //female is the cell that this cell is connected to
            Cell* male=nullptr; // male is the cell that is connected to this cell
            // constructors
            Cell(){}
            Cell(Point position) : pos(position){changeDir(); id++;}

            //methods
            void changeDir() // changes nxtDir with a random value between -1 and 1 but not 0
            {
                double dir_x = 0.01 * tools.randbetween(1, 100) * tools.rand_sign();
                double dir_y = 0.01 * tools.randbetween(1, 100) * tools.rand_sign();
                nxtDir = Point(dir_x, dir_y);
            }

            void morphDir()
            {
                Point diff = Point(nxtDir.x - currDir.x, nxtDir.y - currDir.y); // difference represents the distence needed to reach new goal

                double max_diff = (abs(diff.x) >= abs(diff.y))? abs(diff.x): abs(diff.y);
                if (max_diff==0)
                {
                    changeDir();
                    return;
                }
                double number_of_steps = max_diff/DirSpeed;  // direction speed represents the size of each step

                Point change_ratio = Point(diff.x / max_diff, diff.y/max_diff);
                currDir.x += change_ratio.x * DirSpeed * dt;
                currDir.y += change_ratio.y * DirSpeed * dt;

                if(number_of_steps<=1)
                {
                    //std::cout<<"direction x:"<<currDir.x<<"  direction y"<<currDir.y<<"\n";  // debug
                    //std::cout<<"new direction x:"<<nxtDir.x<<" new direction y"<<nxtDir.y<<"\n";  // debug
                    changeDir();
                }
            }

            void draw()
            {
                // drawing cells
                glPointSize(cell_size);
                cell_color.set(cell_alpha * 255);
                pos.draw();

                // drawing connecting lines
                if(female != nullptr) // if we have female
                {
                    glLineWidth(line_width);
                    line_color.set((1 - pos.distance(female->pos)/ConnDist) * 255);
                    pos.draw_line(female->pos, false);
                }
                // debug
//                if(male != nullptr) // if we have male
//                {
//                    Color(50,129,234).set((1 - pos.distance(male->pos)/ConnDist) * 255);
//                    male->pos.draw_line(pos, true);
//                }

                // drawing triangles
                if(female != nullptr && male != nullptr && female->pos.distance(male->pos) < ConnDist)
                {
                    double alpha = ( 1 - (pos.distance(female->pos)/ConnDist + female->pos.distance(male->pos)/ConnDist + male->pos.distance(pos)/ConnDist)/3 );
                    tri_color.set(alpha * 255);
                    glBegin(GL_TRIANGLES);
                    glVertex2d(pos.x, pos.y);
                    glVertex2d(female->pos.x, female->pos.y);
                    glVertex2d(male->pos.x, male->pos.y);
                    glEnd();
                }
                // debug
//                Color(20, 170, 70).set(80);
//                tools.renderText(std::to_string(ID), pos);

            }

            void update(Cell arr[], int arrSize, Rect area)
            {
                // connecting
                for(int i = 0; i < arrSize; i++)// marrying conditions
                {
                    Cell &other = arr[i];
                    if (pos.distance(other.pos) <= ConnDist && this != &other)   // if the distance is close enough and the cell isnt me
                    { // +
                        if(other.male == nullptr && female == nullptr && other.female != this) // if the cells male is empty and its female is not me
                        {
                            if (other.female != nullptr && other.female != male) // | and its female != my male ( if female == my male do nothing)
                            {
                                female = &other;
                                other.male = this;
                            }
                            else if (other.female == nullptr)  // if female is null then connect without thinking
                            {
                                female = &other;
                                other.male = this;
                            }
                        }
                    }
                } // for loop end

                // disconnecting
                if (female != nullptr && pos.distance(female->pos) > ConnDist) {female->male = nullptr; female = nullptr;}
                if (male != nullptr && pos.distance(male->pos) > ConnDist) {male->female = nullptr; male = nullptr;}

                // move
                pos.x += speed * currDir.x * dt;
                pos.y += speed * currDir.y * dt;

                // slowly change direction;
                morphDir();

                // lock inside area
                if(pos.x > area.topright.x)
                {
                    pos.x = area.topright.x;
                    currDir = nxtDir;
                    nxtDir = Point(-1, nxtDir.y);
                }

                if(pos.x < area.topleft.x)
                {
                    pos.x = area.topleft.x;
                    currDir = nxtDir;
                    nxtDir = Point(1, nxtDir.y);

                }
                if (pos.y > area.topleft.y)
                {
                    pos.y = area.topleft.y;
                    currDir = nxtDir;
                    nxtDir = Point(nxtDir.x, -1);
                }
                if (pos.y < area.bottomleft.y)
                {
                    pos.y = area.bottomleft.y;
                    currDir = nxtDir;
                    nxtDir = Point(nxtDir.x, 1);
                }
            } // update end
    };
    // other attributes
    Cell arr[cellsNum];
    ConnectingCells(){}
    ConnectingCells(Rect area) : area(area)
    {
        for(Cell &cell : arr) // initilize the cells array
        {
            double x = tools.randbetween(area.topleft.x, area.topright.x);
            double y = tools.randbetween(area.bottomleft.y, area.topleft.y);
            cell = Cell(Point(x, y));
        }
    }

    void draw()
    {

        for(Cell &cell : arr)
        {
            cell.draw();
        }

        // debug
//        glColor3f(1, 1, 1);
//        area.draw();
//        Point txtpos = Point(area. topleft.x - 230, area.topleft.y);
//        for(int i = 0; i < cellsNum; i++)
//        {
//            txtpos.y -= 15;
//            std::string m = (arr[i].male != nullptr)? std::to_string(arr[i].male->ID) : "N";
//            std::string f = (arr[i].female != nullptr)? std::to_string(arr[i].female->ID) : "N";
//            tools.renderText(std::to_string(arr[i].ID) + ": M -> " + m + ": F -> " + f, txtpos);
//        }

    }

    void update()
    {
        for(Cell &cell : arr)
        {
            cell.update(arr, cellsNum, area);
        }
    }
};

// okay
