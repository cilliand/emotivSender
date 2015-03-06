#include "ofApp.h"

EmoEngineEventHandle eEvent			= EE_EmoEngineEventCreate();
EmoStateHandle eState				= EE_EmoStateCreate();
unsigned int userID					= 0;
const unsigned short composerPort	= 1726;
float secs							= 1;
unsigned int datarate				= 0;
bool readytocollect					= false;
int option							= 0;
int state							= 0;
int stimIndex                       = 0;
EE_DataChannel_t targetChannelList[] = {
		ED_O1
	};

const char header[] = "O1,";
std::ofstream ofs("data/data.csv",std::ios::trunc);
ofImage upArrow, leftArrow, rightArrow;
bool resultReceived = false;
int result = -1;
clock_t t1, t2;
int r = 0;
//--------------------------------------------------------------
void ofApp::setup(){
    t1 = clock();
    upArrow.loadImage("images/up.png");
    leftArrow.loadImage("images/left.png");
    rightArrow.loadImage("images/right.png");

	ofBackground(255);
    if (EE_EngineConnect() != EDK_OK) {
					throw "Emotiv Engine start up failed.";
    }
	// open an outgoing connection to HOST:PORT
	sender.setup(HOST, PORT);
    receiver.setup(7402);
    hData = EE_DataCreate();
    EE_DataSetBufferSizeInSec(secs);
    ofxOscMessage m;
    m.setAddress("/start");
    sender.sendMessage(m);
}

//--------------------------------------------------------------
void ofApp::update(){
    t2=clock();
    if((t2-t1)/CLOCKS_PER_SEC >= 2){
        ofxOscMessage m;
        m.setAddress("/end");
        sender.sendMessage(m);
        ofExit(0);
    } else {
    state = EE_EngineGetNextEvent(eEvent);
    if (state == EDK_OK) {

				EE_Event_t eventType = EE_EmoEngineEventGetType(eEvent);
				EE_EmoEngineEventGetUserId(eEvent, &userID);

				// Log the EmoState if it has been updated
				if (eventType == EE_UserAdded) {
					std::cout << "User added";
					EE_DataAcquisitionEnable(userID,true);
					readytocollect = true;
				}
			}

			if (readytocollect) {
                float * buffer = new float[128];
                for(int i = 0; i < 128; i++){
                    buffer[i] = ofRandom(4800.0,4900.0);
                }
                for(int i = 0; i < 128; i++){
                    ofxOscMessage m;
                    m.setAddress("/d");
                    std::cout << buffer[i] << std::endl;
                    m.addFloatArg(buffer[i]);
                    sender.sendMessage(m);
                }

//                    EE_DataUpdateHandle(0, hData);
//
//					unsigned int nSamplesTaken=0;
//                    EE_DataGetNumberOfSample(hData,&nSamplesTaken);
//
//                    //std::cout << "Updated " << nSamplesTaken << std::endl;
//
//
//						if (nSamplesTaken != 0) {
//						unsigned int channelCount = sizeof(targetChannelList)/sizeof(EE_DataChannel_t);
//						double ** buffer = new double*[channelCount];
//                        for (int i=0; i<channelCount; i++)
//                            buffer[i] = new double[nSamplesTaken];
//
//                        EE_DataGetMultiChannels(hData, targetChannelList, channelCount, buffer, nSamplesTaken);
//                        for (int sampleIdx=0 ; sampleIdx<(int)nSamplesTaken ; ++ sampleIdx) {
//                            for (int i = 0 ; i<sizeof(targetChannelList)/sizeof(EE_DataChannel_t) ; i++) {
//
//                                std::cout << buffer[i][sampleIdx] << std::endl;
//                                ofxOscMessage m;
//                                m.setAddress("/d");
//                                m.addFloatArg(buffer[i][sampleIdx]);
//                                sender.sendMessage(m);
//                            }
//                        }
//                        for (int i=0; i<channelCount; i++)
//								delete buffer[i];
//							delete buffer;
//
//						}

			}
			Sleep(100);
        if(receiver.hasWaitingMessages()){
            ofxOscMessage msg;
            while(receiver.getNextMessage(&msg)) {
                 if (msg.getAddress() == "/r"){
                    resultReceived = true;
                    result = msg.getArgAsInt32(0);
                 }
            }
        }
        }
}


//--------------------------------------------------------------
void ofApp::draw(){
    if(r >= 3){
        r = 0;
    }
	// display instructions
	string buf;
	buf = "sending osc messages to " + string(HOST) + " " + ofToString(PORT);
	ofDrawBitmapString(buf, 10, 20);
    if(resultReceived == false){

        if(r == 0){
            upArrow.draw((ofGetWidth()/2)-32,0, 64, 64);
            ofSleepMillis(150);
            ofxOscMessage m;
            m.setAddress("/st");
            m.addIntArg(0);
            sender.sendMessage(m);
        } else if(r == 1){
            leftArrow.draw(0,ofGetHeight()/2, 64, 64);
            ofSleepMillis(150);
            ofxOscMessage m;
            m.setAddress("/st");
            m.addIntArg(1);
            sender.sendMessage(m);
        } else if(r == 2){
            rightArrow.draw(ofGetWidth()-64,ofGetHeight()/2, 64, 64);
            ofSleepMillis(150);
            ofxOscMessage m;
            m.setAddress("/st");
            m.addIntArg(2);
            sender.sendMessage(m);
        }
    } else {
        if(result == 0){
            upArrow.draw((ofGetWidth()/2)-32,0, 64, 64);
        } else if(result == 1){
            leftArrow.draw(0,ofGetHeight()/2, 64, 64);
        } else if(result == 2){
            rightArrow.draw(ofGetWidth()-64,ofGetHeight()/2, 64, 64);
        }
    }
    r++;
  //  ofSleepMillis(800);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'N'){
        ofxOscMessage m;
        m.setAddress("/end");
        sender.sendMessage(m);
    }
}

void ofApp::exit(){
    EE_DataFree(hData);
    EE_EngineDisconnect();
    EE_EmoStateFree(eState);
    EE_EmoEngineEventFree(eEvent);
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

