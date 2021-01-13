//==================================================================================================================
//
// ピース[piece.cpp]
// Author:Ryouma Inoue
//
//==================================================================================================================
#include "manager.h"
#include "renderer.h"
#include "piece.h"
#include "inputKeyboard.h"
#include "player.h"
#include "game.h"
#include "scene2D.h"
#include "takaseiLibrary.h"
#include "debug.h"
#include "box.h"
#include "pieceSelect.h"
#include "inputController.h"
#include "sound.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define WhileX 50.0f																// イチマスの長さ横
#define WhileY 50.0f																// イチマスの長さ高さ
#define WhileZ 50.0f																// イチマスの長さ縦
#define Range_X 13																	// 横の範囲
#define Range_Y 6																	// 縦の範囲
#define SmallUp 1																	// 小アップ
#define MediumUp 2																	// 中アップ
#define GreatUp 3																	// 大アップ

//==================================================================================================================
// 静的メンバ変数の初期化
//==================================================================================================================
LPDIRECT3DTEXTURE9 CPiece::m_pTexture = NULL;			// テクスチャ変数

//==================================================================================================================
// グローバル変数
//==================================================================================================================

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CPiece::CPiece(PRIORITY type = CScene::PRIORITY_UI) :CScene(type)
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CPiece::~CPiece()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
HRESULT CPiece::Init(void)
{
	// 初期化
	m_nCntMove_X = 0;
	m_nCntMove_Y = 0;
	m_fSpeed = 0;
	m_fRate = 0;
	m_fTurning = 0;
	m_fDecay = 0;
	m_nPower = 0;
	m_bPlacement = false;
	m_bRelease = false;
	m_bMove = false;
	m_bPut = true;
	m_bMap = false;
	m_bRoute = false;
	m_bRanking = false;
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ブロックの初期化
	for (int nCntDepth = 0; nCntDepth < Piece_Depth; nCntDepth++)
	{
		for (int nCntWidth = 0; nCntWidth < Piece_Width; nCntWidth++)
		{
			m_bPuzzle[nCntDepth][nCntWidth] = false;
			m_bBox[nCntDepth][nCntWidth] = false;
			m_pBlock[nCntDepth][nCntWidth] = CScene2D::Create(PRIORITY_UI);

			if (m_pBlock[nCntDepth][nCntWidth] != NULL)
			{
				m_pBlock[nCntDepth][nCntWidth]->SetPosition(D3DXVECTOR3(100.0f + nCntWidth * 55.0f,
					100.0f + nCntDepth * 55.0f,
					0.0f));
				m_pBlock[nCntDepth][nCntWidth]->SetSize(D3DXVECTOR3(50.0f, 50.0f, 0.0f));
				m_pBlock[nCntDepth][nCntWidth]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
				m_pBlock[nCntDepth][nCntWidth]->SetTransform();

			}
		}
	}
	return S_OK;
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CPiece::Uninit(void)
{

}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CPiece::Update(void)
{
	// キーボード取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();
	// ゲームパッドの取得
	CInputController *pGamepad = CManager::GetInputController();

	float nValueH = 0;									//コントローラー
	float nValueV = 0;									//コントローラー

	// ピースの設定
	SetPiece();

	SetStatus();

	// ====================== コントローラー ====================== //

	if (pGamepad != NULL)
	{// ゲームパッドが存在したとき
		if (pGamepad->GetJoypadUse(0))
		{// 使用可能だったとき
			pGamepad->GetJoypadStickLeft(0, &nValueH, &nValueV);

			//// ゲームパッド処理
			//InputGemepad(nValueH, nValueV, fTireRotSpeed, aVec);

			if (pGamepad->GetControllerPress(0, JOYPADKEY_B))
			{
				if (m_bPut == true)
				{
					CSound *pSound = CManager::GetSound();				// サウンドの取得

					pSound->PlaySoundA(SOUND_LABEL_SE_WithParts);			// ダメージ音の再生
					pSound->SetVolume(SOUND_LABEL_SE_WithParts, 5.0f);

					m_bPlacement = true;
					m_bMove = true;
				}
			}	// 配置していなかったら
			if (m_bPlacement == false)
			{

				// 左にスティックが倒れたとき
				if (pGamepad->GetControllerTrigger(0, JOYPADKEY_LEFT))
				{
					m_nCntMove_X--;
				}
				else if (pGamepad->GetControllerTrigger(0, JOYPADKEY_RIGHT))
				{// 右にスティックが倒れたとき
					m_nCntMove_X++;
				}
				// 下にスティックが倒れたとき
				if (pGamepad->GetControllerTrigger(0, JOYPADKEY_UP))
				{
					m_nCntMove_Y--;
				}
				else if (pGamepad->GetControllerTrigger(0, JOYPADKEY_DOWN))
				{// 上にスティックが倒れたとき
					m_nCntMove_Y++;
				}
			}
			else
			{
				// 縦をカウント
				for (int nDepth = 0; nDepth < Piece_Depth; nDepth++)
				{
					// 横をカウント
					for (int nWide = 0; nWide < Piece_Width; nWide++)
					{
						// 状態確認
						if (m_bPuzzle[nDepth][nWide] == true)
						{
							// 色の変更
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(m_col));
						}
					}
				}

			}
		}
	}
	// 配置決定
	if (pKeyboard->GetTriggerKeyboard(MOVE_JUMP))
	{
		if (m_bPut == true)
		{
			CSound *pSound = CManager::GetSound();				// サウンドの取得

			pSound->PlaySoundA(SOUND_LABEL_SE_WithParts);			// ダメージ音の再生
			pSound->SetVolume(SOUND_LABEL_SE_WithParts, 5.0f);

			m_bPlacement = true;
			m_bMove = true;
		}
	}
	for (int nCntDepth = 0; nCntDepth < Piece_Depth; nCntDepth++)
	{
		for (int nCntWidth = 0; nCntWidth < Piece_Width; nCntWidth++)
		{
			if (m_bRelease == true)
			{
				// ブロック消去
				m_pBlock[nCntDepth][nCntWidth]->Release();
				Release();
			}
		}
	}
	// 配置していなかったら
	if (m_bPlacement == false)
	{
		// -----------------------------------------
		// 移動処理
		// -----------------------------------------
		// 左右操作
		if (pKeyboard->GetTriggerKeyboard(MOVE_LEFT))
		{
			m_nCntMove_X--;
		}
		else if (pKeyboard->GetTriggerKeyboard(MOVE_RIGHT))
		{
			m_nCntMove_X++;
		}
		// 上下操作
		else if (pKeyboard->GetTriggerKeyboard(MOVE_ACCEL))
		{
			m_nCntMove_Y--;
		}
		else if (pKeyboard->GetTriggerKeyboard(MOVE_BRAKE))
		{
			m_nCntMove_Y++;
		}
	}
	else
	{
		// 縦をカウント
		for (int nDepth = 0; nDepth < Piece_Depth; nDepth++)
		{
			// 横をカウント
			for (int nWide = 0; nWide < Piece_Width; nWide++)
			{
				// 状態確認
				if (m_bPuzzle[nDepth][nWide] == true)
				{
					// 色の変更
					m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(m_col));
				}
			}
		}

	}
}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CPiece::Draw(void)
{

}

