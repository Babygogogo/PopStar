#ifndef _FLOATWORD_H_
#define _FLOATWORD_H_
#include "cocos2d.h"

class FloatWord : public cocos2d::Node{
public:
	static FloatWord* create(const std::string& word,const int fontSize,cocos2d::Point begin);
	bool init(const std::string& word,const int fontSize,cocos2d::Point begin);
	void floatIn(const float delay,std::function<void()> callback);
	void floatOut(const float delay,std::function<void()> callback);
	void floatInOut(const float speed,const float delay,std::function<void()> callback);

private:
	int _fontSize;
	cocos2d::Point _begin;
	cocos2d::Label* _label;
};
#endif