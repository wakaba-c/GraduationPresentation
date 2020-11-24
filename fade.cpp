//=============================================================================
//
// フェード処理 [fade.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "fade.h"
#include "renderer.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define TEX_FADE "data/tex/fade.jpg"
#define ACTION_COUNT (20)

//=============================================================================
// 静的メンバ変数
//=============================================================================
LPDIRECT3DVERTEXBUFFER9 CFade::m_pVtxBuffFade = NULL;								// 頂点バッファへのポインタ
CFade::FADE				CFade::m_fade = CFade::FADE_NONE;							// フェード状態
CFade::FADETYPE			CFade::m_fadeType = CFade::FADETYPE_NORMAL;					// フェードタイプ
CManager::MODE			CFade::m_modeNext = CManager::MODE_NONE;					// 次の画面（モード）
D3DXCOLOR				CFade::m_colorFade = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);		// フェード色
D3DXVECTOR3				CFade::m_pos = D3DXVECTOR3_ZERO;							// 位置
D3DXVECTOR3				CFade::m_move = D3DXVECTOR3_ZERO;							// 移動量
bool					CFade::m_bReflection = false;								// バウンドしたかどうか
int						CFade::m_nCntAction = 0;									// アクションカウンター

//=============================================================================
// コンストラクタ
//=============================================================================
CFade::CFade()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CFade::~CFade()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
void CFade::Init(CManager::MODE modeNext)
{
	CRenderer *Renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = Renderer->GetDevice();

	m_fade = FADE_IN;										// フェードインに設定
	m_fadeType = FADETYPE_NORMAL;								// フェードタイプをなしに設定
	m_modeNext = modeNext;									// 次のモードを入れる
	m_colorFade = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);		// 黒い画面（不透明）
	m_bReflection = false;
	m_nCntAction = 0;

	m_pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);			// 位置
	m_move = D3DXVECTOR3_ZERO;			// 移動量

	MakeVertex(pDevice);									// 頂点情報の設定
}

