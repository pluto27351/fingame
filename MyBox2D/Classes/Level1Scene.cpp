#include "Level1Scene.h"
#include "Level2Scene.h"
#include "StartScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"


USING_NS_CC;
using namespace cocostudio::timeline;

#define MAX_CIRCLE_OBJECTS  11
#define MAX_2(X,Y) (X)>(Y) ? (X) : (Y)


Color3B BlockColor[3] = { Color3B(208,45,45), Color3B(77,204,42), Color3B(252,223,56) };
Color3B BlockColor2[3] = { Color3B(242, 123, 123), Color3B(149, 234, 126), Color3B(255, 240, 158) };

Level1::~Level1()
{

#ifdef BOX2D_DEBUG
	if (_DebugDraw != NULL) delete _DebugDraw;
#endif

	if (_b2World != nullptr) delete _b2World;
	//  for releasing Plist&Texture
	//SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("box2d.plist");
	Director::getInstance()->getTextureCache()->removeUnusedTextures();

}

Scene* Level1::createScene(int num[4][3], int level)
{
	auto scene = Scene::create();
	auto layer = Level1::create();
	layer->addBall(num);
	layer->maxLevel = level;
	scene->addChild(layer);
	return scene;
}

void Level1::addBall(int num[4][3]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			levelball[i][j] = num[i][j];
		}
	}
	nr = 5 + num[2][0];  ng = 2 + num[2][1];  ny = 10 + num[2][2];

	CCString* sr = CCString::createWithFormat("%d", nr);
	const char* cr = sr->getCString();
	_redNum->setString(cr);

	CCString* sg = CCString::createWithFormat("%d", ng);
	const char* cg = sg->getCString();
	_greenNum->setString(cg);

	CCString* sy = CCString::createWithFormat("%d", ny);
	const char* cy = sy->getCString();
	_yellowNum->setString(cy);
}

// on "init" you need to initialize your instance
bool Level1::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
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
	_csbRoot = CSLoader::createNode("Level1.csb");

	SimpleAudioEngine::getInstance()->playBackgroundMusic("./audio/bg.mp3", true);


#ifdef BOX2D_DEBUG
	// �]�w��ܭI���ϥ�
	auto bgSprite = _csbRoot->getChildByName("bg64_2");
	bgSprite->setVisible(false);

#endif

	addChild(_csbRoot, 1);

	createStaticBoundary();
	setStaticWall();
	setBoards();
	setFinalBox();
	setbtn();
	setUIbtn();
	setEndUi();
	setStar();

#ifdef BOX2D_DEBUG
	//DebugDrawInit
	_DebugDraw = nullptr;
	_DebugDraw = new GLESDebugDraw(PTM_RATIO);
	//�]�wDebugDraw
	_b2World->SetDebugDraw(_DebugDraw);
	//���ø�s���O
	uint32 flags = 0;
	flags += GLESDebugDraw::e_shapeBit;						//ø�s�Ϊ�
	flags += GLESDebugDraw::e_pairBit;
	flags += GLESDebugDraw::e_jointBit;
	flags += GLESDebugDraw::e_centerOfMassBit;
	flags += GLESDebugDraw::e_aabbBit;
	//�]�wø�s����
	_DebugDraw->SetFlags(flags);
#endif

	_b2World->SetContactListener(&_colliderSeneor);
	_listener1 = EventListenerTouchOneByOne::create();	//�Ыؤ@�Ӥ@��@���ƥ��ť��
	_listener1->onTouchBegan = CC_CALLBACK_2(Level1::onTouchBegan, this);		//�[�JĲ�I�}�l�ƥ�
	_listener1->onTouchMoved = CC_CALLBACK_2(Level1::onTouchMoved, this);		//�[�JĲ�I���ʨƥ�
	_listener1->onTouchEnded = CC_CALLBACK_2(Level1::onTouchEnded, this);		//�[�JĲ�I���}�ƥ�

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener1, this);	//�[�J��Ыت��ƥ��ť��
	this->schedule(CC_SCHEDULE_SELECTOR(Level1::doStep));

	return true;
}

