//=============================================================================
//
// レンダラー処理 [renderer.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _RENDERER_H_
#define _RENDERER_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// マクロ定義
//=============================================================================
// ２Ｄポリゴン頂点フォーマット( 頂点座標[2D] / 頂点カラー / テクスチャ座標 )
#define	FVF_VERTEX_2D	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
// ３Ｄポリゴン頂点フォーマット( 頂点座標[3D] / 頂点カラー / テクスチャ座標 )
#define	FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//=============================================================================
// 構造体定義
//=============================================================================
// ２Ｄポリゴン頂点フォーマットに合わせた構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点座標
	float rhw;			// 座標変換用係数(1.0fで固定)
	D3DCOLOR col;		// 頂点カラー
	D3DXVECTOR2 tex;	// テクスチャ座標
} VERTEX_2D;

// ３Ｄポリゴン頂点フォーマットに合わせた構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点座標
	D3DXVECTOR3 nor;	//法線ベクトル
	D3DCOLOR	col;	//頂点カラー
	D3DXVECTOR2 tex;
} VERTEX_3D;

//=============================================================================
// 前方宣言
//=============================================================================
class CDebugProc;

class CFade;
class CStage;

//=============================================================================
// クラス定義
//=============================================================================
class CRenderer
{
public:
	CRenderer();													// コンストラクタ
	~CRenderer();													// デストラクタ
	HRESULT Init(HINSTANCE hInstance, HWND hWnd, bool bWindow);		// 初期化処理
	void Uninit(void);												// 開放処理
	void Update(void);												// 更新処理
	void Draw(void);												// 描画処理
	void SetUpdate(bool bValue);									// 更新処理の有無切り替え
	void RendererRay(D3DXVECTOR3 start, D3DXVECTOR3 end);			// レイの描画

	LPDIRECT3DDEVICE9 GetDevice(void) { return m_pD3DDevice; }		// デバイスの取得

private:
	LPDIRECT3D9					m_pD3D = NULL;						// Direct3Dオブジェクトへのポインタ
	LPDIRECT3DDEVICE9			m_pD3DDevice = NULL;				// Direct3Dデバイスへのポインタ(描画に必要)
	LPDIRECT3DTEXTURE9			m_pTextureBG = NULL;				// テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9		m_pVtxBuffBG = NULL;				// 頂点バッファへのポインタ

	CFade *m_pFade;					// フェードのポインタ
	CStage *m_pStage;				// ステージのポインタ
	bool m_bWireFrame;				// ワイヤーフレームの有無
	bool m_bUpdate;					// 更新処理の有無

	void Debug(void);				// デバッグ処理
	static CDebugProc *m_pDebug;	// デバッグのポインタ
};
#endif