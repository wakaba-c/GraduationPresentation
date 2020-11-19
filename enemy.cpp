//=============================================================================
//
// 敵処理 [enemy.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "enemy.h"
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "scene3D.h"
#include "meshField.h"
#include "title.h"
#include "model.h"
#include "colliderSphere.h"
#include "meshCapsule.h"
#include "meshCube.h"
#include "inputKeyboard.h"
#include "player.h"
#include "colliderBox.h"
#include "life.h"
#include "fade.h"
#include "purpose.h"
#include "stage.h"
#include "house.h"
#include "phase.h"
#include "effect.h"
#include "result.h"
#include "sound.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define SCRIPT_ENEMY "data/animation/car01.txt"		// 敵配置情報のアドレス
#define INTERVAL 100								// インターバル
#define MAX_LIFE 10									// 体力の最大値

//=============================================================================
// 静的メンバ変数
//=============================================================================
PDIRECT3DTEXTURE9 CEnemy::m_pTexture = NULL;		// テクスチャ情報のポインタ
int	CEnemy::m_nKill = 0;							// 倒した数

//==============================================================================
// コンストラクタ
//==============================================================================
CEnemy::CEnemy(CScene::PRIORITY obj = CScene::PRIORITY_ENEMY) : CCharacter(obj)
{
	// 優先度の設定
	SetObjType(CScene::PRIORITY_ENEMY);

	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 回転の初期化
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動量の初期化
	m_dest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動先の初期化
	m_difference = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 差の初期化
	m_bJump = false;									// ジャンプの初期化
	m_nLife = MAX_LIFE;									// 体力の初期化
	m_pSphere = NULL;									// 当たり判定(体)の初期化
	m_pAttack = NULL;									// 当たり判定(攻撃)の初期化
	m_pBox = NULL;										// 当たり判定の初期化
	m_pLife = NULL;										// ライフバーポインタの初期化
}

//=============================================================================
// デストラクタ
//=============================================================================
CEnemy::~CEnemy()
{

}

