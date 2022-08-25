//*********************************************************************
//  Codigo exemplo para aula pratica de Computacao Grafica
//  Assunto: Rasterizacao e Transformacoes Geometricas
//
//  Autor: Prof. Laurindo de Sousa Britto Neto e João Victor Melo da Silva 
//*********************************************************************

// Bibliotecas utilizadas pelo OpenGL
#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Biblioteca com functions matematicas
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>
// Variaveis Globais

bool click[4] = {false, false, false, false};

int x_p[4], y_p[4];

int width = 512, height = 512; // Largura e altura da janela
enum functions {
  linha,
  retangulo,
  triangulo,
  poligono,
  circunferencia,
  floodfill,

};
enum functions function = linha;

// Estrututa de dados para o armazenamento dinamico dos pontos
// selecionados pelos algoritmos de rasterizacao
struct Point {
  int x;
  int y;

public:
  Point(int _x, int _y) : x(_x), y(_y) {}
  void setCoord(int _x, int _y) {
    this->x = _x;
    this->y = _y;
  }
};

typedef struct {
  std::list<Point> pontos;
  int *x;
  int *y;
  int type;
} Shape;

std::list<Shape> shapes;
std::list<Point> pointsFF;
std::list<Point> borderPoints;
std::vector<std::vector<int>> matAux(height, std::vector<int>(width, 0));

// Declaracoes forward das functions utilizadas
void init(void);
void reshape(int w, int h);
void display(void);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void disableClicks();
// function que implementa o Algoritmo Imediato para rasterizacao de retas
void retaImediata(int x[], int y[]);
std::list<Point> bresenham(int x1, int x2, int y1, int y2);
std::list<Point> reduce2TheFirstOct(int x1, int x2, int y1, int y2);
std::list<Point> bresenhamCircunference(int x1, int y1, int R);
void drawLine(int x[], int y[]);
void drawRectangle(int x[], int y[]);
void drawTriangle(int x[], int y[]);
void drawPolygon(int x[], int y[]);
void drawCircunference(int x[], int y[]);
void scale();
void rotate();
void reflection();
void translation();
void shear();
void drawTransformation(Shape shape);
void floodFill(int x, int y);
// // function que percorre a lista de pontos desenhando-os na tela
void drawPoints();
// function que desfaz a ultima forma feita
void undoDraw();

int main(int argc, char **argv) {
  glutInit(&argc, argv); // Passagens de parametro C para o glut
  glutInitDisplayMode(
      GLUT_DOUBLE |
      GLUT_RGB); // Selecao do Modo do Display e do Sistema de cor utilizado
  glutInitWindowSize(width, height); // Tamanho da janela do OpenGL
  glutInitWindowPosition(100, 100);  // Posicao inicial da janela do OpenGL
  glutCreateWindow("Rasterizacao");  // Da nome para uma janela OpenGL
  init();                            // Chama function init();
  glutReshapeFunc(reshape);          // function callback para redesenhar a tela
  glutKeyboardFunc(keyboard);        // function callback do teclado
  glutMouseFunc(mouse);              // function callback do mouse
  glutDisplayFunc(display);          // function callback de desenho
  glutMainLoop();                    // executa o loop do OpenGL
  return 0; // retorna 0 para o tipo inteiro da function main();
}

// function com alguns comandos para a inicializacao do OpenGL;
void init(void) {
  glClearColor(1.0, 1.0, 1.0, 1.0); // Limpa a tela com a cor branca;
}

void reshape(int w, int h) {
  // Reinicializa o sistema de coordenadas
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Definindo o Viewport para o tamanho da janela
  glViewport(0, 0, w, h);

  width = w;
  height = h;
  glOrtho(0, w, 0, h, -1, 1);
  matAux.clear();
  matAux.resize(height, std::vector<int>(width, 0));
  // muda para o modo GL_MODELVIEW (não pretendemos alterar a projecção
  // quando estivermos a desenhar na tela)
  glMatrixMode(GL_MODELVIEW);
  drawPoints();
}

// function usada na function callback para utilizacao das teclas normais do
// teclado
void keyboard(unsigned char key, int x, int y) {
  switch (key) { // key - variavel que possui valor ASCII da tecla precionada
  case 27:       // codigo ASCII da tecla ESC
    exit(0);     // comando pra finalizacao do programa
    break;
  case 'z':
    undoDraw();
    break;
  case 'l':
    function = linha;
    break;
  case 'r':
    function = retangulo;
    break;
  case 't':
    function = triangulo;
    break;
  case 'p':
    function = poligono;
    break;
  case 'c':
    function = circunferencia;
    break;
  case 'f':
    function = floodfill;
    break;
  case 'S':
    scale();
    break;
  case 'T':
    translation();
    break;
  case 'R':
    reflection();
    break;
  case 'G':
    rotate();
    break;
  case 'C':
    shear();
    break;
  }
}

