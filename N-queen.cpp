#include <iostream>
#include<ctime>
#include<algorithm>
#include <vector>
#include <windows.h>
#include <GL/glut.h>

// Debug Macro
#define what(x) cerr << #x << " => " << x << endl;

using namespace std;
bool drawBoardFlag = false;
bool gameStarted = false;
bool displayPage = false;

// Constants
const int WINDOW_WIDTH = 700;
const int WINDOW_HEIGHT = 700;
const int OFFSET_X = 100;
const int OFFSET_Y = 100;
const int SIDE_LENGTH = 70;
const int SLEEP_TIME = 500;

enum Mode{ NOSTEPS,
           STEPS,
};

// Datatype to represent Black and White Color
enum Color{ BLACK,
            WHITE,
          };

// Datatype to represent box states: ocuupied or empty
enum State{ EMPTY,
            OCCUPIED,
          };

// Datatype to represent safe and unsafe states
enum Safe{ UNSAFE,
           SAFE,

};

// Datatype to store information of each boxes
struct box{ int x1, y1, x2, y2;
            Color color;
};

struct textColor{ float red, green, blue, alpha;
};

// Size of board
int N = 7;
Mode mode = STEPS;
int total_solutions = 0;

// Display text
string text = "Solving for N = " + to_string(N);
string display_text = "Displaying solution: ";
// Vector to store info{end coordinates of box and color} of N * N boxes
vector<box> boxes;

// Board of size N * N
vector<vector<int>> grid(N, vector<int>(N, EMPTY));
// 2D array of box_infos to map with grid of N * N
vector<vector<box>> box_coordinates(N, vector<box>(N));

// Function Prototypes
void draw_box(Color c, int x1, int y1, int x2, int y2);
void put_queen(int x1, int y1);
void put_text(int x1, int y1, textColor color,  char c);
bool is_safe(vector<vector<int>>grid, int r, int c);
bool solve(vector<vector<int>> grid, int row);
void flip_color(Color &color);
void draw_board();
void display_texts(int x, int y, int width, textColor color, string s);

void init(){
  // Set background color to blue
  glClearColor (0.0, 0.0, 1.0, 1.0) ;
  glClear(GL_COLOR_BUFFER_BIT) ;
  glPointSize(4.0);
  glMatrixMode(GL_PROJECTION);
  // Bottom left corner is (0, 0) and upper right corner is (WINDOW_WIDTH, WINDOW_HEIGHT)
  gluOrtho2D(0, WINDOW_WIDTH,0,WINDOW_HEIGHT);
}

// Function to draw queen inside box
void put_queen(int x1, int y1){
  glColor3f(0,255,0);
  int mid = SIDE_LENGTH / 2;

  glBegin(GL_POINTS);
    glVertex2i(x1 + mid + 12, y1 + SIDE_LENGTH - 16);
    glVertex2i(x1 + mid - 12, y1 + SIDE_LENGTH - 16);
    glVertex2i(x1 + mid + 24, y1 + SIDE_LENGTH - 24);
    glVertex2i(x1 + mid - 24, y1 + SIDE_LENGTH - 24);
  glEnd();

  glBegin(GL_LINES);
   glVertex2i(x1 + mid - 24, y1 + SIDE_LENGTH - 24);
   glVertex2i(x1 + mid - 16, y1 + 16);

   glVertex2i(x1 + mid - 16, y1 + 16);
   glVertex2i(x1 + mid + 16, y1 + 16);

   glVertex2i(x1 + mid + 16, y1 + 16);
   glVertex2i(x1 + mid + 24, y1 + SIDE_LENGTH - 24);

   glVertex2i(x1 + mid + 24, y1 + SIDE_LENGTH - 24);
   glVertex2i(x1 + mid + 12, y1 + SIDE_LENGTH - 32);

   glVertex2i(x1 + mid + 12, y1 + SIDE_LENGTH - 32);
   glVertex2i(x1 + mid + 12, y1 + SIDE_LENGTH - 16);

   glVertex2i(x1 + mid + 12, y1 + SIDE_LENGTH - 16);
   glVertex2i(x1 + mid, y1 + SIDE_LENGTH - 32);

   glVertex2i(x1 + mid, y1 + SIDE_LENGTH - 32);
   glVertex2i(x1 + mid - 12, y1 + SIDE_LENGTH - 16);

   glVertex2i(x1 + mid - 12, y1 + SIDE_LENGTH - 16);
   glVertex2i(x1 + mid - 12, y1 + SIDE_LENGTH - 32);

   glVertex2i(x1 + mid - 12, y1 + SIDE_LENGTH - 32);
   glVertex2i(x1 + mid - 24, y1 + SIDE_LENGTH - 24);

  glEnd();
  glFlush();
}