//==================================================================================================================
// ポリゴン生成
//==================================================================================================================
CPiece *CPiece::Create(void)
{
	// シーン動的に確保
	CPiece *pPiece = new CPiece(CScene::PRIORITY_UI);

	if (pPiece != NULL)
	{
		// シーン初期化
		pPiece->Init();
	}

	// 値を返す
	return pPiece;
}

//==================================================================================================================
// テクスチャ情報ロード
//==================================================================================================================
HRESULT CPiece::Load(void)
{
	return S_OK;
}

//==================================================================================================================
// テクスチャ情報破棄
//==================================================================================================================
void CPiece::Unload(void)
{
	// テクスチャの開放

}

//==================================================================================================================
// ステータスの設定
//==================================================================================================================
void CPiece::SetStatus(void)
{
	// 配置してたら
	if (m_bPlacement == true)
	{
		// 形を変える
		switch (m_PieceType)
		{
			// ■■
			// ■■
		case PieceType_Square:
			switch (m_StatusType)
			{
			case StatusType_Route:
				m_fTurning = MediumUp;
				m_bRoute = true;
				break;

			case StatusType_Ranking:
				m_fSpeed = MediumUp;
				m_bRanking = true;
				break;
			}
			break;

			// ■■
			// ■■
			// ■■
			// ■■
			// ■■
		case PieceType_Rectangle:

			m_fSpeed = MediumUp;
			break;

			// ■
			// ■
			// ■
			// ■■
		case PieceType_L_Type:
			m_fTurning = 3.0f;
			break;
			// ■
		case PieceType_Square_1:

			switch (m_StatusType)
			{
			case StatusType_MaxSpeed_SmallUp:
				m_fSpeed = SmallUp;
				break;

			case StatusType_Rate_SmallUp:
				m_fRate = SmallUp;
				break;

			case StatusType_Turning_SmallUp:
				m_fTurning = SmallUp;
				break;
			}
			break;

			// ■
			// ■
		case PieceType_Rectangle_1:
			switch (m_StatusType)
			{
			case StatusType_MaxSpeed_MediumUp:
				m_fSpeed = MediumUp;
				break;

			case StatusType_Rate_MediumUp:
				m_fRate = MediumUp;
				break;

			case StatusType_Turning_MediumUp:
				m_fTurning = MediumUp;
				break;
			}
			break;

			// ■
			// ■
			// ■
			// ■
		case PieceType_Rectangle_2:
			switch (m_StatusType)
			{
			case StatusType_Decay_Down:
				m_fDecay = MediumUp;
				break;
			}

			break;

			// ■■
			// ■■■
			// 　　■■
		case PieceType_Speed:
			m_fSpeed = GreatUp;
			break;

			// ■■
			// 　■
			// 　■
			// ■■
			// ■
		case PieceType_Speed_1:
			m_fRate = GreatUp;
			break;
		}

	}
}

