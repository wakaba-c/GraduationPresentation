//=============================================================================
//
// ゲージ処理 [gauge2D.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "gauge2D.h"
#include "manager.h"
#include "renderer.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CGauge2D::CGauge2D(CScene::PRIORITY obj = CScene::PRIORITY_UI) : CScene2D(obj)
{
	SetObjType(PRIORITY_UI);				//オブジェクトタイプ

	//値の初期化
	m_move = D3DXVECTOR3(0, 0, 0.0f);			// 移動量の初期化
	m_size = D3DXVECTOR3(180.0f, 15.0f, 0.0f);	// サイズの初期化
	m_nDamage = 1.0f;							// ダメージの初期化
	m_fHP = 1;									// HPの初期化

	for (int nCount = 0; nCount < DRAWINGTYPE_MAX; nCount++)
	{
		if (m_apScene2D[nCount] != NULL)
		{// シーンが存在していたとき
			m_apScene2D[nCount] = NULL;			// NULLを代入する
		}
	}
}

//=============================================================================
// デストラクタ
//=============================================================================
CGauge2D::~CGauge2D()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CGauge2D::Init(void)
{
	m_frameSize = D3DXVECTOR3(m_size.x + 20, m_size.y + 16, m_size.z);		// ゲージフレームのサイズ設定

	// サイズの設定
	SetSize(m_size);

	m_apScene2D[DRAWINGTYPE_GLAS] = CScene2D::Create(PRIORITY_UI);			// 生成

	if (m_apScene2D[DRAWINGTYPE_GLAS] != NULL)
	{// ゲージグラスが存在していたとき
		m_apScene2D[DRAWINGTYPE_GLAS]->SetColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f));		// 色の設定
		m_apScene2D[DRAWINGTYPE_GLAS]->SetCenter(CScene2D::TEXTUREVTX_LEFT);			// 中心位置を左に設定
		m_apScene2D[DRAWINGTYPE_GLAS]->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));		// 位置の設定
		m_apScene2D[DRAWINGTYPE_GLAS]->SetSize(m_size);									// サイズの設定
		m_apScene2D[DRAWINGTYPE_GLAS]->SetTransform();									// 頂点情報の更新
	}

	m_apScene2D[DRAWINGTYPE_DAMAGE] = CScene2D::Create(PRIORITY_UI);		// 生成

	if (m_apScene2D[DRAWINGTYPE_DAMAGE] != NULL)
	{// ダメージゲージが存在していたとき
		m_apScene2D[DRAWINGTYPE_DAMAGE]->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));	// 色の設定
		m_apScene2D[DRAWINGTYPE_DAMAGE]->SetCenter(CScene2D::TEXTUREVTX_LEFT);			// 中心位置を左に設定
		m_apScene2D[DRAWINGTYPE_DAMAGE]->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));	// 位置の設定
		m_apScene2D[DRAWINGTYPE_DAMAGE]->SetSize(m_size);								// サイズの設定
		m_apScene2D[DRAWINGTYPE_DAMAGE]->SetTransform();								// 頂点情報の更新
	}

	m_apScene2D[DRAWINGTYPE_BACK] = CScene2D::Create(PRIORITY_UI);			// 生成

	if (m_apScene2D[DRAWINGTYPE_BACK] != NULL)
	{// ゲージ背景が存在していたとき
		m_apScene2D[DRAWINGTYPE_BACK]->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));		// 色の設定
		m_apScene2D[DRAWINGTYPE_BACK]->SetCenter(CScene2D::TEXTUREVTX_LEFT);			// 中心位置を左に設定
		m_apScene2D[DRAWINGTYPE_BACK]->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));		// 位置の設定
		m_apScene2D[DRAWINGTYPE_BACK]->SetSize(m_size);									// サイズの設定
		m_apScene2D[DRAWINGTYPE_BACK]->SetTransform();									// 頂点情報の更新
	}

	m_apScene2D[DRAWINGTYPE_FRONT] = CScene2D::Create(PRIORITY_UI);			// 生成

	if (m_apScene2D[DRAWINGTYPE_FRONT] != NULL)
	{// ゲージUIが存在していたとき
		m_apScene2D[DRAWINGTYPE_FRONT]->BindTexture("data/tex/Gauge.png");	//UI用の画像を設定
		m_apScene2D[DRAWINGTYPE_FRONT]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));				// 色の設定
		m_apScene2D[DRAWINGTYPE_FRONT]->SetCenter(CScene2D::TEXTUREVTX_LEFT);						// 中心位置を左に設定
		m_apScene2D[DRAWINGTYPE_FRONT]->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));					// 位置の設定
		m_apScene2D[DRAWINGTYPE_FRONT]->SetSize(m_frameSize);										// サイズの設定
		m_apScene2D[DRAWINGTYPE_FRONT]->SetTransform();												// 頂点情報の更新
	}

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CGauge2D::Uninit(void)
{
	//終了処理
	for (int nCntLife = 0; nCntLife < DRAWINGTYPE_MAX; nCntLife++)
	{
		if (m_apScene2D[nCntLife] != NULL)
		{// ゲージが存在していたとき
			m_apScene2D[nCntLife]->Release();			// 削除予約
			m_apScene2D[nCntLife] = NULL;				// NULLを代入
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CGauge2D::Update(void)
{
	if (m_nDamage * m_size.x < m_fHP * m_size.x)
	{// 現在の大きさよりダメージ用のゲージが大きい場合
		m_fHP -= 0.005f;			// 引く
	}

	m_apScene2D[DRAWINGTYPE_DAMAGE]->SetSize(D3DXVECTOR3(m_fHP * m_size.x, m_size.y, 0.0f));		// サイズの設定

	for (int nCount = 0; nCount < DRAWINGTYPE_FRONT; nCount++)
	{
		m_apScene2D[nCount]->SetPosition(D3DXVECTOR3(20.0f, -15.0f, 0.0f) + m_pos);			// 位置の設定
		m_apScene2D[nCount]->SetTransform();												// 頂点情報の更新
	}

	m_apScene2D[DRAWINGTYPE_FRONT]->SetPosition(m_pos);				// 位置の設定
	m_apScene2D[DRAWINGTYPE_FRONT]->SetTransform();					// 頂点情報の更新
}

//=============================================================================
// 描画処理
//=============================================================================
void CGauge2D::Draw(void)
{
	for (int nCntLife = 0; nCntLife < DRAWINGTYPE_MAX; nCntLife++)
	{
		if (m_apScene2D[nCntLife] != NULL)
		{// ゲージが存在していたとき
			m_apScene2D[nCntLife]->Draw();			// 描画
		}
	}
}

//=============================================================================
// クリエイト処理
//=============================================================================
CGauge2D *CGauge2D::Create(void)
{
	CGauge2D *pGauge;
	pGauge = new CGauge2D(PRIORITY_UI);

	if (pGauge != NULL)
	{// ゲージが存在していたとき
		pGauge->Init();			// 初期化処理
	}
	return pGauge;
}

//=============================================================================
// ロード処理
//=============================================================================
HRESULT CGauge2D::Load(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice;

	//デバイスを取得する
	pDevice = pRenderer->GetDevice();

	// テクスチャの作成
	CManager::Load("data/tex/Gauge.png");

	return S_OK;
}

//=============================================================================
// ポジションの設定
//=============================================================================
void CGauge2D::SetPosition(D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//=============================================================================
// ライフバーの表示割合の設定
//=============================================================================
void CGauge2D::SetLifeBar(bool bDie, float fValue)
{
	m_apScene2D[DRAWINGTYPE_BACK]->SetSize(D3DXVECTOR3(m_size.x * fValue, m_size.y, 0.0f));
	m_nDamage = fValue;
}

//=============================================================================
// ゲージサイズの設定
//=============================================================================
void CGauge2D::SetGaugeSize(D3DXVECTOR3	size, D3DXVECTOR3 frameSize)
{
	m_size = size;
	m_frameSize = frameSize;

	for (int nCount = 0; nCount < DRAWINGTYPE_FRONT; nCount++)
	{
		m_apScene2D[nCount]->SetSize(m_size);
	}

	m_apScene2D[DRAWINGTYPE_FRONT]->SetSize(m_frameSize);
}