// Function to draw character on screen
void put_text(int x1, int y1, textColor text_color, char c){
  glColor3f(text_color.red, text_color.green, text_color.blue);
  glRasterPos2i (x1 + 27, y1 + 27);
  glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24, c);
  glFlush();
}

void mark(int r, int c, Safe st){
  auto Point = box_coordinates[r][c];
  int x1 = Point.x1, y1 = Point.y1, x2 = Point.x2, y2 = Point.y2;
  if(st == UNSAFE){
    glColor3f(255, 0, 0);
    glRectf(x1, y1, x2, y2);
  }
  else{
    draw_box(Point.color, x1, y1, x2, y2);
    put_queen(x1, y1);
    put_text(x1, y1, {0, 255, 0, 0}, 'Q');
  }
  glFlush();
}

// Checks if it is safe to put queen at cell (r, c)
bool is_safe(vector<vector<int>>grid, int r, int c){
  // For every other rows of column c
  for(int i = r - 1; i >= 0; i--){
      if(grid[i][c] == OCCUPIED){
          mark(i, c, UNSAFE);
          mark(r, c, UNSAFE);
          Sleep(SLEEP_TIME);
          mark(i, c, SAFE);
          mark(r, c, SAFE);
          return false;
      }
  }

  // Check Diagonally
  int dx[] = {-1, -1};
  int dy[] = {1, -1};

  for(int i = 0; i < 2; i++){
      int x = r;
      int y = c;
      while(x + dx[i] >= 0 and x + dx[i] < grid.size()
            and y + dy[i] >= 0 and y + dy[i] < grid.size()){
          x += dx[i];
          y += dy[i];
          if(grid[x][y] == OCCUPIED){
            mark(x, y, UNSAFE);
            mark(r, c, UNSAFE);
            Sleep(SLEEP_TIME);
            mark(x, y, SAFE);
            mark(r, c, SAFE);
            return false;
          }
      }
  }
  return true;
}

// Draws black or white boxes
void draw_box(Color c, int x1, int y1, int x2, int y2){
     if(c == BLACK){
        glColor3f(0,0,0);
     }
     else{
       glColor3f(255,255,255);
     }
     boxes.push_back({x1, y1, x2, y2, c});
     glRectf(x1, y1, x2, y2);
     glFlush();
}

// Solves N Queen Problem
bool solve(vector<vector<int>> grid, int row){
  // If N queens are placed in every rows
  // we have the solution
  if(row == grid.size()){
    total_solutions++;
    display_texts(10, 20, 13, {0, 255, 0, 0}, display_text + to_string(total_solutions));
    cout << "Displaying solution " << total_solutions << endl;
    Sleep(15 * SLEEP_TIME);
    display_texts(10, 20, 13, {0, 0, 255, 0}, display_text + to_string(total_solutions));
    return false;
  }

  for(int col = 0; col < grid.size(); col++){
    auto point = box_coordinates[row][col];
    int x1 = point.x1, y1 = point.y1, x2 = point.x2, y2 = point.y2;
    Color color = point.color;

    put_text(x1, y1 - 5,{0, 255, 0, 0}, 'Q');
    put_queen(point.x1, point.y1);
    if(mode == STEPS){
      Sleep(SLEEP_TIME);
    }

    if(is_safe(grid, row, col)){
      grid[row][col] = OCCUPIED;
      Sleep(2 * SLEEP_TIME);
      if(solve(grid, row + 1)){
        return true;
      }
      if(mode == STEPS){
        Sleep(SLEEP_TIME);
      }
      grid[row][col] = EMPTY;
      draw_box(color, x1, y1, x2, y2);
    }
    else{
      if(mode == STEPS){
       Sleep(SLEEP_TIME);
      }
      draw_box(color, x1, y1, x2, y2);
    }
  }
  return false;
}