//==================================================================================================================
// ピース設定
//==================================================================================================================
void CPiece::SetPiece(void)
{
	// 縦をカウント
	for (int nDepth = 0; nDepth < Piece_Depth; nDepth++)
	{
		// 横をカウント
		for (int nWide = 0; nWide < Piece_Width; nWide++)
		{
			m_bBox[nDepth][nWide] = CPieceSelect::GetPuzzle(nDepth, nWide);
			// 形を変える
			switch (m_PieceType)
			{
			// ■■
			// ■■
			case PieceType_Square:
				// 配置
				if (nDepth == m_nCntMove_Y && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y && nWide == m_nCntMove_X + 1)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 1 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 1 && nWide == m_nCntMove_X + 1)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else
				{
					m_bPuzzle[nDepth][nWide] = false;
				}

				// 状態確認
				if (m_bPuzzle[nDepth][nWide] == true)
				{
					// テクスチャ変更
					m_pBlock[nDepth][nWide]->BindTexture("data/tex/grass.jpg");
					// 配置しているかどうか
					if (m_bPlacement == false)
					{
						// 色の変更
						if (m_bBox[m_nCntMove_Y][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y][m_nCntMove_X + 1] == false &&
							m_bBox[m_nCntMove_Y + 1][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y + 1][m_nCntMove_X + 1] == false)
						{
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f));
							m_bPut = true;
						}
						else
						{
							// 色の変更
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
							m_bPut = false;
						}
					}
					else
					{
						// 色の変更
						m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
					}

				}
				else
				{
					// 色設定
					m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
				}

				// 枠外に行かないようにする
				if (m_nCntMove_X <= 0)
				{
					m_nCntMove_X = 0;
				}
				else if (m_nCntMove_X >= Range_X)
				{
					m_nCntMove_X = Range_X;
				}

				if (m_nCntMove_Y <= 0)
				{
					m_nCntMove_Y = 0;
				}
				else if (m_nCntMove_Y >= Range_Y)
				{
					m_nCntMove_Y = Range_Y;
				}
				break;

			// ■■
			// ■■
			// ■■
			// ■■
			// ■■
			case PieceType_Rectangle:

				// 配置
				if (nDepth == m_nCntMove_Y && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y && nWide == m_nCntMove_X + 1)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 1 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 1 && nWide == m_nCntMove_X + 1)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 2 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 2 && nWide == m_nCntMove_X + 1)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 3 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 3 && nWide == m_nCntMove_X + 1)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 4 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 4 && nWide == m_nCntMove_X + 1)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else
				{
					m_bPuzzle[nDepth][nWide] = false;
				}

				// 状態確認
				if (m_bPuzzle[nDepth][nWide] == true)
				{
					// テクスチャ変更
					m_pBlock[nDepth][nWide]->BindTexture("data/tex/grass.jpg");
					// 配置しているかどうか
					if (m_bPlacement == false)
					{
						// 色の変更
						if (m_bBox[m_nCntMove_Y][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y][m_nCntMove_X + 1] == false &&
							m_bBox[m_nCntMove_Y + 1][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y + 1][m_nCntMove_X + 1] == false &&
							m_bBox[m_nCntMove_Y + 2][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y + 2][m_nCntMove_X + 1] == false &&
							m_bBox[m_nCntMove_Y + 3][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y + 3][m_nCntMove_X + 1] == false &&
							m_bBox[m_nCntMove_Y + 4][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y + 4][m_nCntMove_X + 1] == false)
						{
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f));
							m_bPut = true;
						}
						else
						{
							// 色の変更
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
							m_bPut = false;
						}
					}
					else
					{
						// 色の変更
						m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
					}

				}
				else
				{
					// 色設定
					m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
				}
				// 枠外に行かないようにする
				if (m_nCntMove_X <= 0)
				{
					m_nCntMove_X = 0;
				}
				else if (m_nCntMove_X >= Range_X)
				{
					m_nCntMove_X = Range_X;
				}

				if (m_nCntMove_Y <= 0)
				{
					m_nCntMove_Y = 0;
				}
				else if (m_nCntMove_Y >= Range_Y - 3)
				{
					m_nCntMove_Y = Range_Y - 3;
				}
				break;
			// ■
			// ■
			// ■
			// ■■
			case PieceType_L_Type:

				// 配置
				if (nDepth == m_nCntMove_Y && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				// 配置
				else if (nDepth == m_nCntMove_Y + 1 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				// 配置
				else if (nDepth == m_nCntMove_Y + 2 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				// 配置
				else if (nDepth == m_nCntMove_Y + 3 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 3 && nWide == m_nCntMove_X + 1)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else
				{
					m_bPuzzle[nDepth][nWide] = false;
				}

				// 状態確認
				if (m_bPuzzle[nDepth][nWide] == true)
				{
					// テクスチャ変更
					m_pBlock[nDepth][nWide]->BindTexture("data/tex/grass.jpg");
					// 配置しているかどうか
					if (m_bPlacement == false)
					{
						// 色の変更
						if (m_bBox[m_nCntMove_Y][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y + 1][m_nCntMove_X] == false &&
							m_bBox[m_nCntMove_Y + 2][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y + 3][m_nCntMove_X] == false &&
							m_bBox[m_nCntMove_Y + 3][m_nCntMove_X + 1] == false)
						{
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f));
							m_bPut = true;
						}
						else
						{
							// 色の変更
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
							m_bPut = false;
						}
					}
					else
					{
						// 色の変更
						m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
					}

				}
				else
				{
					// 色設定
					m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
				}

				// 枠外に行かないようにする
				if (m_nCntMove_X <= 0)
				{
					m_nCntMove_X = 0;
				}
				else if (m_nCntMove_X >= Range_X)
				{
					m_nCntMove_X = Range_X;
				}

				if (m_nCntMove_Y <= 0)
				{
					m_nCntMove_Y = 0;
				}
				else if (m_nCntMove_Y >= Range_Y - 2)
				{
					m_nCntMove_Y = Range_Y - 2;
				}

				break;
			// ■
			case PieceType_Square_1:

				// 配置
				if (nDepth == m_nCntMove_Y && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else
				{
					m_bPuzzle[nDepth][nWide] = false;
				}

				// 状態確認
				if (m_bPuzzle[nDepth][nWide] == true)
				{
					// テクスチャ変更
					m_pBlock[nDepth][nWide]->BindTexture("data/tex/grass.jpg");
					// 配置しているかどうか
					if (m_bPlacement == false)
					{
						// 色の変更
						if (m_bBox[m_nCntMove_Y][m_nCntMove_X] == false)
						{
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f));
							m_bPut = true;
						}
						else
						{
							// 色の変更
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
							m_bPut = false;
						}
					}
					else
					{
						// 色の変更
						m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
					}

				}
				else
				{
					// 色設定
					m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
				}

				// 枠外に行かないようにする
				if (m_nCntMove_X <= 0)
				{
					m_nCntMove_X = 0;
				}
				else if (m_nCntMove_X >= Range_X + 1)
				{
					m_nCntMove_X = Range_X + 1;
				}

				if (m_nCntMove_Y <= 0)
				{
					m_nCntMove_Y = 0;
				}
				else if (m_nCntMove_Y >= Range_Y + 1)
				{
					m_nCntMove_Y = Range_Y + 1;
				}

				break;

			// ■
			// ■
			case PieceType_Rectangle_1:

				// 配置
				if (nDepth == m_nCntMove_Y && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				// 配置
				else if (nDepth == m_nCntMove_Y + 1 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else
				{
					m_bPuzzle[nDepth][nWide] = false;
				}

				// 状態確認
				if (m_bPuzzle[nDepth][nWide] == true)
				{
					// テクスチャ変更
					m_pBlock[nDepth][nWide]->BindTexture("data/tex/grass.jpg");
					//m_pBlock[nDepth][nWide]->SpriteAnimation(D3DXVECTOR2(3.0f, 5.0f), 1, 1);
					// 配置しているかどうか
					if (m_bPlacement == false)
					{
						// 色の変更
						if (m_bBox[m_nCntMove_Y][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y + 1][m_nCntMove_X] == false)
						{
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f));
							m_bPut = true;
						}
						else
						{
							// 色の変更
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
							m_bPut = false;
						}
					}
					else
					{
						// 色の変更
						m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
					}

				}
				else
				{
					// 色設定
					m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
				}

				// 枠外に行かないようにする
				if (m_nCntMove_X <= 0)
				{
					m_nCntMove_X = 0;
				}
				else if (m_nCntMove_X >= Range_X + 1)
				{
					m_nCntMove_X = Range_X + 1;
				}

				if (m_nCntMove_Y <= 0)
				{
					m_nCntMove_Y = 0;
				}
				else if (m_nCntMove_Y >= Range_Y)
				{
					m_nCntMove_Y = Range_Y;
				}

				break;

			// ■
			// ■
			// ■
			// ■
			case PieceType_Rectangle_2:

				// 配置
				if (nDepth == m_nCntMove_Y && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				// 配置
				else if (nDepth == m_nCntMove_Y + 1 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				// 配置
				else if (nDepth == m_nCntMove_Y + 2 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				// 配置
				else if (nDepth == m_nCntMove_Y + 3 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else
				{
					m_bPuzzle[nDepth][nWide] = false;
				}

				// 状態確認
				if (m_bPuzzle[nDepth][nWide] == true)
				{
					// テクスチャ変更
					m_pBlock[nDepth][nWide]->BindTexture("data/tex/grass.jpg");
					// 配置しているかどうか
					if (m_bPlacement == false)
					{
						// 色の変更
						if (m_bBox[m_nCntMove_Y][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y + 1][m_nCntMove_X] == false &&
							m_bBox[m_nCntMove_Y + 2][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y + 3][m_nCntMove_X] == false)
						{
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f));
							m_bPut = true;
						}
						else
						{
							// 色の変更
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
							m_bPut = false;
						}
					}
					else
					{
						// 色の変更
						m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
					}

				}
				else
				{
					// 色設定
					m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
				}

				// 枠外に行かないようにする
				if (m_nCntMove_X <= 0)
				{
					m_nCntMove_X = 0;
				}
				else if (m_nCntMove_X >= Range_X + 1)
				{
					m_nCntMove_X = Range_X + 1;
				}

				if (m_nCntMove_Y <= 0)
				{
					m_nCntMove_Y = 0;
				}
				else if (m_nCntMove_Y >= Range_Y - 2)
				{
					m_nCntMove_Y = Range_Y - 2;
				}

				break;

				// ■■
				// ■■■
				// 　　■■
			case PieceType_Speed:
				// 配置
				if (nDepth == m_nCntMove_Y && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y && nWide == m_nCntMove_X + 1)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 1 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 1 && nWide == m_nCntMove_X + 1)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 1 && nWide == m_nCntMove_X + 2)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 2 && nWide == m_nCntMove_X + 2)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 2 && nWide == m_nCntMove_X + 3)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else
				{
					m_bPuzzle[nDepth][nWide] = false;
				}

				// 状態確認
				if (m_bPuzzle[nDepth][nWide] == true)
				{
					// テクスチャ変更
					m_pBlock[nDepth][nWide]->BindTexture("data/tex/grass.jpg");
					// 配置しているかどうか
					if (m_bPlacement == false)
					{
						// 色の変更
						if (m_bBox[m_nCntMove_Y][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y][m_nCntMove_X + 1] == false &&
							m_bBox[m_nCntMove_Y + 1][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y + 1][m_nCntMove_X + 1] == false &&
							m_bBox[m_nCntMove_Y + 1][m_nCntMove_X + 2] == false && m_bBox[m_nCntMove_Y + 2][m_nCntMove_X + 2] == false &&
							m_bBox[m_nCntMove_Y + 2][m_nCntMove_X + 3] == false)
						{
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f));
							m_bPut = true;
						}
						else
						{
							// 色の変更
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
							m_bPut = false;
						}
					}
					else
					{
						// 色の変更
						m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
					}

				}
				else
				{
					// 色設定
					m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
				}

				// 枠外に行かないようにする
				if (m_nCntMove_X <= 0)
				{
					m_nCntMove_X = 0;
				}
				else if (m_nCntMove_X >= Range_X - 2)
				{
					m_nCntMove_X = Range_X - 2;
				}

				if (m_nCntMove_Y <= 0)
				{
					m_nCntMove_Y = 0;
				}
				else if (m_nCntMove_Y >= Range_Y - 1)
				{
					m_nCntMove_Y = Range_Y - 1;
				}
				break;

			// ■■
			// 　■
			// 　■
			// ■■
			// ■
			case PieceType_Speed_1:

				// 配置
				if (nDepth == m_nCntMove_Y && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				// 配置
				else if (nDepth == m_nCntMove_Y && nWide == m_nCntMove_X + 1)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				// 配置
				else if (nDepth == m_nCntMove_Y + 1 && nWide == m_nCntMove_X + 1)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				// 配置
				else if (nDepth == m_nCntMove_Y + 2 && nWide == m_nCntMove_X + 1)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				// 配置
				else if (nDepth == m_nCntMove_Y + 3 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 3 && nWide == m_nCntMove_X + 1)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else if (nDepth == m_nCntMove_Y + 4 && nWide == m_nCntMove_X)
				{
					m_bPuzzle[nDepth][nWide] = true;
				}
				else
				{
					m_bPuzzle[nDepth][nWide] = false;
				}

				// 状態確認
				if (m_bPuzzle[nDepth][nWide] == true)
				{
					// テクスチャ変更
					m_pBlock[nDepth][nWide]->BindTexture("data/tex/grass.jpg");
					// 配置しているかどうか
					if (m_bPlacement == false)
					{
						// 色の変更
						if (m_bBox[m_nCntMove_Y][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y][m_nCntMove_X + 1] == false &&
							m_bBox[m_nCntMove_Y + 1][m_nCntMove_X + 1] == false && m_bBox[m_nCntMove_Y + 2][m_nCntMove_X + 1] == false &&
							m_bBox[m_nCntMove_Y + 3][m_nCntMove_X] == false && m_bBox[m_nCntMove_Y + 3][m_nCntMove_X + 1] == false &&
							m_bBox[m_nCntMove_Y + 4][m_nCntMove_X] == false)
						{
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f));
							m_bPut = true;
						}
						else
						{
							// 色の変更
							m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
							m_bPut = false;
						}
					}
					else
					{
						// 色の変更
						m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
					}

				}
				else
				{
					// 色設定
					m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
				}

				// 枠外に行かないようにする
				if (m_nCntMove_X <= 0)
				{
					m_nCntMove_X = 0;
				}
				else if (m_nCntMove_X >= Range_X)
				{
					m_nCntMove_X = Range_X;
				}

				if (m_nCntMove_Y <= 0)
				{
					m_nCntMove_Y = 0;
				}
				else if (m_nCntMove_Y >= Range_Y - 3)
				{
					m_nCntMove_Y = Range_Y - 3;
				}

				break;
			}
		}
	}
}