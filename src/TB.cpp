#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <iostream> 

void setAutoPOV(int view);

// KONSTANTA
const float PI = 3.14159265f;
const float MOVE_SPEED = 0.5f; 
const float MOUSE_SENS = 0.2f;

// KAMERA 
void setAutoPOV(int view);
float camX = 20.0f, camY = 10.0f, camZ = 30.0f;
float camYaw = 0.0f, camPitch = -15.0f;
bool autoCam = false; //otomatis
float orbitAngle = 0.0f;
float orbitRadius = 120.0f; // jarak kamera
float orbitHeight = 80.0f;  // ketinggian
float autoAngle = 0.0f;  // rotasi 360
bool keys[256] = { false };
float autoYaw = 0.0f;
int winW = 1280, winH = 720;

// ANIMASI
bool isLoading = true;
float fireAnim = 0.0f;
float loadingProgress = 0.0f;

// Siang/Malam
bool isDay = true; 

// transformasi balon
float balScale = 1.0f;      // Skala awal
float balAngle = 0.0f;      // Sudut rotasi
float balTx = 0.0f;         // Geser X
float balTy = 0.0f;         // Geser Y
float balTz = 0.0f;         // Geser Z

// TRANSFORMASI KADO 
float giftTx[3] = {0, 0, 0};
float giftTy[3] = {0, 0, 0};
float giftTz[3] = {0, 0, 0};

int activeGift = 0; // kado aktif (0,1,2)


// ================= INIT ======================
void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_LIGHT0); 
}

// ================= CAHAYA =========
void setupLighting() {
    if (isDay) {
        // SIANG
        glClearColor(0.9f, 0.9f, 0.9f, 1.0f); 
        glDisable(GL_LIGHT1); 

        GLfloat dayAmbient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
        GLfloat dayDiffuse[]  = {0.9f, 0.9f, 0.9f, 1.0f}; 
        GLfloat daySpecular[] = {0.7f, 0.7f, 0.7f, 1.0f};
        GLfloat lightPos[]    = {20.0f, 50.0f, 20.0f, 1.0f}; 

        glLightfv(GL_LIGHT0, GL_AMBIENT, dayAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, dayDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, daySpecular);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

        GLfloat globalAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    } else {
        // MODE MALAM ---
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

        GLfloat nightAmbient[] = {0.05f, 0.05f, 0.05f, 1.0f};
        GLfloat nightDiffuse[] = {0.1f, 0.1f, 0.15f, 1.0f}; 
        GLfloat nightSpec[]    = {0.1f, 0.1f, 0.1f, 1.0f};
        GLfloat moonPos[]      = {-20.0f, 50.0f, -20.0f, 1.0f};

        glLightfv(GL_LIGHT0, GL_AMBIENT, nightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, nightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, nightSpec);
        glLightfv(GL_LIGHT0, GL_POSITION, moonPos);

        GLfloat globalAmbient[] = {0.05f, 0.05f, 0.05f, 1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

        glEnable(GL_LIGHT1);

        GLfloat candleDiffuse[]  = {1.0f, 0.6f, 0.2f, 1.0f}; 
        GLfloat candleSpecular[] = {1.0f, 0.8f, 0.4f, 1.0f};
        GLfloat candlePos[]      = {0.0f, 5.0f, 0.0f, 1.0f}; 

        glLightfv(GL_LIGHT1, GL_DIFFUSE, candleDiffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, candleSpecular);
        glLightfv(GL_LIGHT1, GL_POSITION, candlePos);

        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.5f);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05f);
        glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01f);
    }
}

// ================= LOADING SCREEN ============
void drawLoadingScreen() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, winW, 0, winH);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LIGHTING);

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(winW/2 - 60, winH/2 + 40);
    const char* text = "READY FOR PARTY?...";
    for (int i = 0; text[i]; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);

    float barW = 400, barH = 25;
    float x = winW/2 - barW/2;
    float y = winH/2;

    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + barW, y);
        glVertex2f(x + barW, y + barH);
        glVertex2f(x, y + barH);
    glEnd();

    glColor3f(0.2f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + barW * (loadingProgress / 100.0f), y);
        glVertex2f(x + barW * (loadingProgress / 100.0f), y + barH);
        glVertex2f(x, y + barH);
    glEnd();

    glutSwapBuffers();
}

