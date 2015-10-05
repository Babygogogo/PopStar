#ifndef __TITLE_MENU_SCRIPT__
#define __TITLE_MENU_SCRIPT__

#include <memory>
#include <string>

#include "../Actor/BaseScriptComponent.h"

class TitleMenuScript final : public BaseScriptComponent
{
public:
	TitleMenuScript();
	~TitleMenuScript();

	static const std::string Type;

	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	TitleMenuScript(const TitleMenuScript &) = delete;
	TitleMenuScript(TitleMenuScript &&) = delete;
	TitleMenuScript & operator=(const TitleMenuScript &) = delete;
	TitleMenuScript & operator=(TitleMenuScript &&) = delete;

private:
	//Implementation stuff.
	struct TitleMenuScriptImpl;
	std::unique_ptr<TitleMenuScriptImpl> pimpl;
};

#endif // !__TITLE_MENU_SCRIPT__
