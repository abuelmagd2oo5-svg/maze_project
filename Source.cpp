#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <queue>
#include <algorithm>
#include <random>
using namespace std;

// ------------------------------------------------------------
//helper classes and structs
// ------------------------------------------------------------
int framesElapsed = 0;
int frameRate = 10;
int score = 0;

string s = "Hello maybe?";
const int cellSize = 32;
int WIDTH = 1280+cellSize;
int HEIGHT = 1024+cellSize;

const int xCells = WIDTH / cellSize;
const int yCells = HEIGHT / cellSize;
#define WALL 1

struct Point {
    int x, y;
    Point(int xx, int yy)
    {
        x = xx, y = yy;
    }
};
struct Player {
    Point pos = Point(3*cellSize+cellSize/2,3*cellSize+cellSize/2);
}player;
struct Game {
    Point scorePos = (Point(32,HEIGHT-16));
    Point timePos = (Point(WIDTH-128, HEIGHT - 16));
    int grid[1024][1024];

}game;




// ------------------------------------------------------------
//helper functions
// ------------------------------------------------------------
Point cellToPoint(Point cords)
{
    return { cords.x * cellSize,cords.y * cellSize };
}
Point pointToCell(Point cords)
{
    return { cords.x / cellSize,cords.y / cellSize };
}

bool canPressW = 1;
bool canPressS = 1;
bool canPressA = 1;
bool canPressD = 1;

bool pressedW = 0;
bool pressedS = 0;
bool pressedA = 0;
bool pressedD = 0;

void keyPressed(unsigned char key, int x, int y)
{
    if (tolower(key) == 'w' && canPressW)
    {
        canPressW = 0;
        pressedW = 1;
    }
    if (tolower(key) == 's' && canPressS)
    {
        canPressS = 0;
        pressedS = 1;
    }
    if (tolower(key) == 'a' && canPressA)
    {
        canPressA = 0;
        pressedA = 1;
    }
    if (tolower(key) == 'd' && canPressD)
    {
        canPressD = 0;
        pressedD = 1;
    }
    cout << "Key pressed: " << key << endl;

    if (key == 27)  // ESC
        exit(0);
}
void keyReleased(unsigned char key, int x, int y)
{
    cout << "Key released: " << key << endl;
    if (tolower(key) == 'w')
    {
        canPressW = 1;
        pressedW = 0;
    }
    if (tolower(key) == 's')
    {
        canPressS = 1;
        pressedS = 0;
    }
    if (tolower(key) == 'a')
    {
        canPressA = 1;
        pressedA = 0;
    }
    if (tolower(key) == 'd')
    {
        canPressD = 1;
        pressedD = 0;
    }
    if (key == 27)  // ESC
        exit(0);
}
void update();
void display();
int getTimeFrames()
{
    return framesElapsed;
}
int getTimeSeconds()
{
    return framesElapsed / frameRate;
}
string getTimeString()
{
    int s = getTimeSeconds();

    int rh = getTimeSeconds() / 3600;
    int rm = getTimeSeconds()%3600 / 60;
    int rs = getTimeSeconds() % 60;
    
    string rhs = to_string(rh);
    if (rh < 10)rhs = "0" + rhs;

    string rms = to_string(rm);
    if (rm < 10)rms = "0" + rms;

    string rss = to_string(rs);
    if (rs < 10)rss = "0" + rss;

    return (rhs + ":" + rms + ":" + rss);
}
void init()
{
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(2.0f);
}
void gameLoop(int value)
{
    update();                // update game logic
    glutPostRedisplay();     // redraw

    glutTimerFunc(1000/frameRate, gameLoop, 0); // ~60 FPS
}

