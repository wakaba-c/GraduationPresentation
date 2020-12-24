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
#include "sound.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
int CCharacterSelect::m_nCarType = 0;			// 車のタイプ

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
	for (int nCnt = 0; nCnt < MAX_SELECT_UI; nCnt++)
	{
		pBack[nCnt] = CScene2D::Create(CScene::PRIORITY_UI);
	}
	if (pBack[0] != NULL)//Selectのロゴ
	{
		pBack[0]->BindTexture("data/tex/Select.png");
		pBack[0]->SetPosition(D3DXVECTOR3(970, 640, 0.0f));
		pBack[0]->SetSize(D3DXVECTOR3(130, 130, 0.0f));
		pBack[0]->SetTransform();
	}
	if (pBack[1] != NULL)//Decideのロゴ
	{
		pBack[1]->BindTexture("data/tex/Decide.png");
		pBack[1]->SetPosition(D3DXVECTOR3(1150, 640, 0.0f));
		pBack[1]->SetSize(D3DXVECTOR3(130, 130, 0.0f));
		pBack[1]->SetTransform();
	}
	if (pBack[2] != NULL)//車１
	{
		pBack[2]->BindTexture("data/tex/car01.jpg");
		pBack[2]->SetPosition(D3DXVECTOR3(SCREEN_WIDTH/2 - 240, 520, 0.0f));
		pBack[2]->SetSize(D3DXVECTOR3(150, 130, 0.0f));
		pBack[2]->SetTransform();
	}
	if (pBack[3] != NULL)//車２
	{
		pBack[3]->BindTexture("data/tex/car02.jpg");
		pBack[3]->SetPosition(D3DXVECTOR3(SCREEN_WIDTH / 2 - 80 , 520, 0.0f));
		pBack[3]->SetSize(D3DXVECTOR3(150, 130, 0.0f));
		pBack[3]->SetTransform();
	}
	if (pBack[4] != NULL)//車３
	{
		pBack[4]->BindTexture("data/tex/car03.jpg");
		pBack[4]->SetPosition(D3DXVECTOR3(SCREEN_WIDTH / 2 + 80 , 520, 0.0f));
		pBack[4]->SetSize(D3DXVECTOR3(150, 130, 0.0f));
		pBack[4]->SetTransform();
	}
	if (pBack[5] != NULL)//車４
	{
		pBack[5]->BindTexture("data/tex/car04.jpg");
		pBack[5]->SetPosition(D3DXVECTOR3(SCREEN_WIDTH / 2 + 240, 520, 0.0f));
		pBack[5]->SetSize(D3DXVECTOR3(150, 130, 0.0f));
		pBack[5]->SetTransform();
	}

	if (pBack[6] != NULL)//CarSelectのロゴ
	{
		pBack[6]->BindTexture("data/tex/CarSelect.png");
		pBack[6]->SetPosition(D3DXVECTOR3(SCREEN_WIDTH / 2, 70, 0.0f));
		pBack[6]->SetSize(D3DXVECTOR3(470, 85, 0.0f));
		pBack[6]->SetTransform();
	}
	if (pBack[7] != NULL)//セレクトするときの枠　※一番最後に描画する事！
	{
		pBack[7]->BindTexture("data/tex/SelectFrame.png");
		pBack[7]->SetPosition(D3DXVECTOR3(SCREEN_WIDTH / 2 - 240, 520, 0.0f));
		pBack[7]->SetSize(D3DXVECTOR3(150, 130, 0.0f));
		pBack[7]->SetTransform();
	}

	// 左スティックの倒れた状況初期化
	for (int nCnt = 0; nCnt < STICK_MAX; nCnt++)
	{
		m_bStick[nCnt] = false;
	}

	// 空の作成
	CSky::Create();
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CCharacterSelect::Uninit(void)
{
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
				CSound *pSound = CManager::GetSound();				// サウンドの取得

				pSound->PlaySoundA(SOUND_LABEL_SE_Decision);			// ダメージ音の再生
				CFade::SetFade(CManager::MODE_PUZZLE_CUSTOM, CFade::FADETYPE_SLIDE);					//フェードを入れる
			}
			//車の選択処理
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
			float nValueH = 0;				// コントローラー
			float nValueV = 0;				// コントローラー

			// 左スティックの情報取得
			pInputController->GetJoypadStickLeft(0, &nValueH, &nValueV);

			// 左スティックが倒れていないとき
			if (!m_bStick[STICK_LEFT] && !m_bStick[STICK_RIGHT])
			{
				// 左にスティックが倒れたとき
				if (nValueH > 0)
				{
					// 左スティックが倒れた
					m_bStick[STICK_LEFT] = true;
				}
				else if (nValueH < 0)
				{// 右にスティックが倒れたとき
				 // 右スティックが倒れた
					m_bStick[STICK_RIGHT] = true;
				}

				// 左スティックの倒れた状況がtrueのとき
				if (m_bStick[STICK_LEFT] && m_nCarType > 0)
				{
					pos = pBack[7]->GetPosition();
					pBack[7]->SetPosition(D3DXVECTOR3(pos.x - 160, pos.y, pos.z));
					pBack[7]->SetTransform();
					m_nCarType--;
				}
				else if (m_bStick[STICK_RIGHT] && m_nCarType< 3)
				{// 右スティックの倒れた状況がtrueのとき
					pos = pBack[7]->GetPosition();
					pBack[7]->SetPosition(D3DXVECTOR3(pos.x + 160, pos.y, pos.z));
					pBack[7]->SetTransform();
					m_nCarType++;
				}
			}

			// スティックが倒れていない
			if (nValueH == 0)
			{
				// 左スティックが倒れていない
				m_bStick[STICK_LEFT] = false;
				// 右スティックが倒れていない
				m_bStick[STICK_RIGHT] = false;
			}

			//ゲームの遷移
			if (pInputController->GetControllerTrigger(0, JOYPADKEY_A) ||			// ゲームパッドのAボダンが押されたとき
				pInputController->GetControllerTrigger(0, JOYPADKEY_START))			// ゲームパッドのSTARTボタンが押されたとき
			{
				CSound *pSound = CManager::GetSound();				// サウンドの取得

				pSound->PlaySoundA(SOUND_LABEL_SE_Decision);			// ダメージ音の再生

				CFade::SetFade(CManager::MODE_PUZZLE_CUSTOM, CFade::FADETYPE_SLIDE);					//フェードを入れる
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
}