#include "ChessAI.h"
#include "SceneGame.h"

// 局面结构类
struct PositionStruct {
	int sdPlayer;                   // 轮到谁走，0=红方，1=黑方
	BYTE ucpcSquares[256];          // 棋盘上的棋子
	int vlWhite, vlBlack;           // 红、黑双方的子力价值
	int nDistance;                  // 距离根节点的步数

	int Evaluate(void) const {      // 局面评价函数
		return (SceneGame::GetInstance()->_redTrun == true ? vlWhite - vlBlack : vlBlack - vlWhite) + ADVANCED_VALUE;
	}
	int GenerateMoves(std::vector<StonePos> &StoneCanMoveList) const;          // 生成所有走法
};

// 生成所有走法
int PositionStruct::GenerateMoves(std::vector<StonePos> &StoneCanMoveList) const {
	// 生成所有走法，需要经过以下几个步骤：
	int index = 0;
	int indexLimit = 0;
	int KillIdMin = 0;
	int KillIdMax = 0;
	int nGenMoves = 0;
	if (SceneGame::GetInstance()->_redTrun == true) {
		//到红棋走的
		index = 0;
		indexLimit = 16;
		KillIdMin = 16;
		KillIdMax = 32;
	}
	else {
		//到黑棋走的
		index = 16;
		indexLimit = 32;
		KillIdMin = 0;
		KillIdMax = 16;
	}
	for (; index < indexLimit; index++) {
		Stone* _CurStone = SceneGame::GetInstance()->_s[index];
		// 1. 找到一个本方棋子，再做以下判断：
		if (!SceneGame::GetInstance()->_s[index]->getDead()) {//这里需要一个变量判断下
			//一共就80个坐标，暴力遍历
			for (int _ReadyMoveX = 0; _ReadyMoveX <= 8; _ReadyMoveX++) {
				for (int _ReadyMoveY = 0; _ReadyMoveY <= 9; _ReadyMoveY++) {
					int Offx = _ReadyMoveX - _CurStone->getX();
					int Offy = _ReadyMoveY - _CurStone->getY();
					int _KillID = SceneGame::GetInstance()->getStone(_ReadyMoveX, _ReadyMoveY);
					if (((_KillID <= KillIdMax) && (_KillID >= KillIdMin)) || _KillID == -1) {
						//能杀对方的，能移动到空位置，但是不能杀自己
						if (SceneGame::GetInstance()->canMove(_CurStone->getID(), _KillID, _ReadyMoveX, _ReadyMoveY)) {
							//如果能移动就记录下来,这里直接就用自己的结构，方便
							//ID值就是下标，可以直接去位置
							StonePos _StonePos;
							_StonePos.SrcX = _CurStone->getX();
							_StonePos.SrcY = _CurStone->getY();
							_StonePos.SrcID = _CurStone->getID();
							_StonePos.DstX = _ReadyMoveX;
							_StonePos.DstY = _ReadyMoveY;
							_StonePos.DstID = _KillID;
							_StonePos.offX = Offx;
							_StonePos.offY = Offy;
							StoneCanMoveList.push_back(_StonePos);
							//mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							//返回一共可以走多少步
							nGenMoves++;
						}
					}
				}
			}
		}
	}
	return nGenMoves;
};

//棋局局面
PositionStruct pos;

int ChessAI::GetRedValue() {
	int _Value = 0;
	for (int idx = 0; idx < 16; idx++) {
		Stone* _CurStone = SceneGame::GetInstance()->_s[idx];
		if (!_CurStone->getDead()) {
			int _type = _CurStone->getType();
			int _PosIndex = 0;
			if (SceneGame::GetInstance()->_redSide) {
				//玩家执红子,红的就在下面
				_PosIndex = _CurStone->getX() + (9 - _CurStone->getY()) * 9;
			}
			else {
				//玩家执黑子,红的就在上面
				_PosIndex = _CurStone->getX() + _CurStone->getY() * 9;
			}
			_Value += cucvlPiecePos[_type][_PosIndex];
		}
	}
	return _Value;
}


int ChessAI::GetBlackValue() {
	int _Value = 0;
	for (int idx = 16; idx < 32; idx++) {
		Stone* _CurStone = SceneGame::GetInstance()->_s[idx];
		if (!_CurStone->getDead()) {
			int _type = _CurStone->getType();
			int _PosIndex = 0;
			if (SceneGame::GetInstance()->_redSide) {
				//玩家执红子,黑的就在上面
				_PosIndex = _CurStone->getX() + _CurStone->getY() * 9;
			}
			else {
				//玩家执黑子,黑的就在下面
				_PosIndex = _CurStone->getX() + (9 - _CurStone->getY()) * 9;
			}
			_Value += cucvlPiecePos[_type][_PosIndex];
		}
	}
	return _Value;
}
						   // 局面结构
/*
// 与搜索有关的全局变量
static struct {
	int mvResult;             // 电脑走的棋
	int nHistoryTable[65536]; // 历史表
} Search;

// "qsort"按历史表排序的比较函数
static int CompareHistory(const void *lpmv1, const void *lpmv2) {
	return Search.nHistoryTable[*(int *)lpmv2] - Search.nHistoryTable[*(int *)lpmv1];
}*/


