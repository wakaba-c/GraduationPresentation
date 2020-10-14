//=============================================================================
//
// メッシュキューブ処理 [meshCube.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "meshCube.h"
#include "manager.h"
#include "renderer.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
PDIRECT3DTEXTURE9 CMeshCube::m_pTexture = NULL;				// テクスチャ情報のポインタ

//=============================================================================
// コンストラクタ
//=============================================================================
CMeshCube::CMeshCube(CScene::PRIORITY obj = CScene::PRIORITY_MESH) : CScene(obj)
{
	SetObjType(CScene::PRIORITY_MESH);						// オブジェクトタイプ

	// 位置・回転の初期設定
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);					// 回転の初期化
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);				// 色の初期化
	m_size = D3DXVECTOR3(100.0f, 100.0f, 100.0f);			// サイズの初期化
}

//=============================================================================
// デストラクタ
//=============================================================================
CMeshCube::~CMeshCube()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CMeshCube::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得

	//総頂点数
	m_nNumVertex = (DEPTH + 1) * (WIDE + 1);

	//インデックス数
	m_nNumIndex = ((DEPTH + 1) * 2) * DEPTH + ((WIDE - 1) * 2);

	//総ポリゴン数
	m_nNumPolygon = DEPTH * WIDE * 2 + 4 * (DEPTH - 1);

	// 頂点情報の作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVertex, D3DUSAGE_WRITEONLY, FVF_VERTEX_3D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);

	//インデックスバッファを生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIndex, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIdxBuff, NULL);

	int nPolygon = DEPTH * WIDE * 2;

	//頂点情報の作成
	MakeVertex();

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CMeshCube::Uninit(void)
{
	// テクスチャの開放
	if (m_pTexture != NULL)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}

	// インデックスバッファの開放
	if (m_pIdxBuff != NULL)
	{
		m_pIdxBuff->Release();
		m_pIdxBuff = NULL;
	}

	// 頂点バッファの開放
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CMeshCube::Update(void)
{
#ifdef _DEBUG
	Debug();
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CMeshCube::Draw(void)
{
	D3DXVECTOR3 pos = GetPosition();
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得
	D3DXMATRIX	mtxRot, mtxTrans;				//計算用マトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

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

	// 頂点バッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//頂点フォーマットの設定
	pDevice->SetTexture(0, m_pTexture);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVertex, 0, m_nNumPolygon);

	//頂点フォーマットの設定
	pDevice->SetTexture(0, NULL);
}

//=============================================================================
// クリエイト処理
//=============================================================================
CMeshCube *CMeshCube::Create(void)
{
	CMeshCube *pMeshCube;

	pMeshCube = new CMeshCube(CScene::PRIORITY_MESH);
	pMeshCube->Init();
	return pMeshCube;
}

//=============================================================================
// ロード関数
//=============================================================================
HRESULT CMeshCube::Load(void)
{
	return S_OK;
}

//=============================================================================
// テクスチャの解放関数
//=============================================================================
void CMeshCube::MakeVertex(void)
{
	VERTEX_3D *pVtx;
	WORD *pIdx;									//インデックスデータへポインタ

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-m_size.x / 2, -m_size.y / 2,  -m_size.z / 2);
	pVtx[1].pos = D3DXVECTOR3(m_size.x / 2, -m_size.y / 2,  -m_size.z / 2);
	pVtx[2].pos = D3DXVECTOR3(-m_size.x / 2, -m_size.y / 2,  m_size.z / 2);
	pVtx[3].pos = D3DXVECTOR3(m_size.x / 2, -m_size.y / 2,  m_size.z / 2);

	pVtx[4].pos = D3DXVECTOR3(-m_size.x / 2, m_size.y / 2,  m_size.z / 2);
	pVtx[5].pos = D3DXVECTOR3(m_size.x / 2, m_size.y / 2,  m_size.z / 2);

	pVtx[6].pos = D3DXVECTOR3(-m_size.x / 2, m_size.y / 2,  -m_size.z / 2);
	pVtx[7].pos = D3DXVECTOR3(m_size.x / 2, m_size.y / 2,  -m_size.z / 2);

	pVtx[8].pos = D3DXVECTOR3(-m_size.x / 2, -m_size.y / 2,  -m_size.z / 2);
	pVtx[9].pos = D3DXVECTOR3(m_size.x / 2, -m_size.y / 2,  -m_size.z / 2);

	//法線
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pVtx[4].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[5].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pVtx[6].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[7].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pVtx[8].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[9].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	//頂点カラー
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	pVtx[4].col = m_col;
	pVtx[5].col = m_col;

	pVtx[6].col = m_col;
	pVtx[7].col = m_col;

	pVtx[8].col = m_col;
	pVtx[9].col = m_col;

	//テクスチャ描写の位置
	pVtx[0].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx[4].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVtx[5].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx[6].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVtx[7].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx[8].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVtx[9].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx++;

	//頂点データのアンロック
	m_pVtxBuff->Unlock();

	int nCnt = 0;

	//インデックスバッファをロックし、インデックスデータへのポインタ
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	for (int nCntY = 0; nCntY < 5; nCntY++)
	{
		pIdx[0] = 2 * nCntY;
		pIdx[1] = 2 * nCntY + 1;
		pIdx += 2;
		nCnt += 2;
	}

	pIdx[0] = 9;
	pIdx[1] = 7;
	pIdx += 2;

	pIdx[0] = 7;
	pIdx[1] = 5;
	pIdx[2] = 1;
	pIdx[3] = 3;
	pIdx += 4;

	pIdx[0] = 3;
	pIdx[1] = 4;
	pIdx += 2;

	pIdx[0] = 4;
	pIdx[1] = 6;
	pIdx[2] = 2;
	pIdx[3] = 0;

	//インデックスバッファをロックし、インデックスデータへのポインタ
	m_pIdxBuff->Unlock();
}

//=============================================================================
// 色を決める関数
//=============================================================================
void CMeshCube::SetColor(D3DXCOLOR col)
{
	m_col = col;								// 色の代入
}

//=============================================================================
// サイズを決める関数
//=============================================================================
void CMeshCube::SetSize(D3DXVECTOR3 size)
{
	m_size = size;								// サイズの代入
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CMeshCube::Debug(void)
{

}
#endif