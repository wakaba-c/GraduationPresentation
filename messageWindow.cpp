//=============================================================================
//
// 目的表示処理 [messageWindow.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "messageWindow.h"
#include "manager.h"
#include "renderer.h"
#include "fade.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CMessageWindow::CMessageWindow(CScene::PRIORITY obj = CScene::PRIORITY_UI) : CScene2D(obj)
{
	(CScene::PRIORITY_UI);				//オブジェクトタイプ

	// 値の初期化
	m_fade = FADE_OUT;			// フェードタイプを設定
	m_nPage = 0;				// ページ
	m_nCount = 0;				// カウンター
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);		// カラー
}

//=============================================================================
// デストラクタ
//=============================================================================
CMessageWindow::~CMessageWindow()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CMessageWindow::Init(void)
{
	// 初期化処理
	CScene2D::Init();

	// 背面テクスチャの設定
	BindTexture(CManager::GetResource("data/tex/MessageWindow.png"));

	SetPosition(D3DXVECTOR3(640.0f, 500.0f, 0.0f));		// 背面の位置設定
	SetSize(D3DXVECTOR3(SCREEN_WIDTH, 150.0f, 0.0f));		// 背面の大きさ設定
	SetColor(m_col);
	SetTransform();					// 頂点情報の更新

	m_pMessage = CScene2D::Create(PRIORITY_UI);

	if (m_pMessage != NULL)
	{// メッセージが存在していたとき
		m_pMessage->BindTexture(CManager::GetResource("data/tex/message.png"));		// テクスチャの設定

		m_pMessage->SetPosition(D3DXVECTOR3(640.0f, 500.0f, 0.0f));		// メッセージの位置設定
		m_pMessage->SetSize(D3DXVECTOR3(1000.0f, 150.0f, 0.0f));		// メッセージの大きさ設定
		m_pMessage->SetColor(m_col);				// メッセージのカラー設定
		m_pMessage->SpriteAnimation(D3DXVECTOR2(2.0f, 2.0f), m_type, 1);
		m_pMessage->SetTransform();					// 頂点情報の更新
	}
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CMessageWindow::Uninit(void)
{
	if (m_pMessage != NULL)
	{// メッセージが存在していたとき
		m_pMessage->Release();
	}

	CScene2D::Uninit();
}

//=============================================================================
// 更新処理
//=============================================================================
void CMessageWindow::Update(void)
{
	if (m_fade != FADE_NONE)
	{
		if (m_fade == FADE_IN)
		{
			m_col.a -= 0.05f;				//画面を透明にしていく

			if (m_col.a <= 0.0f)
			{// α値が0以下だったとき
				m_col.a = 0.0f;				// α値を0にする

				if (m_type == MESSAGETYPE_CLEAR)
				{// クリアテロップだったとき
					CFade::SetFade(CManager::MODE_RESULT);		// リザルトに遷移
				}

				Release();			// 削除予約
			}

			SetColor(m_col);		// カラーの更新

			if (m_pMessage != NULL)
			{// メッセージが存在していたとき
				m_pMessage->SetColor(m_col);	// メッセージカラーの更新
			}
		}
		else if (m_fade == FADE_OUT)
		{
			m_col.a += 0.05f;

			if (m_col.a > 1.5f)
			{// α値が1.5より大きいとき
				//テロップ処理に切り替え
				m_col.a = 1.0f;				// α値を1にセット
				m_fade = FADE_TELOP;		// テロップ表示に切り替える
			}

			SetColor(m_col);				// 色を設定

			if (m_pMessage != NULL)
			{// メッセージが存在しているとき
				m_pMessage->SetColor(m_col);		// 色を設定
			}
		}
		else if (m_fade == FADE_TELOP)
		{// テロップ処理だったとき
			m_nCount++;		// カウンタに1プラス

			if (m_nCount > 15)
			{// カウンターが15より大きいとき
				//フェードイン処理に切り替え
				m_fade = FADE_IN;
			}
		}
	}
	CScene2D::Update();			// 更新処理
}

//=============================================================================
// 描画処理
//=============================================================================
void CMessageWindow::Draw(void)
{
	CScene2D::Draw();
}

//=============================================================================
// クリエイト処理
//=============================================================================
CMessageWindow *CMessageWindow::Create(MESSAGETYPE type)
{
	CMessageWindow *pMessage;
	pMessage = new CMessageWindow(PRIORITY_UI);

	if (pMessage != NULL)
	{// メッセージが存在していたとき
		pMessage->m_type = type;				// タイプの設定
		pMessage->Init();						// 初期化処理
	}
	return pMessage;
}

//=============================================================================
// ロード処理
//=============================================================================
HRESULT CMessageWindow::Load(void)
{
	//テクスチャの読み込み
	CManager::Load("data/tex/MessageWindow.png");
	CManager::Load("data/tex/message.png");
	return S_OK;
}