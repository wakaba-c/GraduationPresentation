//=============================================================================
//
// 木処理 [tree.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "tree.h"
#include "manager.h"
#include "renderer.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
PDIRECT3DTEXTURE9 CTree::m_pTexture = NULL;				// テクスチャのポインタ

//=============================================================================
// コンストラクタ
//=============================================================================
CTree::CTree(CScene::PRIORITY obj = CScene::PRIORITY_TREE) : CScene(obj)
{
	SetObjType(CScene::PRIORITY_TREE);								// オブジェクトタイプの設定

	/* =============== 値の初期化 =============== */
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);								// 位置の初期化
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);								// 回転の初期化
	m_size = D3DXVECTOR3(120.0f, 200.0f, 0.0f);							// サイズの初期化
}

//=============================================================================
// デストラクタ
//=============================================================================
CTree::~CTree()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CTree::Init(void)
{
	m_pTexture = CManager::GetResource("data/tex/tree.png");			// テクスチャの設定
	MakeVertex();														// 頂点情報の更新
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CTree::Uninit(void)
{
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
void CTree::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void CTree::Draw(void)
{
		CRenderer *pRenderer = CManager::GetRenderer();
		LPDIRECT3DDEVICE9 pDevice;

		D3DXMATRIX	mtxRot, mtxTrans, mtxView;				//計算用マトリックス

		//デバイスを取得する
		pDevice = pRenderer->GetDevice();

		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);				// 裏面(左回り)をカリングする
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&m_mtxWorld);

		pDevice->GetTransform(D3DTS_VIEW, &mtxView);

		m_mtxWorld._11 = mtxView._11;
		m_mtxWorld._12 = mtxView._21;
		m_mtxWorld._13 = mtxView._31;
		m_mtxWorld._31 = mtxView._13;
		m_mtxWorld._32 = mtxView._23;
		m_mtxWorld._33 = mtxView._33;

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

		//頂点フォーマットの設定
		pDevice->SetTexture(0, m_pTexture);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		//頂点フォーマットの設定
		pDevice->SetTexture(0, NULL);

		// レンダーステート(通常ブレンド処理)
		pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面(左回り)をカリングする


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
CTree *CTree::Create(TREETYPE type)
{
	CTree *pTree = new CTree(PRIORITY_TREE);
	pTree->SetType(type);
	pTree->Init();
	return pTree;
}

//=============================================================================
// ロード処理
//=============================================================================
HRESULT CTree::Load(void)
{
	CManager::Load("data/tex/tree.png");
	return S_OK;
}

//=============================================================================
// テクスチャの解放処理
//=============================================================================
void CTree::Unload(void)
{
	// テクスチャの開放
	if (m_pTexture != NULL)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
}

//=============================================================================
// モデルの読込
//=============================================================================
void CTree::LoadModel(char *add)
{
	FILE *pFile = NULL;							// ファイル
	char cReadText[128] = {};					// 文字
	char cHeadText[128] = {};					// 比較
	float aData[4];								// 答え
	CTree *pTree = NULL;

	pFile = fopen(add, "r");					// ファイルを開くまたは作る

	if (pFile != NULL)													// ファイルが読み込めた場合
	{
		fgets(cReadText, sizeof(cReadText), pFile);						// 行を飛ばす
		fgets(cReadText, sizeof(cReadText), pFile);						// 行を飛ばす
		fgets(cReadText, sizeof(cReadText), pFile);						// 行を飛ばす

		while (strcmp(cHeadText, "End") != 0)
		{
			CManager::ConvertStringToFloat(cReadText, ",", aData);

			pTree = CTree::Create(TREETYPE_NORMAL);
			pTree->SetPosition(D3DXVECTOR3(aData[0] - 15.0f, aData[1] + 200.0f, aData[2]));			// ポジションを決める

			fgets(cReadText, sizeof(cReadText), pFile);				// 行を飛ばす
			sscanf(cReadText, "%s", &cHeadText);
		}

		fclose(pFile);				// ファイルを閉じる
	}
	else
	{
		MessageBox(NULL, "モデル情報のアクセス失敗！", "WARNING", MB_ICONWARNING);	// メッセージボックスの生成
	}
}

//=============================================================================
// 種類の設定関数
//=============================================================================
void CTree::SetType(TREETYPE type)
{
	m_type = type;
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void CTree::MakeVertex(void)
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
	pVtx[0].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	//テクスチャ描写の位置
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CTree::Debug(void)
{

}
#endif