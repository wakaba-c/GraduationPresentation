//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "player.h"
#include "game.h"
#include "manager.h"
#include "renderer.h"
#include "inputKeyboard.h"
#include "inputController.h"
#include "scene3D.h"
#include "meshField.h"
#include "model.h"
#include "meshCapsule.h"
#include "colliderSphere.h"
#include "meshCube.h"
#include "enemy.h"
#include "meshOrbit.h"
#include "camera.h"
#include "colliderBox.h"
#include "stage.h"
#include "sound.h"
#include "scene2D.h"
#include "effect.h"
#include "result.h"
#include "fade.h"
#include "takaseiLibrary.h"
#include "sceneX.h"
#include "speed.h"
#include "wall.h"
#include "object.h"
#include "puzzle.h"
#include "number.h"
#include "network.h"
#include "distanceNext.h"
#include "ui.h"
#include "shadow.h"
#include "characterSelect.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define ROT_AMOUNT 0.1f								// 回転の差を減らしていく量
#define ROT_SPEED_DRIFT 0.5f						// ドリフト時回転速度
#define MODEL_FRONT 2								// モデル前輪番号
#define MODEL_REAR 1								// モデル後輪番号
#define MODEL_TIRE 2								// タイヤモデルの数
#define CAMERA_ROT_SPEED 0.4f						// カメラの回転速度
#define TIRE_ROT_SPEED 0.1f							// タイヤの回転速度
#define ACCEKERATION 3.0f							// ドリフト加速度初期値
#define ACCEKERATION_ADDITION 0.02f					// ドリフト加速度加算量
#define DRIFT_DECREACE 0.6f							// ドリフト時速度減少
#define DRIFT_DEST 0.25f							// ドリフト時タイヤの向き
#define	INIT_ROT 1.38f
#define SLOPE_SPEED 0.3f							// 坂の速度

//=============================================================================
// コンストラクタ
//=============================================================================
CPlayer::CPlayer(CScene::PRIORITY obj = CScene::PRIORITY_PLAYER) : CCharacter(obj)
{
	// 優先度の設定
	SetObjType(CScene::PRIORITY_PLAYER);				// オブジェクトタイプ

	m_nLife = 100;										// 体力の初期化
	m_rot = D3DXVECTOR3(0.0f, INIT_ROT, 0.0f);			// 回転の初期化
	m_fSpeed = 0;										// スピードの初期化
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動量の初期化
	m_dest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動先の初期化
	m_difference = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 差の初期化
	m_vectorOld = ZeroVector3;							// 前回のベクトル向き
	m_cameraRot = D3DXVECTOR3(0, D3DX_PI, 0);			// カメラの回転情報初期化
	m_pColPlayerSphere = NULL;							// プレイヤー当たり判定ポインタの初期化
	m_pDistanceNext = NULL;								// 次のプレイヤーとの距離のUI
	m_bHit = false;										// 当たり判定フラグの初期亜化
	m_bJump = false;									// ジャンプフラグの初期化
	m_nActionCount = 0;									// アクションカウンタの初期化
	m_nParticleCount = 0;								// パーティクルカウンタの初期化
	m_nPointNum = 0;									// ポイント番号初期化
	m_fDeathblow = 0.0f;								// 必殺技ポイントの初期化
	m_fAcceleration = ACCEKERATION;						// 加速度
	m_fSlopeSpeed = 0.0f;								// 坂の速度
	m_bEvent = true;									// イベント発生フラグの初期化
	m_bMove = false;									// 現在動いているかのフラグ
	m_bAccel = false;									// アクセルを押しているかのフラグ
	m_bColliderWithWall = true;							// 壁の当たり判定
	m_bGoal = false;									// ゴールフラグ
	m_bSlope = false;									// 坂にいるかどうかのフラグ

	m_pRank = NULL;

	m_nRound = 0;			// 現在の周回回数

	for (int nCnt = 0; nCnt < DRIFT_MAX; nCnt++)
	{
		m_bDrift[nCnt] = false;							// ドリフトフラグ判定
	}
}

//=============================================================================
// デストラクタ
//=============================================================================
CPlayer::~CPlayer()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CPlayer::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice;
	CRenderer *pRenderer = CManager::GetRenderer();
	CNetwork *pNetwork = CManager::GetNetwork();

	//デバイスを取得する
	pDevice = pRenderer->GetDevice();
	m_fPuzzleMaxSPeed = 0;
	CCamera *pCamera = CManager::GetCamera();
	D3DXVECTOR3 pos = GetPosition();							// プレイヤーの位置取得

	switch (pNetwork->GetId())
	{
	case 0:
		pos = D3DXVECTOR3(1286.00f, -4113.86f, 15932.30f);			// プレイヤーの位置設定
		break;
	case 1:
		pos = D3DXVECTOR3(1390.30f, -4113.86f, 15296.58f);			// プレイヤーの位置設定
		break;
	case 2:
		pos = D3DXVECTOR3(1191.18f, -4113.86f, 16327.53f);			// プレイヤーの位置設定
		break;
	case 3:
		pos = D3DXVECTOR3(1105.84f, -4113.86f, 16769.35f);			// プレイヤーの位置設定
		break;
	}

	if (pCamera != NULL)
	{
		pCamera->SetPosCamera(pos, D3DXVECTOR3(0.0f, D3DX_PI, 0.0f));
	}

	// キャラクターの初期化処理
	CCharacter::Init();

	// アニメーションの設定
	AnimationSwitch(ANIMATIONTYPE_NONE);

	//// プレイヤーモデル情報の読み込み
	//switch (CCharacterSelect::GetCarType())
	//{
	//case 0:
	//	LoadScript(SCRIPT_CAR01, ANIMATIONTYPE_MAX);
	//	break;
	//case 1:
	//	LoadScript(SCRIPT_CAR02, ANIMATIONTYPE_MAX);
	//	break;
	//case 2:
	//	LoadScript(SCRIPT_CAR03, ANIMATIONTYPE_MAX);
	//	break;
	//case 3:
	//	LoadScript(SCRIPT_CAR04, ANIMATIONTYPE_MAX);
	//	break;
	//}

	if (CManager::GetMode() == CManager::MODE_GAME)
	{
		// プレイヤーの当たり判定を生成
		m_pColPlayerSphere = CColliderSphere::Create(false, 50.0f);

		if (m_pColPlayerSphere != NULL)
		{ //球体のポインタがNULLではないとき
			m_pColPlayerSphere->SetScene(this);
			m_pColPlayerSphere->SetTag("player");										// タグ の設定
			m_pColPlayerSphere->SetPosition(pos);										// 位置 の設定
			m_pColPlayerSphere->SetOffset(D3DXVECTOR3(0.0f, 20.0f, 0.0f));
		}
	}

	// 位置の設定
	SetPosition(pos);

	int nCntPiece = CPuzzle::GetPieceNum();

	for (int nCnt = 0; nCnt < nCntPiece; nCnt++)
	{
		m_fPuzzleSpeed[nCnt] = CPuzzle::GetSpeed(nCnt);
		m_fPuzzleDecay[nCnt] = CPuzzle::GetDecay(nCnt);
		m_fPuzzlePower[nCnt] = CPuzzle::GetPower(nCnt);
		m_fPuzzleRate[nCnt] = CPuzzle::GetRate(nCnt);
		m_fPuzzleTurning[nCnt] = CPuzzle::GetTurning(nCnt);
	}

	SetSpeed(nCntPiece);

	if (m_fPuzzleMaxSPeed <= 0)
	{
		m_fPuzzleMaxSPeed = NORMAL_SPEED;
	}

	for (int nCnt = 0; nCnt < nCntPiece; nCnt++)
	{
		m_bRankingSign[nCnt] = false;
	}
	for (int nCnt = 0; nCnt < nCntPiece; nCnt++)
	{
		m_bRankingSign[nCnt] = CPuzzle::GetRank(nCnt);
		if (m_bRankingSign[nCnt] == true)
		{
			m_bRanking = true;
		}
	}
	if (m_bRanking == true)
	{
		m_pRank = CNumber::Create();

		if (m_pRank != NULL)
		{
			m_pRank->BindTexture("data/tex/number_rank.png");
			m_pRank->SetPosition(D3DXVECTOR3(1110.0f, 75.0f, 0.0f));
			m_pRank->SetSize(D3DXVECTOR3(100.0f, 100.0f, 0.0f));
			m_pRank->SetTransform();
		}
		CUi *pRankUi = CUi::Create();

		if (pRankUi != NULL)
		{
			pRankUi->LoadScript("data/text/ui/NowRank.txt");
			pRankUi->SetPosition(D3DXVECTOR3(1150.0f, 100.0f, 0.0f));
		}
	}

	if (CManager::GetMode() == CManager::MODE_GAME)
	{
		m_pDistanceNext = CDistanceNext::Create();

		if (m_pDistanceNext != NULL)
		{
			m_pDistanceNext->SetPosition(D3DXVECTOR3(200.0f, 80.0f, 0.0f));
			m_pDistanceNext->SetDistance(D3DXVECTOR3(-10.0f, -8.0f, 0.0f));
			m_pDistanceNext->SetIntervalNum(D3DXVECTOR3(45.0f, 0.0f, 0.0f));
			m_pDistanceNext->SetNumber(256);
		}
	}

	// 影の生成
	m_pShadow = CShadow::Create();

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CPlayer::Uninit(void)
{
	if (m_pColPlayerSphere != NULL)
	{// 武器の当たり判定が存在していたとき
		m_pColPlayerSphere->Release();
	}

	if (m_pDistanceNext != NULL)
	{
		m_pDistanceNext->Uninit();
		delete m_pDistanceNext;
		m_pDistanceNext = NULL;
	}

	CCharacter::Uninit();
}

