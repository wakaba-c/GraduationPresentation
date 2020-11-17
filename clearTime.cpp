//=============================================================================
//
// クリアタイム処理 [clearTime.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "clearTime.h"
#include "manager.h"
#include "renderer.h"
#include "number.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
LPDIRECT3DTEXTURE9 CClearTime::m_pTexture = NULL;		// テクスチャ情報の初期化

//=============================================================================
// コンストラクタ
//=============================================================================
CClearTime::CClearTime(CScene::PRIORITY obj = CScene::PRIORITY_UI) : CScene2D(obj)
{
	SetObjType(PRIORITY_UI);				// オブジェクトタイプ

	// 値の初期化
	m_nNumber = 0;
	m_pScene2D = NULL;
}

//=============================================================================
// デストラクタ
//=============================================================================
CClearTime::~CClearTime()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CClearTime::Init(void)
{
	// テクスチャの割当
	BindTexture("data/tex/cleartime.png");

	// テクスチャの中心位置の設定
	SetCenter(TEXTUREVTX_CENTER);

	D3DXVECTOR3 pos;				// 位置設定用変数

	// 番号表示
	for (int nCntNumber = 0; nCntNumber < MAX_CLEARTIME_NUMBER; nCntNumber++)
	{
		m_apNumber[nCntNumber] = CNumber::Create();
		pos = D3DXVECTOR3(150.0f + 38 * nCntNumber, 150.0f, 0.0f);

		if (nCntNumber > 1)
		{
			pos.x += 50;
		}

		m_apNumber[nCntNumber]->SetPosition(pos);
		m_apNumber[nCntNumber]->Init();
	}

	CScene2D::Init();

	// スコアの設定
	SetNumber(4);
	SetSize(D3DXVECTOR3(120.0f, 210.0f, 0.0f));
	SetPosition(D3DXVECTOR3(300, 100.0f, 0.0f));
	SetTransform();

	m_pScene2D = CScene2D::Create(PRIORITY_UI);

	if (m_pScene2D != NULL)
	{
		m_pScene2D->BindTexture("data/tex/colon.png");
		m_pScene2D->SetPosition(D3DXVECTOR3(150.0f + 38.0f * MAX_CLEARTIME_NUMBER / 2, 150.0f, 0.0f));
		m_pScene2D->SetSize(D3DXVECTOR3(120.0f, 210.0f, 0.0f));
		m_pScene2D->SetTransform();
	}
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CClearTime::Uninit(void)
{
	//UIの開放
	CScene2D::Uninit();
}

//=============================================================================
// 更新処理
//=============================================================================
void CClearTime::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void CClearTime::Draw(void)
{
	CScene2D::Draw();
}

//=============================================================================
// クリエイト処理
//=============================================================================
CClearTime *CClearTime::Create(void)
{
	CClearTime *pClearTime;
	pClearTime = new CClearTime(PRIORITY_UI);

	if (pClearTime != NULL)
	{// クリアタイムが存在していたとき
		pClearTime->Init();			// 初期化
	}
	return pClearTime;
}

//=============================================================================
// テクスチャのロード関数
//=============================================================================
HRESULT CClearTime::Load(void)
{
	// テクスチャ読み込み
	CManager::Load("data/tex/cleartime.png");
	CManager::Load("data/tex/colon.png");
	return S_OK;
}

//=============================================================================
// 数字の位置設定
//=============================================================================
void CClearTime::SetNumberPos(D3DXVECTOR3 pos, float fInterval)
{
	D3DXVECTOR3 workPos;				// 位置設定用変数

	// 数字の位置設定
	for (int nCntNumber = 0; nCntNumber < MAX_CLEARTIME_NUMBER; nCntNumber++)
	{
		if (m_apNumber[nCntNumber] != NULL)
		{
			workPos = D3DXVECTOR3(pos.x + fInterval * nCntNumber, pos.y, 0.0f);		// 位置設定

			if (nCntNumber > 1)
			{// 1以上だったとき
				workPos.x += 50;
			}

			m_apNumber[nCntNumber]->SetPosition(workPos);		// 位置設定
			m_apNumber[nCntNumber]->SetTransform();				// 頂点情報の更新
		}
	}

	if (m_pScene2D != NULL)
	{
		m_pScene2D->BindTexture("data/tex/colon.png");
		m_pScene2D->SetPosition(D3DXVECTOR3(pos.x + fInterval * MAX_CLEARTIME_NUMBER / 2, pos.y, 0.0f));
		m_pScene2D->SetTransform();
	}
}

//=============================================================================
// 数字のサイズ設定
//=============================================================================
void CClearTime::SetNumderSize(D3DXVECTOR3 size)
{
	// 数字の位置設定
	for (int nCntNumber = 0; nCntNumber < MAX_CLEARTIME_NUMBER; nCntNumber++)
	{
		if (m_apNumber[nCntNumber] != NULL)
		{// 数字が存在していたとき
			m_apNumber[nCntNumber]->SetSize(size);		// サイズの設定
			m_apNumber[nCntNumber]->SetTransform();		// 頂点情報の更新
		}
	}
}

//=============================================================================
// 数の更新
//=============================================================================
void CClearTime::SetNumber(int nValue)
{
	int nNumber;
	m_nNumber = nValue;

	for (int nCntNumber = 0; nCntNumber < MAX_CLEARTIME_NUMBER; nCntNumber++)
	{
		nNumber = (int)nValue % (int)pow(10, MAX_CLEARTIME_NUMBER - nCntNumber) / (int)pow(10, MAX_CLEARTIME_NUMBER - 1 - nCntNumber);
		if (m_apNumber[nCntNumber] != NULL)
		{
			m_apNumber[nCntNumber]->SetNumber(nNumber);
		}
	}
}

//=============================================================================
// 数の加算
//=============================================================================
void CClearTime::AddNumber(int nValue)
{
	m_nNumber += nValue;

	SetNumber(m_nNumber);
}