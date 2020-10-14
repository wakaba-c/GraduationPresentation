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
#include "enemyUi.h"
#include "effect.h"
#include "messageWindow.h"
#include "result.h"
#include "sound.h"
#include "gauge2D.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define SCRIPT_ENEMY "data/animation/wolf.txt"		// 敵配置情報のアドレス
#define INTERVAL 100								// インターバル
#define MAX_LIFE 10									// 体力の最大値

//=============================================================================
// 静的メンバ変数
//=============================================================================
PDIRECT3DTEXTURE9 CEnemy::m_pTexture = NULL;		// テクスチャ情報のポインタ
LPD3DXMESH CEnemy::m_pMesh = NULL;					// メッシュ情報
DWORD CEnemy::m_nNumMat = NULL;						// マテリアル情報
LPD3DXBUFFER CEnemy::m_pBuffMat = NULL;				// バッファ
int	CEnemy::m_nKill = 0;							// 倒した数

//==============================================================================
// コンストラクタ
//==============================================================================
CEnemy::CEnemy(CScene::PRIORITY obj = CScene::PRIORITY_ENEMY) : CScene(obj)
{
	// 優先度の設定
	SetObjType(CScene::PRIORITY_ENEMY);

	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 回転の初期化
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動量の初期化
	m_dest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動先の初期化
	m_difference = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 差の初期化
	m_nNumParts = 0;									// パーツ総数の初期化
	m_bAnimSwitch = true;								// アニメーションスイッチの初期化
	m_bJump = false;									// ジャンプの初期化
	m_nCurrentFrame = 0;								// 現在のフレーム数の初期化
	m_nCurrentKey = 0;									// 現在のキー数の初期化
	m_nLife = MAX_LIFE;									// 体力の初期化
	m_pSphere = NULL;									// 当たり判定(体)の初期化
	m_pAttack = NULL;									// 当たり判定(攻撃)の初期化
	m_apCurrentPos = NULL;								// 1フレームあたりの位置の移動量
	m_apCurrentRot = NULL;								// 1フレームあたりの回転の移動量
	m_pModel = NULL;									// モデルのポインタを初期化
	m_pBox = NULL;										// 当たり判定の初期化
	m_nInterval = 0;									// インターバルの初期化
	m_AnimationType = ANIMATIONTYPE_NONE;				// アニメーションの初期化

	// アニメーション情報の初期化
	for (int nCount = 0; nCount < ANIMATIONTYPE_MAX; nCount++)
	{
		m_pAnimation[nCount].apKeyInfo = NULL;
	}

	m_apCurrentPos = NULL;								// 現在位置の初期化
	m_apCurrentRot = NULL;								// 現在回転の初期化
	m_pModel = NULL;									// モデルポインタの初期化
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
	m_AnimationType = ANIMATIONTYPE_NEUTRAL;			// アニメーションタイプ の初期化

	// 敵モデル情報の読み込み
	LoadScript();

	//球体の生成
	m_pSphere = CColliderSphere::Create(false, 70.0f);

	//球体のポインタがNULLではないとき
	if (m_pSphere != NULL)
	{
		m_pSphere->SetScene(this);
		m_pSphere->SetTag("enemy");
		m_pSphere->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		m_pSphere->SetOffset(D3DXVECTOR3(0.0f, 80.0f, 0.0f));
		m_pSphere->SetMoving(false);
	}

	m_pAttack = CColliderSphere::Create(true, 50.0f);

	//球体のポインタがNULLではないとき
	if (m_pAttack != NULL)
	{
		m_pAttack->SetScene(this);
		m_pAttack->SetUse(false);
		m_pAttack->SetTag("weapon");
		m_pAttack->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		m_pAttack->SetOffset(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		m_pAttack->SetMoving(false);
	}

	SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CEnemy::Uninit(void)
{
	if (m_apCurrentPos != NULL)
	{// パーツの1フレームごとの移動量を格納する変数がNULLじゃなかったとき
		delete[] m_apCurrentPos;
		m_apCurrentPos = NULL;
	}

	if (m_apCurrentRot != NULL)
	{// パーツの1フレームごとの回転量を格納する変数がNULLじゃなかったとき
		delete[] m_apCurrentRot;
		m_apCurrentRot = NULL;
	}

	if (m_pModel != NULL)
	{//パーツが存在する場合
		for (int nCount = 0; nCount < m_nNumParts; nCount++)
		{
			m_pModel[nCount].Uninit();								// 開放処理
		}

		delete[] m_pModel;											// パーツの総数分のメモリを開放する
		m_pModel = NULL;											// パーツのアドレスにNULLを入れる
	}

	//アニメーション分回す
	for (int nCount = 0; nCount < ANIMATIONTYPE_MAX; nCount++)
	{
		if (m_pAnimation[nCount].apKeyInfo != NULL)
		{//NULLではないとき
			delete[] m_pAnimation[nCount].apKeyInfo;										//パーツの総数分メモリを取得する
			m_pAnimation[nCount].apKeyInfo = NULL;											//パーツ分のアドレスにNULLを入れる
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CEnemy::Update(void)
{
	float fHeight = 0.0f;
	D3DXVECTOR3 pos = GetPosition();		// 位置の取得
	CScene *pScene = NowFloor(pos);			// 現在いるフィールドを取得

	if (pScene != NULL)
	{// 今立っている床が存在したとき
		CMeshField *pFloor = (CMeshField*)pScene;		// キャスト
		fHeight = pFloor->GetHeight(pos);				// 床の高さを求める
	}

	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();		// キーボードの取得

	if (m_pSphere != NULL)
	{//球体のポインタがNULLではないとき
		m_pSphere->SetPosition(pos);				// 位置の設定
	}

	if (m_pAttack != NULL)
	{
		D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXMATRIX	mtxRot, mtxTrans, mtxView, mtxMeshRot, mtxMeshTrans;		//計算用マトリックス
		D3DXMATRIX mtx;				// 武器のマトリックス

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtx);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, rot.y, rot.x, rot.z);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxMeshTrans, pos.x, pos.y, pos.z);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshTrans);

		// マトリックスの合成
		D3DXMatrixMultiply(&mtx, &mtx, &m_pModel[7].GetMtxWorld());

		m_pAttack->SetPosition(D3DXVECTOR3(mtx._41, mtx._42, mtx._43));			// 位置の設定
	}

	if (m_pBox != NULL)
	{// ボックスコライダーが存在していたとき
		m_pBox->SetPosition(pos);				// 位置の設定
	}

	if (pKeyboard->GetTriggerKeyboard(DIK_SPACE))
	{// スペースキーが押されたとき
		if (!m_bJump)
		{
			m_move = D3DXVECTOR3(0.0f, 5.0f, 0.0f);
			m_bJump = true;
		}
	}

	if (!m_bJump)
	{// ジャンプしていないとき
		// 行動処理
		Move(pos);
	}

	if (m_bAnimSwitch)
	{// モーションを再現するかどうか
		// アニメーション更新
		Animation();
	}

	// 当たり判定の更新
	Collider();

	// 位置更新
	pos += m_move;

	// 減速
	m_move.x += (0 - m_move.x) * 0.12f;
	m_move.z += (0 - m_move.z) * 0.12f;

	//重力処理
	if (m_bJump)
	{
		if (pos.y > fHeight)
		{// 自分が地面の高さより高かったとき
			m_move.y += -0.7f;					// 移動量に加算する
		}
		else
		{
			pos.y = fHeight;					// 床の高さを求める
			m_move.y = 0.0f;					// 移動量を0にする
			if (m_bJump)
			{// ジャンプしていたら
				m_bJump = false;				// ジャンプの有無を変更
			}
		}
	}
	else
	{
		pos.y = fHeight;						// 床の高さを求める
	}

	if (m_pLife != NULL)
	{// 体力ゲージが存在していたとき
		if (m_AnimationType == ANIMATIONTYPE_ATTACK || m_AnimationType == ANIMATIONTYPE_RUN)
		{// アニメーションが攻撃と走っているとき
			m_pLife->SetPosition(D3DXVECTOR3(pos.x, pos.y + 200.0f, pos.z));			// ライフバーの表示位置を変更
		}
		else
		{
			m_pLife->SetPosition(D3DXVECTOR3(pos.x, pos.y + 150.0f, pos.z));			// ライフバーの表示位置を変更
		}
	}

	if (m_AnimationType == ANIMATIONTYPE_DIE)
	{
		D3DXVECTOR3 pos = GetPosition();				// 位置の取得
		CEffect::Purification(pos);						// エフェクトの発生
	}

	SetPosition(pos);		// 位置情報の更新

#ifdef _DEBUG
	if (pKeyboard->GetTriggerKeyboard(DIK_0))
	{// 0が押されたとき
		AnimationSwitch(ANIMATIONTYPE_ATTACK);			// アニメーションの変更
	}
	if (pKeyboard->GetTriggerKeyboard(DIK_5))
	{// 5が押されたとき
		AnimationSwitch(ANIMATIONTYPE_DIE);				// アニメーションの変更
	}

	// デバッグ処理
	Debug();
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CEnemy::Draw(void)
{
	D3DXVECTOR3 pos = GetPosition();
	D3DXMATRIX		mtxRot, mtxTrans;				//計算用マトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	for (int nCount = 0; nCount < m_nNumParts; nCount++)
	{
		if (m_pModel != NULL)
		{//パーツが存在する場合
			m_pModel[nCount].Draw(&m_mtxWorld);		//描画
		}
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
	std::string sTag = col->GetTag();

	if (m_AnimationType != ANIMATIONTYPE_DIE)
	{
		if (col->GetScene()->GetObjType() == PRIORITY_PLAYER)
		{
			if (sTag == "weapon")
			{// タグが 武器 だったとき
				if (m_AnimationType != ANIMATIONTYPE_DIE)
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
						if (pPlayer->GetState() == pPlayer->PLAYERSTATE_FLOWER)
						{// プレイヤーが暴走中だったとき
							CEffect::Slashing(GetPosition(), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
							m_nLife -= 2;				// 体力を減らす
						}
						else
						{
							m_nLife--;				// 体力を減らす
						}

						CPlayerUi *pPlayerUi = pPlayer->GetPlayerUi();				// プレイヤーUIの取得

						if (pPlayerUi != NULL)
						{// プレイヤーUIが存在していたとき
							float fDeathblow = pPlayer->GetDeathblow();				// 現在の必殺技ポイントを取得

							if (fDeathblow < 50.0f)
							{// 必殺技ポイントが5より小さかったとき
							}
						}

						if (pPlayer->GetAnimType() == CPlayer::ANIMATIONTYPE_ATTACK_1 ||
							pPlayer->GetAnimType() == CPlayer::ANIMATIONTYPE_ATTACK_2)
						{// ノックバック処理
							D3DXVECTOR3 vec;

							vec = GetPosition() - pPlayer->GetPosition();		//差分を求める(方向を求めるため)
							D3DXVec3Normalize(&vec, &vec);			//正規化する

							m_move.x = vec.x * 10;
							m_move.z = vec.z * 10;
						}

						if (pPlayer->GetAnimType() == CPlayer::ANIMATIONTYPE_ATTACK_3)
						{// ノックバック処理
							D3DXVECTOR3 vec;

							vec = GetPosition() - pPlayer->GetPosition();		//差分を求める(方向を求めるため)
							D3DXVec3Normalize(&vec, &vec);			//正規化する

							m_move.x = vec.x * 10;
							m_move.z = vec.z * 10;
							m_move.y = 10;
							m_bJump = true;
						}

						if (pPlayer->GetAnimType() == CPlayer::ANIMATIONTYPE_ATTACK_5)
						{// ノックバック処理
							D3DXVECTOR3 vec;

							vec = GetPosition() - pPlayer->GetPosition();		//差分を求める(方向を求めるため)
							D3DXVec3Normalize(&vec, &vec);			//正規化する

							m_move.x = vec.x * 100;
							m_move.z = vec.z * 100;
							m_move.y = 5;
							m_bJump = true;
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
// パーツのロード
//========================================================================================
void CEnemy::LoadScript(void)
{
	FILE *pFile;
	char cReadText[128];		// 文字
	char cHeadText[128];		// 比較
	char cDie[128];
	int nCntPointer = 0;			// ポインターの数値

	char sAdd[64];											// モデルのアドレス
	int nIndex;
	int nParent;

	int nCntMotion = 0;												// 参照するポインタの値を初期化
	int nCntKey = 0;

	int nMaxModel = 0;

	int nCntMaya = 0;				// mayaのデータ参照

	// テキストデータロード
	pFile = fopen(SCRIPT_ENEMY, "r");

	if (pFile != NULL)
	{
		// ポインターのリセット
		nCntPointer = 0;

		// スクリプトが来るまでループ
		while (strcmp(cHeadText, "SCRIPT") != 0)
		{
			fgets(cReadText, sizeof(cReadText), pFile);
			sscanf(cReadText, "%s", &cHeadText);
		}

		// スクリプトだったら
		if (strcmp(cHeadText, "SCRIPT") == 0)
		{
			// エンドスクリプトが来るまで
			while (strcmp(cHeadText, "END_SCRIPT") != 0)
			{
				fgets(cReadText, sizeof(cReadText), pFile);
				sscanf(cReadText, "%s", &cHeadText);

				// 改行
				if (strcmp(cReadText, "\n") != 0)
				{
					if (strcmp(cHeadText, "NUM_MODEL") == 0)
					{// パーツ総数のとき
						if (m_pModel == NULL)
						{
							sscanf(cReadText, "%s %s %d", &cDie, &cDie, &nMaxModel);				// パーツの数を取得

							const int nNumParts = nMaxModel;										// パーツの数を定数として定義

							if (m_pModel == NULL)
							{// モデルを格納する変数がNULLだったとき
								m_pModel = new CModel[nNumParts];									// パーツの総数分メモリを取得する
							}

							if (m_apCurrentPos == NULL)
							{// パーツの1フレームごとの移動量を格納する変数がNULLだったとき
								m_apCurrentPos = new D3DXVECTOR3[nNumParts];
							}

							if (m_apCurrentRot == NULL)
							{// パーツの1フレームごとの回転量を格納する変数がNULLだったとき
								m_apCurrentRot = new D3DXVECTOR3[nNumParts];
							}

							for (int nCount = 0; nCount < ANIMATIONTYPE_MAX; nCount++)
							{
								if (m_pAnimation[nCount].apKeyInfo == NULL)
								{
									m_pAnimation[nCount].apKeyInfo = new KEY_INFO[nNumParts];		// パーツの総数分メモリを取得する
								}
							}

							m_nNumParts = nNumParts;												// パーツの総数を格納する
						}
					}
					else if (strcmp(cHeadText, "MODEL_FILENAME") == 0)
					{// パーツモデルのアドレス情報のとき
						sscanf(cReadText, "%s %s %s", &cDie, &cDie, &sAdd[0]);						// アドレスの取得

						if (m_pModel != NULL)
						{
							m_pModel[nCntPointer].Load(sAdd);										// パーツモデルのロード
							m_pModel[nCntPointer].Init();											// パーツモデルの初期化
							nCntPointer++;															// ポインターを4Bプラス
						}
					}

					else if (strcmp(cHeadText, "CHARACTERSET") == 0)
					{// キャラクターの初期情報のとき
						nCntPointer = 0;															// 参照するポインタの値を初期化

						// エンドキャラクターセットが来るまでループ
						while (strcmp(cHeadText, "END_CHARACTERSET") != 0)
						{
							fgets(cReadText, sizeof(cReadText), pFile);
							sscanf(cReadText, "%s", &cHeadText);

							// パーツセットだったら
							if (strcmp(cHeadText, "PARTSSET") == 0)
							{
								// エンドパーツセットが来るまでループ
								while (strcmp(cHeadText, "END_PARTSSET") != 0)
								{
									fgets(cReadText, sizeof(cReadText), pFile);
									sscanf(cReadText, "%s", &cHeadText);

									//インデックス
									if (strcmp(cHeadText, "INDEX") == 0)
									{
										sscanf(cReadText, "%s %s %d", &cDie, &cDie, &nIndex);			// インデックスの値を取得
										m_pModel[nCntPointer].SetIndex(nIndex);							// インデックスの値を現在参照中のパーツモデルに格納
									}

									// 親
									if (strcmp(cHeadText, "PARENT") == 0)
									{
										sscanf(cReadText, "%s %s %d", &cDie, &cDie, &nParent);			// 親の値を取得

										if (nParent == -1)
										{// 親の値が-1だったとき
											m_pModel[nCntPointer].SetParent(NULL);				// NULLを格納する
										}
										else
										{// 親の値が-1ではないとき
											if (m_pModel != NULL)
											{// モデルの中身がNULLではないとき

											 // 現在ロードされているモデル分の中から対象が見つかるまで回す
												for (int nCount = 0; nCount < nMaxModel; nCount++)
												{
													if (nParent == m_pModel[nCount].GetIndex())
													{// パーツモデルのインデック値と自分の親の値が一致したとき
														m_pModel[nCntPointer].SetParent(&m_pModel[nCount]);				// 親のポインタを格納する
														break;
													}
												}
											}
										}
									}

									if (strcmp(cHeadText, "POS") == 0)
									{//位置
										D3DXVECTOR3 originPos;
										sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
											&originPos.x,
											&originPos.y,
											&originPos.z);

										m_pModel[nCntPointer].SetPosPreset(originPos);
										m_pModel[nCntPointer].SetPosition(originPos);
									}

									if (strcmp(cHeadText, "ROT") == 0)
									{//回転
										D3DXVECTOR3 originRot;
										sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
											&originRot.x,
											&originRot.y,
											&originRot.z);

										m_pModel[nCntPointer].SetRotPreset(originRot);
										m_pModel[nCntPointer].SetRotation(originRot);
									}
								}

								nCntPointer++;											//参照するポインタの値を進める
							}
						}
					}
					else if (strcmp(cHeadText, "MOTIONSET") == 0)
					{// キャラクターセットだったら

					 // エンドキャラクターセットが来るまでループ
						while (strcmp(cHeadText, "END_MOTIONSET") != 0)
						{
							fgets(cReadText, sizeof(cReadText), pFile);
							sscanf(cReadText, "%s", &cHeadText);

							if (strcmp(cHeadText, "LOOP") == 0)
							{// ループするかどうか
								sscanf(cReadText, "%s %s %d", &cDie, &cDie, &m_pAnimation[nCntMotion].nLoop);
							}
							else if (strcmp(cHeadText, "NUM_KEY") == 0)
							{// キー数
								sscanf(cReadText, "%s %s %d", &cDie, &cDie, &m_pAnimation[nCntMotion].nMaxKey);
							}

							if (strcmp(cHeadText, "KEYSET") == 0)
							{// キーセットだったら

							 // 参照するポインタの値を初期化
								nCntPointer = 0;
								// エンドキーセットが来るまでループ
								while (strcmp(cHeadText, "END_KEYSET") != 0)
								{
									fgets(cReadText, sizeof(cReadText), pFile);
									sscanf(cReadText, "%s", &cHeadText);

									// フレーム数
									if (strcmp(cHeadText, "FRAME") == 0)
									{
										sscanf(cReadText, "%s %s %d", &cDie, &cDie, &m_pAnimation[nCntMotion].apKeyInfo[nCntKey].nFrame);
									}

									// キーだったら
									if (strcmp(cHeadText, "KEY") == 0)
									{
										// エンドキーが来るまでループ
										while (strcmp(cHeadText, "END_KEY") != 0)
										{
											fgets(cReadText, sizeof(cReadText), pFile);
											sscanf(cReadText, "%s", &cHeadText);

											// 位置
											if (strcmp(cHeadText, "POS") == 0)
											{
												sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].pos.x,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].pos.y,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].pos.z);
											}

											// 回転
											if (strcmp(cHeadText, "ROT") == 0)
											{
												sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].rot.x,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].rot.y,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].rot.z);
											}
										}

										// パーツのキー情報を格納する場所を進める
										nCntPointer++;
									}
								}

								// keyのカウンターに1プラス
								nCntKey++;
								nCntPointer = 0;
							}
						}

						// アニメーションの種類を1プラスする
						nCntMotion++;
						nCntKey = 0;
					}
				}
			}
		}

		// ファイル閉
		fclose(pFile);
	}
	else
	{

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

	if (m_target == TARGETTYPE_HOUSE)
	{//	現在の対象が[家]だったとき
		targetPos = pHouse->GetPosition();
	}
	else if (m_target == TARGETTYPE_PLAYER)
	{// 現在の攻撃対象が[プレイヤー]だったとき
		targetPos = pPlayer->GetPosition();
	}

	D3DXVECTOR3 pos = pPos;

	float fDifference_x = pos.x - targetPos.x;
	float fDifference_z = pos.z - targetPos.z;
	float fDifference = sqrtf(fDifference_x * fDifference_x + fDifference_z * fDifference_z);

	// 敵が攻撃中かどうか
	if (m_AnimationType != ANIMATIONTYPE_ATTACK && m_AnimationType != ANIMATIONTYPE_DAMAGE && m_AnimationType != ANIMATIONTYPE_DIE)
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
					if (m_AnimationType != ANIMATIONTYPE_ATTACK && m_AnimationType != ANIMATIONTYPE_DAMAGE)
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
					if (m_AnimationType != ANIMATIONTYPE_ATTACK && m_AnimationType != ANIMATIONTYPE_DAMAGE)
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
// アニメーション関連関数
//=============================================================================
void CEnemy::Animation(void)
{
	// 最初のフレームの時
	if (m_nCurrentFrame == 0)
	{
		for (int nCount = 0; nCount < m_nNumParts; nCount++)
		{
			D3DXVECTOR3 pos = m_pModel[nCount].GetPosition();
			D3DXVECTOR3 rot = m_pModel[nCount].GetRotation();

			// posの計算((目標のkey + プリセットの配置) - 現在のキー)
			m_apCurrentPos[nCount] = ((m_pAnimation[m_AnimationType].apKeyInfo[nCount].aKey[m_nCurrentKey].pos + m_pModel[nCount].GetPosPreset()) - pos) / (float)m_pAnimation[m_AnimationType].apKeyInfo[m_nCurrentKey].nFrame;

			// rotの計算((目標のkey + プリセットの配置) - 現在のキー)
			m_apCurrentRot[nCount] = ((m_pAnimation[m_AnimationType].apKeyInfo[nCount].aKey[m_nCurrentKey].rot + m_pModel[nCount].GetRotPreset()) - rot) / (float)m_pAnimation[m_AnimationType].apKeyInfo[m_nCurrentKey].nFrame;
		}
	}
	else
	{// それ以外のフレーム
		for (int nCount = 0; nCount < m_nNumParts; nCount++)
		{
			D3DXVECTOR3 pos = m_pModel[nCount].GetPosition();
			D3DXVECTOR3 rot = m_pModel[nCount].GetRotation();

			// rotの移動
			m_pModel[nCount].SetRotation(rot + m_apCurrentRot[nCount]);

			// posの移動
			m_pModel[nCount].SetPosition(pos + m_apCurrentPos[nCount]);
		}
	}

	if (m_pAnimation[m_AnimationType].apKeyInfo != NULL)
	{// キー情報がNULLではないとき
	 // フレームの最大数に満たない場合
		if (m_nCurrentFrame < m_pAnimation[m_AnimationType].apKeyInfo[m_nCurrentKey].nFrame)
		{
			m_nCurrentFrame++;
		}
		// フレーム数の最大値に達した場合
		else if (m_nCurrentFrame >= m_pAnimation[m_AnimationType].apKeyInfo[m_nCurrentKey].nFrame)
		{
			m_nCurrentKey++;
			m_nCurrentFrame = 0;
		}
	}

	// キー数の最大値に達した場合
	if (m_nCurrentKey >= m_pAnimation[m_AnimationType].nMaxKey)
	{
		// ループするかどうか
		if (m_AnimationType == ANIMATIONTYPE_DIE)
		{
			Release();
			m_bAnimSwitch = false;
		}
		else if (m_pAnimation[m_AnimationType].nLoop)
		{
			// キーのリセット
			m_nCurrentKey = 0;
			m_nCurrentFrame = 0;
		}
		else
		{
			// ニュートラルモーション
			m_AnimationType = ANIMATIONTYPE_NEUTRAL;
			m_nCurrentFrame = 0;

			// キーのリセット
			m_nCurrentKey = 0;
		}
	}

	if (m_AnimationType != ANIMATIONTYPE_ATTACK && m_AnimationType != ANIMATIONTYPE_DAMAGE && m_AnimationType != ANIMATIONTYPE_DIE)
	{
		if (m_nInterval > INTERVAL)
		{// 現在のカウントが目標値を超えたとき
			if (fabs(m_move.x) > 2 || fabs(m_move.z) > 2)
			{
				if (m_AnimationType != ANIMATIONTYPE_RUN)
				{// アニメーションが走る以外のとき
					AnimationSwitch(ANIMATIONTYPE_RUN);			// 走るモーションに変更
				}
			}
			else
			{
				if (m_AnimationType != ANIMATIONTYPE_NEUTRAL)
				{// 待機モーション以外だったとき
					AnimationSwitch(ANIMATIONTYPE_NEUTRAL);		// 待機モーションに変更
				}
			}
		}
		else
		{
			if (m_AnimationType != ANIMATIONTYPE_NEUTRAL)
			{// 待機モーション以外だったとき
				AnimationSwitch(ANIMATIONTYPE_NEUTRAL);			// 待機モーション
			}
		}
	}
}

//=============================================================================
// アニメーションの切り替え
//=============================================================================
void CEnemy::AnimationSwitch(ANIMATIONTYPE Type)
{
	m_AnimationType = Type;					// アニメーションの切り替え
	m_nCurrentFrame = 0;					// 現在のフレーム数をリセット
	m_nCurrentKey = 0;						// 現在のキー数をリセット
}

//=============================================================================
// 当たり判定処理
//=============================================================================
void CEnemy::Collider(void)
{
	switch (m_AnimationType)
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
			if (m_nCurrentKey >= 1)
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