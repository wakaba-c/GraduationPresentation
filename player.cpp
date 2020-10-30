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
#include "gauge2D.h"
#include "messageWindow.h"
#include "result.h"
#include "fade.h"
#include "takaseiLibrary.h"
#include "sceneX.h"
#include "speed.h"
#include "wall.h"
#include "object.h"

//=============================================================================
// マクロ定義
//=============================================================================
//#define	SCRIPT_PLAYER "data/animation/girl.txt"		// 赤ずきんのモデル情報アドレス
#define	SCRIPT_CAR01 "data/animation/car01.txt"		// 車01のモデル情報アドレス
#define COUNTER_COMBO 30							// 派生攻撃受付カウンタ
#define JUMP_MAX 10									// ジャンプの加速度
#define ROT_AMOUNT 0.1f								// 回転の差を減らしていく量
#define ROT_SPEED 0.2f								// 回転速度
#define ROT_SPEED_DRIFT 0.5f						// ドリフト時回転速度
#define MODEL_FRONT 2								// モデル前輪
#define MODEL_TIRE 2								// タイヤモデルの数
#define SPEED_DOWN 0.06f							// スピード減少

//=============================================================================
// コンストラクタ
//=============================================================================
CPlayer::CPlayer(CScene::PRIORITY obj = CScene::PRIORITY_PLAYER) : CCharacter(obj)
{
	// 優先度の設定
	SetObjType(CScene::PRIORITY_PLAYER);				// オブジェクトタイプ

	m_nLife = 100;										// 体力の初期化
	m_fSpeed = NORMAL_SPEED;							// スピードの初期化
	m_rot = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);			// 回転の初期化
	m_fSpeed = 0;;										// スピードの初期化
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動量の初期化
	m_dest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動先の初期化
	m_difference = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 差の初期化
	m_cameraRot = D3DXVECTOR3(0, D3DX_PI, 0);			// カメラの回転情報初期化
	m_pColPlayerSphere = NULL;							// プレイヤー当たり判定ポインタの初期化
	m_bHit = false;										// 当たり判定フラグの初期亜化
	m_bJump = false;									// ジャンプフラグの初期化
	m_nActionCount = 0;									// アクションカウンタの初期化
	m_nParticleCount = 0;								// パーティクルカウンタの初期化
	m_fDeathblow = 0.0f;								// 必殺技ポイントの初期化
	m_bEvent = false;									// イベント発生フラグの初期化
	m_bDrift = false;									// ドリフトフラグ判定
	m_bMove = false;									// 現在動いているかのフラグ

	m_pPlayerUi = NULL;
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

	//デバイスを取得する
	pDevice = pRenderer->GetDevice();

	CCamera *pCamera = CManager::GetCamera();
	D3DXVECTOR3 pos = GetPosition();				// プレイヤーの位置取得

	pos = D3DXVECTOR3(0.0f, 50.0f, 0.0f);			// プレイヤーの位置設定

	if (pCamera != NULL)
	{
		pCamera->SetPosCamera(pos, D3DXVECTOR3(0.0f, D3DX_PI, 0.0f));
	}

	// キャラクターの初期化処理
	CCharacter::Init();

	// アニメーションの設定
	AnimationSwitch(ANIMATIONTYPE_NONE);

	// プレイヤーモデル情報の読み込み
	LoadScript(SCRIPT_CAR01, ANIMATIONTYPE_MAX);

	// プレイヤーの当たり判定を生成
	m_pColPlayerSphere = CColliderSphere::Create(false, 20.0f);

	if (m_pColPlayerSphere != NULL)
	{ //球体のポインタがNULLではないとき
		m_pColPlayerSphere->SetScene(this);
		m_pColPlayerSphere->SetTag("player");										// タグ の設定
		m_pColPlayerSphere->SetPosition(pos);										// 位置 の設定
		m_pColPlayerSphere->SetOffset(D3DXVECTOR3(0.0f, 20.0f, 0.0f));
	}

	// 位置の設定
	SetPosition(pos);

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

	CCharacter::Uninit();
}