//=============================================================================
// 更新処理
//=============================================================================
void CPlayer::Update(void)
{
	D3DXVECTOR3 pos;
	CSound *pSound = CManager::GetSound();
	CNetwork *pNetwork = CManager::GetNetwork();
	float fHeight = 0.0f;
	CModel *pModel = GetModel();

	// アニメーション情報の取得
	ANIMATIONTYPE animType = (ANIMATIONTYPE)GetAnimType();
	int currentKey = GetCurrentKey();
	int currentFrame = GetCurrentFrame();
	bool bAnimPlayState = GetAnimPlayState();

	// 坂でのプレイヤー処理
	//m_bSlope = SlopeMove();

	// 入力処理
	if (!m_bHit)
	{// 当たっていなかったとき
		CCamera *pCamera = CManager::GetCamera();
		if (pCamera->GetStalker())
		{
			// 入力処理
			Input();
		}
	}

	pos = GetPosition();				// 位置の取得

	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();

	VERTEX_PLANE plane = {};

	CCollider::RayBlockCollision(pos, &pModel[0].GetMtxWorld(), 110, 250.0f, plane);

	D3DXVECTOR3 AB = plane.a - plane.b;
	D3DXVECTOR3 BC = plane.b - plane.c;

	D3DXVECTOR3 norwork;

	D3DXVec3Cross(&norwork, &BC, &AB);
	D3DXVec3Normalize(&norwork, &norwork);

	//CDebugProc::Log("a地点 : %f, %f, %f\n", plane.a.x, plane.a.y, plane.a.z);
	//CDebugProc::Log("b地点 : %f, %f, %f\n", plane.b.x, plane.b.y, plane.b.z);
	//CDebugProc::Log("c地点 : %f, %f, %f\n", plane.c.x, plane.c.y, plane.c.z);

	//床の高さを取得する
	CScene *pSceneNext = NULL;														// 初期化
	CScene *pSceneNow = GetScene(PRIORITY_FLOOR);									// シーンの先頭アドレスを取得

	CScene *pScene = NowFloor(pos);													// 現在いるフィールドを取得

	if (pScene != NULL)
	{// 今立っている床が存在したとき
		CMeshField *pFloor = (CMeshField*)pScene;									// キャスト
		fHeight = pFloor->GetHeight(pos);

		RANDTYPE Type = pFloor->GetRandType();

		if (animType == ANIMATIONTYPE_RUN)
		{
			if (currentKey == 5 || currentKey == 0)
			{
				if (currentFrame == 0)
				{
					if (Type == RANDTYPE_GRASS)
					{
						//	pSound->PlaySoundA((SOUND_LABEL)(CManager::GetRand(3) + (int)SOUND_LABEL_SE_GRASS_1));
					}
					else if (Type == RANDTYPE_SAND)
					{
						//	pSound->PlaySoundA((SOUND_LABEL)(CManager::GetRand(3) + (int)SOUND_LABEL_SE_SAND_1));
					}
				}
			}
		}
	}

	// 位置更新
	pos += m_move;

	// ジャンプしていないとき
	if (!m_bJump)
	{
		// 減速
		m_move.x += (0 - m_move.x) * CManager::GetSpeedDampingRate();
		m_move.z += (0 - m_move.z) * CManager::GetSpeedDampingRate();
	}

	////重力処理
	//if (m_bJump)
	//{// ジャンプしていたとき
	//	if (animType != ANIMATIONTYPE_JUMP_1)
	//	{
	//		if (pos.y > fHeight)
	//		{// 現在の高さが床より高かったとき
	//			m_move.y += -0.7f;
	//		}
	//		else
	//		{// 現在の高さが床より低かった時
	//			pos.y = fHeight;											// 床の高さを求める
	//			m_move.y = 0.0f;
	//			if (m_bJump)
	//			{
	//				// 砂煙のエフェクト表現
	//				CEffect::SandSmokeEffect(pos);
	//				m_bJump = false;										// ジャンプ判定を変える
	//				SetAnimPlayState(true);									// アニメーションの再開
	//				AnimationSwitch(ANIMATIONTYPE_JUMP_5);					// アニメーションの変更
	//				pSound->PlaySoundA(SOUND_LABEL_SE_LANDING);				// 着地音の再生
	//			}
	//		}
	//	}
	//}
	//else
	//{
	//	pos.y = fHeight;											// 床の高さを求める
	//}

	// 位置設定
	SetPosition(pos);

	if (m_bColliderWithWall)
	{
		// 当たり判定管理処理
		Collision();
	}

	if (m_pColPlayerSphere != NULL)
	{// 武器の当たり判定が存在していたとき
		m_pColPlayerSphere->SetPosition(pos);
	}

	// キャラクターの更新処理
	CCharacter::Update();

	if (m_pRank != NULL)
	{
		m_pRank->SetNumber(pNetwork->GetRank(pNetwork->GetId()));
	}

	if (m_pDistanceNext != NULL)
	{
		m_pDistanceNext->Update();
	}

	// ドリフトしていたとき
	if (m_bDrift[DRIFT_LEFT] || m_bDrift[DRIFT_RIGHT])
	{
		// パーティクル全体の位置計算
		D3DXVECTOR3 vecPos = D3DXVECTOR3(-25.0f, 0.0f, -5.0f);
		D3DXMATRIX mtxMeshRot, mtxMeshTrans;				// 計算用マトリックス
		D3DXMATRIX mtx;										// 武器のマトリックス
		D3DXMATRIX mtxPlayer;

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtx);

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtxPlayer);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, 0.0f, 0.0f, 0.0f);
		D3DXMatrixMultiply(&mtxPlayer, &mtxPlayer, &mtxMeshRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxMeshTrans, vecPos.x, vecPos.y, vecPos.z);
		D3DXMatrixMultiply(&mtxPlayer, &mtxPlayer, &mtxMeshTrans);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, 0.0f, 0.0f, 0.0f);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxMeshTrans, pos.x, pos.y, pos.z);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshTrans);

		D3DXMatrixMultiply(&mtx, &mtx, &mtxPlayer);

		for (int nCount = 0; nCount < 2; nCount++)
		{
			CEffect::CreateEffect("spark", D3DXVECTOR3(mtx._41, mtx._42, mtx._43), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		}

		// パーティクル全体の位置計算
		vecPos = D3DXVECTOR3(25.0f, 0.0f, -5.0f);

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtx);

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtxPlayer);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, 0.0f, 0.0f, 0.0f);
		D3DXMatrixMultiply(&mtxPlayer, &mtxPlayer, &mtxMeshRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxMeshTrans, vecPos.x, vecPos.y, vecPos.z);
		D3DXMatrixMultiply(&mtxPlayer, &mtxPlayer, &mtxMeshTrans);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, 0.0f, 0.0f, 0.0f);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxMeshTrans, pos.x, pos.y, pos.z);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshTrans);

		D3DXMatrixMultiply(&mtx, &mtx, &mtxPlayer);

		for (int nCount = 0; nCount < 2; nCount++)
		{
			CEffect::CreateEffect("spark", D3DXVECTOR3(mtx._41, mtx._42, mtx._43), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		}
	}

	if (CSpeed::GetSpeed() > 10)
	{
		CEffect::SandSmoke(pos + D3DXVECTOR3(0.0f, 50.0f, 0.0f));
	}

