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
vec3 camPos = {0, 0.5, 0};//カメラ位置
vec3 lookAt;
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

const int groundSize = 50;
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
    for (j = -groundSize; j < groundSize; ++j) {
        for (i = -groundSize; i < groundSize; ++i) {
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
    
    //光源
    static GLfloat lightPosition[4] = { 0.0, 0.0, 10.0, 1.0 }; //光源の位置
    static GLfloat lightDiffuse[3]  = { 1.0,   1.0, 1.0  }; //拡散光
    static GLfloat lightAmbient[3]  = { 0.25, 0.25, 0.25 }; //環境光
    static GLfloat lightSpecular[3] = { 1.0,   1.0, 1.0  }; //鏡面光
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    
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

int mySpecialValue = 0;//移動キー入力

//マウス入力
void motion(int mx, int my) {
    static bool wrap = false;
    const float mousespeed = 0.003;//マウス感度調節変数
    const bool mouseReverseVertical = true;
    if (!mySpecialValue) {//移動キーと同時にマウスを動かすとReleaseがうまく反応しないバグを回避
        if(!wrap) {
            int ww = glutGet(GLUT_WINDOW_WIDTH);
            int wh = glutGet(GLUT_WINDOW_HEIGHT);
            
            int dx = mx - ww / 2;//マウスの物理移動量
            int dy = (mouseReverseVertical) ? -(my - wh / 2) : my - wh / 2;
            
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
    }
    
    glutPostRedisplay();
}

const int forward = 1 << 0;//0ビット目を1に
const int left = 1 << 1;
const int right = 1 << 2;
const int backward = 1 << 3;

//キーボードを押した時
void keyPush(unsigned char key, int x, int y){
    switch (key) {
        case ' '://ジャンプ
            // TODO:
            break;
            
        case '\033'://esc
            exit(0);//終了
            
        case 'w':
            mySpecialValue |= forward;
            break;
        case 'a':
            mySpecialValue |= left;
            break;
        case 'd':
            mySpecialValue |= right;
            break;
        case 's':
            mySpecialValue |= backward;
            break;
            
        default:
            break;
    }
    glutPostRedisplay();
}

//キーボードを離した時
void keyRelease(unsigned char key, int x, int y) {
    switch (key){
        case 'w':
            mySpecialValue &= ~(forward);
            break;
        case 'a':
            mySpecialValue &= ~(left);
            break;
        case 'd':
            mySpecialValue &= ~(right);
            break;
        case 's':
            mySpecialValue &= ~(backward);
            break;
            
        default:
            break;
    }
}

//特殊キー
void spKeyPush(int key, int x, int y) {
    switch (key){
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

void spKeyRelease(int key, int x, int y) {
    switch (key){
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
    
    //範囲外に移動禁止
    if(camPos.x > groundSize) camPos.x = groundSize;
    else if(camPos.x < -groundSize) camPos.x = -groundSize;
    if(camPos.y > groundSize) camPos.y = groundSize;
    else if(camPos.y < -groundSize) camPos.y = -groundSize;
    if(camPos.z > groundSize) camPos.z = groundSize;
    else if(camPos.z < -groundSize) camPos.z = -groundSize;
    
    // Redraw the scene
    glutPostRedisplay();
}

//初期化
void init(void){
    glClearColor(135/255.0, 206/255.0, 250/255.0, 0.0);//背景を空色に
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
    glutCreateWindow("windowTitle");//TODO:
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    
    glutKeyboardFunc(keyPush);//キーボード入力
    glutKeyboardUpFunc(keyRelease);
    glutSpecialFunc(spKeyPush);
    glutSpecialUpFunc(spKeyRelease);
    glutIdleFunc(idle);
    
    glutMotionFunc(motion);//マウス入力
    glutPassiveMotionFunc(motion);
    glutSetCursor(GLUT_CURSOR_NONE);//マウスカーソル無効
    
    init();
    glutMainLoop();
    return 0;
    
}
