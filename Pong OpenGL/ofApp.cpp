#include "ofApp.h"
#include <cmath>



//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetVerticalSync(false);
    ofBackground(20, 20, 20);
    ofDisableArbTex();
    glEnable(GL_DEPTH_TEST);
    is3D = true;
    firstPersonView = false;
    leftPlayerPosY = 0.0f;
    rightPlayerPosY = 0.0f;
    playerWidth = 20;
    playerHeight = 40;
    maxMoveYDrt = 175.0f;
    maxMoveYEsq = 180.0f;
    rightPlayerPosZ = 0.0f;
    leftPlayerPosZ = 0.0f;
    rightPlayerPosX = 150.0f;
    leftPlayerPosX = -150;
    leftPlayerSpeed = 5.0f;

    lensAngle = 70;
    alpha = 400;
    beta = 800;

    leftPlayerScore = 0;
    rightPlayerScore = 0;
    tempoDeJogo = 0.0f; 
    jogoAcabou = false; 
    Pausa = false;

    esferaPosX = 0.0f;
    esferaPosY = 0.0f;
    esferaPosZ = 0.0f;
    esferaSpeedX = 0.0f;
    esferaSpeedY = 0.0f;
    esferaSpeedZ = 0.0f;
    esferaRadius = 5.0f;
    cubeSize = 400.0f;
    bounciness = 1.005f;

    piscaDireita = false;
    piscaEsquerda = false;
    piscaTRestante = 0.0f;
    tempoDecremento = 0.016f;
    goloJEsquerda = false;
    goloJDireita = false;
    std::string vencedor = "";

    resetBall();

    //iluminação
    ambientOn = true;
    dirOn = false;
    dirAmbientOn = true;
    dirDiffuseOn = true;
    dirSpecularOn = true;
    pointAmbientOn = true;
    pointDiffuseOn = true;
    pointSpecularOn = true;
    pointOn = false;
    spotOn = false;
    spotAmbientOn = true;
    spotDiffuseOn = true; 
    spotSpecularOn = true;
    autoMove = true;
    localViewer = false;

    pointZtheta = 0;
    dirVecTheta = 0.;
    pointDirection = 1;

    spotExponent = 10;
    spotCutoff = 65;
    spotTheta = 0;
    spotDirVec = ofVec3f(0, 0, 0);
    spotDirAux = ofVec3f(0, 0, 0);
    spotPosVec = ofVec3f(0., gh() * 0.35, gh() * 0.25);

    img1.load("cubo.jpg");
    img2.load("bola.jpeg");
    img3.load("player1.jpg");
}


void ofApp::resetBall() {
    esferaPosX = 0.0f;
    esferaPosY = 0.0f;
    esferaPosZ = 0.0f;

    fixedSpeed = 4.2f;

    //esquerda ou direita
    if (rand() % 2 == 0) {
        directionX = 1;
    }
    else {
        directionX = -1;
    }


    float angleXY = ((rand() % 40) - 10.0f) * (PI / 180.0f); // -10º a 30º
    float angleZ = (rand() % 30 - 15) * (PI / 180.0f); //-15º a 15º

    esferaSpeedX = fixedSpeed * directionX;
    esferaSpeedY = fixedSpeed * sin(angleXY); //movimento em Y
    if (is3D) {
        esferaSpeedZ = fixedSpeed * sin(angleZ); //profundidade
    }
    else {
        esferaSpeedZ = 0.0f;
    }
    
}

void ofApp::voltarAoCentro() {
    esferaPosX = esferaPosY = esferaPosZ = 0;
    esferaSpeedX = esferaSpeedY = esferaSpeedZ = 0;
    leftPlayerPosX = -150;
    rightPlayerPosX = 150;
    leftPlayerPosY = rightPlayerPosY = 0;
    leftPlayerPosZ = rightPlayerPosZ = 0;
}



