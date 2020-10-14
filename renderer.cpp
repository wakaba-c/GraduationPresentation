//=============================================================================
//
// レンダラー処理 [renderer.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "renderer.h"
#include "inputKeyboard.h"
#include "fade.h"
#include "game.h"
#include "scene.h"
#include "scene2D.h"
#include "fade.h"
#include "camera.h"
#include "light.h"
#include "circle.h"
#include "collider.h"
#include "stage.h"
#include "player.h"
#include "ranking.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
CDebugProc *CRenderer::m_pDebug = NULL;

//=============================================================================
// コンストラクタ
//=============================================================================
CRenderer::CRenderer()
{
	m_bWireFrame = false;
	m_bUpdate = true;
}

//=============================================================================
// デストラクタ
//=============================================================================
CRenderer::~CRenderer()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CRenderer::Init(HINSTANCE hInstance, HWND hWnd, bool bWindow)
{
	D3DDISPLAYMODE d3ddm;			// ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp;	// プレゼンテーションパラメータ

	// Direct3Dオブジェクトの生成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3D == NULL)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));							// ワークをゼロクリア
	d3dpp.BackBufferWidth = SCREEN_WIDTH;						// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;						// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat = d3ddm.Format;						// バックバッファの形式
	d3dpp.BackBufferCount = 1;									// バックバッファの数
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					// ダブルバッファの切り替え(映像信号に同期)
	d3dpp.EnableAutoDepthStencil = TRUE;						// デプスバッファ(Ｚバッファ)とステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;					// デプスバッファとして16bitを使う
	d3dpp.Windowed = bWindow;									// ウィンドウモード
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	// リフレッシュレート(現在の速度に合わせる)
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// クライアント領域を直ちに更新する

	// Direct3Dデバイスの生成
	// [デバイス作成制御]<描画>と<頂点処理>をハードウェアで行なう
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,					// ディスプレイアダプタ
		D3DDEVTYPE_HAL,						// デバイスタイプ
		hWnd,								// フォーカスするウインドウへのハンドル
		D3DCREATE_HARDWARE_VERTEXPROCESSING,// デバイス作成制御の組み合わせ
		&d3dpp,								// デバイスのプレゼンテーションパラメータ
		&m_pD3DDevice)))					// デバイスインターフェースへのポインタ
	{
		// 上記の設定が失敗したら
		// [デバイス作成制御]<描画>をハードウェアで行い、<頂点処理>はCPUで行なう
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&m_pD3DDevice)))
		{
			// 上記の設定が失敗したら
			// [デバイス作成制御]<描画>と<頂点処理>をCPUで行なう
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&m_pD3DDevice)))
			{
				// 初期化失敗
				return E_FAIL;
			}
		}
	}

	m_pDebug = new CDebugProc;
	m_pDebug->Init(hWnd);

	// レンダーステートパラメータの設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面(左回り)をカリングする
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);						// Zバッファを使用
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);						// ライティングモード有効
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				// αブレンドを行う
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// αデスティネーションカラーの指定

	// サンプラーステートの設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// テクスチャステージステートの設定
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// アルファブレンディング処理
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// 最初のアルファ引数
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);	// ２番目のアルファ引数

	//マウスカーソルを消す
	ShowCursor(true);

	//フェード
	m_pFade = CFade::Create();

	// ステージの作成
	m_pStage = CStage::Create();
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CRenderer::Uninit(void)
{
	//デバッグ処理の開放
	if (m_pDebug != NULL)
	{
		m_pDebug->Uninit();			// デバッグの終了処理
		delete m_pDebug;			// デバッグのメモリ解放
		m_pDebug = NULL;			// デバッグのポインタをNULLに指定
	}

	// Direct3Dデバイスの開放
	if (m_pD3DDevice != NULL)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	// Direct3Dオブジェクトの開放
	if (m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}

	//フェードの開放
	if (m_pStage != NULL)
	{
		m_pStage->Uninit();			// ステージの開放処理
		delete m_pStage;			// ステージのメモリ解放
		m_pStage = NULL;			// ステージのポインタにNULLを代入
	}

	//フェードの開放
	if (m_pFade != NULL)
	{
		m_pFade->Uninit();			// フェードの開放処理
		delete m_pFade;				// フェードのメモリ解放
		m_pFade = NULL;				// フェードのポインタにNULLを代入
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CRenderer::Update(void)
{
	CCamera *pCamera = CManager::GetCamera();
	CLight *pLight = CManager::GetLight();

	m_pDebug->Update();
	Debug();

	if (m_bUpdate)
	{// 更新が有効だったとき
		CScene::UpdateAll();
	}

	// 各種更新
	pCamera->Update();
	pLight->Update();
	m_pFade->Update();
	m_pStage->Update();
}

//=============================================================================
// 描画処理
//=============================================================================
void CRenderer::Draw(void)
{
	// バックバッファ＆Ｚバッファのクリア
	m_pD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// 描画の開始
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{
		CCamera *pCamera = CManager::GetCamera();

		CScene::DrawAll();

		CDebugProc::Log("FPS : %d", GetFPS());									// FPS

		if (m_bWireFrame)
		{// 　ワイヤーフレームが有効だったとき
			m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			m_pDebug->Draw();
			m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		}
		else
		{
			m_pDebug->Draw();
		}

		// 当たり判定の可視化
		CCollider::DrawAll();

		m_pStage->Draw();
		m_pFade->Draw();
		pCamera->SetCamera();

		// 描画の終了
		m_pD3DDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

//=============================================================================
// 更新処理の有無
//=============================================================================
void CRenderer::SetUpdate(bool bValue)
{
	m_bUpdate = bValue;
}

//=============================================================================
// デバッグ処理
//=============================================================================
void CRenderer::Debug(void)
{
	bool bWork = m_bWireFrame;

	ImGui::Begin("System");

	ImGui::Checkbox("Wire frame", &m_bWireFrame);

	ImGui::End();

	if (bWork != m_bWireFrame)
	{
		if (m_bWireFrame)
		{
			m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		}
		else
		{
			m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}
	}
}