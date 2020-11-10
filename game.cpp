//=============================================================================
//
// ゲーム処理 [game.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "game.h"
#include "fade.h"
#include "renderer.h"
#include "scene.h"
#include "manager.h"
#include "inputKeyboard.h"
#include "meshField.h"
#include "player.h"
#include "scene3D.h"
#include "meshSphere.h"
#include "meshCube.h"
#include "meshCapsule.h"
#include "enemy.h"
#include "sky.h"
#include "tree.h"
#include "object.h"
#include "circle.h"
#include "stage.h"
#include "meshOrbit.h"
#include "gauge2D.h"
#include "effect.h"
#include "tree.h"
#include "number.h"
#include "enemyUi.h"
#include "messageWindow.h"
#include "time.h"
#include "result.h"
#include "ui.h"
#include "network.h"
#include "wall.h"
#include "speed.h"
#include "GuideSign.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
CMeshField	*CGame::m_pMeshField = NULL;		// メッシュフィールドのポインタ
CPlayer		*CGame::m_pPlayer = NULL;			// プレイヤーのポインタ
CMeshSphere *CGame::m_pMeshSphere = NULL;		// メッシュスフィアのポインタ
CEnemy		*CGame::m_pEnemy = NULL;			// 敵のポインタ
CSky		*CGame::m_pSky = NULL;				// 空のポインタ
CHouse		*CGame::m_pHouse = NULL;			// 家のポインタ
CSpeed		*CGame::m_pSpeed = NULL;			// 時速のポインタ

//=============================================================================
// コンストラクタ
//=============================================================================
CGame::CGame()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CGame::~CGame()
{

}

//=============================================================================
//Init処理
//=============================================================================
HRESULT CGame::Init(void)
{
	// エフェクトの生成
	CEffect::Create();

	// プレイヤーの生成
	m_pPlayer = CPlayer::Create();

	// 空の作成
	m_pSky = CSky::Create();

	CObject::LoadScript();

	// 各種アセットの生成＆設置
	//CMeshField::LoadRand("data/stage/rand.csv", false);			// 床情報の読込
	//CObject::LoadModel("data/stage/object.csv");					// モデル情報の読込
	//CEnemy::LoadEnemy("data/stage/enemy.csv");					// 敵情報の読込

	CEnemy *pEnemy = CEnemy::Create();			// クリエイト処理

	if (pEnemy != NULL)
	{// 敵が存在していたとき
		pEnemy->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -100.0f));			// ポジションを決める
		pEnemy->SetTarget(TARGETTYPE_PLAYER);				// 攻撃対象の設定
	}

	// メッセージウィンドウの作成
	CMessageWindow::Create(CMessageWindow::MESSAGETYPE_START);

	// 時間のクリエイト処理
	CTime::Create();

	// 時速の生成
	CSpeed::Create();

	// モデル情報の読み込み
	CObject::LoadModelTest("data/text/model.txt");

	// 壁情報の読み込み
	CMeshWall::LoadWall("data/text/wall.txt", false);

	// 案内矢印の生成
	CGuideSign::Create();

	// ネットワークでのゲーム時初期化処理
	CManager::GetNetwork()->InitGame();

	return S_OK;
}

//=============================================================================
// Update関数
//=============================================================================
void CGame::Update(void)
{

}

//=============================================================================
// Draw関数
//=============================================================================
void CGame::Draw(void)
{

}

//=============================================================================
// Uninit関数
//=============================================================================
void CGame::Uninit(void)
{
	CObject::Unload();

	// ポリゴンの開放
	CScene::ReleaseAll();
}

//=============================================================================
// アセットの読み込み
//=============================================================================
void CGame::LoadAsset(void)
{
	// =============== アセットの読み込み ===============//
	CPlayer::Load();
	CSky::Load();
	CMeshField::Load();
	CMeshOrbit::Load();
	CEnemy::Load();
	CObject::Load();
	CMeshSphere::Load();
	CGauge2D::Load();
	CEffect::Load();
	CNumber::Load();
	CEnemyUi::Load();
	CMessageWindow::Load();
	CGuideSign::Load();
}