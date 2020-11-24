//=============================================================================
//
// 空処理 [sky.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "sky.h"
#include "manager.h"
#include "renderer.h"
#include "meshSphere.h"

//=============================================================================
// コンストラクタ処理
//=============================================================================
CSky::CSky(CScene::PRIORITY obj = CScene::PRIORITY_SKY) : CScene(obj)
{
	SetObjType(CScene::PRIORITY_SKY);

	// 位置・回転の初期設定
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);						// 回転
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);					// 色
	m_fRadius = 100000.0;
}

//=============================================================================
//Init処理
//=============================================================================
HRESULT CSky::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();				// 描画情報の取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスの取得

	// ================== 四角形 ================== //

	m_nNumVertex = (DIVISION + 1) * (DIVISION + 1);				//総頂点数
	m_nNumIndex = ((DIVISION + 1) * 2) * DIVISION + ((DIVISION - 1) * 2);		//インデックス数

	//総ポリゴン数	※分割数 * 分割数 * 2 + 縮退ポリゴン分 * 分割数(最後の列は除く)
	m_nNumPolygon = DIVISION * DIVISION * 2 + 4 * (DIVISION - 1);

	// ================== 円形 ================== //

	m_nNumVertexSphere = DIVISION + 2;								//総頂点数
	m_nNumIndexSphere = DIVISION;									//インデックス数
	m_nNumPolygonSphere = DIVISION;									//総ポリゴン数

	// 頂点情報の作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * ((m_nNumVertexSphere * 2) + m_nNumVertex), D3DUSAGE_WRITEONLY, FVF_VERTEX_3D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);

	// インデックスバッファを生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * (((m_nNumIndexSphere + 2) * 2) + m_nNumIndex) + 1, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIdxBuff, NULL);

	// 頂点情報の格納
	MakeVertex();

	m_bDraw = true;
	return S_OK;
}

//=============================================================================
// Update関数
//=============================================================================
void CSky::Update(void)
{
#ifdef _DEBUG
	// デバッグ処理
	Debug();
#endif
}

//=============================================================================
// Draw関数
//=============================================================================
void CSky::Draw(void)
{
	if (m_bDraw)
	{
		D3DXVECTOR3 pos = GetPosition();
		CRenderer *pRenderer = CManager::GetRenderer();
		LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得

		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);				// 裏面(左回り)をカリングする

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

		// 頂点フォーマットの設定
		pDevice->SetTexture(0, CManager::GetResource("data/tex/skydome.png"));

		// ポリゴンの描画		※1番上
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, m_nNumVertexSphere, 0, m_nNumPolygonSphere);

		// ポリゴンの描画		※2番目以降～最後の1歩手前まで
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVertex - m_nNumVertexSphere, m_nNumIndexSphere + 2, m_nNumPolygon - m_nNumPolygonSphere * 2);

		//ポリゴンの描画		※最後
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, m_nNumVertex + m_nNumVertexSphere, m_nNumIndex - m_nNumPolygonSphere + 2, m_nNumPolygonSphere);

		// 頂点フォーマットの設定
		pDevice->SetTexture(0, NULL);

		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面(左回り)をカリングする
	}
}

