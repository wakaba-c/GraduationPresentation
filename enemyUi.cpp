//=============================================================================
//
// 敵の数表示処理 [enemyUi.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "enemyUi.h"
#include "manager.h"
#include "renderer.h"
#include "number.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
LPDIRECT3DTEXTURE9 CEnemyUi::m_pTexture = NULL;		// テクスチャ情報のポインタ

//=============================================================================
// コンストラクタ
//=============================================================================
CEnemyUi::CEnemyUi(CScene::PRIORITY obj = CScene::PRIORITY_UI) : CScene2D(obj)
{
	SetObjType(PRIORITY_UI);				//オブジェクトタイプ

	//値の初期化
	m_nNumber = 0;
}

//=============================================================================
// デストラクタ
//=============================================================================
CEnemyUi::~CEnemyUi()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CEnemyUi::Init(void)
{
	//テクスチャの割当
	BindTexture("data/tex/enemy.png");

	//テクスチャの中心位置の設定
	SetCenter(TEXTUREVTX_CENTER);

	//番号表示
	for (int nCntNumber = 0; nCntNumber < MAX_ENEMY_NUMBER; nCntNumber++)
	{
		m_apNumber[nCntNumber] = CNumber::Create();

		if (m_apNumber[nCntNumber] != NULL)
		{// 数字が存在していたとき
			m_apNumber[nCntNumber]->SetPosition(D3DXVECTOR3(150.0f + 38 * nCntNumber, 150.0f, 0.0f));		// 位置の設定
			m_apNumber[nCntNumber]->SetSize(D3DXVECTOR3(50.0f, 100.0f, 0.0f));								// サイズの設定
			m_apNumber[nCntNumber]->Init();																	// 初期化処理
		}
	}

	// 初期化処理
	CScene2D::Init();

	//スコアの設定
	SetNumber(4);									// 数字の設定
	SetSize(D3DXVECTOR3(120.0f, 210.0f, 0.0f));		// サイズの設定
	SetPosition(D3DXVECTOR3(300, 100.0f, 0.0f));	// 位置の設定
	SetTransform();									// 頂点情報の更新
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CEnemyUi::Uninit(void)
{
	// 開放
	CScene2D::Uninit();
}

//=============================================================================
// 更新処理
//=============================================================================
void CEnemyUi::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void CEnemyUi::Draw(void)
{
	// 描画
	CScene2D::Draw();
}

//=============================================================================
// クリエイト処理
//=============================================================================
CEnemyUi *CEnemyUi::Create(void)
{
	CEnemyUi *pEnemyUi;
	pEnemyUi = new CEnemyUi(PRIORITY_UI);

	if (pEnemyUi != NULL)
	{// 敵が存在していたとき
		pEnemyUi->Init();		// 初期化処理
	}
	return pEnemyUi;
}

//=============================================================================
// ロード処理
//=============================================================================
HRESULT CEnemyUi::Load(void)
{
	// テクスチャの読み込み
	CManager::Load("data/tex/SignBoard3.png");
	CManager::Load("data/tex/enemy.png");
	return S_OK;
}

//=============================================================================
// 数字の位置設定
//=============================================================================
void CEnemyUi::SetNumberPos(D3DXVECTOR3 pos, float fInterval)
{
	// 数字の位置設定
	for (int nCntNumber = 0; nCntNumber < MAX_ENEMY_NUMBER; nCntNumber++)
	{
		if (m_apNumber[nCntNumber] != NULL)
		{// 数が存在していたとき
			m_apNumber[nCntNumber]->SetPosition(D3DXVECTOR3(pos.x + fInterval * nCntNumber, pos.y, 0.0f));		// 位置の設定
			m_apNumber[nCntNumber]->MakeVertex();																// 頂点情報の更新
		}
	}
}

//=============================================================================
// サイズの設定
//=============================================================================
void CEnemyUi::SetNumderSize(D3DXVECTOR3 size)
{
	// 数字の位置設定
	for (int nCntNumber = 0; nCntNumber < MAX_ENEMY_NUMBER; nCntNumber++)
	{
		if (m_apNumber[nCntNumber] != NULL)
		{// 数字が存在していたとき
			m_apNumber[nCntNumber]->SetSize(size);			// サイズの設定
			m_apNumber[nCntNumber]->MakeVertex();			// 頂点情報の更新
		}
	}
}

//=============================================================================
// 数の更新
//=============================================================================
void CEnemyUi::SetNumber(int nValue)
{
	int nNumber;
	m_nNumber = nValue;

	// 数字の更新
	for (int nCntNumber = 0; nCntNumber < MAX_ENEMY_NUMBER; nCntNumber++)
	{
		nNumber = (int)nValue % (int)pow(10, MAX_ENEMY_NUMBER - nCntNumber) / (int)pow(10, MAX_ENEMY_NUMBER - 1 - nCntNumber);

		if (m_apNumber[nCntNumber] != NULL)
		{// 数字が存在していたとき
			m_apNumber[nCntNumber]->SetNumber(nNumber);			// 数の設定
		}
	}
}

//=============================================================================
// 数の加算
//=============================================================================
void CEnemyUi::AddNumber(int nValue)
{
	m_nNumber += nValue;			// 加算
	SetNumber(m_nNumber);			// 更新
}