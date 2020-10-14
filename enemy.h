//=============================================================================
//
// 敵処理 [enemy.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _ENEMY_H_
#define _ENEMY_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MAX_KEY 30				// キーの最大値

//=============================================================================
// 列挙体定義
//=============================================================================
// ターゲットタイプ
typedef enum
{
	TARGETTYPE_NONE = -1,
	TARGETTYPE_PLAYER = 0,
	TARGETTYPE_HOUSE,
	TARGETTYPE_MAX
} TARGETTYPE;

//=============================================================================
// 前方宣言
//=============================================================================
class CMeshCapsule;
class CColliderSphere;
class CColliderBox;
class CMeshCube;
class CCollider;
class CLife;
class CModel;

//=============================================================================
// クラス定義
//=============================================================================
class CEnemy : public CScene
{
public:
	//モーションタイプ
	typedef enum
	{
		ANIMATIONTYPE_NONE = -1,
		ANIMATIONTYPE_NEUTRAL = 0,
		ANIMATIONTYPE_RUN,
		ANIMATIONTYPE_ATTACK,
		ANIMATIONTYPE_DAMAGE,
		ANIMATIONTYPE_DIE,
		ANIMATIONTYPE_MAX
	} ANIMATIONTYPE;

	//キー要素
	typedef struct
	{
		D3DXVECTOR3 pos;			// 位置
		D3DXVECTOR3 rot;			// 回転
	} KEY;

	//キー情報
	typedef struct
	{
		KEY aKey[MAX_KEY];			// パーツのキー情報
		int nFrame;					// フレーム数
	} KEY_INFO;

	// アニメーション情報
	typedef struct
	{
		int	nLoop;					// ループするかどうか
		int	nMaxKey;				// キー数
		KEY_INFO *apKeyInfo;		// キーの構造体
	} ANIMATION;

	CEnemy(PRIORITY obj);			// コンストラクタ
	~CEnemy();						// デストラクタ
	HRESULT Init(void);				// 初期化処理
	void Uninit(void);				// 開放処理
	void Update(void);				// 更新処理
	void Draw(void);				// 描画処理

	static CEnemy *Create(void);	// クリエイト処理
	static HRESULT Load(void);		// ロード処理
	static void LoadEnemy(char *add);		// 敵配置情報読み込み処理
	void SetTarget(TARGETTYPE target);		// ターゲットの設定

	void OnTriggerEnter(CCollider *col);
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

	static int GetEnemyKill(void) { return m_nKill; }					// 倒した数の取得
	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; }					// マトリックスの取得
	CColliderSphere *GetSphere(void) { return m_pSphere; }				// スフィアコライダーの取得
	TARGETTYPE GetTarget(void) { return m_target; }						// ターゲットの取得

private:
#ifdef _DEBUG
	void Debug(void);													// デバッグ処理
#endif
	void LoadScript(void);												// スクリプトデータのロード関数
	void Move(D3DXVECTOR3 &pPos);										// 移動処理
	void Animation(void);												// アニメーション関連関数
	void AnimationSwitch(ANIMATIONTYPE Type);							// アニメーションの切り替え処理
	void Collider(void);												// 当たり判定マネージャ

	static LPDIRECT3DTEXTURE9		m_pTexture;							// テクスチャへのポインタ
	D3DXMATRIX						m_mtxWorld;							// ワールドマトリックス

	/*===============3Dレンダリング関連===============*/
	LPDIRECT3DVERTEXBUFFER9			m_pVtxBuff;							// 頂点バッファへのポインタ
	static LPD3DXMESH				m_pMesh;							// メッシュ情報へのポインタ
	static DWORD					m_nNumMat;							// マテリアル情報の数
	static LPD3DXBUFFER				m_pBuffMat;							// マテリアル情報へのポインタ
	CModel							*m_pModel;							// パーツモデルのポインタ

	/*===============アニメーション関連===============*/
	int								m_nNumKey;							// キーの総数
	int								m_nNumParts;						// パーツの総数
	int								m_nCurrentKey;						// 現在のキーNo.
	int								m_nCurrentFrame;					// 現在のフレームNo.
	int								m_nCntMotion;						// カウンターモーション
	ANIMATION						m_pAnimation[ANIMATIONTYPE_MAX];	// キー情報ポインタ
	ANIMATIONTYPE					m_AnimationType;					// 現在のアニメーションタイプ
	D3DXVECTOR3						*m_apCurrentPos;					// 1フレーム当たりの移動量
	D3DXVECTOR3						*m_apCurrentRot;					// 1フレーム当たりの回転量
	int								m_nInterval;						// 休憩時間

	/*=============== 当たり判定 ===============*/
	CColliderSphere					*m_pSphere;							// 当たり判定(体)のポインタ
	CColliderSphere					*m_pAttack;							// 当たり判定(攻撃)のポインタ
	CColliderBox					*m_pBox;							// 当たり判定(キューブ)のポインタ

	/*=============== ステータス ===============*/
	static int						m_nKill;							// 倒した数
	D3DXVECTOR3						m_rot;								// 回転
	D3DXVECTOR3						m_move;								// 移動量
	D3DXVECTOR3						m_dest;								// 移動先
	D3DXVECTOR3						m_difference;						// 差
	int								m_nLife;							// 体力
	bool							m_bAnimSwitch;						// アニメーションの有無
	bool							m_bHit;								// 当たり判定
	bool							m_bJump;							// ジャンプ
	CLife							*m_pLife;							// 体力ゲージ
	TARGETTYPE						m_target;							// 攻撃対象
};
#endif