void  Level1::setbtn() {
	//red
	auto btnSprite = _csbRoot->getChildByName("redbtn");
	_redBtn = CButton::create();
	_redBtn->setButtonInfo("playBtn01_01.png", "playBtn01_02.png", btnSprite->getPosition());
	_redBtn->setScale(btnSprite->getScale());
	this->addChild(_redBtn, 5);
	_csbRoot->removeChildByName("redbtn");

	_redNum = cocos2d::ui::Text::create("0", "Marker Felt.ttf", 36);
	_redNum->setPosition(btnSprite->getPosition());
	this->addChild(_redNum, 6);

	//green
	btnSprite = _csbRoot->getChildByName("greenbtn");
	_greenBtn = CButton::create();
	_greenBtn->setButtonInfo("playBtn02_01.png", "playBtn02_02.png", btnSprite->getPosition());
	_greenBtn->setScale(btnSprite->getScale());
	this->addChild(_greenBtn, 5);
	_csbRoot->removeChildByName("greenbtn");

	_greenNum = cocos2d::ui::Text::create("0", "Marker Felt.ttf", 36);
	_greenNum->setPosition(btnSprite->getPosition());
	this->addChild(_greenNum, 6);

	//yellow
	btnSprite = _csbRoot->getChildByName("bluebtn");
	_blueBtn = CButton::create();
	_blueBtn->setButtonInfo("playBtn03_01.png", "playBtn03_02.png", btnSprite->getPosition());
	_blueBtn->setScale(btnSprite->getScale());
	this->addChild(_blueBtn, 5);
	_csbRoot->removeChildByName("bluebtn");

	_yellowNum = cocos2d::ui::Text::create("0", "Marker Felt.ttf", 36);
	_yellowNum->setPosition(btnSprite->getPosition());
	this->addChild(_yellowNum, 6);

	//pen
	btnSprite = _csbRoot->getChildByName("penbtn");
	_penBtn = CSwitchButton::create();
	_penBtn->setButtonInfo("playBtn00_01.png", "playBtn00_02.png", btnSprite->getPosition());
	_penBtn->setScale(btnSprite->getScale());
	this->addChild(_penBtn, 5);
	_csbRoot->removeChildByName("penbtn");
}

void  Level1::setUIbtn() {
	auto btnSprite = _csbRoot->getChildByName("homebtn");
	_homeBtn = CButton::create();
	_homeBtn->setButtonInfo("uiBtn01_01.png", "uiBtn01_02.png", btnSprite->getPosition());
	_homeBtn->setScale(btnSprite->getScale());
	this->addChild(_homeBtn, 5);
	_csbRoot->removeChildByName("homebtn");


	btnSprite = _csbRoot->getChildByName("replaybtn");
	_replayBtn = CButton::create();
	_replayBtn->setButtonInfo("uiBtn02_01.png", "uiBtn02_02.png", btnSprite->getPosition());
	_replayBtn->setScale(btnSprite->getScale());
	this->addChild(_replayBtn, 5);
	_csbRoot->removeChildByName("replaybtn");
}

void Level1::setEndUi() {
	_endUi = new Node;
	this->addChild(_endUi, 20);

	auto bg = Sprite::createWithSpriteFrameName("bgwhite.png");
	bg->setPosition(640, 360);
	bg->setScale(20);
	_endUi->addChild(bg, 1);

	auto level = Sprite::createWithSpriteFrameName("levelup.png");
	level->setPosition(640, 360);
	level->setScale(2);
	_endUi->addChild(level, 2);

	auto btnSprite = _csbRoot->getChildByName("endhome");
	_homeBtn2 = CButton::create();
	_homeBtn2->setButtonInfo("uiBtn01_01.png", "uiBtn01_02.png", btnSprite->getPosition());
	_homeBtn2->setScale(btnSprite->getScale());
	_endUi->addChild(_homeBtn2, 5);
	_csbRoot->removeChildByName("endhome");

	btnSprite = _csbRoot->getChildByName("endreplay");
	_replayBtn2 = CButton::create();
	_replayBtn2->setButtonInfo("uiBtn02_01.png", "uiBtn02_02.png", btnSprite->getPosition());
	_replayBtn2->setScale(btnSprite->getScale());
	_endUi->addChild(_replayBtn2, 5);
	_csbRoot->removeChildByName("endreplay");

	btnSprite = _csbRoot->getChildByName("endplay");
	_nextBtn = CButton::create();
	_nextBtn->setButtonInfo("uiBtn03_01.png", "uiBtn03_02.png", btnSprite->getPosition());
	_nextBtn->setScale(btnSprite->getScale());
	_endUi->addChild(_nextBtn, 5);
	_csbRoot->removeChildByName("endplay");

	for (int i = 0; i < 3; i++) {
		_score[i] = cocos2d::ui::Text::create("0", "Marker Felt.ttf", 36);
		_score[i]->setPosition(Vec2(495 + 145 * i , 337));  //(495,337),(640,337),(785,337)
		_endUi->addChild(_score[i], 6);
	}

	_endUi->setVisible(false);

}