#ifdef _DEBUG
	CDebugProc::Log("速度 : %.2f", CSpeed::GetSpeed());

	Debug();
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CPlayer::Draw(void)
{
	CCharacter::Draw();
}

//=============================================================================
// クリエイト処理
//=============================================================================
CPlayer *CPlayer::Create(void)
{
	CPlayer *pPlayer;
	pPlayer = new CPlayer(CScene::PRIORITY_PLAYER);
	pPlayer->Init();
	return pPlayer;
}

//=============================================================================
// ロード処理
//=============================================================================
HRESULT CPlayer::Load(void)
{
	return S_OK;
}

//=============================================================================
// 必殺技ポイント数の設定
//=============================================================================
void CPlayer::SetDeathblow(float nValue)
{
	m_fDeathblow = nValue;
}

//=============================================================================
// イベントフラグの設定
//=============================================================================
void CPlayer::SetEvent(bool bValue)
{
	m_bEvent = bValue;
}

//=============================================================================
// ゴールフラグの設定
//=============================================================================
void CPlayer::SetGoalState(bool bValue)
{
	m_bGoal = bValue;
}

//=============================================================================
// 当たり判定(trigger)
//=============================================================================
void CPlayer::OnTriggerEnter(CCollider *col)
{
	std::string sTag = col->GetTag();
	CModel *pModel = GetModel();
	std::vector<CObject*> pointObj = CObject::GetPointObj();

	if (col->GetScene()->GetObjType() == PRIORITY_ENEMY)
	{
		if (sTag == "weapon")
		{
			CSound *pSound = CManager::GetSound();				// サウンドの取得
		//	pSound->PlaySoundA(SOUND_LABEL_SE_PUNCH);			// ダメージ音の再生
			m_nLife -= 5;										// 体力を削る
			AnimationSwitch(ANIMATIONTYPE_DAMAGE);				// アニメーションを変更

			D3DXVECTOR3 vec;

			vec = GetPosition() - col->GetPosition();			//差分を求める(方向を求めるため)
			D3DXVec3Normalize(&vec, &vec);						//正規化する

			m_move.x = vec.x * 25;
			m_move.z = vec.z * 25;

			if (m_nLife < 0)
			{
				CResult::SetIdxKill(CEnemy::GetEnemyKill());			// Kill数をリザルトに渡す
				CFade::SetFade(CManager::MODE_RESULT, CFade::FADETYPE_SLIDE);					// リザルトに遷移
			}
		}
	}
	if (sTag == "house")
	{

	}
	if (sTag == "goal")
	{
		CNetwork *pNetwork = CManager::GetNetwork();
		pNetwork->SendTCP("GOAL", sizeof("GOAL"));
		m_bEvent = true;

		//if (CFade::GetFade() == CFade::FADE_NONE)
		//{//フェードが処理をしていないとき
		//	CFade::SetFade(CManager::MODE_RESULT, CFade::FADETYPE_NORMAL);					//フェードを入れる
		//}
	}
}

//=============================================================================
// 当たり判定(collider)
//=============================================================================
void CPlayer::OnCollisionEnter(CCollider *col)
{
	std::string sTag = col->GetTag();

	if (sTag == "enemy")
	{
		 //当たり状態なので、滑らせる
		//D3DXVECTOR3 move;
		//CManager::calcReflectVector(&move, m_move, normal);
		//m_move = move * 20;

		// パーティクルの再現
		for (int nCount = 0; nCount < 5; nCount++)
		{
			CEffect::CreateEffect("star", GetPosition(), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		}
	}
}

//========================================================================================
// アニメーションフレームの最大数に到達したときの処理
//========================================================================================
void CPlayer::BehaviorForMaxFrame(void)
{

}

//========================================================================================
// アニメーションキーの最大数に到達したときの処理
//========================================================================================
void CPlayer::BehaviorForMaxKey(void)
{
	CModel *pModel = GetModel();
	ANIMATIONTYPE animType = (ANIMATIONTYPE)GetAnimType();
	D3DXVECTOR3 rot = pModel[0].GetRotation();		// 回転量取得

}

//=============================================================================
// 近くにいる敵の近くまで移動する処理
//=============================================================================
void CPlayer::MoveNearEnemy(void)
{
	bool bTarget = false;				// 範囲内に敵が存在していたとき
	float fMinDistance = 750000.0f;		// 現在の最短距離
	float fDistance = 0.0f;				// 現在の距離
	D3DXVECTOR3 enemyPos;				// 最短距離にいる敵の位置
	CScene *pSceneNext = NULL;														// 初期化
	CScene *pSceneNow = CScene::GetScene(CScene::PRIORITY_ENEMY);					// 先頭アドレスの取得

	// 次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = CScene::GetSceneNext(pSceneNow, CScene::PRIORITY_ENEMY);		//次回アップデート対象を控える
		fDistance = CManager::GetDistance(GetPosition(), pSceneNow->GetPosition());		// 距離を求める

		if (fDistance < fMinDistance)
		{
			if (!bTarget)
			{// ターゲットの存在が確認されていなかったとき
				bTarget = true;				// 位置の移動を許可する
			}

			enemyPos = pSceneNow->GetPosition();			// 敵の位置を記録する
		}

		pSceneNow = pSceneNext;								//次回アップデート対象を格納
	}

	if (bTarget)
	{// 移動の許可が出ていたとき
		SetPosition(enemyPos);			// 位置の移動
	}
}

