//=============================================================================
//
// puzzle処理 [puzzle.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "puzzle.h"
#include "fade.h"
#include "renderer.h"
#include "scene.h"
#include "manager.h"
#include "inputKeyboard.h"
#include "inputController.h"
#include "network.h"
#include "pieceSelect.h"
#include "ui.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
float CPuzzle::m_fSpeed[Piece_Num] = {};
float CPuzzle::m_fRate[Piece_Num] = {};
float CPuzzle::m_fTurning[Piece_Num] = {};
float CPuzzle::m_fDecay[Piece_Num] = {};
int CPuzzle::m_nPower[Piece_Num] = {};
bool CPuzzle::m_bRoute[Piece_Num] = {};
bool CPuzzle::m_bRank[Piece_Num] = {};
int CPuzzle::m_nPieceNum = 0;

//=============================================================================
// コンストラクタ
//=============================================================================
CPuzzle::CPuzzle()
{
	m_pPieceSelect = NULL;
}

//=============================================================================
// デストラクタ
//=============================================================================
CPuzzle::~CPuzzle()
{

}

//=============================================================================
//Init処理
//=============================================================================
HRESULT CPuzzle::Init(void)
{
	LoadAsset();

	CUi *pUi = CUi::Create();

	if (pUi != NULL)
	{
		pUi->LoadScript("data/text/ui/puzzleUI.txt");
	}

	CBox::Create();

	m_pPieceSelect = CPieceSelect::Create();

	// 各種アセットの生成＆設置
	//CMeshField::LoadRand("data/stage/rand.csv", false);				// 床情報の読込
	//CObject::LoadModel("data/stage/object.csv");						// モデル情報の読込
	//CEnemy::LoadEnemy("data/stage/enemy.csv");						// 敵情報の読込

	return S_OK;
}

//=============================================================================
// Update関数
//=============================================================================
void CPuzzle::Update(void)
{
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();
	CInputController *pInputController = CManager::GetInputController();
	CNetwork *pNetwork = CManager::GetNetwork();

	m_nPieceNum = m_pPieceSelect->GetPieceNum();

	if (m_pPieceSelect != NULL)
	{
		for ( int nCnt = 0;nCnt < m_nPieceNum; nCnt++)
		{
			m_fSpeed[nCnt] = m_pPieceSelect->GetSpeed(nCnt);
			m_fRate[nCnt] = m_pPieceSelect->GetRate(nCnt);
			m_fTurning[nCnt] = m_pPieceSelect->GetTurning(nCnt);
			m_fDecay[nCnt] = m_pPieceSelect->GetDecay(nCnt);
			m_nPower[nCnt] = m_pPieceSelect->GetPower(nCnt);
			m_bRoute[nCnt] = m_pPieceSelect->GetRoute(nCnt);
			m_bRank[nCnt] = m_pPieceSelect->GetRank(nCnt);
		}
	}

	if (CFade::GetFade() == CFade::FADE_NONE)
	{//フェードが処理をしていないとき
		if (pInputKeyboard != NULL)
		{// キーボードが存在していたとき
			if (pInputKeyboard->GetTriggerKeyboard(DIK_RETURN))
			{// 指定のキーが押されたとき
				if (pNetwork != NULL)
				{
					//if (pNetwork->Connect() == S_OK)
					{
						CFade::SetFade(CManager::MODE_GAME, CFade::FADETYPE_SLIDE);					//フェードを入れる
					}
				}
			}
		}
		if (pInputController->GetJoypadUse(0))
		{// コントローラーが生成されているとき
		 //ゲームの遷移
			if (pInputController->GetControllerTrigger(0, JOYPADKEY_A) ||			// ゲームパッドのAボダンが押されたとき
				pInputController->GetControllerTrigger(0, JOYPADKEY_START))			// ゲームパッドのSTARTボタンが押されたとき
			{
				CFade::SetFade(CManager::MODE_GAME, CFade::FADETYPE_SLIDE);			//フェードを入れる
			}
		}
	}
}

//=============================================================================
// Draw関数
//=============================================================================
void CPuzzle::Draw(void)
{

}

//=============================================================================
// Uninit関数
//=============================================================================
void CPuzzle::Uninit(void)
{
	if (m_pPieceSelect != NULL)
	{
		m_pPieceSelect->Uninit();
		m_pPieceSelect->Release();
		m_pPieceSelect = NULL;
	}

	// ポリゴンの開放
	CScene::ReleaseAll();
}

//=============================================================================
// アセットの読み込み
//=============================================================================
void CPuzzle::LoadAsset(void)
{
	CBox::Load();
}