void Level1::createStaticBoundary()
{
	// ������ Body, �]�w�������Ѽ�

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody; // �]�w�o�� Body �� �R�A��
	bodyDef.userData = NULL;
	// �b b2World �����͸� Body, �öǦ^���ͪ� b2Body ���󪺫���
	// ���ͤ@���A�N�i�H���᭱�Ҧ��� Shape �ϥ�
	b2Body *body = _b2World->CreateBody(&bodyDef);
	_bottomBody = body;
	// �����R�A��ɩһݭn�� EdgeShape
	b2EdgeShape edgeShape;
	b2FixtureDef edgeFixtureDef; // ���� Fixture
	edgeFixtureDef.shape = &edgeShape;
	// bottom edge
	/*edgeShape.Set(b2Vec2(0.0f / PTM_RATIO, 0.0f / PTM_RATIO), b2Vec2(_visibleSize.width / PTM_RATIO, 0.0f / PTM_RATIO));
	body->CreateFixture(&edgeFixtureDef);
	*/
	// left edge
	edgeShape.Set(b2Vec2(0.0f / PTM_RATIO, 0.0f / PTM_RATIO), b2Vec2(0.0f / PTM_RATIO, _visibleSize.height / PTM_RATIO));
	body->CreateFixture(&edgeFixtureDef);

	// right edge
	edgeShape.Set(b2Vec2(_visibleSize.width / PTM_RATIO, 0.0f / PTM_RATIO), b2Vec2(_visibleSize.width / PTM_RATIO, _visibleSize.height / PTM_RATIO));
	body->CreateFixture(&edgeFixtureDef);

	// top edge
	edgeShape.Set(b2Vec2(0.0f / PTM_RATIO, _visibleSize.height / PTM_RATIO), b2Vec2(_visibleSize.width / PTM_RATIO, _visibleSize.height / PTM_RATIO));
	body->CreateFixture(&edgeFixtureDef);
}

void Level1::setStaticWall() {
	char tmp[20] = "";

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.userData = NULL;
	b2Body *body = _b2World->CreateBody(&bodyDef);

	b2PolygonShape polyshape;
	b2FixtureDef fixtureDef; // ���� Fixture
	fixtureDef.shape = &polyshape;

	for (int i = 0; i < 1; i++)
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
	_born = Sprite::createWithSpriteFrameName("pipe01.png");
	_born->setPosition(pt);
	this->addChild(_born, 10);
	_csbRoot->removeChildByName("born");


}