//=============================================================================
// 当たり判定処理
//=============================================================================
void CPlayer::Collision(void)
{
	// 壁の当たり判定
	m_bHit = CollisionWall();
}

//=============================================================================
// キー入力情報処理
//=============================================================================
void CPlayer::Input(void)
{
	ANIMATIONTYPE animType = (ANIMATIONTYPE)GetAnimType();

	// キーボードの取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();

	// ゲームパッドの取得
	CInputController *pGamepad = CManager::GetInputController();

	// カメラの取得
	CCamera *pCamera = CManager::GetCamera();

	// モデルの取得
	CModel *pModel = GetModel();

	D3DXVECTOR3 rot = pCamera->GetRotation();
	D3DXVECTOR3 Diff;	// 計算用格納変数
	float nValueH = 0;									//コントローラー
	float nValueV = 0;									//コントローラー

	D3DXVECTOR3 modelFrontDiff = D3DXVECTOR3(0, 0, 0);			// モデル前輪計算用マトリックス
	D3DXVECTOR3 aVec = D3DXVECTOR3(0, 0, 0);					// プレイヤー加速度ベクトル
	D3DXVECTOR3 cameraVec = D3DXVECTOR3(0, 0, 0);				// カメラの方向ベクトル
	D3DXVECTOR3 moveVec = D3DXVECTOR3(0, 0, 0);					// プレイヤー運動ベクトル
	D3DXVECTOR3 fModelRot = pModel[MODEL_FRONT].GetRotation();	// モデル前輪回転情報
	D3DXVECTOR3 fModelRotRear = pModel[MODEL_REAR].GetRotation();// モデル後輪回転情報
	float fTireRotSpeed = 0.0f;									// タイヤ回転速度
	m_fSpeed = 0;

	// ====================== コントローラー ====================== //
	if (!m_bEvent)
	{// イベントが発生していなかったとき
		if (pGamepad != NULL)
		{// ゲームパッドが存在したとき
			if (pGamepad->GetJoypadUse(0))
			{// 使用可能だったとき
				pGamepad->GetJoypadStickLeft(0, &nValueH, &nValueV);

				//上下操作
				if (pGamepad->GetControllerPress(0, JOYPADKEY_A))
				{
					// 前輪モデルの最終目的座標
					m_dest.y = 0.0f;

					// 速度設定
					m_fSpeed = -m_fPuzzleMaxSPeed;

					// タイヤ回転方向設定
					fTireRotSpeed = TIRE_ROT_SPEED;

					// 動いていい
					m_bMove = true;

					// アクセルボタンを押した
					m_bAccel = true;
				}
				else if (pGamepad->GetControllerPress(0, JOYPADKEY_B))
				{
					// 前輪モデルの最終目的座標
					m_dest.y = 0.0f;

					// 速度設定
					m_fSpeed = m_fPuzzleMaxSPeed;

					// タイヤ回転方向設定
					fTireRotSpeed = -TIRE_ROT_SPEED;

					// 動いていい
					m_bMove = true;
				}

				// アクセルボタンを離したとき
				if (!pGamepad->GetControllerPress(0, JOYPADKEY_A))
				{
					// アクセルボタンを離した
					m_bAccel = false;
				}

				// ドリフトボタンを押していないとき
				if (!pGamepad->GetControllerPress(0, JOYPADKEY_RIGHT_TRIGGER))
				{
					// 左にスティックが倒れたとき
					if (nValueH <= JOY_MAX_RANGE && nValueH > 0)
					{
						// 前輪モデルの最終目的座標
						m_dest.y = -CManager::GetTurnVelocity();
					}
					else if (nValueH >= -JOY_MAX_RANGE && nValueH < 0)
					{// 右にスティックが倒れたとき
					 // 前輪モデルの最終目的座標
						m_dest.y = CManager::GetTurnVelocity();
					}

					// ブレーキボタンが押されたとき
					if (pGamepad->GetControllerPress(0, JOYPADKEY_B))
					{
						// 左にスティックが倒れたとき
						if (nValueH <= JOY_MAX_RANGE && nValueH > 0)
						{
							// 前輪モデルの最終目的座標
							m_dest.y = CManager::GetTurnVelocity();
						}
						else if (nValueH >= -JOY_MAX_RANGE && nValueH < 0)
						{// 右にスティックが倒れたとき
						 // 前輪モデルの最終目的座標
							m_dest.y = -CManager::GetTurnVelocity();
						}
					}
				}

				// アクセル状態のとき
				if (m_bAccel)
				{
					// ドリフトしていないとき
					if (!m_bDrift[DRIFT_RIGHT] && !m_bDrift[DRIFT_LEFT])
					{
						// ドリフトボタンを押したとき
						if (pGamepad->GetControllerPress(0, JOYPADKEY_RIGHT_TRIGGER))
						{
							// 左にスティックが倒れたとき
							if (nValueH <= JOY_MAX_RANGE && nValueH > 0)
							{
								// ドリフトしている状態にする
								m_bDrift[DRIFT_LEFT] = true;
							}
							else if (nValueH >= -JOY_MAX_RANGE && nValueH < 0)
							{// 右にスティックが倒れたとき
								// ドリフトしている状態にする
								m_bDrift[DRIFT_RIGHT] = true;
							}
						}
					}

					// 右ドリフトしているとき
					if (m_bDrift[DRIFT_RIGHT])
					{
						// 前輪モデルの最終目的地座標
						m_dest.y = DRIFT_DEST;

						// 左にスティックが倒れたとき
						if (nValueH <= JOY_MAX_RANGE && nValueH > 0)
						{
							// 前輪モデルの最終目的地座標
							m_dest.y = 0.0f;

							// 加速度
							m_fAcceleration -= ACCEKERATION_ADDITION;
						}
						else if (nValueH >= -JOY_MAX_RANGE && nValueH < 0)
						{// 右にスティックが倒れたとき
						 // 前輪モデルの最終目的地座標
							m_dest.y = ROT_SPEED_DRIFT;

							// 加速度
							m_fAcceleration += ACCEKERATION_ADDITION;
						}

						// 加速度ベクトル設定
						aVec.x = sinf(m_rot.y + m_dest.y + D3DX_PI / 2) * m_fAcceleration + m_fSlopeSpeed;
						aVec.z = cosf(m_rot.y + m_dest.y + D3DX_PI / 2) * m_fAcceleration + m_fSlopeSpeed;

						// ドリフトボタンを離したとき
						if (!pGamepad->GetControllerPress(0, JOYPADKEY_RIGHT_TRIGGER))
						{
							// ドリフト最大までカウント
							for (int nCnt = 0; nCnt < DRIFT_MAX; nCnt++)
							{
								// ドリフトしていない状態にする
								m_bDrift[nCnt] = false;

								// 加速度初期化
								m_fAcceleration = ACCEKERATION;
							}
						}
					}
					else if (m_bDrift[DRIFT_LEFT])
					{// 左ドリフトのとき
						// 前輪モデルの最終目的地座標
						m_dest.y = -DRIFT_DEST;

						// 左にスティックが倒れたとき
						if (nValueH <= JOY_MAX_RANGE && nValueH > 0)
						{
							// 前輪モデルの最終目的地座標
							m_dest.y = -ROT_SPEED_DRIFT;

							// 加速度
							m_fAcceleration += ACCEKERATION_ADDITION;
						}
						else if (nValueH >= -JOY_MAX_RANGE && nValueH < 0)
						{// 右にスティックが倒れたとき
							// 前輪モデルの最終目的地座標
							m_dest.y = 0.0f;

							// 加速度
							m_fAcceleration -= ACCEKERATION_ADDITION;
						}

						// 加速度ベクトル設定
						aVec.x = sinf(m_rot.y + m_dest.y - D3DX_PI / 2) * m_fAcceleration + m_fSlopeSpeed;
						aVec.z = cosf(m_rot.y + m_dest.y - D3DX_PI / 2) * m_fAcceleration + m_fSlopeSpeed;

						// ドリフトボタンを離したとき
						if (!pGamepad->GetControllerPress(0, JOYPADKEY_RIGHT_TRIGGER))
						{
							// ドリフト最大までカウント
							for (int nCnt = 0; nCnt < DRIFT_MAX; nCnt++)
							{
								// ドリフトしていない状態にする
								m_bDrift[nCnt] = false;

								// 加速度初期化
								m_fAcceleration = ACCEKERATION;
							}
						}
					}
				}

#ifdef _DEBUG
				CDebugProc::Log("移動量 : %.2f %.2f %.2f", m_move.x, m_move.y, m_move.z);

				if (pGamepad->GetControllerPress(0, JOYPADKEY_A))
				{
					CDebugProc::Log("コントローラー : 0番\n");
				}
				if (pGamepad->GetControllerPress(1, JOYPADKEY_A))
				{
					CDebugProc::Log("コントローラー : 1番\n");
				}

				CDebugProc::Log("コントローラーH : %f\n", nValueH);
				CDebugProc::Log("コントローラーV : %f\n", nValueV);
#endif
			}
		}

		// ====================== キーボード ====================== //

		if (pKeyboard->GetTriggerKeyboard(DIK_1))
		{
			for (int nCount = 0; nCount < 20; nCount++)
			{
				float fAngle = float(CManager::GetRand(314)) - float(CManager::GetRand(314));
				float fAngle_x = float(CManager::GetRand(314)) - float(CManager::GetRand(314));

				D3DXVECTOR3 particlePos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

				particlePos.x = cosf(D3DX_PI + fAngle) * cosf(D3DX_PI + fAngle_x);
				particlePos.y = sinf(D3DX_PI + fAngle_x);
				particlePos.z = sinf(D3DX_PI + fAngle) * cosf(D3DX_PI + fAngle_x);

				fAngle = float(CManager::GetRand(314)) / 100.0f - float(CManager::GetRand(314)) / 100.0f;
				D3DXVECTOR3 rot;
				rot = D3DXVECTOR3(sinf(fAngle) * 10, cosf(fAngle) * 10, 0.0f);

				CEffect::SetEffect("data/tex/effect/rose_01.png",			// パーティクルのタイプ
					GetPosition(),											// 発生位置
					D3DXVECTOR3(8.0f, 8.0f, 0.0f),							// サイズ
					particlePos * 5.0f,										// 方向ベクトル
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					D3DXVECTOR3(0.02f, 0.02f, 0.0f),						// 回転の変化量
					D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),						// 色の変化量
					EASINGTYPE_NONE,
					rot,													// テクスチャの回転
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),						// カラー
					200,													// パーティクルの生存カウント数
					-0.98f,													// 重力
					0,														// 抵抗
					true,													// ビルボード
					0,														// 表示する箇所(横)
					0,														// 表示する箇所(縦)
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					0,
					0,
					0,
					D3DXVECTOR2(1.0f, 1.0f),								// 画像の分割数
					false,													// 加算合成の有無
					false,													// Zバッファの比較有無
					false													// フェード
				);
			}

		}

		if (pKeyboard->GetTriggerKeyboard(DIK_2))
		{
			CEffect::PetalCluster(GetPosition(), D3DXVECTOR3(D3DX_PI / 2, m_rot.y, m_rot.z));
		}
	}

	//static int nCntAura = 0;
	//nCntAura++;

	//CEffect::Aura(nCntAura, false, GetPosition());

	// モデルがあるとき
	if (pModel != NULL)
	{
		if (!m_bEvent)
		{// イベントが発生していなかったとき

		//上下操作
			if (pKeyboard->GetPressKeyboard(MOVE_ACCEL))
			{
				// 前輪モデルの最終目的座標
				m_dest.y = 0.0f;

				// 速度設定
				m_fSpeed = -m_fPuzzleMaxSPeed;

				// タイヤ回転方向設定
				fTireRotSpeed = TIRE_ROT_SPEED;

				// 動いていい
				m_bMove = true;

				// アクセルボタンを押した
				m_bAccel = true;
			}
			else if (pKeyboard->GetPressKeyboard(MOVE_BRAKE))
			{
				// 前輪モデルの最終目的座標
				m_dest.y = 0.0f;

				// 速度設定
				m_fSpeed = m_fPuzzleMaxSPeed;

				// タイヤ回転方向設定
				fTireRotSpeed = -TIRE_ROT_SPEED;

				// 動いていい
				m_bMove = true;
			}

			// アクセルボタンを離したとき
			if (!pKeyboard->GetPressKeyboard(MOVE_ACCEL))
			{
				// アクセルボタンを離した
				m_bAccel = false;
			}

			// ドリフトボタンを押していないとき
			if (!pKeyboard->GetPressKeyboard(MOVE_DRIFT))
			{
				//左右操作
				if (pKeyboard->GetPressKeyboard(MOVE_LEFT))
				{
					// 前輪モデルの最終目的座標
					m_dest.y = -CManager::GetTurnVelocity();
				}
				else if (pKeyboard->GetPressKeyboard(MOVE_RIGHT))
				{
					// 前輪モデルの最終目的座標
					m_dest.y = CManager::GetTurnVelocity();
				}

				// ブレーキボタンが押されたとき
				if (pKeyboard->GetPressKeyboard(MOVE_BRAKE))
				{
					//左右操作
					if (pKeyboard->GetPressKeyboard(MOVE_LEFT))
					{
						// 前輪モデルの最終目的座標
						m_dest.y = CManager::GetTurnVelocity();
					}
					else if (pKeyboard->GetPressKeyboard(MOVE_RIGHT))
					{
						// 前輪モデルの最終目的座標
						m_dest.y = -CManager::GetTurnVelocity();
					}
				}
			}
		}

		// アクセル状態のとき
		if (m_bAccel)
		{
			// ドリフトしていないとき
			if (!m_bDrift[DRIFT_RIGHT] && !m_bDrift[DRIFT_LEFT])
			{
				// ドリフトボタンを押したとき
				if (pKeyboard->GetPressKeyboard(MOVE_DRIFT))
				{
					//左右操作
					if (pKeyboard->GetPressKeyboard(MOVE_LEFT))
					{
						// ドリフトしている状態にする
						m_bDrift[DRIFT_LEFT] = true;
					}
					else if (pKeyboard->GetPressKeyboard(MOVE_RIGHT))
					{
						// ドリフトしている状態にする
						m_bDrift[DRIFT_RIGHT] = true;
					}
				}
			}

			// 右ドリフトしているとき
			if (m_bDrift[DRIFT_RIGHT])
			{
				// 前輪モデルの最終目的地座標
				m_dest.y = DRIFT_DEST;

				//左右操作
				if (pKeyboard->GetPressKeyboard(MOVE_LEFT))
				{
					// 前輪モデルの最終目的地座標
					m_dest.y = 0.0f;

					// 加速度
					m_fAcceleration -= ACCEKERATION_ADDITION;
				}
				else if (pKeyboard->GetPressKeyboard(MOVE_RIGHT))
				{
					// 前輪モデルの最終目的地座標
					m_dest.y = ROT_SPEED_DRIFT;

					// 加速度
					m_fAcceleration += ACCEKERATION_ADDITION;
				}

				// 加速度ベクトル設定
				aVec.x = sinf(m_rot.y + m_dest.y + D3DX_PI / 2) * m_fAcceleration + m_fSlopeSpeed;
				aVec.z = cosf(m_rot.y + m_dest.y + D3DX_PI / 2) * m_fAcceleration + m_fSlopeSpeed;

				// ドリフトボタンを離したとき
				if (!pKeyboard->GetPressKeyboard(MOVE_DRIFT))
				{
					// ドリフト最大までカウント
					for (int nCnt = 0; nCnt < DRIFT_MAX; nCnt++)
					{
						// ドリフトしていない状態にする
						m_bDrift[nCnt] = false;

						// 加速度初期化
						m_fAcceleration = ACCEKERATION;
					}
				}
			}
			else if (m_bDrift[DRIFT_LEFT])
			{// 左ドリフトのとき
			 // 前輪モデルの最終目的地座標
				m_dest.y = -DRIFT_DEST;

				//左右操作
				if (pKeyboard->GetPressKeyboard(MOVE_LEFT))
				{
					// 前輪モデルの最終目的地座標
					m_dest.y = -ROT_SPEED_DRIFT;

					// 加速度
					m_fAcceleration += ACCEKERATION_ADDITION;
				}
				else if (pKeyboard->GetPressKeyboard(MOVE_RIGHT))
				{
					// 前輪モデルの最終目的地座標
					m_dest.y = 0.0f;

					// 加速度
					m_fAcceleration -= ACCEKERATION_ADDITION;
				}

				// 加速度ベクトル設定
				aVec.x = sinf(m_rot.y + m_dest.y - D3DX_PI / 2) * m_fAcceleration + m_fSlopeSpeed;
				aVec.z = cosf(m_rot.y + m_dest.y - D3DX_PI / 2) * m_fAcceleration + m_fSlopeSpeed;

				// ドリフトボタンを離したとき
				if (!pKeyboard->GetPressKeyboard(MOVE_DRIFT))
				{
					// ドリフト最大までカウント
					for (int nCnt = 0; nCnt < DRIFT_MAX; nCnt++)
					{
						// ドリフトしていない状態にする
						m_bDrift[nCnt] = false;

						// 加速度初期化
						m_fAcceleration = ACCEKERATION;
					}
				}
			}
		}

		{// 前輪設定 & タイヤ設定
			// モデルの回転と目標地点の差を格納
			modelFrontDiff.y = fModelRot.y - m_dest.y;

			// 回転の補正
			CTakaseiLibrary::RotRevision(&modelFrontDiff);

			// モデルを徐々に回転させていく
			fModelRot.y -= modelFrontDiff.y * ROT_AMOUNT;
		}

		{// カメラ設定
			// タイヤの回転の半分を差と格納
			cameraVec.y = m_cameraRot.y - (m_rot.y + (m_dest.y * 0.85f));

			// 回転の補正
			CTakaseiLibrary::RotRevision(&cameraVec);

			// カメラを徐々に回転させていく
			m_cameraRot.y -= cameraVec.y * CAMERA_ROT_SPEED;

			// 回転の補正
			CTakaseiLibrary::RotRevision(&m_cameraRot);

			// カメラ回転設定
			SetCameraRot(m_cameraRot);
		}

		// 移動していいとき
		if (m_bMove)
		{
			// タイヤを回す
			fModelRot.x -= fTireRotSpeed;
			fModelRotRear.x -= fTireRotSpeed;

			// 回転の補正
			CTakaseiLibrary::RotRevision(&fModelRot);
			// 回転の補正
			CTakaseiLibrary::RotRevision(&fModelRotRear);

			// モデルの回転の設定
			pModel[MODEL_FRONT].SetRotation(fModelRot);
			// モデルの回転の設定
			pModel[MODEL_REAR].SetRotation(fModelRotRear);

			// ドリフトしているとき
			if (m_bDrift[DRIFT_RIGHT] || m_bDrift[DRIFT_LEFT])
			{
				// 運動ベクトル計算
				moveVec.x += sinf(m_rot.y) * m_fSpeed * DRIFT_DECREACE;
				moveVec.z += cosf(m_rot.y) * m_fSpeed * DRIFT_DECREACE;
			}
			else
			{
				// 運動ベクトル計算
				moveVec.x += sinf(m_rot.y) * m_fSpeed;
				moveVec.z += cosf(m_rot.y) * m_fSpeed;
			}

			// 回転の補正
			CTakaseiLibrary::RotRevision(&m_dest);

			// プレイヤーを徐々に回転させていく
			m_rot.y += m_dest.y * ROT_AMOUNT;
		}

		// アクセルボタンとブレーキボタンを離したとき
		if (!pGamepad->GetControllerPress(0, JOYPADKEY_A) && !pGamepad->GetControllerPress(0, JOYPADKEY_B) &&
			!pKeyboard->GetPressKeyboard(MOVE_ACCEL) && !pKeyboard->GetPressKeyboard(MOVE_BRAKE))
		{
			// 移動不可
			m_bMove = false;
		}

		// 移動量設定
		m_move.x += (moveVec.x + aVec.x) / 2;
		m_move.z += (moveVec.z + aVec.z) / 2;

		// 回転の補正
		CTakaseiLibrary::RotRevision(&m_rot);

		// 回転の設定
		SetRotation(m_rot);
	}

	CSound *pSound = CManager::GetSound();				// サウンドの取得
	if (pKeyboard->GetTriggerKeyboard(DIK_5))
	{

		pSound->PlaySoundA(SOUND_LABEL_SE_kyuin);			// ダメージ音の再生

	}
	if (pKeyboard->GetTriggerKeyboard(DIK_4))
	{

		pSound->PlaySoundA(SOUND_LABEL_SE_Cheers);			// ダメージ音の再生

	}
	if (pKeyboard->GetTriggerKeyboard(DIK_6))
	{

		pSound->PlaySoundA(SOUND_LABEL_SE_Horn);			// ダメージ音の再生

	}

