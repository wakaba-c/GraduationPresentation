//=============================================================================
//
// デバッグ処理 [debug.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _DEBUG_H_
#define _DEBUG_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_dx9.h"
#include "imgui\imgui_impl_win32.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define FLOOR_LIMIT 20
#define NAME_SIZE 32

//=============================================================================
// 構造体定義
//=============================================================================
typedef enum
{
	DEBUGMODE_NONE = 0,
	DEBUGMODE_RAND,
	DEBUGMODE_WALL,
	DEBUGMODE_MANY,
	DEBUGMODE_INDIVIDUAL,
	DEBUGMODE_DELETE,
	DEBUGMODE_PAINT,
	DEBUGMODE_ENEMY,
	DEBUGMODE_PARTICLE,
	DEBUGMODE_UI,
	DEBUGMODE_MAX
} DEBUGMODE;

//=============================================================================
// 前方宣言
//=============================================================================
class CCircle;
class CScene;
class CObject;
class CEnemy;
class CMeshField;
class CUi;

//=============================================================================
// クラス定義
//=============================================================================
class CDebugProc
{
public:
	CDebugProc();						// コンストラクタ
	~CDebugProc();						// デストラクタ
	HRESULT Init(HWND hWnd);			// 初期化処理
	void Uninit(void);					// 開放処理
	void Update(void);					// 更新処理
	void Draw(void);					// 描画処理

	static void Log(char* fmt, ...);	// デバッグログの処理

	static CCircle *GetCircle(void) { return m_pCircle; }	// 円の範囲内にいるか
	static bool GetDebugState(void) { return m_bDebug; }	// デバッグが使用中か
	static void SelectModel(void);

private:
	static void Debug(void);								// デバッグ処理
	static void MenuBar(void);								// メニューバー処理
	static void TabBar(D3DXVECTOR3 &worldPos);								// タブ処理

	static void LandScape(D3DXVECTOR3 &worldPos);			// ランドスケープ処理
	static void EditWallVertex(void);						// 壁の頂点編集処理
	static void Paint(D3DXVECTOR3 &worldPos);				// ペイント処理
	static void CreateEnemy(D3DXVECTOR3 &worldPos);			// エネミー配置処理
	static void CreateObject(D3DXVECTOR3 &worldPos);		// 木配置処理
	static void CreateIndividual(D3DXVECTOR3 &worldPos);	// 単体配置処理
	static void	DeleteObject(D3DXVECTOR3 &worldPos);		// オブジェクト範囲内削除処理
	static void CreateParticle(void);						// パーティクル作成
	static void DebugFloorCreate(const int &nWide, const int &nDepth, D3DXVECTOR3 &createPos);
	static void ShowInspector(void);
	static void SelectAssetWithModel(void);					// モデル選択処理
	static void SelectAssetWithTexture(void);				// テクスチャ選択処理
	static void SelectAssetWithUI(void);					// UI選択処理

	LPD3DXFONT	m_pFont;									// フォントへのポインタ
	static char m_aStr[1024];								// 文字列
	static CObject *m_pSample;								// 見本用オブジェクトポインタ
	static CScene *m_pSelect;								// 選択したオブジェクト
	static CEnemy *m_pEnemy;								// 見本用敵ポインタ
	static float m_fPaintSize;								// 地形編集ブラシの大きさ
	static int m_nCreateIndex;								// 1フレームの生成数
	static bool m_bDebug;									// デバッグモードの切り替え
	static bool m_bInspectorWind;							// インスペクターウィンドウの表示切替
	static bool m_bHeightCalculation;						// 高さ計算使用フラグ
	static int m_nCntGeneration;							// 生成数
	static int m_nMode;										// モード選択
	static D3DXVECTOR3 m_mouseOld;							// マウスの前のワールド座標
	static int m_nCntContinue;								// 再演算回数
	static CCircle *m_pCircle;								// 円のポインタ
	static D3DXVECTOR2 m_CreateRand;						// 床の量
	static D3DXVECTOR2 m_CreateRandOld;						// 床の量
	static CMeshField *m_apMeshField[FLOOR_LIMIT * FLOOR_LIMIT];
	static D3DXVECTOR3 m_createPos;
	static float m_fSliderPow;

	static std::string m_currentModel;						// モデルのアドレス
	static std::string m_currentTexture;					// テクスチャアドレス
	static std::string m_currentUi;							// UIアドレス

	static HWND m_hWnd;										// ウィンドウハンドル
	static bool m_bMouseCursorPosition;						// マウスカーソル座標の使用フラグ

	// エフェクト作成関連
	static int m_nParticleShape;							// パーティクル形状
	static int m_particleLife;								// パーティクルの生存時間
	static int m_nCreate;									// 生成数
	static int m_nInterval;									// インターバル
	static float m_fStartRadius;							// 始まりの
	static float m_fRadius;									// 半径
	static float m_fMinSpeed;								// 最低スピード
	static float m_fSpeed;									// スピード

	static bool m_bLoop;									// 生成を繰り返す
	static bool m_bGravity;									// 重力の有無
	static bool m_bRandomSpeed;								// スピードランダム化の有無

	// UI生成関連
	static void LoadAddWithUI(void);			// UIアドレスの読み込み
	static char m_CreateName[NAME_SIZE];		// 生成名
	static CUi *m_pCreateUi;					// UI
	static std::vector<std::string> m_AddUi;					// Uiアドレスの配列
};
#endif