//=============================================================================
// 更新処理
//=============================================================================
void CPlayer::Update(void)
{
	D3DXVECTOR3 pos;
	CSound *pSound = CManager::GetSound();
	float fHeight = 0.0f;
	CModel *pModel = GetModel();

	// アニメーション情報の取得
	ANIMATIONTYPE animType = (ANIMATIONTYPE)GetAnimType();
	int currentKey = GetCurrentKey();
	int currentFrame = GetCurrentFrame();
	bool bAnimPlayState = GetAnimPlayState();

	if (!m_bEvent)
	{// イベントが発生していなかったとき
		// 入力処理
		if (!m_bHit)
		{// 当たっていなかったとき
			// 入力処理
			Input();
		}
	}

	pos = GetPosition();				// 位置の取得

	D3DXVECTOR3 pointLeft, pointRight;
	float fLeftLength, fRightLength;

	CCollider::RayBlockCollision(pos, &pModel[0].GetMtxWorld(), 12.28f, 30.0f);
	pointRight = CCollider::RayRightWallCollision(fRightLength, pos, m_rot, m_move, &pModel[0].GetMtxWorld());
	pointLeft = CCollider::RayLeftWallCollision(fLeftLength, pos, m_rot, m_move, &pModel[0].GetMtxWorld());

	//if (m_pColPlayerSphere != NULL)
	//{
	//	D3DXVECTOR3 center = (pointLeft + pointRight) * 0.5f;
	//	m_pColPlayerSphere->SetPosition(center);

	//	float fLength, playerLength;
	//	fLength = (pointLeft.x - pointRight.x) * (pointLeft.x - pointRight.x) + (pointLeft.z - pointRight.z) * (pointLeft.z - pointRight.z);

	//	playerLength = (center.x - pos.x) * (center.x - pos.x) + (center.z - pos.z) * (center.z - pos.z);

	//	if (fLength * 0.25f < playerLength)
	//	{
	//		D3DXVECTOR3 save = pos - D3DXVECTOR3(center.x, pos.y, center.z);
	//		D3DXVECTOR3 vec;
	//		D3DXVec3Normalize(&vec, &save);			//正規化する

	//		float y = pos.y;

	//		// 食い込んだ分を求める
	//		pos = vec * ((fRightLength + fLeftLength) * 0.5f);
	//		pos.y = y;

	//		// 食い込んだ分だけ戻す
	//		SetPosition(pos);
	//	}

	//	m_pColPlayerSphere->SetRadius((fRightLength + fLeftLength) * 0.5f);
	//	CDebugProc::Log("距離 : %.2f", playerLength);
	//}

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
						pSound->PlaySoundA((SOUND_LABEL)(CManager::GetRand(3) + (int)SOUND_LABEL_SE_GRASS_1));
					}
					else if (Type == RANDTYPE_SAND)
					{
						pSound->PlaySoundA((SOUND_LABEL)(CManager::GetRand(3) + (int)SOUND_LABEL_SE_SAND_1));
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
		m_move.x += (0 - m_move.x) * SPEED_DOWN;
		m_move.z += (0 - m_move.z) * SPEED_DOWN;
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

	// 当たり判定管理処理
	Collision();

	//	D3DXVECTOR3 move = CManager::Slip(playerPos + m_move, vNormal);// 滑りベクトルを計算


	// キャラクターの更新処理
	CCharacter::Update();

#ifdef _DEBUG
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
// 当たり判定(trigger)
//=============================================================================
void CPlayer::OnTriggerEnter(CCollider *col)
{
	std::string sTag = col->GetTag();
	CModel *pModel = GetModel();

	if (col->GetScene()->GetObjType() == PRIORITY_ENEMY)
	{
		if (sTag == "weapon")
		{
			CSound *pSound = CManager::GetSound();				// サウンドの取得
			pSound->PlaySoundA(SOUND_LABEL_SE_PUNCH);			// ダメージ音の再生
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
				CFade::SetFade(CManager::MODE_RESULT);					// リザルトに遷移
			}
		}
	}
	if (sTag == "house")
	{

	}
	if (sTag == "goal")
	{
		if (CFade::GetFade() == CFade::FADE_NONE)
		{//フェードが処理をしていないとき
			CFade::SetFade(CManager::MODE_PUZZLE_CUSTOM);					//フェードを入れる
		}
	}
}

