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
#include "piece.h"

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
bool CBox::m_bPuzzle[Box_Depth][Box_Width] = {};

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
	m_nCntChange = 0;
	m_nPieceNum = 0;
	m_nSelect = 0;
	m_bPlacement = false;
	m_bRelease = false;
	m_bCreate = false;
	m_bMove = false;

	// ブロックの初期化
	for (int nCntDepth = 0; nCntDepth < Box_Depth; nCntDepth++)
	{
		for (int nCntWidth = 0; nCntWidth < Box_Width; nCntWidth++)
		{
			// パズル初期化
			m_bPuzzle[nCntDepth][nCntWidth] = false;
			// 格納用
			m_bPuzzleStorage[nCntDepth][nCntWidth] = false;
			// 生成
			m_pBlock[nCntDepth][nCntWidth] = CScene2D::Create(PRIORITY_UI);

			if (m_pBlock[nCntDepth][nCntWidth] != NULL)
			{
				// 位置
				m_pBlock[nCntDepth][nCntWidth]->SetPosition(D3DXVECTOR3(100.0f + nCntWidth * 55.0f,
					100.0f + nCntDepth * 55.0f,
					0.0f));
				// サイズ
				m_pBlock[nCntDepth][nCntWidth]->SetSize(D3DXVECTOR3(50.0f, 50.0f, 0.0f));
				// 色
				m_pBlock[nCntDepth][nCntWidth]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
				// 反映
				m_pBlock[nCntDepth][nCntWidth]->SetTransform();
			}
		}
	}

	// 最初のピース生成
	m_pPiece[m_nPieceNum] = CPiece::Create();
	m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);


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

	if (m_pPiece[m_nPieceNum] != NULL)
	{
		// 配置情報取得
		m_bPiece = m_pPiece[m_nPieceNum]->GetMove();
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

		// 生成
		if (pKeyboard->GetTriggerKeyboard(DIK_V))
		{
			// ピース数加算
			m_nPieceNum++;
			// ピース生成
			m_pPiece[m_nPieceNum] = CPiece::Create();
			// ピースタイプ設定
			m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle);
			// 配置情報
			m_pPiece[m_nPieceNum]->SetMove(false);
		}
		// 生成
		if (pKeyboard->GetTriggerKeyboard(DIK_B))
		{
			// ピース数加算
			m_nPieceNum++;
			// ピース生成
			m_pPiece[m_nPieceNum] = CPiece::Create();
			// ピースタイプ設定
			m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_T_Type);
			// 配置情報
			m_pPiece[m_nPieceNum]->SetMove(false);
		}
		// 生成
		if (pKeyboard->GetTriggerKeyboard(DIK_X))
		{
			// ピース数加算
			m_nPieceNum++;
			// ピース生成
			m_pPiece[m_nPieceNum] = CPiece::Create();
			// ピースタイプ設定
			m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Delete);
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
		// ピース生成
		
		//m_pPiece[m_nPieceNum]->SetPlaacement(false);
	}
	else
	{
		// Zを押されたら
		if (pKeyboard->GetTriggerKeyboard(DIK_Z))
		{
			// チェンジ
			if (m_nCntChange == 0)
			{
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Square);
				// カウント加算
				m_nCntChange++;
			}
			else if (m_nCntChange == 1)
			{
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Rectangle);
				// カウント加算
				m_nCntChange++;
			}
			else if (m_nCntChange == 2)
			{
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_T_Type);
				// カウント加算
				m_nCntChange++;
			}
			else if (m_nCntChange == 3)
			{
				// タイプ変更
				m_pPiece[m_nPieceNum]->SetPieceType(CPiece::PieceType_Delete);
				// カウント初期化
				m_nCntChange = 0;
			}
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