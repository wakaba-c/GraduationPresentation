//=============================================================================
//
// 軌跡処理 [meshOrbit.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "meshOrbit.h"
#include "game.h"
#include "manager.h"
#include "renderer.h"
#include "player.h"
#include "model.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
PDIRECT3DTEXTURE9 CMeshOrbit::m_pTexture = NULL;				// テクスチャ情報の初期化
D3DXVECTOR3 point[2 * (ORBIT_WIDE + 1)];						// ポイント

//=============================================================================
// コンストラクタ
//=============================================================================
CMeshOrbit::CMeshOrbit(CScene::PRIORITY obj = CScene::PRIORITY_ORBIT) : CScene(obj)
{
	SetObjType(CScene::PRIORITY_ORBIT);								// オブジェクトタイプの設定
}

//=============================================================================
// デストラクタ
//=============================================================================
CMeshOrbit::~CMeshOrbit()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CMeshOrbit::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得

	// 位置・回転の初期設定
	m_aOrbit.pVtxBuffOrbit = NULL;
	m_aOrbit.pIdxBuffMeshOrbit = NULL;
	m_aOrbit.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_aOrbit.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_bUse = false;

	for (int nCount = 0; nCount < ORBIT_WIDE; nCount++)
	{
		point[nCount] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		point[(ORBIT_WIDE - 1) + nCount] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	//総頂点数
	m_aOrbit.nNumVertexMeshOrbit = (ORBIT_DEPTH + 1) * (ORBIT_DEPTH + 1);

	//インデックス数
	m_nNumIndexOrbit = ((ORBIT_DEPTH + 1) * 2) * ORBIT_DEPTH + ((ORBIT_WIDE - 1) * 2);

	//総ポリゴン数
	m_aOrbit.nNumPolygonMeshOrbit = ORBIT_DEPTH * ORBIT_WIDE * 2 + 4 * (ORBIT_DEPTH - 1);

	// 頂点情報の作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * (ORBIT_DEPTH + 1) * (ORBIT_WIDE + 1), D3DUSAGE_WRITEONLY, FVF_VERTEX_3D, D3DPOOL_MANAGED, &m_aOrbit.pVtxBuffOrbit, NULL);

	//インデックスバッファを生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * ORBIT_DEPTH * ORBIT_WIDE * (2 + 4), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_aOrbit.pIdxBuffMeshOrbit, NULL);

	m_aMtxMeshOrbit.pos = D3DXVECTOR3(0.0f, 130.0f, 0.0f);

	// 頂点情報の作成
	MakeVertex();

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CMeshOrbit::Uninit(void)
{
	//インデックスバッファの開放
	if (m_aOrbit.pIdxBuffMeshOrbit != NULL)
	{
		m_aOrbit.pIdxBuffMeshOrbit->Release();
		m_aOrbit.pIdxBuffMeshOrbit = NULL;
	}

	// 頂点バッファの開放
	if (m_aOrbit.pVtxBuffOrbit != NULL)
	{
		m_aOrbit.pVtxBuffOrbit->Release();
		m_aOrbit.pVtxBuffOrbit = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CMeshOrbit::Update(void)
{
	VERTEX_3D *pVtx;																// 頂点情報のポインタ
	CPlayer *pPlayer;																// プレイヤーのポインタ

	pPlayer = CGame::GetPlayer();													// プレイヤーの取得
	CModel *pModel = pPlayer->GetModel();											// モデルの取得

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_aOrbit.pVtxBuffOrbit->Lock(0, 0, (void**)&pVtx, 0);

	// 最新の軌跡描画位置を取得
	pVtx[0].pos = D3DXVECTOR3(m_mtxWorldWeapon._41, m_mtxWorldWeapon._42, m_mtxWorldWeapon._43);
	pVtx[ORBIT_WIDE + 1].pos = D3DXVECTOR3(pModel[14].GetMtxWorld()._41, pModel[14].GetMtxWorld()._42, pModel[14].GetMtxWorld()._43);

	point[0] = D3DXVECTOR3(m_mtxWorldWeapon._41, m_mtxWorldWeapon._42, m_mtxWorldWeapon._43);
	point[ORBIT_WIDE + 1] = D3DXVECTOR3(pModel[14].GetMtxWorld()._41, pModel[14].GetMtxWorld()._42, pModel[14].GetMtxWorld()._43);

	// 軌跡の値を一つずらす
	for (int nCntOrbit = 0; nCntOrbit < ORBIT_WIDE; nCntOrbit++)
	{
		pVtx[nCntOrbit + 1].pos = D3DXVECTOR3(
			m_mtxWorldWeaponOld[nCntOrbit]._41,
			m_mtxWorldWeaponOld[nCntOrbit]._42,
			m_mtxWorldWeaponOld[nCntOrbit]._43);

		pVtx[nCntOrbit + ORBIT_WIDE + 2].pos = D3DXVECTOR3(
			m_mtxWorldMeshOrbitOld[nCntOrbit]._41,
			m_mtxWorldMeshOrbitOld[nCntOrbit]._42,
			m_mtxWorldMeshOrbitOld[nCntOrbit]._43);

		//控え
		point[nCntOrbit + 1] = D3DXVECTOR3(m_mtxWorldWeaponOld[nCntOrbit]._41, m_mtxWorldWeaponOld[nCntOrbit]._42, m_mtxWorldWeaponOld[nCntOrbit]._43);

		point[nCntOrbit + ORBIT_WIDE + 2] = D3DXVECTOR3(m_mtxWorldMeshOrbitOld[nCntOrbit]._41,
			m_mtxWorldMeshOrbitOld[nCntOrbit]._42,
			m_mtxWorldMeshOrbitOld[nCntOrbit]._43);
	}

	// 過去のワールドマトリックス情報をずらす
	for (int nCntObit = ORBIT_WIDE - 1; nCntObit > 0; nCntObit--)
	{
		m_mtxWorldMeshOrbitOld[nCntObit] = m_mtxWorldMeshOrbitOld[nCntObit - 1];
		m_mtxWorldWeaponOld[nCntObit] = m_mtxWorldWeaponOld[nCntObit - 1];
	}

	// 最後に最新の位置を入れる
	m_mtxWorldMeshOrbitOld[0] = pModel[14].GetMtxWorld();

	m_mtxWorldWeaponOld[0] = m_mtxWorldWeapon;

	//頂点データのアンロック
	m_aOrbit.pVtxBuffOrbit->Unlock();

#ifdef _DEBUG
	Debug();
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CMeshOrbit::Draw(void)
{
	// ワールドマトリックスの初期化
	CPlayer *pPlayer = CGame::GetPlayer();
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();								// デバイスの取得
	CModel *pModel = pPlayer->GetModel();

	D3DXMATRIX	mtxRot, mtxTrans, mtxView, mtxMeshRot, mtxMeshTrans;				//計算用マトリックス

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);							// 裏面(左回り)をカリングする

	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// レンダーステート(加算合成処理)
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorldMeshOrbit);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorldWeapon);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_aOrbit.rot.y, m_aOrbit.rot.x, m_aOrbit.rot.z);
	D3DXMatrixMultiply(&m_mtxWorldMeshOrbit, &m_mtxWorldMeshOrbit, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_aOrbit.pos.x, m_aOrbit.pos.y, m_aOrbit.pos.z);
	D3DXMatrixMultiply(&m_mtxWorldMeshOrbit, &m_mtxWorldMeshOrbit, &mtxTrans);

	//D3DXMatrixMultiply(&m_mtxWorldMeshOrbit, &m_mtxWorldMeshOrbit, &pModel[14].GetMtxWorld());

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, m_aMtxMeshOrbit.rot.y, m_aMtxMeshOrbit.rot.x, m_aMtxMeshOrbit.rot.z);
	D3DXMatrixMultiply(&m_mtxWorldWeapon, &m_mtxWorldWeapon, &mtxMeshRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxMeshTrans, m_aMtxMeshOrbit.pos.x, m_aMtxMeshOrbit.pos.y, m_aMtxMeshOrbit.pos.z);
	D3DXMatrixMultiply(&m_mtxWorldWeapon, &m_mtxWorldWeapon, &mtxMeshTrans);

	D3DXMatrixMultiply(&m_mtxWorldWeapon, &m_mtxWorldWeapon, &pModel[14].GetMtxWorld());

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorldMeshOrbit);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_aOrbit.pVtxBuffOrbit, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// 頂点バッファをデータストリームに設定
	pDevice->SetIndices(m_aOrbit.pIdxBuffMeshOrbit);

	//頂点フォーマットの設定
	pDevice->SetTexture(0, m_pTextureOrbit);

	if(m_bUse)
	{
		// ポリゴンの描画
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_aOrbit.nNumVertexMeshOrbit, 0, m_aOrbit.nNumPolygonMeshOrbit);
	}


	//頂点フォーマットの設定
	pDevice->SetTexture(0, NULL);

	// レンダーステート(通常ブレンド処理)
	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面(左回り)をカリングする
}

