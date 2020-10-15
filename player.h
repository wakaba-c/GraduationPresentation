//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

//=============================================================================
// インクルード
//=============================================================================
#include "main.h"
#include "character.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MAX_KEY 30
#define NORMAL_SPEED 1.0f
#define SP_SPEED 2.0f

//=============================================================================
// 前方宣言
//=============================================================================
class CMeshCapsule;
class CMeshCube;
class CMeshOrbit;
class CColliderSphere;
class CColliderBox;
class CScene2D;
class CModel;
class CPlayerUi;

//=============================================================================
// クラス定義
//=============================================================================
class CPlayer : public CCharacter
{
public:
	//モーションタイプ
	typedef enum
	{
		ANIMATIONTYPE_NONE = -1,				// なし
		ANIMATIONTYPE_NEUTRAL = 0,				// 待機
		ANIMATIONTYPE_WALK,						// 歩く
		ANIMATIONTYPE_RUN,						// 走る
		ANIMATIONTYPE_ATTACK_1,					// 基本攻撃1
		ANIMATIONTYPE_ATTACK_2,					// 基本攻撃2
		ANIMATIONTYPE_ATTACK_3,					// 基本攻撃3
		ANIMATIONTYPE_ATTACK_4,					// 基本攻撃4
		ANIMATIONTYPE_ATTACK_5,					// ため攻撃
		ANIMATIONTYPE_ATTACK_6,					// 派生攻撃1
		ANIMATIONTYPE_RUNATTACK,				// 走り攻撃
		ANIMATIONTYPE_DAMAGE,					// ダメージ
		ANIMATIONTYPE_SWITCHWEAPON,				// 武器の切り替え
		ANIMATIONTYPE_JUMP_1,					// ジャンプ1
		ANIMATIONTYPE_JUMP_2,					// ジャンプ2
		ANIMATIONTYPE_JUMP_3,					// ジャンプ3
		ANIMATIONTYPE_JUMP_4,					// ジャンプ4
		ANIMATIONTYPE_JUMP_5,					// ジャンプ5
		ANIMATIONTYPE_MAX						// アニメーションの最大数
	} ANIMATIONTYPE;							// アニメーションタイプ

	typedef enum
	{
		PLAYERSTATE_NONE = -1,					// 何もない
		PLAYERSTATE_NORMAL = 0,					// 通常
		PLAYERSTATE_FLOWER,						// 暴走
		PLAYERSTATE_MAX
	} PLAYERSTATE;

	CPlayer(PRIORITY obj);						// プレイヤーのコンストラクタ(オブジェクトタイプ)
	~CPlayer();									// プレイヤーのデストラクタ
	HRESULT Init(void);							// 初期化処理
	void Uninit(void);							// 開放処理
	void Update(void);							// 更新処理
	void Draw(void);							// 描画処理

	static CPlayer *Create(void);				// プレイヤー生成
	static HRESULT Load(void);					// 素材データの取得
	void SetDeathblow(float nValue);			// 必殺技ポイント数の設定

	void OnTriggerEnter(CCollider *col);
	void OnCollisionEnter(CCollider *col);
	void ShowInspector(void) {};

	void BehaviorForMaxFrame(void);				// 最大フレーム数に到達したときの処理
	void BehaviorForMaxKey(void);				// 最大キー数に到達したときの処理

	float GetDeathblow(void) { return m_fDeathblow; }				// 必殺技ポイント数の取得
	CPlayerUi *GetPlayerUi(void) { return m_pPlayerUi; }			// プレイヤーUIの取得
	D3DXVECTOR3 GetMove(void) { return m_move; }					// 移動量の取得
	PLAYERSTATE GetState(void) { return m_state; }					// プレイヤーの状態を取得
	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; }				// ワールドマトリックスの取得
	D3DXVECTOR3 GetRotDest(void) { return m_dest; }					// 回転最終到達地点

private:
#ifdef _DEBUG
	void Debug(void);																// デバッグ処理関数
#endif

	void MoveNearEnemy(void);														// 近くにいる敵に移動する処理
	void Collision(void);															// 当たり判定処理
	void Input(void);																// キー入力情報関数

	/*================= プレイヤー関連 =================*/
	PLAYERSTATE						m_state;										// プレイヤー状態
	D3DXVECTOR3						m_dest;											// モデルの最終到達点
	D3DXVECTOR3						m_difference;									// モデルの最大回転
	D3DXVECTOR3						m_move;											// 移動量
	D3DXVECTOR3						m_size;											// 大きさ
	D3DXVECTOR3						m_rot;											// 回転量
	D3DXCOLOR						m_color;										// 色
	float							m_fSpeed;										// スピード
	float							m_fDeathblow;									// 必殺技ポイント
	int								m_nLife;										// 体力
	int								m_nActionCount;									// 次のアクションまでのカウンタ
	int								m_nParticleCount;								// パーティクル生成までのカウンタ
	bool							m_bJump;										// ジャンプ
	bool							m_bEvent;										// イベント発生フラグ
	CPlayerUi						*m_pPlayerUi;									// キャラクター情報のUI

	/*=============== 3Dレンダリング関連 ===============*/
	LPDIRECT3DVERTEXBUFFER9			m_pVtxBuff;										// 頂点バッファへのポインタ
	LPD3DXMESH						m_pMesh;										// メッシュ情報へのポインタ
	DWORD							m_nNumMat;										// マテリアル情報の数
	LPD3DXBUFFER					m_pBuffMat;										// マテリアル情報へのポインタ
	D3DXMATRIX						m_mtxWorld;										// ワールドマトリックス

	CMeshOrbit						*m_pMeshOrbit;									// 軌跡のポインタ
	int								m_nCntAttacCombo;								// コンボが持続するまでのカウンタ

	/*===============当たり判定===============*/
	CColliderSphere					*m_pColWeaponSphere;							// 武器の当たり判定のポインタ
	CColliderSphere					*m_pColHandSphere;								// 手の当たり判定のポインタ
	CColliderSphere					*m_pColPlayerSphere;							// プレイヤーの当たり判定のポインタ
	CColliderBox					*m_pBox;										// 箱
};
#endif