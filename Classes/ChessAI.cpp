#include "ChessAI.h"
#include "SceneGame.h"

// ����ṹ��
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
					if (((_KillID <= KillIdMax) && (_KillID >= KillIdMin)) || _KillID == -1) {
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
};

//��־���
PositionStruct pos;

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
						   // ����ṹ
/*
// �������йص�ȫ�ֱ���
static struct {
	int mvResult;             // �����ߵ���
	int nHistoryTable[65536]; // ��ʷ��
} Search;

// "qsort"����ʷ������ıȽϺ���
static int CompareHistory(const void *lpmv1, const void *lpmv2) {
	return Search.nHistoryTable[*(int *)lpmv2] - Search.nHistoryTable[*(int *)lpmv1];
}*/


// �����߽�(Fail-Soft)��Alpha-Beta��������
int ChessAI::SearchFull(int vlAlpha, int vlBeta, int nDepth) {
	int i, nGenMoves, pcCaptured;
	int vl, vlBest;
	std::vector<StonePos> StoneCanMoveList;

	StonePos mvBest;//20161020 xuxp add
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
	if (nDepth == this->SearchDepth)
	{
		mvBestResult = mvBest;
	}
	return vlBest;
}

// ����������������
void ChessAI::SearchMain() {
	int i, t, vl;

	// ��ʼ��
	//memset(Search.nHistoryTable, 0, 65536 * sizeof(int)); // �����ʷ��
	t = clock();       // ��ʼ����ʱ��
	pos.nDistance = 0; // ��ʼ����

					   // �����������
	for (i = 1; i <= LIMIT_DEPTH; i++) {
		this->SearchDepth = i;
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
	SceneGame::GetInstance()->moveStone(mvBestResult.SrcID, mvBestResult.DstID, mvBestResult.DstX, mvBestResult.DstY);
	//pos.MakeMove(Search.mvResult, pcCaptured);
}
