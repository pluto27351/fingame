#ifndef __LEVEL2_SCENE_H__
#define __LEVEL2_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "ui/UIWidget.h"
#include "Box2D/Box2D.h"
#include "Common/CButton.h"
#include "Common/CSwitchButton.h"
#include "Common/CContactListener.h"
#include "SimpleAudioEngine.h"

#ifdef BOX2D_DEBUG
#include "Common/GLES-Render.h"
#include "Common/GB2ShapeCache-x.h"
#endif

#define PTM_RATIO 32.0f
#define RepeatCreateBallTime 3
#define AccelerateMaxNum 2
#define AccelerateRatio 1.5f

using namespace CocosDenshion;
class Level2 : public cocos2d::Layer
{
public:

	~Level2();
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene(int [4][3],int);
	Node *_csbRoot;

	// for Box2D
	b2World* _b2World;
	cocos2d::Size _visibleSize;

	//球數量
	cocos2d::ui::Text *_redNum, *_greenNum, *_yellowNum;
	int nr, ng, ny;
	int maxLevel;
	int levelball[4][3];

	// for MouseJoint
	b2Body *_bottomBody; // 底部的 edgeShape
	b2MouseJoint* _MouseJoint;
	bool _bTouchOn;   //與場景物件產生關西
	bool _bMouseOn = false;  //滑鼠移動

	//過關
	bool _bboxR = false, _bboxG = false, _bboxB = false;

	//分子特效
	cocos2d::BlendFunc blendFunc;
	float _tdelayTime; 
	bool  _bSparking;  
	float _balltime = 0.0f;

	//出生
	Point bornpt;
	Sprite *_born;

	bool startGame = true;
	CContactListener _colliderSeneor;

	//按鈕類
	CButton *_redBtn, *_blueBtn, *_greenBtn, *_homeBtn, *_replayBtn;
	CButton *_homeBtn2, *_replayBtn2, *_nextBtn;
	CSwitchButton *_penBtn;

	b2Body *_star;
	bool _bstar = false;

	cocos2d::ui::Text *_score[3];
	Node *_endUi;

	void addBall(int [4][3]);
	void createStaticBoundary();
	void setStaticWall();
	void setPendulum();
	void setFinalBox();
	void setStar();
	void setbtn();
	void setUIbtn();
	void setEndUi();
	void renderball(char *, int);


#ifdef BOX2D_DEBUG
	//DebugDraw
	GLESDebugDraw* _DebugDraw;
	virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags);
#endif

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
	void doStep(float dt);

	cocos2d::EventListenerTouchOneByOne *_listener1;
	bool onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent); //觸碰開始事件
	void onTouchMoved(cocos2d::Touch *pTouch, cocos2d::Event *pEvent); //觸碰移動事件
	void onTouchEnded(cocos2d::Touch *pTouch, cocos2d::Event *pEvent); //觸碰結束事件 

	
    // implement the "static create()" method manually
    CREATE_FUNC(Level2);
};

#endif // __StaticDynamic_SCENE_H__