// function usada na function callback para a utilizacao do mouse
void mouse(int button, int state, int x, int y) {
  switch (button) {
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_DOWN) {
      if (click[2]) {
        click[3] = true;
        x_p[3] = x;
        y_p[3] = height - y;

        printf("x4y4(%.0d,%.0d)\n", x_p[3], y_p[3]);
        glutPostRedisplay();

      } else if (click[1]) {
        click[2] = true;
        x_p[2] = x;
        y_p[2] = height - y;
        printf("x3y3(%.0d,%.0d)\n", x_p[2], y_p[2]);

        if (function == triangulo)
          glutPostRedisplay();
      } else if (click[0]) {
        click[1] = true;
        x_p[1] = x;
        y_p[1] = height - y;
        printf("x2y2(%.0d,%.0d)\n", x_p[1], y_p[1]);

        if (function == linha || function == retangulo ||
            function == circunferencia)
          glutPostRedisplay();
      } else {
        click[0] = true;
        x_p[0] = x;
        y_p[0] = height - y;
        printf("x1y1(%.0d,%.0d)\n", x_p[0], y_p[0]);
        if (function == floodfill)
          glutPostRedisplay();
      }
    }
    break;
  default:
    break;
  }
}

// function usada na function callback para desenhar na tela
void display(void) {
  glClear(GL_COLOR_BUFFER_BIT); // Limpa o buffer de cores e reinicia a matriz
  glLoadIdentity();

  glColor3f(0.0, 0.0, 0.0); // Seleciona a cor default como preto

  switch (function) {
  case linha:
    if (click[0] && click[1]) {
      drawLine(x_p, y_p);
      disableClicks();
    }
    break;
  case retangulo:
    if (click[0] && click[1]) {
      drawRectangle(x_p, y_p);
      disableClicks();
    }
    break;
  case triangulo:
    if (click[0] && click[1] && click[2]) {
      drawTriangle(x_p, y_p);
      disableClicks();
    }
    break;
  case poligono:
    if (click[0] && click[1] && click[2] && click[3]) {
      drawPolygon(x_p, y_p);
      disableClicks();
    }
    break;
  case circunferencia:
    if (click[0] && click[1]) {
      drawCircunference(x_p, y_p);
      disableClicks();
    }
    break;
  case floodfill:
    if (click[0]) {
      std::cout << "x: " << x_p[0] << " y:" << y_p[0] << std::endl;
      floodFill(x_p[0], y_p[0]);
      disableClicks();
    }
    break;
  default:
    break;
  }

  drawPoints();
  glutSwapBuffers(); // manda o OpenGl renderizar as primitivas
}
void storeIntoSafeZone(Point pnt, std::list<Point> &pnts) {
  if (pnt.x >= 0 && pnt.x < width && pnt.y >= 0 && pnt.y < height) {
    pnts.push_front(pnt);
  }
}
void drawLine(int x[], int y[]) {
  Shape forma;
  forma.x = x;
  forma.y = y;
  forma.type = 0;
  std::list<Point> pnts(reduce2TheFirstOct(x[0], x[1], y[0], y[1]));
  forma.pontos.assign(pnts.begin(), pnts.end());
  shapes.push_front(forma);
}
void drawRectangle(int x[], int y[]) {
  Shape forma;
  forma.x = x;
  forma.y = y;
  forma.type = 1;
  std::list<Point> pnts(reduce2TheFirstOct(x[0], x[1], y[0], y[0]));
  pnts.splice(pnts.end(), reduce2TheFirstOct(x[0], x[0], y[0], y[1]));
  pnts.splice(pnts.end(), reduce2TheFirstOct(x[1], x[1], y[0], y[1]));
  pnts.splice(pnts.end(), reduce2TheFirstOct(x[0], x[1], y[1], y[1]));

  forma.pontos.assign(pnts.begin(), pnts.end());
  shapes.push_front(forma);
}

void drawTriangle(int x[], int y[]) {
  Shape forma;
  forma.x = x;
  forma.y = y;
  forma.type = 2;
  std::list<Point> pnts(reduce2TheFirstOct(x[0], x[1], y[0], y[1]));
  pnts.splice(pnts.end(), reduce2TheFirstOct(x[1], x[2], y[1], y[2]));
  pnts.splice(pnts.end(), reduce2TheFirstOct(x[0], x[2], y[0], y[2]));

  forma.pontos.assign(pnts.begin(), pnts.end());
  shapes.push_front(forma);
}

