/*
 *  ofxBox2dBaseShape.cpp
 *  ofxBox2d
 *
 *  Created by Nick Hardeman on 1/14/11.
 *  Copyright 2011 Arnold Worldwide. All rights reserved.
 *
 */

#include "ofxBox2dBaseShape.h"

//----------------------------------------
ofxBox2dBaseShape::ofxBox2dBaseShape() {
	
	setMassFromShape = true;
	alive = false;
	body  = NULL;
	
	density     = 0.0;
	bounce		= 0.0;
	friction	= 0.0;
	//bodyDef.allowSleep = true; // nm: already set in constructor
	bodyTypeSet = false;
}

//----------------------------------------
ofxBox2dBaseShape::~ofxBox2dBaseShape() {
    ofLog(OF_LOG_VERBOSE, "~ofxBox2dBaseShape(%p)\n", body);
    destroy();
}

//------------------------------------------------
void ofxBox2dBaseShape::destroy() {
	
	if(getWorld() == NULL) {
		ofLog(OF_LOG_VERBOSE, "ofxBox2dBaseShape:: - must have a valid world -");
		return;
	}
	else if(body == NULL) {
		ofLog(OF_LOG_VERBOSE, "ofxBox2dBaseShape:: - null body -");
		return;
	}
    
	getWorld()->DestroyBody(body);
	body  = NULL;
	alive = false;
}

//----------------------------------------
bool ofxBox2dBaseShape::shouldRemove(ofPtr<ofxBox2dBaseShape> shape) {
    return !shape.get()->alive;
}
bool ofxBox2dBaseShape::shouldRemoveOffScreen(ofPtr<ofxBox2dBaseShape> shape) {
    return !ofRectangle(0, 0, ofGetWidth(), ofGetHeight()).inside(shape.get()->getPosition());
}

//----------------------------------------
bool ofxBox2dBaseShape::isBody() {
	if (body == NULL) {
		//cout << __FILE__ << __func__ << endl;
		ofLog(OF_LOG_VERBOSE, "ofxBox2dBaseShape:: - body is not defined -");
		return false;
	}
	return true;
}

bool ofxBox2dBaseShape::isFixed() {
	return density == 0.f ? true : false;
}

bool ofxBox2dBaseShape::isSleeping() {
    if(isBody()) {
        return !body->IsAwake();
    }
    else { 
        ofLog(OF_LOG_VERBOSE, "ofxBox2dBaseShape:: - body is not defined -");
        return false;
    }
}
//
b2World* ofxBox2dBaseShape::getWorld() {
	if (isBody()) {
		return body->GetWorld();
	}
	return NULL;
}

b2Body* ofxBox2dBaseShape::getBody() {
    return body;
}

//----------------------------------------
void ofxBox2dBaseShape::create() {}

//------------------------------------------------ 
void ofxBox2dBaseShape::setBounce(float val) {
	bounce = val;
}

//------------------------------------------------ 
void ofxBox2dBaseShape::setDensity(float val) {
	// -- this is not working ! --
	/*b2MassData data;
	 data.mass = 3;
	 data.center = body->GetPosition();
	 data.I = body->GetInertia();
	 body->SetMass(&data);
	 */
}

//----------------------------------------
void ofxBox2dBaseShape::setFriction(float val) {
	friction	= val;
}

//------------------------------------------------ 
void ofxBox2dBaseShape::setPhysics(float density, float bounce, float friction) {
	this->density = density; this->bounce = bounce; this->friction = friction;
}

//------------------------------------------------
void ofxBox2dBaseShape::setBodyType(b2BodyType bodyType) {
    bodyDef.type = bodyType;
    bodyTypeSet = true;
}

//------------------------------------------------
void* ofxBox2dBaseShape::setData(void*data) {
	
	if(data == NULL) {
		ofLog(OF_LOG_VERBOSE, "ofxBox2dBaseShape:: - data is NULL -");
		return NULL;
	}
	
	if(isBody()) {
		ofLog(OF_LOG_VERBOSE, "ofxBox2dBaseShape:: - custom data set %p", data);
		body->SetUserData(data);
		return data;
	}
	else {
		ofLog(OF_LOG_VERBOSE, "ofxBox2dBaseShape:: - must have a valid body -");
        return NULL;
	}
}

//------------------------------------------------ 
void* ofxBox2dBaseShape::getData() {
	if(body) {
		return body->GetUserData();
	}
	else {
		ofLog(OF_LOG_VERBOSE, "ofxBox2dBaseShape:: - must have a valid body -");
		return NULL;
	}
}

//------------------------------------------------
void ofxBox2dBaseShape::setFilterData(b2Filter filter) {
    for( b2Fixture * f = body->GetFixtureList(); f; f = f->GetNext() ){
        f->SetFilterData(filter);
    }
}

