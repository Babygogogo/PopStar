#include "StatusBar.h"
#include "Label.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"

struct StatusBar::impl
{
	impl(){};
	~impl(){};

	std::unique_ptr<GameObject> createHighScoreLabel(const std::string &text);
};

std::unique_ptr<GameObject> StatusBar::impl::createHighScoreLabel(const std::string &text)
{
	auto label_object = GameObject::create();
	auto label_underlying = label_object->addComponent<Label>();

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visibleSize.width / 2, visibleSize.height - 50);
	label_underlying->setText(text);

	return label_object;
}

StatusBar::StatusBar(GameObject *game_object) :Script("StatusBar", game_object), pimpl(new impl)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Node>();
	game_object->addChild(pimpl->createHighScoreLabel("TestHighScore"));
}

StatusBar::~StatusBar()
{

}