//==============================================================================
// 初期化処理
//==============================================================================
HRESULT CEnemy::Init(void)
{
	// キャラクターの初期化処理
	CCharacter::Init();

	// アニメーションの設定
	AnimationSwitch(ANIMATIONTYPE_NEUTRAL);

	// 敵モデル情報の読み込み
	LoadScript(SCRIPT_ENEMY, ANIMATIONTYPE_MAX);

	// 位置の設定
	SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CEnemy::Uninit(void)
{
	// キャラクターの開放処理
	CCharacter::Uninit();
}

//=============================================================================
// 更新処理
//=============================================================================
void CEnemy::Update(void)
{
#ifdef _DEBUG
	// キャラクターの更新処理
	CCharacter::Update();

	// デバッグ処理
	Debug();
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CEnemy::Draw(void)
{
	if (CManager::GetMode() == CManager::MODE_GAME)
	{
		CCharacter::Draw();
	}
}

//=============================================================================
// クリエイト関数
//=============================================================================
CEnemy *CEnemy::Create(void)
{
	CEnemy *pEnemy;
	pEnemy = new CEnemy(CScene::PRIORITY_ENEMY);

	if (pEnemy != NULL)
	{// 敵が存在していたとき
		pEnemy->Init();				// 初期化処理
	}
	return pEnemy;
}

//=============================================================================
// モデルのロード関数
//=============================================================================
HRESULT CEnemy::Load(void)
{
	return S_OK;
}

//=============================================================================
// モデルの読込
//=============================================================================
void CEnemy::LoadEnemy(char *add)
{
	FILE *pFile = NULL;						// ファイル
	char cReadText[128] = {};				// 文字
	char cHeadText[128] = {};				// 比較
	float aData[3];							// 答え
	CEnemy *pEnemy = NULL;

	pFile = fopen(add, "r");				// ファイルを開くまたは作る

	if (pFile != NULL)						// ファイルが読み込めた場合
	{
		fgets(cReadText, sizeof(cReadText), pFile);			// 行を飛ばす
		fgets(cReadText, sizeof(cReadText), pFile);			// 行を飛ばす
		fgets(cReadText, sizeof(cReadText), pFile);			// 行を飛ばす

		while (strcmp(cHeadText, "End") != 0)
		{
			CManager::ConvertStringToFloat(cReadText, ",", aData);		// もらった文字列をコンマで区切り格納する

			pEnemy = CEnemy::Create();			// クリエイト処理

			if (pEnemy != NULL)
			{// 敵が存在していたとき
				pEnemy->SetPosition(D3DXVECTOR3(aData[0], aData[1], aData[2]));			// ポジションを決める
				pEnemy->SetTarget(TARGETTYPE_PLAYER);				// 攻撃対象の設定
			}

			fgets(cReadText, sizeof(cReadText), pFile);				// 行を飛ばす
			sscanf(cReadText, "%s", &cHeadText);					// 行を読み込む
		}

		fclose(pFile);				// ファイルを閉じる
	}
	else
	{
		MessageBox(NULL, "モデル情報のアクセス失敗！", "WARNING", MB_ICONWARNING);	// メッセージボックスの生成
	}
}

//=============================================================================
// 攻撃対象の設定処理
//=============================================================================
void CEnemy::SetTarget(TARGETTYPE target)
{
	m_target = target;
}

//=============================================================================
// 当たり判定 コライダー
//=============================================================================
void CEnemy::OnTriggerEnter(CCollider *col)
{
	// アニメーション情報の取得
	ANIMATIONTYPE animType = (ANIMATIONTYPE)GetAnimType();

	std::string sTag = col->GetTag();

	if (animType != ANIMATIONTYPE_DIE)
	{
		if (col->GetScene()->GetObjType() == PRIORITY_PLAYER)
		{
			if (sTag == "weapon")
			{// タグが 武器 だったとき
				if (animType != ANIMATIONTYPE_DIE)
				{
					CPlayer *pPlayer = CGame::GetPlayer();				// プレイヤーの取得
					CSound *pSound = CManager::GetSound();				// サウンドの取得

					pSound->PlaySoundA(SOUND_LABEL_SE_PUNCH);			// ダメージ音の再生

					if (m_target != TARGETTYPE_PLAYER)
					{// 攻撃対象がプレイヤー以外だったとき
						m_target = TARGETTYPE_PLAYER;				// 標的をプレイヤーに定める
					}

					if (m_pLife != NULL)
					{
						m_pLife->SetLifeBar((float)m_nLife / MAX_LIFE);
					}
					else
					{// 体力バーの生成
						m_pLife = CLife::Create();
						m_pLife->SetLifeBar((float)m_nLife / MAX_LIFE);
					}

					if (pPlayer != NULL)
					{// プレイヤーが存在していたとき
						CPlayerUi *pPlayerUi = pPlayer->GetPlayerUi();				// プレイヤーUIの取得

						if (pPlayerUi != NULL)
						{// プレイヤーUIが存在していたとき
							float fDeathblow = pPlayer->GetDeathblow();				// 現在の必殺技ポイントを取得

							if (fDeathblow < 50.0f)
							{// 必殺技ポイントが5より小さかったとき
							}
						}

						if (pPlayer->GetAnimType() == CPlayer::ANIMATIONTYPE_ATTACK_1)
						{// ノックバック処理
							D3DXVECTOR3 vec;

							vec = GetPosition() - pPlayer->GetPosition();		//差分を求める(方向を求めるため)
							D3DXVec3Normalize(&vec, &vec);			//正規化する

							m_move.x = vec.x * 10;
							m_move.z = vec.z * 10;
						}
					}

					if (m_nLife < 1)
					{// 体力が 1 を下回ったとき
						D3DXVECTOR3 vec;

						AnimationSwitch(ANIMATIONTYPE_DIE);
						m_nKill++;

						// 体の当たり判定の開放
						if (m_pSphere != NULL)
						{// 体の当たり判定が存在していたとき
							m_pSphere->Release();
							m_pSphere = NULL;
						}

						// 拳の当たり判定の開放
						if (m_pAttack != NULL)
						{// 拳の当たり判定が存在していたとき
							m_pAttack->Release();
							m_pAttack = NULL;
						}

						// 体力ゲージの開放
						if (m_pLife != NULL)
						{// 体力の開放が存在していたとき
							m_pLife->Uninit();
							m_pLife->Release();
							m_pLife = NULL;
						}

						if (pPlayer != NULL)
						{// プレイヤーが存在していたとき
							vec = GetPosition() - pPlayer->GetPosition();		//差分を求める(方向を求めるため)
						}
						D3DXVec3Normalize(&vec, &vec);			//正規化する

						m_move.x = vec.x * 25;
						m_move.z = vec.z * 25;
						m_move.y = 10;
						m_bJump = true;
					}
					else
					{
						AnimationSwitch(ANIMATIONTYPE_DAMAGE);									// ダメージモーション
					}
				}
			}
		}
	}
}

//========================================================================================
// アニメーションフレームの最大数に到達したときの処理
//========================================================================================
void CEnemy::BehaviorForMaxFrame(void)
{
}

//========================================================================================
// アニメーションキーの最大数に到達したときの処理
//========================================================================================
void CEnemy::BehaviorForMaxKey(void)
{
	// アニメーション情報の取得
	ANIMATIONTYPE animType = (ANIMATIONTYPE)GetAnimType();
	ANIMATION *pAnimation = GetAnimData();

	// ループするかどうか
	if (animType == ANIMATIONTYPE_DIE)
	{
		Release();
	}
	else if (pAnimation[animType].nLoop)
	{
		// キーのリセット
		ResetKeyAndFrame();
	}
	else
	{
		// ニュートラルモーション
		AnimationSwitch(ANIMATIONTYPE_NEUTRAL);
		// キーのリセット
		ResetKeyAndFrame();
	}
}

//=============================================================================
// 行動関数
//=============================================================================
void CEnemy::Move(D3DXVECTOR3 &pPos)
{
	CPlayer *pPlayer = CGame::GetPlayer();
	CHouse *pHouse = CGame::GetHouse();
	D3DXVECTOR3 targetPos;

	// アニメーション情報の取得
	ANIMATIONTYPE animType = (ANIMATIONTYPE)GetAnimType();

	if (m_target == TARGETTYPE_HOUSE)
	{//	現在の対象が[家]だったとき
		if (pHouse != NULL)
		{
			targetPos = pHouse->GetPosition();
		}
	}
	else if (m_target == TARGETTYPE_PLAYER)
	{// 現在の攻撃対象が[プレイヤー]だったとき
		if (pPlayer != NULL)
		{
			targetPos = pPlayer->GetPosition();
		}
	}

	D3DXVECTOR3 pos = pPos;

	float fDifference_x = pos.x - targetPos.x;
	float fDifference_z = pos.z - targetPos.z;
	float fDifference = sqrtf(fDifference_x * fDifference_x + fDifference_z * fDifference_z);

	// 敵が攻撃中かどうか
	if (animType != ANIMATIONTYPE_ATTACK && animType != ANIMATIONTYPE_DAMAGE && animType != ANIMATIONTYPE_DIE)
	{
		if (m_nInterval > INTERVAL)
		{
			if (m_target == TARGETTYPE_PLAYER)
			{
			}
			else if (m_target == TARGETTYPE_HOUSE)
			{
				// 敵が範囲内に入ってきたかどうか
				if (fDifference < 3000.0f && fDifference > 450)
				{// プレイヤーに向かって走る
					D3DXVECTOR3 nor = targetPos - pos;

					D3DXVec3Normalize(&nor, &nor);

					nor.y = 0;
					m_move = nor * 10;
					m_rot.y = atan2f(fDifference_x, fDifference_z);
				}
			}

			if (m_target == TARGETTYPE_PLAYER)
			{
				// クールタイムを終えているかどうか
				if (fDifference < 200)
				{// 指定した距離より近づいていたとき
					if (animType != ANIMATIONTYPE_ATTACK && animType != ANIMATIONTYPE_DAMAGE)
					{
						if (CManager::GetRand(10) > 8)
						{
							AnimationSwitch(ANIMATIONTYPE_ATTACK);
							m_nInterval = 0;
						}
					}
				}
			}
			else if (m_target == TARGETTYPE_HOUSE)
			{
				// クールタイムを終えているかどうか
				if (fDifference < 460)
				{// 指定した距離より近づいていたとき
					if (animType != ANIMATIONTYPE_ATTACK && animType != ANIMATIONTYPE_DAMAGE)
					{
						if (CManager::GetRand(10) > 8)
						{
							AnimationSwitch(ANIMATIONTYPE_ATTACK);
							m_nInterval = 0;
						}
					}
				}
			}
		}
		else
		{
			m_nInterval++;
		}
	}

	pPos = pos;
}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CEnemy::Collider(void)
{
	// アニメーション情報の取得
	ANIMATIONTYPE animType = (ANIMATIONTYPE)GetAnimType();
	int currentKey = GetCurrentKey();

	switch (animType)
	{
	case ANIMATIONTYPE_NEUTRAL:					// 待機モーションのとき
		if (m_pAttack != NULL)
		{// 当たり判定が存在していたとき
			if (m_pAttack->GetUse())
			{// 当たり判定の対象だったとき
				m_pAttack->SetUse(false);		// 対象から外す
			}
		}
		break;
	case ANIMATIONTYPE_ATTACK:				// 攻撃モーションのとき
		if (m_pAttack != NULL)
		{
			if (currentKey >= 1)
			{
				if (!m_pAttack->GetUse())
				{// 当たり判定の対象外だったとき
					m_pAttack->SetUse(true);		// 対象にする
				}
			}
		}
		break;
	}
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CEnemy::Debug(void)
{
	ImGui::Begin("System");													// 「System」ウィンドウに追加します。なければ作成します。
	D3DXVECTOR3 size;
	D3DXVECTOR3 pos = GetPosition();

	if (ImGui::TreeNode("enemy"))
	{//アニメーションのツリーノードを探します。なければ作成します。
		ImGui::Text("pos%d = %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);							// プレイヤーの現在位置を表示

		if (ImGui::TreeNode("collider"))
		{
			ImGui::DragFloat3("ColliderSize", (float*)&size);											// コライダーの大きさを設定

			ImGui::DragFloat3("ColliderPos", (float*)&pos);

			ImGui::TreePop();																			// 終了
		}

		ImGui::TreePop();																			// 終了
	}

	//デバッグ処理を終了
	ImGui::End();
}
#endif