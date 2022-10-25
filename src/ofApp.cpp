#include "ofApp.h"

void AgentList::add(Agent s) {
	agents.push_back(s);
}

void AgentList::remove(int i) {
	agents.erase(agents.begin() + i);
}

void AgentList::update() {

	if (agents.size() == 0) return;
	vector<Agent>::iterator a = agents.begin();
	vector<Agent>::iterator tmp;

	// check which agents have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (a != agents.end()) {
		if ((a->lifespan != -1 && a->age() > a->lifespan) || a->isCollide) {
			tmp = agents.erase(a);
			a = tmp;
		}
		else a++;
	}
}

void AgentList::draw() {
	for (int i = 0; i < agents.size(); i++) {
		agents[i].draw();
	}
}

void Player::draw() {

	ofSetColor(ofColor::white);
	ofPushMatrix();
	ofMultMatrix(getMatrix());
	image.draw(-image.getWidth() / 2.0, -image.getHeight() / 2.0);
	ofPopMatrix();
}

void AgentEmitter::update(int nAgent, Player *player) {
	if (!started) return;

	float time = ofGetElapsedTimeMillis();

	if ((time - lastSpawned) > (1000.0 / rate)) {

		// call virtual to spawn a new sprite
		//
		for (int i = 0; i < groupSize; i++)
			if (agentSys->agents.size() < nAgent) {
				spawnSprite();
			}

		lastSpawned = time;
	}
	
	for (int i = 0; i < agentSys->agents.size(); i++) {
		agentSys->agents[i].updateAgentDir(player);
		moveSprite(&agentSys->agents[i]);
		agentSys->agents[i].checkCollide(player);
	}

	// update sprite list
//
	if (agentSys->agents.size() == 0) return;
	vector<Agent>::iterator a = agentSys->agents.begin();
	vector<Agent>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (a != agentSys->agents.end()) {
		if ((a->lifespan != -1 && a->age() > a->lifespan) || a->isCollide) {
			tmp = agentSys->agents.erase(a);
			a = tmp;
		}
		else a++;
	}
	
	
}
//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(true);
	player = new Player();
	player->pos = glm::vec3(ofGetWindowWidth() / 2.0 + 300, ofGetWindowHeight() / 2.0, 0);

	if (!background.load("images/background.png")) {
		cout << "Can't load image" << endl;
		ofExit();
	}
	if (!player->image.load("images/dragon.png")) {
		cout << "Can't load image" << endl;
		ofExit();
	}
	if (!agentImage.load("images/Agent.png")) {
		cout << "Can't load image" << endl;
		ofExit();
	}

	player->speed = 700;
	player->scale = 1;

	font.load("arial.ttf", 32);

	gui.setup();
	gui.add(sp.setup("Speed", player->speed, 1, 2000));
	gui.add(sc.setup("Scale", player->scale, 0.5, 10));
	gui.add(numEng.setup("Energy Level", 10, 1, 20));
	gui.add(numAgent.setup("Number of Agent", 5 , 1, 20));
	gui.add(rate.setup("rate", 1, 1, 10));
	gui.add(groupNum.setup("Agent spawn group size", 1, 1, 5));
	gui.add(life.setup("Agent life time", 5, .1, 20));
	gui.add(shImage.setup("Show Player and Agent Sprite", false));
	bHide = false;

	player->nEnergy = numEng;

	emitter = new AgentEmitter();

	emitter->pos = glm::vec3(ofRandom(0, ofGetWindowWidth()), ofRandom(0, ofGetWindowHeight()), 0);
	emitter->drawable = false;
	emitter->setChildImage(agentImage, shImage);
	emitter->setGroupSize(groupNum);
	//emitter->start();
}

//--------------------------------------------------------------
void ofApp::update() {
	player->speed = sp;
	player->scale = sc;
	player->showImage = shImage;
	emitter->setRate(rate);
	emitter->setLifespan(life * 1000);    // convert to milliseconds 
	emitter->setChildImage(agentImage, shImage);
	emitter->setGroupSize(groupNum);

	if (startScreen) {
		player->nEnergy = numEng;
		elapedTime = 0;
	}
	if (gameScreen) {
		emitter->pos = glm::vec3(ofRandom(0, ofGetWindowWidth()), ofRandom(0, ofGetWindowHeight()), 0);
		emitter->update(numAgent, player);
		elapedTime = ofGetElapsedTimef() - gameStartTime;

		//check for arrow keys press
		if (keymap[OF_KEY_UP]) {
			if (player->pos.x >= ofGetWindowWidth())
				player->pos.x -= 1;
			else if (player->pos.x <= 0)
				player->pos.x += 1;
			else if (player->pos.y >= ofGetWindowHeight())
				player->pos.y -= 1;
			else if (player->pos.y <= 0)
				player->pos.y += 1;
			else
				player->pos += player->header() * (player->speed / ofGetFrameRate());
		}

		if (keymap[OF_KEY_DOWN]) {
			if (player->pos.x >= ofGetWindowWidth())
				player->pos.x -= 1;
			else if (player->pos.x <= 0)
				player->pos.x += 1;
			else if (player->pos.y >= ofGetWindowHeight())
				player->pos.y -= 1;
			else if (player->pos.y <= 0)
				player->pos.y += 1;
			else
				player->pos -= player->header() * (player->speed / ofGetFrameRate());
		}

		if (keymap[OF_KEY_LEFT])
			player->rotation -= rotDeg;

		if (keymap[OF_KEY_RIGHT])
			player->rotation += rotDeg;
	}
	if (player->nEnergy < 1) {
		overScreen = true;
		gameScreen = false;
		emitter->agentSys->agents.clear();
		emitter->stop();
		player->pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0);
		player->rotation = 0;
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(ofColor::white);
	background.resize(ofGetWindowWidth(), ofGetWindowHeight());
	background.draw(0,0);

	if (startScreen) {
		ofSetColor(ofColor::white);
		font.drawString(startText, ofGetWindowWidth() / 2 - font.stringWidth(startText) / 2,
			ofGetWindowHeight() / 2 - font.stringHeight(startText) / 2);
	}
	else if (gameScreen) {
		if (player->showImage)
			player->draw();
		else
		{
			ofSetColor(ofColor::red);
			player->TriangleShape::draw();
		}
		emitter->draw();
	}
	else if (overScreen) {
		ofSetColor(ofColor::white);
		font.drawString("You dead... Game Time: " + std::to_string(elapedTime) + " seconds\nPress 'b' to return to start screen.",
			ofGetWindowWidth() / 2 - font.stringWidth(startText) / 2 - 150,
			ofGetWindowHeight() / 2 - font.stringHeight(startText) / 2);
	}

	string str;
	str += "Player Energy: " + std::to_string(player->nEnergy) + "/" + std::to_string(numEng) +
		"   Elapsed Time: " + std::to_string(elapedTime) +
		"   Frame Rate: " + std::to_string(ofGetFrameRate());
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str, ofGetWindowWidth() - 550, 15);

	if (!bHide) gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	keymap[key] = true;

	if(keymap['h'])
		bHide = !bHide;

	if (keymap['b']) {
		if (overScreen) {
			overScreen = false;
			startScreen = true;
		}
	}	
	if (keymap[' ']) {
		if (startScreen) {
			emitter->start();
			gameStartTime = ofGetElapsedTimef();
			startScreen = false;
			gameScreen = true;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	keymap[key] = false;
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
