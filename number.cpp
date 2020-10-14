//=============================================================================
//
// 数字処理 [number.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "number.h"
#include "manager.h"
#include "renderer.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CNumber::CNumber(CScene::PRIORITY obj = CScene::PRIORITY_UI) : CScene2D(obj)
{
	//値の初期化
	m_nNumOld = 0;
}

//=============================================================================
// デストラクタ
//=============================================================================
CNumber::~CNumber()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CNumber::Init(void)
{
	// 頂点情報の作成
	MakeVertex();
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CNumber::Uninit(void)
{
	//頂点バッファの開放
	if (m_pVtxBuff != NULL)
	{// 頂点バッファが存在していたとき
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CNumber::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void CNumber::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice;
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスを取得する
	pDevice = pRenderer->GetDevice();

	//頂点バッファをデバイスのデータにバインド
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	//テクスチャの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	//頂点フォーマットの設定
	pDevice->SetTexture(0, CManager::GetResource("data/tex/number001.png"));

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//=============================================================================
// クリエイト処理
//=============================================================================
CNumber *CNumber::Create(void)
{
	CNumber *pNumber;
	pNumber = new CNumber(PRIORITY_UI);

	if (pNumber != NULL)
	{// 数字が存在していたとき
		pNumber->Init();			// 初期化処理
	}
	return pNumber;
}

//=============================================================================
// ロード処理
//=============================================================================
HRESULT CNumber::Load(void)
{
	// テクスチャの取得
	CManager::Load("data/tex/number001.png");

	return S_OK;
}

//=============================================================================
// 数字の設定
//=============================================================================
void CNumber::SetNumber(int nNum)
{
	VERTEX_2D *pVtx;

	//前回の数字と今回の数字が違う場合
	if (m_nNumOld != nNum)
	{
		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		m_nNumOld = nNum;

		//テクスチャ描写の位置
		pVtx[0].tex = D3DXVECTOR2(nNum * 0.1f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(nNum * 0.1f + 0.1f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(nNum * 0.1f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(nNum * 0.1f + 0.1f, 1.0f);

		// 頂点データをアンロックする
		m_pVtxBuff->Unlock();
	}
}

//=============================================================================
// 頂点の作成
//=============================================================================
void CNumber::MakeVertex(void)
{
	LPDIRECT3DDEVICE9 pDevice;
	CRenderer *pRenderer = CManager::GetRenderer();
	D3DXVECTOR3 pos = GetPosition();					// 位置の取得
	D3DXVECTOR3 size = GetSize();						// サイズの取得

	pDevice = pRenderer->GetDevice();

	VERTEX_2D *pVtx;

	// オブジェクトの頂点バッファを生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	//頂点座標の設定(基準のx座標 + 間隔 * nCntScore (+ 幅), 基準のy座標)
	pVtx[0].pos = D3DXVECTOR3(pos.x - size.x / 2, pos.y - size.y / 2, pos.z);
	pVtx[1].pos = D3DXVECTOR3(pos.x + size.x / 2, pos.y - size.y / 2, pos.z);
	pVtx[2].pos = D3DXVECTOR3(pos.x - size.x / 2, pos.y + size.y / 2, pos.z);
	pVtx[3].pos = D3DXVECTOR3(pos.x + size.x / 2, pos.y + size.y / 2, pos.z);

	//1.0で固定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//カラーチャートの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//テクスチャ描写の位置
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(0.1f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(0.1f, 1.0f);

	// 頂点データをアンロックする
	m_pVtxBuff->Unlock();
}