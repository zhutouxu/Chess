#pragma once
#ifndef  _ChessAI_H_
#define  _ChessAI_H_

#include "cocos2d.h"
#include "AICommon.h"
USING_NS_CC;

//棋子类
class ChessAI
{
public:
	ChessAI(){ };
	~ChessAI(){ };
	int SearchFull(int vlAlpha, int vlBeta, int nDepth);
	void SearchMain();
	void ResponseMove();
	int GetRedValue();
	int GetBlackValue();
private:
	//搜索深度
	int SearchDepth;//20161020 xuxp add
	//最佳走步
	StonePos mvBestResult; //20161020 xuxp add
};

#endif
