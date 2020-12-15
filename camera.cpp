//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "camera.h"
#include "manager.h"
#include "renderer.h"
#include "inputKeyboard.h"
#include "inputMouse.h"
#include "enemy.h"
#include "meshField.h"
#include "game.h"
#include "player.h"
#include "inputController.h"
#include "takaseiLibrary.h"
#include "speed.h"

//=============================================================================
// 静的メンバ変数の初期化
//=============================================================================

//=============================================================================
// マクロ定義
//=============================================================================
#define ROT_SHRINK 0.1f				// 回転の縮める速度
#define posV_Height 200.0f			// 視点の高さ
#define posR_Height 50.0f			// 注視点の高さ
#define posR_Length 50.0f			// モデルと注視点の位置
#define ROT_COUNT 6					// 回転を始めるカウント
#define DISTANCE 500				// 視点と注視点の距離

//=============================================================================
// コンストラクタ
//=============================================================================
CCamera::CCamera()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CCamera::~CCamera()
{

}

//==============================================================================
// 初期化処理
//==============================================================================
HRESULT CCamera::Init(void)
{
	CRenderer *Renderer = CManager::GetRenderer();				// レンダラーの取得
	LPDIRECT3DDEVICE9 pDevice;									// デバイス
	D3DXVECTOR3		originPos;									// 位置

	pDevice = Renderer->GetDevice();							// デバイスの取得
	m_worldPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);					// マウスのワールド座標
	m_bStorker = true;											// プレイヤー追尾
	m_bSmooth = false;											// スムーズ移動
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);						// 回転量
	m_rotDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);					// 回転量の目的地
	m_posV = D3DXVECTOR3(0.0f, 500.0f, -316.0f);				// 視点
	m_posVDest = m_posV;										// 視点の目的地
	m_posR = D3DXVECTOR3(0.0f, 100.0f, 0.0f);					// 注視点
	m_posRDest = m_posR;										// 注視点の目的地
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);						// 上方向ベクトル
	m_target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);					// 最終加算量
	m_currentRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 現在の加算値
	m_rotMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);					// 1フレーム当たりの回転量
	m_originPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// カメラの位置
	originPos = m_posR - m_posV;								// カメラ位置 の代入
	m_fDistance = sqrtf(originPos.y * originPos.y + originPos.z * originPos.z + 50);	// 距離 の算出
	m_nWatchingId = 0;											// 観戦するID
	m_move = D3DXVECTOR3_ZERO;

	// 描画領域 の設定
	SetViewport(D3DXVECTOR2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2),
		D3DXVECTOR2(SCREEN_WIDTH, SCREEN_HEIGHT));

#ifdef _DEBUG
	m_nType = 0;	// モデルタイプ

					// デバッグ表示用フォントを設定
	D3DXCreateFont(pDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "ＭＳ ゴシック", &m_pCameraFont);
