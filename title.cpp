//=============================================================================
//
// タイトル処理 [title.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "fade.h"
#include "renderer.h"
#include "scene.h"
#include "manager.h"
#include "inputKeyboard.h"
#include "title.h"
#include "bg.h"
#include "inputController.h"
#include "meshField.h"
#include "object.h"
#include "camera.h"
#include "sky.h"
#include "sound.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CTitle::CTitle()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CTitle::~CTitle()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CTitle::Init(void)
{
	//CCamera *pCamera = CManager::GetCamera();		// カメラの取得

	//if (pCamera != NULL)
	//{// カメラが存在していたとき
	//	pCamera->SetPosCamera(D3DXVECTOR3(7099.40f, 187.26f, 7523.8f), D3DXVECTOR3(0.1f, -2.33f, 0.0f));		// 位置の設定
	//}

	//m_pTitleLogo = CTitlelogo::Create();		// タイトルロゴの生成
	for (int nCnt = 0; nCnt < MAX_UI; nCnt++)
	{
		pBack[nCnt] = CScene2D::Create(CScene::PRIORITY_UI);
	}
		if (pBack[0] != NULL)
		{
			pBack[0]->BindTexture("data/tex/title.png");
			pBack[0]->SetPosition(D3DXVECTOR3(300, 300, 0.0f));
			pBack[0]->SetSize(D3DXVECTOR3(600, 330, 0.0f));
			pBack[0]->SetTransform();
		}
		if (pBack[1] != NULL)
		{
			pBack[1]->BindTexture("data/tex/Press.png");
			pBack[1]->SetPosition(D3DXVECTOR3(1050, 600, 0.0f));
			pBack[1]->SetSize(D3DXVECTOR3(100, 45, 0.0f));
			pBack[1]->SetTransform();
		}
		if (pBack[2] != NULL)
		{
			pBack[2]->BindTexture("data/tex/PressLR.png");
			pBack[2]->SetPosition(D3DXVECTOR3(1050, 670, 0.0f));
			pBack[2]->SetSize(D3DXVECTOR3(200, 45, 0.0f));
			pBack[2]->SetTransform();
		}
		if (pBack[3] != NULL)
		{
			pBack[3]->BindTexture("data/tex/TeamName.png");
			pBack[3]->SetPosition(D3DXVECTOR3(190, 670, 0.0f));
			pBack[3]->SetSize(D3DXVECTOR3(250, 45, 0.0f));
			pBack[3]->SetTransform();
		}

	//// 空の作成
	CSky::Create();

	//// 各種アセットの生成＆設置
	//CMeshField::LoadRand("data/stage/rand.csv", false);				// 床情報の読込
	//CObject::LoadModel("data/stage/object.csv");					// モデル情報の読込
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CTitle::Uninit(void)
{
	//ポリゴンの開放
	CScene::ReleaseAll();
}

//=============================================================================
// 更新処理
//=============================================================================
void CTitle::Update(void)
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

				CFade::SetFade(CManager::MODE_CHARACTER_SELECT, CFade::FADETYPE_SLIDE);					//フェードを入れる
			}
		}
		if (pInputController->GetJoypadUse(0))
		{// コントローラーが生成されているとき
			//ゲームの遷移
			if (pInputController->GetControllerPress(0, JOYPADKEY_LEFT_SHOULDER) &&			// ゲームパッドのLボダンが押されたとき
				pInputController->GetControllerPress(0, JOYPADKEY_RIGHT_SHOULDER))			// ゲームパッドのRボタンが押されたとき
			{
				CSound *pSound = CManager::GetSound();				// サウンドの取得

				pSound->PlaySoundA(SOUND_LABEL_SE_Decision);			// ダメージ音の再生

				CFade::SetFade(CManager::MODE_CHARACTER_SELECT, CFade::FADETYPE_SLIDE);					//フェードを入れる
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CTitle::Draw(void)
{

}

//=============================================================================
// アセットの読み込み
//=============================================================================
void CTitle::LoadAsset(void)
{

}