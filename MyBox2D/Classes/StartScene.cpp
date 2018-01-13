#include "StartScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"


USING_NS_CC;
using namespace cocostudio::timeline;

#define MAX_2(X,Y) (X)>(Y) ? (X) : (Y)

extern Color3B BlockColor[3];
extern Color3B BlockColor2[3];

StartScene::~StartScene()
{

	if (_b2World != nullptr) delete _b2World;
//  for releasing Plist&Texture
	//SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("box2d.plist");
	Director::getInstance()->getTextureCache()->removeUnusedTextures();

}

Scene* StartScene::createScene(int num[4][3],int level)
{
    auto scene = Scene::create();
    auto layer = StartScene::create();
	layer->setballNum(num,level);
    scene->addChild(layer);
    return scene;
}

void StartScene::setballNum(int num[4][3],int level) {
	if (level > maxLevel)maxLevel = level;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			levelball[i][j] = num[i][j];
		}
	}

	for (int i = 4; i > maxLevel; i--) {
		_LevelBtn[i - 1]->setUneable();
	}
}


// on "init" you need to initialize your instance
bool StartScene::init()
{   
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

//  For Loading Plist+Texture
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("box2d.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("UIBTN.plist");

	_visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
   

	// �إ� Box2D world
	_b2World = nullptr;
	b2Vec2 Gravity = b2Vec2(0.0f, -9.8f);	//���O��V
	bool AllowSleep = true;					//���\�ε�
	_b2World = new b2World(Gravity);		//�Ыإ@��
	_b2World->SetAllowSleeping(AllowSleep);	//�]�w���󤹳\�ε�

	// Ū�J CSB ��
	_csbRoot = CSLoader::createNode("StartScene.csb");

	addChild(_csbRoot, 1);

	setStaticWall();
	setUIbtn();
	setbtn();

	SimpleAudioEngine::getInstance()->playBackgroundMusic("./audio/bg.mp3", true);

	/*auto bkmusic = (cocostudio::ComAudio *)_csbRoot->getChildByName("BG_Music")->getComponent("BG_Music");
	bkmusic->playBackgroundMusic();*/

	_listener1 = EventListenerTouchOneByOne::create();	//�Ыؤ@�Ӥ@��@���ƥ��ť��
	_listener1->onTouchBegan = CC_CALLBACK_2(StartScene::onTouchBegan, this);		//�[�JĲ�I�}�l�ƥ�
	_listener1->onTouchMoved = CC_CALLBACK_2(StartScene::onTouchMoved, this);		//�[�JĲ�I���ʨƥ�
	_listener1->onTouchEnded = CC_CALLBACK_2(StartScene::onTouchEnded, this);		//�[�JĲ�I���}�ƥ�

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener1, this);	//�[�J��Ыت��ƥ��ť��
	this->schedule(CC_SCHEDULE_SELECTOR(StartScene::doStep));

    return true;
}

void  StartScene::setbtn() {
	auto btnSprite = _csbRoot->getChildByName("redbtn");
	_redBtn = CButton::create();
	_redBtn->setButtonInfo("playBtn01_01.png", "playBtn01_02.png", btnSprite->getPosition());
	_redBtn->setScale(btnSprite->getScale());
	this->addChild(_redBtn, 5);
	btnSprite->setVisible(false);

	btnSprite = _csbRoot->getChildByName("greenbtn");
	_greenBtn = CButton::create();
	_greenBtn->setButtonInfo("playBtn02_01.png", "playBtn02_02.png", btnSprite->getPosition());
	_greenBtn->setScale(btnSprite->getScale());
	this->addChild(_greenBtn, 5);
	btnSprite->setVisible(false);

	btnSprite = _csbRoot->getChildByName("bluebtn");
	_blueBtn = CButton::create();
	_blueBtn->setButtonInfo("playBtn03_01.png", "playBtn03_02.png", btnSprite->getPosition());
	_blueBtn->setScale(btnSprite->getScale());
	this->addChild(_blueBtn, 5);
	btnSprite->setVisible(false);
}

void  StartScene::setUIbtn() {
	char tmp[20] = "";
	char tmp2[20] = "";
	Node *btnSprite;
	for (int i = 1; i < 5; i++) {
		sprintf(tmp, "Level_%02d", i);
		btnSprite = _csbRoot->getChildByName(tmp);
		_LevelBtn[i-1] = CSwitchButton::create();
		sprintf(tmp, "levelBtn%02d_01.png", i);
		sprintf(tmp2, "levelBtn%02d_02.png", i);
		_LevelBtn[i-1]->setButtonInfo(tmp, tmp2, btnSprite->getPosition());
		this->addChild(_LevelBtn[i-1], 5);
		btnSprite->setVisible(false);
	}
	
	btnSprite = _csbRoot->getChildByName("startbtn");
	_startBtn = CButton::create();
	_startBtn->setButtonInfo("startBtn_01.png", "startBtn_02.png", btnSprite->getPosition());
	this->addChild(_startBtn, 5);
	btnSprite->setVisible(false);

	_NowLevel = _LevelBtn[0];
	_LevelBtn[0]->setStatus(true);
}


