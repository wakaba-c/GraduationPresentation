//=============================================================================
//
// scene3D処理 [scene3D.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "scene3D.h"
#include "manager.h"
#include "renderer.h"
#include "inputKeyboard.h"
#include "inputMouse.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CScene3D::CScene3D(CScene::PRIORITY obj) : CScene(obj)
{
	// 値の初期化
	m_pTexture = NULL;
	m_size = D3DXVECTOR3(100.0f, 100.0f, 100.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

//=============================================================================
// デストラクタ
//=============================================================================
CScene3D::~CScene3D()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CScene3D::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 頂点情報の作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX_3D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);

	SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));		// 位置の設定

	// 頂点情報の作成
	MakeVertex();
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CScene3D::Uninit(void)
{
	//頂点バッファの開放
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CScene3D::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void CScene3D::Draw(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice;

	D3DXVECTOR3 pos = GetPosition();					// 位置を取得

	D3DXMATRIX	mtxRot, mtxTrans, mtxView;				// 計算用マトリックス

	// デバイスを取得する
	pDevice = pRenderer->GetDevice();

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	m_mtxWorld._11 = mtxView._11;
	m_mtxWorld._12 = mtxView._21;
	m_mtxWorld._13 = mtxView._31;
	m_mtxWorld._21 = mtxView._12;
	m_mtxWorld._22 = mtxView._22;
	m_mtxWorld._23 = mtxView._32;
	m_mtxWorld._31 = mtxView._13;
	m_mtxWorld._32 = mtxView._23;
	m_mtxWorld._33 = mtxView._33;

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// 頂点フォーマットの設定
	pDevice->SetTexture(0, m_pTexture);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// レンダーステート(通常ブレンド処理)
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// 頂点フォーマットの設定
	pDevice->SetTexture(0, NULL);

}

//=============================================================================
// クリエイト関数
//=============================================================================
CScene3D *CScene3D::Create(CScene::PRIORITY obj)
{
	CScene3D *pScene3D;
	pScene3D = new CScene3D(obj);

	if (pScene3D == NULL) return NULL;
	pScene3D->Init();
	return pScene3D;
}

//=============================================================================
// セットテクスチャ
//=============================================================================
void CScene3D::BindTexture(LPDIRECT3DTEXTURE9 tex)
{
	m_pTexture = tex;
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void CScene3D::MakeVertex(void)
{
	VERTEX_3D *pVtx;

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	switch (m_Vtx)
	{
	case TEXTUREVTX_CENTER:
		//頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-m_size.x / 2, m_size.y / 2, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(m_size.x / 2, m_size.y / 2, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-m_size.x / 2, -m_size.y / 2, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(m_size.x / 2, -m_size.y / 2, 0.0f);
		break;
	case TEXTUREVTX_LEFT:
		//頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(0.0f, m_size.y / 2, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(m_size.x, m_size.y / 2, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(0.0f, -m_size.y / 2, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(m_size.x, -m_size.y / 2, 0.0f);
		break;
	}

	//法線ベクトル
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	//頂点カラー
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	//テクスチャ描写の位置
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
// 中央の設定関数
//=============================================================================
void CScene3D::SetCenter(TEXTUREVTX vtx)
{
	m_Vtx = vtx;
}

//=============================================================================
// サイズの設定関数
//=============================================================================
void CScene3D::SetSize(D3DXVECTOR3 size)
{
	m_size = size;
}

//=============================================================================
// 色の設定
//=============================================================================
void CScene3D::SetColor(D3DXCOLOR col)
{
	m_col = col;
}