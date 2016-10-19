#include "ChessAI.h"
#include "SceneGame.h"

ChessAI::ChessAI()
{
}

ChessAI::~ChessAI() 
{
}

// ��������
const int MAX_GEN_MOVES = 128; // ���������߷���
const int LIMIT_DEPTH = 32;    // �����������
const int MATE_VALUE = 10000;  // ��߷�ֵ���������ķ�ֵ
const int WIN_VALUE = MATE_VALUE - 100; // ������ʤ���ķ�ֵ���ޣ�������ֵ��˵���Ѿ�������ɱ����
const int ADVANCED_VALUE = 3;  // ����Ȩ��ֵ

// ����λ�ü�ֵ��
static const int cucvlPiecePos[7][90] = {
	{ // ˧(��)	
	0,  0,  0,  0,  0,  0,  0,  0,  0, 
	0,  0,  0,  0,  0,  0,  0,  0,  0, 
	0,  0,  0,  0,  0,  0,  0,  0,  0, 
	0,  0,  0,  0,  0,  0,  0,  0,  0, 
	0,  0,  0,  0,  0,  0,  0,  0,  0, 
	0,  0,  0,  0,  0,  0,  0,  0,  0, 
	0,  0,  0,  0,  0,  0,  0,  0,  0, 
	0,  0,  0,  1,  1,  1,  0,  0,  0, 
	0,  0,  0,  2,  2,  2,  0,  0,  0, 
	0,  0,  0, 11, 15, 11,  0,  0,  0
	},
	{ // ��(ʿ)
	 0,  0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0, 20,  0, 20,  0,  0,  0,
	 0,  0,  0,  0, 23,  0,  0,  0,  0,
	 0,  0,  0, 20,  0, 20,  0,  0,  0
	},
	{ // ��(��)
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	  0,  0, 20,  0,  0,  0, 20,  0,  0,  
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	 18,  0,  0,  0, 23,  0,  0,  0, 18,  
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	  0,  0, 20,  0,  0,  0, 20,  0,  0 
	
	},
	{ // ��
	206,208,207,213,214,213,207,208,206,
	206,212,209,216,233,216,209,212,206,
	206,208,207,214,216,214,207,208,206,
	206,213,213,216,216,216,213,213,206,
	208,211,211,214,215,214,211,211,208,
	208,212,212,214,215,214,212,212,208,
	204,209,204,212,214,212,204,209,204,
	198,208,204,212,212,212,204,208,198,
	200,208,206,212,200,212,206,208,200,
	194,206,204,212,200,212,204,206,194
	},
	{ // ��
	90, 90, 90, 96, 90, 96, 90, 90, 90,  
	90, 96,103, 97, 94, 97,103, 96, 90,  
	92, 98, 99,103, 99,103, 99, 98, 92,  
	93,108,100,107,100,107,100,108, 93,  
	90,100, 99,103,104,103, 99,100, 90,  
	90, 98,101,102,103,102,101, 98, 90,  
	92, 94, 98, 95, 98, 95, 98, 94, 92,  
	93, 92, 94, 95, 92, 95, 94, 92, 93,  
	85, 90, 92, 93, 78, 93, 92, 90, 85,  
	88, 85, 90, 88, 90, 88, 90, 85, 88 
	},
	{ // ��
	100,100, 96, 91, 90, 91, 96,100,100,  
	 98, 98, 96, 92, 89, 92, 96, 98, 98,  
	 97, 97, 96, 91, 92, 91, 96, 97, 97,  
	 96, 99, 99, 98,100, 98, 99, 99, 96,  
	 96, 96, 96, 96,100, 96, 96, 96, 96,  
	 95, 96, 99, 96,100, 96, 99, 96, 95,  
	 96, 96, 96, 96, 96, 96, 96, 96, 96,  
	 97, 96,100, 99,101, 99,100, 96, 97,  
	 96, 97, 98, 98, 98, 98, 98, 97, 96,  
	 96, 96, 97, 99, 99, 99, 97, 96, 96  
	},
	{ // ��(��)	
	  9,  9,  9, 11, 13, 11,  9,  9,  9,  
	 19, 24, 34, 42, 44, 42, 34, 24, 19,  
	 19, 24, 32, 37, 37, 37, 32, 24, 19,  
	 19, 23, 27, 29, 30, 29, 27, 23, 19,  
	 14, 18, 20, 27, 29, 27, 20, 18, 14,  
	  7,  0, 13,  0, 16,  0, 13,  0,  7,  
	  7,  0,  7,  0, 15,  0,  7,  0,  7,  
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	  0,  0,  0,  0,  0,  0,  0,  0,  0  
	}
};