void reshape(int w, int h)
{
    WIDTH = w;
    HEIGHT = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 1 OpenGL unit = 1 pixel
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void drawLine(Point a,Point b)
{
    glBegin(GL_POINTS);

    int dx = b.x - a.x;
    int dy = b.y - a.y;
    int steps = max(abs(dx), abs(dy));

    float x = a.x;
    float y = a.y;

    float xinc = dx / (float)steps;
    float yinc = dy / (float)steps;

    for (int i = 0; i <= steps; i++)
    {
        glVertex2i(round(x), round(y));
        x += xinc;
        y += yinc;
    }

    glEnd();
}
void drawText(Point start, string s)
{
    glRasterPos2f(start.x, start.y);
    for(auto i:s)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, i);
}
void drawSolidRectangle(Point bl, Point br, Point tr, Point tl)
{

    glBegin(GL_TRIANGLE_STRIP);
    glVertex2i(bl.x, bl.y);                   // bottom-left
    glVertex2i(br.x, br.y);            // bottom-right

    glVertex2i(tl.x, tl.y);            // top-left
    glVertex2i(tr.x, tr.y);     // top-right
    glEnd();
}
void drawRectangle(Point tl, Point tr, Point br, Point bl)
{
    drawLine(tl, tr);
    drawLine(tr, br);
    drawLine(br, bl);
    drawLine(bl, tl);
}
void drawPlayer(Point p)
{
    int ps = cellSize / 2 - 4;
    int dx[] = { -ps,ps,ps,-ps};
    int dy[] = { -ps,-ps,ps,ps };
    vector<Point> corners;
    for (int i = 0; i < 4; i++)
    {
        corners.push_back(Point(p.x + dx[i], p.y + dy[i]));
    }
    glColor3f(0.0f, 1.0f, 0.0f);
    drawSolidRectangle(corners[0], corners[1], corners[2], corners[3]);
}
void drawGrid()
{
    glColor3f(0.0f, 0.0f, 0.0f);
    glPointSize(0.1f);
    for (int i = 0; i + cellSize - 1 < WIDTH; i+=cellSize)
    {
        drawLine(Point(i, 0), Point(i, HEIGHT));
    }
    for (int i = 0; i + cellSize - 1 < HEIGHT; i += cellSize)
    {
        drawLine(Point(0, i), Point(WIDTH, i));
    }
    glPointSize(2.0f);
}



bool canMove(Point a, Point b)
{
    //if there is something between point a and b, then I cant move there
    //if point b is out of bounds, then I cant move there
    if (b.x >= WIDTH || b.y >= HEIGHT || b.x < 0 || b.y < 0)return 0;
    if (game.grid[b.x / cellSize][b.y / cellSize] == WALL)return 0;
    //handle walls here
    return 1;
}
vector<Point>trail;
void handleMovement()
{
    Point p = player.pos;
    bool moved = 0;
    if (pressedW)
    {
        Point nxt = player.pos;
        nxt.y += cellSize;
        if (canMove(player.pos, nxt))
            player.pos = nxt;
        pressedW = 0;
    }
    if (pressedA)
    {
        Point nxt = player.pos;
        nxt.x -= cellSize;
        if (canMove(player.pos, nxt))
            player.pos = nxt;
        pressedA = 0;
    }
    if (pressedS)
    {
        Point nxt = player.pos;
        nxt.y -= cellSize;
        if (canMove(player.pos, nxt))
            player.pos = nxt;
        pressedS = 0;
    }
    if (pressedD)
    {
        Point nxt = player.pos;
        nxt.x += cellSize;
        if (canMove(player.pos, nxt))
            player.pos = nxt;
        pressedD = 0;
    }
    if (player.pos.x!= p.x|| player.pos.y!=p.y)
    {
        trail.push_back(p);
    }
}
mt19937 rng(3);
void generateMaze()
{
    stack<pair<int, int>>st;
    st.push({ 0,0 });
    vector<vector<bool>>vis(WIDTH / cellSize, vector<bool>(HEIGHT / cellSize, 0));
    vis[0][0] = 1;
    int dx[] = { 0,0,1,-1 };
    int dy[] = { 1,-1,0,0 };
    for (int i = 0; i < xCells; i++)for (int j = 0; j < yCells; j++)game.grid[i][j] = WALL;
    for (int i = 0; i < xCells/2; i++) {
        for (int j = 0; j < yCells/2; j++) {
            game.grid[2 * i + 1][2 * j + 1] = 0;
        }
    }
    while (st.size())
    {

        int cx = st.top().first, cy = st.top().second;
        st.pop();
        vector<int>ord;
        for (int i = 0; i < 4; i++)ord.push_back(i);
        shuffle(ord.begin(), ord.end(), rng);
        for (auto i : ord)
        {
            int ncx = cx + dx[i];
            int ncy = cy + dy[i];

            if (ncx < 0 || ncx >= xCells/2 || ncy < 0 || ncy >= yCells/2 || vis[ncx][ncy])continue;
            vis[ncx][ncy] = 1;
            game.grid[ncx + cx + 1][ncy + cy + 1] = 0;
            st.push({ ncx,ncy });

            
        }

    }


}


