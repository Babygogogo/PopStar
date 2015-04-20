#include "Label.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../Common/SingletonContainer.h"
#include "../Event/EventDispatcher.h"

struct Label::impl
{
	impl(){};
	~impl(){};

	cocos2d::Label *m_label{ nullptr };
};

Label::Label(GameObject *game_object) : Script("Label", game_object), pimpl(new impl)
{
	pimpl->m_label = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>(
		[](){return cocos2d::Label::createWithSystemFont("", "Verdana - Bold", 30); });
}

Label::~Label()
{
	if (auto singleton_container = SingletonContainer::instance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

void Label::onEvent(Event* e)
{

}

void Label::setText(const std::string &text)
{
	pimpl->m_label->setString(text);
}

void Label::setFontName(const std::string &font_name)
{
	pimpl->m_label->setSystemFontName(font_name);
}

void Label::setFontSize(float font_size)
{
	pimpl->m_label->setSystemFontSize(font_size);
}

void Label::setPosition(float x, float y)
{
	pimpl->m_label->setPositionX(x);
	pimpl->m_label->setPositionY(y);
}