#ifdef _DEBUG
	if (pKeyboard->GetTriggerKeyboard(DIK_K))
	{// Kが押されたとき
		m_fDeathblow = 50.0f;				// 必殺技ポイントを最大値まで上げる
	}
	{// Kが押されたとき
		m_fDeathblow = 50.0f;				// 必殺技ポイントを最大値まで上げる
	}

#endif
}

//=============================================================================
// 壁の当たり判定
//=============================================================================
bool CPlayer::CollisionWall(void)
{
	CScene *pSceneNext = NULL;														// 初期化
	CScene *pSceneNow = CScene::GetScene(CScene::PRIORITY_WALL);					// 先頭アドレスの取得
	D3DXVECTOR3 normal = D3DXVECTOR3_ZERO;

	// 次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = CScene::GetSceneNext(pSceneNow, CScene::PRIORITY_WALL);		//次回アップデート対象を控える
		CMeshWall *pMeshWall = (CMeshWall*)pSceneNow;

		if (pMeshWall->GetWallTest(this, normal, m_rot))
		{
			D3DXVECTOR3 playerPos = GetPosition();

			OutputDebugString("当たった");

			// //当たり状態なので、滑らせる
			D3DXVECTOR3 move;
			CManager::calcReflectVector(&move, m_move, normal);
			m_move = move * 20;
			return true;
		}

		pSceneNow = pSceneNext;								//次回アップデート対象を格納
	}

	return false;
}