void flip_color(Color &color){
  if(color == BLACK){
      color = WHITE;
  }
  else{
      color = BLACK;
  }
}

void display_texts(int x, int y, int width, textColor text_color, string s){
    int currX=x;
  for(auto c: s){
    put_text(currX, y, text_color, c);
    currX += width;
  }
}

void draw_board(){
  boxes.clear();
  Color color = WHITE;

  int x = 10, y = 0;
  display_texts(x, y, 10, {0, 255, 0, 0},  text);
  x = 100, y = 100;
  for(int row = 0; row < N; row++){
    int x1 = x;
    int y1 = y;
    int x2 = x1 + SIDE_LENGTH;
    int y2 = y1 + SIDE_LENGTH;
    // Increment row
    y = y2;
    flip_color(color);
    if(N % 2){
      flip_color(color);
    }
    for(int col = 0; col < N; col++){
      // Draw box
      draw_box(color, x1, y1, x2, y2);
      // Flip color
      flip_color(color);
      x1 += SIDE_LENGTH;
      x2 += SIDE_LENGTH;
    }
  }
  // Sort by row coordinates
  // Then col coordinates
  sort(boxes.begin(), boxes.end(), [](box l, box r){
       if(l.y1 == r.y1){
        return l.x1 < r.x1;
       }
       return l.y1 > r.y1;
  });

  int k = 0;
  for(int i = 0; i < N; i++){
    for(int j = 0; j < N; j++){
      box_coordinates[i][j] = boxes[k++];
    }
  }
 solve(grid, 0);
 glColor3f(0.0,0,0);
 glRectf(100,100,200,200);
 glFlush();
}

