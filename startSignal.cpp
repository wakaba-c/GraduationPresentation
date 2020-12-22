//=============================================================================
//
// 数字処理 [number.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "startSignal.h"
#include "manager.h"
#include "renderer.h"
#include "time.h"
#include "ui.h"
#include "counter.h"
#include "game.h"
#include "player.h"
#include "sound.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CStartSignal::CStartSignal(CScene::PRIORITY obj = CScene::PRIORITY_UI) : CScene(obj)
{
	m_pTime = NULL;
	m_pUi = NULL;
	m_bEnd = false;
}

//=============================================================================
// デストラクタ
//=============================================================================
CStartSignal::~CStartSignal()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CStartSignal::Init(void)
{
	m_pTime = CTime::Create();

	if (m_pTime != NULL)
	{
		m_pTime->SetSeconds(3);
		m_pTime->SetUpdateTimer(false);
	}

	m_pUi = CUi::Create();

	if (m_pUi != NULL)
	{
		m_pUi->LoadScript("data/text/ui/NowRankMove.txt");
		m_pUi->SetPosition(D3DXVECTOR3(640.0f, -50.0f, 0.0f));

		CCounter *pCounter = m_pUi->GetCounter("time");

		if (pCounter != NULL)
		{
			pCounter->SetNumber(m_pTime->GetSeconds());
		}
	}
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CStartSignal::Uninit(void)
{
	if (m_pTime != NULL)
	{
		m_pTime->Uninit();
		m_pTime->Release();
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CStartSignal::Update(void)
{
	if(m_pUi == NULL) return;

	if(m_pTime == NULL) return;

	if (m_pUi->GetOpType() == CUi::OPERATIONTYPE_NONE)
	{
		if (!m_bEnd)
		{
			m_pTime->SetUpdateTimer(true);
			m_bEnd = true;
		}
		CCounter *pCounter = m_pUi->GetCounter("time");

		if (pCounter != NULL)
		{
			pCounter->SetNumber(m_pTime->GetSeconds());
		}

		if (m_pTime->GetSeconds() <= 0)
		{
			CGame::GetPlayer()->SetEvent(false);
			CSound *pSound = CManager::GetSound();				// サウンドの取得

			pSound->PlaySoundA(SOUND_LABEL_SE_RaceStart);			// ダメージ音の再生
			pSound->PlaySoundA(SOUND_LABEL_SE_RaceStart_Overlapping);			// ダメージ音の再生


			m_pUi->SetNextOperation();
			Release();
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CStartSignal::Draw(void)
{
}

//=============================================================================
// クリエイト処理
//=============================================================================
CStartSignal *CStartSignal::Create(void)
{
	CStartSignal *pStartSignal;
	pStartSignal = new CStartSignal(PRIORITY_UI);

	if (pStartSignal != NULL)
	{// 数字が存在していたとき
		pStartSignal->Init();					// 初期化処理
	}
	return pStartSignal;
}