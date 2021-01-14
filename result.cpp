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
#include "sound.h"
#include "camera.h"
#include "sky.h"
#include "object.h"

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
	CCamera *pCamera = CManager::GetCamera();		// カメラの取得

	int nTotal = 0;

	// 空の作成
	CSky::Create();

	// モデル情報の読み込み
	CObject::LoadModelTest("data/text/model.txt");

	// 表彰台を生成
	CObject *pObj = CObject::Create();

	if (pObj != NULL)
	{
		pObj->BindModel("data/model/Podium.x");
		pObj->SetPosition(D3DXVECTOR3(14013.69f, -3849.46f, -16564.68f));
		pObj->SetRotation(D3DXVECTOR3(0.0f, 1.10f, 0.0f));
		pObj->SetSize(D3DXVECTOR3(2.0f, 2.0f, 2.0f));
	}

	if (pCamera != NULL)
	{
		pCamera->SetStoker(false);
		pCamera->SetPosCamera(D3DXVECTOR3(13951.36f, -3800.70f, -16626.40f), D3DXVECTOR3(-0.12f, -1.88f, 0.0f));
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
				CSound *pSound = CManager::GetSound();				// サウンドの取得

				pSound->PlaySoundA(SOUND_LABEL_SE_Decision);			// ダメージ音の再生

				CFade::SetFade(CManager::MODE_TITLE, CFade::FADETYPE_SLIDE);					//フェードを入れる
			}
		}
		if (pInputController->GetJoypadUse(0))
		{// コントローラーが生成されているとき
		 //ゲームの遷移
			if (pInputController->GetControllerTrigger(0, JOYPADKEY_A) ||			// ゲームパッドのAボダンが押されたとき
				pInputController->GetControllerTrigger(0, JOYPADKEY_START))			// ゲームパッドのSTARTボタンが押されたとき
			{
				CSound *pSound = CManager::GetSound();				// サウンドの取得

				pSound->PlaySoundA(SOUND_LABEL_SE_Decision);			// ダメージ音の再生

				CFade::SetFade(CManager::MODE_TITLE, CFade::FADETYPE_SLIDE);					//フェードを入れる
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