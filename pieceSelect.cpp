//=============================================================================
//
// �s�[�X�Z���N�g���� [pieceSelect.cpp]
// Author : Ryouma Inoue
//
//=============================================================================
#include "pieceSelect.h"
#include "manager.h"
#include "renderer.h"
#include "inputKeyboard.h"
#include "inputController.h"
#include "takaseiLibrary.h"
#include "box.h"
#include "piece.h"

//=============================================================================
// �}�N����`
//=============================================================================
#define	PIECE_01 100		// ��01�̃��f�����A�h���X
#define PIECE_FILE "data/text/piece.txt"

//=============================================================================
// �ÓI�����o�ϐ��̏�����
//=============================================================================
bool CPieceSelect::m_bPuzzle[Box_Depth][Box_Width] = {};

//==============================================================================
// �R���X�g���N�^
//==============================================================================
CPieceSelect::CPieceSelect(PRIORITY type = CScene::PRIORITY_UI) :CScene(type)
{
	SetObjType(PRIORITY_BG);	//�I�u�W�F�N�g�^�C�v�̐ݒ�

}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
CPieceSelect::~CPieceSelect()
{

}

//==============================================================================
// ����������
//==============================================================================
HRESULT CPieceSelect::Init(void)
{
	// ������
	m_nSelectCnt = 0;
	m_nPieceNum = 0;
	m_bPiece = false;
	m_bPlacement = false;

	m_pPieceExplanation = CScene2D::Create(PRIORITY_UI);
	m_pPieceExplanation->BindTexture("data/tex/piece.png");					// �e�N�X�`���̃|�C���^��n��
	m_pPieceExplanation->SetSize(D3DXVECTOR3(500.0f, 200.0f, 0.0f));		// �傫���̐ݒ�
	m_pPieceExplanation->SetPosition(D3DXVECTOR3(1080.0f, 150.0f, 0.0f));	// �ʒu�̐ݒ�
	m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 0);
	m_pPieceExplanation->SetTransform();
	for (int nCnt = 0; nCnt < MAX_CORE; nCnt++)
	{
		m_pPieceSelect[nCnt] = CScene2D::Create(PRIORITY_UI);
		m_bSelect[nCnt] = false;
	}
	for (int nCnt = 0; nCnt < Piece_Num; nCnt++)
	{
		m_fSpeed[nCnt] = 0;
		m_bRoute[nCnt] = false;
	}
	// �u���b�N�̏�����
	for (int nCntDepth = 0; nCntDepth < Box_Depth; nCntDepth++)
	{
		for (int nCntWidth = 0; nCntWidth < Box_Width; nCntWidth++)
		{
			// �p�Y��������
			m_bPuzzle[nCntDepth][nCntWidth] = false;
			// �i�[�p
			m_bPuzzleStorage[nCntDepth][nCntWidth] = false;
		}
	}

	// �ŏ��̃s�[�X����
	m_pPiece[m_nPieceNum] = CPiece::Create();
	m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);


	LoadPiece();
	//SetPiece(PIECETYPE_CORE_00, D3DXVECTOR3(500.0f, 500.0f, 0.0f), D3DXVECTOR3(100.0f, 100.0f, 0.0f), "data/tex/core.png");

	return S_OK;
}

//=============================================================================
// �J������
//=============================================================================
void CPieceSelect::Uninit(void)
{

}

