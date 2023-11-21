#include "letter.h"
#include "ui_mainwindow.h"
#include <QDebug>
GLfloat VertexArray[104][3];
GLubyte IndexArray[13][4];
letter::letter(QWidget *parent)
    : QGLWidget(parent)
{
    //setGeometry(400,200,800,600);
    // setFormat(QGLFormat(QGL::DepthBuffer)); // использовать буфер глубины
                                               // установлено по умолчанию в контексте

    // начальные значения
    xRot=0; yRot=0; zRot=0; zTra=-1; nSca=1;

    // передает дальше указатель на объект pwgt
}

void letter::initializeGL()
{
    qglClearColor(Qt::white); // цвет для очистки буфера изображения - здесь просто фон окна
    glEnable(GL_DEPTH_TEST);  // устанавливает режим проверки глубины пикселей
    glShadeModel(GL_FLAT);    // отключает режим сглаживания цветов
    glEnableClientState(GL_VERTEX_ARRAY); // активизация массива вершин
}

void letter::resizeGL(int nWidth, int nHeight)
{
    glMatrixMode(GL_PROJECTION); // устанавливает текущей проекционную матрицу
    glLoadIdentity();            // присваивает проекционной матрице единичную матрицу

    GLfloat ratio=(GLfloat)nHeight/(GLfloat)nWidth; // отношение высоты окна виджета к его ширине

    // мировое окно
    if (nWidth>=nHeight)
       glOrtho(-1.0/ratio, 1.0/ratio, -1.0, 1.0, -5.0, 5.0); // параметры видимости ортогональной проекции
    else
       glOrtho(-1.0, 1.0, -1.0*ratio, 1.0*ratio, -5.0, 5.0); // параметры видимости ортогональной проекции
    // плоскости отсечения (левая, правая, верхняя, нижняя, передняя, задняя)

    // glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 10.0); // параметры видимости перспективной проекции
    // плоскости отсечения (левая, правая, верхняя, нижняя, ближняя, дальняя)

    // поле просмотра
    glViewport(0, 0, (GLint)nWidth, (GLint)nHeight);
}

void letter::paintGL()
{
    glClearColor(1,1,1,0); // цвет фона
    glClear(GL_COLOR_BUFFER_BIT); // окно виджета очищается текущим цветом очистки
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // очистка буфера изображения и глубины
    glMatrixMode(GL_MODELVIEW); // устанавливает положение и ориентацию матрице моделирования
    glLoadIdentity();           // загружает единичную матрицу моделирования
    // последовательные преобразования
    glScalef(nSca, nSca, nSca);        // масштабирование
    glTranslatef(0.0f, zTra, 0.0f);  // трансляция
    glRotatef(xRot, 1.0f, 0.0f, 0.0f); // поворот вокруг оси X
    glRotatef(yRot, 0.0f, 1.0f, 0.0f); // поворот вокруг оси Y
    glRotatef(zRot, 0.0f, 0.0f, 1.0f); // поворот вокруг оси Z
    drawAxis();
    double e=0,r=0,t=0;
    colour.getRgbF(&e,&r,&t,nullptr);
    glColor4f(e, r, t, 0.0f);
    getVertexArray(1);
    getIndexArray();
    if(check==1)
    draw_xy_projection();
    else if(check==2)
    draw_xz_projection();
    else if(check==3)
    draw_zy_projection();
    else if(check==4)
    draw_scaling();
    else if(check==5)
    draw_transfer();
    else if(check==6)
    draw_rotation_x();
    else if(check==7)
    draw_rotation_y();
    else if(check==8)
    draw_rotation_z();
    else if(check==9)
    drawFigure();
    QFont tmpfont;
            tmpfont.setFamily("Arial Black");
            tmpfont.setPointSize(10);
            tmpfont.setBold(false);
            glColor3f(0,0,0);
            renderText(10,0.0,0.0,"X",tmpfont);
            glColor3f(0,0,0);
            renderText(0.0,10,0.0,"Y",tmpfont);
            glColor3f(0,0,0);
            renderText(0.0,0.0,10,"Z",tmpfont);

}

