//=============================================================================
//
// 時速処理 [speed.cpp]
// Author : Seiya Takahashi
//
//=============================================================================
#include "speed.h"
#include "game.h"
#include "manager.h"
#include "renderer.h"
#include "number.h"
#include "player.h"
#include "takaseiLibrary.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define INTERVAL 33.0f		// 数字と数字の間

//=============================================================================
// 静的メンバ変数の初期化
//=============================================================================
float CSpeed::m_fDigit = 0.0f;

//=============================================================================
// コンストラクタ
//=============================================================================
CSpeed::CSpeed(CScene::PRIORITY obj = CScene::PRIORITY_UI) : CScene(obj)
{
	SetObjType(PRIORITY_UI);	//オブジェクトタイプ

	//値の初期化
	m_dTimeFrame = 0;			// フレーム数の初期化

	// 最大桁数までカウント
	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// 生成処理
		m_apNumber[nCnt] = CNumber::Create();

		m_apNumber[nCnt]->SetPosition(D3DXVECTOR3(50 + INTERVAL * nCnt, 700.0f, 0));	// 位置設定
		m_apNumber[nCnt]->SetSize(D3DXVECTOR3(INTERVAL, 60, 0));						// 大きさ設定
		m_apNumber[nCnt]->Init();														// 初期化処理
	}
}

//=============================================================================
// デストラクタ
//=============================================================================
CSpeed::~CSpeed()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CSpeed::Init(void)
{
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CSpeed::Uninit(void)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CSpeed::Update(void)
{
	// プレイヤー情報取得
	CPlayer *pPlayer = CGame::GetPlayer();
	D3DXVECTOR3 pos, posOld;		// 現在の位置、前回の位置
	float fDistance;				// 距離

	// プレイヤーがいるとき
	if (pPlayer != NULL)
	{
		// プレイヤーの位置取得
		pos = pPlayer->GetPosition();

		// 現在の位置を前回の位置に代入
		posOld = pPlayer->GetPosOld();;
	}

	m_dTimeFrame++;				// カウンターに1加算する

	// 現在と前回との距離計算
	fDistance = CTakaseiLibrary::OutputDistance(posOld, pos);
	
	// 距離÷時間	fDigit = fDistance / m_dTimeFrame;
	m_fDigit = fDistance;

	// 時速設定
	SetDigit(m_fDigit);

	// カウンタが0以上のとき
	if (m_fDigit >= 0)
	{
		SetTime((int)m_fDigit);		// タイムの更新
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CSpeed::Draw(void)
{
	// 最大桁数までカウント
	for (int nCntScore = 0; nCntScore < MAX_DIGIT; nCntScore++)
	{
		// 数字があるとき
		if (m_apNumber[nCntScore] != NULL)
		{
			// 描画処理
			m_apNumber[nCntScore]->Draw();
		}
	}
}

//=============================================================================
// クリエイト処理
//=============================================================================
CSpeed *CSpeed::Create(void)
{
	CSpeed *pSpeed;

	pSpeed = new CSpeed(CScene::PRIORITY_UI);
	pSpeed->Init();
	return pSpeed;
}

//=============================================================================
// タイムの変更
//=============================================================================
void CSpeed::SetTime(int nTime)
{
	int nNumber;

	// タイムをフレームで割る
	nTime = (int)(nTime / 1.0f);

	// 最大桁数までカウント
	for (int nCount = 0; nCount < MAX_DIGIT; nCount++)
	{
		// 表示する数字計算
		nNumber = nTime % (int)powf(10.0f, (float)MAX_DIGIT - nCount) / (int)powf(10.0f, MAX_DIGIT - 1.0f - nCount);

		// 数字設定
		m_apNumber[nCount]->SetNumber(nNumber);
	}
}