int ChessAI::GetRedValue() {
	int _Value = 0;
	for (int idx = 0; idx < 16; idx++) {
		Stone* _CurStone = SceneGame::GetInstance()->_s[idx];
		if (!_CurStone->getDead()) {
			int _type = _CurStone->getType();
			int _PosIndex = 0;
			if (SceneGame::GetInstance()->_redSide) {
				//���ִ����,��ľ�������
				_PosIndex = _CurStone->getX() + (9 - _CurStone->getY()) * 9;
			}
			else {
				//���ִ����,��ľ�������
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
				//���ִ����,�ڵľ�������
				_PosIndex = _CurStone->getX() + _CurStone->getY() * 9;
			}
			else {
				//���ִ����,�ڵľ�������
				_PosIndex = _CurStone->getX() + (9 - _CurStone->getY()) * 9;
			}
			_Value += cucvlPiecePos[_type][_PosIndex];
		}
	}
	return _Value;
}


struct StonePos {
	int SrcX, SrcY, SrcID;           // �졢��˫����������ֵ
	int DstX, DstY, DstID;
	int offX,offY;
};
StonePos mvBest;

// ����ṹ
struct PositionStruct {
	int sdPlayer;                   // �ֵ�˭�ߣ�0=�췽��1=�ڷ�
	BYTE ucpcSquares[256];          // �����ϵ�����
	int vlWhite, vlBlack;           // �졢��˫����������ֵ
	int nDistance;                  // ������ڵ�Ĳ���
	
	int Evaluate(void) const {      // �������ۺ���
		return (SceneGame::GetInstance()->_redTrun == true ? vlWhite - vlBlack : vlBlack - vlWhite) + ADVANCED_VALUE;
	}
	int GenerateMoves(std::vector<StonePos> &StoneCanMoveList) const;          // ���������߷�
};

static PositionStruct pos; // ����ʵ��

						   // ����ṹ

// �������йص�ȫ�ֱ���
static struct {
	int mvResult;             // �����ߵ���
	int nHistoryTable[65536]; // ��ʷ��
} Search;

// "qsort"����ʷ������ıȽϺ���
static int CompareHistory(const void *lpmv1, const void *lpmv2) {
	return Search.nHistoryTable[*(int *)lpmv2] - Search.nHistoryTable[*(int *)lpmv1];
}

// ���������߷�
int PositionStruct::GenerateMoves(std::vector<StonePos> &StoneCanMoveList) const {
		// ���������߷�����Ҫ�������¼������裺
	int index = 0;
	int indexLimit = 0;
	int KillIdMin = 0;
	int KillIdMax = 0;
	int nGenMoves = 0;
	if (SceneGame::GetInstance()->_redTrun == true) {
		//�������ߵ�
		index = 0;
		indexLimit = 16;
		KillIdMin = 16;
		KillIdMax = 32; 
	}
	else {
		//�������ߵ�
		index = 16;
		indexLimit = 32;
		KillIdMin = 0;
		KillIdMax = 16;
	}
	for (; index < indexLimit; index++) {
		Stone* _CurStone = SceneGame::GetInstance()->_s[index];
		// 1. �ҵ�һ���������ӣ����������жϣ�
		if (!SceneGame::GetInstance()->_s[index]->getDead()) {//������Ҫһ�������ж���
				//һ����80�����꣬��������
			for (int _ReadyMoveX = 0; _ReadyMoveX <= 8; _ReadyMoveX++) {
				for (int _ReadyMoveY = 0; _ReadyMoveY <= 9; _ReadyMoveY++) {
					int Offx = _ReadyMoveX - _CurStone->getX();
					int Offy = _ReadyMoveY - _CurStone->getY();
					int _KillID = SceneGame::GetInstance()->getStone(_ReadyMoveX, _ReadyMoveY);
					if (((_KillID <= KillIdMax)&&(_KillID >= KillIdMin)) || _KillID == -1) {
						//��ɱ�Է��ģ����ƶ�����λ�ã����ǲ���ɱ�Լ�
						if (SceneGame::GetInstance()->canMove(_CurStone->getID(), _KillID, _ReadyMoveX, _ReadyMoveY)) {
							//������ƶ��ͼ�¼����,����ֱ�Ӿ����Լ��Ľṹ������
							//IDֵ�����±꣬����ֱ��ȥλ��
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
							//����һ�������߶��ٲ�
							nGenMoves++;
						}
					}
				}
			}
		}
	}
	return nGenMoves;

}