/*virtual*/void letter::mousePressEvent(QMouseEvent* pe) // нажатие клавиши мыши
{
   // при нажатии пользователем кнопки мыши переменной ptrMousePosition будет
   // присвоена координата указателя мыши
   ptrMousePosition = pe->pos();

   // ptrMousePosition = (*pe).pos(); // можно и так написать
}

/*virtual*/void letter::mouseReleaseEvent(QMouseEvent* pe) // отжатие клавиши мыши
{
   // некоторые функции, которые должны выполняться при отжатии клавиши мыши
}

/*virtual*/void letter::mouseMoveEvent(QMouseEvent* pe) // изменение положения стрелки мыши
{
   xRot += 1/M_PI*(GLfloat)(pe->y()-ptrMousePosition.y());//height(); // вычисление углов поворота
   yRot += 1/M_PI*(GLfloat)(pe->x()-ptrMousePosition.x());//width();
   ptrMousePosition = pe->pos();
   updateGL(); // обновление изображения
}

/*virtual*/void letter::wheelEvent(QWheelEvent* pe) // вращение колёсика мыши
{
   if ((pe->delta())>0) scale_plus(); else if ((pe->delta())<0) scale_minus();

   updateGL(); // обновление изображения
}

/*virtual*/void letter::keyPressEvent(QKeyEvent* pe) // нажатие определенной клавиши
{
   switch (pe->key())
   {
      case Qt::Key_Plus:
         scale_plus();     // приблизить сцену
      break;

      case Qt::Key_Equal:
         scale_plus();     // приблизить сцену
      break;

      case Qt::Key_Minus:
         scale_minus();    // удалиться от сцены
      break;

      case Qt::Key_W:
         rotate_up();      // повернуть сцену вверх
      break;

      case Qt::Key_S:
         rotate_down();    // повернуть сцену вниз
      break;

      case Qt::Key_A:
        rotate_left();     // повернуть сцену влево
      break;

      case Qt::Key_D:
         rotate_right();   // повернуть сцену вправо
      break;

      case Qt::Key_Z:
         translate_down(); // транслировать сцену вниз
      break;

      case Qt::Key_X:
         translate_up();   // транслировать сцену вверх
      break;

      case Qt::Key_Space:  // клавиша пробела
         defaultScene();   // возвращение значений по умолчанию
      break;

      case Qt::Key_Escape: // клавиша "эскейп"
         this->close();    // завершает приложение
      break;
   }

   updateGL(); // обновление изображения
}


void letter::scale_plus() // приблизить сцену
{
   nSca = nSca*1.1;
}

void letter::scale_minus() // удалиться от сцены
{
   nSca = nSca/1.1;
}

void letter::rotate_up() // повернуть сцену вверх
{
   xRot += 1.0;
}

void letter::rotate_down() // повернуть сцену вниз
{
   xRot -= 1.0;
}

void letter::rotate_left() // повернуть сцену влево
{
   zRot += 1.0;
}

void letter::rotate_right() // повернуть сцену вправо
{
   zRot -= 1.0;
}

void letter::translate_down() // транслировать сцену вниз
{
   zTra -= 0.05;
}

void letter::translate_up() // транслировать сцену вверх
{
   zTra += 0.05;
}