// ================= UPDATE KAMERA =============
void updateCamera() {
    float yawRad = camYaw * PI / 180.0f;
    float pitchRad = camPitch * PI / 180.0f;

    float fx = sin(yawRad) * cos(pitchRad);
    float fy = -sin(pitchRad);
    float fz = -cos(yawRad) * cos(pitchRad);

    float rx = sin(yawRad + PI / 2);
    float rz = -cos(yawRad + PI / 2);

    if (keys['w']) { camX += fx * MOVE_SPEED; camY += fy * MOVE_SPEED; camZ += fz * MOVE_SPEED; }
    if (keys['s']) { camX -= fx * MOVE_SPEED; camY -= fy * MOVE_SPEED; camZ -= fz * MOVE_SPEED; }
    if (keys['a']) { camX -= rx * MOVE_SPEED; camZ -= rz * MOVE_SPEED; }
    if (keys['d']) { camX += rx * MOVE_SPEED; camZ += rz * MOVE_SPEED; }
}

// ================= GAMBAR LED WAVE ===========
void drawLEDWave(float x1, float z1, float x2, float z2) {
    glDisable(GL_LIGHTING); 

    int lampCount = 40;
    float topY = 30.0f;
    float waveAmp = 3.0f;
    float time = glutGet(GLUT_ELAPSED_TIME) * 0.002f; 

    for (int i = 0; i <= lampCount; i++) {
        float t = (float)i / lampCount;
        float x = x1 + (x2 - x1) * t;
        float z = z1 + (z2 - z1) * t;
        float y = topY - sin(t * PI * 2 + time) * waveAmp;

        switch (i % 4) {
            case 0: glColor3ub(255, 80, 80);  break;
            case 1: glColor3ub(80, 255, 120); break;
            case 2: glColor3ub(80, 120, 255); break;
            case 3: glColor3ub(255, 255, 120);break;
        }

        glPushMatrix();
            glTranslatef(x, y, z);
            glutSolidSphere(0.6f, 16, 16);
        glPopMatrix();
    }
    if(isDay || !isDay) glEnable(GL_LIGHTING); 
}

// ================= DISPLAY UTAMA =============
void display() {
    if (isLoading) {
        drawLoadingScreen();
        return;
    }

    setupLighting();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)winW / winH, 0.1, 500.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float yawRad = camYaw * PI / 180.0f;
    float pitchRad = camPitch * PI / 180.0f;
    float fx = sin(yawRad) * cos(pitchRad);
    float fy = -sin(pitchRad);
    float fz = -cos(yawRad) * cos(pitchRad);

	if (autoCam) {
	    gluLookAt(camX, camY, camZ,
	              0, 0, 0,   // posisi kamera (tengah)
	              0, 1, 0);
	} else {
	    gluLookAt(camX, camY, camZ,
	              camX + fx, camY + fy, camZ + fz,
	              0, 1, 0);
	}


	// ================= GAMBAR RUANGAN =================
	glDisable(GL_LIGHTING);
	
	if (isDay)
	    glColor3f(0.5f, 0.75f, 0.8f);
	else
	    glColor3f(0.2f, 0.3f, 0.35f);
	
	glBegin(GL_QUADS);
	    glVertex3f(-100, -15, -100);
	    glVertex3f( 100, -15, -100);
	    glVertex3f( 100, -15,  100);
	    glVertex3f(-100, -15,  100);
	glEnd();
	
	if (isDay)
	    glColor3f(1.0f, 0.8f, 0.9f);
	else
	    glColor3f(0.4f, 0.3f, 0.35f);
	
	glBegin(GL_QUADS);
	    glVertex3f(-50, -15,  50);
	    glVertex3f( 50, -15,  50);
	    glVertex3f( 50,  35,  50);
	    glVertex3f(-50,  35,  50);
	
	    glVertex3f(-50, -15, -50);
	    glVertex3f( 50, -15, -50);
	    glVertex3f( 50,  35, -50);
	    glVertex3f(-50,  35, -50);
	
	    glVertex3f(-50, -15, -50);
	    glVertex3f(-50, -15,  50);
	    glVertex3f(-50,  35,  50);
	    glVertex3f(-50,  35, -50);
	
	    glVertex3f( 50, -15, -50);
	    glVertex3f( 50, -15,  50);
	    glVertex3f( 50,  35,  50);
	    glVertex3f( 50,  35, -50);
	glEnd();