#endif

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CCamera::Uninit(void)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CCamera::Update(void)
{
	D3DXVECTOR3 Calculation;					// 計算用
	D3DXVECTOR3 fDiff;							// 計算用格納変数

	D3DXVECTOR3 pos = D3DXVECTOR3_ZERO;
	D3DXVECTOR3 rot = D3DXVECTOR3_ZERO;

	Calculation.y = m_rotDest.y - m_rot.y;		// 現在の値と目的の値の差 の算出

	if (Calculation.y > D3DX_PI)
	{// 回転量 が既定値を超えたとき
		m_rotDest.y -= D3DX_PI * 2;				// 回転の補正
	}
	else if (Calculation.y < -D3DX_PI)
	{// 回転量 が既定値を超えたとき
		m_rotDest.y += D3DX_PI * 2;				// 回転の補正
	}

	if (fabsf(Calculation.y) < 0.0f)
	{// 計算の値 が既定値を超えたとき
		Calculation.y = m_rotDest.y;			// 計算の値を補正
	}

	if (CManager::GetMode() == CManager::MODE_GAME)
	{
		if (m_bStorker)
		{// プレイヤー追尾が許可されているとき
			// プレイヤー取得
			CPlayer *pPlayer = CGame::GetPlayer();

			if (pPlayer != NULL)
			{// プレイヤーが存在していたとき
				pos = pPlayer->GetPosition();
				rot = pPlayer->GetCameraRot();
				pos.y += 120.0f;								// プレイヤーの位置にオフセットを加算
				m_originPos = pos;
				//m_rot = rot;								// 回転目標値を設定
			}
		}
		else
		{
			// 位置更新
			m_originPos += m_move;

			// 減速
			m_move.x += (0 - m_move.x) * 0.12f;
			m_move.y += (0 - m_move.y) * 0.12f;
			m_move.z += (0 - m_move.z) * 0.12f;
		}
	}

	// 回転の最終目的地
	m_rotDest.y = rot.y;

	// 回転情報の差を格納
	fDiff.y = m_rot.y - m_rotDest.y;

	// 回転の補正
	CTakaseiLibrary::RotRevision(&fDiff);

	//カメラの位置計算
	m_posVDest.x = m_originPos.x + sinf(D3DX_PI + m_rot.y) * cosf(D3DX_PI + m_rot.x) * m_fDistance;
	m_posVDest.y = m_originPos.y + sinf(D3DX_PI + m_rot.x) * m_fDistance;
	m_posVDest.z = m_originPos.z + cosf(D3DX_PI + m_rot.y) * cosf(D3DX_PI + m_rot.x) * m_fDistance;

	m_posRDest.x = m_originPos.x + cosf(D3DX_PI + m_rot.y) * cosf(D3DX_PI + m_rot.x);
	m_posRDest.y = m_originPos.y + sinf(D3DX_PI + m_rot.x);
	m_posRDest.z = m_originPos.z + sinf(D3DX_PI + m_rot.y) * cosf(D3DX_PI + m_rot.x);

	if (m_bSmooth)
	{
		//カメラの位置適応
		m_posV += (m_posVDest - m_posV) * SPLIT;
		m_posR += (m_posRDest - m_posR) * SPLIT;

		m_posVDest = m_originPos + m_rot;
	}
	else
	{
		//カメラの位置適応
		m_posV = m_posVDest;
		m_posR = m_posRDest;
	}

	if (m_bStorker)
	{// フォーカスをあわせるとき
		// 差を徐々に縮めていく
		m_rot.y -= fDiff.y * ROT_SHRINK;
	}
	else
	{
		// カメラ単体操作
		CameraMove();
	}

	//回転数のリセット
	if (m_rot.y < -D3DX_PI)
	{
		m_rot.y += D3DX_PI * 2;
	}
	else if (m_rot.y > D3DX_PI)
	{
		m_rot.y -= D3DX_PI * 2;
	}

	// マウスの取得
	CInputMouse *pMouse = CManager::GetInputMouse();

	if (pMouse != NULL)
	{// マウスが存在していたとき
		D3DXVECTOR2 mousePos;									// マウス座標

		// スクリーン座標とXZ平面のワールド座標交点算出
		m_worldPos = CalcScreenToXZ((float)pMouse->GetMouseX(), (float)pMouse->GetMouseY(), SCREEN_WIDTH, SCREEN_HEIGHT, &m_mtxView, &m_mtxProjection);
	}

#ifdef _DEBUG
	//デバッグ処理
	Debug();
#endif
}

//=============================================================================
// 設定処理
//=============================================================================
void CCamera::SetCamera(void)
{
	CRenderer *Renderer = CManager::GetRenderer();		// レンダラーの取得
	LPDIRECT3DDEVICE9 pDevice;							// デバイス
	pDevice = Renderer->GetDevice();					// デバイスの取得

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(
		&m_mtxProjection,
		D3DXToRadian(45.0f + (25 * CManager::easeIn((CSpeed::GetSpeed() * 0.5f) / 25, 0.0f, 1.0f, 1.0f))),							// 視野角
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,		// アスペクト比
		1.0f,											// NearZ値
		1500000.0f);									// FarZ値

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	// ビューマトリックスを作成
	D3DXMatrixLookAtLH(
		&m_mtxView,
		&m_posV,										// カメラの視点
		&m_posR,										// カメラの注視点
		&m_vecU);										// カメラの上方向ベクトル

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);
}

//=============================================================================
// 回転量の設定
//=============================================================================
void CCamera::SetRotation(D3DXVECTOR3 rot)
{
	m_rot = rot;
}

//==============================================================================
// カメラ配置処理
//==============================================================================
void CCamera::SetPosition(D3DXVECTOR3 pos)
{
	m_originPos = pos;
}

//=============================================================================
// 距離の設定処理
//=============================================================================
void CCamera::SetDistance(float fDistance)
{
	m_fDistance = fDistance;
}