// 超出边界(Fail-Soft)的Alpha-Beta搜索过程
int ChessAI::SearchFull(int vlAlpha, int vlBeta, int nDepth) {
	int i, nGenMoves, pcCaptured;
	int vl, vlBest;
	std::vector<StonePos> StoneCanMoveList;

	StonePos mvBest;//20161020 xuxp add
	// 一个Alpha-Beta完全搜索分为以下几个阶段

	// 1. 到达水平线，则返回局面评价值
	if (nDepth == 0) {
		return pos.Evaluate();
	}

	// 2. 初始化最佳值和最佳走法
	vlBest = -MATE_VALUE; // 这样可以知道，是否一个走法都没走过(杀棋)

	// 3. 生成全部走法
	nGenMoves = pos.GenerateMoves(StoneCanMoveList);
	//qsort(mvs, nGenMoves, sizeof(int), CompareHistory);

	// 4. 逐一走这些走法，并进行递归
	for (i = 0; i < nGenMoves; i++) {//逐步分析
		Step* step = Step::create(StoneCanMoveList[i].SrcID, StoneCanMoveList[i].DstID, StoneCanMoveList[i].SrcX, StoneCanMoveList[i].SrcY, StoneCanMoveList[i].DstX, StoneCanMoveList[i].DstY);
		//将棋子的信息添加到数组中
		SceneGame::GetInstance()->_steps->addObject(step);
		SceneGame::GetInstance()->_s[StoneCanMoveList[i].SrcID]->setX(StoneCanMoveList[i].DstX);
		SceneGame::GetInstance()->_s[StoneCanMoveList[i].SrcID]->setY(StoneCanMoveList[i].DstY);
		//如果要移动点上有棋子
		if (-1 != StoneCanMoveList[i].DstID){
			//杀掉移动点上的棋子
			SceneGame::GetInstance()->_s[StoneCanMoveList[i].DstID]->setDead(true);
		}
		//移动了一步棋后
		//切换移动的棋子的颜色
		SceneGame::GetInstance()->_redTrun = !SceneGame::GetInstance()->_redTrun;

		//移动完了要算下目前的棋面的棋子的价值总和
		pos.vlWhite = GetRedValue();
		pos.vlBlack = GetBlackValue();

		vl = -SearchFull(-vlBeta, -vlAlpha, nDepth - 1);
		SceneGame::GetInstance()->StoneBack();

		// 5. 进行Alpha-Beta大小判断和截断
		if (vl > vlBest) {    // 找到最佳值(但不能确定是Alpha、PV还是Beta走法)
			vlBest = vl;        // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界
			if (vl >= vlBeta) { // 找到一个Beta走法
				mvBest = StoneCanMoveList[i];  // Beta走法要保存到历史表
				break;            // Beta截断
			}
			if (vl > vlAlpha) { // 找到一个PV走法
				mvBest = StoneCanMoveList[i];  // PV走法要保存到历史表
				vlAlpha = vl;     // 缩小Alpha-Beta边界
			}
		}
	}

	// 5. 所有走法都搜索完了，把最佳走法(不能是Alpha走法)保存到历史表，返回最佳值
	if (vlBest == -MATE_VALUE) {
		// 如果是杀棋，就根据杀棋步数给出评价
		return pos.nDistance - MATE_VALUE;
	}
//	if (mvBest.SrcX != 0 && mvBest.SrcY != 0 && mvBest.DstX != 0 && mvBest.DstY != 0) {
		// 如果不是Alpha走法，就将最佳走法保存到历史表
// 		Search.nHistoryTable[mvBest] += nDepth * nDepth;
// 		if (pos.nDistance == 0) {
// 			// 搜索根节点时，总是有一个最佳走法(因为全窗口搜索不会超出边界)，将这个走法保存下来
// 			Search.mvResult = mvBest;
// 		}
//	}
	if (nDepth == this->SearchDepth)
	{
		mvBestResult = mvBest;
	}
	return vlBest;
}

// 迭代加深搜索过程
void ChessAI::SearchMain() {
	int i, t, vl;

	// 初始化
	//memset(Search.nHistoryTable, 0, 65536 * sizeof(int)); // 清空历史表
	t = clock();       // 初始化定时器
	pos.nDistance = 0; // 初始步数

					   // 迭代加深过程
	for (i = 1; i <= LIMIT_DEPTH; i++) {
		this->SearchDepth = i;
		vl = SearchFull(-MATE_VALUE, MATE_VALUE, i);
		// 搜索到杀棋，就终止搜索
		if (vl > WIN_VALUE || vl < -WIN_VALUE) {
			break;
		}
		// 超过一秒，就终止搜索
		if (clock() - t > CLOCKS_PER_SEC) {
			break;
		}
	}
}

// 电脑回应一步棋
 void ChessAI::ResponseMove() {
	int pcCaptured;
	// 电脑走一步棋
	SearchMain();
	//通知主场景移动棋子
	SceneGame::GetInstance()->moveStone(mvBestResult.SrcID, mvBestResult.DstID, mvBestResult.DstX, mvBestResult.DstY);
	//pos.MakeMove(Search.mvResult, pcCaptured);
}