glEnable(GL_LIGHTING);

    // ================= MEJA ===================
    GLfloat woodSpec[] = {0.25f, 0.15f, 0.05f, 1};
    glMaterialfv(GL_FRONT, GL_SPECULAR, woodSpec);
    glPushMatrix();
        glTranslatef(0, -5, 0);
        glColor3ub(139, 69, 19);
        glPushMatrix();
            glScalef(30, 1.5, 20); glutSolidCube(1);
        glPopMatrix();
        float px[2] = { 14,-14 }; float pz[2] = { 9,-9 };
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++) {
                glPushMatrix();
                    glTranslatef(px[i], -5, pz[j]);
                    glScalef(1.5, 10, 1.5); glutSolidCube(1);
                glPopMatrix();
            }
    glPopMatrix();

	// ================= KADO =================
	GLfloat giftSpec[] = {0.9f, 0.9f, 0.9f, 1};
	glMaterialfv(GL_FRONT, GL_SPECULAR, giftSpec);
	
	float giftX[3] = { -12.0f, -14.5f, -10.0f };
	float giftZ[3] = {    7.0f,     5.5f,    9.0f };
	float giftS[3] = { 1.6f, 1.2f, 0.9f };
	
	unsigned char giftColor[3][3] = {
	    {255,120,180},
	    {255,170,210},
	    {240, 90,160}
	};