//=============================================================================
// 視錐台カリング
//=============================================================================
bool CCamera::VFCulling(D3DXVECTOR3 pos, int nType, float fAngle, float fNearClip, float fFarClip)
{
	D3DXPLANE VFLeftPlane, VFRightPlane, VFTopPlane, VFBottomPlane;
	D3DXVECTOR3 Pos = pos;
	float fRadius = 0.0f;
	float fAspect = SCREEN_WIDTH / SCREEN_HEIGHT;				// アスペクト比

																// タイプ別半径の設定
	switch (nType)
	{
	case CScene::PRIORITY_ENEMY:
		fRadius = 50.0f;
		break;
	case CScene::PRIORITY_FLOOR:
		fRadius = 5000.0f;
		break;
	case CScene::PRIORITY_MESH:
		fRadius = 1800.0f;
		break;
	case CScene::PRIORITY_MODEL:
		fRadius = 2500.0f;
		break;
	case CScene::PRIORITY_ORBIT:
		fRadius = 1800.0f;
		break;
	case CScene::PRIORITY_PLAYER:
		fRadius = 200.0f;
		break;
	case CScene::PRIORITY_TREE:
		fRadius = 2500.0f;
		break;
	}

	// ジオメトリの位置ベクトルをワールドからビュー空間に変換
	D3DXVec3TransformCoord(&Pos, &Pos, &m_mtxView);

	// 左右上下の平面を計算
	D3DXVECTOR3 p1, p2, p3;
	// 左面
	p1 = D3DXVECTOR3(0, 0, 0);
	p2.x = -fFarClip * ((FLOAT)tan(fAngle / 2) * fAspect);
	p2.y = -fFarClip * (FLOAT)tan(fAngle / 2);
	p2.z = fFarClip;
	p3.x = p2.x;
	p3.y = -p2.y;
	p3.z = p2.z;
	D3DXPlaneFromPoints(&VFLeftPlane, &p1, &p2, &p3);
	// 右面
	p1 = D3DXVECTOR3(0, 0, 0);
	p2.x = fFarClip * ((FLOAT)tan(fAngle / 2) * fAspect);
	p2.y = fFarClip * (FLOAT)tan(fAngle / 2);
	p2.z = fFarClip;
	p3.x = p2.x;
	p3.y = -p2.y;
	p3.z = p2.z;
	D3DXPlaneFromPoints(&VFRightPlane, &p1, &p2, &p3);
	// 上面
	p1 = D3DXVECTOR3(0, 0, 0);
	p2.x = -fFarClip * ((FLOAT)tan(fAngle / 2) * fAspect);
	p2.y = fFarClip * (FLOAT)tan(fAngle / 2);
	p2.z = fFarClip;
	p3.x = -p2.x;
	p3.y = p2.y;
	p3.z = p2.z;
	D3DXPlaneFromPoints(&VFTopPlane, &p1, &p2, &p3);
	// 下面
	p1 = D3DXVECTOR3(0, 0, 0);
	p2.x = fFarClip * ((FLOAT)tan(fAngle / 2) * fAspect);
	p2.y = -fFarClip * (FLOAT)tan(fAngle / 2);
	p2.z = fFarClip;
	p3.x = -p2.x;
	p3.y = p2.y;
	p3.z = p2.z;
	D3DXPlaneFromPoints(&VFBottomPlane, &p1, &p2, &p3);

	// 6つの平面とジオメトリ境界球をチェック
	// ニアクリップ面
	if ((Pos.z + fRadius) < fNearClip)
	{
		return false;
	}
	// ファークリップ面
	if ((Pos.z - fRadius) > fFarClip)
	{
		return false;
	}
	// 左クリップ面
	FLOAT Distance = (Pos.x * VFLeftPlane.a) + (Pos.z * VFLeftPlane.c);
	if (Distance > fRadius)
	{
		return false;
	}
	// 右クリップ面
	Distance = (Pos.x * VFRightPlane.a) + (Pos.z * VFRightPlane.c);
	if (Distance > fRadius)
	{
		return false;
	}
	// 上クリップ面
	Distance = (Pos.y * VFTopPlane.b) + (Pos.z * VFTopPlane.c);
	if (Distance > fRadius)
	{
		return false;
	}
	// 下クリップ面について
	Distance = (Pos.y * VFBottomPlane.b) + (Pos.z * VFBottomPlane.c);
	if (Distance > fRadius)
	{
		return false;
	}

	return true;
}

