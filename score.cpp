//=============================================================================
//
// スコア処理 [score.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "score.h"
#include "manager.h"
#include "renderer.h"
#include "number.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CScore::CScore(CScene::PRIORITY obj = CScene::PRIORITY_UI) : CScene2D(obj)
{
	SetObjType(PRIORITY_UI);				//オブジェクトタイプ

	//値の初期化
	m_nNumber = 0;

	for (int nCount = 0; nCount < MAX_SCORE_NUMBER; nCount++)
	{
		m_apNumber[nCount] = NULL;
	}
}

//=============================================================================
// デストラクタ
//=============================================================================
CScore::~CScore()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CScore::Init(void)
{
	//テクスチャの割当
	BindTexture("data/tex/total.png");

	//テクスチャの中心位置の設定
	SetCenter(TEXTUREVTX_CENTER);

	//番号表示
	for (int nCntNumber = 0; nCntNumber < MAX_SCORE_NUMBER; nCntNumber++)
	{
		m_apNumber[nCntNumber] = CNumber::Create();

		if (m_apNumber[nCntNumber] != NULL)
		{// 数字が存在していたとき
			m_apNumber[nCntNumber]->SetPosition(D3DXVECTOR3(150.0f + 38 * nCntNumber, 150.0f, 0.0f));		// 位置の設定
			m_apNumber[nCntNumber]->Init();																	// 初期化処理
		}
	}

	CScene2D::Init();

	//スコアの設定
	SetNumber(4);									// 数の設定
	SetSize(D3DXVECTOR3(120.0f, 210.0f, 0.0f));		// サイズの設定
	SetPosition(D3DXVECTOR3(300, 100.0f, 0.0f));	// 位置の設定
	SetTransform();									// 頂点情報の更新
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CScore::Uninit(void)
{
	//UIの開放
	CScene2D::Uninit();
}

//=============================================================================
// 更新処理
//=============================================================================
void CScore::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void CScore::Draw(void)
{
	CScene2D::Draw();
}

//=============================================================================
// クリエイト処理
//=============================================================================
CScore *CScore::Create(void)
{
	CScore *pScore;
	pScore = new CScore(PRIORITY_UI);
	pScore->Init();
	return pScore;
}

//=============================================================================
// ロード処理
//=============================================================================
HRESULT CScore::Load(void)
{
	CManager::Load("data/tex/total.png");
	return S_OK;
}

//=============================================================================
// 数字の位置設定
//=============================================================================
void CScore::SetNumberPos(D3DXVECTOR3 pos, float fInterval)
{
	// 数字の位置設定
	for (int nCntNumber = 0; nCntNumber < MAX_SCORE_NUMBER; nCntNumber++)
	{
		if (m_apNumber[nCntNumber] != NULL)
		{
			m_apNumber[nCntNumber]->SetPosition(D3DXVECTOR3(pos.x + fInterval * nCntNumber, pos.y, 0.0f));
			m_apNumber[nCntNumber]->MakeVertex();
		}
	}
}

//=============================================================================
// 数字のサイズ設定
//=============================================================================
void CScore::SetNumderSize(D3DXVECTOR3 size)
{
	// 数字の位置設定
	for (int nCntNumber = 0; nCntNumber < MAX_SCORE_NUMBER; nCntNumber++)
	{
		if (m_apNumber[nCntNumber] != NULL)
		{
			m_apNumber[nCntNumber]->SetSize(size);
			m_apNumber[nCntNumber]->MakeVertex();
		}
	}
}

//=============================================================================
// 数の更新
//=============================================================================
void CScore::SetNumber(int nValue)
{
	int nNumber;
	m_nNumber = nValue;

	// 数の更新
	for (int nCntNumber = 0; nCntNumber < MAX_SCORE_NUMBER; nCntNumber++)
	{
		nNumber = (int)nValue % (int)pow(10, MAX_SCORE_NUMBER - nCntNumber) / (int)pow(10, MAX_SCORE_NUMBER - 1 - nCntNumber);
		if (m_apNumber[nCntNumber] != NULL)
		{
			m_apNumber[nCntNumber]->SetNumber(nNumber);
		}
	}
}

//=============================================================================
// 数の加算
//=============================================================================
void CScore::AddNumber(int nValue)
{
	m_nNumber += nValue;

	SetNumber(m_nNumber);
}