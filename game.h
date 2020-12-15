//=============================================================================
//
// ゲーム処理 [game.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _GAME_H_
#define _GAME_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "box.h"

//=============================================================================
// 前方宣言
//=============================================================================
class CScene3D;
class CMeshField;
class CPlayer;
class CMeshSphere;
class CEnemy;
class CSky;
class CHouse;
class CHouseUi;
class CSpeed;
class CTime;

//=============================================================================
// クラス定義
//=============================================================================
class CGame
{
public:
	CGame();					// コンストラクタ
	~CGame();					// デストラクタ
	HRESULT Init(void);			// 初期化処理
	void Uninit(void);			// 開放処理
	void Update(void);			// 更新処理
	void Draw(void);			// 描画処理

	static void	LoadAsset(void);		// アセットの読み込み処理

	static CMeshField	*GetMesh(void) { return m_pMeshField; }				// メッシュフィールドの取得
	static CPlayer		*GetPlayer(void) { return m_pPlayer; }				// プレイヤーの取得
	static CMeshSphere	*GetMeshSphere(void) { return m_pMeshSphere; }		// メッシュスフィアの取得
	static CEnemy		*GetEnemy(void) { return m_pEnemy; }				// 敵の取得
	static CSky			*GetSky(void) { return m_pSky; }					// 空の取得
	static CHouse		*GetHouse(void) { return m_pHouse; }				// 家の取得

private:
	static CMeshField	*m_pMeshField;										// メッシュフィールドのポインタ
	static CPlayer		*m_pPlayer;											// プレイヤーのポインタ
	static CMeshSphere	*m_pMeshSphere;										// メッシュスフィアのポインタ
	static CEnemy		*m_pEnemy;											// 敵のポインタ
	static CSky			*m_pSky;											// 空のポインタ
	static CHouse		*m_pHouse;											// 家のポインタ
	static CSpeed		*m_pSpeed;											// 時速のポインタ

	static CUi			*m_pUi;
	static CTime		*m_pTime;

	bool m_bGuideSign[Piece_Num];
	bool m_bRate;
	bool m_bRoute;
};
#endif