//=============================================================================
// 終了処理
//=============================================================================
void CFade::Uninit(void)
{
	// 頂点バッファの開放
	if (m_pVtxBuffFade != NULL)
	{
		m_pVtxBuffFade->Release();
		m_pVtxBuffFade = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CFade::Update(void)
{
	if (m_fade != FADE_NONE)
	{// 何かしていたとき
		switch (m_fadeType)
		{
		case FADETYPE_NORMAL:	// ノーマルフェードだったとき
			NormalFade();
			break;
		case FADETYPE_SLIDE:	// スライドフェードだったとき
			SlideFade();
			break;
		}

		// 頂点情報の更新
		SetTransform();
	}
}

//=============================================================================
// Draw関数
//=============================================================================
void CFade::Draw(void)
{
	CRenderer *Renderer = CManager::GetRenderer();						//レンダラーの取得
	LPDIRECT3DDEVICE9 pDevice = Renderer->GetDevice();					//デバイスの取得

	// 頂点バッファをデバイスのデータストリームにバインド
	pDevice->SetStreamSource(0, m_pVtxBuffFade, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	switch (m_fadeType)
	{
	case FADETYPE_NORMAL:	// ノーマルフェードだったとき
	// テクスチャの設定
		pDevice->SetTexture(0, NULL);
		break;
	case FADETYPE_SLIDE:	// スライドフェードだったとき
		// テクスチャの設定
		pDevice->SetTexture(0, CManager::GetResource(TEX_FADE));
		break;
	}

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//=============================================================================
// 頂点の作成
//=============================================================================
void CFade::MakeVertex(LPDIRECT3DDEVICE9 pDevice)
{
	VERTEX_2D *pVtx;

	// オブジェクトの頂点バッファを生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &m_pVtxBuffFade, NULL);

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuffFade->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(SCREEN_WIDTH, 0.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f, SCREEN_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = m_colorFade;
	pVtx[1].col = m_colorFade;
	pVtx[2].col = m_colorFade;
	pVtx[3].col = m_colorFade;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点データをアンロックする
	m_pVtxBuffFade->Unlock();
}

//=============================================================================
// 頂点情報の更新
//=============================================================================
void CFade::SetTransform(void)
{
	VERTEX_2D *pVtx;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuffFade->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - SCREEN_WIDTH / 2, m_pos.y - SCREEN_HEIGHT / 2, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + SCREEN_WIDTH / 2, m_pos.y - SCREEN_HEIGHT / 2, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - SCREEN_WIDTH / 2, m_pos.y + SCREEN_HEIGHT / 2, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + SCREEN_WIDTH / 2, m_pos.y + SCREEN_HEIGHT / 2, 0.0f);

	// 頂点カラーの設定
	pVtx[0].col = m_colorFade;
	pVtx[1].col = m_colorFade;
	pVtx[2].col = m_colorFade;
	pVtx[3].col = m_colorFade;

	// 頂点データをアンロックする
	m_pVtxBuffFade->Unlock();
}

//=============================================================================
// 通常フェード
//=============================================================================
void CFade::NormalFade(void)
{
	switch (m_fade)
	{
	case FADE_IN:	// フェードインだったとき
		m_colorFade.a -= 0.05f;			// 画面を透明にしていく
		if (m_colorFade.a <= 0.0f)
		{// α値が0以下のとき
			m_colorFade.a = 0.0f;		// α値を0にする
			m_fade = FADE_NONE;			// フェード状態を変更
		}
		break;
	case FADE_OUT:	// フェードアウトだったとき
		m_colorFade.a += 0.05f;			// 画面を暗くしていく
		if (m_colorFade.a > 1.5f)
		{// α値が1.5以上だったとき
		 //フェードイン処理に切り替え
			m_colorFade.a = 1.0f;		// α値を1.0にする
			m_fade = FADE_IN;			// フェード状態を変更

			//モード切替
			CManager::SetMode(m_modeNext);
		}
		break;
	}
}

//=============================================================================
// スライドフェード
//=============================================================================
void CFade::SlideFade(void)
{
	switch (m_fade)
	{
	case FADE_IN:	// フェードインだったとき
		if (m_nCntAction > ACTION_COUNT)
		{
			m_colorFade.a -= 0.05f;			// 画面を透明にしていく
			if (m_colorFade.a <= 0.0f)
			{// α値が0以下のとき
				m_colorFade.a = 0.0f;		// α値を0にする
				m_fade = FADE_NONE;			// フェード状態を変更
			}
		}
		else
		{
			m_nCntAction++;					// カウンタにプラスする
		}
		break;
	case FADE_OUT:	// フェードアウトだったとき
		m_move.y += 0.5f;			// 画面を暗くしていく
		if (m_pos.y > SCREEN_HEIGHT / 2)
		{// 位置が640以上だったとき
			if (m_bReflection)
			{
				//フェードイン処理に切り替え
				m_pos.y = SCREEN_HEIGHT / 2;		// 位置を640にする
				m_move = D3DXVECTOR3_ZERO;			// 移動量を0にする
				m_fade = FADE_IN;					// フェード状態を変更

				//モード切替
				CManager::SetMode(m_modeNext);
			}
			else
			{
				m_pos.y = SCREEN_HEIGHT / 2;		// 位置を640にする

				m_bReflection = true;
				m_move *= -0.5f;
			}
		}

		m_pos += m_move;
		break;
	}
}

//=============================================================================
// クリエイト関数
//=============================================================================
CFade *CFade::Create(void)
{
	CFade *pFade;
	pFade = new CFade();

	if (pFade != NULL)
	{// フェードが存在していたとき
		pFade->Init(CManager::MODE_TITLE);				// 初期化処理
	}
	return pFade;
}

//=============================================================================
// フェードの状態設定
//=============================================================================
void CFade::SetFade(CManager::MODE modeNext, FADETYPE type)
{
	m_fade = FADE_OUT;										// フェード状態の設定
	m_fadeType = type;										// フェードタイプ
	m_modeNext = modeNext;									// 次のモードを入れる

	switch (m_fadeType)
	{
	case FADETYPE_NORMAL:	// ノーマルフェードだったとき
		m_colorFade = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);		//黒い画面(透明)
		m_pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
		break;
	case FADETYPE_SLIDE:	// スライドフェードだったとき
		m_colorFade = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);		//黒い画面(透明)
		m_pos = D3DXVECTOR3(SCREEN_WIDTH / 2, 0.0f - SCREEN_HEIGHT / 2, 0.0f);
		m_nCntAction = 0;
		m_bReflection = false;
		break;
	}
}