void drawPolygon(int x[], int y[]) {
  Shape forma;
  forma.x = x;
  forma.y = y;
  forma.type = 3;
  std::list<Point> pnts(reduce2TheFirstOct(x[0], x[1], y[0], y[1]));
  pnts.splice(pnts.end(), reduce2TheFirstOct(x[1], x[2], y[1], y[2]));
  pnts.splice(pnts.end(), reduce2TheFirstOct(x[2], x[3], y[2], y[3]));
  pnts.splice(pnts.end(), reduce2TheFirstOct(x[0], x[3], y[0], y[3]));

  forma.pontos.assign(pnts.begin(), pnts.end());
  shapes.push_front(forma);
}
void drawCircunference(int x[], int y[]) {
  double aux = std::pow(x[1] - x[0], 2) + std::pow(y[1] - y[0], 2);
  int R = (int)std::sqrt(aux);
  Shape forma;
  forma.x = x;
  forma.y = y;
  forma.type = 4;
  std::list<Point> pnts(bresenhamCircunference(x[0], y[0], R));
  forma.pontos.assign(pnts.begin(), pnts.end());
  shapes.push_front(forma);
}
// function que desenha os pontos contidos em uma lista de pontos
void drawPoints() {

  glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
  for (auto forma : shapes) {
    for (auto pnt : forma.pontos) {
      glVertex2i(pnt.x, pnt.y);
      matAux[pnt.y][pnt.x] = 1;
    }
  }
  glColor3f(0., 1.0, 0.0);
  for (auto pnt : pointsFF) {
    glVertex2i(pnt.x, pnt.y);
    matAux[pnt.y][pnt.x] = 1;
  }

  glEnd(); // indica o fim do desenho
}
void undoDraw() {
  if (shapes.size() == 0)
    return;
  shapes.pop_front();
  glutPostRedisplay();
}

std::list<Point> bresenham(int x1, int x2, int y1, int y2, bool simetrico,
                           bool declive) {
  std::list<Point> pontos;
  double dx, dy, d, incE, incNE;
  int x = x1, y = y1, x_aux, y_aux;
  Point *pnt;
  dx = x2 - x1;
  dy = y2 - y1;
  d = 2 * dy - dx;
  incE = 2 * dy;
  incNE = 2 * (dy - dx);

  while (x < x2) {
    if (d <= 0) {
      d += incE;
    } else {
      d += incNE;
      y++;
    }
    x_aux = x;
    y_aux = y;

    if (declive) {
      std::swap(y_aux, x_aux);
    }
    if (simetrico) {
      y_aux = -y_aux;
    }
    Point pnt(x_aux, y_aux);
    storeIntoSafeZone(pnt, pontos);
    x++;
  }
  return pontos;
}

std::list<Point> reduce2TheFirstOct(int x1, int x2, int y1, int y2) {
  bool declive = false, simetrico = false;
  double dx, dy;
  dx = x2 - x1;
  dy = y2 - y1;

  if (dx * dy < 0) {
    simetrico = true;
    y1 = -y1;
    y2 = -y2;
    dy = y2 - y1;
  }
  if (std::abs(dx) < std::abs(dy)) {
    declive = true;
    std::swap(y1, x1);
    std::swap(y2, x2);
    std::swap(dx, dy);
  }
  if (x1 > x2) {

    std::swap(y1, y2);
    std::swap(x1, x2);
  }
  return bresenham(x1, x2, y1, y2, simetrico, declive);
}

std::list<Point> bresenhamCircunference(int x1, int y1, int R) {
  int d, incE, incSE, x, y, des_x, des_y;
  std::list<Point> pnts;
  d = 1 - R;
  incE = 3;
  incSE = -2 * R + 5;
  des_x = x1;
  des_y = y1;
  x = 0;
  y = R;
  while (y > x) {
    if (d < 0) {
      d += incE;
      incE += 2;
      incSE += 2;
    } else {
      d += incSE;
      incE += 2;
      incSE += 4;
      y--;
    }
    Point pnt(x + des_x, y + des_y);
    storeIntoSafeZone(pnt, pnts);

    pnt.setCoord(-x + des_x, y + des_y);
    storeIntoSafeZone(pnt, pnts);

    pnt.setCoord(x + des_x, -y + des_y);
    storeIntoSafeZone(pnt, pnts);

    pnt.setCoord(-x + des_x, -y + des_y);
    storeIntoSafeZone(pnt, pnts);

    pnt.setCoord(y + des_x, x + des_y);
    storeIntoSafeZone(pnt, pnts);

    pnt.setCoord(-y + des_x, x + des_y);
    storeIntoSafeZone(pnt, pnts);

    pnt.setCoord(y + des_x, -x + des_y);
    storeIntoSafeZone(pnt, pnts);

    pnt.setCoord(-y + des_x, -x + des_y);
    storeIntoSafeZone(pnt, pnts);

    x++;
  }
  return pnts;
}
void translation() {
  Shape sp = shapes.front();
  int length = (int)sizeof(sp.x) / sizeof(sp.x[0]);
  std::cout << length << std::endl;
  int Dx, Dy;
  fflush(stdin);
  std::cout << "Digite a distância deslocar em x:" << std::endl;
  std::cin >> Dx;
  fflush(stdin);
  std::cout << "Digite a distância a deslocar em y:" << std::endl;
  std::cin >> Dy;
  for (int i = 0; i < length; i++) {
    sp.x[i] += Dx;
    sp.y[i] += Dy;
  }
  drawTransformation(sp);
}
void scale() {
  Shape sp = shapes.front();
  int length = (int)sizeof(sp.x) / sizeof(sp.x[0]);
  std::cout << length << std::endl;
  float Sx, Sy;
  fflush(stdin);
  std::cout << "Digite o valor de Sx:" << std::endl;
  std::cin >> Sx;
  fflush(stdin);
  std::cout << "Digite o valor de Sy:" << std::endl;
  std::cin >> Sy;
  int des_x = sp.x[0];
  int des_y = sp.y[0];
  for (int i = 0; i < length; i++) {
    sp.x[i] = (sp.x[i] - des_x) * Sx + des_x;
    sp.y[i] = (sp.y[i] - des_y) * Sy + des_y;
  }
  drawTransformation(sp);
}