void displayFront() {
    glClearColor(0, 0.3, 0.7, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

    if (!gameStarted) {
        glColor3f(1, 1, 0);
        std::string aboveTitle = "SRINIVAS INSTITUTE TECHNOLOGY";
        glRasterPos2i(250, 600);
        for (const char& c : aboveTitle) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        glColor3f(1, 0, 0);
        // Write additional text above the title
        std::string line0 = "CS&E";
        glRasterPos2i(325, 550);
        for (const char& c : line0) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }
        // Display front screen
        glColor3f(1, 1, 1);
        glRasterPos2i(260, 500);
        std::string title = "N-QUEENS ALGORITHM";
        for (const char& c : title) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
        }

        // Display five lines with center alignment
        int lineSpacing = 50; // Adjust this value to control line spacing
        glColor3f(0, 0, 0.3);
        std::string line1 = "Submitted by";
        int line1Width = line1.size() * 9; // Adjust this value to control line width
        int line1X = (WINDOW_WIDTH - line1Width) / 2 + 25;
        int line1Y = 475;
        glRasterPos2i(line1X, line1Y);
        for (const char& c : line1) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }

        std::string line2 = "APEKSHA U S 4SN20CS013";
        int line2Width = line2.size() * 9; // Adjust this value to control line width
        int line2X = (WINDOW_WIDTH - line2Width) / 2 + 20;
        int line2Y = line1Y - (lineSpacing);
        glRasterPos2i(line2X, line2Y);
        for (const char& c : line2) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }

        std::string line3 = "ASHWITH  4SN20CS014";
        int line3Width = line3.size() * 9; // Adjust this value to control line width
        int line3X = (WINDOW_WIDTH - line3Width) / 2 + 20;
        int line3Y = line2Y - (0.5 * lineSpacing);
        glRasterPos2i(line3X, line3Y);
        for (const char& c : line3) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }

        std::string line4 = "UNDER THE GUIDANCE OF";
        int line4Width = line4.size() * 9; // Adjust this value to control line width
        int line4X = (WINDOW_WIDTH - line4Width) / 2 + 40;
        int line4Y = line3Y - lineSpacing;
        glRasterPos2i(line4X, line4Y);
        for (const char& c : line4) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }

        std::string line5 = "Prof. MAMATHA S";
        int line5Width = line5.size() * 9; // Adjust this value to control line width
        int line5X = (WINDOW_WIDTH - line5Width) / 2 + 15;
        int line5Y = line4Y - lineSpacing;
        glRasterPos2i(line5X, line5Y);
        for (const char& c : line5) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        std::string line6 = "CG MINI PROJECT";
        int line6Width = line6.size() * 9; // Adjust this value to control line width
        int line6X = (WINDOW_WIDTH - line6Width) / 2 + 13;
        int line6Y = line5Y - lineSpacing;
        glRasterPos2i(line6X, line6Y);
        for (const char& c : line6) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }

        std::string line7 = "18CSL67";
        int line7Width = line6.size() * 9; // Adjust this value to control line width
        int line7X = (WINDOW_WIDTH - line7Width) / 2 + 48;
        int line7Y = line6Y - (0.5 * lineSpacing);
        glRasterPos2i(line7X, line7Y);
        for (const char& c : line7) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }

        std::string line8 = "PRESS ENTER TO DISPLAY";
        int line8Width = line6.size() * 9; // Adjust this value to control line width
        int line8X = (WINDOW_WIDTH - line7Width) / 2;
        int line8Y = line7Y - (lineSpacing);
        glRasterPos2i(line8X, line8Y);
        for (const char& c : line8) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }
    else if (displayPage) {
        // Display the page content
        // ...
        glColor3f(1, 1, 1);
        glRasterPos2i(200, 600);
        std::string line1 = "Rules of N-QUEEN";
        for (const char& c : line1) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        glRasterPos2i(250, 550);
        std::string line2 = "1) Start with an empty N*N chessboard.";
        for (const char& c : line2) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        glRasterPos2i(250, 500);
        std::string line3 = "2)Place a queen in the first row and move to the next row..";
        for (const char& c : line3) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        glRasterPos2i(250, 450);
        std::string line4 = "3)For each column in the current row, check if placing a queen is safe:";
        for (const char& c : line4) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        glRasterPos2i(260, 400);
        std::string line5 = "*No other queens exist in the same column";
        for (const char& c : line5) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        glRasterPos2i(260, 350);
        std::string line6 = "*No other queens exist in the same diagonal (both left and right diagonals).";
        for (const char& c : line6) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        glRasterPos2i(250, 300);
        std::string line7 = "4)If a safe placement is found, move to the next row and repeat step 3.";
        for (const char& c : line7) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        glRasterPos2i(250, 250);
        std::string line8 = "5)If no safe placement is found in the current row, backtrack to the .";
        for (const char& c : line8) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        glRasterPos2i(250, 200);
        std::string line9 = "previous row and try a different column.";
        for (const char& c : line9) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        glRasterPos2i(250, 150);
        std::string line10 = "Press S to display";
        for (const char& c : line10) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }

    glFlush();
}

// Function to draw the chessboard

void display() {
    glClearColor(0, 0.3, 0.7, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

    if (!gameStarted || displayPage) {
        displayFront();
    }
    else if (drawBoardFlag) {
        draw_board();
    }

    glFlush();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 13 && !gameStarted) {
        gameStarted = true;
        displayPage = true;
        drawBoardFlag = false;
        glutPostRedisplay();
    }
    else if (gameStarted && !displayPage) {
        displayPage = true;
        drawBoardFlag = false;
        glutPostRedisplay();
    }
    else if (gameStarted && displayPage) {
        displayPage = false;
        drawBoardFlag = true;
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
       //  cin >> N;
    if(argc >= 2){
        //to_string(argv[1]);
        if(true){
            cout << "Here";
            N = stoi(argv[2]);
        }

    }

    // Initialize GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("N Queens Problem");

    // Set the callback functions
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    init();
    glutPostRedisplay();
    // Enter the GLUT event loop
    glutMainLoop();

    return 0;
}