//------------------------------------------------ 
void ofxBox2dBaseShape::enableGravity(bool b) {
	//bodyDef.isGravitated = b;
}

//------------------------------------------------ 
void ofxBox2dBaseShape::setFixedRotation(bool b) {
	bodyDef.fixedRotation = b;
}

//------------------------------------------------ 
void ofxBox2dBaseShape::setRotationFriction(float f) {
	bodyDef.angularDamping = f;
}

//------------------------------------------------
float ofxBox2dBaseShape::getRotation() {
	if(body != NULL) {
		return ofRadToDeg(body->GetAngle());
	}
    else return 0;
}

void ofxBox2dBaseShape::setRotation(float angle){
    body->SetTransform(body->GetWorldCenter(), DEG_TO_RAD * angle);
}


//------------------------------------------------ 
void ofxBox2dBaseShape::setPosition(float x, float y) {
	if(!body || body == NULL) {
		ofLog(OF_LOG_VERBOSE, "ofxBox2dBaseShape:: - Body is NULL -");
		return;
	}
	body->SetTransform(b2Vec2(b2dNum(x), b2dNum(y)), 0);
	body->SetLinearVelocity(b2Vec2(0, 0)); // maybe bring this back...
	body->SetAwake(true); // this sounds backwards but that is what the doc says todo...
}

void ofxBox2dBaseShape::setPosition(glm::vec2 p) {
	setPosition(p.x, p.y);
}

//------------------------------------------------ 
glm::vec2 ofxBox2dBaseShape::getPosition() {
	glm::vec2 p;
	if(body != NULL) {
        const b2Transform& xf = body->GetTransform();
        b2Vec2 pos      = body->GetLocalCenter();
        b2Vec2 b2Center = b2Mul(xf, pos);
		p = worldPtToscreenPt(b2Center);
    }
	return p;
}

//------------------------------------------------
glm::vec2 ofxBox2dBaseShape::getB2DPosition() {
	return getPosition() / OFX_BOX2D_SCALE;
}

//------------------------------------------------ 
void ofxBox2dBaseShape::setVelocity(float x, float y) {
	if(body != NULL) {
		body->SetLinearVelocity(b2Vec2(x, y));
	}
}
void ofxBox2dBaseShape::setVelocity(glm::vec2 p) {
	setVelocity(p.x, p.y);
}
glm::vec2 ofxBox2dBaseShape::getVelocity() {
	return glm::vec2(body->GetLinearVelocity().x, body->GetLinearVelocity().y);
}

//------------------------------------------------ 
void ofxBox2dBaseShape::setDamping(float fx, float fy) {
	if(body != NULL) {
		b2Vec2 v = body->GetLinearVelocity();
		v.x *= fx;	v.y *= fy;
		body->SetLinearVelocity(v);
	}
}
void ofxBox2dBaseShape::setDamping(float f) {
	setDamping(f, f);
}


//------------------------------------------------
void ofxBox2dBaseShape::addForce(glm::vec2 frc, float scale) {
	if(body != NULL) {
		frc *= scale;
		body->ApplyForce(b2Vec2(frc.x, frc.y), body->GetPosition(), true);
	}
}

//------------------------------------------------
void ofxBox2dBaseShape::addImpulseForce(glm::vec2 pt, glm::vec2 amt) {
	if(body != NULL) {
		body->ApplyLinearImpulse(b2Vec2(pt.x/OFX_BOX2D_SCALE, pt.y/OFX_BOX2D_SCALE), b2Vec2(amt.x, amt.y), true);
	}
}

//------------------------------------------------
void ofxBox2dBaseShape::addRepulsionForce(glm::vec2 pt, float radius, float amt) {
	/*if(body != NULL) {
	 b2Vec2 P(pt.x/OFX_BOX2D_SCALE, pt.y/OFX_BOX2D_SCALE);
	 b2Vec2 D = P - body->GetPosition(); 
	 if(D.LengthSquared() < minDis) {;
	 P.Normalize();
	 b2Vec2 F = amt * D;
	 body->ApplyForce(F, P);
	 }
	 }*/
	
	
	if(body != NULL) {
		b2Vec2 P(pt.x/OFX_BOX2D_SCALE, pt.y/OFX_BOX2D_SCALE);
		b2Vec2 D = P - body->GetPosition(); 
		if(D.LengthSquared() < radius) {;
			P.Normalize();
			b2Vec2 F = amt * D;
			body->ApplyForce(-F, body->GetWorldCenter(), true);
		}
	}
}



//------------------------------------------------
void ofxBox2dBaseShape::update() { }
void ofxBox2dBaseShape::draw() { }
