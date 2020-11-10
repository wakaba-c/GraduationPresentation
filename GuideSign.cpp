//=============================================================================
//
// 案内矢印処理 [GuideSign.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "GuideSign.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define GUIDESIGN "data/model/yajirusi.x"

//=============================================================================
// コンストラクタ
//=============================================================================
CGuideSign::CGuideSign(CScene::PRIORITY obj = CScene::PRIORITY_UI) : CSceneX(obj)
{
	SetObjType(CScene::PRIORITY_UI);
}

//=============================================================================
// デストラクタ
//=============================================================================
CGuideSign::~CGuideSign()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CGuideSign::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得

	// 初期化処理
	CSceneX::Init();

	SetShader(SHADERTYPE_TOON);

	LPD3DXMESH		pMesh;							// メッシュ情報へのポインタ
	DWORD			nNumMat;							// マテリアル情報の数
	LPD3DXBUFFER	pBuffMat;						// マテリアル情報へのポインタ

	CManager::GetModelResource(GUIDESIGN, pBuffMat, nNumMat, pMesh);
	BindModel(pMesh, nNumMat, pBuffMat);

	D3DXVECTOR3 pos, size, rot;
	pos = D3DXVECTOR3(0, 50.0f, 50.0f);
	size = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	rot = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);

	SetPosition(pos);			// 位置の設定
	SetSize(size);			// 大きさ設定
	SetRotation(rot);			// 回転設定
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CGuideSign::Uninit(void)
{
	// 開放処理
	CSceneX::Uninit();
}

//=============================================================================
// 更新処理
//=============================================================================
void CGuideSign::Update(void)
{
	CCamera *pCamera = CManager::GetCamera();
	D3DXVECTOR3 pos = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 rot = D3DXVECTOR3(0, 0, 0);

	// カメラの情報があるとき
	if (pCamera != NULL)
	{
		pos = pCamera->GetPosition();	// 位置取得
		rot = pCamera->GetRotation();	// 回転取得
	}

#ifdef _DEBUG
	Debug();				// デバッグ処理
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CGuideSign::Draw(void)
{
	CSceneX::Draw();
}

//=============================================================================
// クリエイト関数
//=============================================================================
CGuideSign *CGuideSign::Create(void)
{
	CGuideSign *pGuideSign = NULL;
	pGuideSign = new CGuideSign(CScene::PRIORITY_UI);

	if (pGuideSign != NULL)
	{
		pGuideSign->Init();
	}
	return pGuideSign;
}

//=============================================================================
// ロード処理
//=============================================================================
HRESULT CGuideSign::Load(void)
{
	CManager::LoadModel(GUIDESIGN);
	return S_OK;
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CGuideSign::Debug(void)
{

}
#endif