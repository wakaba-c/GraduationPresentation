//=============================================================================
//
// メッシュカプセル処理 [meshCapsule.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "meshCapsule.h"
#include "manager.h"
#include "renderer.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
PDIRECT3DTEXTURE9 CMeshCapsule::m_pTexture = NULL;				// テクスチャ情報のポインタ

//=============================================================================
// コンストラクタ
//=============================================================================
CMeshCapsule::CMeshCapsule(CScene::PRIORITY obj = CScene::PRIORITY_BG) : CScene(obj)
{
	SetObjType(CScene::PRIORITY_BG);							// オブジェクトタイプの設定

	// 位置・回転の初期設定
	m_size = D3DXVECTOR3(0.0f, 100.0f, 0.0f);					// サイズ
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);						// 回転
	m_col = D3DXCOLOR(0.0f, 1.0f, 1.0f, 0.3f);					// 色
}

//=============================================================================
// デストラクタ
//=============================================================================
CMeshCapsule::~CMeshCapsule()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CMeshCapsule::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();				// 描画情報の取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスの取得

	// ================== 四角形 ================== //

	m_nNumVertex = (CAPSULE_DIVISION + 1) * (CAPSULE_DIVISION + 1);				//総頂点数
	m_nNumIndex = ((CAPSULE_DIVISION + 1) * 2) * CAPSULE_DIVISION + ((CAPSULE_DIVISION - 1) * 2);		//インデックス数

	//総ポリゴン数	※分割数 * 分割数 * 2 + 縮退ポリゴン分 * 分割数(最後の列は除く)
	m_nNumPolygon = CAPSULE_DIVISION * CAPSULE_DIVISION * 2 + 4 * (CAPSULE_DIVISION - 1);

	// ================== 円形 ================== //

	m_nNumVertexSphere = CAPSULE_DIVISION + 2;								//総頂点数
	m_nNumIndexSphere = CAPSULE_DIVISION;									//インデックス数
	m_nNumPolygonSphere = CAPSULE_DIVISION;									//総ポリゴン数

	// 頂点情報の作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * ((m_nNumVertexSphere * 2) + m_nNumVertex), D3DUSAGE_WRITEONLY, FVF_VERTEX_3D, D3DPOOL_MANAGED, &m_pVtxBuffSphere, NULL);

	// インデックスバッファを生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * (((m_nNumIndexSphere + 2) * 2) + m_nNumIndex) + 1, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIdxBuffSphere, NULL);

	// 頂点情報の格納
	MakeVertex();

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CMeshCapsule::Uninit(void)
{
	// テクスチャの開放
	if (m_pTexture != NULL)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}

	// インデックスバッファの開放
	if (m_pIdxBuffSphere != NULL)
	{
		m_pIdxBuffSphere->Release();
		m_pIdxBuffSphere = NULL;
	}

	// 頂点バッファの開放
	if (m_pVtxBuffSphere != NULL)
	{
		m_pVtxBuffSphere->Release();
		m_pVtxBuffSphere = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CMeshCapsule::Update(void)
{
#ifdef _DEBUG
	Debug();
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CMeshCapsule::Draw(void)
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
	pDevice->SetStreamSource(0, m_pVtxBuffSphere, 0, sizeof(VERTEX_3D));

	// 頂点バッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuffSphere);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// 頂点フォーマットの設定
	pDevice->SetTexture(0, m_pTexture);

	// ポリゴンの描画		※1番上
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, m_nNumVertexSphere, 0, m_nNumPolygonSphere);

	// ポリゴンの描画		※2番目以降～最後の1歩手前まで
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVertex - m_nNumVertexSphere, m_nNumIndexSphere + 2, m_nNumPolygon - m_nNumPolygonSphere * 2);

	//ポリゴンの描画		※最後
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, m_nNumVertex + m_nNumVertexSphere, m_nNumIndex - m_nNumPolygonSphere + 2, m_nNumPolygonSphere);
}

//=============================================================================
// クリエイト処理
//=============================================================================
CMeshCapsule *CMeshCapsule::Create(void)
{
	CMeshCapsule *pMeshCapsule;
	pMeshCapsule = new CMeshCapsule(CScene::PRIORITY_BG);

	if (pMeshCapsule != NULL)
	{// メッシュカプセルが存在していたとき
		pMeshCapsule->Init();									// 初期化処理
	}
	return pMeshCapsule;
}