void StartScene::setStaticWall() {
	char tmp[20] = "";

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.userData = NULL;
	b2Body *body = _b2World->CreateBody(&bodyDef);

	b2PolygonShape polyshape;
	b2FixtureDef fixtureDef; // ���� Fixture
	fixtureDef.shape = &polyshape;

	for (int i = 0; i < 3; i++)
	{
		sprintf(tmp, "wall_%02d", i);
		auto wallSprite = (Sprite *)_csbRoot->getChildByName(tmp);
		Size ts = wallSprite->getContentSize();
		Point loc = wallSprite->getPosition();
		float angle = wallSprite->getRotation();
		float scaleX = wallSprite->getScaleX();
		float scaleY = wallSprite->getScaleY();

		Point lep[4], wep[4];
		lep[0].x = (ts.width - 4) / 2.0f;;  lep[0].y = (ts.height - 4) / 2.0f;
		lep[1].x = -(ts.width - 4) / 2.0f;; lep[1].y = (ts.height - 4) / 2.0f;
		lep[2].x = -(ts.width - 4) / 2.0f;; lep[2].y = -(ts.height - 4) / 2.0f;
		lep[3].x = (ts.width - 4) / 2.0f;;  lep[3].y = -(ts.height - 4) / 2.0f;

		cocos2d::Mat4 modelMatrix, rotMatrix;
		modelMatrix.m[0] = scaleX;  // ���]�w X �b���Y��
		modelMatrix.m[5] = scaleY;  // ���]�w Y �b���Y��
		cocos2d::Mat4::createRotationZ(angle*M_PI / 180.0f, &rotMatrix);
		modelMatrix.multiply(rotMatrix);
		modelMatrix.m[3] = loc.x; //�]�w Translation�A�ۤv���[�W���˪�
		modelMatrix.m[7] = loc.y; //�]�w Translation�A�ۤv���[�W���˪�
		for (size_t j = 0; j < 4; j++)
		{
			wep[j].x = lep[j].x * modelMatrix.m[0] + lep[j].y * modelMatrix.m[1] + modelMatrix.m[3];
			wep[j].y = lep[j].x * modelMatrix.m[4] + lep[j].y * modelMatrix.m[5] + modelMatrix.m[7];
		}
		b2Vec2 vecs[] = {
			b2Vec2(wep[0].x / PTM_RATIO, wep[0].y / PTM_RATIO),
			b2Vec2(wep[1].x / PTM_RATIO, wep[1].y / PTM_RATIO),
			b2Vec2(wep[2].x / PTM_RATIO, wep[2].y / PTM_RATIO),
			b2Vec2(wep[3].x / PTM_RATIO, wep[3].y / PTM_RATIO) };

		polyshape.Set(vecs, 4);
		if (i == 1)fixtureDef.density = 900;
		else fixtureDef.density = 10;
		body->CreateFixture(&fixtureDef);
	}

	auto bornSprite = (Sprite *)_csbRoot->getChildByName("born");
	Point pt = bornSprite->getPosition();
	bornpt = pt;
	auto _born = Sprite::createWithSpriteFrameName("pipe01.png");
	_born->setPosition(pt);
	this->addChild(_born, 10);
	_csbRoot->removeChildByName("born");
}

void StartScene::doStep(float dt)
{
	int velocityIterations = 8;	// �t�׭��N����
	int positionIterations = 1; // ��m���N���� ���N���Ƥ@��]�w��8~10 �V���V�u����Ĳv�V�t
	// Instruct the world to perform a single step of simulation.
	// It is generally best to keep the time step and iterations fixed.
	_b2World->Step(dt, velocityIterations, positionIterations);

	// ���o _b2World ���Ҧ��� body �i��B�z
	// �̥D�n�O�ھڥثe�B�⪺���G�A��s���ݦb body �� sprite ����m
	for (b2Body* body = _b2World->GetBodyList(); body; body = body->GetNext())
	{
		//body->ApplyForce(b2Vec2(10.0f, 10.0f), body->GetWorldCenter(), true);
		// �H�U�O�H Body ���]�t Sprite ��ܬ���
		if (body->GetUserData() != NULL)
		{
			Sprite *ballData = (Sprite*)body->GetUserData();
			ballData->setPosition(body->GetPosition().x*PTM_RATIO, body->GetPosition().y*PTM_RATIO);
			ballData->setRotation(-1 * CC_RADIANS_TO_DEGREES(body->GetAngle()));
		}
	}

}