void reflection() {
  Shape sp = shapes.front();
  int length = (int)sizeof(sp.x) / sizeof(sp.x[0]);
  std::cout << length << std::endl;
  std::string eixo;
  fflush(stdin);
  std::cout << "Digite qual eixo deseja refletir:" << std::endl;
  std::cin >> eixo;
  for (int i = 0; i < length; i++) {
    if (std::string::npos != eixo.find("xy")) {
      sp.x[i] = width - sp.x[i];
      sp.y[i] = height - sp.y[i];
    } else if (std::string::npos != eixo.find("x")) {
      sp.y[i] = height - sp.y[i];
    } else if (std::string::npos != eixo.find("y")) {
      sp.x[i] = width - sp.x[i];
    }
  }
  drawTransformation(sp);
}
void rotate() {
  Shape sp = shapes.front();
  int length = (int)sizeof(sp.x) / sizeof(sp.x[0]);
  std::cout << length << std::endl;
  float angle;
  fflush(stdin);
  std::cout << "Digite qual angulo deseja rotacionar:" << std::endl;
  std::cin >> angle;
  for (int i = 0; i < length; i++) {
    sp.x[i] = (int)(sp.x[i] - sp.x[0]) * std::cos(angle) -
              (sp.y[i] - sp.y[0]) * std::sin(angle) + sp.x[0];
    sp.y[i] = (int)(sp.y[i] - sp.y[0]) * std::cos(angle) +
              (sp.x[i] - sp.x[0]) * std::sin(angle) + sp.y[0];
  }
  drawTransformation(sp);
}
void shear() {
  Shape sp = shapes.front();
  int length = (int)sizeof(sp.x) / sizeof(sp.x[0]);
  std::cout << length << std::endl;
  float Cx, Cy;
  fflush(stdin);
  std::cout << "Digite o valor de Cx:" << std::endl;
  std::cin >> Cx;
  fflush(stdin);
  std::cout << "Digite o valor de Cy:" << std::endl;
  std::cin >> Cy;
  for (int i = 0; i < length; i++) {
    sp.x[i] -= sp.x[0];
    sp.x[i] += (int)Cy * (sp.y[i] - sp.y[0]) + sp.x[0];
    sp.y[i] -= sp.y[0];
    sp.y[i] += (int)Cx * (sp.x[i] - sp.x[0]) + sp.y[0];
  }
  drawTransformation(sp);
}
void drawTransformation(Shape sp) {
  shapes.pop_front();
  switch (sp.type) {
  case 0:
    drawLine(sp.x, sp.y);
    break;
  case 1:
    drawRectangle(sp.x, sp.y);
    break;
  case 2:
    drawTriangle(sp.x, sp.y);
    break;
  case 3:
    drawPolygon(sp.x, sp.y);
    break;
  case 4:
    drawCircunference(sp.x, sp.y);
    break;
  }

  glutPostRedisplay();
}

void setPixelColor(int x, int y) {
  Point pnt{x, y};
  pointsFF.push_front(pnt);
  matAux[pnt.y][pnt.x] = 1;
}

void floodFill(int x, int y) {
  if (matAux[y][x] == 0) {
    setPixelColor(x, y);

    if (x > 0 && x < width && y > 0 && y < height) {
      floodFill(x + 1, y);
      floodFill(x, y + 1);
      floodFill(x - 1, y);
      floodFill(x, y - 1);
    }
  }
  return;
}
void disableClicks() {
  for (int i = 0; i < 4; i++)
    click[i] = false;
}