//=============================================================================
// ロード処理
//=============================================================================
HRESULT CMeshCapsule::Load(void)
{
	return S_OK;
}

//=============================================================================
// 色を決める関数
//=============================================================================
void CMeshCapsule::SetColor(D3DXCOLOR col)
{
	m_col = col;								// 色の代入
}

//=============================================================================
// サイズを決める関数
//=============================================================================
void CMeshCapsule::SetSize(D3DXVECTOR3 size)
{
	m_size = size;								// サイズの代入
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void CMeshCapsule::MakeVertex(void)
{
	D3DXVECTOR3 pos = GetPosition();
	VERTEX_3D *pVtx;							// 頂点情報へのポインタ
	WORD *pIdx;									// インデックスデータへのポインタ

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuffSphere->Lock(0, 0, (void**)&pVtx, 0);

	// ================= 円の中心 =================== //
	pVtx[0].pos = D3DXVECTOR3(pos.x, (pos.y - m_size.y / 2) - RADIUS / 2, pos.z);

	// 法線ベクトル
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// 頂点カラー
	pVtx[0].col = m_col;

	// テクスチャ描写の位置
	pVtx[0].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx++;

	// ================= 円を形成 =================== //
	for (int nDepth = 0; nDepth <= CAPSULE_DIVISION; nDepth++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(pos.x / 2 + sinf(D3DX_PI - ((D3DX_PI * 2 / CAPSULE_DIVISION) * nDepth)) * sinf(D3DX_PI + ((D3DX_PI / (CAPSULE_DIVISION + 1)) * 1)) * RADIUS / 2,
			(pos.y - m_size.y / 2) - RADIUS - cosf(D3DX_PI + ((D3DX_PI / (CAPSULE_DIVISION)) * 1)) * RADIUS / 2,
			pos.z / 2 + cosf(D3DX_PI - ((D3DX_PI * 2 / CAPSULE_DIVISION) * nDepth)) * sinf(D3DX_PI + ((D3DX_PI / (CAPSULE_DIVISION + 1)) * 1)) * RADIUS / 2);

		// 法線ベクトル
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// 頂点カラー
		pVtx[0].col = m_col;

		// テクスチャ描写の位置
		pVtx[0].tex = D3DXVECTOR2(1.0f, 1.0f * nDepth);

		// 頂点データのポインタを1つ分進める
		pVtx++;
	}

	// 四角形のメッシュを円形にする
	for (int nDepth = 1; nDepth < CAPSULE_DIVISION + 1; nDepth++)
	{
		for (int nWide = 0; nWide < CAPSULE_DIVISION + 1; nWide++)
		{
			// 高さを求める
			float fWork = sinf(D3DX_PI + ((D3DX_PI / (CAPSULE_DIVISION + 1)) * nDepth)) * RADIUS / 2;

			if (nDepth < (CAPSULE_DIVISION + 1) / 2)
			{
				// 頂点座標の設定
				pVtx[0].pos = D3DXVECTOR3(pos.x / 2 + sinf(D3DX_PI + ((D3DX_PI * 2 / CAPSULE_DIVISION) * nWide)) * fWork,
					(pos.y - m_size.y / 2) + cosf(D3DX_PI + ((D3DX_PI / (CAPSULE_DIVISION + 1)) * nDepth)) * RADIUS / 2,
					pos.z / 2 + cosf(D3DX_PI + ((D3DX_PI * 2 / CAPSULE_DIVISION) * nWide)) * fWork);
			}

			else if (nDepth >= (CAPSULE_DIVISION + 1) / 2)
			{
				// 頂点座標の設定
				pVtx[0].pos = D3DXVECTOR3(pos.x / 2 + sinf(D3DX_PI + ((D3DX_PI * 2 / CAPSULE_DIVISION) * nWide)) * fWork,
					(pos.y + m_size.y / 2) + cosf(D3DX_PI + ((D3DX_PI / (CAPSULE_DIVISION + 1)) * nDepth)) * RADIUS / 2,
					pos.z / 2 + cosf(D3DX_PI + ((D3DX_PI * 2 / CAPSULE_DIVISION) * nWide)) * fWork);
			}

			// 法線
			pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 頂点カラー
			pVtx[0].col = m_col;

			//テクスチャ描写の位置
			pVtx[0].tex = D3DXVECTOR2(1.0f * nWide, 1.0f * nDepth);

			// 頂点データのポインタを1つ分進める
			pVtx++;
		}
	}

	// ================= 円の中心 =================== //

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(pos.x, (pos.y + m_size.y / 2) + RADIUS / 2, pos.z);

	//法線ベクトル
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	//頂点カラー
	pVtx[0].col = m_col;

	//テクスチャ描写の位置
	pVtx[0].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点データのポインタを1つ分進める
	pVtx++;

	// ================= 円を形成 =================== //
	for (int nDepth = 0; nDepth <= CAPSULE_DIVISION; nDepth++)
	{
		//頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(pos.x / 2 + sinf(D3DX_PI + ((D3DX_PI * 2 / CAPSULE_DIVISION) * nDepth)) * sinf(D3DX_PI + ((D3DX_PI / (CAPSULE_DIVISION + 1)) * 1)) * RADIUS / 2,
			(pos.y + m_size.y / 2) + cosf(D3DX_PI + ((D3DX_PI / (CAPSULE_DIVISION)) * 1)) * -RADIUS / 2,
			pos.z / 2 + cosf(D3DX_PI - ((D3DX_PI * 2 / CAPSULE_DIVISION) * nDepth)) * sinf(D3DX_PI + ((D3DX_PI / (CAPSULE_DIVISION + 1)) * 1)) * RADIUS / 2);

		//法線ベクトル
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		//頂点カラー
		pVtx[0].col = m_col;

		//テクスチャ描写の位置
		pVtx[0].tex = D3DXVECTOR2(1.0f, 1.0f * nDepth);

		pVtx++;
	}

	// 2段目以降のことです。

	// 頂点データのアンロック
	m_pVtxBuffSphere->Unlock();

	// インデックスバッファをロックし、インデックスデータへのポインタ
	m_pIdxBuffSphere->Lock(0, 0, (void**)&pIdx, 0);
	int nCount = 0;

	// 頂点,1段目～
	for (nCount = 0; nCount < m_nNumVertexSphere; nCount++)
	{
		pIdx[0] = nCount;
		pIdx++;
	}

	// 縮退ポリゴン
	pIdx[0] = nCount;
	pIdx++;

	// 2段目以降
	for (nCount = 1; nCount < CAPSULE_DIVISION; nCount++)
	{
		// 縮退ポリゴン
		pIdx[0] = (CAPSULE_DIVISION + 1) + nCount * (CAPSULE_DIVISION + 1);
		pIdx++;

		for (int nCntX = 0; nCntX < CAPSULE_DIVISION + 1; nCntX++)
		{
			pIdx[0] = (CAPSULE_DIVISION + 1) + nCntX + nCount * (CAPSULE_DIVISION + 1);
			pIdx[1] = nCntX + nCount * (CAPSULE_DIVISION + 1) + 1;
			pIdx += 2;
		}

		// 縮退ポリゴン
		pIdx[0] = CAPSULE_DIVISION + (nCount + 1) * (CAPSULE_DIVISION + 1);
		pIdx++;
	}

	// 縮退ポリゴン
	pIdx[0] = (CAPSULE_DIVISION + 1) + nCount * (CAPSULE_DIVISION + 1);
	pIdx++;

	// 四角形の総頂点数に1プラスする
	nCount = m_nNumVertex + 1;

	// 縮退ポリゴン
	pIdx[0] = nCount;
	pIdx++;

	// 最後から1番手前と最後の頂点
	for (nCount += 1; nCount < m_nNumVertex + m_nNumVertexSphere + 1; nCount++)
	{
		// 現在の番号に1プラスする
		pIdx[0] = nCount;
		pIdx++;
	}

	// インデックスバッファをアンロック
	m_pIdxBuffSphere->Unlock();
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CMeshCapsule::Debug(void)
{

}
#endif