void generateMaze2()
{
    vector<vector<bool>> vis(xCells / 2, vector<bool>(yCells / 2, 0));

    int dx[] = { 0, 0, 1, -1 };
    int dy[] = { 1, -1, 0, 0 };

    // initialize all walls
    for (int i = 0; i < xCells; i++)
        for (int j = 0; j < yCells; j++)
            game.grid[i][j] = WALL;
    
    // open all cell centers
    for (int i = 0; i < xCells / 2; i++)
        for (int j = 0; j < yCells / 2; j++)
            game.grid[2 * i + 1][2 * j + 1] = 0;
    // frontier walls
    vector<tuple<int, int, int, int>> walls;
    // (wall_x, wall_y, cell_x, cell_y)

    auto in_bounds = [&](int x, int y) {
        return x >= 0 && y >= 0 && x < xCells / 2 && y < yCells / 2;
        };

    // start from (0,0)
    vis[0][0] = 1;

    // add its neighbors
    for (int d = 0; d < 4; d++) {
        int nx = 0 + dx[d];
        int ny = 0 + dy[d];
        if (in_bounds(nx, ny)) {
            walls.emplace_back(
                1 + dx[d],      // wall x
                1 + dy[d],      // wall y
                nx, ny          // target cell
            );
        }
    }

    while (!walls.empty())
    {
        uniform_int_distribution<int> dist(0, (int)walls.size() - 1);
        int idx = dist(rng);

        auto [wx, wy, cx, cy] = walls[idx];
        walls[idx] = walls.back();
        walls.pop_back();

        if (vis[cx][cy]) continue;

        // carve wall
        game.grid[wx][wy] = 0;
        game.grid[2 * cx + 1][2 * cy + 1] = 0;

        vis[cx][cy] = 1;

        // add new frontier walls
        for (int d = 0; d < 4; d++) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];

            if (!in_bounds(nx, ny) || vis[nx][ny]) continue;

            walls.emplace_back(
                2 * cx + 1 + dx[d],
                2 * cy + 1 + dy[d],
                nx, ny
            );
        }
    }
}
// ------------------------------------------------------------
// Main loop
// ------------------------------------------------------------
bool first = 0;
void update()
{

    //do stuff
    if (!first)generateMaze2(), first = 1;
    handleMovement();
    
    framesElapsed++;
}


// ------------------------------------------------------------
// Main draw
// ------------------------------------------------------------

void drawTrail()
{
    for (auto i : trail)
    {
        drawLine({ i.x - 1,i.y - 1 }, { i.x + 1,i.y + 1 });
    }
}
void drawWalls()
{
    

    for (int cx = 0; cx < WIDTH / cellSize; cx++)
    {
        for (int cy = 0; cy < HEIGHT / cellSize; cy++)
        {
            if (game.grid[cx][cy] == WALL)
            {

                //glColor3f((float)(rng() % 1000000000) / 1000000000.00, (float)(rng() % 1000000000) / 1000000000.00, (float)(rng() % 1000000000) / 1000000000.00);
                drawSolidRectangle(cellToPoint({ cx,cy }), cellToPoint({ cx + 1,cy }), cellToPoint({ cx+1,cy + 1 }), cellToPoint({ cx,cy + 1 }));
            }
        }
    }
}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    //drawGrid();


    glColor3f(0.2f, 0.2f, 1.0f);
    

    //game.grid[0][0] = WALL;
    //game.grid[10][10] = WALL;
    drawWalls();
    drawTrail();


    drawPlayer(player.pos);

    glColor3f(1.0f, 0.0f, 0.0f);    
    drawText(game.timePos, getTimeString());
    drawText(game.scorePos, "Score: " + to_string(score));



    glFlush();
}

int main(int argc, char** argv)
{
    cout << "HI" << endl;
    cout << HEIGHT / cellSize << " " << WIDTH / cellSize << endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 0);
    glutCreateWindow("Maze maybe");


    init();

    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyReleased);
    glutTimerFunc(0, gameLoop, 0);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMainLoop();



    return 0;
}