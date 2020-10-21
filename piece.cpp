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

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define WhileX 50.0f																// イチマスの長さ横
#define WhileY 50.0f																// イチマスの長さ高さ
#define WhileZ 50.0f																// イチマスの長さ縦
#define Range_X 13																	// 横の範囲
#define Range_Y 6																	// 縦の範囲

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
CPiece::CPiece(PRIORITY type = CScene::PRIORITY_FLOOR) :CScene(type)
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
	m_bPlacement = false;
	m_bCreate = false;
	m_bMove = false;
	m_Status = PieceStatus_None;

	for (int nCntDepth = 0; nCntDepth < Piece_Depth; nCntDepth++)
	{
		for (int nCntWidth = 0; nCntWidth < Piece_Width; nCntWidth++)
		{
			m_bPuzzle[nCntDepth][nCntWidth] = false;

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

	// 縦をカウント
	for (int nDepth = 0; nDepth < Piece_Depth; nDepth++)
	{
		// 横をカウント
		for (int nWide = 0; nWide < Piece_Width; nWide++)
		{
			// ポジション取得
			m_pos = m_pBlock[nDepth][nWide]->GetPosition();

			// 初期配置
			if (nDepth == m_nCntMove_Y && nWide == m_nCntMove_X)
			{
				m_bPuzzle[nDepth][nWide] = true;
				m_Status = PieceStatus_Plaacement;
			}
			else if (nDepth == m_nCntMove_Y && nWide == m_nCntMove_X + 1)
			{
				m_bPuzzle[nDepth][nWide] = true;
				m_Status = PieceStatus_Plaacement;
			}
			else if (nDepth == m_nCntMove_Y + 1 && nWide == m_nCntMove_X)
			{
				m_bPuzzle[nDepth][nWide] = true;
				m_Status = PieceStatus_Plaacement;
			}
			else if (nDepth == m_nCntMove_Y + 1 && nWide == m_nCntMove_X + 1)
			{
				m_bPuzzle[nDepth][nWide] = true;
				m_Status = PieceStatus_Plaacement;
			}
			else
			{
				m_bPuzzle[nDepth][nWide] = false;
				m_Status = PieceStatus_Not;
			}

			// 状態確認
			if (m_bPuzzle[nDepth][nWide] == true)
			{
				// テクスチャ変更
				m_pBlock[nDepth][nWide]->BindTexture(CManager::GetResource("data/tex/grass.jpg"));
				// 配置しているかどうか
				if (m_bPlacement == false)
				{
					// 色の変更
					m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f));
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
	// 配置決定
	if (pKeyboard->GetTriggerKeyboard(MOVE_JUMP))
	{
		m_bPlacement = true;
		m_bMove = true;
	}
	if (m_bMove == true)
	{
		// 生成
		if (pKeyboard->GetTriggerKeyboard(DIK_C))
		{
			m_bCreate = true;
			m_bMove = false;
		}
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
	CPiece *pPiece = new CPiece(CScene::PRIORITY_FLOOR);

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