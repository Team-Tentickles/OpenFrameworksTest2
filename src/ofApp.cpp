#include "ofApp.h"



void ofApp::setup(){
    
    ofSetFrameRate(63);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofBackground(22);
    ofSetWindowPosition(20, 20);
    
    ofxSimpleHttp::createSslContext();
    
    downloadList.push_back("https://placeholdit.imgix.net/~text?txtsize=33&txt=500%C3%97500&w=500&h=500");
    
    http.addCustomHttpHeader("Accept", "application/json"); //you can supply custom headers if you need to
    
    http.setCopyBufferSize(16);
    http.setSpeedLimit(1000);
    
    mainOutputSyphonServer.setName("Screen Output");
    
    //add download listener
    ofAddListener(http.httpResponse, this, &ofApp::newResponse);
}


void ofApp::update(){
    
    http.update();
}

void ofApp::draw(){
    http.draw(30,30);
    ofSetColor(255);
    
    //clock hand to see threading in action
    ofPushMatrix();
    ofTranslate(ofGetWidth() - 60,60, 0);
    ofRotate( ofGetFrameNum() * 3, 0,0,1);
    ofSetColor(255,255,255);
    float h = 5;
    ofRect(-h/2,h/2, h,50);
    ofPopMatrix();
    
    //instructions
    ofDrawBitmapStringHighlight("press '1' to download asset to disk on a bg thread\n"
                                "press '2' to download asset to disk blocking (main thread)\n"
                                "press '3' to download asset to RAM on bg thread\n"
                                "press '4' to download asset to RAM blocking (main thread)\n"
                                "press '5' to download multiple assets to Disk on main thread\n"
                                "press '6' call an https based API, print result on console\n"
                                "press '7' to download (copy) a local file (file:// url)\n"
                                "press 'c' to cancel current download\n"
                                "press 'C' to cancel current download and empty queue",
                                20,
                                ofGetHeight() - 133
                                );
    
    if (img.isAllocated()) {
        cout << "drawing" << endl;
        img.draw(0, 0);
    }
    
    mainOutputSyphonServer.publishScreen();
    
}


void ofApp::keyPressed(int key){
    
    
    if(key=='1'){
        string url = downloadList[floor(ofRandom(downloadList.size()))];
        http.fetchURLToDisk(url , true/*notify when done*/, OUTPUT_DIRECTORY);
    }
    
    if(key=='2'){
        string url = downloadList[floor(ofRandom(downloadList.size()))];
        http.fetchURLtoDiskBlocking(url, OUTPUT_DIRECTORY);
    }
    
    if(key=='3'){
        string url = downloadList[floor(ofRandom(downloadList.size()))];
        http.fetchURL(url , true/*notify when done*/);
    }
    
    if(key=='4'){
        string url = downloadList[floor(ofRandom(downloadList.size()))];
        http.fetchURLBlocking(url);
    }
    
    if(key=='5'){
        for(int i=0 ; i < downloadList.size(); i++){
            //by quickly adding more urls to download, we create a download queue
            http.fetchURLToDisk(downloadList[i], true/*notify when done*/, OUTPUT_DIRECTORY);
        }
    }
    
    if(key=='6'){
        http.setUserAgent("My Fake Browser"); //github api requires User Agent string
        ofxSimpleHttpResponse res = http.fetchURLBlocking("https://placeholdit.imgix.net/~text?txtsize=33&txt=500%C3%97500&w=500&h=500");
        cout << res.responseBody << endl;
    }
    
    
    if(key=='7'){ //also supports file:// urls for local filesystem
        http.fetchURLToDisk("file:///System/Library/Fonts/Optima.ttc", true, OUTPUT_DIRECTORY );
    }
    
    if(key=='c'){
        http.stopCurrentDownload(false);
    }
    
    if(key=='C'){
        http.stopCurrentDownload(true);
    }
    
}


void ofApp::newResponse(ofxSimpleHttpResponse &r){
    
    cout << "#########################################################" << endl;
    cout << "download of " << r.url << " returned : "<< string(r.ok ? "OK" : "KO") << endl;
    cout << "server reported size is " << r.serverReportedSize << endl;
    cout << "server status is " << r.status << endl;
    cout << "file content type is " << r.contentType << endl;
    cout << "file name is " << r.fileName << endl;
    
    if(r.downloadToDisk){
        cout << "file was saved to " << r.absolutePath << endl;
        img.load(r.absolutePath);
        
        //move the file to wherever you need..
        ofDirectory dir;
        dir.createDirectory("mySortedDownloads");
        
        ofDirectory f;
        f.open( r.absolutePath );
        if( f.exists() ){
            f.moveTo("mySortedDownloads/" + r.fileName);
            cout << r.fileName << endl;
            img.load(r.fileName);
        }else{
            cout << "file was not downloaded???!" << r.absolutePath << endl;
        }
    }
}

void ofApp::exit(){
    ofxSimpleHttp::destroySslContext();
}