#ifndef _CHINESEWORD_H_
#define _CHINESEWORD_H_
#include <string>
#include <cocos2d.h>

static cocos2d::ValueVector txt_vec = cocos2d::FileUtils::getInstance()->getValueVectorFromFile("ChineseWords.xml");

std::string ChineseWord(const char* wordId);

#endif