void Level1::setBoards() {
	char tmp[20] = "";
	Sprite *gearSprite[6];
	Point loc[6];
	Size  size[6];
	float sx, sy;
	b2Body* staticBody[6];
	b2Body* dynamicBody[6];

	b2BodyDef staticBodyDef;
	staticBodyDef.type = b2_staticBody;
	staticBodyDef.userData = NULL;

	b2CircleShape staticShape;
	staticShape.m_radius = 5 / PTM_RATIO;
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &staticShape;

	for (int i = 0; i < 6; i++)
	{
		int color;
		if (i < 2) {                             //0.1   red
			color = 1;
			sprintf(tmp, "redblock_%02d", i);
		}
		else if (i < 3) {                        //2     green
			color = 2;
			sprintf(tmp, "greenblock_%02d", i);
		}
		else if (i < 6) {                        //3.4.5  yellow 
			color = 3;
			sprintf(tmp, "yellowblock_%02d", i);
		} 

		gearSprite[i] = (Sprite *)_csbRoot->getChildByName(tmp);
		loc[i] = gearSprite[i]->getPosition();
		size[i] = gearSprite[i]->getContentSize();
		sx = gearSprite[i]->getScaleX();
		sy = gearSprite[i]->getScaleY();

		staticBodyDef.position.Set((loc[i].x) / PTM_RATIO, loc[i].y / PTM_RATIO);
		staticBody[i] = _b2World->CreateBody(&staticBodyDef);
		fixtureDef.filter.categoryBits = 1 << color;  // 1 =  red ;2 = green ;3 = yellow
		staticBody[i]->CreateFixture(&fixtureDef);
	}

	b2BodyDef dynamicBodyDef;
	dynamicBodyDef.type = b2_dynamicBody;

	b2PolygonShape polyShape;
	fixtureDef.shape = &polyShape;
	fixtureDef.density = 1000.0f;
	fixtureDef.friction = 0.2f;
	fixtureDef.restitution = 0.25f;

	for (int i = 0; i < 6; i++)
	{
		int color;
		if (i < 2)color = 1;
		else if (i < 3)color = 2;
		else color = 3;

		polyShape.SetAsBox((size[i].width - 4) *0.5f *sx / PTM_RATIO, (size[i].height - 4) *0.5f *sy / PTM_RATIO);

		dynamicBodyDef.userData = gearSprite[i];
		gearSprite[i]->setColor(BlockColor[color-1]);   // 0 =  red ;1 = green ;2 = yellow
		dynamicBodyDef.position.Set(loc[i].x / PTM_RATIO, loc[i].y / PTM_RATIO);
		dynamicBody[i] = _b2World->CreateBody(&dynamicBodyDef);
		fixtureDef.filter.categoryBits = 1 << color;  // 1 =  red ;2 = green ;3 = yellow
		dynamicBody[i]->CreateFixture(&fixtureDef);
	}

	b2RevoluteJointDef RJoint;	// �������`
	for (int i = 0; i < 6; i++)
	{
		b2RevoluteJoint*  RevJoint;
		RJoint.Initialize(staticBody[i], dynamicBody[i], dynamicBody[i]->GetWorldCenter());
		RevJoint = (b2RevoluteJoint*)_b2World->CreateJoint(&RJoint);
	}
}


