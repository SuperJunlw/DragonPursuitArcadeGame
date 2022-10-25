#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Emitter.h"
#include "Shape.h"

//Player class
class Player : public TriangleShape {
public:

	Player() {}

	void draw();

	//header function
	glm::vec3 header()
	{
		glm::vec3 d = glm::vec3(-cos(ofDegToRad(rotation + 90)), -sin(ofDegToRad(rotation + 90)), 0);
		return glm::normalize(d);
	}

	float getRadius() {
		if (showImage)
			return (image.getWidth() / 2) * scale;
		else
			return 20 * scale;
	}

	ofImage image;
	bool showImage = false;
	float speed = 0;
	int nEnergy = 0;
};

class Agent : public Sprite {
public:
	Agent() : Sprite() {}
	
	void updateAgentDir(Player *player) {
		glm::vec3 diff = glm::normalize(player->pos - pos);
		float deg = glm::orientedAngle(glm::vec2(header()), glm::vec2(diff));
		rotation += deg;
	}

	glm::vec3 header() {
		glm::vec3 d = glm::vec3(-cos(ofDegToRad(rotation + 90)), -sin(ofDegToRad(rotation + 90)), 0);
		return glm::normalize(d);
	}

	float getRadius() {
		if(bShowImage)
			return (spriteImage.getWidth() / 2) * scale;
		else
			return 20 * scale;
	}

	void checkCollide(Player *p) {
		float d = glm::distance(p->pos, pos);
		if (d < (p->getRadius() + getRadius())) {
			isCollide = true;
			p->nEnergy--;
		}
	}

	bool isCollide = false;
};

class AgentList {
public:
	void add(Agent);
	void remove(int);
	void update();
	void draw();
	vector<Agent> agents;
};


class AgentEmitter : public Emitter {
public:
	AgentEmitter() : Emitter() {
		agentSys = new AgentList;
	}
	void moveSprite(Agent* agent) {
		agent->pos += agent->header()* (120 / ofGetFrameRate());
	}

	void spawnSprite() {
		Agent agent;
		if (haveChildImage) agent.setImage(childImage);
		agent.lifespan = lifespan;
		agent.pos = pos;
		agent.rotation = ofRandom(0, 360);
		agent.birthtime = ofGetElapsedTimeMillis();
		agentSys->add(agent);
	}

	void draw() {
		Emitter::draw();
		agentSys->draw();
	}

	void setChildImage(ofImage img, bool showImage) {
		childImage = img;
		if (showImage)
			haveChildImage = true;
		else
			haveChildImage = false;
	}

	void setGroupSize(int s) {
		groupSize = s;
	}
	void update(int nAgent, Player *player);

	int groupSize;

	AgentList* agentSys;
};



class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	Player *player = NULL;
	AgentEmitter *emitter = NULL;

	float rotDeg = 6.0;
	map<int, bool> keymap;

	bool bHide;
	ofxFloatSlider sp;
	ofxFloatSlider sc;
	ofxFloatSlider rate;
	ofxFloatSlider life;
	ofxVec3Slider velocity;
	ofxIntSlider numAgent;
	ofxIntSlider numEng;
	ofxIntSlider groupNum;
	ofxToggle shImage;

	ofxPanel gui;
	ofImage background;
	ofImage agentImage;

	ofTrueTypeFont font;
	string startText = "Press 'space' to start the game!";
	string overText = "You dead... Press 'b' to return to start screen.";

	bool startScreen = true;
	bool overScreen = false;
	bool gameScreen = false;
	float elapedTime = 0;
	float gameStartTime = 0;
};