void letter::defaultScene() // наблюдение сцены по умолчанию
{
   xRot=-90; yRot=0; zRot=0; zTra=0; nSca=1;
}
void letter::drawAxis()
{
    glLineWidth(3.0f); // устанавливаю ширину линии приближённо в пикселях
    // до вызова команды ширина равна 1 пикселю по умолчанию

    glColor4f(1.00f, 0.00f, 0.00f, 1.0f); // устанавливается цвет последующих примитивов
    // ось x красного цвета
    glBegin(GL_LINES); // построение линии
       glVertex3f( 10.0f,  0.0f,  0.0f); // первая точка
       glVertex3f(-10.0f,  0.0f,  0.0f); // вторая точка
    glEnd();

    QColor halfGreen(0, 128, 0, 255);
    qglColor(halfGreen);
    glBegin(GL_LINES);
       // ось y зеленого цвета
       glVertex3f( 0.0f,  10.0f,  0.0f);
       glVertex3f( 0.0f, -10.0f,  0.0f);

       glColor4f(0.00f, 0.00f, 1.00f, 1.0f);
       // ось z синего цвета
       glVertex3f( 0.0f,  0.0f,  10.0f);
       glVertex3f( 0.0f,  0.0f, -10.0f);

        glColor4f(0.00f, 0.00f, 0.00f, 0.0f);
       for (float i = -10; i < 10; i+=0.25){
           float k = 20.0;
           glBegin(GL_LINES);
           glVertex3f(i, -1.0 / k, 0.0f);
           glVertex3f(i, 1.0 / k, 0.0f);
           glVertex3f(-1.0 / k, i, 0.0f);
           glVertex3f(1.0 / k, i, 0.0f);
           glVertex3f( 0.0f, 1.0 / k, i);
           glVertex3f( 0.0f, -1.0 / k, i);
       }

    glEnd();
}