for (int i = 0; i < 3; i++) {
    glPushMatrix();

        glTranslatef(
    giftX[i] + giftTx[i],
    -3.4f + giftTy[i],
    giftZ[i] + giftTz[i]
);

        glScalef(giftS[i], giftS[i], giftS[i]);

        glColor3ub(
            giftColor[i][0],
            giftColor[i][1],
            giftColor[i][2]
        );
        glutSolidCube(1.5); // Box

        glColor3ub(255, 255, 255);
        glPushMatrix();
            glScalef(0.25f, 1.55f, 1.55f);
            glutSolidCube(1.0); // Pita 1
        glPopMatrix();

        glPushMatrix();
            glScalef(1.55f, 0.25f, 1.55f);
            glutSolidCube(1.0); // Pita 2
        glPopMatrix();

        glColor3ub(255, 200, 230);
        glPushMatrix();
            glTranslatef(0, 0.9f, 0);
            glScalef(1.6f, 0.25f, 1.6f);
            glutSolidCube(1.0); // Tutup
        glPopMatrix();

        glColor3ub(255, 80, 150);
        glPushMatrix();
            glTranslatef(0.25f, 1.1f, 0);
            glutSolidSphere(0.2, 14, 14);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-0.25f, 1.1f, 0);
            glutSolidSphere(0.2, 14, 14);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0, 1.0f, 0);
            glRotatef(-90, 1, 0, 0);
            glutSolidCone(0.1, 0.3, 12, 5);
        glPopMatrix();

    glPopMatrix();
}

	// ================= KUE ULANG TAHUN =================
	GLfloat cakeSpec[] = {0.08f, 0.08f, 0.08f, 1};
	glMaterialfv(GL_FRONT, GL_SPECULAR, cakeSpec);
	
	GLUquadric* quad = gluNewQuadric();
	
	glPushMatrix();
    glTranslatef(0, -2.8f, 0);

    // Tingkat 1
    glColor3ub(160, 82, 45);
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        gluCylinder(quad, 6.5, 6.5, 3, 40, 40);
    glPopMatrix();

    glColor3ub(255, 248, 240);
    glPushMatrix();
        glTranslatef(0, 3, 0);
        glScalef(7, 0.7, 7);
        glutSolidSphere(1, 30, 30);
    glPopMatrix();

    // Tingkat 2
    glTranslatef(0, 3.6f, 0);

    glColor3ub(255, 182, 193);
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        gluCylinder(quad, 4.5, 4.5, 2.5, 40, 40);
    glPopMatrix();

    glColor3ub(255, 250, 250);
    glPushMatrix();
        glTranslatef(0, 2.5, 0);
        glScalef(5, 0.6, 5);
        glutSolidSphere(1, 30, 30);
    glPopMatrix();

    // Tingkat 3
    glTranslatef(0, 3.0f, 0);

    glColor3ub(173, 216, 230);
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        gluCylinder(quad, 3, 3, 2, 40, 40);
    glPopMatrix();

    glColor3ub(255, 255, 255);
    glPushMatrix();
        glTranslatef(0, 2, 0);
        glScalef(3.6, 0.5, 3.6);
        glutSolidSphere(1, 30, 30);
    glPopMatrix();

        fireAnim += 0.08f;
        float flicker = (sinf(fireAnim) + 1.0f) * 0.12f; 

        for (int i = 0; i < 5; i++) {
            float a = i * (360.0f / 5.0f) * PI / 180.0f;
            float x = cosf(a) * 1.5f;
            float z = sinf(a) * 1.5f;

            glEnable(GL_LIGHTING);
            GLfloat waxSpec[] = {0.6f, 0.6f, 0.3f, 1.0f};
            glMaterialfv(GL_FRONT, GL_SPECULAR, waxSpec);
            glColor3ub(255, 255, 220);
            glPushMatrix();
                glTranslatef(x, 2.7f, z);
                glScalef(0.25f, 1.1f, 0.25f);
                glutSolidCube(1.0f);
            glPopMatrix();

            glColor3ub(20, 20, 20);
            glPushMatrix();
                glTranslatef(x, 3.3f, z);
                glScalef(0.05f, 0.2f, 0.05f);
                glutSolidCube(1.0f);
            glPopMatrix();

            if (!isDay) {
                glDisable(GL_LIGHTING); 
                glColor3ub(255, 220, 100);
                glPushMatrix();
                    glTranslatef(x, 3.3f + flicker * 0.5f, z);
                    glScalef(0.2f + flicker, 0.5f, 0.12f + flicker);
                    glutSolidSphere(0.25f, 16, 16);
                glPopMatrix();

                glColor3ub(255, 120, 0);
                glPushMatrix();
                    glTranslatef(x, 3.35f + flicker, z);
                    glScalef(0.12f + flicker, 0.8f, 0.2f + flicker);
                    glutSolidSphere(0.25f, 20, 20);
                glPopMatrix();
                
                glEnable(GL_LIGHTING);
            }
        }
    glPopMatrix();
    gluDeleteQuadric(quad);

    //lampu
	drawLEDWave(-45,   49.9f,  45,   49.9f);
	drawLEDWave(-45,  -49.9f,  45,  -49.9f);
	drawLEDWave(-49.9f, -45, -49.9f,  45);
	drawLEDWave( 49.9f, -45,  49.9f,  45);
	
	//transformasi balon
	glEnable(GL_LIGHTING);
	
	GLfloat balloonSpec[]  = {0.8f, 0.8f, 0.8f, 1};
	GLfloat balloonShine[] = {80};
	
	glMaterialfv(GL_FRONT, GL_SPECULAR,  balloonSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, balloonShine);
	
	float bx[4] = { -25,  25, -20,  20 };
	float bz[4] = { -20, -20,  25,  25 };
	
	for (int i = 0; i < 4; i++) {
    glPushMatrix();

        // Translate
        glTranslatef(
            bx[i] + balTx,
            8.0f + balTy,
            bz[i] + balTz
        );

        // Rotate
        glRotatef(balAngle, 0, 1, 0);

        // Scale
        glScalef(balScale, balScale, balScale);

        // ---------- BALON ----------
        glColor3ub(255, 80, 120);
        glPushMatrix();
            glTranslatef(0, 3.5f, 0);
            glScalef(1, 1.3f, 1);
            glutSolidSphere(2, 30, 30);
        glPopMatrix();

        GLUquadric* q = gluNewQuadric();

        glColor3ub(220, 60, 100);
        glPushMatrix();
            glTranslatef(0, 1.8f, 0);
            glRotatef(-90, 1, 0, 0);
            gluCylinder(q, 0.4, 0.4, 1.2, 20, 10);
        glPopMatrix();

        glColor3ub(200, 40, 80);
        glPushMatrix();
            glTranslatef(0, 1.6f, 0);
            glutSolidTorus(0.15, 0.45, 16, 24);
        glPopMatrix();

        // ---------- HIASAN BALON ----------
        glColor3ub(255, 215, 215);
        glPushMatrix();
            glTranslatef(0, 0.5f, 0.2f);
            glScalef(2, 1.2f, 2);

            glPushMatrix();
                glTranslated(0, 0.2, 0);
                glutSolidSphere(0.2, 10, 10);
            glPopMatrix();

            glPushMatrix();
                glTranslatef(-0.35f, 0.1f, 0);
                glRotatef(35, 0, 0, 1);
                glScalef(0.6f, 0.3f, 0.1f);
                glutSolidSphere(1.0, 10, 10);
            glPopMatrix();

            glPushMatrix();
                glTranslatef(0.35f, 0.1f, 0);
                glRotatef(-35, 0, 0, 1);
                glScalef(0.6f, 0.3f, 0.1f);
                glutSolidSphere(1.0, 10, 10);
            glPopMatrix();

            glPushMatrix();
                glTranslatef(-0.35f, 0.4f, 0);
                glRotatef(-35, 0, 0, 1);
                glScalef(0.6f, 0.3f, 0.1f);
                glutSolidSphere(1.0, 10, 10);
            glPopMatrix();

            glPushMatrix();
                glTranslatef(0.35f, 0.4f, 0);
                glRotatef(35, 0, 0, 1);
                glScalef(0.6f, 0.3f, 0.1f);
                glutSolidSphere(1.0, 10, 10);
            glPopMatrix();

            glPushMatrix();
                glTranslatef(0.2f, -0.2f, 0);
                glRotatef(15, 0, 0, 1);
                glScalef(0.15f, 0.8f, 0.05f);
                glutSolidCube(1.0);
            glPopMatrix();

            glPushMatrix();
                glTranslatef(-0.2f, -0.2f, 0);
                glRotatef(-15, 0, 0, 1);
                glScalef(0.15f, 0.8f, 0.05f);
                glutSolidCube(1.0);
            glPopMatrix();
        glPopMatrix();

        // ---------- TALI BALON ----------
        glColor3ub(50, 50, 50);
        glPushMatrix();
            glTranslatef(0, 1.4f, 0);
            glRotatef(90, 1, 0, 0);
            gluCylinder(q, 0.02, 0.10, 6.5, 8, 1);
        glPopMatrix();

        gluDeleteQuadric(q);
    glPopMatrix();
}

