//=============================================================================
//
// リザルト処理 [result.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "result.h"
#include "fade.h"
#include "manager.h"
#include "scene.h"
#include "inputKeyboard.h"
#include "inputController.h"
#include "bg.h"
#include "number.h"
#include "ranking.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MAX_MAGNIFICATION 5						// 倍率

//=============================================================================
// 静的メンバ変数
//=============================================================================
int CResult::m_nKill = 0;						// 倒した数
int CResult::m_nSeconds = 0;					// 秒数
int CResult::m_nMinutes = 0;					// 分数

//=============================================================================
// コンストラクタ
//=============================================================================
CResult::CResult()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CResult::~CResult()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CResult::Init(void)
{
	int nTotal = 0;

	CBg::Create();			// 背景の作成

	// =============== タイトル ================ //
	CScene2D *pScene2D = CScene2D::Create(CScene::PRIORITY_UI);

	if (pScene2D != NULL)
	{// pScene2Dが存在していたとき
		pScene2D->BindTexture("data/tex/result.png");		// 画像の読み込み
		pScene2D->SetPosition(D3DXVECTOR3(300.0f, 150.0f, 0.0f));					// 位置設定
		pScene2D->SetSize(D3DXVECTOR3(500.0f, 150.0f, 0.0f));						// 大きさ設定
		pScene2D->SetTransform();													// 頂点情報の更新
	}

	CRanking::SetResultIndex(m_nKill * ((MAX_MAGNIFICATION - m_nMinutes) * 100));	// ランキングに今回の得点を送る
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CResult::Uninit(void)
{
	m_nKill = 0;		// Kill数を初期化
	m_nSeconds = 0;		// 秒数を初期化
	m_nMinutes = 0;		// 分数を初期化

	// ポリゴンの開放
	CScene::ReleaseAll();
}

//=============================================================================
// 更新処理
//=============================================================================
void CResult::Update(void)
{
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();
	CInputController *pInputController = CManager::GetInputController();

	if (CFade::GetFade() == CFade::FADE_NONE)
	{//フェードが処理をしていないとき
		if (pInputKeyboard != NULL)
		{// キーボードが存在していたとき
			if (pInputKeyboard->GetTriggerKeyboard(DIK_RETURN))
			{// 指定のキーが押されたとき
				CFade::SetFade(CManager::MODE_RANKING);					//フェードを入れる
			}
		}
		if (pInputController->GetJoypadUse(0))
		{// コントローラーが生成されているとき
		 //ゲームの遷移
			if (pInputController->GetControllerTrigger(0, JOYPADKEY_A) ||			// ゲームパッドのAボダンが押されたとき
				pInputController->GetControllerTrigger(0, JOYPADKEY_START))			// ゲームパッドのSTARTボタンが押されたとき
			{
				CFade::SetFade(CManager::MODE_RANKING);					//フェードを入れる
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CResult::Draw(void)
{

}

//=============================================================================
// アセットの読み込み
//=============================================================================
void CResult::LoadAsset(void)
{
	CBg::Load();
	CNumber::Load();
}

//=============================================================================
// 敵の討伐数の設定
//=============================================================================
void CResult::SetIdxKill(int nValue)
{
	m_nKill = nValue;
}

//=============================================================================
// 秒数の設定
//=============================================================================
void CResult::SetSeconds(int nValue)
{
	m_nSeconds = nValue;
}

//=============================================================================
// 分数の設定
//=============================================================================
void CResult::SetMinutes(int nValue)
{
	m_nMinutes = nValue;
}