//=============================================================================
// 当たり判定(collider)
//=============================================================================
void CPlayer::OnCollisionEnter(CCollider *col)
{
	std::string sTag = col->GetTag();
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
	//// 壁の当たり判定
	//m_bHit = CollisionWall();
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

	// ====================== コントローラー ====================== //

	if (pGamepad != NULL)
	{// ゲームパッドが存在したとき
		if (pGamepad->GetJoypadUse(0))
		{// 使用可能だったとき
			pGamepad->GetJoypadStickLeft(0, &nValueH, &nValueV);

			//移動
			m_move += D3DXVECTOR3(sinf(D3DX_PI * 1.0f + rot.y) * (m_fSpeed * nValueV), 0, cosf(D3DX_PI * 1.0f + rot.y) * (m_fSpeed * nValueV));
			m_move += D3DXVECTOR3(sinf(D3DX_PI * 0.5f + rot.y) * (m_fSpeed * nValueH), 0, cosf(D3DX_PI * 0.5f + rot.y) * (m_fSpeed * nValueH));

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

			CEffect::SetEffect(EFFECTTYPE_ROSE,										// パーティクルのタイプ
				GetPosition(),											// 発生位置
				D3DXVECTOR3(8.0f, 8.0f, 0.0f),							// サイズ
				particlePos * 5.0f,										// 方向ベクトル
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),
				EASINGTYPE_NONE,
				rot,													// テクスチャの回転
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),						// カラー
				200,													// パーティクルの生存カウント数
				true,													// 重力
				0,														// 抵抗
				true,													// ビルボード
				0,														// 表示する箇所(横)
				0,														// 表示する箇所(縦)
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),
				0,
				0,
				0);
		}
	}

	// モデルがあるとき
	if (pModel != NULL)
	{
		D3DXVECTOR3 modelFrontDiff = D3DXVECTOR3(0, 0, 0);			// モデル前輪計算用マトリックス
		D3DXVECTOR3 aVec = D3DXVECTOR3(0, 0, 0);					// プレイヤー加速度ベクトル
		D3DXVECTOR3 cameraVec = D3DXVECTOR3(0, 0, 0);				// カメラの方向ベクトル
		D3DXVECTOR3 moveVec = D3DXVECTOR3(0, 0, 0);					// プレイヤー運動ベクトル
		D3DXVECTOR3 fModelRot = pModel[MODEL_FRONT].GetRotation();	// モデル前輪回転情報
		float acceleration = 0.0f;									// 加速度
		float fRadius = 100.0f;										// 回転半径
		float fDigit = CSpeed::GetDigit();							// 時速取得
		m_fSpeed = 0;

		//if (pKeyboard->GetTriggerKeyboard(DIK_SPACE))
		//{// スペースキーが押されたとき
		//	if (!m_bJump)
		//	{
		//		AnimationSwitch(ANIMATIONTYPE_JUMP_1);		// ジャンプモーションに切り替え
		//		m_bJump = true;			// ジャンプしている
		//	}
		//}

		//上下操作
		if (pKeyboard->GetPressKeyboard(MOVE_ACCEL))
		{
			// 前輪モデルの最終目的座標
			m_dest.y = 0.0f;

			// 速度設定
			m_fSpeed = -NORMAL_SPEED;

			// 動いていい
			m_bMove = true;
		}
		else if (pKeyboard->GetPressKeyboard(MOVE_BRAKE))
		{
			// 前輪モデルの最終目的座標
			m_dest.y = 0.0f;

			// 速度設定
			m_fSpeed = NORMAL_SPEED;

			// 動いていい
			m_bMove = true;
		}

		//左右操作
		if (pKeyboard->GetPressKeyboard(MOVE_LEFT))
		{
			// 前輪モデルの最終目的座標
			m_dest.y = -ROT_SPEED;
		}
		else if (pKeyboard->GetPressKeyboard(MOVE_RIGHT))
		{
			// 前輪モデルの最終目的座標
			m_dest.y = ROT_SPEED;
		}

		// ドリフトしていないとき
		if (!m_bDrift)
		{
			// ドリフトボタンを押したとき
			if (pKeyboard->GetPressKeyboard(MOVE_DRIFT))
			{
				//左右操作
				if (pKeyboard->GetPressKeyboard(MOVE_LEFT))
				{
					// ドリフトしている状態にする
					m_bDrift = true;
				}
				else if (pKeyboard->GetPressKeyboard(MOVE_RIGHT))
				{
					// ドリフトしている状態にする
					m_bDrift = true;
				}
			}
		}
		else
		{// ドリフトしているとき
			//左右操作
			if (pKeyboard->GetPressKeyboard(MOVE_LEFT))
			{
				// 回転半径変更
				fRadius = 1000.0f;
			}
			else if (pKeyboard->GetPressKeyboard(MOVE_RIGHT))
			{
				// 回転半径変更
				fRadius = 1000.0f;
			}

			// 加速度計算
			acceleration = CTakaseiLibrary::OutputAcceleration(fDigit / 2, fRadius);


			// 加速度ベクトル設定
			aVec.x += sinf(m_rot.y - D3DX_PI / 2) * acceleration;
			aVec.z += cosf(m_rot.y - D3DX_PI / 2) * acceleration;

#ifdef _DEBUG
			CDebugProc::Log("加速度：%f\n", acceleration);
			CDebugProc::Log("加速度ベクトル：%f, %f, %f\n", aVec.x, aVec.y, aVec.z);
#endif

			// ドリフトボタンを離したとき
			if (!pKeyboard->GetPressKeyboard(MOVE_DRIFT))
			{
				// ドリフトしていない状態にする
				m_bDrift = false;
			}
		}

		{// 前輪設定
			// モデルの回転と目標地点の差を格納
			modelFrontDiff.y = fModelRot.y - m_dest.y;

			// 回転の補正
			CTakaseiLibrary::RotRevision(&modelFrontDiff);

			// モデルを徐々に回転させていく
			fModelRot.y -= modelFrontDiff.y * ROT_AMOUNT;

			// 回転の補正
			CTakaseiLibrary::RotRevision(&fModelRot);

			// モデルの回転の設定
			pModel[MODEL_FRONT].SetRotation(fModelRot);
		}

		{// カメラ設定
			// タイヤの回転の半分を差と格納
			cameraVec.y = m_cameraRot.y - (m_rot.y + (m_dest.y / 2));

			// 回転の補正
			CTakaseiLibrary::RotRevision(&cameraVec);

			// カメラを徐々に回転させていく
			m_cameraRot.y -= cameraVec.y * ROT_AMOUNT;

			// 回転の補正
			CTakaseiLibrary::RotRevision(&m_cameraRot);

			// カメラ回転設定
			SetCameraRot(m_cameraRot);
		}

		// 移動していいとき
		if (m_bMove)
		{
			// 運動ベクトル計算
			moveVec.x += sinf(m_rot.y + m_dest.y) * m_fSpeed;
			moveVec.z += cosf(m_rot.y + m_dest.y) * m_fSpeed;

			//// 運動ベクトル計算
			//m_move.x += sinf(m_rot.y + m_dest.y) * m_fSpeed;
			//m_move.z += cosf(m_rot.y + m_dest.y) * m_fSpeed;

			// 回転の補正
			//CTakaseiLibrary::RotRevision(&m_dest);

			// プレイヤーを徐々に回転させていく
			m_rot.y += m_dest.y * ROT_AMOUNT;

#ifdef _DEBUG
			CDebugProc::Log("運動ベクトル：%f, %f, %f\n", moveVec.x, moveVec.y, moveVec.z);
#endif
		}

		// プレイヤーが動いていないとき
		if (fabs(m_move.x) <= 2 && fabs(m_move.z) <= 2)
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

		//CDebugProc::Log("プレイヤー回転情報：%f\n", m_rot.y);
		//CDebugProc::Log("スピード：%f\n", m_fSpeed);
		//CDebugProc::Log("移動量：%f, %f, %f\n", m_move.x, m_move.y, m_move.z);
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

		if (pMeshWall->GetWallHit(this, normal))
		{
			D3DXVECTOR3 playerPos = GetPosition();
			//playerPos.y += 5.0f;

			OutputDebugString("当たった");
			// //当たり状態なので、滑らせる
			D3DXVECTOR3 move;
			//move = CManager::Slip(playerPos + m_move, normal);// 滑りベクトルを計算
			//D3DXVec3Normalize(&move, &move);
			//move *= m_fSpeed;
			//SetPosition(GetPosition() + move);

			//CManager::calcWallScratchVector(&move, m_move, normal);
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
		ImGui::Text("move = %.2f, %.2f, %.2f", m_move.x, m_move.y, m_move.z);								// プレイヤーの現在位置を表示
		ImGui::Text("HP = %d", m_nLife);				// プレイヤーの体力を表示

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