//--------------------------------------------------------------
void ofApp::update() {
    if (Pausa) {
        return;
    }
    setFrWt();

    //movimento da bola
    esferaPosX += esferaSpeedX;
    esferaPosY += esferaSpeedY;
    if (is3D) {
        esferaPosZ += esferaSpeedZ;
    }

    //automatização jogador esquerda
    if (leftPlayerPosY < esferaPosY) {
        leftPlayerPosY += leftPlayerSpeed;
    }
    else if (leftPlayerPosY > esferaPosY ) {
        leftPlayerPosY -= leftPlayerSpeed;
    }

    if (leftPlayerPosY > maxMoveYEsq) leftPlayerPosY = maxMoveYEsq;
    if (leftPlayerPosY < -maxMoveYEsq) leftPlayerPosY = -maxMoveYEsq;

    if (leftPlayerPosZ < esferaPosZ) {
        leftPlayerPosZ += leftPlayerSpeed;
    }
    else if (leftPlayerPosZ > esferaPosZ) {
        leftPlayerPosZ -= leftPlayerSpeed;
    }

    if (leftPlayerPosZ > maxMoveYEsq) leftPlayerPosZ = maxMoveYEsq;
    if (leftPlayerPosZ < -maxMoveYEsq) leftPlayerPosZ = -maxMoveYEsq;

    aguardaReset = false;

    // golo e fim de jogo
    if (esferaPosX + esferaRadius > cubeSize / 2 && !jogoAcabou && !aguardaReset) {
        leftPlayerScore++;
        piscaEsquerda = true;
        piscaTRestante = 0.7f;
        aguardaReset = true;  
        voltarAoCentro();
    }
    if (esferaPosX - esferaRadius < -cubeSize / 2 && !jogoAcabou && !aguardaReset) {
        rightPlayerScore++;
        piscaDireita = true;
        piscaTRestante = 0.7f;
        aguardaReset = true;
        voltarAoCentro(); 
    }

    // festejo "pisca"
    if (piscaEsquerda) {
        piscaTRestante -= tempoDecremento;
        if (piscaTRestante > 0.0f) {
            goloJEsquerda = true;
        }
        else {
            piscaEsquerda = false;
            goloJEsquerda = false;
            aguardaReset = false; 

            if (leftPlayerScore == 6) {
                jogoAcabou = true;
                tempoDeJogo = 0.0f;
                vencedor = "Jogador da Esquerda venceu!";
            }
            else {
                resetBall();
            }
        }
    }
    if (piscaDireita) {
        piscaTRestante -= tempoDecremento;
        if (piscaTRestante > 0.0f) {
            goloJDireita = true;
        }
        else {
            piscaDireita = false;
            goloJDireita = false;
            aguardaReset = false;

            if (rightPlayerScore == 6) {
                jogoAcabou = true;
                tempoDeJogo = 0.0f;
                vencedor = "Jogador da Direita venceu!";
            }
            else {
                resetBall();
            }
        }
    }

    //fim do jogo
    if (jogoAcabou) {
        tempoDeJogo += tempoDecremento;

        if (tempoDeJogo < 2.0f) {
            esferaPosX = 0;
            esferaPosY = 0;
            esferaPosZ = 0;
        }
        else {
            resetBall();
            jogoAcabou = false;
            leftPlayerScore = 0;
            rightPlayerScore = 0;
        }
    }

    //colisao da bola
    if (esferaPosY - esferaRadius <= -cubeSize / 2) {
        esferaPosY = -cubeSize / 2 + esferaRadius;
        esferaSpeedY *= -bounciness;
    }
    if (esferaPosY + esferaRadius >= cubeSize / 2) {
        esferaPosY = cubeSize / 2 - esferaRadius;
        esferaSpeedY *= -bounciness;
    }
    if (is3D) {
        if (esferaPosZ - esferaRadius <= -cubeSize / 2) {
            esferaPosZ = -cubeSize / 2 + esferaRadius;
            esferaSpeedZ *= -bounciness;
        }
        if (esferaPosZ + esferaRadius >= cubeSize / 2) {
            esferaPosZ = cubeSize / 2 - esferaRadius;
            esferaSpeedZ *= -bounciness;
        }
    }

    if (esferaPosX - esferaRadius < leftPlayerPosX + playerWidth / 2 &&
        esferaPosX + esferaRadius > leftPlayerPosX - playerWidth / 2 &&
        esferaPosY - esferaRadius < leftPlayerPosY + playerHeight / 2 &&
        esferaPosY + esferaRadius > leftPlayerPosY - playerHeight / 2 &&
        (!is3D || (esferaPosZ - esferaRadius < leftPlayerPosZ + playerWidth / 2 && esferaPosZ + esferaRadius > leftPlayerPosZ - playerWidth / 2))) {
        
        if (esferaPosX +esferaRadius < leftPlayerPosX + playerWidth / 2 ) {
            // Colisão superior/inferior 
            if (esferaPosY < leftPlayerPosY) {
                esferaPosY = leftPlayerPosY - playerHeight / 2 - esferaRadius;
                esferaSpeedX *= -bounciness;
                esferaSpeedY *= -bounciness;  
            }
            else {
                esferaPosY = leftPlayerPosY + playerHeight / 2 + esferaRadius;
                esferaSpeedX *= -bounciness;
                esferaSpeedY *= -bounciness;  
            }
        }
        // Colisão frente
        else {
            esferaPosX = leftPlayerPosX + playerWidth / 2 + esferaRadius;  
            esferaSpeedX *= -bounciness; 
        }
    }
    if (esferaPosX + esferaRadius > rightPlayerPosX - playerWidth / 2 &&
        esferaPosX - esferaRadius < rightPlayerPosX + playerWidth / 2 &&
        esferaPosY - esferaRadius < rightPlayerPosY + playerHeight / 2 &&
        esferaPosY + esferaRadius > rightPlayerPosY - playerHeight / 2 &&
        (!is3D || (esferaPosZ - esferaRadius < rightPlayerPosZ + playerWidth / 2 && esferaPosZ + esferaRadius > rightPlayerPosZ - playerWidth / 2))) {
        //Colisão cima/baixo
        if ( esferaPosX - esferaRadius > rightPlayerPosX - playerWidth / 2) {
            if (esferaPosY < rightPlayerPosY) {
                // Parte inferior do jogador
                esferaPosY = rightPlayerPosY - playerHeight / 2 - esferaRadius;
                esferaSpeedX *= -bounciness;
                esferaSpeedY *= -bounciness;  
            }
            else {
                // Parte superior do jogador
                esferaPosY = rightPlayerPosY + playerHeight / 2 + esferaRadius;
                esferaSpeedX *= -bounciness;
                esferaSpeedY *= -bounciness;  
            }
        }
        // Colisão frente
        else {
            esferaPosX = rightPlayerPosX - playerWidth / 2 - esferaRadius;  
            esferaSpeedX *= -bounciness;  
        }
    }

    if (autoMove) {

        //posicao da luz pontual
        pointPos[0] = gw() * 0.5 * cos(pointZtheta * PI / 180.0); // De -gw() * 0.5 para +gw() * 0.5
        pointPos[1] = gh() * 0.5 * sin(pointZtheta * PI / 180.0); 
        pointPos[2] = 0; 

        pointZtheta += pointDirection * 0.25;

        if (pointZtheta >= 180.0) {
            pointZtheta = 180.0;
            pointDirection = -1; //Trás
        }
        if (pointZtheta <= 0.0) {
            pointZtheta = 0.0;
            pointDirection = 1; //Frente
        }

        //direcao da luz direcional
        float z = gh() * 0.60 * (cos(dirVecTheta * PI / 180.));
        float y = gh() * 0.40;
        float x = 0;
        dirVec3f = ofVec3f(x, y, z) - ofVec3f(0, 0, 0);//aponta da origem para a posição
        dirVecTheta += 0.5;

        //direcao do spot
        float sx = gh() * 0.1 * cos(spotTheta * PI / 180.) ;
        float sy = cubeSize/2;
        spotDirAux = ofVec3f(sx, sy , 0.);
        spotDirVec = spotDirAux - spotPosVec;//posição da luz e posição auxiliar
        spotDirVec = spotDirVec.normalize();
        spotTheta += 0.75;

    }
    else {
        //luz pontual
        pointPos[0] = 0;
        pointPos[1] = gh() * 0.5;
        pointPos[2] = 45;
        
        //direção luz direcional
        dirVec3f = ofVec3f(0, gh() * 0.35, gh() * 0.10) - ofVec3f(0, 0, 0);
        
        //luz spot
        spotDir[0] = 0;
        spotDir[1] = 0;

    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    glViewport(0, 0, gw(), gh());

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (is3D) {
        perspective(lensAngle, alpha, beta);
    }
    else {
        glOrtho(-gw() * 0.4, gw() * 0.4, -gh() * 0.4, gh() * 0.4, -800, 800);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (is3D) {
        if (firstPersonView) {

            cameraX = rightPlayerPosX - 5;
            cameraY = rightPlayerPosY;
            cameraZ = rightPlayerPosZ;

             
            lookAtX = cameraX - 0.5;
            lookAtY = cameraY;
            lookAtZ = cameraZ;

            lookat(cameraX, cameraY, cameraZ, lookAtX, lookAtY, lookAtZ, 0, 1, 0);
        }
        else {
            cameraDistance = 600.0f;
            lookat(0, 0, cameraDistance, 0, 0, 0, 0, 1, 0);
        }
    }

    

    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, localViewer);

    if (ambientOn) {
        ambientLight[0] = 1.;//R
        ambientLight[1] = 1.;//G
        ambientLight[2] = 1.;//B
        ambientLight[3] = 1;
    }
    else {
        ambientLight[0] = 0.;
        ambientLight[1] = 0.;
        ambientLight[2] = 0.;
        ambientLight[3] = 1;
    }
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    //luz direcional
    dirVec[0] = dirVec3f.x;//x
    dirVec[1] = dirVec3f.y;//y
    dirVec[2] = dirVec3f.z;//z
    dirVec[3] = 0;

    dirAmb[0] = 2.0;//R
    dirAmb[1] = 2.0;//G
    dirAmb[2] = 2.0;//B
    dirAmb[3] = 1.0;//constante

    dirDif[0] = 2.0;//R
    dirDif[1] = 2.0;//G
    dirDif[2] = 2.0;//B
    dirDif[3] = 1.;//constante

    dirSpec[0] = 2.0;//R
    dirSpec[1] = 2.0;//G
    dirSpec[2] = 2.0;//B
    dirSpec[3] = 1.0;//constante

    glLightfv(GL_LIGHT0, GL_POSITION, dirVec);
    GLfloat off[4] = { 0.0, 0.0, 0.0, 1.0 };
    if (dirAmbientOn) {
        glLightfv(GL_LIGHT0, GL_AMBIENT, dirAmb);
    }
    else {
        glLightfv(GL_LIGHT0, GL_AMBIENT, off);
    }

    if (dirDiffuseOn) {
        glLightfv(GL_LIGHT0, GL_DIFFUSE, dirDif); 
    }
    else {
        glLightfv(GL_LIGHT0, GL_DIFFUSE, off);
    }

    if (dirSpecularOn) {
        glLightfv(GL_LIGHT0, GL_SPECULAR, dirSpec);
    }
    else {
        glLightfv(GL_LIGHT0, GL_SPECULAR, off);
    }
    if (dirOn) {
        glEnable(GL_LIGHT0);
    }
    else {
        glDisable(GL_LIGHT0);
    }
    
    //luz pontual
    pointPos[3] = 1;

    pointAmb[0] = 2.0;//R
    pointAmb[1] = 2.0;//G
    pointAmb[2] = 2.0;//B
    pointAmb[3] = 1.0;//constante

    pointDif[0] = 2.0;//R
    pointDif[1] = 2.0;//G
    pointDif[2] = 2.0;//B
    pointDif[3] = 1.0;//constante

    pointSpec[0] = 2.0;//R
    pointSpec[1] = 2.0;//G
    pointSpec[2] = 2.0;//B
    pointSpec[3] = 1.0;//constante

    glLightfv(GL_LIGHT1, GL_POSITION, pointPos);
    if (pointAmbientOn) {
        glLightfv(GL_LIGHT1, GL_AMBIENT, pointAmb); 
    }
    else {
        glLightfv(GL_LIGHT1, GL_AMBIENT, off);
    }

    
    if (pointDiffuseOn) {
        glLightfv(GL_LIGHT1, GL_DIFFUSE, pointDif); 
    }
    else { 
        glLightfv(GL_LIGHT1, GL_DIFFUSE, off);
    }

    if (pointSpecularOn) {
        glLightfv(GL_LIGHT1, GL_SPECULAR, pointSpec); 
    }
    else {
        glLightfv(GL_LIGHT1, GL_SPECULAR, off);
    }

    //atenuação
    pointAtC = 1;
    pointAtL = 0.0001;
    pointAtQ = 0.00001;
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, pointAtC);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, pointAtL);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, pointAtQ);
    if (pointOn) {
        glEnable(GL_LIGHT1);
    }
    else {
        glDisable(GL_LIGHT1);
    }

    //luz foco left
    //posicao
    spotPos[0] = 0.;
    spotPos[1] = gh() * 0.30;
    spotPos[2] = gh() * 0.55;
    spotPos[3] = 1.;

    //direcao
    spotDir[0] = spotDirVec.x;
    spotDir[1] = spotDirVec.y;
    spotDir[2] = spotDirVec.z;

    //ambiente
    spotAmb[0] = 2.0;//R
    spotAmb[1] = 2.0;//G
    spotAmb[2] = 2.0;//B
    spotAmb[3] = 1.0;//constante

    //difusa
    spotDif[0] = 2.0;//R
    spotDif[1] = 2.0;//G
    spotDif[2] = 2.0;//B
    spotDif[3] = 1.0;//constante

    //specular
    spotSpecular[0] = 2.0;//R
    spotSpecular[1] = 2.0;//G
    spotSpecular[2] = 2.0;//B
    spotSpecular[3] = 2.0;//constante

    if (spotOn) {
        glEnable(GL_LIGHT2);
    }
    else {
        glDisable(GL_LIGHT2);
    }

    glLightfv(GL_LIGHT2, GL_POSITION, spotPos);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotDir);
    if (spotAmbientOn) {
        glLightfv(GL_LIGHT2, GL_AMBIENT, spotAmb);
    }
    else {
        glLightfv(GL_LIGHT2, GL_AMBIENT, off);
    }

    if (spotDiffuseOn) {
        glLightfv(GL_LIGHT2, GL_DIFFUSE, spotDif);
    }
    else {
        glLightfv(GL_LIGHT2, GL_DIFFUSE, off);
    }

    if (spotSpecularOn) {
        glLightfv(GL_LIGHT2, GL_SPECULAR, spotSpecular); 
    }
    else { 
        glLightfv(GL_LIGHT2, GL_SPECULAR, off);
    }

    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, spotExponent);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, spotCutoff);

    spotAtC = 1.;
    spotAtL = 0.;
    spotAtQ = 0.;
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, spotAtC);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, spotAtL);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, spotAtQ);

    //desenharObjetos
    drawObjects();

    glDisable(GL_LIGHTING);
    //cubo fonte de luz pontual
    if (pointOn) {
        glColor3f(1, 1, 1);
        glPushMatrix();
        glTranslatef(pointPos[0], pointPos[1], pointPos[2]);
        glScalef(30, 30, 30);
        cube_unit(0.5);
        glPopMatrix();
    }

    //cubo fonte de luz direcional
    if (dirOn) {
        glColor3f(1, 1, 1);
        glPushMatrix();
        glTranslatef(dirVec3f.x, dirVec3f.y, dirVec3f.z);
        glScalef(30, 30, 30);
        cube_unit(0.5);
        glPopMatrix();
    }

    //cubo fonte de luz foco
    if (spotOn) {
        glColor3f(1, 1, 1);
        glPushMatrix();
        glTranslatef(spotPos[0], spotPos[1], spotPos[2]);
        glScalef(30, 30, 30);
        cube_unit(0.5);
        glPopMatrix();
    }

    if (is3D && !firstPersonView) {
        ofDrawBitmapString("Placar:", -40, 340);
        ofDrawBitmapString("Jogador Esquerda(AI): " + std::to_string(leftPlayerScore), -300, 320);
        ofDrawBitmapString("Jogador Direita: " + std::to_string(rightPlayerScore), 130, 320);
    }
    else if(is3D && firstPersonView){
        ofDrawBitmapString("Esquerda(AI): " + std::to_string(leftPlayerScore), -200, 50, 300);
        ofDrawBitmapString("Direita: " + std::to_string(rightPlayerScore), -200, 50, -250);
    }
    else {
        ofDrawBitmapString("Placar:", -20, 240);
        ofDrawBitmapString("Jogador Esquerda(AI): " + std::to_string(leftPlayerScore), -200, 220);
        ofDrawBitmapString("Jogador Direita: " + std::to_string(rightPlayerScore), 50, 220);
    }
    if (Pausa) {
        ofDrawBitmapString("Jogo Pausado", -40, 0);  
        return;
    }
    if (jogoAcabou) {
        ofSetColor(255, 255, 255);
        ofDrawBitmapString(vencedor, -100, 30);
    }

   
}