glutSwapBuffers();
}

// ================= LOADING & INPUT =============
void loadingTimer(int) {
    if (loadingProgress < 100) {
        loadingProgress += 2.0f;
        glutPostRedisplay();
        glutTimerFunc(40, loadingTimer, 0);
    } else {
        isLoading = false;
        std::cout << "Loading Complete." << std::endl;
        std::cout << "Kontrol Balon:" << std::endl;
        std::cout << "  [R] : Rotasi Balon" << std::endl;
        std::cout << "  [Z] : Perbesar (Scale Up)" << std::endl;
        std::cout << "  [X] : Perkecil (Scale Down)" << std::endl;
        std::cout << "  [I/J/K/L] : Geser Balon (Translate X/Z)" << std::endl;
        std::cout << "  [U/O] : Geser Balon Atas/Bawah (Translate Y)" << std::endl;
        std::cout << "  [N] : Ganti Siang/Malam" << std::endl;
    }
}

void keyboard(unsigned char key, int, int) {
    keys[key] = true;
    
    if (key == 'n' || key == 'N') {
        isDay = !isDay;
        if(isDay) std::cout << "Mode: SIANG (Lilin Mati)" << std::endl;
        else      std::cout << "Mode: MALAM (Lilin Nyala)" << std::endl;
    }

    // [LOGIKA BARU: KONTROL BALON]
    // 1. ROTATE
    if (key == 'r' || key == 'R') {
        balAngle += 5.0f;
        if (balAngle >= 360) balAngle -= 360;
    }

    // 2. SCALE
    if (key == 'z' || key == 'Z') balScale += 0.1f;
    if (key == 'x' || key == 'X') {
        balScale -= 0.1f;
        if (balScale < 0.1f) balScale = 0.1f; // Batas minimal
    }

    // 3. TRANSLATE
    if (key == 'j' || key == 'J') balTx -= 0.5f; // Kiri
    if (key == 'l' || key == 'L') balTx += 0.5f; // Kanan
    if (key == 'i' || key == 'I') balTz -= 0.5f; // Maju
    if (key == 'k' || key == 'K') balTz += 0.5f; // Mundur
    if (key == 'u' || key == 'U') balTy += 0.5f; // Atas
    if (key == 'o' || key == 'O') balTy -= 0.5f; // Bawah

    if (key == 27) exit(0);
		// ===== AUTO CAMERA MODE =====
	if (key == 'c' || key == 'C') autoCam = !autoCam;


	if (key == '1') setAutoPOV(1); // kiri
	if (key == '2') setAutoPOV(2); // kanan
	if (key == '3') setAutoPOV(3); // atas
	if (key == '4') setAutoPOV(4); // bawah
	
	// KONTROL KADO 

if (key == '[') activeGift = (activeGift + 2) % 3; // kado sebelumnya
if (key == ']') activeGift = (activeGift + 1) % 3; // kado berikutnya

// Geser kado
if (key == 't' || key == 'T') giftTx[activeGift] -= 0.5f;
if (key == 'y' || key == 'Y') giftTx[activeGift] += 0.5f;

if (key == 'g' || key == 'G') giftTz[activeGift] -= 0.5f;
if (key == 'h' || key == 'H') giftTz[activeGift] += 0.5f;

if (key == 'v' || key == 'V') giftTy[activeGift] += 0.5f;
if (key == 'b' || key == 'B') giftTy[activeGift] -= 0.5f;

	

}