bool StartScene::onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)//Ĳ�I�}�l�ƥ�
{
	Point touchLoc = pTouch->getLocation();

	_redBtn->touchesBegin(touchLoc);
	_greenBtn->touchesBegin(touchLoc);
	_blueBtn->touchesBegin(touchLoc);
	//_penBtn->touchesBegin(touchLoc);
	for (int i = 0; i < 4; i++) _LevelBtn[i]->touchesBegan(touchLoc);
	_startBtn->touchesBegin(touchLoc);

	return true;
}

void  StartScene::onTouchMoved(cocos2d::Touch *pTouch, cocos2d::Event *pEvent) //Ĳ�I���ʨƥ�
{
	Point touchLoc = pTouch->getLocation();

	_redBtn->touchesBegin(touchLoc);
	_greenBtn->touchesBegin(touchLoc);
	_blueBtn->touchesBegin(touchLoc);
	for (int i = 0; i < 4; i++) _LevelBtn[i]->touchesMoved(touchLoc);
	_startBtn->touchesBegin(touchLoc);
}

void  StartScene::onTouchEnded(cocos2d::Touch *pTouch, cocos2d::Event *pEvent) //Ĳ�I�����ƥ� 
{
	Point touchLoc = pTouch->getLocation();

	if (_redBtn->touchesEnded(touchLoc))   renderball("ball_01.png", 1);
	if (_greenBtn->touchesEnded(touchLoc)) renderball("ball_02.png", 2);
	if (_blueBtn->touchesEnded(touchLoc))  renderball("ball_03.png", 3);

	if (_LevelBtn[0]->touchesEnded(touchLoc)) {
		if (_NowLevel != _LevelBtn[0]) _NowLevel->setStatus(false);
		LV = 1;
		_NowLevel = _LevelBtn[0];
	}
	if (_LevelBtn[1]->touchesEnded(touchLoc)) {
		if (_NowLevel != _LevelBtn[1]) _NowLevel->setStatus(false);
		LV = 2;
		_NowLevel = _LevelBtn[1];
	}
	if (_LevelBtn[2]->touchesEnded(touchLoc)) {
		if (_NowLevel != _LevelBtn[2]) _NowLevel->setStatus(false);
		LV = 3;
		_NowLevel = _LevelBtn[2];
	}
	if (_LevelBtn[3]->touchesEnded(touchLoc)) {
		if (_NowLevel != _LevelBtn[3]) _NowLevel->setStatus(false);
		LV = 4;
		_NowLevel = _LevelBtn[3];
	}
	if (_startBtn->touchesEnded(touchLoc)) {
		this->unschedule(schedule_selector(StartScene::doStep));
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("box2d.plist");
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("UIBTN.plist");
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
		TransitionFade *pageTurn;
		if(LV == 1)pageTurn = TransitionFade::create(1.0F, Level1::createScene(levelball, maxLevel));
		else if (LV == 2)pageTurn = TransitionFade::create(1.0F, Level2::createScene(levelball,maxLevel));
		else if (LV == 3)pageTurn = TransitionFade::create(1.0F, Level3::createScene(levelball,maxLevel));
		else if (LV == 4)pageTurn = TransitionFade::create(1.0F, Level4::createScene(levelball,maxLevel));
		
		Director::getInstance()->replaceScene(pageTurn);
	


	}
	
}
void StartScene::renderball(char *name, int mask) {
	auto ballSprite = Sprite::createWithSpriteFrameName(name);
	this->addChild(ballSprite, 2);

	// �إߤ@��²�檺�ʺA�y��
	b2BodyDef bodyDef;	// ���H���c b2BodyDef �ŧi�@�� Body ���ܼ�
	bodyDef.type = b2_dynamicBody; // �]�w���ʺA����
	bodyDef.userData = ballSprite;	// �]�w Sprite ���ʺA���骺��ܹϥ�
	bodyDef.position.Set(bornpt.x / PTM_RATIO, bornpt.y / PTM_RATIO);
	// �H bodyDef �b b2World  ���إ߹���öǦ^�ӹ��骺����
	b2Body *ballBody = _b2World->CreateBody(&bodyDef);

	// �]�w�Ӫ��骺�~��
	b2CircleShape ballShape;	//  �ŧi���骺�~�������ܼơA���B�O��Ϊ���
	Size ballsize = ballSprite->getContentSize();	// �ھ� Sprite �ϧΪ��j�p�ӳ]�w��Ϊ��b�|
	ballShape.m_radius = 0.5f*(ballsize.width - 4) / PTM_RATIO;
	// �H b2FixtureDef  ���c�ŧi���鵲�c�ܼơA�ó]�w���骺�������z�Y��
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &ballShape;			// ���w���骺�~�������
	fixtureDef.restitution = 0.15f;			// �]�w�u�ʫY��
	fixtureDef.density = 1.0f;				// �]�w�K��
	fixtureDef.friction = 0.15f;			// �]�w�����Y��
	fixtureDef.filter.maskBits = 1 << mask | 1; //�]�w�s��
	ballBody->CreateFixture(&fixtureDef);	// �b Body �W���ͳo�ӭ��骺�]�w
}