//=============================================================================
// カメラの視点変更
//=============================================================================
void CCamera::SetPosCamera(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	m_originPos = pos;					// 位置を設定
	m_originPos.y += 120;				// プレイヤーの高さを考慮

	m_rotDest = rot;					// 回転目標値を設定
	m_rot = rot;						// 回転値を設定

										// カメラの位置計算
	m_posVDest.x = m_originPos.x + sinf(D3DX_PI + m_rot.y) * cosf(D3DX_PI + m_rot.x) * m_fDistance;
	m_posVDest.y = m_originPos.y + sinf(D3DX_PI + m_rot.x) * m_fDistance;
	m_posVDest.z = m_originPos.z + cosf(D3DX_PI + m_rot.y) * cosf(D3DX_PI + m_rot.x) * m_fDistance;

	m_posRDest.x = m_originPos.x + cosf(D3DX_PI + m_rot.y) * cosf(D3DX_PI + m_rot.x);
	m_posRDest.y = m_originPos.y + sinf(D3DX_PI + m_rot.x);
	m_posRDest.z = m_originPos.z + sinf(D3DX_PI + m_rot.y) * cosf(D3DX_PI + m_rot.x);

	// カメラの位置適応
	m_posV = m_posVDest;
	m_posR = m_posRDest;
}

//==============================================================================
// 描画領域を設定
//==============================================================================
HRESULT CCamera::SetViewport(D3DXVECTOR2 pos, D3DXVECTOR2 size)
{
	CRenderer *Renderer = CManager::GetRenderer();																// レンダラーの取得
	LPDIRECT3DDEVICE9 pDevice;																					// デバイス
	pDevice = Renderer->GetDevice();																			// デバイスの取得

	D3DVIEWPORT9 vp;																							// 描画領域

	vp.X = (DWORD)(pos.x - size.x / 2);																			// X軸の位置設定
	vp.Y = (DWORD)(pos.y - size.y / 2);																			// Y軸の位置設定
	vp.Width = (DWORD)size.x;																					// X軸の大きさ設定
	vp.Height = (DWORD)size.y;																					// Y軸の大きさ設定
	vp.MinZ = 0;
	vp.MaxZ = 1;

	if (FAILED(pDevice->SetViewport(&vp)))
	{// 描画領域 の設定 に失敗したとき
		MessageBox(NULL, "ビューポート切り替え失敗", "エラー", MB_OK);												// エラーウィンドウの作成
		return E_FAIL;
	}
	return S_OK;
}

void CCamera::Test(int nTest = 0)
{
}

//==============================================================================
// XZ平面とスクリーン座標の交点算出関数
//==============================================================================
D3DXVECTOR3 CCamera::CalcScreenToXZ(float fSx, float fSy, int nScreen_w, int nScreen_h, D3DXMATRIX* mView, D3DXMATRIX* mPrj)
{
	D3DXVECTOR3 Answer;
	D3DXVECTOR3 nearpos;
	D3DXVECTOR3 farpos;
	D3DXVECTOR3 ray;
	CalcScreenToWorld(&nearpos, fSx, fSy, 0.0f, nScreen_w, nScreen_h, mView, mPrj);
	CalcScreenToWorld(&farpos, fSx, fSy, 1.0f, nScreen_w, nScreen_h, mView, mPrj);
	ray = farpos - nearpos;
	D3DXVec3Normalize(&ray, &ray);

	if (ray.y <= 0)
	{// 床との交差が起きている場合は交点を
	 // 床交点
		float Lray = D3DXVec3Dot(&ray, &D3DXVECTOR3(0, 1, 0));
		float LP0 = D3DXVec3Dot(&(-nearpos), &D3DXVECTOR3(0, 1, 0));
		Answer = nearpos + (LP0 / Lray)*ray;
	}
	else
	{// 起きていない場合は遠くの壁との交点を出力
		Answer = farpos;
	}

	return Answer;
}

//==============================================================================
// スクリーン座標をワールド座標に変換
//==============================================================================
D3DXVECTOR3* CCamera::CalcScreenToWorld(D3DXVECTOR3* pout, float fSx, float fSy, float fZ, int nScreen_w, int nScreen_h, D3DXMATRIX* mView, D3DXMATRIX* mPrj)
{
	// 各行列の逆行列を算出
	D3DXMATRIX InvView, InvPrj, VP, InvViewport;
	D3DXMatrixInverse(&InvView, NULL, mView);
	D3DXMatrixInverse(&InvPrj, NULL, mPrj);
	D3DXMatrixIdentity(&VP);
	VP._11 = nScreen_w / 2.0f; VP._22 = -nScreen_h / 2.0f;
	VP._41 = nScreen_w / 2.0f; VP._42 = nScreen_h / 2.0f;
	D3DXMatrixInverse(&InvViewport, NULL, &VP);

	// 逆変換
	D3DXMATRIX tmp = InvViewport * InvPrj * InvView;
	D3DXVec3TransformCoord(pout, &D3DXVECTOR3(fSx, fSy, fZ), &tmp);

	return pout;
}

