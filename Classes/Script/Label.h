#ifndef __LABEL__
#define __LABEL__

#include <memory>

#include "Script.h"
#include "../Common/IEventListener.h"

class Label final: public Script, public IEventListener
{
	friend class GameObject;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	Label(const Label&) = delete;
	Label(Label&&) = delete;
	Label& operator=(const Label&) = delete;
	Label& operator=(Label&&) = delete;
	
public:
	~Label();

	void setText(const std::string &text);
	void setFontName(const std::string &font_name);
	void setFontSize(float font_size);
	void setPosition(float x, float y);

protected:
	inline virtual void onEvent(Event* e);

private:
	Label(GameObject *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__LABEL__