//=============================================================================
// レイによる壁の当たり判定
//=============================================================================
bool CPlayer::CollisionWallWithRay(void)
{
	FLOAT fDistance = 0;
	D3DXVECTOR3 vNormal;

	D3DXVECTOR3 rot = CManager::GetCamera()->GetRotation();
	D3DXVECTOR3 endPoint;
	endPoint.x = sinf(D3DX_PI * 1.0f + rot.y);
	endPoint.y = 0.0f;
	endPoint.z = cosf(D3DX_PI * 1.0f + rot.y);

	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 playerPos = pos;
	CModel *pModel = GetModel();
	playerPos.y += 15.0f;

	CDebugProc::Log("加速度 : %.2f %.2f %.2f\n", m_move.x, m_move.y, m_move.z);
	CDebugProc::Log("始点 : %.2f %.2f %.2f\n", pos.x, pos.y, pos.z);
	CDebugProc::Log("終点 : %.2f %.2f %.2f\n", playerPos.x + m_move.x, playerPos.y + m_move.y, playerPos.z + m_move.z);

	if (CObject::Collide(playerPos, m_move, &fDistance, &vNormal, pModel[0].GetMtxWorld()) && fDistance <= 30.0f)
	{
		// 当たり状態なので、滑らせる
		D3DXVECTOR3 move = CManager::Slip(m_move, vNormal);// 滑りベクトルを計算
		//D3DXVec3Normalize(&move, &move);
		//move *= m_fSpeed;
		m_move = move;

		// 滑りベクトル先の壁とのレイ判定 ２重に判定
		if (CObject::Collide(playerPos, playerPos + endPoint, &fDistance, &vNormal, pModel[0].GetMtxWorld()) && fDistance <= 20.0f)
		{
			m_move = D3DXVECTOR3_ZERO;//止める
		}

		return true;
	}

	CDebugProc::Log("距離 : %.2f", fDistance);

	return false;
}