void letter::getVertexArray(float k){
//1
    VertexArray[0][0]=0;
    VertexArray[0][1]=0;
    VertexArray[0][2]=0;

    VertexArray[1][0]=0;
    VertexArray[1][1]=0;
    VertexArray[1][2]=-0.25*k;

    VertexArray[2][0]=0;
    VertexArray[2][1]=2*k;
    VertexArray[2][2]=-0.25*k;

    VertexArray[3][0]=0;
    VertexArray[3][1]=2*k;
    VertexArray[3][2]=0;
//2
    VertexArray[4][0]=0;
    VertexArray[4][1]=2*k;
    VertexArray[4][2]=0;

    VertexArray[5][0]=0;
    VertexArray[5][1]=2*k;
    VertexArray[5][2]=-0.25*k;

    VertexArray[6][0]=0.25*k;
    VertexArray[6][1]=2*k;
    VertexArray[6][2]=-0.25*k;

    VertexArray[7][0]=0.25*k;
    VertexArray[7][1]=2*k;
    VertexArray[7][2]=0;
//3
    VertexArray[8][0]=0;
    VertexArray[8][1]=0;
    VertexArray[8][2]=0;

    VertexArray[9][0]=0;
    VertexArray[9][1]=0;
    VertexArray[9][2]=-0.25*k;

    VertexArray[10][0]=0.25*k;
    VertexArray[10][1]=0;
    VertexArray[10][2]=-0.25*k;

    VertexArray[11][0]=0.25*k;
    VertexArray[11][1]=0;
    VertexArray[11][2]=0;
//4
    VertexArray[12][0]=0;
    VertexArray[12][1]=0;
    VertexArray[12][2]=0;

    VertexArray[13][0]=0.25*k;
    VertexArray[13][1]=0;
    VertexArray[13][2]=0;

    VertexArray[14][0]=0.25*k;
    VertexArray[14][1]=2*k;
    VertexArray[14][2]=0;

    VertexArray[15][0]=0;
    VertexArray[15][1]=2*k;
    VertexArray[15][2]=0;
//5
    VertexArray[16][0]=0;
    VertexArray[16][1]=0;
    VertexArray[16][2]=-0.25*k;

    VertexArray[17][0]=0.25*k;
    VertexArray[17][1]=0;
    VertexArray[17][2]=-0.25*k;

    VertexArray[18][0]=0.25*k;
    VertexArray[18][1]=2*k;
    VertexArray[18][2]=-0.25*k;

    VertexArray[19][0]=0;
    VertexArray[19][1]=2*k;
    VertexArray[19][2]=-0.25*k;
//6
    VertexArray[20][0]=0;
    VertexArray[20][1]=1.75*k;
    VertexArray[20][2]=-0.25*k;

    VertexArray[21][0]=0;
    VertexArray[21][1]=2*k;
    VertexArray[21][2]=-0.25*k;

    VertexArray[22][0]=1.2*k;
    VertexArray[22][1]=2*k;
    VertexArray[22][2]=-0.25*k;

    VertexArray[23][0]=1.14*k;
    VertexArray[23][1]=1.75*k;
    VertexArray[23][2]=-0.25*k;
//up front
    VertexArray[24][0]=0;
    VertexArray[24][1]=1.75*k;
    VertexArray[24][2]=0;

    VertexArray[25][0]=0;
    VertexArray[25][1]=2*k;
    VertexArray[25][2]=0;

    VertexArray[26][0]=1.2*k;
    VertexArray[26][1]=2*k;
    VertexArray[26][2]=0;

    VertexArray[27][0]=1.14*k;
    VertexArray[27][1]=1.75*k;
    VertexArray[27][2]=0;
//up up
    VertexArray[28][0]=0;
    VertexArray[28][1]=1.75*k;
    VertexArray[28][2]=0;

    VertexArray[29][0]=0;
    VertexArray[29][1]=1.75*k;
    VertexArray[29][2]=-0.25*k;

    VertexArray[30][0]=1.14*k;
    VertexArray[30][1]=1.75*k;
    VertexArray[30][2]=-0.25*k;

    VertexArray[31][0]=1.14*k;
    VertexArray[31][1]=1.75*k;
    VertexArray[31][2]=0;
//up down
    VertexArray[32][0]=0;
    VertexArray[32][1]=2*k;
    VertexArray[32][2]=0;

    VertexArray[33][0]=0;
    VertexArray[33][1]=2*k;
    VertexArray[33][2]=-0.25*k;

    VertexArray[34][0]=1.2*k;
    VertexArray[34][1]=2*k;
    VertexArray[34][2]=-0.25*k;

    VertexArray[35][0]=1.2*k;
    VertexArray[35][1]=2*k;
    VertexArray[35][2]=0;
// забытое ребро
    VertexArray[36][0]=0.25*k;
    VertexArray[36][1]=0;
    VertexArray[36][2]=0;

    VertexArray[37][0]=0.25*k;
    VertexArray[37][1]=0;
    VertexArray[37][2]=-0.25*k;

    VertexArray[38][0]=0.25*k;
    VertexArray[38][1]=2*k;
    VertexArray[38][2]=-0.25*k;

    VertexArray[39][0]=0.25*k;
    VertexArray[39][1]=2*k;
    VertexArray[39][2]=0;
//right front and back
    VertexArray[40][0]=1.14*k;
    VertexArray[40][1]=1.75*k;
    VertexArray[40][2]=0;

    VertexArray[41][0]=1.2*k;
    VertexArray[41][1]=2*k;
    VertexArray[41][2]=0;

    VertexArray[42][0]=1.51*k;
    VertexArray[42][1]=1.56*k;
    VertexArray[42][2]=0;

    VertexArray[43][0]=1.26*k;
    VertexArray[43][1]=1.56*k;
    VertexArray[43][2]=0;
//right back
    VertexArray[44][0]=1.14*k;
    VertexArray[44][1]=1.75*k;
    VertexArray[44][2]=-0.25*k;

    VertexArray[45][0]=1.2*k;
    VertexArray[45][1]=2*k;
    VertexArray[45][2]=-0.25*k;

    VertexArray[46][0]=1.51*k;
    VertexArray[46][1]=1.56*k;
    VertexArray[46][2]=-0.25*k;

    VertexArray[47][0]=1.26*k;
    VertexArray[47][1]=1.56*k;
    VertexArray[47][2]=-0.25*k;
//
    VertexArray[48][0]=1.26*k;
    VertexArray[48][1]=1.56*k;
    VertexArray[48][2]=0;

    VertexArray[49][0]=1.26*k;
    VertexArray[49][1]=1.56*k;
    VertexArray[49][2]=-0.25*k;

    VertexArray[50][0]=1.14*k;
    VertexArray[50][1]=1.75*k;
    VertexArray[50][2]=-0.25*k;

    VertexArray[51][0]=1.14*k;
    VertexArray[51][1]=1.75*k;
    VertexArray[51][2]=0;
//////////
    VertexArray[52][0]=1.2*k;
    VertexArray[52][1]=2*k;
    VertexArray[52][2]=0;

    VertexArray[53][0]=1.2*k;
    VertexArray[53][1]=2*k;
    VertexArray[53][2]=-0.25*k;

    VertexArray[54][0]=1.51*k;
    VertexArray[54][1]=1.56*k;
    VertexArray[54][2]=-0.25*k;

    VertexArray[55][0]=1.51*k;
    VertexArray[55][1]=1.56*k;
    VertexArray[55][2]=0;
// front and back
    VertexArray[56][0]=1.26*k;
    VertexArray[56][1]=1.56*k;
    VertexArray[56][2]=0;

    VertexArray[57][0]=1.51*k;
    VertexArray[57][1]=1.56*k;
    VertexArray[57][2]=0;

    VertexArray[58][0]=1.51*k;
    VertexArray[58][1]=1.31*k;
    VertexArray[58][2]=0;

    VertexArray[59][0]=1.26*k;
    VertexArray[59][1]=1.31*k;
    VertexArray[59][2]=0;
// back
    VertexArray[60][0]=1.26*k;
    VertexArray[60][1]=1.56*k;
    VertexArray[60][2]=-0.25*k;

    VertexArray[61][0]=1.51*k;
    VertexArray[61][1]=1.56*k;
    VertexArray[61][2]=-0.25*k;

    VertexArray[62][0]=1.51*k;
    VertexArray[62][1]=1.31*k;
    VertexArray[62][2]=-0.25*k;

    VertexArray[63][0]=1.26*k;
    VertexArray[63][1]=1.31*k;
    VertexArray[63][2]=-0.25*k;
//right left and right
    VertexArray[64][0]=1.26*k;
    VertexArray[64][1]=1.31*k;
    VertexArray[64][2]=0;

    VertexArray[65][0]=1.26*k;
    VertexArray[65][1]=1.56*k;
    VertexArray[65][2]=0;

    VertexArray[66][0]=1.26*k;
    VertexArray[66][1]=1.56*k;
    VertexArray[66][2]=-0.25*k;

    VertexArray[67][0]=1.26*k;
    VertexArray[67][1]=1.31*k;
    VertexArray[67][2]=-0.25*k;
// right
    VertexArray[68][0]=1.51*k;
    VertexArray[68][1]=1.31*k;
    VertexArray[68][2]=0;

    VertexArray[69][0]=1.51*k;
    VertexArray[69][1]=1.56*k;
    VertexArray[69][2]=0;

    VertexArray[70][0]=1.51*k;
    VertexArray[70][1]=1.56*k;
    VertexArray[70][2]=-0.25*k;

    VertexArray[71][0]=1.51*k;
    VertexArray[71][1]=1.31*k;
    VertexArray[71][2]=-0.25*k;
//////////////// front and back
    VertexArray[72][0]=1.13*k;
    VertexArray[72][1]=1.12*k;
    VertexArray[72][2]=0;

    VertexArray[73][0]=1.26*k;
    VertexArray[73][1]=1.31*k;
    VertexArray[73][2]=0;

    VertexArray[74][0]=1.51*k;
    VertexArray[74][1]=1.31*k;
    VertexArray[74][2]=0;

    VertexArray[75][0]=1.2*k;
    VertexArray[75][1]=0.87*k;
    VertexArray[75][2]=0;
 // back
    VertexArray[76][0]=1.13*k;
    VertexArray[76][1]=1.12*k;
    VertexArray[76][2]=-0.25*k;

    VertexArray[77][0]=1.26*k;
    VertexArray[77][1]=1.31*k;
    VertexArray[77][2]=-0.25*k;

    VertexArray[78][0]=1.51*k;
    VertexArray[78][1]=1.31*k;
    VertexArray[78][2]=-0.25*k;

    VertexArray[79][0]=1.2*k;
    VertexArray[79][1]=0.87*k;
    VertexArray[79][2]=-0.25*k;
//right right and left
    VertexArray[80][0]=1.13*k;
    VertexArray[80][1]=1.12*k;
    VertexArray[80][2]=0;

    VertexArray[81][0]=1.26*k;
    VertexArray[81][1]=1.31*k;
    VertexArray[81][2]=0;

    VertexArray[82][0]=1.26*k;
    VertexArray[82][1]=1.31*k;
    VertexArray[82][2]=-0.25*k;

    VertexArray[83][0]=1.13*k;
    VertexArray[83][1]=1.12*k;
    VertexArray[83][2]=-0.25*k;
//left
    VertexArray[84][0]=1.51*k;
    VertexArray[84][1]=1.31*k;
    VertexArray[84][2]=-0.25*k;

    VertexArray[85][0]=1.2*k;
    VertexArray[85][1]=0.87*k;
    VertexArray[85][2]=-0.25*k;

    VertexArray[86][0]=1.2*k;
    VertexArray[86][1]=0.87*k;
    VertexArray[86][2]=0;

    VertexArray[87][0]=1.51*k;
    VertexArray[87][1]=1.31*k;
    VertexArray[87][2]=0;
// front and back
    VertexArray[88][0]=0;
    VertexArray[88][1]=1.12*k;
    VertexArray[88][2]=0;

    VertexArray[89][0]=1.13*k;
    VertexArray[89][1]=1.12*k;
    VertexArray[89][2]=0;

    VertexArray[90][0]=1.2*k;
    VertexArray[90][1]=0.87*k;
    VertexArray[90][2]=0;

    VertexArray[91][0]=0;
    VertexArray[91][1]=0.87*k;
    VertexArray[91][2]=0;
// back
    VertexArray[92][0]=0;
    VertexArray[92][1]=1.12*k;
    VertexArray[92][2]=-0.25*k;

    VertexArray[93][0]=1.13*k;
    VertexArray[93][1]=1.12*k;
    VertexArray[93][2]=-0.25*k;

    VertexArray[94][0]=1.2*k;
    VertexArray[94][1]=0.87*k;
    VertexArray[94][2]=-0.25*k;

    VertexArray[95][0]=0;
    VertexArray[95][1]=0.87*k;
    VertexArray[95][2]=-0.25*k;
 //up and down
    VertexArray[96][0]=0;
    VertexArray[96][1]=1.12*k;
    VertexArray[96][2]=0;

    VertexArray[97][0]=1.13*k;
    VertexArray[97][1]=1.12*k;
    VertexArray[97][2]=0;

    VertexArray[98][0]=1.13*k;
    VertexArray[98][1]=1.12*k;
    VertexArray[98][2]=-0.25*k;

    VertexArray[99][0]=0;
    VertexArray[99][1]=1.12*k;
    VertexArray[99][2]=-0.25*k;
// down
    VertexArray[100][0]=0;
    VertexArray[100][1]=0.87*k;
    VertexArray[100][2]=-0.25*k;

    VertexArray[101][0]=1.2*k;
    VertexArray[101][1]=0.87*k;
    VertexArray[101][2]=-0.25*k;

    VertexArray[102][0]=1.2*k;
    VertexArray[102][1]=0.87*k;
    VertexArray[102][2]=0;

    VertexArray[103][0]=0;
    VertexArray[103][1]=0.87*k;
    VertexArray[103][2]=0;
}
void letter::getIndexArray()
{
    int index=0;
    int vertexIndex=0;
    for (int i = 0; i < 26; i++) {
           for (int j = 0; j < 4; j++) {
               IndexArray[index][j] = vertexIndex++;
           }
           index++;
    }
}
void letter::draw_rotation_z()
{
    GLfloat c[104][3];
    GLfloat conv[3][3] ;
    conv[0][0]=cos(angle*(M_PI/180.0));;
    conv[0][1]=-sin(angle*(M_PI/180.0));;
    conv[0][2]=0;

    conv[1][0]=sin(angle*(M_PI/180.0));
    conv[1][1]=cos(angle*(M_PI/180.0));
    conv[1][2]=0;

    conv[2][0]=0;
    conv[2][1]=0;
    conv[2][2]=1;
    qDebug()<<"Z rotation matrix"<<endl;
    for(int i=0; i < 3; i++)
    {
    qDebug()<<conv[i][0]<<" "<<conv[i][1]<<" "<<conv[i][2]<<" ";
     qDebug()<<endl;
    }

    for(int i=0; i < 104; i++)
    {
     for(int j=0; j < 3; j++)
     {
     c[i][j]=0;
     for(int k=0; k < 3; k++)
     {
     c[i][j]+=VertexArray[i][k]*conv[k][j];
     }
     }
    }
    glVertexPointer(3, GL_FLOAT, 0, c);
    glDrawElements(GL_QUADS, 104, GL_UNSIGNED_BYTE, IndexArray);

}
void letter::draw_rotation_y()
{
    GLfloat c[104][3];
    GLfloat conv[3][3] ;
    conv[0][0]=cos(angle*(M_PI/180.0));
    conv[0][1]=0;
    conv[0][2]=sin(angle*(M_PI/180.0));

    conv[1][0]=0;
    conv[1][1]=1;
    conv[1][2]=0;

    conv[2][0]=-sin(angle*(M_PI/180.0));
    conv[2][1]=0;
    conv[2][2]=cos(angle*(M_PI/180.0));

    qDebug()<<"Y rotation matrix"<<endl;
    for(int i=0; i < 3; i++)
    {
     qDebug()<<conv[i][0]<<" "<<conv[i][1]<<" "<<conv[i][2]<<" ";
     qDebug()<<endl;
    }

    for(int i=0; i < 104; i++)
    {
     for(int j=0; j < 3; j++)
     {
     c[i][j]=0;
     for(int k=0; k < 3; k++)
     {
     c[i][j]+=VertexArray[i][k]*conv[k][j];
     }
     }
    }
    glVertexPointer(3, GL_FLOAT, 0, c);
    glDrawElements(GL_QUADS, 104, GL_UNSIGNED_BYTE, IndexArray);

}
void letter::draw_rotation_x()
{
    GLfloat c[104][3];
    GLfloat conv[3][3] ;
    conv[0][0]=1;
    conv[0][1]=0;
    conv[0][2]=0;

    conv[1][0]=0;
    conv[1][1]=cos(angle*(M_PI/180.0));
    conv[1][2]=-sin(angle*(M_PI/180.0));

    conv[2][0]=0;
    conv[2][1]=sin(angle*(M_PI/180.0));
    conv[2][2]=cos(angle*(M_PI/180.0));

    qDebug()<<"X rotation matrix"<<endl;
    for(int i=0; i < 3; i++)
    {
    qDebug()<<conv[i][0]<<" "<<conv[i][1]<<" "<<conv[i][2]<<" ";
     qDebug()<<endl;
    }

    for(int i=0; i < 104; i++)
    {
     for(int j=0; j < 3; j++)
     {
     c[i][j]=0;
     for(int k=0; k < 3; k++)
     {
     c[i][j]+=VertexArray[i][k]*conv[k][j];
     }
     }
    }
    glVertexPointer(3, GL_FLOAT, 0, c);
    glDrawElements(GL_QUADS, 104, GL_UNSIGNED_BYTE, IndexArray);

}
void letter::draw_transfer()
{
    GLfloat c[104][3];
    GLfloat conv[4][3] ;
    conv[0][0]=1;
    conv[0][1]=0;
    conv[0][2]=0;

    conv[1][0]=0;
    conv[1][1]=1;
    conv[1][2]=0;

    conv[2][0]=0;
    conv[2][1]=0;
    conv[2][2]=1;

    conv[3][0]=x_transfer;
    conv[3][1]=y_transfer;
    conv[3][2]=z_transfer;

    qDebug()<<"transfer matrix"<<endl;
    for(int i=0; i < 4; i++)
    {
       qDebug()<<conv[i][0]<<" "<<conv[i][1]<<" "<<conv[i][2]<<" ";
       qDebug()<<endl;
    }

    for(int i=0; i < 104; i++)
    {
     for(int j=0; j < 3; j++)
     {
     c[i][j]=0;
     for(int k=0; k < 4; k++)
     {
         if(k==3)
      c[i][j]+=1*conv[k][j];
         else
     c[i][j]+=VertexArray[i][k]*conv[k][j];
     }
     }
    }
    glVertexPointer(3, GL_FLOAT, 0, c);
    glDrawElements(GL_QUADS, 104, GL_UNSIGNED_BYTE, IndexArray);
}
void letter::draw_scaling()
{
    GLfloat c[104][3];
    GLfloat conv[3][3] ;
    conv[0][0]=x_scale;
    conv[0][1]=0;
    conv[0][2]=0;

    conv[1][0]=0;
    conv[1][1]=y_scale;
    conv[1][2]=0;

    conv[2][0]=0;
    conv[2][1]=0;
    conv[2][2]=z_scale;

    qDebug()<<"scale matrix"<<endl;
    for(int i=0; i < 3; i++)
    {
     qDebug()<<conv[i][0]<<" "<<conv[i][1]<<" "<<conv[i][2]<<" ";
     qDebug()<<endl;
    }

    for(int i=0; i < 104; i++)
    {
     for(int j=0; j < 3; j++)
     {
     c[i][j]=0;
     for(int k=0; k < 3; k++)
     c[i][j]+=VertexArray[i][k]*conv[k][j];
     }
    }
    glVertexPointer(3, GL_FLOAT, 0, c);
    glDrawElements(GL_QUADS, 104, GL_UNSIGNED_BYTE, IndexArray);
}
void letter::draw_xy_projection()
{
    GLfloat c[104][3];
    GLfloat conv[3][3] ;
    conv[0][0]=1;
    conv[0][1]=0;
    conv[0][2]=0;

    conv[1][0]=0;
    conv[1][1]=1;
    conv[1][2]=0;

    conv[2][0]=0;
    conv[2][1]=0;
    conv[2][2]=0;
    for(int i=0; i < 104; i++)
    {
     for(int j=0; j < 3; j++)
     {
     c[i][j]=0;
     for(int k=0; k < 3; k++)
     c[i][j]+=VertexArray[i][k]*conv[k][j];
     }
    }
    glVertexPointer(3, GL_FLOAT, 0, c);
    glDrawElements(GL_QUADS, 104, GL_UNSIGNED_BYTE, IndexArray);
}
void letter::draw_xz_projection()
{
    GLfloat c[104][3];
    GLfloat conv[3][3] ;
    conv[0][0]=1;
    conv[0][1]=0;
    conv[0][2]=0;

    conv[1][0]=0;
    conv[1][1]=0;
    conv[1][2]=0;

    conv[2][0]=0;
    conv[2][1]=0;
    conv[2][2]=1;
    for(int i=0; i < 104; i++)
    {
     for(int j=0; j < 3; j++)
     {
     c[i][j]=0;
     for(int k=0; k < 3; k++)
     c[i][j]+=VertexArray[i][k]*conv[k][j];
     }
    }
    glVertexPointer(3, GL_FLOAT, 0, c);
    glDrawElements(GL_QUADS, 104, GL_UNSIGNED_BYTE, IndexArray);
}
void letter::draw_zy_projection()
{
    GLfloat c[104][3];
    GLfloat conv[3][3] ;
    conv[0][0]=0;
    conv[0][1]=0;
    conv[0][2]=0;

    conv[1][0]=0;
    conv[1][1]=1;
    conv[1][2]=0;

    conv[2][0]=0;
    conv[2][1]=0;
    conv[2][2]=1;
    for(int i=0; i < 104; i++)
    {
     for(int j=0; j < 3; j++)
     {
     c[i][j]=0;
     for(int k=0; k < 3; k++)
     c[i][j]+=VertexArray[i][k]*conv[k][j];
     }
    }
    glVertexPointer(3, GL_FLOAT, 0, c);
    glDrawElements(GL_QUADS, 104, GL_UNSIGNED_BYTE, IndexArray);
}
void letter::drawFigure(){
    glVertexPointer(3, GL_FLOAT, 0, VertexArray);
//  glEnableClientState(GL_VERTEX_ARRAY);
    glDrawElements(GL_QUADS, 104, GL_UNSIGNED_BYTE, IndexArray);
//  glDisableClientState(GL_VERTEX_ARRAY);
}