void Level1::setFinalBox() {
	//���I�c�l
	char tmp[20] = "";
	Sprite *boxSprite[12];
	Point loc[12];
	Size  size[12];
	b2Body* boxBody[12];
	b2FixtureDef fixtureDef;
	b2BodyDef boxBodyDef;
	boxBodyDef.type = b2_staticBody;

	b2PolygonShape polyShape;
	fixtureDef.friction = 0.2f;
	fixtureDef.restitution = 0.0f;
	fixtureDef.shape = &polyShape;
	fixtureDef.density = 1.0f;
	for (int i = 0; i < 9; i++)
	{
		sprintf(tmp, "finbox_%02d", i);
		boxSprite[i] = (Sprite *)_csbRoot->getChildByName(tmp);
		loc[i] = boxSprite[i]->getPosition();
		size[i] = boxSprite[i]->getContentSize();
		float sx = boxSprite[i]->getScaleX();
		float sy = boxSprite[i]->getScaleY();
		polyShape.SetAsBox((size[i].width - 4) *0.5f *sx / PTM_RATIO, (size[i].height - 4) *0.5f *sy / PTM_RATIO);

		boxBodyDef.userData = boxSprite[i];
		boxSprite[i]->setColor(BlockColor[(i / 3)]);	// �ϥ� filterColor �w�g�إߪ��C��
		boxBodyDef.position.Set(loc[i].x / PTM_RATIO, loc[i].y / PTM_RATIO);
		boxBody[i] = _b2World->CreateBody(&boxBodyDef);
		fixtureDef.filter.categoryBits = 1 << (i / 3 + 1);   // 0.1.2 =1  3.4.2 =2  6.7.8 =3
		boxBody[i]->CreateFixture(&fixtureDef);
	}

	//���I�P����
	fixtureDef.friction = 0.2f;
	fixtureDef.restitution = 0.0f;
	fixtureDef.shape = &polyShape;
	for (int i = 9; i < 12; i++)
	{
		sprintf(tmp, "boxSensor_%02d", i - 8);
		boxSprite[i] = (Sprite *)_csbRoot->getChildByName(tmp);
		loc[i] = boxSprite[i]->getPosition();
		size[i] = boxSprite[i]->getContentSize();
		float sx = boxSprite[i]->getScaleX();
		float sy = boxSprite[i]->getScaleY();
		polyShape.SetAsBox((size[i].width - 4) *0.5f *sx / PTM_RATIO, (size[i].height - 4) *0.5f *sy / PTM_RATIO);

		boxBodyDef.userData = boxSprite[i];
		boxSprite[i]->setColor(BlockColor2[(i % 3)]);	// �ϥ� filterColor �w�g�إߪ��C��
		boxBodyDef.position.Set(loc[i].x / PTM_RATIO, loc[i].y / PTM_RATIO);
		boxBody[i] = _b2World->CreateBody(&boxBodyDef);
		fixtureDef.filter.categoryBits = 1 << ((i % 3 + 1));   // 0.1.2 =1  3.4.2 =2  6.7.8 =3
		fixtureDef.density = 500.0f + 100 * (i - 9);  //500 600 700
		fixtureDef.isSensor = true;
		boxBody[i]->CreateFixture(&fixtureDef);
	}

}

void Level1::setStar() {
	auto circleSprite = _csbRoot->getChildByName("star");
	auto loc = circleSprite->getPosition();
	auto size = circleSprite->getContentSize();
	b2CircleShape circleShape;
	circleShape.m_radius = size.width*0.5f / PTM_RATIO;
	//�P����
	auto _collisionSprite = (Sprite *)_csbRoot->getChildByName("star");
	_colliderSeneor.setCollisionTarget2(*_collisionSprite);

	b2BodyDef circleBodyDef;
	b2FixtureDef fixtureDef;
	circleBodyDef.type = b2_staticBody;
	circleBodyDef.position.Set(loc.x / PTM_RATIO, loc.y / PTM_RATIO);
	circleBodyDef.userData = circleSprite;
	_star = _b2World->CreateBody(&circleBodyDef);
	fixtureDef.shape = &circleShape;
	fixtureDef.density = 1.0f;
	fixtureDef.isSensor = true;
	_star->CreateFixture(&fixtureDef);


}

