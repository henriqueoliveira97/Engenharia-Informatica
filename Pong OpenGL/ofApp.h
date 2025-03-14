#include "ofMain.h"
#include "cg_extras.h"
#include "cg_cam_extras.h"
#include "cg_drawing.h"
#include "materiais.h"

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override; 

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;


		void drawObjects();
		void resetBall();
		void voltarAoCentro();

		int view;
		GLfloat lensAngle;
		GLfloat alpha, beta;

		GLfloat cameraX;
		GLfloat cameraY;
		GLfloat cameraZ;
		GLfloat lookAtX;
		GLfloat lookAtY;
		GLfloat lookAtZ;
		GLfloat cameraDistance;

		string vencedor;
		bool is3D;
		bool firstPersonView;

		GLfloat maxMoveYDrt;
		GLfloat maxMoveYEsq;
		GLfloat rightPlayerPosZ;
		GLfloat leftPlayerPosZ;
		GLfloat leftPlayerPosY;
		GLfloat rightPlayerPosY;
		GLfloat leftPlayerPosX;
		GLfloat rightPlayerPosX;
		GLfloat leftPlayerSpeed;
		GLfloat playerWidth;
		GLfloat playerHeight;
		int directionX;
		GLfloat fixedSpeed;

		int leftPlayerScore;
		int rightPlayerScore;
		GLfloat tempoDeJogo;
		bool jogoAcabou;
		bool aguardaReset;
		bool Pausa;

		bool piscaDireita ; 
		bool piscaEsquerda;
		GLfloat piscaTRestante;
		GLfloat tempoDecremento;
		bool goloJEsquerda;
		bool goloJDireita;

		

		GLfloat esferaPosX, esferaPosY, esferaPosZ;    
		GLfloat esferaSpeedX, esferaSpeedY, esferaSpeedZ;
		GLfloat esferaRadius;             
		GLfloat cubeSize;              
		GLfloat bounciness;  
		GLfloat fixSpeed;


		//iluminação
		bool ambientOn;
		bool dirOn;
		bool pointOn;
		bool dirAmbientOn;
		bool dirDiffuseOn;
		bool dirSpecularOn;
		bool pointAmbientOn;
		bool pointDiffuseOn;
		bool pointSpecularOn;
		bool spotAmbientOn;
		bool spotDiffuseOn;
		bool spotSpecularOn;
		bool autoMove;
		bool localViewer;

		GLfloat ambientLight[4];

		GLfloat dirVec[4];
		GLfloat dirAmb[4];
		GLfloat dirDif[4];
		GLfloat dirSpec[4];
		ofVec3f dirVec3f;
		GLfloat dirVecTheta;

		GLfloat pointPos[4];
		GLfloat pointAmb[4];
		GLfloat pointDif[4];
		GLfloat pointSpec[4];
		GLfloat pointZtheta;
		GLfloat pointAtC, pointAtL, pointAtQ;
		int pointDirection;

		GLfloat spotPos[4];
		GLfloat spotDir[3];
		ofVec3f spotDirVec;
		ofVec3f spotDirAux;
		ofVec3f spotPosVec;
		GLfloat spotAmb[4];
		GLfloat spotDif[4];
		GLfloat spotSpecular[4];
		GLfloat spotExponent;
		GLfloat spotCutoff;
		GLfloat spotTheta;
		GLfloat spotAtC, spotAtL, spotAtQ;
		bool spotOn;

		

		ofImage img1;
		ofImage img2;
		ofImage img3;
		ofImage img4;
};
