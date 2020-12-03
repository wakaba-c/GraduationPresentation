//==================================================================================================================
//
// 影ポリゴン[shadow.cpp]
// Author:Seiya Takahashi
//
//==================================================================================================================
#include "manager.h"
#include "renderer.h"
#include "shadow.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define SHADOW_SIZE 40.0f		// 影サイズ
#define SIZE_DOWN 3				// 影サイズの減少割合

//==================================================================================================================
// 静的メンバ変数の初期化
//==================================================================================================================
LPDIRECT3DTEXTURE9 CShadow::m_pTexture = NULL;			// テクスチャ情報

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CShadow::CShadow(PRIORITY type = CScene::PRIORITY_MESH) :CScene(type)
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CShadow::~CShadow()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
HRESULT CShadow::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();					// レンダラー情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// デバイスの取得

	// 変数の初期化
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 位置
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動量
	m_size = D3DXVECTOR3(1.0f, 1.0f, 1.0f);				// 大きさ
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 回転

	// オブジェクトの頂点バッファを生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	// 座標の設定
	// [0]頂点
	m_pVtx[0].pos.x = m_pos.x - m_size.x / 2;
	m_pVtx[0].pos.y = 0;
	m_pVtx[0].pos.z = m_pos.z - m_size.z / 2;

	// [1]頂点
	m_pVtx[1].pos.x = m_pos.x + m_size.x / 2;
	m_pVtx[1].pos.y = 0;
	m_pVtx[1].pos.z = m_pos.z - m_size.z / 2;

	// [2]頂点
	m_pVtx[2].pos.x = m_pos.x - m_size.x / 2;
	m_pVtx[2].pos.y = 0;
	m_pVtx[2].pos.z = m_pos.z + m_size.z / 2;

	// [3]頂点
	m_pVtx[3].pos.x = m_pos.x + m_size.x / 2;
	m_pVtx[3].pos.y = 0;
	m_pVtx[3].pos.z = m_pos.z + m_size.z / 2;

	//法線ベクトルの設定
	m_pVtx[0].nor = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pVtx[1].nor = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pVtx[2].nor = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pVtx[3].nor = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

	// 頂点カラー
	m_pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);
	m_pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);
	m_pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);
	m_pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);

	// テクスチャ座標の設定
	m_pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	m_pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	m_pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	m_pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点データをアンロックする
	m_pVtxBuff->Unlock();

	return S_OK;
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CShadow::Uninit(void)
{
	// 頂点バッファがあるとき
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();		// 開放
		m_pVtxBuff = NULL;			// NULLにする
	}
}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CShadow::Update(void)
{
	// 大きさ取得
	m_size = D3DXVECTOR3(SHADOW_SIZE, 1.0f, SHADOW_SIZE);
}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CShadow::Draw(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();			// レンダラー情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;					// 計算用マトリックス

	// 減算合成
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, m_size.x, m_size.y, m_size.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, m_pTexture);

	// 表面をカリングする
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// 裏面(左回り)をカリングする
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//テクスチャの設定
	pDevice->SetTexture(0, NULL);

	// 通常ブレンド
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//==================================================================================================================
// 生成処理
//==================================================================================================================
CShadow *CShadow::Create(void)
{
	CShadow *pShadow = NULL;						// 影のポインタNULL
	pShadow = new CShadow(CScene::PRIORITY_MESH);	// 動的確保
	pShadow->Init();								// 初期化処理

	// 値を返す
	return pShadow;
}

//==================================================================================================================
// テクスチャロード
//==================================================================================================================
HRESULT CShadow::Load(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();						// レンダラー情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();					// デバイスの取得

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, SHADOW_TEX, &m_pTexture);

	// 値を返す
	return S_OK;
}

//==================================================================================================================
// テクスチャ破棄
//==================================================================================================================
void CShadow::Unload(void)
{
	// テクスチャがあるとき
	if (m_pTexture != NULL)
	{
		m_pTexture->Release();					// テクスチャ解放
		m_pTexture = NULL;						// NULLにする
	}
}

//==================================================================================================================
// 位置設定
//==================================================================================================================
void CShadow::SetPos(D3DXVECTOR3 &pos, D3DXVECTOR3 &move, bool &bJump)
{
	// 位置設定
	m_pos = pos;

	// 移動量設定
	m_move = move;
}

//==================================================================================================================
// 影の削除
//==================================================================================================================
void CShadow::ReleaseShadow(void)
{
	Release();
}

