#ifndef __LEVEL4_SCENE_H__
#define __LEVEL4_SCENE_H__


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
#define DRAW_MIN 5
#define DRAW_HEIGHT 3

using namespace CocosDenshion;

struct DrawPoint {
	cocos2d::Point pt;
	Sprite *texture;
	float r;
	struct DrawPoint *next;
};

class Level4 : public cocos2d::Layer
{
public:

	~Level4();
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene(int [4][3],int);
	Node *_csbRoot;

	// for Box2D
	b2World* _b2World;
	cocos2d::Size _visibleSize;

	//num
	cocos2d::ui::Text *_redNum, *_greenNum, *_yellowNum;
	int nr, ng, ny;
	int maxLevel;
	int levelball[4][3];

	// for MouseJoint
	b2Body *_bottomBody; // ������ edgeShape
	b2MouseJoint* _MouseJoint;
	bool _bTouchOn;   //�P�������󲣥�����
	bool _bMouseOn = false;  //�ƹ�����

	//�L��
	bool _bboxR = false, _bboxG = false, _bboxB = false;
	
	//�X��
	Point bornpt;
	Sprite *_born;

	b2Body *_star;
	bool _bstar = false;

	//���l�S��
	cocos2d::BlendFunc blendFunc;
	float _tdelayTime; // �Ω���᪺���͡A���n�ƥ�i�J�Ӧh�ӾɭP�@�U���͹L�h������
	bool  _bSparking;  // true: �i�H�Q�X����Afalse: ����
	float _balltime = 0.0f;

	bool startGame = true;
	CContactListener _colliderSeneor;
	//��ø
	int pencolor = 0;
	bool drawOn = false;
	void drawLine();
	struct DrawPoint *_HDrawPt = NULL, *_NDrawPt = NULL;
	bool _bDraw = false;

	void addBall(int [4][3]);
	void createStaticBoundary();
	void setStaticWall();
	void setBoards();
	void setPendulum();
	void setFinalBox();
	void setCar();
	void setRope();
	void setStar();
	void setbtn();
	void setUIbtn();
	void setEndUi();

	void renderball(char *, int);
	CButton *_redBtn, *_blueBtn, *_greenBtn, *_homeBtn, *_replayBtn;
	CButton *_homeBtn2, *_replayBtn2, *_nextBtn;
	CSwitchButton *_penBtn;

	cocos2d::ui::Text *_score[3];
	Node *_endUi;

#ifdef BOX2D_DEBUG
	//DebugDraw
	GLESDebugDraw* _DebugDraw;
	virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags);
#endif

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
	void doStep(float dt);

	cocos2d::EventListenerTouchOneByOne *_listener1;
	bool onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent); //Ĳ�I�}�l�ƥ�
	void onTouchMoved(cocos2d::Touch *pTouch, cocos2d::Event *pEvent); //Ĳ�I���ʨƥ�
	void onTouchEnded(cocos2d::Touch *pTouch, cocos2d::Event *pEvent); //Ĳ�I�����ƥ� 

	
    // implement the "static create()" method manually
    CREATE_FUNC(Level4);
};

#endif // __LEVEL4_SCENE_H__
