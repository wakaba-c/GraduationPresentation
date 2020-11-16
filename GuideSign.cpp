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
#include "debug.h"
#include "game.h"
#include "takaseiLibrary.h"
#include "player.h"
#include "object.h"
#include "math.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define GUIDESIGN "data/model/Arrow.x"
#define SIGN_DISTANCE -200.0f	// カメラからの距離
#define SIGN_HEIGHT 210.0f	// プレイヤーからの高さ
#define ROT_AMOUNT 0.1f		// 回転の差を減らしていく量

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
	DWORD			nNumMat;						// マテリアル情報の数
	LPD3DXBUFFER	pBuffMat;						// マテリアル情報へのポインタ

	CManager::GetModelResource(GUIDESIGN, pBuffMat, nNumMat, pMesh);
	BindModel(pMesh, nNumMat, pBuffMat);

	D3DXVECTOR3 pos, size, rot;
	pos = ZeroVector3;
	size = D3DXVECTOR3(1.5f, 1.5f, 1.5f);
	rot = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);

	SetPosition(pos);			// 位置の設定
	SetSize(size);				// 大きさ設定
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
	CCamera *pCamera = CManager::GetCamera();	// カメラ情報取得
	CPlayer *pPlayer = CGame::GetPlayer();		// プレイヤー情報取得
	D3DXVECTOR3 rotCamera = ZeroVector3;		// カメラの回転変数
	D3DXVECTOR3 pos = GetPosition();			// 位置取得
	D3DXVECTOR3 rot = GetRotation();			// 回転取得
	D3DXVECTOR3 posPlayer = ZeroVector3;		// プレイヤー位置変数
	std::vector<CObject*> pointObj = CObject::GetPointObj();
	int pointNum = CObject::GetPointNum();		// 現在のポイント番号取得
	D3DXVECTOR3 distance;						// 二点間の差
	D3DXVECTOR3 dest;							// 回転の最終目的地座標
	D3DXVECTOR3 Diff;							// 計算変数

	// カメラの情報があるとき
	if (pCamera != NULL)
	{
		rotCamera = pCamera->GetRotation();	// 回転取得
	}

	// プレイヤーがいるとき
	if (pPlayer != NULL)
	{
		posPlayer = pPlayer->GetPosition();	// 位置取得
	}

	// 二点間の差計算
	distance = posPlayer - pointObj[pointNum]->GetPosition();

	// 次の目的地を見る
	dest.y = (float)atan2(distance.x, distance.z);

	// モデルの回転と目標地点の差を格納
	Diff.y = rot.y - dest.y;

	// 回転の補正
	CTakaseiLibrary::RotRevision(&Diff);

	// モデルを徐々に回転させていく
	rot.y -= Diff.y * ROT_AMOUNT;

	// 矢印モデルの位置設定
	pos.x = posPlayer.x + sinf(rotCamera.y - D3DX_PI) * SIGN_DISTANCE;
	pos.y = posPlayer.y + SIGN_HEIGHT;
	pos.z = posPlayer.z + cosf(rotCamera.y - D3DX_PI) * SIGN_DISTANCE;

	// 回転設定
	SetRotation(rot);

	// 位置設定
	SetPosition(pos);

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