//=============================================================================
// 坂の処理
//=============================================================================
bool CPlayer::SlopeMove(void)
{
	bool bSlope = false;
	D3DXVECTOR3 pos = GetPosition();	// 現在の位置
	D3DXVECTOR3 posOld = GetPosOld();	// 過去の位置

	// 過去の位置のほうが高かったとき
	if (posOld.y > pos.y)
	{
		bSlope = true;
		m_fSlopeSpeed -= SLOPE_SPEED;
	}
	else if (posOld.y < pos.y)
	{// 過去位置のほうが低かったとき
		m_fSlopeSpeed += SLOPE_SPEED;
	}

	return bSlope;
}

//=============================================================================
// スピード設定
//=============================================================================
void CPlayer::SetSpeed(int nCntSpeed)
{
	switch (nCntSpeed)
	{
	case 0:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0];
		break;
	case 1:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0];
		break;
	case 2:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1];
		break;
	case 3:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2];
		break;
	case 4:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2] + m_fPuzzleSpeed[3];
		break;
	case 5:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2] + m_fPuzzleSpeed[3] + m_fPuzzleSpeed[4];
		break;
	case 6:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2] + m_fPuzzleSpeed[3] + m_fPuzzleSpeed[4] +
			m_fPuzzleSpeed[5];
		break;
	case 7:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2] + m_fPuzzleSpeed[3] + m_fPuzzleSpeed[4] +
			m_fPuzzleSpeed[5] + m_fPuzzleSpeed[6];
		break;
	case 8:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2] + m_fPuzzleSpeed[3] + m_fPuzzleSpeed[4] +
			m_fPuzzleSpeed[5] + m_fPuzzleSpeed[6] + m_fPuzzleSpeed[7];
		break;
	case 9:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2] + m_fPuzzleSpeed[3] + m_fPuzzleSpeed[4] +
			m_fPuzzleSpeed[5] + m_fPuzzleSpeed[6] + m_fPuzzleSpeed[7] + m_fPuzzleSpeed[8];
		break;
	case 10:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2] + m_fPuzzleSpeed[3] + m_fPuzzleSpeed[4] +
			m_fPuzzleSpeed[5] + m_fPuzzleSpeed[6] + m_fPuzzleSpeed[7] + m_fPuzzleSpeed[8] + m_fPuzzleSpeed[9];
		break;
	case 11:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2] + m_fPuzzleSpeed[3] + m_fPuzzleSpeed[4] +
			m_fPuzzleSpeed[5] + m_fPuzzleSpeed[6] + m_fPuzzleSpeed[7] + m_fPuzzleSpeed[8] + m_fPuzzleSpeed[9] + m_fPuzzleSpeed[10];
		break;
	case 12:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2] + m_fPuzzleSpeed[3] + m_fPuzzleSpeed[4] +
			m_fPuzzleSpeed[5] + m_fPuzzleSpeed[6] + m_fPuzzleSpeed[7] + m_fPuzzleSpeed[8] + m_fPuzzleSpeed[9] + m_fPuzzleSpeed[10] +
			m_fPuzzleSpeed[11];
		break;
	case 13:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2] + m_fPuzzleSpeed[3] + m_fPuzzleSpeed[4] +
			m_fPuzzleSpeed[5] + m_fPuzzleSpeed[6] + m_fPuzzleSpeed[7] + m_fPuzzleSpeed[8] + m_fPuzzleSpeed[9] + m_fPuzzleSpeed[10] +
			m_fPuzzleSpeed[11] + m_fPuzzleSpeed[12];
		break;
	case 14:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2] + m_fPuzzleSpeed[3] + m_fPuzzleSpeed[4] +
			m_fPuzzleSpeed[5] + m_fPuzzleSpeed[6] + m_fPuzzleSpeed[7] + m_fPuzzleSpeed[8] + m_fPuzzleSpeed[9] + m_fPuzzleSpeed[10] +
			m_fPuzzleSpeed[11] + m_fPuzzleSpeed[12] + m_fPuzzleSpeed[13];
		break;
	case 15:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2] + m_fPuzzleSpeed[3] + m_fPuzzleSpeed[4] +
			m_fPuzzleSpeed[5] + m_fPuzzleSpeed[6] + m_fPuzzleSpeed[7] + m_fPuzzleSpeed[8] + m_fPuzzleSpeed[9] + m_fPuzzleSpeed[10] +
			m_fPuzzleSpeed[11] + m_fPuzzleSpeed[12] + m_fPuzzleSpeed[13] + m_fPuzzleSpeed[14];
		break;
	case 16:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2] + m_fPuzzleSpeed[3] + m_fPuzzleSpeed[4] +
			m_fPuzzleSpeed[5] + m_fPuzzleSpeed[6] + m_fPuzzleSpeed[7] + m_fPuzzleSpeed[8] + m_fPuzzleSpeed[9] + m_fPuzzleSpeed[10] +
			m_fPuzzleSpeed[11] + m_fPuzzleSpeed[12] + m_fPuzzleSpeed[13] + m_fPuzzleSpeed[14] + m_fPuzzleSpeed[15];
		break;
	case 17:
		m_fPuzzleMaxSPeed = m_fPuzzleSpeed[0] + m_fPuzzleSpeed[1] + m_fPuzzleSpeed[2] + m_fPuzzleSpeed[3] + m_fPuzzleSpeed[4] +
			m_fPuzzleSpeed[5] + m_fPuzzleSpeed[6] + m_fPuzzleSpeed[7] + m_fPuzzleSpeed[8] + m_fPuzzleSpeed[9] + m_fPuzzleSpeed[10] +
			m_fPuzzleSpeed[11] + m_fPuzzleSpeed[12] + m_fPuzzleSpeed[13] + m_fPuzzleSpeed[14] + m_fPuzzleSpeed[15] + m_fPuzzleSpeed[16];
		break;
	}
	m_fPuzzleMaxSPeed += NORMAL_SPEED;
}