//=============================================================================
// カメラの操作
//=============================================================================
void CCamera::CameraMove(void)
{
	if (CManager::GetMode() == CManager::MODE_GAME)
	{// モードがゲームだったとき
		CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();		// キーボードの取得
		CInputMouse *pMouse = CManager::GetInputMouse();				// マウスの取得
		CInputController *pGamepad = CManager::GetInputController();	// ゲームパッドの取得

		D3DXVECTOR2 mousePos;									// マウス座標

		float nValueH = 0;									//コントローラー
		float nValueV = 0;									//コントローラー

		if (pGamepad != NULL)
		{// ゲームパッドが存在していたとき
			if (pGamepad->GetJoypadUse(0))
			{// 使用可能だったとき
			 // 右スティックの角度を取得
				pGamepad->GetJoypadStickRight(0, &nValueH, &nValueV);

				// カメラが指定の範囲外だったとき
				if (m_rot.x < -0.7f)
				{
					// 最大値まで戻す
					m_rot.x = -0.7f;
				}
				else if (m_rot.x > 0.1f)
				{
					// 最小値まで戻す
					m_rot.x = 0.1f;
				}

				// カメラの回転
				// カメラの縦に旋回をする
				m_rot.x += nValueV * 0.05f;

				// カメラを横に旋回する
				m_rot.y -= nValueH * 0.05f;
			}
		}

		// スクリーン座標とXZ平面のワールド座標交点算出
		m_worldPos = CalcScreenToXZ((float)pMouse->GetMouseX(), (float)pMouse->GetMouseY(), SCREEN_WIDTH, SCREEN_HEIGHT, &m_mtxView, &m_mtxProjection);

		// 左Altキーが押さているとき
		if (pKeyboard->GetPressKeyboard(DIK_LALT))
		{
			// 左ボタンが押されたとき
			if (pMouse->GetTriggerMouse(MOUSE_LEFT))
			{
				//現在のマウス座標を取得
				m_mousePosOld.x = (float)pMouse->GetMouseX();
				m_mousePosOld.y = (float)pMouse->GetMouseY();
			}
			else if (pMouse->GetTriggerMouse(MOUSE_RIGHT))
			{// 右ボタンが押されたとき

			 // 現在のマウス座標を取得
				m_mousePosOld.y = (float)pMouse->GetMouseY();
			}
			else if (pMouse->GetTriggerMouse(MOUSE_CENTER))
			{// 中ボタンが押されたとき
			 // 現在のマウス座標を取得
				m_mousePosOld.x = (float)pMouse->GetMouseX();
				m_mousePosOld.y = (float)pMouse->GetMouseY();
			}
			else if (pMouse->GetPressMouse(MOUSE_LEFT))
			{// 左ボタンが押されている間
			 // マウス座標の取得
				mousePos.x = (float)pMouse->GetMouseX();
				mousePos.y = (float)pMouse->GetMouseY();

				// 前回の位置との差分を求める
				D3DXVECTOR2 mouseMove = mousePos - m_mousePosOld;

				// 差分回転させる
				m_rot.x -= mouseMove.y * 0.01f;
				m_rot.y += mouseMove.x * 0.01f;

				// 古い情報を更新する
				m_mousePosOld = mousePos;
			}
			else if (pMouse->GetPressMouse(MOUSE_RIGHT))
			{// 右ボタンが押されている間
			 // マウス座標の取得
				mousePos.y = (float)pMouse->GetMouseY();

				// 前回の位置との差分を求める
				D3DXVECTOR2 mouseMove = mousePos - m_mousePosOld;

				// 差分だけ更新する
				m_fDistance += mouseMove.y;

				// 古い情報を更新する
				m_mousePosOld = mousePos;
			}
			else if (pMouse->GetPressMouse(MOUSE_CENTER))
			{// 中ボタンが押されている間
			 // マウス座標の取得
				mousePos.x = (float)pMouse->GetMouseX();
				mousePos.y = (float)pMouse->GetMouseY();

				// 前回の位置との差分を求める
				D3DXVECTOR2 work = mousePos - m_mousePosOld;

				//// カメラのY方向移動
				//m_originPos.x -= sinf(D3DX_PI * 1.0f) * work.y;
				//m_originPos.y -= cosf(D3DX_PI * 1.0f) * work.y;

				//// カメラのX方向移動
				//m_originPos.x += sinf(D3DX_PI * 0.5f + m_rot.y) * work.x * 0.5f;
				//m_originPos.y += cosf(D3DX_PI * 0.5f + m_rot.y) * work.x * 0.5f;

				m_originPos.x += work.x;
				m_originPos.y = work.y;

				SetPosCamera(m_originPos, m_rot);

				// 古い情報を更新する
				m_mousePosOld = mousePos;
			}
		}

		if (pKeyboard->GetPressKeyboard(DIK_LSHIFT))
		{// 左シフトキーが押されたとき
			m_move.y -= NORMAL_SPEED;
		}

		if (pKeyboard->GetPressKeyboard(DIK_SPACE))
		{// スペースキーが押されたとき
			m_move.y += NORMAL_SPEED;
		}

		//左右操作
		if (pKeyboard->GetPressKeyboard(DIK_A))
		{
			m_move.x += sinf(D3DX_PI * 0.5f + m_rot.y) * NORMAL_SPEED;
			m_move.z += cosf(D3DX_PI * 0.5f + m_rot.y) * NORMAL_SPEED;
		}
		if (pKeyboard->GetPressKeyboard(DIK_D))
		{
			m_move.x += sinf(-D3DX_PI * 0.5f + m_rot.y) * NORMAL_SPEED;
			m_move.z += cosf(-D3DX_PI * 0.5f + m_rot.y) * NORMAL_SPEED;
		}

		//上下操作
		if (pKeyboard->GetPressKeyboard(DIK_W))
		{
			m_move.x += sinf(D3DX_PI * 1.0f + m_rot.y) * NORMAL_SPEED;
			m_move.z += cosf(D3DX_PI * 1.0f + m_rot.y) * NORMAL_SPEED;

		}
		if (pKeyboard->GetPressKeyboard(DIK_S))
		{
			m_move.x += sinf(D3DX_PI * 0.0f + m_rot.y) * NORMAL_SPEED;
			m_move.z += cosf(D3DX_PI * 0.0f + m_rot.y) * NORMAL_SPEED;
		}
	}
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CCamera::Debug(void)
{
	ImGui::Begin("Camera");									// 「camera」というウィンドウを作成しますそれに追加します。

	ImGui::Text("Camera Setting");							// いくつかのテキストを表示します（フォーマット文字列も使用できます）

	ImGui::LabelText("", "CameraV Pos:(%.2f, %.2f, %.2f )", m_posV.x, m_posV.y, m_posV.z);
	ImGui::LabelText("", "CameraR Pos:(%.2f, %.2f, %.2f )", m_posR.x, m_posR.y, m_posR.z);
	ImGui::LabelText("", "Distance : %.2f", m_fDistance);
	ImGui::LabelText("", "Rot : %.2f, %.2f, %.2f", m_rot.x, m_rot.y, m_rot.z);

	if (ImGui::Button("Reset"))                            // ボタンはクリックするとtrueを返します（ほとんどのウィジェットは編集/アクティブ化するとtrueを返します）
	{
		m_posV = D3DXVECTOR3(-2061.0f, 1122.0f, 1904.0f);
	}

	if (ImGui::Button("Model"))                            // ボタンはクリックするとtrueを返します（ほとんどのウィジェットは編集/アクティブ化するとtrueを返します）
	{
		m_posV = D3DXVECTOR3(0.0f, 349.0f, 560.0f);
	}

	if (ImGui::Button("Debug"))                            // ボタンはクリックするとtrueを返します（ほとんどのウィジェットは編集/アクティブ化するとtrueを返します）
	{
		m_posV = D3DXVECTOR3(92.99f, 31153.32f, -967.5f);
		m_posR = D3DXVECTOR3(99.52f, 51.00f, 375.68f);
		m_rot = D3DXVECTOR3(-0.67f, 3.14f, 0.0f);
		m_fDistance = 31132.31f;
	}
	if (ImGui::Button("Player"))
	{
		m_posV = D3DXVECTOR3(100.52f, 50.00f, -227.31f);
		m_posR = D3DXVECTOR3(99.00f, 50.00f, 100.00f);
		m_rot = D3DXVECTOR3(-0.67f, 3.14f, 0.0f);
		m_fDistance = 327.31f;
	}

	ImGui::Checkbox(u8"フォーカス", &m_bStorker);

	ImGui::End();											//最後につける
}
#endif