//=============================================================================
// Uninit関数
//=============================================================================
void CSky::Uninit(void)
{
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
// デストラクタ処理
//=============================================================================
CSky::~CSky()
{

}

//=============================================================================
// クリエイト関数
//=============================================================================
CSky *CSky::Create(void)
{
	CSky *pSky;
	pSky = new CSky(CScene::PRIORITY_SKY);
	pSky->Init();
	return pSky;
}

//=============================================================================
// モデルのロード関数
//=============================================================================
HRESULT CSky::Load(void)
{
	//テクスチャの読み込み
	CManager::Load("data/tex/skydome.png");
	return S_OK;
}

//=============================================================================
// テクスチャの解放関数
//=============================================================================
void CSky::Unload(void)
{

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void CSky::MakeVertex(void)
{
	VERTEX_3D *pVtx;							// 頂点情報へのポインタ
	WORD *pIdx;									// インデックスデータへのポインタ

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// ================= 円の中心 =================== //
	pVtx[0].pos = D3DXVECTOR3(0.0f, -m_fRadius, 0.0f);

	// 法線ベクトル
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// 頂点カラー
	pVtx[0].col = m_col;

	// テクスチャ描写の位置
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);

	// 頂点データのポインタを1つ
	pVtx++;

	// ================= 円を形成 =================== //
	for (int nDepth = 0; nDepth <= DIVISION; nDepth++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(sinf(D3DX_PI - ((D3DX_PI * 2 / DIVISION) * nDepth)) * sinf(D3DX_PI + ((D3DX_PI / (DIVISION + 1)) * 1)) * m_fRadius,
			cosf(D3DX_PI + ((D3DX_PI / (DIVISION)) * 1)) * m_fRadius,
			cosf(D3DX_PI - ((D3DX_PI * 2 / DIVISION) * nDepth)) * sinf(D3DX_PI + ((D3DX_PI / (DIVISION + 1)) * 1)) * m_fRadius);

		// 法線ベクトル
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// 頂点カラー
		pVtx[0].col = m_col;

		// テクスチャ描写の位置
		pVtx[0].tex = D3DXVECTOR2((1.0f / DIVISION) * nDepth, (1.0f / DIVISION) * 1);

		// 頂点データのポインタを1つ分進める
		pVtx++;
	}

	// 四角形のメッシュを円形にする
	for (int nDepth = 1; nDepth < DIVISION + 1; nDepth++)
	{
		for (int nWide = 0; nWide < DIVISION + 1; nWide++)
		{
			// 高さを求める
			float fWork = sinf(D3DX_PI + ((D3DX_PI / (DIVISION + 1)) * nDepth)) * m_fRadius;

			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(sinf(D3DX_PI + ((D3DX_PI * 2 / DIVISION) * nWide)) * fWork,
				cosf(D3DX_PI + ((D3DX_PI / (DIVISION + 1)) * nDepth)) * m_fRadius,
				cosf(D3DX_PI + ((D3DX_PI * 2 / DIVISION) * nWide)) * fWork);

			// 法線
			pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 頂点カラー
			pVtx[0].col = m_col;

			//テクスチャ描写の位置
			pVtx[0].tex = D3DXVECTOR2((1.0f / DIVISION) * nWide, (1.0f / DIVISION) * nDepth);

			// 頂点データのポインタを1つ分進める
			pVtx++;
		}
	}

	// ================= 円の中心 =================== //

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(0.0f, m_fRadius, 0.0f);

	//法線ベクトル
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	//頂点カラー
	pVtx[0].col = m_col;

	//テクスチャ描写の位置
	pVtx[0].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点データのポインタを1つ分進める
	pVtx++;

	// ================= 円を形成 =================== //
	for (int nDepth = 0; nDepth <= DIVISION; nDepth++)
	{
		//頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(sinf(D3DX_PI + ((D3DX_PI * 2 / DIVISION) * nDepth)) * sinf(D3DX_PI + ((D3DX_PI / (DIVISION + 1)) * 1)) * m_fRadius,
			cosf(D3DX_PI + ((D3DX_PI / (DIVISION)) * 1)) * -m_fRadius,
			cosf(D3DX_PI - ((D3DX_PI * 2 / DIVISION) * nDepth)) * sinf(D3DX_PI + ((D3DX_PI / (DIVISION + 1)) * 1)) * m_fRadius);

		//法線ベクトル
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		//頂点カラー
		pVtx[0].col = m_col;

		//テクスチャ描写の位置
		pVtx[0].tex = D3DXVECTOR2((1.0f / DIVISION) * nDepth, (1.0f / DIVISION) * 1);

		pVtx++;
	}

	// 2段目以降のことです。

	// 頂点データのアンロック
	m_pVtxBuff->Unlock();

	// インデックスバッファをロックし、インデックスデータへのポインタ
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);
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
	for (nCount = 1; nCount < DIVISION; nCount++)
	{
		// 縮退ポリゴン
		pIdx[0] = (DIVISION + 1) + nCount * (DIVISION + 1);
		pIdx++;

		for (int nCntX = 0; nCntX < DIVISION + 1; nCntX++)
		{
			pIdx[0] = (DIVISION + 1) + nCntX + nCount * (DIVISION + 1);
			pIdx[1] = nCntX + nCount * (DIVISION + 1) + 1;
			pIdx += 2;
		}

		// 縮退ポリゴン
		pIdx[0] = DIVISION + (nCount + 1) * (DIVISION + 1);
		pIdx++;
	}

	// 縮退ポリゴン
	pIdx[0] = (DIVISION + 1) + nCount * (DIVISION + 1);
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
	m_pIdxBuff->Unlock();
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CSky::Debug(void)
{
	ImGui::Begin("System");													// 「Hello、world！」というウィンドウを作成しますそれに追加します。

	if (ImGui::CollapsingHeader("sky"))
	{
		ImGui::Checkbox("draw", &m_bDraw);
	}

	//デバッグ処理を終了
	ImGui::End();
}
#endif