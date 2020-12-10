//=============================================================================
//
// ピースセレクト処理 [pieceSelect.cpp]
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
// マクロ定義
//=============================================================================
#define	PIECE_01 100		// 車01のモデル情報アドレス
#define PIECE_FILE "data/text/piece.txt"

//=============================================================================
// 静的メンバ変数の初期化
//=============================================================================
bool CPieceSelect::m_bPuzzle[Box_Depth][Box_Width] = {};

//==============================================================================
// コンストラクタ
//==============================================================================
CPieceSelect::CPieceSelect(PRIORITY type = CScene::PRIORITY_UI) :CScene(type)
{
	SetObjType(PRIORITY_BG);	//オブジェクトタイプの設定

}

//=============================================================================
// デストラクタ
//=============================================================================
CPieceSelect::~CPieceSelect()
{

}

//==============================================================================
// 初期化処理
//==============================================================================
HRESULT CPieceSelect::Init(void)
{
	// 初期化
	m_nSelectCnt = 0;
	m_nPieceNum = 0;
	m_bPiece = false;
	m_bPlacement = false;


	m_pPieceExplanation = CScene2D::Create(PRIORITY_UI);
	m_pPieceExplanation->BindTexture("data/tex/piece.png");					// テクスチャのポインタを渡す
	m_pPieceExplanation->SetSize(D3DXVECTOR3(500.0f, 200.0f, 0.0f));		// 大きさの設定
	m_pPieceExplanation->SetPosition(D3DXVECTOR3(1080.0f, 150.0f, 0.0f));	// 位置の設定
	m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 0);
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
	// ブロックの初期化
	for (int nCntDepth = 0; nCntDepth < Box_Depth; nCntDepth++)
	{
		for (int nCntWidth = 0; nCntWidth < Box_Width; nCntWidth++)
		{
			// パズル初期化
			m_bPuzzle[nCntDepth][nCntWidth] = false;
			// 格納用
			m_bPuzzleStorage[nCntDepth][nCntWidth] = false;
		}
	}

	// 最初のピース生成
	m_pPiece[m_nPieceNum] = CPiece::Create();
	m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);


	LoadPiece();
	//SetPiece(PIECETYPE_CORE_00, D3DXVECTOR3(500.0f, 500.0f, 0.0f), D3DXVECTOR3(100.0f, 100.0f, 0.0f), "data/tex/core.png");

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CPieceSelect::Uninit(void)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CPieceSelect::Update(void)
{
	// キーボード取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();
	// ゲームパッドの取得
	CInputController *pGamepad = CManager::GetInputController();

	float nValueH = 0;									//コントローラー
	float nValueV = 0;									//コントローラー

	m_bPiece = CBox::GetPiece();

	if (m_pPiece[m_nPieceNum] != NULL)
	{
		// 配置情報取得
		m_bPiece = m_pPiece[m_nPieceNum]->GetMove();
	}

	// ====================== コントローラー ====================== //

	if (pGamepad != NULL)
	{// ゲームパッドが存在したとき
		if (pGamepad->GetJoypadUse(0))
		{// 使用可能だったとき
			pGamepad->GetJoypadStickLeft(0, &nValueH, &nValueV);

			//// ゲームパッド処理
			//InputGemepad(nValueH, nValueV, fTireRotSpeed, aVec);

			if (m_bPiece == true)
			{
				if (pGamepad->GetControllerTrigger(0, JOYPADKEY_Y))
				{
					// ピース数加算
					m_nPieceNum++;
					// ピース生成
					m_pPiece[m_nPieceNum] = CPiece::Create();
					// ピースタイプ設定
					m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
					// 配置情報
					m_pPiece[m_nPieceNum]->SetMove(false);
				}


				// 下にスティックが倒れたとき
				if (nValueV <= JOY_MAX_RANGE && nValueV > 0)
				{
					// セレクトカウント減算
					m_nSelect--;
				}
				else if (nValueV >= -JOY_MAX_RANGE && nValueV < 0)
				{// 上にスティックが倒れたとき
				 // セレクトカウント加算
					m_nSelect++;
				}
			}

			if (m_bPiece == false)
			{
				// Zを押されたら
				if (pGamepad->GetControllerTrigger(0, JOYPADKEY_RIGHT_SHOULDER))
				{
					switch (m_nSelectCnt)
					{
					case 0:
						m_bSelect[PIECETYPE_CORE_09] = false;
						m_bSelect[PIECETYPE_CORE_01] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Route);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 0);
						m_nSelectCnt++;
						break;
					case 1:
						m_bSelect[PIECETYPE_CORE_01] = false;
						m_bSelect[PIECETYPE_CORE_02] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Ranking);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 1);
						m_nSelectCnt++;
						break;
					case 2:
						m_bSelect[PIECETYPE_CORE_02] = false;
						m_bSelect[PIECETYPE_CORE_03] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_MiniMap);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 2);
						m_nSelectCnt++;
						break;
					case 3:
						m_bSelect[PIECETYPE_CORE_03] = false;
						m_bSelect[PIECETYPE_CORE_04] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 3);
						m_nSelectCnt++;
						break;
					case 4:
						m_bSelect[PIECETYPE_CORE_04] = false;
						m_bSelect[PIECETYPE_CORE_00] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Rate_MediumUp);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 4);
						m_nSelectCnt++;
						break;
					case 5:
						m_bSelect[PIECETYPE_CORE_00] = false;
						m_bSelect[PIECETYPE_CORE_05] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Speed);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 5);
						m_nSelectCnt++;
						break;
					case 6:
						m_bSelect[PIECETYPE_CORE_05] = false;
						m_bSelect[PIECETYPE_CORE_08] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_2);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Power_MediumUp);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 6);
						m_nSelectCnt++;
						break;
					case 7:
						m_bSelect[PIECETYPE_CORE_08] = false;
						m_bSelect[PIECETYPE_CORE_14] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_L_Type);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 7);
						m_nSelectCnt++;
						break;
					case 8:
						m_bSelect[PIECETYPE_CORE_14] = false;
						m_bSelect[PIECETYPE_CORE_15] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Speed_1);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 8);
						m_nSelectCnt++;
						break;
					case 9:
						m_bSelect[PIECETYPE_CORE_15] = false;
						m_bSelect[PIECETYPE_CORE_16] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Diagonal);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 9);
						m_nSelectCnt++;
						break;
					case 10:
						m_bSelect[PIECETYPE_CORE_16] = false;
						m_bSelect[PIECETYPE_CORE_06] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_MaxSpeed_MediumUp);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 10);
						m_nSelectCnt++;
						break;
					case 11:
						m_bSelect[PIECETYPE_CORE_06] = false;
						m_bSelect[PIECETYPE_CORE_07] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Turning_MediumUp);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 11);
						m_nSelectCnt++;
						break;
					case 12:
						m_bSelect[PIECETYPE_CORE_07] = false;
						m_bSelect[PIECETYPE_CORE_13] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_2);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Decay_Down);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 12);
						m_nSelectCnt++;
						break;
					case 13:
						m_bSelect[PIECETYPE_CORE_13] = false;
						m_bSelect[PIECETYPE_CORE_12] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Power_SmallUp);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 13);
						m_nSelectCnt++;
						break;
					case 14:
						m_bSelect[PIECETYPE_CORE_12] = false;
						m_bSelect[PIECETYPE_CORE_11] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Turning_SmallUp);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 14);
						m_nSelectCnt++;
						break;
					case 15:
						m_bSelect[PIECETYPE_CORE_11] = false;
						m_bSelect[PIECETYPE_CORE_10] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_MaxSpeed_SmallUp);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 15);
						m_nSelectCnt++;
						break;
					case 16:
						m_bSelect[PIECETYPE_CORE_10] = false;
						m_bSelect[PIECETYPE_CORE_09] = true;
						// タイプ変更
						m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
						m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Rate_SmallUp);
						m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 16);
						m_nSelectCnt = 0;
						break;
					}
				}
			}

		}
	}
	if (m_bPiece == true)
	{
		// 生成
		if (pKeyboard->GetTriggerKeyboard(DIK_C))
		{
			// ピース数加算
			m_nPieceNum++;
			// ピース生成
			m_pPiece[m_nPieceNum] = CPiece::Create();
			// ピースタイプ設定
			m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
			// 配置情報
			m_pPiece[m_nPieceNum]->SetMove(false);
		}

		// 上下操作
		if (pKeyboard->GetTriggerKeyboard(MOVE_ACCEL))
		{
			// セレクトカウント加算
			m_nSelect++;
		}
		else if (pKeyboard->GetTriggerKeyboard(MOVE_BRAKE))
		{
			// セレクトカウント減算
			m_nSelect--;
		}

		// セレクトカウント制限
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
				// 選択されているときの色
				m_pPiece[m_nSelect]->SetCol(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));

				if (m_nSelect != m_nPieceNum)
				{
					m_pPiece[m_nSelect + 1]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
				}
				if (m_nSelect != 0)
				{
					m_pPiece[m_nSelect - 1]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
				}

				// Qを押されたら
				if (pKeyboard->GetTriggerKeyboard(DIK_Q))
				{
					if (m_pPiece[m_nSelect] != NULL)
					{
						// 選ばれてるピースの情報格納
						m_bPuzzleStorage[nCntDepth][nCntWidth] = m_pPiece[m_nSelect]->GetPuzzle(nCntDepth, nCntWidth);
						// 状態比較
						if (m_bPuzzle[nCntDepth][nCntWidth] == true && m_bPuzzleStorage[nCntDepth][nCntWidth] == true)
						{
							// 状態初期化
							m_bPuzzle[nCntDepth][nCntWidth] = false;
						}
						// ピース状態変更
						m_pPiece[m_nSelect]->SetRelease(true);
						// 状態変更
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
			m_fRate[nCnt] = m_pPiece[nCnt]->GetRate();												// スピード上昇率
			m_fTurning[nCnt] = m_pPiece[nCnt]->GetTurning();										// 旋回速度
			m_fDecay[nCnt] = m_pPiece[nCnt]->GetDecay();											// 減衰率
			m_nPower[nCnt] = (int)m_pPiece[nCnt]->GetPower();										// パワー
			m_bRoute[nCnt] = m_pPiece[nCnt]->GetRoute();
		}
		// ピース生成

		//m_pPiece[m_nPieceNum]->SetPlaacement(false);
	}


	if (m_bPiece == false)
	{
		// Zを押されたら
		if (pKeyboard->GetTriggerKeyboard(DIK_Z))
		{
			switch (m_nSelectCnt)
			{
			case 0:
				m_bSelect[PIECETYPE_CORE_09] = false;
				m_bSelect[PIECETYPE_CORE_01] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Route);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 0);
				m_nSelectCnt++;
				break;
			case 1:
				m_bSelect[PIECETYPE_CORE_01] = false;
				m_bSelect[PIECETYPE_CORE_02] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Ranking);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 1);
				m_nSelectCnt++;
				break;
			case 2:
				m_bSelect[PIECETYPE_CORE_02] = false;
				m_bSelect[PIECETYPE_CORE_03] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_MiniMap);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 2);
				m_nSelectCnt++;
				break;
			case 3:
				m_bSelect[PIECETYPE_CORE_03] = false;
				m_bSelect[PIECETYPE_CORE_04] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 3);
				m_nSelectCnt++;
				break;
			case 4:
				m_bSelect[PIECETYPE_CORE_04] = false;
				m_bSelect[PIECETYPE_CORE_00] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Rate_MediumUp);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 4);
				m_nSelectCnt++;
				break;
			case 5:
				m_bSelect[PIECETYPE_CORE_00] = false;
				m_bSelect[PIECETYPE_CORE_05] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Speed);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 5);
				m_nSelectCnt++;
				break;
			case 6:
				m_bSelect[PIECETYPE_CORE_05] = false;
				m_bSelect[PIECETYPE_CORE_08] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_2);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Power_MediumUp);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 6);
				m_nSelectCnt++;
				break;
			case 7:
				m_bSelect[PIECETYPE_CORE_08] = false;
				m_bSelect[PIECETYPE_CORE_14] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_L_Type);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 7);
				m_nSelectCnt++;
				break;
			case 8:
				m_bSelect[PIECETYPE_CORE_14] = false;
				m_bSelect[PIECETYPE_CORE_15] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Speed_1);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 8);
				m_nSelectCnt++;
				break;
			case 9:
				m_bSelect[PIECETYPE_CORE_15] = false;
				m_bSelect[PIECETYPE_CORE_16] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Diagonal);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 9);
				m_nSelectCnt++;
				break;
			case 10:
				m_bSelect[PIECETYPE_CORE_16] = false;
				m_bSelect[PIECETYPE_CORE_06] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_MaxSpeed_MediumUp);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 10);
				m_nSelectCnt++;
				break;
			case 11:
				m_bSelect[PIECETYPE_CORE_06] = false;
				m_bSelect[PIECETYPE_CORE_07] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_1);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Turning_MediumUp);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 11);
				m_nSelectCnt++;
				break;
			case 12:
				m_bSelect[PIECETYPE_CORE_07] = false;
				m_bSelect[PIECETYPE_CORE_13] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle_2);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Decay_Down);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 12);
				m_nSelectCnt++;
				break;
			case 13:
				m_bSelect[PIECETYPE_CORE_13] = false;
				m_bSelect[PIECETYPE_CORE_12] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Power_SmallUp);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 13);
				m_nSelectCnt++;
				break;
			case 14:
				m_bSelect[PIECETYPE_CORE_12] = false;
				m_bSelect[PIECETYPE_CORE_11] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Turning_SmallUp);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 14);
				m_nSelectCnt++;
				break;
			case 15:
				m_bSelect[PIECETYPE_CORE_11] = false;
				m_bSelect[PIECETYPE_CORE_10] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_MaxSpeed_SmallUp);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 15);
				m_nSelectCnt++;
				break;
			case 16:
				m_bSelect[PIECETYPE_CORE_10] = false;
				m_bSelect[PIECETYPE_CORE_09] = true;
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square_1);
				m_pPiece[m_nPieceNum]->SetStatusType(CPiece::StatusType_Rate_SmallUp);
				m_pPieceExplanation->SpriteAnimation(D3DXVECTOR2(1, 17), 0, 16);
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
				// 配置情報取得
				m_bPlacement = m_pPiece[m_nPieceNum]->GetPlaacement();
				// 設置されてたら
				if (m_bPlacement == true)
				{
					// 情報格納
					m_bPuzzleStorage[nCntDepth][nCntWidth] = m_pPiece[m_nPieceNum]->GetPuzzle(nCntDepth, nCntWidth);
				}
			}
			if (m_bPuzzle[nCntDepth][nCntWidth] == false && m_bPuzzleStorage[nCntDepth][nCntWidth] == true)
			{
				// 配置
				m_bPuzzle[nCntDepth][nCntWidth] = true;
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CPieceSelect::Draw(void)
{

}

//==============================================================================
// 生成処理
//==============================================================================
CPieceSelect *CPieceSelect::Create(void)
{
	CPieceSelect *pPieceSelect;									// 背景のポインタ

	pPieceSelect = new CPieceSelect(CScene::PRIORITY_BG);		// 背景の生成
	pPieceSelect->Init();										// 背景の初期化
	return pPieceSelect;
}

//==============================================================================
// アセットの生成処理
//==============================================================================
HRESULT CPieceSelect::Load(void)
{
	//テクスチャの読み込み
	CManager::Load("data/tex/back.png");
	return S_OK;
}

//==============================================================================
// ピースの設定
//==============================================================================
void CPieceSelect::SetPiece(PIECETYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 size, std::string Add)
{
	m_pPieceSelect[type]->BindTexture(Add);									// テクスチャのポインタを渡す
	m_pPieceSelect[type]->SetCenter(CScene2D::TEXTUREVTX_CENTER);			// 中心の設定
	m_pPieceSelect[type]->SetSize(D3DXVECTOR3(size.x, size.y, 0.0f));		// 大きさの設定
	m_pPieceSelect[type]->SetPosition(D3DXVECTOR3(pos.x, pos.y, 0.0f));		// 位置の設定
	m_pPieceSelect[type]->SetTransform();
}

//==============================================================================
// ピースのロード
//==============================================================================
void CPieceSelect::LoadPiece(void)
{
	FILE *pFile = NULL;																// ファイル
	char cReadText[128];															// 文字
	char cHeadText[128];															// 比較
	char cDie[128];
	D3DXVECTOR3 pos;
	D3DXVECTOR3 size;
	char aAdd[64];
	int type;

	pFile = fopen(PIECE_FILE, "r");				// ファイルを開くまたは作る

	if (pFile != NULL)						//ファイルが読み込めた場合
	{
		//スクリプトが来るまでループ
		while (strcmp(cHeadText, "SCRIPT") != 0)
		{
			fgets(cReadText, sizeof(cReadText), pFile);
			sscanf(cReadText, "%s", &cHeadText);
		}

		//スクリプトだったら
		if (strcmp(cHeadText, "SCRIPT") == 0)
		{
			//エンドスクリプトが来るまで
			while (strcmp(cHeadText, "END_SCRIPT") != 0)
			{
				fgets(cReadText, sizeof(cReadText), pFile);
				sscanf(cReadText, "%s", &cHeadText);

				//改行
				if (strcmp(cReadText, "\n") != 0)
				{
					if (strcmp(cHeadText, "PIECESET") == 0)
					{//キャラクターの初期情報のとき

							//エンドキャラクターセットが来るまでループ
						while (strcmp(cHeadText, "END_PIECESET") != 0)
						{
							fgets(cReadText, sizeof(cReadText), pFile);
							sscanf(cReadText, "%s", &cHeadText);

							
							if (strcmp(cHeadText, "POS") == 0)
							{//パーツ総数のとき
								sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
									&pos.x,
									&pos.y,
									&pos.z);
							}
							else if (strcmp(cHeadText, "TYPE") == 0)
							{//パーツ総数のとき
								sscanf(cReadText, "%s %s %d", &cDie, &cDie,
									&type);
							}
							else if (strcmp(cHeadText, "SIZE") == 0)
							{//パーツ総数のとき
								sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
									&size.x,
									&size.y,
									&size.z);

							}
							else if (strcmp(cHeadText, "TEX") == 0)
							{//パーツ総数のとき
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
		fclose(pFile);				// ファイルを閉じる
	}
	else
	{
		MessageBox(NULL, "情報のアクセス失敗！", "WARNING", MB_ICONWARNING);	// メッセージボックスの生成
	}
}