//=============================================================================
// 旋回速度設定
//=============================================================================
void CPlayer::SetTurning(int nCntTurning)
{
	switch (nCntTurning)
	{
	case 0:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0];
		break;
	case 1:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0];
		break;
	case 2:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1];
		break;
	case 3:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2];
		break;
	case 4:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2] + m_fPuzzleTurning[3];
		break;
	case 5:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2] + m_fPuzzleTurning[3] + m_fPuzzleTurning[4];
		break;
	case 6:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2] + m_fPuzzleTurning[3] + m_fPuzzleTurning[4] +
			m_fPuzzleTurning[5];
		break;
	case 7:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2] + m_fPuzzleTurning[3] + m_fPuzzleTurning[4] +
			m_fPuzzleTurning[5] + m_fPuzzleTurning[6];
		break;
	case 8:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2] + m_fPuzzleTurning[3] + m_fPuzzleTurning[4] +
			m_fPuzzleTurning[5] + m_fPuzzleTurning[6] + m_fPuzzleTurning[7];
		break;
	case 9:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2] + m_fPuzzleTurning[3] + m_fPuzzleTurning[4] +
			m_fPuzzleTurning[5] + m_fPuzzleTurning[6] + m_fPuzzleTurning[7] + m_fPuzzleTurning[8];
		break;
	case 10:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2] + m_fPuzzleTurning[3] + m_fPuzzleTurning[4] +
			m_fPuzzleTurning[5] + m_fPuzzleTurning[6] + m_fPuzzleTurning[7] + m_fPuzzleTurning[8] + m_fPuzzleTurning[9];
		break;
	case 11:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2] + m_fPuzzleTurning[3] + m_fPuzzleTurning[4] +
			m_fPuzzleTurning[5] + m_fPuzzleTurning[6] + m_fPuzzleTurning[7] + m_fPuzzleTurning[8] + m_fPuzzleTurning[9] + m_fPuzzleTurning[10];
		break;
	case 12:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2] + m_fPuzzleTurning[3] + m_fPuzzleTurning[4] +
			m_fPuzzleTurning[5] + m_fPuzzleTurning[6] + m_fPuzzleTurning[7] + m_fPuzzleTurning[8] + m_fPuzzleTurning[9] + m_fPuzzleTurning[10] +
			m_fPuzzleTurning[11];
		break;
	case 13:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2] + m_fPuzzleTurning[3] + m_fPuzzleTurning[4] +
			m_fPuzzleTurning[5] + m_fPuzzleTurning[6] + m_fPuzzleTurning[7] + m_fPuzzleTurning[8] + m_fPuzzleTurning[9] + m_fPuzzleTurning[10] +
			m_fPuzzleTurning[11] + m_fPuzzleTurning[12];
		break;
	case 14:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2] + m_fPuzzleTurning[3] + m_fPuzzleTurning[4] +
			m_fPuzzleTurning[5] + m_fPuzzleTurning[6] + m_fPuzzleTurning[7] + m_fPuzzleTurning[8] + m_fPuzzleTurning[9] + m_fPuzzleTurning[10] +
			m_fPuzzleTurning[11] + m_fPuzzleTurning[12] + m_fPuzzleTurning[13];
		break;
	case 15:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2] + m_fPuzzleTurning[3] + m_fPuzzleTurning[4] +
			m_fPuzzleTurning[5] + m_fPuzzleTurning[6] + m_fPuzzleTurning[7] + m_fPuzzleTurning[8] + m_fPuzzleTurning[9] + m_fPuzzleTurning[10] +
			m_fPuzzleTurning[11] + m_fPuzzleTurning[12] + m_fPuzzleTurning[13] + m_fPuzzleTurning[14];
		break;
	case 16:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2] + m_fPuzzleTurning[3] + m_fPuzzleTurning[4] +
			m_fPuzzleTurning[5] + m_fPuzzleTurning[6] + m_fPuzzleTurning[7] + m_fPuzzleTurning[8] + m_fPuzzleTurning[9] + m_fPuzzleTurning[10] +
			m_fPuzzleTurning[11] + m_fPuzzleTurning[12] + m_fPuzzleTurning[13] + m_fPuzzleTurning[14] + m_fPuzzleTurning[15];
		break;
	case 17:
		m_fPuzzleMaxTurning = m_fPuzzleTurning[0] + m_fPuzzleTurning[1] + m_fPuzzleTurning[2] + m_fPuzzleTurning[3] + m_fPuzzleTurning[4] +
			m_fPuzzleTurning[5] + m_fPuzzleTurning[6] + m_fPuzzleTurning[7] + m_fPuzzleTurning[8] + m_fPuzzleTurning[9] + m_fPuzzleTurning[10] +
			m_fPuzzleTurning[11] + m_fPuzzleTurning[12] + m_fPuzzleTurning[13] + m_fPuzzleTurning[14] + m_fPuzzleTurning[15] + m_fPuzzleTurning[16];
		break;
	}
	m_fPuzzleMaxTurning += NORMAL_SPEED;
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CPlayer::Debug(void)
{
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 posOld = GetPosOld();
	ImGui::Begin("System");													// 「System」ウィンドウに追加します。なければ作成します。

	if (ImGui::CollapsingHeader("player"))
	{
		ImGui::Text("pos = %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);								// プレイヤーの現在位置を表示
		ImGui::Text("posOld = %.2f, %.2f, %.2f", posOld.x, posOld.y, posOld.z);								// プレイヤーの現在位置を表示
		ImGui::Text("rot = %.2f, %.2f, %.2f", m_rot.x, m_rot.y, m_rot.z);								// プレイヤーの回転を表示
		ImGui::Text("move = %.2f, %.2f, %.2f", m_move.x, m_move.y, m_move.z);								// プレイヤーの現在位置を表示
		ImGui::Text("HP = %d", m_nLife);				// プレイヤーの体力を表示

		ImGui::Checkbox("ColliderWithWall", &m_bColliderWithWall);

		if (ImGui::Button("BOSS"))
		{
			pos = D3DXVECTOR3(5972.14f, 100.0f, 8000.62f);
			SetPosition(pos);
		}
	}

	//デバッグ処理を終了
	ImGui::End();
}
#endif