// �����߽�(Fail-Soft)��Alpha-Beta��������
int ChessAI::SearchFull(int vlAlpha, int vlBeta, int nDepth) {
	int i, nGenMoves, pcCaptured;
	int vl, vlBest;
	std::vector<StonePos> StoneCanMoveList;
	// һ��Alpha-Beta��ȫ������Ϊ���¼����׶�

	// 1. ����ˮƽ�ߣ��򷵻ؾ�������ֵ
	if (nDepth == 0) {
		return pos.Evaluate();
	}

	// 2. ��ʼ�����ֵ������߷�
	vlBest = -MATE_VALUE; // ��������֪�����Ƿ�һ���߷���û�߹�(ɱ��)

	// 3. ����ȫ���߷�
	nGenMoves = pos.GenerateMoves(StoneCanMoveList);
	//qsort(mvs, nGenMoves, sizeof(int), CompareHistory);

	// 4. ��һ����Щ�߷��������еݹ�
	for (i = 0; i < nGenMoves; i++) {//�𲽷���
		Step* step = Step::create(StoneCanMoveList[i].SrcID, StoneCanMoveList[i].DstID, StoneCanMoveList[i].SrcX, StoneCanMoveList[i].SrcY, StoneCanMoveList[i].DstX, StoneCanMoveList[i].DstY);
		//�����ӵ���Ϣ��ӵ�������
		SceneGame::GetInstance()->_steps->addObject(step);
		SceneGame::GetInstance()->_s[StoneCanMoveList[i].SrcID]->setX(StoneCanMoveList[i].DstX);
		SceneGame::GetInstance()->_s[StoneCanMoveList[i].SrcID]->setY(StoneCanMoveList[i].DstY);
		//���Ҫ�ƶ�����������
		if (-1 != StoneCanMoveList[i].DstID){
			//ɱ���ƶ����ϵ�����
			SceneGame::GetInstance()->_s[StoneCanMoveList[i].DstID]->setDead(true);
		}
		//�ƶ���һ�����
		//�л��ƶ������ӵ���ɫ
		SceneGame::GetInstance()->_redTrun = !SceneGame::GetInstance()->_redTrun;

		//�ƶ�����Ҫ����Ŀǰ����������ӵļ�ֵ�ܺ�
		pos.vlWhite = GetRedValue();
		pos.vlBlack = GetBlackValue();

		vl = -SearchFull(-vlBeta, -vlAlpha, nDepth - 1);
		SceneGame::GetInstance()->StoneBack();

		// 5. ����Alpha-Beta��С�жϺͽض�
		if (vl > vlBest) {    // �ҵ����ֵ(������ȷ����Alpha��PV����Beta�߷�)
			vlBest = vl;        // "vlBest"����ĿǰҪ���ص����ֵ�����ܳ���Alpha-Beta�߽�
			if (vl >= vlBeta) { // �ҵ�һ��Beta�߷�
				mvBest = StoneCanMoveList[i];  // Beta�߷�Ҫ���浽��ʷ��
				break;            // Beta�ض�
			}
			if (vl > vlAlpha) { // �ҵ�һ��PV�߷�
				mvBest = StoneCanMoveList[i];  // PV�߷�Ҫ���浽��ʷ��
				vlAlpha = vl;     // ��СAlpha-Beta�߽�
			}
		}
	}

	// 5. �����߷����������ˣ�������߷�(������Alpha�߷�)���浽��ʷ���������ֵ
	if (vlBest == -MATE_VALUE) {
		// �����ɱ�壬�͸���ɱ�岽����������
		return pos.nDistance - MATE_VALUE;
	}
//	if (mvBest.SrcX != 0 && mvBest.SrcY != 0 && mvBest.DstX != 0 && mvBest.DstY != 0) {
		// �������Alpha�߷����ͽ�����߷����浽��ʷ��
// 		Search.nHistoryTable[mvBest] += nDepth * nDepth;
// 		if (pos.nDistance == 0) {
// 			// �������ڵ�ʱ��������һ������߷�(��Ϊȫ�����������ᳬ���߽�)��������߷���������
// 			Search.mvResult = mvBest;
// 		}
//	}
	return vlBest;
}

// ����������������
void ChessAI::SearchMain() {
	int i, t, vl;

	// ��ʼ��
	memset(Search.nHistoryTable, 0, 65536 * sizeof(int)); // �����ʷ��
	t = clock();       // ��ʼ����ʱ��
	pos.nDistance = 0; // ��ʼ����

					   // �����������
	for (i = 1; i <= LIMIT_DEPTH; i++) {
		vl = SearchFull(-MATE_VALUE, MATE_VALUE, i);
		// ������ɱ�壬����ֹ����
		if (vl > WIN_VALUE || vl < -WIN_VALUE) {
			break;
		}
		// ����һ�룬����ֹ����
		if (clock() - t > CLOCKS_PER_SEC) {
			break;
		}
	}
}

// ���Ի�Ӧһ����
 void ChessAI::ResponseMove() {
	int pcCaptured;
	// ������һ����
	SearchMain();
	//֪ͨ�������ƶ�����
	SceneGame::GetInstance()->moveStone(mvBest.SrcID, mvBest.DstID, mvBest.DstX, mvBest.DstY);
	//pos.MakeMove(Search.mvResult, pcCaptured);
}