void Level1::doStep(float dt)
{
	int velocityIterations = 8;	// �t�׭��N����
	int positionIterations = 1; // ��m���N���� ���N���Ƥ@��]�w��8~10 �V���V�u����Ĳv�V�t
								// Instruct the world to perform a single step of simulation.
								// It is generally best to keep the time step and iterations fixed.
	_b2World->Step(dt, velocityIterations, positionIterations);

	// ���o _b2World ���Ҧ��� body �i��B�z
	// �̥D�n�O�ھڥثe�B�⪺���G�A��s���ݦb body �� sprite ����m
	for (b2Body* body = _b2World->GetBodyList(); body;)
	{
		//body->ApplyForce(b2Vec2(10.0f, 10.0f), body->GetWorldCenter(), true);
		// �H�U�O�H Body ���]�t Sprite ��ܬ���
		if (body->GetUserData() != NULL)
		{
			Sprite *ballData = (Sprite*)body->GetUserData();
			ballData->setPosition(body->GetPosition().x*PTM_RATIO, body->GetPosition().y*PTM_RATIO);
			ballData->setRotation(-1 * CC_RADIANS_TO_DEGREES(body->GetAngle()));
			int d = body->GetFixtureList()->GetDensity() * 100.0;
			if(d == 111){
				auto sparkSprite = Sprite::createWithSpriteFrameName("star.png");
				sparkSprite->setScale(0.5);
				sparkSprite->setBlendFunc(BlendFunc::ADDITIVE);
				this->addChild(sparkSprite, 5);
				//���ͤp������
				b2BodyDef RectBodyDef;
				RectBodyDef.position.Set(body->GetPosition().x, body->GetPosition().y);
				RectBodyDef.type = b2_dynamicBody;
				RectBodyDef.userData = sparkSprite;
				b2PolygonShape RectShape;
				RectShape.SetAsBox(5 / PTM_RATIO, 5 / PTM_RATIO);
				b2Body* RectBody = _b2World->CreateBody(&RectBodyDef);
				b2FixtureDef RectFixtureDef;
				RectFixtureDef.shape = &RectShape;
				RectFixtureDef.density = 1.0f;
				RectFixtureDef.filter.maskBits = 0;
				b2Fixture*RectFixture = RectBody->CreateFixture(&RectFixtureDef);
			}
		}
		// �]�X�ù��~���N������q b2World ������
		if (body->GetType() == b2BodyType::b2_dynamicBody) {
			float x = body->GetPosition().x * PTM_RATIO;
			float y = body->GetPosition().y * PTM_RATIO;
			if (x > _visibleSize.width || x < 0 || y >  _visibleSize.height || y < 0) {
				if (body->GetUserData() != NULL) {
					Sprite* spriteData = (Sprite *)body->GetUserData();
					this->removeChild(spriteData);
				}
				_b2World->DestroyBody(body);
				body = NULL;
			}
			else body = body->GetNext(); //�_�h�N�~���s�U�@��Body
		}
		else body = body->GetNext(); //�_�h�N�~���s�U�@��Body
		
	}

	
	//���N
	if (_colliderSeneor.star == true) {
		for (int i = 0; i < 20; i++) {
			// �إ� Spark Sprite �ûP�ثe�����鵲�X
			auto sparkSprite = Sprite::createWithSpriteFrameName("star.png");
			sparkSprite->setScale(0.5);
			sparkSprite->setBlendFunc(BlendFunc::ADDITIVE);
			this->addChild(sparkSprite, 5);
			//���ͤp������
			b2BodyDef RectBodyDef;
			Vec2 pt;
			float r = i*M_PI / 10;  // 18*i*PI/180
			pt.x = cosf(r);  pt.y = sinf(r);
			RectBodyDef.position.Set(pt.x + _star->GetPosition().x , pt.y + _star->GetPosition().y);
			RectBodyDef.type = b2_dynamicBody;
			RectBodyDef.userData = sparkSprite;
			b2PolygonShape RectShape;
			RectShape.SetAsBox(5 / PTM_RATIO, 5 / PTM_RATIO);
			b2Body* RectBody = _b2World->CreateBody(&RectBodyDef);
			b2FixtureDef RectFixtureDef;
			RectFixtureDef.shape = &RectShape;
			RectFixtureDef.density = 1.0f;
			RectFixtureDef.filter.maskBits = 0;
			b2Fixture*RectFixture = RectBody->CreateFixture(&RectFixtureDef);

			//���O�q
			RectBody->ApplyForce(b2Vec2(pt.x * 10, pt.y * 10), _colliderSeneor._createLoc, true);
		}
		_colliderSeneor.star = false;
		_bstar = true;
		Sprite* spriteData = (Sprite *)_star->GetUserData();
		_csbRoot->removeChild(spriteData);
		_b2World->DestroyBody(_star);
	}

	//���I�P�_
	if (_bboxR && _bboxG && _bboxB) {
		CCLOG("LEVEL UP!");
		startGame = false;
		_endUi->setVisible(true);
		levelball[1][0] = nr;
		levelball[1][1] = ng;
		levelball[1][2] = ny;
		if (maxLevel < 2)maxLevel = 2;
		for (int i = 0; i < 3; i++) {
			CCString* s = CCString::createWithFormat("+%d", levelball[1][i]);
			const char* c = s->getCString();
			_score[i]->setString(c);
		}
		if (_bstar) {
			auto starsprite = Sprite::createWithSpriteFrameName("star.png");
			starsprite->setPosition(Vec2(834,461));
			starsprite->setScale(1.6);
			starsprite->setRotation(15);
			this->addChild(starsprite, 30);
		}
	}
	if (!_bboxR) {
		if (_colliderSeneor.inBoxR == true) _bboxR = true;
	}
	if (!_bboxG) {
		if (_colliderSeneor.inBoxG == true) _bboxG = true;
	}
	if (!_bboxB) {
		if (_colliderSeneor.inBoxB == true) _bboxB = true;
	}
}

