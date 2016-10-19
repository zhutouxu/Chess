#pragma once
#ifndef  _ChessAI_H_
#define  _ChessAI_H_

#include "cocos2d.h"
USING_NS_CC;


//������
class ChessAI
{
public:

	ChessAI();
	~ChessAI();
	int SearchFull(int vlAlpha, int vlBeta, int nDepth);
	void SearchMain();
	void ResponseMove();
	int GetRedValue();
	int GetBlackValue();
private:

};

#endif
