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

//=============================================================================
// マクロ定義
//=============================================================================
//#define	SCRIPT_PLAYER "data/animation/girl.txt"		// 赤ずきんのモデル情報アドレス
#define	SCRIPT_CAR01 "data/animation/car01.txt"		// 車01のモデル情報アドレス
#define COUNTER_COMBO 30							// 派生攻撃受付カウンタ
#define JUMP_MAX 10									// ジャンプの加速度
#define ROT_AMOUNT 0.05f							// 回転の差を減らしていく量
#define ROT_SPEED 0.3f								// 回転速度

//=============================================================================
// コンストラクタ
//=============================================================================
CPlayer::CPlayer(CScene::PRIORITY obj = CScene::PRIORITY_PLAYER) : CCharacter(obj)
{
	// 優先度の設定
	SetObjType(CScene::PRIORITY_PLAYER);				// オブジェクトタイプ

	m_nLife = 100;										// 体力の初期化
	m_fSpeed = NORMAL_SPEED;							// スピードの初期化
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 回転の初期化
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動量の初期化
	m_dest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動先の初期化
	m_difference = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 差の初期化
	m_pColPlayerSphere = NULL;							// プレイヤー当たり判定ポインタの初期化
	m_pColWeaponSphere = NULL;							// 刃当たり判定ポインタの初期化
	m_pColHandSphere = NULL;							// 手の当たり判定の初期化
	m_pBox = NULL;
	m_bJump = false;									// ジャンプフラグの初期化
	m_nCntAttacCombo = COUNTER_COMBO;					// 攻撃派生カウンタの初期化
	m_nActionCount = 0;									// アクションカウンタの初期化
	m_nParticleCount = 0;								// パーティクルカウンタの初期化
	m_fDeathblow = 0.0f;								// 必殺技ポイントの初期化
	m_bEvent = false;									// イベント発生フラグの初期化

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
	D3DXVECTOR3 pos = GetPosition();						//プレイヤーの位置取得

	pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// プレイヤーの位置設定

	if (pCamera != NULL)
	{
		pCamera->SetPosCamera(pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	}

	// キャラクターの初期化処理
	CCharacter::Init();

	// アニメーションの設定
	AnimationSwitch(ANIMATIONTYPE_NONE);

	// プレイヤーモデル情報の読み込み
	LoadScript(SCRIPT_CAR01, ANIMATIONTYPE_MAX);

	// 武器の当たり判定を生成
	m_pColWeaponSphere = CColliderSphere::Create(true, 30.0f);

	if (m_pColWeaponSphere != NULL)
	{ // 球体のポインタがNULLではないとき
		m_pColWeaponSphere->SetScene(this);											// 呼び出し主 の設定
		m_pColWeaponSphere->SetTag("weapon");										// タグ の設定
	}

	// 武器の当たり判定を生成
	m_pColHandSphere = CColliderSphere::Create(true, 50.0f);

	if (m_pColHandSphere != NULL)
	{ // 球体のポインタがNULLではないとき
		m_pColHandSphere->SetScene(this);											// 呼び出し主 の設定
		m_pColHandSphere->SetTag("weapon");											// タグ の設定
	}

	// プレイヤーの当たり判定を生成
	m_pColPlayerSphere = CColliderSphere::Create(false, 20.0f);

	if (m_pColPlayerSphere != NULL)
	{ //球体のポインタがNULLではないとき
		m_pColPlayerSphere->SetScene(this);
		m_pColPlayerSphere->SetTag("player");										// タグ の設定
		m_pColPlayerSphere->SetPosition(pos);										// 位置 の設定
		m_pColPlayerSphere->SetOffset(D3DXVECTOR3(0.0f, 100.0f, 0.0f));
	}

	// 位置の設定
	SetPosition(pos);

	// 軌跡の生成
	m_pMeshOrbit = CMeshOrbit::Create();

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CPlayer::Uninit(void)
{
	if (m_pColWeaponSphere != NULL)
	{// 武器の当たり判定が存在していたとき
		if (m_pColWeaponSphere != NULL)
		{
			m_pColWeaponSphere->Release();
		}
	}

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
		Input();
	}

	pos = GetPosition();				// 位置の取得

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

	// 当たり判定管理処理
	Collision();

	// 位置更新
	pos += m_move;

	if (!m_bJump)
	{
		// 減速
		m_move.x += (0 - m_move.x) * 0.12f;
		m_move.z += (0 - m_move.z) * 0.12f;
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

	//球体のポインタがNULLではないとき
	if (m_pColWeaponSphere != NULL)
	{
		D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 40.0f, -80.0f);
		D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXMATRIX	mtxRot, mtxTrans, mtxView, mtxMeshRot, mtxMeshTrans;				//計算用マトリックス
		D3DXMATRIX mtx;			// 武器のマトリックス

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtx);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, rot.y, rot.x, rot.z);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxMeshTrans, pos.x, pos.y, pos.z);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshTrans);

		D3DXMatrixMultiply(&mtx, &mtx, &pModel[14].GetMtxWorld());

		m_pColWeaponSphere->SetPosition(D3DXVECTOR3(mtx._41, mtx._42, mtx._43));
	}

	if (m_pColHandSphere != NULL)
	{// 手の当たり判定が存在していたとき
		D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, -50.0f, 0.0f);
		D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXMATRIX	mtxRot, mtxTrans, mtxView, mtxMeshRot, mtxMeshTrans;				//計算用マトリックス
		D3DXMATRIX mtx;			// 武器のマトリックス

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtx);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, rot.y, rot.x, rot.z);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxMeshTrans, pos.x, pos.y, pos.z);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshTrans);

		D3DXMatrixMultiply(&mtx, &mtx, &pModel[4].GetMtxWorld());

		m_pColHandSphere->SetPosition(D3DXVECTOR3(mtx._41, mtx._42, mtx._43));
	}

	//球体のポインタがNULLではないとき
	if (m_pColPlayerSphere != NULL)
	{
		m_pColPlayerSphere->SetPosition(pos);		// 現在位置 の更新
	}

	// 位置設定
	SetPosition(pos);

	if (m_pBox != NULL)
	{
		m_pBox->SetPosition(pos);
	}

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
	ANIMATIONTYPE animType = (ANIMATIONTYPE)GetAnimType();

	switch (animType)
	{
	case ANIMATIONTYPE_ATTACK_1:				// 攻撃モーションのとき
		if (m_pColWeaponSphere != NULL)
		{// 武器の当たり判定が存在していたとき
			if (!m_pColWeaponSphere->GetUse())
			{// 当たり判定の対象外だったとき
				m_pColWeaponSphere->SetUse(true);		// 対象にする
			}
		}
		if (m_pMeshOrbit != NULL)
		{// 軌跡が存在していたとき
			if (!m_pMeshOrbit->GetOrbit())
			{
				m_pMeshOrbit->SetOrbit(true);
			}
		}
		break;
	}
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

	// サウンドの取得
	CSound *pSound = CManager::GetSound();

	D3DXVECTOR3 rot = pCamera->GetRotation();
	D3DXVECTOR3 nor;
	D3DXVECTOR3 Diff;	// 計算用格納変数
	float nValueH = 0;									//コントローラー
	float nValueV = 0;									//コントローラー

	if (m_nCntAttacCombo <= COUNTER_COMBO)
	{// 攻撃派生受付カウンタが15カウント未満だったとき
		m_nCntAttacCombo++;				// カウンタに1プラス
	}

	// ====================== コントローラー ====================== //

	// 攻撃
	{
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
			//左右操作
			if (pKeyboard->GetPressKeyboard(MOVE_LEFT))
			{
				D3DXVec3Normalize(&nor, &D3DXVECTOR3(m_move.z, m_move.y, -m_move.x));
				m_dest.y = m_rot.y - ROT_SPEED;
			}
			else if (pKeyboard->GetPressKeyboard(MOVE_RIGHT))
			{
				D3DXVec3Normalize(&nor, &D3DXVECTOR3(m_move.z, m_move.y, -m_move.x));
				m_dest.y = m_rot.y + ROT_SPEED;
			}
			else
			{
				m_move.x += sinf(D3DX_PI * 1.0f + rot.y) * m_fSpeed;
				m_move.z += cosf(D3DX_PI * 1.0f + rot.y) * m_fSpeed;

				D3DXVec3Normalize(&nor, &D3DXVECTOR3(m_move.z, m_move.y, -m_move.x));
			}
		}
		else if (pKeyboard->GetPressKeyboard(MOVE_BRAKE))
		{
			//左右操作
			if (pKeyboard->GetPressKeyboard(MOVE_LEFT))
			{
				D3DXVec3Normalize(&nor, &D3DXVECTOR3(m_move.z, m_move.y, -m_move.x));
				m_dest.y = m_rot.y + ROT_SPEED;
			}
			else if (pKeyboard->GetPressKeyboard(MOVE_RIGHT))
			{
				D3DXVec3Normalize(&nor, &D3DXVECTOR3(m_move.z, m_move.y, -m_move.x));
				m_dest.y = m_rot.y - ROT_SPEED;
			}
			else
			{
				m_move.x += sinf(D3DX_PI * 0.0f + rot.y) * m_fSpeed;
				m_move.z += cosf(D3DX_PI * 0.0f + rot.y) * m_fSpeed;

				D3DXVec3Normalize(&nor, &D3DXVECTOR3(m_move.z, m_move.y, -m_move.x));
			}
		}

		////左右操作
		//if (pKeyboard->GetPressKeyboard(MOVE_LEFT))
		//{
		//	//m_move.x += sinf(D3DX_PI * 0.5f + rot.y) * m_fSpeed;
		//	//m_move.z += cosf(D3DX_PI * 0.5f + rot.y) * m_fSpeed;

		//	D3DXVec3Normalize(&nor, &D3DXVECTOR3(m_move.z, m_move.y, -m_move.x));
		//	m_dest.y = m_rot.y - ROT_SPEED;
		//}
		//else if (pKeyboard->GetPressKeyboard(MOVE_RIGHT))
		//{
		//	//m_move.x += sinf(-D3DX_PI * 0.5f + rot.y) * m_fSpeed;
		//	//m_move.z += cosf(-D3DX_PI * 0.5f + rot.y) * m_fSpeed;

		//	D3DXVec3Normalize(&nor, &D3DXVECTOR3(m_move.z, m_move.y, -m_move.x));
		//	m_dest.y = m_rot.y + ROT_SPEED;
		//}

		// 左右ボタンを押しているとき
		if (pKeyboard->GetPressKeyboard(MOVE_LEFT) || pKeyboard->GetPressKeyboard(MOVE_RIGHT))
		{
			// プレイヤーの回転と目標地点の差を格納
			Diff.y = m_rot.y - m_dest.y;

			// 回転の補正
			CTakaseiLibrary::RotRevision(&Diff);

			// プレイヤーを徐々に回転させていく
			m_rot.y -= Diff.y * ROT_AMOUNT;

			// 回転の補正
			CTakaseiLibrary::RotRevision(&m_rot);

			// 回転の設定
			SetRotation(m_rot);
		}
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