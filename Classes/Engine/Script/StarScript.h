#ifndef __STAR_SCRIPT__
#define __STAR_SCRIPT__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

//Forward declaration.
namespace cocos2d
{
	struct Color4F;
}

class StarScript final :public BaseScriptComponent
{
public:
	StarScript();
	~StarScript();

	static const std::string Type;

	void randomize(int row_num, int col_num, float pos_x, float pos_y);
	void moveTo(float pos_x, float pos_y);
	bool canGroupWith(StarScript *star) const;

	cocos2d::Color4F getColor4F() const;

	float getPositionX() const;
	float getPositionY() const;

	void setVisible(bool visible);

	bool isSelected() const;
	void setSelected(bool selected);

	int getRowNum() const;
	void setRowNum(int row_num);
	int getColNum() const;
	void setColNum(int col_num);

	virtual const std::string & getType() const override;

	const static int WIDTH = 48;
	const static int HEIGHT = 48;

	//Disable copy/move constructor and operator=.
	StarScript(const StarScript&) = delete;
	StarScript(StarScript&&) = delete;
	StarScript& operator=(const StarScript&) = delete;
	StarScript& operator=(StarScript&&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct StarScriptImpl;
	std::unique_ptr<StarScriptImpl> pimpl;
};

#endif // !__STAR_SCRIPT__
