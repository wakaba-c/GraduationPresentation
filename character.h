//=============================================================================
//
// キャラクター処理 [character.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _CHARACTER_H_
#define _CHARACTER_H_

//=============================================================================
// インクルード
//=============================================================================
#include "main.h"
#include "scene.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MAX_KEY 30

//=============================================================================
// 構造体定義
//=============================================================================
// キー要素
typedef struct
{
	D3DXVECTOR3		pos;																			// 位置
	D3DXVECTOR3		rot;																			// 回転
} KEY;

// キー情報
typedef struct
{
	KEY				aKey[MAX_KEY];																	// パーツのキー情報
	int				nFrame;																			// フレーム数
} KEY_INFO;

// アニメーション
typedef struct
{
	int				nLoop;																			// ループするかどうか
	int				nMaxKey;																		// キー数
	int				nSwitchFrame;																	// 切り替えるまでのフレーム数
	KEY_INFO		*apKeyInfo;																		// キーの構造体
} ANIMATION;

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
class CCharacter : public CScene
{
public:
	CCharacter(PRIORITY obj);						// プレイヤーのコンストラクタ(オブジェクトタイプ)
	~CCharacter();									// プレイヤーのデストラクタ
	HRESULT Init(void);							// 初期化処理
	void Uninit(void);							// 開放処理
	void Update(void);							// 更新処理
	void Draw(void);							// 描画処理

	static HRESULT Load(void);					// 素材データの取得

	void OnTriggerEnter(CCollider *col);
	void OnCollisionEnter(CCollider *col);
	void ShowInspector(void) {};

	virtual void BehaviorForMaxFrame(void) = 0;		// 最大フレーム数に到達したときの処理
	virtual void BehaviorForMaxKey(void) = 0;		// 最大キー数に到達したときの処理

	D3DXVECTOR3 GetMove(void) { return m_move; }				// 移動量の取得
	D3DXVECTOR3 GetRotation(void) { return m_rot; }				// 回転の取得
	CModel *GetModel(void) { return m_pModel; }					// パーツの先頭アドレス取得
	ANIMATION *GetAnimData(void) { return m_pAnimation; }		// 各アニメーションデータの取得
	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; }			// ワールドマトリックスの取得
	void LoadScript(std::string add, const int nMaxAnim);		// スクリプト読み込み
	void SetRotation(D3DXVECTOR3 rot) { m_rot = rot; }			// 回転の設定

	/*================= アニメーション関連 =================*/
	// 設定
	void AnimationSwitch(int nType);								// アニメーションの切り替え処理
	void SetAnimPlayState(bool bValue) { m_bAnimation = bValue; }	// アニメーション再生停止の設定
	void SetCurrentKey(int nValue) { m_nCurrentKey = nValue; }		// 現在のキー数を設定
	void SetCurrentFrame(int nValue) { m_nCurrentFrame = nValue; }	// 現在のフレーム数を設定
	void ResetKeyAndFrame(void);									// キー数とフレーム数のリセット

	// 取得
	int GetAnimType(void) { return m_nAnimationType; }			// アニメーションタイプを取得
	int GetCurrentKey(void) { return m_nCurrentKey; }			// 現在のキー数を取得
	int GetCurrentFrame(void) { return m_nCurrentFrame; }		// 現在のフレーム数を取得
	bool GetAnimPlayState(void) { return m_bAnimation; }		// アニメーション再生停止の取得

private:
#ifdef _DEBUG
	void Debug(void);																// デバッグ処理関数
#endif

	void Animation(void);															// アニメーション関連関数

	/*================= プレイヤー関連 =================*/
	D3DXVECTOR3						m_dest;											// モデルの最終到達点
	D3DXVECTOR3						m_difference;									// モデルの最大回転
	D3DXVECTOR3						m_move;											// 移動量
	D3DXVECTOR3						m_size;											// 大きさ
	D3DXVECTOR3						m_rot;											// 回転量
	D3DXCOLOR						m_color;										// 色
	float							m_fSpeed;										// スピード
	int								m_nActionCount;									// 次のアクションまでのカウンタ

	/*=============== 3Dレンダリング関連 ===============*/
	LPDIRECT3DVERTEXBUFFER9			m_pVtxBuff;										// 頂点バッファへのポインタ
	LPD3DXMESH						m_pMesh;										// メッシュ情報へのポインタ
	DWORD							m_nNumMat;										// マテリアル情報の数
	LPD3DXBUFFER					m_pBuffMat;										// マテリアル情報へのポインタ
	CModel							*m_pModel;										// パーツモデルのポインタ
	D3DXMATRIX						m_mtxWorld;										// ワールドマトリックス

	/*=============== アニメーション関連 ===============*/
	int								m_nNumKey;										// キーの総数
	int								m_nNumParts;									// パーツの総数
	int								m_nCurrentKey;									// 現在のキーNo.
	int								m_nCurrentFrame;								// 現在のフレームNo.
	int								m_nCntMotion;									// カウンターモーション
	int								m_nMaxAnimationType;							// アニメーションタイプの最大数
	ANIMATION						*m_pAnimation;									// キー情報ポインタ
	int								m_nAnimationType;								// 現在のアニメーションタイプ
	int								m_nAnimationTypeOld;							// 前のアニメーションタイプ

	D3DXVECTOR3						*m_apCurrentPos;								// 1フレーム当たりの移動量
	D3DXVECTOR3						*m_apCurrentRot;								// 1フレーム当たりの回転量

	bool							m_bAnimSwitch;									// アニメーションの切り替え用フレームを使用するか
	bool							m_bAnimation;									// アニメーションの再生
};
#endif