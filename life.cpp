//=============================================================================
//
// 体力ゲージ処理 [life.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "life.h"
#include "manager.h"
#include "renderer.h"
#include "scene3D.h"
#include "inputKeyboard.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CLife::CLife(CScene::PRIORITY obj = PRIORITY_LIFE) : CScene(obj)
{
	SetObjType(PRIORITY_LIFE);				//オブジェクトタイプ

	// 値の初期化
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 回転の初期化
	m_size = D3DXVECTOR3(50.0f, 10.0f, 0.0f);	// サイズの初期化
}

//=============================================================================
// デストラクタ
//=============================================================================
CLife::~CLife()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CLife::Init(void)
{
	m_apScene3D[DRAWINGTYPE_BACK] = CScene3D::Create(PRIORITY_UI);		// 生成

	if (m_apScene3D[DRAWINGTYPE_BACK] != NULL)
	{// シーンが存在していたとき
		m_apScene3D[DRAWINGTYPE_BACK]->SetCenter(CScene3D::TEXTUREVTX_LEFT);			// 中心位置を左に設定
		m_apScene3D[DRAWINGTYPE_BACK]->SetSize(m_size);									// サイズの設定
		m_apScene3D[DRAWINGTYPE_BACK]->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));		// 位置の設定
		m_apScene3D[DRAWINGTYPE_BACK]->SetColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f));		// 色の設定
		m_apScene3D[DRAWINGTYPE_BACK]->MakeVertex();									// 頂点情報の更新
	}

	m_apScene3D[DRAWINGTYPE_FRONT] = CScene3D::Create(PRIORITY_UI);		// 生成

	if (m_apScene3D[DRAWINGTYPE_FRONT] != NULL)
	{// シーンが存在していたとき
		m_apScene3D[DRAWINGTYPE_FRONT]->SetCenter(CScene3D::TEXTUREVTX_LEFT);			// 中心位置を左に設定
		m_apScene3D[DRAWINGTYPE_FRONT]->SetSize(m_size);								// サイズの設定
		m_apScene3D[DRAWINGTYPE_FRONT]->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));		// 位置の設定
		m_apScene3D[DRAWINGTYPE_FRONT]->SetColor(D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f));	// 色の設定
		m_apScene3D[DRAWINGTYPE_FRONT]->MakeVertex();									// 頂点情報の更新
	}

	m_apScene3D[DRAWINGTYPE_FRAME] = CScene3D::Create(PRIORITY_UI);		// 生成

	if (m_apScene3D[DRAWINGTYPE_FRAME] != NULL)
	{// シーンが存在していたとき
		m_apScene3D[DRAWINGTYPE_FRAME]->SetCenter(CScene3D::TEXTUREVTX_LEFT);								// 中心位置を左に設定
		m_apScene3D[DRAWINGTYPE_FRAME]->SetSize(D3DXVECTOR3(m_size.x + 5.0f, m_size.y * 2, 0.0f));			// サイズの設定
		m_apScene3D[DRAWINGTYPE_FRAME]->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));							// 位置の設定
		m_apScene3D[DRAWINGTYPE_FRAME]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));						// 色の設定
		m_apScene3D[DRAWINGTYPE_FRAME]->BindTexture(CManager::GetResource("data/tex/HitPointGauge.png"));	// テクスチャを設定
		m_apScene3D[DRAWINGTYPE_FRAME]->MakeVertex();														// 頂点情報の更新
	}
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CLife::Uninit(void)
{
	for (int nCount = 0; nCount < DRAWINGTYPE_MAX; nCount++)
	{
		if (m_apScene3D[nCount] != NULL)
		{// シーンが存在していたとき
			m_apScene3D[nCount]->Uninit();			// 開放処理
			m_apScene3D[nCount]->Release();			// 削除予約
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CLife::Update(void)
{
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();
	D3DXVECTOR3 pos = GetPosition();

	m_apScene3D[DRAWINGTYPE_BACK]->SetPosition(pos + D3DXVECTOR3(3.0f, 0.0f, 0.0f));
	m_apScene3D[DRAWINGTYPE_FRONT]->SetPosition(pos + D3DXVECTOR3(3.0f, 0.0f, 0.0f));
	m_apScene3D[DRAWINGTYPE_FRAME]->SetPosition(pos + D3DXVECTOR3(0.0f, 2.5f, 0.0f));
}

//=============================================================================
// 描画処理
//=============================================================================
void CLife::Draw(void)
{

}

//=============================================================================
// クリエイト処理
//=============================================================================
CLife *CLife::Create(void)
{
	CLife *pLife;
	pLife = new CLife(PRIORITY_LIFE);

	if (pLife != NULL)
	{// ライフが存在していたとき
		pLife->Init();				// 初期化処理
	}
	return pLife;
}

//=============================================================================
// テクスチャのロード関数
//=============================================================================
HRESULT CLife::Load(void)
{
	// テクスチャの読み込み
	CManager::Load("data/tex/HitPointGauge.png");
	return S_OK;
}

//=============================================================================
// ライフバーの表示割合の設定
//=============================================================================
void CLife::SetLifeBar(float fValue)
{
	if (m_apScene3D[DRAWINGTYPE_FRONT] != NULL)
	{// シーンが存在していたとき
		m_apScene3D[DRAWINGTYPE_FRONT]->SetSize(D3DXVECTOR3(m_size.x * fValue, m_size.y, 0.0f));	// サイズの設定
		m_apScene3D[DRAWINGTYPE_FRONT]->MakeVertex();												// 頂点情報の更新
	}
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void CLife::MakeVertex(void)
{
	//デバイスを取得する
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 頂点情報の作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX_3D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);
	VERTEX_3D *pVtx;

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-m_size.x, m_size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_size.x, m_size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_size.x, -m_size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_size.x, -m_size.y, 0.0f);

	//法線ベクトル
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	//頂点カラー
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//テクスチャ描写の位置
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
}