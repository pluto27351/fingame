#pragma once
#include "cocos2d.h"
#include "Box2D/Box2D.h"
#define PTM_RATIO 32.0f

class CContactListener : public b2ContactListener
{
public:
	cocos2d::Sprite *_PendulumSprite; // �Ω�P�_�O�_
	cocos2d::Sprite *_StarSprite; // �Ω�P�_�O�_
	bool star = false;
	bool _bCreateSpark;		//���ͤ���
	bool _bApplyImpulse;	// �����������ĤO
	b2Vec2 _createLoc;
	int  _NumOfSparks;
	bool inBoxR = false, inBoxG = false, inBoxB = false;
	bool lv3Open = false;
	bool _bp = false,_bs=false;
	CContactListener();
	~CContactListener();
	//�I���}�l
	virtual void BeginContact(b2Contact* contact);
	//�I������
	virtual void EndContact(b2Contact* contact);
	void setCollisionTarget(cocos2d::Sprite &targetSprite);
	void setCollisionTarget2(cocos2d::Sprite &targetSprite);
	//void setballsensor();
	//CREATE_FUNC(CContactListener);
};