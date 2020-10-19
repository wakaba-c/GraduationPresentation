//=============================================================================
//
// カメラ処理 [camera.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _CAMERA_H_
#define _CAMERA_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define SPLIT 0.12f;								//1フレームあたりの代入数

//=============================================================================
// 前方宣言
//=============================================================================
class CPlayer;

//=============================================================================
// クラス定義
//=============================================================================
class CCamera
{
public:
	CCamera();						// コンストラクタ
	~CCamera();						// デストラクタ
	HRESULT Init(void);				// 初期化処理
	void Uninit(void);				// 開放処理
	void Update(void);				// 更新処理

	void SetCamera(void);															// カメラの設定
	void SetRotation(D3DXVECTOR3 rot);
	void SetPosition(D3DXVECTOR3 pos);												// カメラ配置処理
	void SetDistance(float fDistance);												// 視点と注視点との距離を設定
	bool VFCulling(D3DXVECTOR3 pos, int nType, float fAngle, float fNearClip, float fFarClip);	// 視錐台カリング処理
	void SetPosCamera(D3DXVECTOR3 pos, D3DXVECTOR3 rot);							// カメラの位置設定
	HRESULT SetViewport(D3DXVECTOR2 pos, D3DXVECTOR2 size);							// 描画領域を設定

	D3DXVECTOR3 GetPosition(void) { return m_originPos; }							// カメラの場所取得
	float GetDistance(void) { return m_fDistance; }									// 視点と注視点との距離を取得
	D3DXVECTOR3 GetRotation(void) { return m_rot; }									// カメラ の回転を取得
	D3DXVECTOR3 GetWorldPos(void) { return m_worldPos; }							// マウス のワールド座標取得
	D3DXMATRIX GetMtxProjection(void) { return m_mtxProjection; }					// プロジェクションマトリックスの取得
	D3DXMATRIX GetMtxView(void) { return m_mtxView; }								// ビューマトリックスの取得
	D3DXVECTOR3 GetPosR(void) { return m_posR; }									// 注視点取得
	D3DXVECTOR3 GetPosV(void) { return m_posV; }									// 注視点取得

private:
#ifdef _DEBUG
	LPD3DXFONT			m_pCameraFont = NULL;										// フォントへのポインタ
	void Debug(void);				// デバッグ処理
#endif
	D3DXVECTOR3 CalcScreenToXZ(float fSx, float fSy, int nScreen_w, int nScreen_h, D3DXMATRIX* mView, D3DXMATRIX* mPrj);	// スクリーンの交点
	D3DXVECTOR3* CalcScreenToWorld(D3DXVECTOR3* pout, float fSx, float fSy, float fZ, int nScreen_w, int nScreen_h, D3DXMATRIX* mView, D3DXMATRIX* mPrj);		// ワールド座標変換
	void CameraMove(void);			// カメラ移動処理

	D3DXVECTOR3	m_posV;				// 視点
	D3DXVECTOR3 m_posR;				// 注視点
	D3DXVECTOR3 m_vecU;				// 上方向ベクトル
	D3DXVECTOR3 m_posRDest;			// 目的 の注視点
	D3DXVECTOR3 m_posVDest;			// 目的 の視点
	D3DXVECTOR3 m_rot;				// 回転
	D3DXVECTOR3 m_rotDest;			// 回転 の目標

	D3DXMATRIX	m_mtxProjection;	// プロジェクションマトリックス
	D3DXMATRIX	m_mtxView;			// ビューマトリックス
	float		m_fDistance;		// 距離
	bool		m_bRotMove;			// 回転運動の更新

	D3DXVECTOR3 m_worldPos;			// マウス のワールド座標

	D3DLIGHT9 m_aLight[3];			// ライト の情報
	int m_nType;					// ライト の種類

	D3DXVECTOR3	m_originPos;		// カメラ の位置

	D3DXVECTOR2 m_mousePosOld;		// 前回の マウス座標

	int m_nCntRot;		// カメラの回転開始カウンタ
};
#endif