bool Level1::onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)//Ĳ�I�}�l�ƥ�
{
	Point touchLoc = pTouch->getLocation();

	if (startGame) {
		for (b2Body* body = _b2World->GetBodyList(); body; body = body->GetNext())
		{
			if (body->GetUserData() != NULL) {// �R�A���餣�B�z	
				Sprite *spriteObj = (Sprite*)body->GetUserData();
				Size objSize = spriteObj->getContentSize();
				Point pt = spriteObj->getPosition();
				float scaleX = spriteObj->getScaleX();
				float scaleY = spriteObj->getScaleY();

				float d = body->GetFixtureList()->GetDensity();
				if (d == 100000) {    //�l�l�ΪO�l
									  //�P�_�I����m�O�_���b�ʺA����@�w���d��
					float fdist = MAX_2(objSize.width*scaleX, objSize.height*scaleY) / 2.0f;
					float x = body->GetPosition().x*PTM_RATIO - touchLoc.x;
					float y = body->GetPosition().y*PTM_RATIO - touchLoc.y;
					float tpdist = x*x + y*y;
					if (tpdist < fdist*fdist) {
						_bTouchOn = true;
						b2MouseJointDef mouseJointDef;
						mouseJointDef.bodyA = _bottomBody;
						mouseJointDef.bodyB = body;
						mouseJointDef.target = b2Vec2(touchLoc.x / PTM_RATIO, touchLoc.y / PTM_RATIO);
						mouseJointDef.collideConnected = true;
						mouseJointDef.maxForce = 1000.0f * body->GetMass();
						_MouseJoint = (b2MouseJoint*)_b2World->CreateJoint(&mouseJointDef); // �s�W Mouse Joint
						body->SetAwake(true);
						_bMouseOn = true;
						break;
					}
				}
				else if (d == 1000) {
					objSize.width *= scaleX;   objSize.height *= scaleY;
					float w = (objSize.width + objSize.height) / 2;
					Rect collider(pt.x - w / 2, pt.y - w / 2, w, w);
					if (collider.containsPoint(touchLoc)) {
						_bTouchOn = true;
						b2MouseJointDef mouseJointDef;
						mouseJointDef.bodyA = _bottomBody;
						mouseJointDef.bodyB = body;
						mouseJointDef.target = b2Vec2(touchLoc.x / PTM_RATIO, touchLoc.y / PTM_RATIO);
						mouseJointDef.collideConnected = true;
						mouseJointDef.maxForce = 1000.0f * body->GetMass();
						_MouseJoint = (b2MouseJoint*)_b2World->CreateJoint(&mouseJointDef); // �s�W Mouse Joint
						body->SetAwake(true);
						_bMouseOn = true;
						break;
					}
				}
			}
		}

		_redBtn->touchesBegin(touchLoc);
		_greenBtn->touchesBegin(touchLoc);
		_blueBtn->touchesBegin(touchLoc);
		_homeBtn->touchesBegin(touchLoc);
		_replayBtn->touchesBegin(touchLoc);
	}
	else {
		_homeBtn2->touchesBegin(touchLoc);
		_replayBtn2->touchesBegin(touchLoc);
		_nextBtn->touchesBegin(touchLoc);
	}


	return true;
}

void  Level1::onTouchMoved(cocos2d::Touch *pTouch, cocos2d::Event *pEvent) //Ĳ�I���ʨƥ�
{
	Point touchLoc = pTouch->getLocation();
	if (startGame) {
		if (_bTouchOn)
		{
			_MouseJoint->SetTarget(b2Vec2(touchLoc.x / PTM_RATIO, touchLoc.y / PTM_RATIO));
		}

		_redBtn->touchesBegin(touchLoc);
		_greenBtn->touchesBegin(touchLoc);
		_blueBtn->touchesBegin(touchLoc);

		_homeBtn->touchesBegin(touchLoc);
		_replayBtn->touchesBegin(touchLoc);
	}
	else {
		_homeBtn2->touchesBegin(touchLoc);
		_replayBtn2->touchesBegin(touchLoc);
		_nextBtn->touchesBegin(touchLoc);
	}
}

