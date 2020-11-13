//=============================================================================
//
// タイトルロゴ処理 [titleLogo.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "titleLogo.h"
#include "manager.h"
#include "renderer.h"
#include "scene2D.h"

//==============================================================================
// コンストラクタ
//==============================================================================
CTitlelogo::CTitlelogo()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CTitlelogo::~CTitlelogo()
{

}

//==============================================================================
// 初期化処理
//==============================================================================
HRESULT CTitlelogo::Init(void)
{
	m_apScene2D[LOGOTYPE_IF] = CScene2D::Create(CScene2D::PRIORITY_UI);		// 背景 の生成

	if (m_apScene2D[LOGOTYPE_IF] != NULL)
	{// 存在していたとき
		m_apScene2D[LOGOTYPE_IF]->BindTexture("data/tex/if.png");			// 背景テクスチャ のポインタ を渡す
		m_apScene2D[LOGOTYPE_IF]->SetCenter(CScene2D::TEXTUREVTX_CENTER);		// 中心 の設定
		m_apScene2D[LOGOTYPE_IF]->SetSize(D3DXVECTOR3(100.0f, 70.0f, 0.0f));	// 大きさ の設定
		m_apScene2D[LOGOTYPE_IF]->SetRotation(D3DXVECTOR3(-0.79f, -0.79f, 0.0f));
		m_apScene2D[LOGOTYPE_IF]->SetPosition(D3DXVECTOR3(320.0f, 150.0f, 0.0f));	// 位置 の設定
		m_apScene2D[LOGOTYPE_IF]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	// 色 の設定
		m_apScene2D[LOGOTYPE_IF]->SetTransform();		// 頂点情報 の設定
	}

	m_apScene2D[LOGOTYPE_AKAZUKIN] = CScene2D::Create(CScene2D::PRIORITY_UI);		// 背景 の生成

	if (m_apScene2D[LOGOTYPE_AKAZUKIN] != NULL)
	{// 存在していたとき
		m_apScene2D[LOGOTYPE_AKAZUKIN]->BindTexture("data/tex/akazukin.png");			// 背景テクスチャ のポインタ を渡す
		m_apScene2D[LOGOTYPE_AKAZUKIN]->SetCenter(CScene2D::TEXTUREVTX_CENTER);		// 中心 の設定
		m_apScene2D[LOGOTYPE_AKAZUKIN]->SetSize(D3DXVECTOR3(500.0f, 150.0f, 0.0f));	// 大きさ の設定
		m_apScene2D[LOGOTYPE_AKAZUKIN]->SetPosition(D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4, 0.0f));	// 位置 の設定
		m_apScene2D[LOGOTYPE_AKAZUKIN]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	// 色 の設定
		m_apScene2D[LOGOTYPE_AKAZUKIN]->SetTransform();		// 頂点情報 の設定
	}

	m_apScene2D[LOGOTYPE_ATTACK] = CScene2D::Create(CScene2D::PRIORITY_UI);		// 背景 の生成

	if (m_apScene2D[LOGOTYPE_ATTACK] != NULL)
	{// 存在していたとき
		m_apScene2D[LOGOTYPE_ATTACK]->BindTexture("data/tex/attack.png");			// 背景テクスチャ のポインタ を渡す
		m_apScene2D[LOGOTYPE_ATTACK]->SetCenter(CScene2D::TEXTUREVTX_CENTER);		// 中心 の設定
		m_apScene2D[LOGOTYPE_ATTACK]->SetSize(D3DXVECTOR3(250.0f, 100.0f, 0.0f));	// 大きさ の設定
		m_apScene2D[LOGOTYPE_ATTACK]->SetPosition(D3DXVECTOR3(1000.0f, SCREEN_HEIGHT / 4 + 50, 0.0f));	// 位置 の設定
		m_apScene2D[LOGOTYPE_ATTACK]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	// 色 の設定
		m_apScene2D[LOGOTYPE_ATTACK]->SetTransform();		// 頂点情報 の設定
	}

	m_apScene2D[LOGOTYPE_PRESS] = CScene2D::Create(CScene2D::PRIORITY_UI);		// 背景 の生成

	if (m_apScene2D[LOGOTYPE_PRESS] != NULL)
	{// 存在していたとき
		m_apScene2D[LOGOTYPE_PRESS]->BindTexture("data/tex/press_enter.png");			// 背景テクスチャ のポインタ を渡す
		m_apScene2D[LOGOTYPE_PRESS]->SetCenter(CScene2D::TEXTUREVTX_CENTER);		// 中心 の設定
		m_apScene2D[LOGOTYPE_PRESS]->SetSize(D3DXVECTOR3(250.0f, 100.0f, 0.0f));	// 大きさ の設定
		m_apScene2D[LOGOTYPE_PRESS]->SetPosition(D3DXVECTOR3(SCREEN_WIDTH / 2, 640, 0.0f));	// 位置 の設定
		m_apScene2D[LOGOTYPE_PRESS]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	// 色 の設定
		m_apScene2D[LOGOTYPE_PRESS]->SetTransform();		// 頂点情報 の設定
	}

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CTitlelogo::Uninit(void)
{
	for (int nCount = 0; nCount < LOGOTYPE_MAX; nCount++)
	{
		if (m_apScene2D[nCount] != NULL)
		{// 描画情報があったとき
			m_apScene2D[nCount]->Release();		// 削除予約
			m_apScene2D[nCount] = NULL;			// NULL を代入
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CTitlelogo::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void CTitlelogo::Draw(void)
{

}

//==============================================================================
// クリエイト処理
//==============================================================================
CTitlelogo *CTitlelogo::Create(void)
{
	CTitlelogo *pTitleLogo;		// 背景 のポインタ

	// 背景 の生成
	pTitleLogo = new CTitlelogo;

	// 背景 の初期化
	pTitleLogo->Init();

	return pTitleLogo;
}

//==============================================================================
// アセットの生成処理
//==============================================================================
HRESULT CTitlelogo::Load(void)
{
	//テクスチャの読み込み
	CManager::Load("data/tex/back.png");
	CManager::Load("data/tex/if.png");
	CManager::Load("data/tex/akazukin.png");
	CManager::Load("data/tex/attack.png");
	CManager::Load("data/tex/press_enter.png");

	return S_OK;
}