void keyboardUp(unsigned char key, int, int) { keys[key] = false; }

void mouseMotion(int x, int y) {
    int cx = winW / 2, cy = winH / 2;
    static bool firstMouse = true;
    if(firstMouse) { glutWarpPointer(cx, cy); firstMouse = false; return; }

    camYaw += (x - cx) * MOUSE_SENS;
    camPitch += (y - cy) * MOUSE_SENS;
    if (camPitch > 89) camPitch = 89;
    if (camPitch < -89) camPitch = -89;
    
    if(x != cx || y != cy) glutWarpPointer(cx, cy);
}

void setAutoPOV(int view) {
    switch (view) {

        case 1: // KIRI (di dalam dinding kiri)
            camX = -45; camY = 10; camZ = 0;
            camYaw = 90; camPitch = 0;
            break;

        case 2: // KANAN (di dalam dinding kanan)
            camX = 45; camY = 10; camZ = 0;
            camYaw = -90; camPitch = 0;
            break;

        case 3: // ATAS (di bawah plafon, TOP-DOWN)
            camX = 0; camY = 33; camZ = 0;
            camYaw = 0; camPitch = -89;
            break;

        case 4: // BAWAH (di atas lantai)
            camX = 0; camY = -10; camZ = 0;
            camYaw = 0; camPitch = 15;
            break;
    }
}


void idle() {
    if (!isLoading) {
        if (autoCam) {
            orbitAngle += 0.2f;
            if (orbitAngle >= 360.0f) orbitAngle -= 360.0f;

            float rad = orbitAngle * PI / 180.0f;

            // POSISI KAMERA ORBIT (LUAR + ATAS)
            camX = cos(rad) * orbitRadius;
            camZ = sin(rad) * orbitRadius;
            camY = orbitHeight;
        } else {
            updateCamera();
        }
    }
    glutPostRedisplay();
}



void reshape(int w, int h) {
    winW = w; winH = h;
    glViewport(0, 0, w, h);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("KELOMPOK 4");
    glutFullScreen();
    glutSetCursor(GLUT_CURSOR_NONE); 

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion);
    glutMotionFunc(mouseMotion); 
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);

    glutTimerFunc(40, loadingTimer, 0);
    glutWarpPointer(winW / 2, winH / 2);

    glutMainLoop();
    return 0;
}