//=============================================================================
// �X�V����
//=============================================================================
void CPieceSelect::Update(void)
{
	// �L�[�{�[�h�擾
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();
	// �Q�[���p�b�h�̎擾
	CInputController *pGamepad = CManager::GetInputController();

	float nValueH = 0;									//�R���g���[���[
	float nValueV = 0;									//�R���g���[���[

	m_bPiece = CBox::GetPiece();

	if (m_pPiece[m_nPieceNum] != NULL)
	{
		// �z�u���擾
		m_bPiece = m_pPiece[m_nPieceNum]->GetMove();
	}

	// ====================== �R���g���[���[ ====================== //

	if (pGamepad != NULL)
	{// �Q�[���p�b�h�����݂����Ƃ�
		if (pGamepad->GetJoypadUse(0))
		{// �g�p�\�������Ƃ�
			pGamepad->GetJoypadStickLeft(0, &nValueH, &nValueV);

			//// �Q�[���p�b�h����
			//InputGemepad(nValueH, nValueV, fTireRotSpeed, aVec);

			if (m_bPiece == true)
			{
				if (pGamepad->GetControllerTrigger(0, JOYPADKEY_Y))
				{
					// �s�[�X�����Z
					m_nPieceNum++;
					// �s�[�X����
					m_pPiece[m_nPieceNum] = CPiece::Create();
					// �s�[�X�^�C�v�ݒ�
					m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
					m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 0);

					// �z�u���
					m_pPiece[m_nPieceNum]->SetMove(false);
					m_nSelectCnt = 0;

					for (int nCnt = 0; nCnt < PIECETYPE_MAX; nCnt++)
					{
						m_bSelect[nCnt] = false;
					}

					m_bSelect[PIECETYPE_CORE_01] = true;
				}


				// ���ɃX�e�B�b�N���|�ꂽ�Ƃ�
				if (nValueV <= JOY_MAX_RANGE && nValueV > 0)
				{
					// �Z���N�g�J�E���g���Z
					m_nSelect--;
				}
				else if (nValueV >= -JOY_MAX_RANGE && nValueV < 0)
				{// ��ɃX�e�B�b�N���|�ꂽ�Ƃ�
				 // �Z���N�g�J�E���g���Z
					m_nSelect++;
				}
				// �Z���N�g�J�E���g����
				if (m_nSelect >= m_nPieceNum)
				{
					m_nSelect = m_nPieceNum;
				}
				else if (m_nSelect <= 0)
				{
					m_nSelect = 0;
				}
				for (int nCntDepth = 0; nCntDepth < Box_Depth; nCntDepth++)
				{
					for (int nCntWidth = 0; nCntWidth < Box_Width; nCntWidth++)
					{
						// �I������Ă���Ƃ��̐F
						m_pPiece[m_nSelect]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

						if (m_nSelect != m_nPieceNum)
						{
							m_pPiece[m_nSelect + 1]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
						}
						if (m_nSelect != 0)
						{
							m_pPiece[m_nSelect - 1]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
						}

						//// Q�������ꂽ��
						//if (pGamepad->GetControllerTrigger(0, JOYPADKEY_RIGHT_TRIGGER))
						//{
						//	if (m_pPiece[m_nSelect] != NULL)
						//	{
						//		// �I�΂�Ă�s�[�X�̏��i�[
						//		m_bPuzzleStorage[nCntDepth][nCntWidth] = m_pPiece[m_nSelect]->GetPuzzle(nCntDepth, nCntWidth);
						//		// ��Ԕ�r
						//		if (m_bPuzzle[nCntDepth][nCntWidth] == true && m_bPuzzleStorage[nCntDepth][nCntWidth] == true)
						//		{
						//			// ��ԏ�����
						//			m_bPuzzle[nCntDepth][nCntWidth] = false;
						//		}
						//		// �s�[�X��ԕύX
						//		m_pPiece[m_nSelect]->SetRelease(true);
						//		// ��ԕύX
						//		m_bRelease = true;
						//	}
						//}
					}
				}
				if (m_bRelease == true)
				{
					m_nSelect = 0;
					m_bRelease = false;
				}

				for (int nCnt = 0; nCnt < m_nPieceNum; nCnt++)
				{
					m_fSpeed[nCnt] = m_pPiece[nCnt]->GetSpeed();
					m_fRate[nCnt] = m_pPiece[nCnt]->GetRate();												// �X�s�[�h�㏸��
					m_fTurning[nCnt] = m_pPiece[nCnt]->GetTurning();										// ���񑬓x
					m_fDecay[nCnt] = m_pPiece[nCnt]->GetDecay();											// ������
					m_nPower[nCnt] = (int)m_pPiece[nCnt]->GetPower();										// �p���[
					m_bRoute[nCnt] = m_pPiece[nCnt]->GetRoute();
					m_bRank[nCnt] = m_pPiece[nCnt]->GetRanking();
				}


			}

			if (m_bPiece == false)
			{
				// Z�������ꂽ��
				if (pGamepad->GetControllerTrigger(0, JOYPADKEY_RIGHT_SHOULDER))
				{
					switch (m_nSelectCnt)
					{
					case 0:
						m_bSelect[PIECETYPE_CORE_09] = false;
						m_bSelect[PIECETYPE_CORE_01] = true;
						// �^�C�v�ύX
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Route);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 0);
						m_nSelectCnt++;
						break;
					case 1:
						m_bSelect[PIECETYPE_CORE_01] = false;
						m_bSelect[PIECETYPE_CORE_02] = true;
						// �^�C�v�ύX
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Ranking);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 1);
						m_nSelectCnt++;
						break;
					case 2:
						m_bSelect[PIECETYPE_CORE_02] = false;
						m_bSelect[PIECETYPE_CORE_04] = true;
						// �^�C�v�ύX
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 2);
						m_nSelectCnt++;
						break;
					case 3:
						m_bSelect[PIECETYPE_CORE_04] = false;
						m_bSelect[PIECETYPE_CORE_00] = true;
						// �^�C�v�ύX
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Rate_MediumUp);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 3);
						m_nSelectCnt++;
						break;
					case 4:
						m_bSelect[PIECETYPE_CORE_00] = false;
						m_bSelect[PIECETYPE_CORE_05] = true;
						// �^�C�v�ύX
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Speed);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 4);
						m_nSelectCnt++;
						break;
					case 5:
						m_bSelect[PIECETYPE_CORE_05] = false;
						m_bSelect[PIECETYPE_CORE_14] = true;
						// �^�C�v�ύX
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_L_Type);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 5);
						m_nSelectCnt++;
						break;
					case 6:
						m_bSelect[PIECETYPE_CORE_14] = false;
						m_bSelect[PIECETYPE_CORE_15] = true;
						// �^�C�v�ύX
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Speed_1);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 6);
						m_nSelectCnt++;
						break;
					case 7:
						m_bSelect[PIECETYPE_CORE_15] = false;
						m_bSelect[PIECETYPE_CORE_06] = true;
						// �^�C�v�ύX
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_MaxSpeed_MediumUp);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 7);
						m_nSelectCnt++;
						break;
					case 8:
						m_bSelect[PIECETYPE_CORE_06] = false;
						m_bSelect[PIECETYPE_CORE_07] = true;
						// �^�C�v�ύX
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Turning_MediumUp);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 8);
						m_nSelectCnt++;
						break;
					case 9:
						m_bSelect[PIECETYPE_CORE_07] = false;
						m_bSelect[PIECETYPE_CORE_13] = true;
						// �^�C�v�ύX
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_2);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Decay_Down);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 9);
						m_nSelectCnt++;
						break;
					case 10:
						m_bSelect[PIECETYPE_CORE_13] = false;
						m_bSelect[PIECETYPE_CORE_11] = true;
						// �^�C�v�ύX
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Turning_SmallUp);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 10);
						m_nSelectCnt++;
						break;
					case 11:
						m_bSelect[PIECETYPE_CORE_11] = false;
						m_bSelect[PIECETYPE_CORE_10] = true;
						// �^�C�v�ύX
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_MaxSpeed_SmallUp);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 11);
						m_nSelectCnt++;
						break;
					case 12:
						m_bSelect[PIECETYPE_CORE_10] = false;
						m_bSelect[PIECETYPE_CORE_09] = true;
						// �^�C�v�ύX
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Rate_SmallUp);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 12);
						m_nSelectCnt = 0;
						break;
					}
				}

				//// Z�������ꂽ��
				//if (pGamepad->GetControllerTrigger(0, JOYPADKEY_LEFT_SHOULDER))
				//{
				//	switch (m_nSelectCnt)
				//	{
				//	case 0:
				//		m_bSelect[PIECETYPE_CORE_09] = true;
				//		m_bSelect[PIECETYPE_CORE_01] = false;
				//		// �^�C�v�ύX
				//		m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
				//		m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Route);
				//		m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 0);
				//		m_nSelectCnt = 12;
				//		break;
				//	case 1:
				//		m_bSelect[PIECETYPE_CORE_01] = true;
				//		m_bSelect[PIECETYPE_CORE_02] = false;
				//		// �^�C�v�ύX
				//		m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
				//		m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Ranking);
				//		m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 1);
				//		m_nSelectCnt--;
				//		break;
				//	case 2:
				//		m_bSelect[PIECETYPE_CORE_02] = true;
				//		m_bSelect[PIECETYPE_CORE_04] = false;
				//		// �^�C�v�ύX
				//		m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle);
				//		m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 2);
				//		m_nSelectCnt--;
				//		break;
				//	case 3:
				//		m_bSelect[PIECETYPE_CORE_04] = true;
				//		m_bSelect[PIECETYPE_CORE_00] = false;
				//		// �^�C�v�ύX
				//		m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
				//		m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Rate_MediumUp);
				//		m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 3);
				//		m_nSelectCnt--;
				//		break;
				//	case 4:
				//		m_bSelect[PIECETYPE_CORE_00] = true;
				//		m_bSelect[PIECETYPE_CORE_05] = false;
				//		// �^�C�v�ύX
				//		m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Speed);
				//		m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 4);
				//		m_nSelectCnt--;
				//		break;
				//	case 5:
				//		m_bSelect[PIECETYPE_CORE_05] = true;
				//		m_bSelect[PIECETYPE_CORE_14] = false;
				//		// �^�C�v�ύX
				//		m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_L_Type);
				//		m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 5);
				//		m_nSelectCnt--;
				//		break;
				//	case 6:
				//		m_bSelect[PIECETYPE_CORE_14] = true;
				//		m_bSelect[PIECETYPE_CORE_15] = false;
				//		// �^�C�v�ύX
				//		m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Speed_1);
				//		m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 6);
				//		m_nSelectCnt--;
				//		break;
				//	case 7:
				//		m_bSelect[PIECETYPE_CORE_15] = true;
				//		m_bSelect[PIECETYPE_CORE_06] = false;
				//		// �^�C�v�ύX
				//		m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
				//		m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_MaxSpeed_MediumUp);
				//		m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 7);
				//		m_nSelectCnt--;
				//		break;
				//	case 8:
				//		m_bSelect[PIECETYPE_CORE_06] = true;
				//		m_bSelect[PIECETYPE_CORE_07] = false;
				//		// �^�C�v�ύX
				//		m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
				//		m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Turning_MediumUp);
				//		m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 8);
				//		m_nSelectCnt--;
				//		break;
				//	case 9:
				//		m_bSelect[PIECETYPE_CORE_07] = true;
				//		m_bSelect[PIECETYPE_CORE_13] = false;
				//		// �^�C�v�ύX
				//		m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_2);
				//		m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Decay_Down);
				//		m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 9);
				//		m_nSelectCnt--;
				//		break;
				//	case 10:
				//		m_bSelect[PIECETYPE_CORE_13] = true;
				//		m_bSelect[PIECETYPE_CORE_11] = false;
				//		// �^�C�v�ύX
				//		m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
				//		m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Turning_SmallUp);
				//		m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 10);
				//		m_nSelectCnt--;
				//		break;
				//	case 11:
				//		m_bSelect[PIECETYPE_CORE_11] = true;
				//		m_bSelect[PIECETYPE_CORE_10] = false;
				//		// �^�C�v�ύX
				//		m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
				//		m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_MaxSpeed_SmallUp);
				//		m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 11);
				//		m_nSelectCnt--;
				//		break;
				//	case 12:
				//		m_bSelect[PIECETYPE_CORE_10] = true;
				//		m_bSelect[PIECETYPE_CORE_09] = false;
				//		// �^�C�v�ύX
				//		m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
				//		m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Rate_SmallUp);
				//		m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 12);
				//		m_nSelectCnt--;
				//		break;
				//	}
				//}
			}

		}
	}
	if (m_bPiece == true)
	{
		// ����
		if (pKeyboard->GetTriggerKeyboard(DIK_C))
		{
			// �s�[�X�����Z
			m_nPieceNum++;
			// �s�[�X����
			m_pPiece[m_nPieceNum] = CPiece::Create();
			// �s�[�X�^�C�v�ݒ�
			m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
			// �z�u���
			m_pPiece[m_nPieceNum]->SetMove(false);
			m_nSelectCnt = 0;

			for (int nCnt = 0; nCnt < PIECETYPE_MAX; nCnt++)
			{
				m_bSelect[nCnt] = false;
			}

			m_bSelect[PIECETYPE_CORE_01] = true;
		}

		// �㉺����
		if (pKeyboard->GetTriggerKeyboard(MOVE_ACCEL))
		{
			// �Z���N�g�J�E���g���Z
			m_nSelect++;
		}
		else if (pKeyboard->GetTriggerKeyboard(MOVE_BRAKE))
		{
			// �Z���N�g�J�E���g���Z
			m_nSelect--;
		}

		// �Z���N�g�J�E���g����
		if (m_nSelect >= m_nPieceNum)
		{
			m_nSelect = m_nPieceNum;
		}
		else if (m_nSelect <= 0)
		{
			m_nSelect = 0;
		}

		for (int nCntDepth = 0; nCntDepth < Box_Depth; nCntDepth++)
		{
			for (int nCntWidth = 0; nCntWidth < Box_Width; nCntWidth++)
			{
				// �I������Ă���Ƃ��̐F
				//m_pPiece[m_nSelect]->SetCol(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));

				if (m_nSelect != m_nPieceNum)
				{
					m_pPiece[m_nSelect + 1]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
				}
				if (m_nSelect != 0)
				{
					m_pPiece[m_nSelect - 1]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
				}

				// Q�������ꂽ��
				if (pKeyboard->GetTriggerKeyboard(DIK_Q))
				{
					if (m_pPiece[m_nSelect] != NULL)
					{
						// �I�΂�Ă�s�[�X�̏��i�[
						m_bPuzzleStorage[nCntDepth][nCntWidth] = m_pPiece[m_nSelect]->GetPuzzle(nCntDepth, nCntWidth);
						// ��Ԕ�r
						if (m_bPuzzle[nCntDepth][nCntWidth] == true && m_bPuzzleStorage[nCntDepth][nCntWidth] == true)
						{
							// ��ԏ�����
							m_bPuzzle[nCntDepth][nCntWidth] = false;
						}
						// �s�[�X��ԕύX
						m_pPiece[m_nSelect]->SetRelease(true);
						// ��ԕύX
						m_bRelease = true;
					}
				}
			}
		}
		if (m_bRelease == true)
		{
			m_nSelect = 0;
			m_bRelease = false;
		}

		for (int nCnt = 0; nCnt < m_nPieceNum; nCnt++)
		{
			m_fSpeed[nCnt] = m_pPiece[nCnt]->GetSpeed();
			m_fRate[nCnt] = m_pPiece[nCnt]->GetRate();												// �X�s�[�h�㏸��
			m_fTurning[nCnt] = m_pPiece[nCnt]->GetTurning();										// ���񑬓x
			m_fDecay[nCnt] = m_pPiece[nCnt]->GetDecay();											// ������
			m_nPower[nCnt] = (int)m_pPiece[nCnt]->GetPower();										// �p���[
			m_bRoute[nCnt] = m_pPiece[nCnt]->GetRoute();
			m_bRank[nCnt] = m_pPiece[nCnt]->GetRanking();
		}
		// �s�[�X����

		//m_pPiece[m_nPieceNum]->SetPlaacement(false);
	}


	if (m_bPiece == false)
	{
		// Z�������ꂽ��
		if (pKeyboard->GetTriggerKeyboard(DIK_Z))
		{
			switch (m_nSelectCnt)
			{
			case 0:
				m_bSelect[PIECETYPE_CORE_09] = false;
				m_bSelect[PIECETYPE_CORE_01] = true;
				// �^�C�v�ύX
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Route);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 0);
				m_nSelectCnt++;
				break;
			case 1:
				m_bSelect[PIECETYPE_CORE_01] = false;
				m_bSelect[PIECETYPE_CORE_02] = true;
				// �^�C�v�ύX
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Ranking);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 1);
				m_nSelectCnt++;
				break;
			case 2:
				m_bSelect[PIECETYPE_CORE_02] = false;
				m_bSelect[PIECETYPE_CORE_04] = true;
				// �^�C�v�ύX
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 2);
				m_nSelectCnt++;
				break;
			case 3:
				m_bSelect[PIECETYPE_CORE_04] = false;
				m_bSelect[PIECETYPE_CORE_00] = true;
				// �^�C�v�ύX
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Rate_MediumUp);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 3);
				m_nSelectCnt++;
				break;
			case 4:
				m_bSelect[PIECETYPE_CORE_00] = false;
				m_bSelect[PIECETYPE_CORE_05] = true;
				// �^�C�v�ύX
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Speed);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 4);
				m_nSelectCnt++;
				break;
			case 5:
				m_bSelect[PIECETYPE_CORE_05] = false;
				m_bSelect[PIECETYPE_CORE_14] = true;
				// �^�C�v�ύX
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_L_Type);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 5);
				m_nSelectCnt++;
				break;
			case 6:
				m_bSelect[PIECETYPE_CORE_14] = false;
				m_bSelect[PIECETYPE_CORE_15] = true;
				// �^�C�v�ύX
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Speed_1);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 6);
				m_nSelectCnt++;
				break;
			case 7:
				m_bSelect[PIECETYPE_CORE_15] = false;
				m_bSelect[PIECETYPE_CORE_06] = true;
				// �^�C�v�ύX
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_MaxSpeed_MediumUp);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 7);
				m_nSelectCnt++;
				break;
			case 8:
				m_bSelect[PIECETYPE_CORE_06] = false;
				m_bSelect[PIECETYPE_CORE_07] = true;
				// �^�C�v�ύX
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Turning_MediumUp);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 8);
				m_nSelectCnt++;
				break;
			case 9:
				m_bSelect[PIECETYPE_CORE_07] = false;
				m_bSelect[PIECETYPE_CORE_13] = true;
				// �^�C�v�ύX
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_2);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Decay_Down);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 9);
				m_nSelectCnt++;
				break;
			case 10:
				m_bSelect[PIECETYPE_CORE_13] = false;
				m_bSelect[PIECETYPE_CORE_11] = true;
				// �^�C�v�ύX
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Turning_SmallUp);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 10);
				m_nSelectCnt++;
				break;
			case 11:
				m_bSelect[PIECETYPE_CORE_11] = false;
				m_bSelect[PIECETYPE_CORE_10] = true;
				// �^�C�v�ύX
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_MaxSpeed_SmallUp);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 11);
				m_nSelectCnt++;
				break;
			case 12:
				m_bSelect[PIECETYPE_CORE_10] = false;
				m_bSelect[PIECETYPE_CORE_09] = true;
				// �^�C�v�ύX
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Rate_SmallUp);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 13), 0, 12);
				m_nSelectCnt = 0;
				break;
			}
		}
	}

	for (int nCnt = 0; nCnt < MAX_CORE; nCnt++)
	{
		if (m_bSelect[nCnt] == true)
		{
			m_pPieceSelect[nCnt]->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
		}
		else
		{
			m_pPieceSelect[nCnt]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	for (int nCntDepth = 0; nCntDepth < Box_Depth; nCntDepth++)
	{
		for (int nCntWidth = 0; nCntWidth < Box_Width; nCntWidth++)
		{
			if (m_pPiece[m_nPieceNum] != NULL)
			{
				// �z�u���擾
				m_bPlacement = m_pPiece[m_nPieceNum]->GetPlaacement();
				// �ݒu����Ă���
				if (m_bPlacement == true)
				{
					// ���i�[
					m_bPuzzleStorage[nCntDepth][nCntWidth] = m_pPiece[m_nPieceNum]->GetPuzzle(nCntDepth, nCntWidth);
				}
			}
			if (m_bPuzzle[nCntDepth][nCntWidth] == false && m_bPuzzleStorage[nCntDepth][nCntWidth] == true)
			{
				// �z�u
				m_bPuzzle[nCntDepth][nCntWidth] = true;
			}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void CPieceSelect::Draw(void)
{

}

//==============================================================================
// ��������
//==============================================================================
CPieceSelect *CPieceSelect::Create(void)
{
	CPieceSelect *pPieceSelect;									// �w�i�̃|�C���^

	pPieceSelect = new CPieceSelect(CScene::PRIORITY_BG);		// �w�i�̐���
	pPieceSelect->Init();										// �w�i�̏�����
	return pPieceSelect;
}

//==============================================================================
// �A�Z�b�g�̐�������
//==============================================================================
HRESULT CPieceSelect::Load(void)
{
	//�e�N�X�`���̓ǂݍ���
	CManager::Load("data/tex/back.png");
	return S_OK;
}

//==============================================================================
// �s�[�X�̐ݒ�
//==============================================================================
void CPieceSelect::SetPiece(PIECETYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 size, std::string Add)
{
	m_pPieceSelect[type]->BindTexture(Add);									// �e�N�X�`���̃|�C���^��n��
	m_pPieceSelect[type]->SetCenter(CScene2D::TEXTUREVTX_CENTER);			// ���S�̐ݒ�
	m_pPieceSelect[type]->SetSize(D3DXVECTOR3(size.x, size.y, 0.0f));		// �傫���̐ݒ�
	m_pPieceSelect[type]->SetPosition(D3DXVECTOR3(pos.x, pos.y, 0.0f));		// �ʒu�̐ݒ�
	m_pPieceSelect[type]->SetTransform();
}

//==============================================================================
// �s�[�X�̃��[�h
//==============================================================================
void CPieceSelect::LoadPiece(void)
{
	FILE *pFile = NULL;																// �t�@�C��
	char cReadText[128];															// ����
	char cHeadText[128];															// ��r
	char cDie[128];
	D3DXVECTOR3 pos;
	D3DXVECTOR3 size;
	char aAdd[64];
	int type;

	pFile = fopen(PIECE_FILE, "r");				// �t�@�C�����J���܂��͍��

	if (pFile != NULL)						//�t�@�C�����ǂݍ��߂��ꍇ
	{
		//�X�N���v�g������܂Ń��[�v
		while (strcmp(cHeadText, "SCRIPT") != 0)
		{
			fgets(cReadText, sizeof(cReadText), pFile);
			sscanf(cReadText, "%s", &cHeadText);
		}

		//�X�N���v�g��������
		if (strcmp(cHeadText, "SCRIPT") == 0)
		{
			//�G���h�X�N���v�g������܂�
			while (strcmp(cHeadText, "END_SCRIPT") != 0)
			{
				fgets(cReadText, sizeof(cReadText), pFile);
				sscanf(cReadText, "%s", &cHeadText);

				//���s
				if (strcmp(cReadText, "\n") != 0)
				{
					if (strcmp(cHeadText, "PIECESET") == 0)
					{//�L�����N�^�[�̏������̂Ƃ�

							//�G���h�L�����N�^�[�Z�b�g������܂Ń��[�v
						while (strcmp(cHeadText, "END_PIECESET") != 0)
						{
							fgets(cReadText, sizeof(cReadText), pFile);
							sscanf(cReadText, "%s", &cHeadText);

							
							if (strcmp(cHeadText, "POS") == 0)
							{//�p�[�c�����̂Ƃ�
								sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
									&pos.x,
									&pos.y,
									&pos.z);
							}
							else if (strcmp(cHeadText, "TYPE") == 0)
							{//�p�[�c�����̂Ƃ�
								sscanf(cReadText, "%s %s %d", &cDie, &cDie,
									&type);
							}
							else if (strcmp(cHeadText, "SIZE") == 0)
							{//�p�[�c�����̂Ƃ�
								sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
									&size.x,
									&size.y,
									&size.z);

							}
							else if (strcmp(cHeadText, "TEX") == 0)
							{//�p�[�c�����̂Ƃ�
								memset(&aAdd, 0, sizeof(aAdd));

								sscanf(cReadText, "%s %s %s", &cDie, &cDie,
									&aAdd);
							}
						}
						SetPiece((PIECETYPE)type, pos, size, aAdd);
					}
				}

			}
		}
		fclose(pFile);				// �t�@�C�������
	}
	else
	{
		MessageBox(NULL, "���̃A�N�Z�X���s�I", "WARNING", MB_ICONWARNING);	// ���b�Z�[�W�{�b�N�X�̐���
	}
}