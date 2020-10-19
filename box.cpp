//==================================================================================================================
//
// box[box.cpp]
// Author:Ryouma Inoue
//
//==================================================================================================================
#include "manager.h"
#include "renderer.h"
#include "Box.h"
#include "inputKeyboard.h"
#include "player.h"
#include "game.h"
#include "scene2D.h"
#include "takaseiLibrary.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define WhileX 50.0f																// イチマスの長さ横
#define WhileY 50.0f																// イチマスの長さ高さ
#define WhileZ 50.0f																// イチマスの長さ縦

//==================================================================================================================
// 静的メンバ変数の初期化
//==================================================================================================================
LPDIRECT3DTEXTURE9 CBox::m_pTexture = NULL;			// テクスチャ変数

//==================================================================================================================
// グローバル変数
//==================================================================================================================

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CBox::CBox(PRIORITY type = CScene::PRIORITY_FLOOR) :CScene(type)
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CBox::~CBox()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
HRESULT CBox::Init(void)
{
	// 初期化
	m_nCntMove_X = 0;
	m_nCntMove_Y = 0;
	m_bPlacement = false;
	m_bCreate = false;
	m_bMove = false;

	for (int nCntDepth = 0; nCntDepth < Box_Depth; nCntDepth++)
	{
		for (int nCntWidth = 0; nCntWidth < Box_Width; nCntWidth++)
		{
			m_bPuzzle[nCntDepth][nCntWidth] = false;

			m_pBlock[nCntDepth][nCntWidth] = CScene2D::Create(PRIORITY_UI);

			if (m_pBlock[nCntDepth][nCntWidth] != NULL)
			{
				m_pBlock[nCntDepth][nCntWidth]->SetPosition(D3DXVECTOR3(100.0f + nCntWidth * 55.0f,
					100.0f + nCntDepth * 55.0f,
					0.0f));
				m_pBlock[nCntDepth][nCntWidth]->SetSize(D3DXVECTOR3(50.0f, 50.0f, 0.0f));
				m_pBlock[nCntDepth][nCntWidth]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
				m_pBlock[nCntDepth][nCntWidth]->SetTransform();
			}
		}
	}

	//m_bPuzzle[nCntMove_Y][nCntMove_X] = true;
	//m_bPuzzle[nCntMove_Y][nCntMove_X + 1] = true;
	//m_bPuzzle[nCntMove_Y + 1][nCntMove_X] = true;
	//m_bPuzzle[nCntMove_Y + 1][nCntMove_X + 1] = true;

	return S_OK;
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CBox::Uninit(void)
{

}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CBox::Update(void)
{
	// キーボード取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();

	// 縦をカウント
	for (int nDepth = 0; nDepth < Box_Depth; nDepth++)
	{
		// 横をカウント
		for (int nWide = 0; nWide < Box_Width; nWide++)
		{
			// 初期配置
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
					m_bCreate = true;
				}
			}
			else
			{
				// テクスチャ変更
				m_pBlock[nDepth][nWide]->BindTexture(CManager::GetResource("data/tex/SignBoard3.png"));
				// 色の変更
				m_pBlock[nDepth][nWide]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			}


		}
	}

	if (m_bMove == false)
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

	// 移動制限
	if (m_nCntMove_X <= 0)
	{
		m_nCntMove_X = 0;
	}
	if (m_nCntMove_Y <= 0)
	{
		m_nCntMove_Y = 0;
	}
	if (m_nCntMove_X >= Box_Width - 2)
	{
		m_nCntMove_X = Box_Width - 2;
	}
	if (m_nCntMove_Y >= Box_Depth - 2)
	{
		m_nCntMove_Y = Box_Depth - 2;
	}
}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CBox::Draw(void)
{

}

//==================================================================================================================
// ポリゴン生成
//==================================================================================================================
CBox *CBox::Create(void)
{
	// シーン動的に確保
	CBox *pBox = new CBox(CScene::PRIORITY_FLOOR);

	if (pBox != NULL)
	{
		// シーン初期化
		pBox->Init();
	}

	// 値を返す
	return pBox;
}

//==================================================================================================================
// テクスチャ情報ロード
//==================================================================================================================
HRESULT CBox::Load(void)
{
	return S_OK;
}

//==================================================================================================================
// テクスチャ情報破棄
//==================================================================================================================
void CBox::Unload(void)
{
	// テクスチャの開放

}