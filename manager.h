//=============================================================================
//
// マネージャー処理 [manager.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _MANAGER_H_
#define _MANAGER_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define D3DXVECTOR3_ZERO D3DXVECTOR3(0.0f, 0.0f, 0.0f);
#define	SHADERADD_LAMBERT ("data/shader/LambertShader.fx")
#define	SHADERADD_TOON ("data/shader/ToonShader.fx")
#define TEXTUREADD_DEFAULT ("data/tex/default.jpg")

//=============================================================================
// 列挙体定義
//=============================================================================
typedef enum
{
	LOADTYPE_TEXTURE = 0,
	LOADTYPE_MODEL,
	LOADTYPE_MAX
} LOADTYPE;

//=============================================================================
// 構造体定義
//=============================================================================
typedef struct
{
	LPD3DXMESH		pMesh;									// メッシュ情報へのポインタ
	DWORD			nNumMat;									// マテリアル情報の数
	LPD3DXBUFFER		pBuffMat;								// マテリアル情報へのポインタ
} MODEL_INFO;

//=============================================================================
// 前方宣言
//=============================================================================
class CRenderer;
class CInputKeyboard;
class CInputMouse;
class CInputController;
class CGame;
class CTitle;
class CPuzzle;
class CCamera;
class CLight;
class CResult;
class CRanking;
class CSound;
class CNetwork;

//=============================================================================
// クラス定義
//=============================================================================
class CManager
{
public:
	// モード
	typedef enum
	{
		MODE_NONE = -1,
		MODE_TITLE,							// タイトル
		MODE_DEMO_PLAY,						// デモプレイ
		MODE_CHARACTER_SELECT,				// キャラクター選択
		MODE_STAGE_SELECT,					// ステージ選択
		MODE_PUZZLE_CUSTOM,					// パズルカスタム
		MODE_GAME,							// ゲーム
		MODE_RESULT,						// リザルト
		MODE_RANKING,						// ランキング
		MODE_MAX
	} MODE;

	CManager();														// コンストラクタ
	~CManager();													// デストラクタ
	HRESULT Init(HINSTANCE hInstance, HWND hWnd, bool bWindow);		// 初期化処理
	void Uninit(void);												// 開放処理
	void Update(void);												// 更新処理
	void Draw(void);												// 描画処理

	static void SetMode(MODE mode);									// モードの設定

	static MODE GetMode(void) { return m_mode; }					// モードを取得
	static CRenderer *GetRenderer(void) { return m_pRenderer; }								// レンダラーの取得
	static CInputKeyboard *GetInputKeyboard(void) { return m_pInputKeyboard; }				// キーボードの取得
	static CInputMouse *GetInputMouse(void) { return m_pInputMouse; }						// マウスの取得
	static CInputController *GetInputController(void) { return m_pInputController; }		// コントローラーの取得
	static CSound *GetSound(void) { return m_pSound; }										// サウンドの取得
	static CCamera *GetCamera(void) { return m_pCamera; }									// カメラの取得
	static CLight *GetLight(void) { return m_pLight; }										// ライトの取得
	static CNetwork *GetNetwork(void) { return m_pNetwork; }								// ネットワークの取得

	static CGame *GetGame(void) { return m_pGame; }											// ゲームの取得

	static void ConvertStringToFloat(char* text, char* delimiter, float* pResult);			// 文字列分割変換(float)処理
	static std::vector<std::string> split(std::string& input, char delimiter);				// 文字列分割処理
	static float GetDistance(D3DXVECTOR3 FirstTarget, D3DXVECTOR3 SecondTarget);			// 2点間の距離を取得
	static int GetRand(int nValue);															// ランダム整数の取得

	static float easeOut(float fValue);														// イージング(Out)
	static float easeIn(float fTime, float fStart, float fDifference, float fTotal);		// イージング(In)
	static float easeInAndOut(float fTime, float fStart, float fDifference, float fTotal);	// イージング(InOut)

	static HRESULT Load(std::string Add);													// テクスチャのロード
	static LPDIRECT3DTEXTURE9 GetResource(std::string Add);									// テクスチャの取得

	static HRESULT LoadModel(std::string &Add);												// Xファイルのロード
	static bool GetModelResource(std::string &Add, LPD3DXBUFFER &pBuffMat, DWORD &nNumMat, LPD3DXMESH &pMesh);	// Xファイルの取得

	static HRESULT LoadShader(std::string &Add);											// Xファイルのロード
	static LPD3DXEFFECT GetShaderResource(std::string &Add);								// Xファイルの取得

	static D3DXVECTOR3 GetCursorPosWithCenter(void);

	static std::map<std::string, MODEL_INFO> GetModelMap(void) { return m_ModelMap; }
	static std::vector<std::vector<std::string>> FileContens(char const * cns_cFile, char delimiter);

private:
	static void	LoadSystemFile(void);					// システムファイルのロード
	static void TexRelease(void);						// テクスチャの開放
	static void ModelRelease(void);						// モデルデータの開放
	static void ShaderRelease(void);					// シェーダーデータの開放

	static MODE m_mode;									// モード
	static CRenderer *m_pRenderer;						// レンダラーのポインタ
	static CInputKeyboard *m_pInputKeyboard;			// キーボードのポインタ
	static CInputMouse *m_pInputMouse;					// マウスのポインタ
	static CInputController *m_pInputController;		// コントローラーのポインタ

	static CCamera *m_pCamera;							// カメラのポインタ
	static CLight *m_pLight;							// ライトのポインタ
	static CNetwork *m_pNetwork;						// ネットワークのポインタ

	static CGame *m_pGame;								// ゲームのポインタ
	static CTitle *m_pTitle;							// タイトルのポインタ
	static CPuzzle *m_pPuzzle;							// パズルのポインタ
	static CResult *m_pResult;							// リザルトのポインタ
	static CRanking *m_pRanking;						// ランキングのポインタ

	static CSound *m_pSound;							// サウンドのポインタ

	static std::map<std::string, LPDIRECT3DTEXTURE9> m_TexMap;			// テクスチャマップ
	static std::map<std::string, MODEL_INFO> m_ModelMap;				// モデルマップ
	static std::map<std::string, LPD3DXEFFECT> m_ShaderMap;				// シェーダーマップ
};
#endif