void  Level1::onTouchEnded(cocos2d::Touch *pTouch, cocos2d::Event *pEvent) //Ĳ�I�����ƥ� 
{
	Point touchLoc = pTouch->getLocation();
	if (_bTouchOn)
	{
		_bTouchOn = false;
		if (_MouseJoint != NULL)
		{
			_b2World->DestroyJoint(_MouseJoint);
			_MouseJoint = NULL;
			_bMouseOn = false;

		}
	}

	if (_redBtn->touchesEnded(touchLoc)) {
		if (nr > 0) {
			nr -= 1;
			renderball("ball_01.png", 1);
		}
	}
	if (_greenBtn->touchesEnded(touchLoc)) {
		if (ng > 0) {
			ng -= 1;
			renderball("ball_02.png", 2);
		}
	}
	if (_blueBtn->touchesEnded(touchLoc)) {
		if (ny > 0) {
			ny -= 1;
			renderball("ball_03.png", 3);
		}
	}

	if (_homeBtn->touchesEnded(touchLoc)) {
		this->unschedule(schedule_selector(Level1::doStep));
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("box2d.plist");
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("UIBTN.plist");
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
		TransitionFade *pageTurn;
		pageTurn = TransitionFade::create(1.0F, StartScene::createScene(levelball, maxLevel));
		Director::getInstance()->replaceScene(pageTurn);

	}
	if (_replayBtn->touchesEnded(touchLoc)) {
		this->unschedule(schedule_selector(Level1::doStep));
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("box2d.plist");
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("UIBTN.plist");
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
		TransitionFade *pageTurn;
		pageTurn = TransitionFade::create(1.0F, Level1::createScene(levelball, maxLevel));
		Director::getInstance()->replaceScene(pageTurn);
		
	}
	if (_homeBtn2->touchesEnded(touchLoc)) {
		this->unschedule(schedule_selector(Level1::doStep));
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("box2d.plist");
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("UIBTN.plist");
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
		TransitionFade *pageTurn;
		pageTurn = TransitionFade::create(1.0F, StartScene::createScene(levelball, maxLevel));
		Director::getInstance()->replaceScene(pageTurn);

	}
	if (_replayBtn2->touchesEnded(touchLoc)) {
		this->unschedule(schedule_selector(Level1::doStep));
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("box2d.plist");
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("UIBTN.plist");
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
		TransitionFade *pageTurn;
		pageTurn = TransitionFade::create(1.0F, Level1::createScene(levelball, maxLevel));
		Director::getInstance()->replaceScene(pageTurn);

	}
	if (_nextBtn->touchesEnded(touchLoc)) {
		this->unschedule(schedule_selector(Level1::doStep));
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("box2d.plist");
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("UIBTN.plist");
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
		TransitionFade *pageTurn;
		pageTurn = TransitionFade::create(1.0F, Level2::createScene(levelball, maxLevel));
		Director::getInstance()->replaceScene(pageTurn);
		

	}

}
void Level1::renderball(char *name, int mask) {
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

	if (mask == 1) {
		CCString* s = CCString::createWithFormat("%d", nr);
		const char* c = s->getCString();
		_redNum->setString(c);
	}
	else if (mask == 2) {
		CCString* s = CCString::createWithFormat("%d", ng);
		const char* c = s->getCString();
		_greenNum->setString(c);
	}
	else if (mask == 3) {
		CCString* s = CCString::createWithFormat("%d", ny);
		const char* c = s->getCString();
		_yellowNum->setString(c);
	}
}


#ifdef BOX2D_DEBUG
//��gø�s��k
void Level1::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	Director* director = Director::getInstance();

	GL::enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION);
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	_b2World->DrawDebugData();
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}
#endif
