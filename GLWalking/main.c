//
//  main.c
//  GLWalking
//
//  Created by 小田竜太郎 on 2023/06/20.
//
//参考サイト
//https://github.com/MYMYtk/FpsGame-byOpenGL/blob/master/Source.cpp
//https://ja.wikibooks.org/wiki/OpenGL%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0/Glescraft_4
//https://tokoik.github.io/opengl/libglut.html#12


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
// #include <gl/glut.h>
#include <GLUT/GLUT.h>

//３次元ベクトル
typedef struct {
    float x;
    float y;
    float z;
} vec3;

vec3 angles;//視線方向
vec3 camPos;//カメラ位置
vec3 lookAt;//
vec3 center;

vec3 vec3_sub(vec3 a, vec3 b) {
    vec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

vec3 vec3_add(vec3 a, vec3 b) {
    vec3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

vec3 vec3_scale(vec3 v, float scalar) {
    vec3 result;
    result.x = v.x * scalar;
    result.y = v.y * scalar;
    result.z = v.z * scalar;
    return result;
}

//初期オブジェクトの配置
void scene(void)
{
    //色の宣言
    static GLfloat red[] = { 0.8, 0.2, 0.2, 1.0 };
    static GLfloat green[] = { 0.2, 0.8, 0.2, 1.0 };
    static GLfloat blue[] = { 0.2, 0.2, 0.8, 1.0 };
    static GLfloat yellow[] = { 0.8, 0.8, 0.2, 1.0 };
    static GLfloat black[] = {0.0, 0.0, 0.0, 1.0};
    static GLfloat ground[][4] = {
        { 0.6, 0.6, 0.6, 1.0 },
        { 0.3, 0.3, 0.3, 1.0 }
    };
    
    int i, j;
    
    glPushMatrix();
    glTranslated(0.0, 0.0, -3.0);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
    glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(0.0, 0.0, 3.0);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
    glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(-3.0, 0.0, 0.0);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
    glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(3.0, 0.0, 0.0);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
    glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(0.0, 3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, black);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //地面
    glBegin(GL_QUADS);
    glNormal3d(0.0, 1.0, 0.0);
    for (j = -5; j < 5; ++j) {
        for (i = -5; i < 5; ++i) {
            glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
            glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
            glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
            glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
            glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
        }
    }
    glEnd();
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// 画面をクリア
    glLoadIdentity();//モデルビュー変換行列の初期化
    
    //カメラ位置
    center = vec3_add(camPos, lookAt);
    gluLookAt(camPos.x, camPos.y, camPos.z, center.x, center.y, center.z, 0.0, 1.0, 0.0);
    
    //光源の位置
    static GLfloat lightpos[] = { 3.0, 4.0, 5.0, 1.0 }; //光源の位置
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    
    // シーンの描画
    scene();
    
    glFlush();
    
    glutSwapBuffers();

}

void resize(int w, int h){
    glViewport(0, 0, w, h);// ウィンドウ全体をビューポートにする
    
    // 透視変換行列
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
    
    glMatrixMode(GL_MODELVIEW);// モデルビュー変換行列の指定
}

//マウス入力
void motion(int mx, int my) {
    static bool wrap = false;
    const float mousespeed = 0.001;//マウス感度調節変数
    
    if(!wrap) {
        int ww = glutGet(GLUT_WINDOW_WIDTH);
        int wh = glutGet(GLUT_WINDOW_HEIGHT);
        
        int dx = mx - ww / 2;//マウスの物理移動量
        int dy = my - wh / 2;
        
        angles.x += dx * mousespeed;
        angles.y += dy * mousespeed;
        
        if(angles.x < -M_PI) angles.x += M_PI * 2;
        else if(angles.x > M_PI) angles.x -= M_PI * 2;
        
        if(angles.y < -M_PI / 2) angles.y = -M_PI / 2;
        if(angles.y > M_PI / 2) angles.y = M_PI / 2;
        
        lookAt.x = sinf(angles.x) * cosf(angles.y);
        lookAt.y = sinf(angles.y);
        lookAt.z = cosf(angles.x) * cosf(angles.y);
        
        wrap = true;
        glutWarpPointer(ww / 2, wh / 2);//マウスポインターを中央に戻す
    } else {
        wrap = false;
    }
    
    glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case ' '://ジャンプ
            // TODO:
            break;
            
        case '\033'://esc
            exit(0);//終了
    }
    
    glutPostRedisplay();
}

int mySpecialValue = 0;//キーボード入力

const int forward = 1 << 0;//0ビット目を1に
const int left = 1 << 1;
const int right = 1 << 2;
const int backward = 1 << 3;

//特殊キーを押した時
void keyPush(int key, int x, int y) {
    switch (key)
    {
        case GLUT_KEY_UP:
            mySpecialValue |= forward;
            break;
        case GLUT_KEY_LEFT:
            mySpecialValue |= left;
            break;
        case GLUT_KEY_RIGHT:
            mySpecialValue |= right;
            break;
        case GLUT_KEY_DOWN:
            mySpecialValue |= backward;
            break;
        default:
            break;
    }
}

//特殊キーを離した時
void keyRelease(int key, int x, int y) {
    switch (key)
    {
        case GLUT_KEY_UP:
            mySpecialValue &= ~(forward);
            break;
        case GLUT_KEY_LEFT:
            mySpecialValue &= ~(left);
            break;
        case GLUT_KEY_RIGHT:
            mySpecialValue &= ~(right);
            break;
        case GLUT_KEY_DOWN:
            mySpecialValue &= ~(backward);
            break;
        default:
            break;
    }
}

void idle(void) {
    static int pt = 0;
    const float movespeed = 10;//移動量調節変数
    
    // Calculate time since last call to idle()
    int t = glutGet(GLUT_ELAPSED_TIME);
    float dt = (t - pt) * 1.0e-3;
    pt = t;
    
    // Calculate movement vectors
    vec3 forward_dir;
    forward_dir.x = sinf(angles.x);
    forward_dir.y = 0;
    forward_dir.z = cosf(angles.x);
    
    vec3 right_dir;
    right_dir.x = -forward_dir.z;
    right_dir.y = 0;
    right_dir.z = forward_dir.x;
    
    // Update camera camPos
    if (mySpecialValue & left)
        camPos = vec3_sub(camPos, vec3_scale(right_dir, movespeed * dt));
    if (mySpecialValue & right)
        camPos = vec3_add(camPos, vec3_scale(right_dir, movespeed * dt));
    if (mySpecialValue & forward)
        camPos = vec3_add(camPos, vec3_scale(forward_dir, movespeed * dt));
    if (mySpecialValue & backward)
        camPos = vec3_sub(camPos, vec3_scale(forward_dir, movespeed * dt));
    
    // Redraw the scene
    glutPostRedisplay();
}


//初期化
void init(void){
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

int main(int argc, char *argv[]){
    glutInit(&argc, argv);
    glutInitWindowPosition(100, 100);//ウィンドウ
    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("windowTitle");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyPush);
    glutSpecialUpFunc(keyRelease);
    glutIdleFunc(idle);
    
    glutMotionFunc(motion);//マウスをキャプチャ
    glutPassiveMotionFunc(motion);
    glutSetCursor(GLUT_CURSOR_NONE);//マウスカーソル無効
    
    init();
    glutMainLoop();
    return 0;
    
}
