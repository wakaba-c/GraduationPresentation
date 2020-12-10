//=============================================================================
//
// キャラ選択処理 [characterSelect.cpp]
// Author : Noriyuki Hanzawa
//
//=============================================================================
#include "fade.h"
#include "renderer.h"
#include "scene.h"
#include "manager.h"
#include "inputKeyboard.h"
#include "characterSelect.h"
#include "bg.h"
#include "inputController.h"
#include "object.h"
#include "camera.h"
#include "sky.h"
#include "titleLogo.h"
//=============================================================================
// 静的メンバ変数
//=============================================================================
CTitlelogo	*CCharacterSelect::m_pTitleLogo = NULL;		// タイトルロゴのポインタ

//=============================================================================
// コンストラクタ
//=============================================================================
CCharacterSelect::CCharacterSelect()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CCharacterSelect::~CCharacterSelect()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CCharacterSelect::Init(void)
{
	m_nCarType = 0;
	for (int nCnt = 0; nCnt < MAX_UI; nCnt++)
	{
		pBack[nCnt] = CScene2D::Create(CScene::PRIORITY_UI);
	}
	if (pBack[0] != NULL)
	{
		pBack[0]->BindTexture("data/tex/Select.png");
		pBack[0]->SetPosition(D3DXVECTOR3(970, 640, 0.0f));
		pBack[0]->SetSize(D3DXVECTOR3(130, 130, 0.0f));
		pBack[0]->SetTransform();
	}
	if (pBack[1] != NULL)
	{
		pBack[1]->BindTexture("data/tex/Decide.png");
		pBack[1]->SetPosition(D3DXVECTOR3(1150, 640, 0.0f));
		pBack[1]->SetSize(D3DXVECTOR3(130, 130, 0.0f));
		pBack[1]->SetTransform();
	}
	if (pBack[2] != NULL)
	{
		pBack[2]->BindTexture("data/tex/silhouette.png");
		pBack[2]->SetPosition(D3DXVECTOR3(SCREEN_WIDTH/2 - 240, 520, 0.0f));
		pBack[2]->SetSize(D3DXVECTOR3(150, 130, 0.0f));
		pBack[2]->SetTransform();
	}
	if (pBack[3] != NULL)
	{
		//pBack[3]->BindTexture("data/tex/Decide.png");
		pBack[3]->SetPosition(D3DXVECTOR3(SCREEN_WIDTH / 2 - 80 , 520, 0.0f));
		pBack[3]->SetSize(D3DXVECTOR3(150, 130, 0.0f));
		pBack[3]->SetTransform();
	}	
	if (pBack[4] != NULL)
	{
		//pBack[4]->BindTexture("data/tex/Decide.png");
		pBack[4]->SetPosition(D3DXVECTOR3(SCREEN_WIDTH / 2 + 80 , 520, 0.0f));
		pBack[4]->SetSize(D3DXVECTOR3(150, 130, 0.0f));
		pBack[4]->SetTransform();
	}
	if (pBack[5] != NULL)
	{
		//pBack[5]->BindTexture("data/tex/Decide.png");
		pBack[5]->SetPosition(D3DXVECTOR3(SCREEN_WIDTH / 2 + 240, 520, 0.0f));
		pBack[5]->SetSize(D3DXVECTOR3(150, 130, 0.0f));
		pBack[5]->SetTransform();
	}

	if (pBack[6] != NULL)
	{
		pBack[6]->BindTexture("data/tex/CarSelect.png");
		pBack[6]->SetPosition(D3DXVECTOR3(SCREEN_WIDTH / 2, 70, 0.0f));
		pBack[6]->SetSize(D3DXVECTOR3(470, 85, 0.0f));
		pBack[6]->SetTransform();
	}
	if (pBack[7] != NULL)
	{
		pBack[7]->BindTexture("data/tex/SelectFrame.png");
		pBack[7]->SetPosition(D3DXVECTOR3(SCREEN_WIDTH / 2 - 240, 520, 0.0f));
		pBack[7]->SetSize(D3DXVECTOR3(150, 130, 0.0f));
		pBack[7]->SetTransform();
	}

	//// 空の作成
	CSky::Create();
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CCharacterSelect::Uninit(void)
{
	// タイトルロゴの開放
	if (m_pTitleLogo != NULL)
	{// 存在していたとき
		m_pTitleLogo->Uninit();			// 開放処理
		delete m_pTitleLogo;			// 削除
		m_pTitleLogo = NULL;			// NULLの代入
	}

	//ポリゴンの開放
	CScene::ReleaseAll();
}

//=============================================================================
// 更新処理
//=============================================================================
void CCharacterSelect::Update(void)
{
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();
	CInputController *pInputController = CManager::GetInputController();
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	if (CFade::GetFade() == CFade::FADE_NONE)
	{//フェードが処理をしていないとき
		if (pInputKeyboard != NULL)
		{// キーボードが存在していたとき
			if (pInputKeyboard->GetTriggerKeyboard(DIK_RETURN))
			{// 指定のキーが押されたとき
				CFade::SetFade(CManager::MODE_PUZZLE_CUSTOM);					//フェードを入れる
			}
			if (pInputKeyboard->GetTriggerKeyboard(DIK_RIGHT)&&m_nCarType< 3)
			{
				pos = pBack[7]->GetPosition();
				pBack[7]->SetPosition(D3DXVECTOR3(pos.x + 160, pos.y, pos.z));
				pBack[7]->SetTransform();
				m_nCarType++;
			}
			else if (pInputKeyboard->GetTriggerKeyboard(DIK_LEFT) && m_nCarType > 0)
			{
				pos = pBack[7]->GetPosition();
				pBack[7]->SetPosition(D3DXVECTOR3(pos.x - 160, pos.y, pos.z));
				pBack[7]->SetTransform();
				m_nCarType--;
			}
		}
		if (pInputController->GetJoypadUse(0))
		{// コントローラーが生成されているとき
		 //ゲームの遷移
			if (pInputController->GetControllerTrigger(0, JOYPADKEY_A) ||			// ゲームパッドのAボダンが押されたとき
				pInputController->GetControllerTrigger(0, JOYPADKEY_START))			// ゲームパッドのSTARTボタンが押されたとき
			{
				CFade::SetFade(CManager::MODE_PUZZLE_CUSTOM);					//フェードを入れる
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CCharacterSelect::Draw(void)
{

}

//=============================================================================
// アセットの読み込み
//=============================================================================
void CCharacterSelect::LoadAsset(void)
{
	CTitlelogo::Load();
}