//=============================================================================
//
// 円板処理 [circle.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "circle.h"
#include "manager.h"
#include "renderer.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
PDIRECT3DTEXTURE9 CCircle::m_pTexture = NULL;		// テクスチャ情報の初期化

//=============================================================================
// コンストラクタ
//=============================================================================
CCircle::CCircle()
{
	// 初期設定
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 回転
	m_col = D3DXCOLOR(0.0f, 1.0f, 1.0f, 0.5f);		// 色
	m_fRadius = 20.0;								// 半径
}

//=============================================================================
// デストラクタ
//=============================================================================
CCircle::~CCircle()
{

}

//==============================================================================
// 初期化処理
//==============================================================================
HRESULT CCircle::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();				// レンダラーの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスの取得

	m_nNumVertex = DIVISION_CIRCLE + 2;							//総頂点数
	m_nNumIndex = DIVISION_CIRCLE;								//インデックス数
	m_nNumPolygon = DIVISION_CIRCLE;							//総ポリゴン数

	// 頂点情報の作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVertex, D3DUSAGE_WRITEONLY, FVF_VERTEX_3D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);

	//インデックスバッファを生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIndex, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIdxBuff, NULL);

	// 頂点情報の作成
	MakeVertex();

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CCircle::Uninit(void)
{
	// テクスチャの開放
	if (m_pTexture != NULL)
	{// テクスチャが存在していたとき
	 // 開放処理
		m_pTexture->Release();
		m_pTexture = NULL;		// NULLの代入
	}

	// インデックスバッファの開放
	if (m_pIdxBuff != NULL)
	{// インデックスバッファが存在していたとき
	 // 開放処理
		m_pIdxBuff->Release();
		m_pIdxBuff = NULL;		// NULLの代入
	}

	// 頂点バッファの開放
	if (m_pVtxBuff != NULL)
	{// 頂点バッファが存在していたとき
	 // 開放処理
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;		// NULLの代入
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CCircle::Update(void)
{
	Debug();	// デバッグ処理
}

//=============================================================================
// 描画処理
//=============================================================================
void CCircle::Draw(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();			// レンダラーの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスの取得
	D3DXMATRIX	mtxRot, mtxTrans;							//計算用マトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

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

	// 頂点バッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//頂点フォーマットの設定
	pDevice->SetTexture(0, m_pTexture);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, (DIVISION_CIRCLE + 1), 0, (DIVISION_CIRCLE + 1));

	//頂点フォーマットの設定
	pDevice->SetTexture(0, NULL);
}

//==============================================================================
// クリエイト処理
//==============================================================================
CCircle *CCircle::Create(void)
{
	CCircle *pCircle;
	pCircle = new CCircle();

	if (pCircle != NULL)
	{// サークルが存在していたとき
		pCircle->Init();				// 初期化
	}
	return pCircle;
}

//==============================================================================
// テクスチャのロード関数
//==============================================================================
HRESULT CCircle::Load(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();		// レンダラーの取得
	LPDIRECT3DDEVICE9 pDevice;							// デバイス

	//デバイスを取得する
	pDevice = pRenderer->GetDevice();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, "data/tex/skydome.png", &m_pTexture);

	return S_OK;
}

//=============================================================================
// テクスチャの解放関数
//=============================================================================
void CCircle::Unload(void)
{
	// テクスチャの開放
	if (m_pTexture != NULL)
	{// テクスチャ情報が存在していたとき
		// 開放処理
		m_pTexture->Release();
		m_pTexture = NULL;			// NULLの代入
	}
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void CCircle::MakeVertex(void)
{
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ
	WORD *pIdx;			// インデックスデータへの

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//================= 円の中心 ===================//
	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(0.0f, m_pos.y, 0.0f);

	//法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	//頂点カラーの設定
	pVtx[0].col = m_col;

	//テクスチャ描写の位置を設定
	pVtx[0].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点データのポインタを1つ分進める
	pVtx++;

	//================= 円を形成 ===================//
	for (int nDepth = 0; nDepth < DIVISION_CIRCLE - 1; nDepth++)
	{
		//頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(sinf(D3DX_PI + 6.28f + ((6.28f / (DIVISION_CIRCLE - 2)) * nDepth)) * m_fRadius,
								m_pos.y,
								cosf(D3DX_PI + 6.28f + ((6.28f / (DIVISION_CIRCLE - 2)) * nDepth)) * m_fRadius);

		//法線ベクトルの設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		//頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);

		//テクスチャ描写の位置を設定
		pVtx[0].tex = D3DXVECTOR2(1.0f, 1.0f * nDepth);

		// 頂点データのポインタを1つ分進める
		pVtx++;
	}

	//頂点データのアンロック
	m_pVtxBuff->Unlock();

	//インデックスバッファをロックし、インデックスデータへのポインタ
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	for (int nCntY = 0; nCntY < m_nNumVertex; nCntY++)
	{
		// インデックスバッファ の設定
		pIdx[0] = nCntY;
		pIdx++;		// インデックスデータのポインタを1つ分進める
	}

	//インデックスバッファをロックし、インデックスデータへのポインタ
	m_pIdxBuff->Unlock();
}

//==============================================================================
// 位置を決める関数
//==============================================================================
void CCircle::SetPosition(D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//==============================================================================
// 回転を決める関数
//==============================================================================
void CCircle::SetRotation(D3DXVECTOR3 rot)
{
	m_rot = rot;
}

//==============================================================================
// 色を決める関数
//==============================================================================
void CCircle::SetColor(D3DXCOLOR col)
{
	m_col = col;
}

//==============================================================================
// 半径を決める関数
//==============================================================================
void CCircle::SetRadius(float fValue)
{
	m_fRadius = fValue;
}

//==============================================================================
// サイズを決める関数
//==============================================================================
void CCircle::SetSize(D3DXVECTOR3 size)
{
	m_size = size;
}

//=============================================================================
// デバッグ処理
//=============================================================================
void CCircle::Debug(void)
{
	ImGui::Begin("System");

	//ImGui::Text("move = %f, %f, %f", m_vecAxis.x, m_vecAxis.y, m_vecAxis.z);										// プレイヤーの現在位置を表示

	//デバッグ処理を終了
	ImGui::End();
}