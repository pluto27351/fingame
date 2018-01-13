#include "CSwitchButton.h"

USING_NS_CC;

CSwitchButton* CSwitchButton::create()
{
	CSwitchButton *btnobj = new (std::nothrow) CSwitchButton();
	if (btnobj)
	{
		btnobj->autorelease();
		return btnobj;
	}
	CC_SAFE_DELETE(btnobj);
	return nullptr;
}


CSwitchButton::CSwitchButton()
{
	_SwitchOffPic = nullptr;
	_SwitchOnPic = nullptr;
}

void CSwitchButton::setButtonInfo(const char *offImg, const char *onImg, const cocos2d::Point locPt)
{
	_SwitchOffPic = (Sprite *)Sprite::createWithSpriteFrameName(offImg);
	_SwitchOnPic = (Sprite *)Sprite::createWithSpriteFrameName(onImg);

	_BtnLoc = locPt;
	_SwitchOffPic->setPosition(_BtnLoc); // �]�w��m
	_SwitchOnPic->setPosition(_BtnLoc); // �]�w��m
	_SwitchOffPic->setVisible(true);
	_SwitchOnPic->setVisible(false);

	_ShowBtn = _SwitchOffPic;

	this->addChild(_SwitchOffPic,1);
	this->addChild(_SwitchOnPic, 1);

	// ���o�j�p
	_BtnSize = _SwitchOffPic->getContentSize();
	// �]�w�P�_�ϰ�
	_BtnRect.size = _BtnSize;
	_BtnRect.origin.x = _BtnLoc.x - _BtnSize.width*0.5f;
	_BtnRect.origin.y = _BtnLoc.y - _BtnSize.height*0.5f;
	_bTouched = false;
	_fScale = 1.0f;
	_bVisible = true;
	_bSwitchOn = false;
}


bool CSwitchButton::touchesBegan(cocos2d::Point inPos)
{
	if( _BtnRect.containsPoint(inPos) && _bVisible )
	{
		_bTouched = true;
		_ShowBtn->setScale(_fScale+0.05f);
		return(true); // �����b�W��
	}
	return(false);
}

bool CSwitchButton::touchesMoved(cocos2d::Point inPos)
{
	if( _bTouched ) { // �u���Q�����ɭԤ~�B�z
		if( !_BtnRect.containsPoint(inPos) ) { // ����Y��m���}���s
			_bTouched = false;
			_ShowBtn->setScale(_fScale);
			return(false);
		}
		else return(true);
	}
	return(false); // �ƫ�A������s�W�N�Q����
}

bool CSwitchButton::touchesEnded(cocos2d::Point inPos)
{
	if( _bTouched ) {
		_bSwitchOn = !_bSwitchOn;
		_ShowBtn->setScale(_fScale);
		_ShowBtn->setVisible(false);
		if (_bSwitchOn) _ShowBtn = _SwitchOnPic;
		else _ShowBtn = _SwitchOffPic;
		_ShowBtn->setVisible(true);

		_bTouched = false;
		if( _BtnRect.containsPoint(inPos) ) return(true);  // ����Y��m���s�ɡA�٦b�ӫ��s�W
	}
	return false;
}

void CSwitchButton::setVisible(bool bVisible)
{
	_bVisible = bVisible;
	if (!_bVisible) {
		_SwitchOffPic->setVisible(false);
	}
	else {
		_SwitchOffPic->setVisible(true);
	}
}

void CSwitchButton::setScale(float scale)
{
	_fScale = scale;
	_SwitchOnPic->setScale(_fScale);
	_SwitchOffPic->setScale(_fScale);
}

bool CSwitchButton::getStatus()
{
	return(_bSwitchOn); // �Ǧ^�ثe���s�����A���}�άO��
}

void CSwitchButton::setStatus(bool status) {
	_SwitchOnPic->setVisible(status);
	_SwitchOffPic->setVisible(!status);
	_bSwitchOn = status;
}

void CSwitchButton::setUneable() {
	_Unable = (Sprite *)Sprite::createWithSpriteFrameName("levelBtn00.png");

	_BtnLoc = _SwitchOffPic->getPosition();
	_Unable->setPosition(_BtnLoc); // �]�w��m

	this->addChild(_Unable, 10);

}