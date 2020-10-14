//=============================================================================
//
// ステージ管理処理 [stage.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "stage.h"
#include "manager.h"
#include "renderer.h"
#include "colliderBox.h"
#include "meshField.h"
#include "object.h"
#include "player.h"
#include "game.h"
#include "enemy.h"
#include "camera.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
LPDIRECT3DTEXTURE9		CStage::m_pTexture = NULL;		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 CStage::m_pVtxBuff = NULL;		// 頂点バッファへのポインタ
CStage::FADE			CStage::m_fade = CStage::FADE_NONE;					// フェード状態
D3DXCOLOR				CStage::m_col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);				// フェード色
D3DXVECTOR3				CStage::m_playerPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// プレイヤーの立っていた場所
bool					CStage::m_bEvent = false;								// イベントかどうか
CEnemy					*CStage::m_apEnemy[MAX_ENEMY] = {};

//==============================================================================
// コンストラクタ
//==============================================================================
CStage::CStage()
{
	// 値の初期化
	m_bEvent = false;
	m_playerPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

//=============================================================================
// デストラクタ
//=============================================================================
CStage::~CStage()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CStage::Init(void)
{
	CRenderer *Renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = Renderer->GetDevice();

	m_fade = FADE_NONE;											// フェードタイプの初期化
	m_col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);					// 黒い画面（不透明）

	// 頂点情報の更新
	MakeVertexFade(pDevice);
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CStage::Uninit(void)
{
	// テクスチャの開放
	if (m_pTexture != NULL)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
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
void CStage::Update(void)
{
	VERTEX_2D *pVtx;

	if (m_fade != FADE_NONE)
	{
		if (m_fade == FADE_IN)
		{
			m_col.a -= 0.05f;				//画面を透明にしていく
			if (m_col.a <= 0.0f)
			{
				m_col.a = 0.0f;
				m_fade = FADE_NONE;
			}
		}
		else if (m_fade == FADE_OUT)
		{
			m_col.a += 0.05f;
			if (m_col.a > 1.5f)
			{
				//フェードイン処理に切り替え
				m_col.a = 1.0f;
				m_fade = FADE_IN;

				//モード切替
				ChengeStage();
			}
		}
	}

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// 頂点データをアンロックする
	m_pVtxBuff->Unlock();
}

//=============================================================================
// 描画処理
//=============================================================================
void CStage::Draw(void)
{
	CRenderer *Renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = Renderer->GetDevice();

	// 頂点バッファをデバイスのデータストリームにバインド
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, NULL);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//=============================================================================
// クリエイト処理
//=============================================================================
CStage *CStage::Create(void)
{
	CStage *pStage;
	pStage = new CStage();
	pStage->Init();
	return pStage;
}

//=============================================================================
// 敵の読み込み(phase)
//=============================================================================
void CStage::LoadPhase(char *add)
{
	FILE *pFile = NULL;																	// ファイル
	char cReadText[128] = {};															// 文字
	char cHeadText[128] = {};															// 比較
	float fData[3];																	// 答え

	pFile = fopen(add, "r");									// ファイルを開くまたは作る

	if (pFile != NULL)																// ファイルが読み込めた場合
	{
		fgets(cReadText, sizeof(cReadText), pFile);										// 行を飛ばす
		fgets(cReadText, sizeof(cReadText), pFile);										// 行を飛ばす
		fgets(cReadText, sizeof(cReadText), pFile);										// 行を飛ばす

		for (int nCount = 0; nCount < MAX_ENEMY; nCount++)
		{
			CManager::ConvertStringToFloat(cReadText, ",", fData);

			m_apEnemy[nCount] = CEnemy::Create();

			if (m_apEnemy[nCount] != NULL)
			{
				m_apEnemy[nCount]->SetPosition(D3DXVECTOR3(fData[0], fData[1], fData[2]));			// ポジションを決める
				m_apEnemy[nCount]->SetActive(false);
			}

			fgets(cReadText, sizeof(cReadText), pFile);									// 行を飛ばす
			sscanf(cReadText, "%s", &cHeadText);
		}

		fclose(pFile);																// ファイルを閉じる
	}
	else
	{
		MessageBox(NULL, "モデル情報のアクセス失敗！", "WARNING", MB_ICONWARNING);	// メッセージボックスの生成
	}
}

//=============================================================================
// フェードの状態設定
//=============================================================================
void CStage::SetStage(void)
{
	m_fade = FADE_OUT;
	m_col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);		//黒い画面(透明)
}

//=============================================================================
// フェーズの開始処理
//=============================================================================
void CStage::SetPhase(void)
{
	for (int nCount = 0; nCount < MAX_ENEMY; nCount++)
	{
		if (m_apEnemy[nCount] != NULL)
		{
			m_apEnemy[nCount]->SetActive(true);
			m_apEnemy[nCount]->SetTarget(TARGETTYPE_HOUSE);
		}
	}
}

//=============================================================================
// ステージの変更処理
//=============================================================================
void CStage::ChengeStage(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();
	CPlayer *pPlayer = CGame::GetPlayer();
	CCamera *pCamera = CManager::GetCamera();

	if (m_bEvent)
	{
		pPlayer->SetPosition(m_playerPos);

		if (pCamera != NULL)
		{
			pCamera->SetPosCamera(m_playerPos, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		}
		m_bEvent = false;
	}
	else
	{
		m_playerPos = pPlayer->GetPosition();
		pPlayer->SetPosition(D3DXVECTOR3(540.88f, 165.00f, 4942.73f));

		if (pCamera != NULL)
		{
			pCamera->SetPosCamera(D3DXVECTOR3(540.88f, 165.00f, 4942.73f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		}
		m_bEvent = true;
	}
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void CStage::MakeVertexFade(LPDIRECT3DDEVICE9 pDevice)
{
	VERTEX_2D *pVtx;

	// オブジェクトの頂点バッファを生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

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
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点データをアンロックする
	m_pVtxBuff->Unlock();
}