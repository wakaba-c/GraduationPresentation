//=============================================================================
//
// エフェクト処理 [effect.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _EFFECT_H_
#define _EFFECT_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MAX_EFFECT 3000					// エフェクトの最大数

//=============================================================================
// 構造体定義
//=============================================================================
// パーティクルデバッグモード
typedef enum
{
	PARTICLESHAPE_CONE = 0,
	PARTICLESHAPE_SPHERE,
	PARTICLESHAPE_BOX,
	PARTICLESHAPE_MAX
} PARTICLESHAPE;

typedef enum
{
	EFFECTTYPE_ROSE = 0,
	EFFECTTYPE_SMOKE,
	EFFECTTYPE_SANDSMOKE,
	EFFECTTYPE_HALO,
	EFFECTTYPE_SHOCKWAVE,
	EFFECTTYPE_STAR,
	EFFECTTYPE_SLASH,
	EFFECTTYPE_SPHERE,
	EFFECTTYPE_WING,
	EFFECTTYPE_AURA,
	EFFECTTYPE_MAX
} EFFECTTYPE;

typedef enum
{
	EASINGTYPE_NONE = 0,
	EASINGTYPE_IN,
	EASINGTYPE_OUT,
	EASINGTYPE_MAX
} EASINGTYPE;

typedef struct
{
	LPDIRECT3DTEXTURE9		pTexture;				// テクスチャ情報のポインタ
	D3DXVECTOR2				sprite;					// 画像の分割数
	bool					bAlpha;					// アルファブレンディングの有無
} EFFECT;

//=============================================================================
// 前方宣言
//=============================================================================
class CMeshCapsule;
class CMeshSphere;
class CMeshCube;

//=============================================================================
// クラス定義
//=============================================================================
class CEffect : public CScene
{
public:
	CEffect(PRIORITY obj);															// コンストラクタ
	~CEffect();																		// デストラクタ
	HRESULT Init(void);																// 初期化処理
	void Uninit(void);																// 終了処理
	void Update(void);																// 更新処理
	void Draw(void);																// 描画処理

	static void Create();															// クリエイト処理
	static HRESULT Load(void);														// 素材の読み込み

	void SetPosition(D3DXVECTOR3 pos);												// 位置の設定
	void SetMove(D3DXVECTOR3 move);													// 移動量の設定
	void SetUse(bool bValue);														// 使用中設定
	void SpriteAnimation(D3DXVECTOR2 patternNo, int nPatternAnim, int nHeight);		// スプライトアニメーション処理
	static void SetEffect(EFFECTTYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 size,		// エフェクトの発生処理
		D3DXVECTOR3 move, D3DXVECTOR3 moveSize, EASINGTYPE easingType,
		D3DXVECTOR3 rot, D3DXCOLOR col, int nLife, bool bGravity, float fResistance,
		bool bBillboard, int nPatternAnim, int nHeight, D3DXVECTOR3 centerPos,
		float fAngle, float fDistance, float fRotationSpeed);

	static void FallingPetals(bool bGravity, D3DXVECTOR3 pos, int nLife);			// 舞い散る花びらのエフェクト
	static void PetalsGather(D3DXVECTOR3 pos);										// 集まる花びらのエフェクト
	static void PetalsSoaring(D3DXVECTOR3 pos);										// 花びらが舞い上がるエフェクト
	static void WingStay(D3DXVECTOR3 pos);											// 羽が舞うエフェクト
	static void SandSmokeEffect(D3DXVECTOR3 pos);									// 砂煙のエフェクト
	static void SandSmokeExplosion(D3DXVECTOR3 pos);								// 粉塵爆発エフェクト
	static void Shockwave(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nLife);				// ショック・ウェーブ
	static void	Halo(D3DXVECTOR3 pos, D3DXVECTOR3 size);							// ハロー現象
	static void Purification(D3DXVECTOR3 pos);										// 浄化分散
	static void Star(D3DXVECTOR3 pos);												// スター
	static void Slashing(D3DXVECTOR3 pos, D3DXVECTOR3 rot);							// 斬撃
	static void Aura(int nCount, bool bJump, D3DXVECTOR3 pos);						// オーラ
	static void PetalCluster(D3DXVECTOR3 pos, D3DXVECTOR3 rot);						// 花びらの集合体

	bool GetUse(void) { return m_bUse; }											// 使用状態の取得
	int GetAnimPage(void) { return m_AnimPage; }									// ページ数の取得
	D3DXVECTOR3 GetMove(void) { return m_move; }									// 移動量の取得
	D3DXVECTOR3 GetPosition(void) { return m_pos; }									// 位置の取得
	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; }								// ワールドマトリックスの取得

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
#ifdef _DEBUG
	void Debug(void);

	/* ===============当たり判定可視化=============== */
	CMeshCapsule					*m_pCapsule;									// カプセルのポインタ
	CMeshSphere						*m_pSphere;										// 球体のポインタ
	CMeshCube						*m_pCube;										// 四角形のポインタ
	ImVec4							debug_size;										// コライダーサイズ
	ImVec4							debug_pos;										// コライダーポジションオフセット値
#endif

	void MakeVertex(void);															// 頂点情報の作成
	void Animation(void);															// アニメーション処理

	static CEffect *m_pEffect[MAX_EFFECT];											// 生成されたエフェクトポインタ

	/* ================ ステータス ================ */
	static LPDIRECT3DTEXTURE9		m_pTexture[EFFECTTYPE_MAX];						// テクスチャへのポインタ
	D3DXVECTOR3						m_pos;											// ポジション

	D3DXVECTOR3						m_centerPos;									// 回転中心
	float							m_fRotationSpeed;								// 回転速度
	float							m_fAngle;
	float							m_fDistance;

	D3DXVECTOR3						m_rot;											// 回転量
	D3DXVECTOR3						m_move;											// 移動量
	D3DXVECTOR3						m_posOld;										// 前のポジション
	D3DXVECTOR3						m_size;											// 大きさ
	D3DXVECTOR3						m_moveSize;
	D3DXCOLOR						m_col;											// 色
	D3DXMATRIX						m_mtxWorld;										// ワールドマトリックス
	EASINGTYPE						m_easingType;									// イージングタイプ
	float							m_fRadius;										// 半径
	float							m_fResistance;									// 抵抗
	int								m_nLife;										// ライフ
	int								m_nMaxLife;										// ライフの最大値
	bool							m_bGravity;										// 重力
	bool							m_bUse;											// 使用しているかどうか
	bool							m_bBillboard;									// ビルボードの使用

	/* ================ パーティクル ================ */
	static EFFECT					m_aEffect[EFFECTTYPE_MAX];						// エフェクト
	EFFECTTYPE						m_type;											// パーティクルタイプ
	int								m_AnimPage;										// 現在のページ数
	int								m_AnimCount;									// 次のページカウント
	int								m_nInterval;									// 次のページまでの間隔

	/*===============3Dレンダリング関連===============*/
	LPDIRECT3DVERTEXBUFFER9			m_pVtxBuff;										// 頂点バッファへのポインタ
	LPD3DXMESH						m_pMesh;										// メッシュ情報へのポインタ
	DWORD							m_nNumMat;										// マテリアル情報の数
	LPD3DXBUFFER					m_pBuffMat;										// マテリアル情報へのポインタ
};
#endif