//--------------------------------------------------------------
void ofApp::drawObjects() {
    glColor3f(1, 1, 1);
    glPushMatrix();

    glTranslatef(0, 0, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //cubo
    glPushMatrix();
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glScalef(cubeSize, cubeSize, cubeSize);
    setMaterial(0);
    cube_unit_with_texture(img1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPopMatrix();

    //bola
    glPushMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (is3D) {
        glTranslatef(esferaPosX, esferaPosY, esferaPosZ);
    }
    else {
        glTranslatef(esferaPosX, esferaPosY, 0);
    }
    glScalef(10, 10, 10);
    setMaterial(1);
    ball_unit_with_texture(img2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPopMatrix();

    //Jogador Esquerda
    glPushMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (is3D) {
        glTranslatef(-150, leftPlayerPosY, leftPlayerPosZ);
    }
    else {
        glTranslatef(-150, leftPlayerPosY, 0);
    }
    glScalef(20, 40, 40);
    if (goloJEsquerda) {
        setMaterial(0);
    }
    else {
        setMaterial(2);
    }
    player_unit_with_texture(img3);
    glPopMatrix();


    //Jogador Direita
    glPushMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (is3D) {
        glTranslatef(150, rightPlayerPosY, rightPlayerPosZ);
    }
    else {
        glTranslatef(150, rightPlayerPosY, 0);
    }
    glScalef(20, 40, 40);
    if (goloJDireita) {
        setMaterial(0);
    }
    else {
        setMaterial(3);
    }
    player_unit_with_texture(img3);
    glPopMatrix();

    glPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    switch (key) {
    case 'v':
        is3D = !is3D;
        break;
    case 'f':
        firstPersonView = !firstPersonView;
        break;
    case 'r':
        setup();
        break;
    case 'p':
        Pausa = !Pausa;
        break;
    case OF_KEY_UP:
        if (is3D) {
            if (rightPlayerPosY < cubeSize / 2 - playerHeight) rightPlayerPosY += 35.0f;
        }
        else if (rightPlayerPosY < maxMoveYDrt) {
            rightPlayerPosY += 35.0f;
        }
        break;
    case OF_KEY_DOWN:
        if (is3D) {
            if (rightPlayerPosY > -cubeSize / 2 + playerHeight) rightPlayerPosY -= 35.0f;
        }
        else if (rightPlayerPosY > -maxMoveYDrt) {
            rightPlayerPosY -= 35.0f;
        }
        break;
    case OF_KEY_RIGHT:
        if (is3D && !firstPersonView && rightPlayerPosZ < cubeSize / 2 - playerWidth * 2) rightPlayerPosZ += 25.0f;
        else if (is3D && firstPersonView && rightPlayerPosZ > -cubeSize / 2 + playerWidth * 2) rightPlayerPosZ -= 25.0f;
        break;
    case OF_KEY_LEFT:
        if (is3D && !firstPersonView && rightPlayerPosZ > -cubeSize / 2 + playerWidth * 2) rightPlayerPosZ -= 35.0f;
        else if (is3D && firstPersonView && rightPlayerPosZ < cubeSize / 2 - playerWidth * 2) rightPlayerPosZ += 35.0f;
        break;
    case 'a':
        ambientOn = !ambientOn;
        break;
    case 'd':
        dirOn = !dirOn;
        break;
    case 'z':
        pointOn = !pointOn;
        break;
    case 'q':
        autoMove = !autoMove;
        break;
    case 's':
        spotOn = !spotOn;
        break;
    case '1': 
        dirAmbientOn = !dirAmbientOn;
        break;
    case '2':  
        dirDiffuseOn = !dirDiffuseOn;
        break;
    case '3': 
        dirSpecularOn = !dirSpecularOn;
        break;
    case '4': 
        pointAmbientOn = !pointAmbientOn;
        break;
    case '5': 
        pointDiffuseOn = !pointDiffuseOn;
        break;
    case '6':
        pointSpecularOn = !pointSpecularOn;
        break;
    case '7': 
        spotAmbientOn = !spotAmbientOn;
        break;
    case '8': 
        spotDiffuseOn = !spotDiffuseOn;
        break;
    case '9': 
        spotSpecularOn = !spotSpecularOn;
        break;
    }
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}