//=============================================================================
// クリエイト処理
//=============================================================================
CMeshOrbit *CMeshOrbit::Create(void)
{
	CMeshOrbit *pMeshOrbit;									// メッシュスフィアのポインタ
	pMeshOrbit = new CMeshOrbit(CScene::PRIORITY_ORBIT);	// メモリの動的確保
	pMeshOrbit->Init();										// メッシュスフィアの初期化
	return pMeshOrbit;										// メッシュスフィアのポインタを返す
}

//=============================================================================
// ロード処理
//=============================================================================
HRESULT CMeshOrbit::Load(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice;

	//デバイスを取得する
	pDevice = pRenderer->GetDevice();

	//テクスチャの読み込み
	m_pTexture = CManager::GetResource("data/tex/orbit.png");

	return S_OK;
}

//=============================================================================
// 軌跡を描画するかどうか
//=============================================================================
void CMeshOrbit::SetOrbit(bool bValue)
{
	m_bUse = bValue;							// 値を格納する
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void CMeshOrbit::MakeVertex(void)
{
	// 頂点情報の設定
	VERTEX_3D *pVtx;

	WORD *pIdx;									// インデックスデータへポインタ

		//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_aOrbit.pVtxBuffOrbit->Lock(0, 0, (void**)&pVtx, 0);

	for (int nDepth = 0; nDepth < ORBIT_DEPTH + 1; nDepth++)
	{
		for (int nWide = 0; nWide < ORBIT_WIDE + 1; nWide++)
		{
			//頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3((-100.0f * ORBIT_WIDE) / 2 + 100 * nWide, (100.0f * ORBIT_DEPTH) - 100 * nDepth, 0.0f);

			//法線ベクトル
			pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

			//頂点カラー
			pVtx[0].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f - nWide * (1.0f / ORBIT_WIDE));

			//テクスチャ描写の位置
			pVtx[0].tex = D3DXVECTOR2(1.0f * nWide, 1.0f * nDepth);

			pVtx++;
		}
	}

	//頂点データのアンロック
	m_aOrbit.pVtxBuffOrbit->Unlock();

	//インデックスバッファをロックし、インデックスデータへのポインタ
	m_aOrbit.pIdxBuffMeshOrbit->Lock(0, 0, (void**)&pIdx, 0);

	for (int nCntY = 0; nCntY < ORBIT_DEPTH; nCntY++)
	{
		if (nCntY != 0)
		{
			pIdx[0] = (ORBIT_WIDE + 1) + nCntY * (ORBIT_WIDE + 1);
			pIdx++;
		}

		for (int nCntX = 0; nCntX < ORBIT_WIDE + 1; nCntX++)
		{
			pIdx[0] = (ORBIT_WIDE + 1) + nCntX + nCntY * (ORBIT_WIDE + 1);
			pIdx[1] = nCntX + nCntY * (ORBIT_WIDE + 1);
			pIdx += 2;
		}

		if (ORBIT_DEPTH > nCntY + 1)
		{
			pIdx[0] = ORBIT_WIDE + nCntY * (ORBIT_WIDE + 1);
			pIdx++;

		}
	}

	//頂点データのアンロック
	m_aOrbit.pIdxBuffMeshOrbit->Unlock();
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CMeshOrbit::Debug(void)
{

}
#endif