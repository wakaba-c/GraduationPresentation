//=============================================================================
//
// エフェクト処理 [effect.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "effect.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "meshCapsule.h"
#include "meshSphere.h"
#include "meshCube.h"
#include "camera.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MANAGER_PARTICLE "data/text/manager/manager_particle.txt"

//=============================================================================
// 静的メンバ変数
//=============================================================================
CEffect *CEffect::m_pEffect[MAX_EFFECT] = {};
std::map<std::string, EFFECT> CEffect::m_effectMap = {};

//=============================================================================
// コンストラクタ
//=============================================================================
CEffect::CEffect(CScene::PRIORITY obj = CScene::PRIORITY_EFFECT) : CScene(obj)
{
	SetObjType(CScene::PRIORITY_EFFECT);								// オブジェクトタイプの設定

	/* =============== 値の初期化 =============== */
	m_pVtxBuff = NULL;													// 頂点バッファの初期化
	m_pTexture = NULL;													// テクスチャの初期化
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);								// 位置の初期化
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);							// カラーの初期化
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);								// 回転の初期化
	m_size = D3DXVECTOR3(5.0f, 5.0f, 0.0f);								// サイズの初期化
	m_move = D3DXVECTOR3(0.0f, 2.0f, 0.0f);								// 移動量の初期化
	m_moveSize = D3DXVECTOR3_ZERO;										// 大きさの変化量
	m_moveRot = D3DXVECTOR3_ZERO;										// 回転の変化量
	m_moveCol = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);						// 色の変化量
	m_nLife = 0;														// 耐久値の初期化
	m_bUse = false;														// 使用状態の初期化
	m_fGravity = 0.0f;													// 重力の初期化
	m_bZBuffer = true;													// Zバッファの参照有無
	m_bAlpha = false;													// 加算合成の有無初期化
	m_AnimCount = 0;													// アニメーションカウンターの初期化
	m_AnimPage = 0;														// アニメーションページの初期化
	m_nInterval = 0;													// インターバルの初期化
	m_fRotationSpeed = 0;												// 回転スピードの初期化
}

//=============================================================================
// デストラクタ
//=============================================================================
CEffect::~CEffect()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CEffect::Init(void)
{
	//デバイスを取得する
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// 頂点情報の作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX_3D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);

	//// 画像情報
	//m_aEffect[EFFECTTYPE_ROSE].sprite = D3DXVECTOR2(1.0f, 1.0f);		// 分割数
	//m_aEffect[EFFECTTYPE_ROSE].bAlpha = false;							// アルファブレンディングの有無

	//m_aEffect[EFFECTTYPE_SMOKE].sprite = D3DXVECTOR2(8.0f, 1.0f);		// 分割数
	//m_aEffect[EFFECTTYPE_SMOKE].bAlpha = false;							// アルファブレンディングの有無

	//m_aEffect[EFFECTTYPE_HALO].sprite = D3DXVECTOR2(1.0f, 1.0f);		// 分割数
	//m_aEffect[EFFECTTYPE_HALO].bAlpha = false;							// アルファブレンディングの有無

	//m_aEffect[EFFECTTYPE_SANDSMOKE].sprite = D3DXVECTOR2(8.0f, 1.0f);	// 分割数
	//m_aEffect[EFFECTTYPE_SANDSMOKE].bAlpha = false;						// アルファブレンディングの有無

	//m_aEffect[EFFECTTYPE_SHOCKWAVE].sprite = D3DXVECTOR2(1.0f, 1.0f);	// 分割数
	//m_aEffect[EFFECTTYPE_SHOCKWAVE].bAlpha = false;						// アルファブレンディングの有無

	//m_aEffect[EFFECTTYPE_STAR].sprite = D3DXVECTOR2(1.0f, 1.0f);		// 分割数
	//m_aEffect[EFFECTTYPE_STAR].bAlpha = false;							// アルファブレンディングの有無

	//m_aEffect[EFFECTTYPE_SLASH].sprite = D3DXVECTOR2(9.0f, 1.0f);		// 分割数
	//m_aEffect[EFFECTTYPE_SLASH].bAlpha = false;							// アルファブレンディングの有無

	//m_aEffect[EFFECTTYPE_SPHERE].sprite = D3DXVECTOR2(1.0f, 1.0f);		// 分割数
	//m_aEffect[EFFECTTYPE_SPHERE].bAlpha = true;							// アルファブレンディングの有無

	//m_aEffect[EFFECTTYPE_WING].sprite = D3DXVECTOR2(1.0f, 1.0f);		// 分割数
	//m_aEffect[EFFECTTYPE_WING].bAlpha = true;							// アルファブレンディングの有無

	//m_aEffect[EFFECTTYPE_AURA].sprite = D3DXVECTOR2(1.0f, 1.0f);		// 分割数
	//m_aEffect[EFFECTTYPE_AURA].bAlpha = true;							// アルファブレンディングの有無

	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);								// サイズの設定

	// 頂点情報の更新
	MakeVertex();

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CEffect::Uninit(void)
{
	// 頂点バッファの開放
	if (m_pVtxBuff != NULL)
	{// 頂点バッファが存在していたとき
		m_pVtxBuff->Release();		// 開放
		m_pVtxBuff = NULL;			// NULLを代入
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CEffect::Update(void)
{
	if (m_bUse)
	{
		float fSize = 1.0f;
		float fRate = (float)m_nLife / (float)m_nMaxLife;

		// 位置更新
		m_pos += m_move;

		// サイズの
		switch (m_easingType)
		{
		case EASINGTYPE_IN:
			fSize = CManager::easeIn(fRate, 0, 1, 1);
			break;
		case EASINGTYPE_OUT:
			fSize = CManager::easeOut(fRate);
			break;
		}

		// 頂点情報の更新
		MakeVertex();

		m_size += m_moveSize * fSize;
		m_rot += m_moveRot;
		m_col += m_moveCol;
		SpriteAnimation(m_sprite, m_AnimPage, 0);

		// 減速
		m_move.x += (0 - m_move.x) * m_fResistance;
		m_move.y += (0 - m_move.y) * m_fResistance;
		m_move.z += (0 - m_move.z) * m_fResistance;

		if (fabs(m_fDistance) >= 1.0f)
		{
			m_fAngle += m_fRotationSpeed;

			// 通常時はこっちどす
			m_pos.x = m_centerPos.x + sinf(D3DX_PI * m_fAngle) * m_fDistance;
			m_pos.z = m_centerPos.z + cosf(D3DX_PI * m_fAngle) * m_fDistance;
		}

		// アニメーション処理
		if (m_nLife < m_nMaxLife)
		{
			Animation();
		}

		// 開放処理
		if (m_nLife < m_nMaxLife)
		{
			m_nLife++;					//ライフを削る

			//重力処理
			m_move.y += m_fGravity;
		}
		else
		{
			if (m_bFadeOut)
			{
				m_col.a -= 0.05f;

				if (m_col.a < 0.0f)
				{
					m_bUse = false;
				}
			}
			else
			{
				m_bUse = false;
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CEffect::Draw(void)
{
	if (m_bUse)
	{
		CRenderer *pRenderer = CManager::GetRenderer();
		LPDIRECT3DDEVICE9 pDevice;

		D3DXMATRIX	mtxRot, mtxTrans, mtxView;				//計算用マトリックス

		//デバイスを取得する
		pDevice = pRenderer->GetDevice();

		if (m_bAlpha)
		{// 加算合成を使用したとき
			// レンダーステート(加算合成処理)
			pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		}

		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);				// 裏面(左回り)をカリングする
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		if (!m_bZBuffer)
		{
			pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		}

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&m_mtxWorld);

		if (m_bBillboard)
		{
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			m_mtxWorld._11 = mtxView._11;
			m_mtxWorld._12 = mtxView._21;
			m_mtxWorld._13 = mtxView._31;
			m_mtxWorld._21 = mtxView._12;
			m_mtxWorld._22 = mtxView._22;
			m_mtxWorld._23 = mtxView._32;
			m_mtxWorld._31 = mtxView._13;
			m_mtxWorld._32 = mtxView._23;
			m_mtxWorld._33 = mtxView._33;
		}

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		//頂点フォーマットの設定
		pDevice->SetTexture(0, m_pTexture);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		// レンダーステート(通常ブレンド処理)
		pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

		// レンダーステート(通常ブレンド処理)
		pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		if (m_bAlpha)
		{// 加算合成を使用したとき
			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}

		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面(左回り)をカリングする

		//頂点フォーマットの設定
		pDevice->SetTexture(0, NULL);
	}
}

//=============================================================================
// クリエイト関数
//=============================================================================
void CEffect::Create(void)
{
	// エフェクトの作成
	for (int nCount = 0; nCount < MAX_EFFECT; nCount++)
	{
		m_pEffect[nCount] = new CEffect(PRIORITY_EFFECT);

		if (m_pEffect[nCount] != NULL)
		{// エフェクトが存在していたとき
			m_pEffect[nCount]->Init();					// 初期化処理
			m_pEffect[nCount]->SetUse(false);			// 使用状態の変更
		}
	}
}

//=============================================================================
// モデルのロード関数
//=============================================================================
HRESULT CEffect::Load(void)
{
	//m_pTexture[EFFECTTYPE_ROSE] = CManager::GetResource("data/tex/effect/rose_01.png");
	//m_pTexture[EFFECTTYPE_SMOKE] = CManager::GetResource("data/tex/effect/explosion002.png");
	//m_pTexture[EFFECTTYPE_SANDSMOKE] = CManager::GetResource("data/tex/effect/explosion001.png");
	//m_pTexture[EFFECTTYPE_HALO] = CManager::GetResource("data/tex/effect/Halo.png");
	//m_pTexture[EFFECTTYPE_SHOCKWAVE] = CManager::GetResource("data/tex/effect/Shockwave.png");
	//m_pTexture[EFFECTTYPE_STAR] = CManager::GetResource("data/tex/effect/Star.png");
	//m_pTexture[EFFECTTYPE_SLASH] = CManager::GetResource("data/tex/effect/Slashing.png");
	//m_pTexture[EFFECTTYPE_SPHERE] = CManager::GetResource("data/tex/effect/sphere.jpg");
	//m_pTexture[EFFECTTYPE_WING] = CManager::GetResource("data/tex/effect/wing.jpg");
	//m_pTexture[EFFECTTYPE_AURA] = CManager::GetResource("data/tex/effect/aura.png");

	return S_OK;
}

//=============================================================================
// エフェクトの位置を設定
//=============================================================================
void CEffect::SetPosition(D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//=============================================================================
// エフェクトの移動量を設定
//=============================================================================
void CEffect::SetMove(D3DXVECTOR3 move)
{
	m_move = move;
}

//=============================================================================
// 使用中かどうか設定する
//=============================================================================
void CEffect::SetUse(bool bValue)
{
	m_bUse = bValue;
}

//=============================================================================
// テクスチャ描画位置の更新
//=============================================================================
void CEffect::SpriteAnimation(D3DXVECTOR2 patternNo, int nPatternAnim, int nHeight)
{// 縦と横のパターン数と現在のページと行

	VERTEX_3D *pVtx;										// 頂点情報へのポインタ

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の更新
	pVtx[0].tex = D3DXVECTOR2(0.0f + nPatternAnim * 1.0f / patternNo.x, 0.0f + (1.0f / patternNo.y) * nHeight);
	pVtx[1].tex = D3DXVECTOR2(1.0f / patternNo.x + nPatternAnim * 1.0f / patternNo.x, 0.0f + (1.0f / patternNo.y) * nHeight);
	pVtx[2].tex = D3DXVECTOR2(0.0f + nPatternAnim * 1.0f / patternNo.x, 1.0f / patternNo.y + (1.0f / patternNo.y) * nHeight);
	pVtx[3].tex = D3DXVECTOR2(1.0f / patternNo.x + nPatternAnim * 1.0f / patternNo.x, 1.0f / patternNo.y + (1.0f / patternNo.y) * nHeight);

	// 頂点データのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
// エフェクトの使用
//=============================================================================
void CEffect::SetEffect(const std::string Add, D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 move,
	D3DXVECTOR3 moveSize, D3DXVECTOR3 moveRot, D3DXCOLOR moveCol, EASINGTYPE easingType, D3DXVECTOR3 rot, D3DXCOLOR col, int nLife,
	float fGravity, float fResistance, bool bBillboard, int nPatternAnim, int nHeight,
	D3DXVECTOR3 centerPos, float fAngle, float fDistance, float fRotationSpeed, D3DXVECTOR2 sprite, bool bAlpha, bool bZBuffer, bool bFadeOut)
{
	for (int nCount = 0; nCount < MAX_EFFECT; nCount++)
	{
		if (!m_pEffect[nCount]->m_bUse)
		{
			m_pEffect[nCount]->m_pTexture = CManager::GetResource(Add);				// テクスチャ情報の初期化
			m_pEffect[nCount]->m_pos = pos;											// 位置情報
			m_pEffect[nCount]->m_rot = rot;											// 回転情報
			m_pEffect[nCount]->m_move = move;										// 移動情報
			m_pEffect[nCount]->m_col = col;											// カラー
			m_pEffect[nCount]->m_nMaxLife = nLife;									// 生存時間の最大数
			m_pEffect[nCount]->m_nLife = 0;											// 生存時間
			m_pEffect[nCount]->m_fGravity = fGravity;								// 重力の使用
			m_pEffect[nCount]->m_AnimPage = nPatternAnim;							// ページ数
			m_pEffect[nCount]->m_size = size;										// 大きさ
			m_pEffect[nCount]->MakeVertex();										// 頂点情報の作成
			m_pEffect[nCount]->SpriteAnimation(sprite, nPatternAnim, nHeight);		// 描画領域の設定
			m_pEffect[nCount]->m_fAngle = fAngle;									// 角度
			m_pEffect[nCount]->m_fDistance = fDistance;								// 距離
			m_pEffect[nCount]->m_centerPos = centerPos;								// 中心位置の設定
			m_pEffect[nCount]->m_fResistance = fResistance;							// 抵抗
			m_pEffect[nCount]->m_fRotationSpeed = fRotationSpeed;					// 回転速度
			m_pEffect[nCount]->m_moveSize = moveSize;								// サイズの加算値
			m_pEffect[nCount]->m_moveRot = moveRot;									// 回転の変化量
			m_pEffect[nCount]->m_moveCol = moveCol;									// 色の変化量
			m_pEffect[nCount]->m_bBillboard = bBillboard;							// ビルボードの有無
			m_pEffect[nCount]->m_easingType = easingType;							// イージングの設定
			m_pEffect[nCount]->m_sprite = sprite;									// 画像の分割数
			m_pEffect[nCount]->m_bZBuffer = bZBuffer;								// Zバッファの参照
			m_pEffect[nCount]->m_bAlpha = bAlpha;									// 加算合成
			m_pEffect[nCount]->m_bFadeOut = bFadeOut;								// 重力の有無
			m_pEffect[nCount]->m_bUse = true;										// 使用中に設定
			break;
		}
	}
}

//=============================================================================
// 舞い散る花びらのエフェクト
//=============================================================================
void CEffect::FallingPetals(bool bGravity, D3DXVECTOR3 pos, int nLife)
{
	float fAngle = 0.0f;
	D3DXVECTOR3 particlePos = D3DXVECTOR3_ZERO;

	// 位置の計算
	particlePos = GetRandomPosWithSphere();
	// 角度
	fAngle = GetRandomAngle();

	D3DXVECTOR3 rot;
	rot = D3DXVECTOR3(sinf(fAngle) * 10, cosf(fAngle) * 10, 0.0f);			// 回転量の設定

	CEffect::SetEffect("data/tex/effect/rose_01.png",			// エフェクトタイプ
		pos,													// 発生位置
		D3DXVECTOR3(10.0f, 10.0f, 0.0f),						// サイズ
		particlePos * 5.0f,										// 移動方向
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// サイズの1フレーム当たりの加算値
		D3DXVECTOR3(0.02f, 0.02f, 0.0f),						// 回転の変化量
		D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),						// 色の変化量
		EASINGTYPE_NONE,										// イージングタイプ
		rot,													// テクスチャの回転
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),						// カラー
		nLife,													// ライフ
		bGravity,												// 重力
		0.025f,													// 抵抗
		true,													// ビルボード
		0,														// 表示する箇所(横)
		0,														// 表示する箇所(縦)
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 中心位置
		0,														// 角度
		0,														// 距離
		0,														// 回転速度
		D3DXVECTOR2(1.0f, 1.0f),								// 画像の分割数
		false,													// 加算合成の有無
		false,													// Zバッファの比較有無
		false													// フェード
	);
}

//=============================================================================
// 集まる花びらのエフェクト
//=============================================================================
void CEffect::PetalsGather(D3DXVECTOR3 pos)
{
	float fAngle = 0.0f;

	D3DXVECTOR3 particlePos;

	// 位置の計算
	particlePos = GetRandomPosWithSphere();

	// 角度
	fAngle = GetRandomAngle();		// ランダムに取得

	D3DXVECTOR3 rot;
	rot = D3DXVECTOR3(sinf(fAngle) * 10, cosf(fAngle) * 10, 0.0f);		// 回転量の設定

	CEffect::SetEffect("data/tex/effect/rose_01.png",							// エフェクトタイプ
		pos + particlePos * 200,								// 発生位置
		D3DXVECTOR3(10.0f, 10.0f, 0.0f),						// サイズ
		particlePos * -7,										// 移動方向
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// サイズの1フレーム当たりの加算値
		D3DXVECTOR3(0.02f, 0.02f, 0.0f),						// 回転の変化量
		D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),						// 色の変化量
		EASINGTYPE_NONE,										// イージングタイプ
		rot,													// テクスチャの回転
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),						// カラー
		30,														// ライフ
		false,													// 重力
		0.025f,													// 抵抗
		true,													// ビルボード
		0,														// 表示する箇所(横)
		0,														// 表示する箇所(縦)
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 中心位置
		0,														// 角度
		0,														// 距離
		0,														// 回転速度
		D3DXVECTOR2(1.0f, 1.0f),								// 画像の分割数
		false,													// 加算合成の有無
		false,													// Zバッファの比較有無
		false													// フェード
	);
}

//=============================================================================
// 花びらが舞い上がるエフェクト
//=============================================================================
void CEffect::PetalsSoaring(D3DXVECTOR3 pos)
{
	float fAngle = 0.0f;
	D3DXVECTOR3 particlePos;

	// 角度
	fAngle = GetRandomAngle();	// ランダムに取得

	// 回転
	D3DXVECTOR3 rot;
	rot = D3DXVECTOR3(sinf(fAngle) * 10, cosf(fAngle) * 10, 0.0f);		// 回転量の設定

	// 距離
	float fDistance = float(CManager::GetRand(200 * 100)) / 100;		// ランダムに取得
	fDistance = 50.0f + fDistance;										// 50加算する

	// 位置
	particlePos = pos;
	particlePos.x += sinf(D3DX_PI * fAngle) * fDistance;
	particlePos.z += cosf(D3DX_PI * fAngle) * fDistance;

	CEffect::SetEffect("data/tex/effect/rose_01.png",							// エフェクトタイプ
		particlePos,											// 発生位置
		D3DXVECTOR3(10.0f, 10.0f, 0.0f),						// サイズ
		D3DXVECTOR3(0.0f, 5.0f, 0.0f),							// 移動方向
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// サイズのフレーム当たりの加算値
		D3DXVECTOR3(0.02f, 0.02f, 0.0f),						// 回転の変化量
		D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),						// 色の変化量
		EASINGTYPE_NONE,										// イージングタイプ
		rot,													// テクスチャの回転
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),						// カラー
		250,													// ライフ
		false,													// 重力
		0.015f,													// 抵抗
		true,													// ビルボード
		0,														// 表示する箇所(横)
		0,														// 表示する箇所(縦)
		pos,													// 中心位置
		fAngle,													// 角度
		fDistance,												// 距離
		0.01f,													// 回転速度
		D3DXVECTOR2(1.0f, 1.0f),								// 画像の分割数
		false,													// 加算合成の有無
		false,													// Zバッファの比較有無
		false													// フェード
	);
}

//=============================================================================
// 羽が舞うエフェクト
//=============================================================================
void CEffect::WingStay(D3DXVECTOR3 pos)
{
	float fAngle = float(CManager::GetRand(314)) - float(CManager::GetRand(314));
	float fAngle_x = float(CManager::GetRand(314)) - float(CManager::GetRand(314));

	D3DXVECTOR3 particlePos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// 位置の計算
	particlePos.x = cosf(D3DX_PI + fAngle) * cosf(D3DX_PI + fAngle_x);
	particlePos.y = sinf(D3DX_PI + fAngle_x);
	particlePos.z = sinf(D3DX_PI + fAngle) * cosf(D3DX_PI + fAngle_x);

	// 角度
	fAngle = float(CManager::GetRand(314)) / 100.0f - float(CManager::GetRand(314)) / 100.0f;		// ランダムに取得

	// 回転
	D3DXVECTOR3 rot;
	rot = D3DXVECTOR3(sinf(fAngle) * 10, cosf(fAngle) * 10, 0.0f);		// ランダムに取得

	// 距離
	float fDistance = float(CManager::GetRand((int)350 * 100)) / 100;	// ランダムに取得
	fDistance = 50.0f + fDistance;										// 50加算する

	// スピード
	float fSpeed = float(CManager::GetRand((int)350 * 100)) / 100;		// ランダムに取得
	fSpeed = 50 + fSpeed;												// 50加算する

	CEffect::SetEffect("data/tex/effect/wing.jpg",							// エフェクトタイプ
		pos + particlePos * fDistance,							// 発生位置
		D3DXVECTOR3(5.0f, 5.0f, 0.0f),							// サイズ
		particlePos,											// 移動方向
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// サイズのフレーム当たりの加算値
		D3DXVECTOR3(0.02f, 0.02f, 0.0f),						// 回転の変化量
		D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),						// 色の変化量
		EASINGTYPE_NONE,										// イージングタイプ
		rot,													// テクスチャの回転
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),						// カラー
		250,													// ライフ
		false,													// 重力
		0.025f,													// 抵抗
		true,													// ビルボード
		0,														// 表示する箇所(横)
		0,														// 表示する箇所(縦)
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 中心位置
		0,														// 角度
		0,														// 距離
		0,														// 回転速度
		D3DXVECTOR2(1.0f, 1.0f),								// 画像の分割数
		true,													// 加算合成の有無
		true,													// Zバッファの比較有無
		false													// フェード
	);
}

//=============================================================================
// 砂煙のエフェクト
//=============================================================================
void CEffect::SandSmokeEffect(D3DXVECTOR3 pos)
{
	for (int nCount = 0; nCount < 15; nCount++)
	{
		float fAngle = float(CManager::GetRand(314)) / 100.0f - float(CManager::GetRand(314)) / 100.0f;
		float fRadius = float(CManager::GetRand(3 * 100)) / 100.0f - float(CManager::GetRand(3 * 100)) / 100.0f;

		D3DXVECTOR3 particlePos;

		// 位置の計算
		particlePos.x = sinf(D3DX_PI * fAngle) * (3.0f);
		particlePos.y = 1.0f;
		particlePos.z = cosf(D3DX_PI * fAngle) * (3.0f);

		D3DXMATRIX mtxMeshRot, mtxMeshTrans;					// 計算用マトリックス
		D3DXMATRIX mtx;											// 武器のマトリックス

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtx);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, 0.0f, 0.0f, 0.0f);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxMeshTrans, particlePos.x, particlePos.y, particlePos.z);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshTrans);

		// サイズ
		float size = CManager::GetRand(100) / 10.0f;			// ランダムに取得

		CEffect::SetEffect("data/tex/effect/explosion001.png",				// エフェクトタイプ
			pos,												// 発生位置
			D3DXVECTOR3(size, size, 0.0f),						// サイズ
			D3DXVECTOR3(mtx._41, mtx._42, mtx._43),				// 移動方向
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),					// サイズのフレーム当たりの加算値
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),						// 回転の変化量
			D3DXCOLOR(0.0f, 0.0f, 0.0f, -0.02f),				// 色の変化量
			EASINGTYPE_NONE,									// イージングタイプ
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),						// テクスチャの回転
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f),					// カラー
			100,												// ライフ
			false,												// 重力
			0.025f,												// 抵抗
			true,												// ビルボードの使用
			0,													// 表示する箇所(横)
			0,													// 表示する箇所(縦)
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),						// 中心位置
			0,													// 角度
			0,													// 距離
			0,													// 回転速度
			D3DXVECTOR2(8.0f, 1.0f),							// 画像の分割数
			false,												// 加算合成の有無
			true,												// Zバッファの比較有無
			false												// フェード
		);
	}
}

//=============================================================================
// 砂煙爆発
//=============================================================================
void CEffect::SandSmokeExplosion(D3DXVECTOR3 pos)
{
	for (int nCount = 0; nCount < 100; nCount++)
	{
		float fAngle = float(CManager::GetRand(314)) / 100.0f - float(CManager::GetRand(314)) / 100.0f;
		float fRadius = float(CManager::GetRand(4 * 100)) / 100 - float(CManager::GetRand(4 * 100)) / 100;

		D3DXVECTOR3 particlePos;

		D3DXVECTOR3 vecPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXMATRIX	mtxMeshRot, mtxMeshTrans;				//計算用マトリックス
		D3DXMATRIX mtx;			// 武器のマトリックス
		D3DXMATRIX mtxPlayer;

		// スピード
		float fSpeed = float(CManager::GetRand(15 * 100)) / 100;		// ランダムに取得
		particlePos.y = 1 + fSpeed;
		particlePos.x = sinf(D3DX_PI * fAngle) * (fSpeed);
		particlePos.z = cosf(D3DX_PI * fAngle) * (fSpeed);

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtx);
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtxPlayer);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, 0.0f, 0.0f, 0.0f);
		D3DXMatrixMultiply(&mtxPlayer, &mtxPlayer, &mtxMeshRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxMeshTrans, vecPos.x, vecPos.y, vecPos.z);
		D3DXMatrixMultiply(&mtxPlayer, &mtxPlayer, &mtxMeshTrans);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, 0.0f, 0.0f, 0.0f);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxMeshTrans, particlePos.x, particlePos.y, particlePos.z);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshTrans);

		// マトリックスの合成
		D3DXMatrixMultiply(&mtx, &mtx, &mtxPlayer);

		D3DXVECTOR3 circlePos = pos;

		// 位置計算
		circlePos.x += sinf(D3DX_PI * fAngle) * 30;
		circlePos.z += cosf(D3DX_PI * fAngle) * 30;

		CEffect::SetEffect("data/tex/effect/explosion001.png",				// エフェクトタイプ
			circlePos,											// 発生位置
			D3DXVECTOR3(15.0f, 15.0f, 0.0f),					// サイズ
			D3DXVECTOR3(mtx._41, mtx._42, mtx._43),				// 移動方向
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),						// サイズのフレーム当たりの加算値
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),						// 回転の変化量
			D3DXCOLOR(0.0f, 0.0f, 0.0f, -0.02f),				// 色の変化量
			EASINGTYPE_IN,										// イージングタイプ
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),						// テクスチャの回転
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f),					// カラー
			15,													// ライフ
			false,												// 重力
			0.025f,												// 抵抗
			true,												// ビルボード
			0,													// 表示する箇所(横)
			0,													// 表示する箇所(縦)
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),						// 中心位置
			0,													// 角度
			0,													// 距離
			0,													// 回転速度
			D3DXVECTOR2(8.0f, 1.0f),							// 画像の分割数
			false,												// 加算合成の有無
			true,												// Zバッファの比較有無
			false												// フェード
		);
	}
}

//=============================================================================
// ショック・ウェーブ
//=============================================================================
void CEffect::Shockwave(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nLife)
{
	CCamera *pCamera = CManager::GetCamera();

	if (pCamera != NULL)
	{// カメラが存在していたとき
		CEffect::SetEffect("data/tex/effect/Shockwave.png",				// エフェクトタイプ
			pos,												// 発生位置
			D3DXVECTOR3(15.0f, 15.0f, 0.0f),					// サイズ
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),						// 移動方向
			D3DXVECTOR3(10.0f, 10.0f, 0.0f),					// サイズのフレーム当たりの加算値
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),						// 回転の変化量
			D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),					// 色の変化量
			EASINGTYPE_NONE,									// イージングタイプ
			D3DXVECTOR3(0.0f, rot.y - pCamera->GetRotation().y, 0.0f),	// テクスチャの回転
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f),					// カラー
			nLife,												// ライフ
			false,												// 重力
			0.025f,												// 抵抗
			false,												// ビルボード
			0,													// 表示する箇所(横)
			0,													// 表示する箇所(縦)
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),						// 中心位置
			0,													// 角度
			0,													// 距離
			0,													// 回転速度
			D3DXVECTOR2(1.0f, 1.0f),							// 画像の分割数
			false,												// 加算合成の
			true,												// Zバッファの比較有無
			false												// フェード
		);
	}
}

//=============================================================================
// ハロー現象
//=============================================================================
void CEffect::Halo(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	float fAngle = float(CManager::GetRand(314)) / 100.0f - float(CManager::GetRand(314)) / 100.0f;

	D3DXVECTOR3 rot;

	// 回転
	rot = D3DXVECTOR3(0.0f, 0.0f, cosf(fAngle) * 10);			// ランダムに取得

	CEffect::SetEffect("data/tex/effect/Halo.png",				// エフェクトタイプ
		pos,													// 発生位置
		D3DXVECTOR3(2.0f * size.x, 2.0f * size.y, 0.0f),		// サイズ
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 移動方向
		D3DXVECTOR3(-2.0f, -2.0f, 0.0f),						// サイズのフレーム当たりの加算値
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 回転の変化量
		D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),						// 色の変化量
		EASINGTYPE_IN,											// イージングタイプ
		rot,													// テクスチャの回転
		D3DXCOLOR(0.0f, 1.0f, 1.0f, 0.8f),						// カラー
		15,														// ライフ
		false,													// 重力
		0.025f,													// 抵抗
		true,													// ビルボード
		0,														// 表示する箇所(横)
		0,														// 表示する箇所(縦)
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 中心位置
		0,														// 角度
		0,														// 距離
		0,														// 回転速度
		D3DXVECTOR2(1.0f, 1.0f),								// 画像の分割数
		false,													// 加算合成の有無
		true,													// Zバッファの比較有無
		false													// フェード
	);
}

//=============================================================================
// 浄化分散
//=============================================================================
void CEffect::Purification(D3DXVECTOR3 pos)
{
	float fAngle = float(CManager::GetRand(314)) / 100.0f - float(CManager::GetRand(314)) / 100.0f;

	D3DXVECTOR3 particlePos;

	D3DXMATRIX mtxMeshRot, mtxMeshTrans;				//計算用マトリックス
	D3DXMATRIX mtx;										// 武器のマトリックス

	// 速度
	float fSpeed = float(CManager::GetRand(3 * 100)) / 100;
	particlePos.y = 1.0f + fSpeed;

	// 位置の計算
	particlePos.x = sinf(D3DX_PI * fAngle) * (fSpeed);
	particlePos.z = cosf(D3DX_PI * fAngle) * (fSpeed);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtx);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, 0.0f, 0.0f, 0.0f);
	D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxMeshTrans, particlePos.x, particlePos.y, particlePos.z);
	D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshTrans);

	D3DXVECTOR3 point = pos;

	// 発生位置の計算
	point.x += sinf(D3DX_PI * fAngle) * 80;
	point.z += cosf(D3DX_PI * fAngle) * 80;

	CEffect::SetEffect("data/tex/effect/explosion002.png",						// エフェクトタイプ
		point,													// 発生位置
		D3DXVECTOR3(20.0f, 20.0f, 0.0f),						// サイズ
		D3DXVECTOR3(0.0f, mtx._42, 0.0f),						// 移動方向
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// サイズのフレーム当たりの加算値
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 回転の変化量
		D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),						// 色の変化量
		EASINGTYPE_NONE,										// イージングタイプ
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// テクスチャの回転
		D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f),						// カラー
		50,														// ライフ
		false,													// 重力
		0.025f,													// 抵抗
		true,													// ビルボード
		0,														// 表示する箇所(横)
		0,														// 表示する箇所(縦)
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 中心位置
		0,														// 角度
		0,														// 距離
		0,														// 回転速度
		D3DXVECTOR2(8.0f, 1.0f),								// 画像の分割数
		false,													// 加算合成の有無
		true,													// Zバッファの比較有無
		false													// フェード
	);
}

//=============================================================================
// スター
//=============================================================================
void CEffect::Star(D3DXVECTOR3 pos)
{
	CEffect::SetEffect("data/tex/effect/Star.png",							// エフェクトタイプ
		pos,													// 発生位置
		D3DXVECTOR3(50.0f, 50.0f, 0.0f),						// サイズ
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 移動方向
		D3DXVECTOR3(50.0f / 15, 50.0f / 15, 0.0f),				// サイズのフレーム当たりの加算値
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 回転の変化量
		D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),						// 色の変化量
		EASINGTYPE_OUT,											// イージングタイプ
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// テクスチャの回転
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),						// カラー
		15,														// ライフ
		false,													// 重力
		0.025f,													// 抵抗
		true,													// ビルボード
		0,														// 表示する箇所(横)
		0,														// 表示する箇所(縦)
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 中心位置
		0,														// 角度
		0,														// 距離
		0,														// 回転速度
		D3DXVECTOR2(1.0f, 1.0f),								// 画像の分割数
		false,													// 加算合成の有無
		true,													// Zバッファの比較有無
		true													// フェード
	);
}

//=============================================================================
// 斬撃
//=============================================================================
void CEffect::Slashing(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	D3DXMATRIX mtxMeshRot, mtxMeshTrans;				//計算用マトリックス
	D3DXMATRIX mtx;										// 武器のマトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtx);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxMeshTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshTrans);

	CEffect::SetEffect("data/tex/effect/Slashing.png",						// エフェクトタイプ
		pos,													// 発生位置
		D3DXVECTOR3(50.0f, 150.0f, 0.0f),						// サイズ
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 移動方向
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// サイズのフレーム当たりの加算値
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 回転の変化量
		D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),						// 色の変化量
		EASINGTYPE_NONE,										// イージングタイプ
		rot,													// テクスチャの回転
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),						// カラー
		15,														// ライフ
		false,													// 重力
		0.025f,													// 抵抗
		true,													// ビルボード
		0,														// 表示する箇所(横)
		0,														// 表示する箇所(縦)
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 中心位置
		0,														// 角度
		0,														// 距離
		0,														// 回転速度
		D3DXVECTOR2(9.0f, 1.0f),								// 画像の分割数
		false,													// 加算合成の有無
		true,													// Zバッファの比較有無
		false													// フェード
	);
}

//=============================================================================
// オーラ
//=============================================================================
void CEffect::Aura(int nCount, bool bJump, D3DXVECTOR3 pos)
{
	float fAngle = float(CManager::GetRand(314)) - float(CManager::GetRand(314));
	float fAngle_x = float(CManager::GetRand(314)) - float(CManager::GetRand(314));

	D3DXVECTOR3 pointPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// 角度
	fAngle = float(CManager::GetRand(314)) / 100.0f - float(CManager::GetRand(314)) / 100.0f;		// ランダムに取得

	// 回転
	D3DXVECTOR3 rot;
	rot = D3DXVECTOR3(sinf(fAngle) * 10, cosf(fAngle) * 10, 0.0f);			// ランダムに取得

	// 距離
	float fDistance = float(CManager::GetRand(200 * 100)) / 100;			// ランダムに取得
	fDistance = 50.0f + fDistance;

	pointPos = pos;

	// 位置の計算
	pointPos.x += sinf(D3DX_PI * fAngle) * fDistance;
	pointPos.z += cosf(D3DX_PI * fAngle) * fDistance;

	CEffect::SetEffect("data/tex/effect/sphere.jpg",						// エフェクトタイプ
		pointPos,												// 発生位置
		D3DXVECTOR3(5.0f, 5.0f, 0.0f),							// サイズ
		D3DXVECTOR3(0.0f, 5.0f, 0.0f),							// 移動方向
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// サイズのフレーム当たりの加算値
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 回転の変化量
		D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),						// 色の変化量
		EASINGTYPE_NONE,										// イージングタイプ
		rot,													// テクスチャの回転
		D3DXCOLOR(0.8f, 0.8f, 1.0f, 1.0f),						// カラー
		100,													// ライフ
		false,													// 重力
		0.005f,													// 抵抗
		true,													// ビルボード
		0,														// 表示する箇所(横)
		0,														// 表示する箇所(縦)
		pos,													// 中心位置
		0,														// 角度
		0,														// 距離
		0,														// 回転速度
		D3DXVECTOR2(1.0f, 1.0f),								// 画像の分割数
		true,													// 加算合成の有無
		true,													// Zバッファの比較有無
		false													// フェード
	);

	// 位置のリセット
	pointPos = pos;

	// 回転
	rot = D3DXVECTOR3(sinf(fAngle) * 10, cosf(fAngle) * 10, 0.0f);		// ランダムに取得

	// 距離
	fDistance = float(CManager::GetRand(50 * 100)) / 100;				// ランダムに取得

	// 位置の計算
	pointPos.x += sinf(D3DX_PI * fAngle) * fDistance;
	pointPos.z += cosf(D3DX_PI * fAngle) * fDistance;

	CEffect::SetEffect("data/tex/effect/aura.png",							// エフェクトタイプ
		pointPos,												// 発生位置
		D3DXVECTOR3(50.0f, 50.0f, 0.0f),						// サイズ
		D3DXVECTOR3(0.0f, 5.0f, 0.0f),							// 移動方向
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// サイズのフレーム当たりの加算値
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 回転の変化量
		D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),						// 色の変化量
		EASINGTYPE_NONE,										// イージングタイプ
		rot,													// テクスチャの回転
		D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.5f),						// カラー
		5 + CManager::GetRand(50),								// ライフ
		false,													// 重力
		0.005f,													// 抵抗
		true,													// ビルボード
		0,														// 表示する箇所(横)
		0,														// 表示する箇所(縦)
		pos,													// 中心位置
		0,														// 角度
		0,														// 距離
		0,														// 回転速度
		D3DXVECTOR2(1.0f, 1.0f),								// 画像の分割数
		true,													// 加算合成の有無
		true,													// Zバッファの比較有無
		false													// フェード
	);

	if (!bJump)
	{// ジャンプしていなかったとき
		if (nCount % 20 == 0)
		{// あまりが0だったとき
			CEffect::SetEffect("data/tex/effect/sphere.jpg",	// エフェクトタイプ
				pos,											// 発生位置
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),					// サイズ
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),					// 移動方向
				D3DXVECTOR3(5.0f, 5.0f, 0.0f),					// サイズのフレーム当たりの加算値
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 回転の変化量
				D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),						// 色の変化量
				EASINGTYPE_OUT,									// イージングタイプ
				D3DXVECTOR3(1.57f, 0.0f, 0.0f),					// テクスチャの回転
				D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.8f),				// カラー
				50,												// ライフ
				false,											// 重力
				0.025f,											// 抵抗
				false,											// ビルボード
				0,												// 表示する箇所(横)
				0,												// 表示する箇所(縦)
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),					// 中心位置
				0,												// 角度
				0,												// 距離
				0,												// 回転速度
				D3DXVECTOR2(1.0f, 1.0f),						// 画像の分割数
				true,											// 加算合成の有無
				true,											// Zバッファの比較有無
				false											// フェード
			);
		}

		if (nCount % 15 == 0)
		{// あまりが0だったとき
			CEffect::SetEffect("data/tex/effect/Shockwave.png",			// エフェクトタイプ
				pos,											// 発生位置
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),					// サイズ
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),					// 移動方向
				D3DXVECTOR3(4.0f, 4.0f, 0.0f),					// サイズのフレーム当たりの加算値
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),					// 回転の変化量
				D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),				// 色の変化量
				EASINGTYPE_OUT,									// イージングタイプ
				D3DXVECTOR3(1.57f, 0.0f, 0.0f),					// テクスチャの回転
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f),				// カラー
				50,												// ライフ
				false,											// 重力
				0.025f,											// 抵抗
				false,											// ビルボード
				0,												// 表示する箇所(横)
				0,												// 表示する箇所(縦)
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),					// 中心位置
				0,												// 角度
				0,												// 距離
				0,												// 回転速度
				D3DXVECTOR2(1.0f, 1.0f),						// 分割数
				false,													// 加算合成の有無
				true,											// Zバッファの比較有無
				false											// フェード
			);
		}
	}
}

//=============================================================================
// 花びらの集合体
//=============================================================================
void CEffect::PetalCluster(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	for (int nCount = 0; nCount < 150; nCount++)
	{
		float fAngle = float(CManager::GetRand(314)) / 100.0f - float(CManager::GetRand(314)) / 100.0f;
		float fRadius = float(CManager::GetRand(10 * 100)) / 100.0f - float(CManager::GetRand(10 * 100)) / 100.0f;

		D3DXVECTOR3 petalPos;

		// 位置の計算
		petalPos.x = sinf(D3DX_PI * fAngle) * (fRadius);
		petalPos.y = 15.0f;
		petalPos.z = cosf(D3DX_PI * fAngle) * (fRadius);

		D3DXVECTOR3 vecPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXMATRIX mtxMeshRot, mtxMeshTrans;				// 計算用マトリックス
		D3DXMATRIX mtx;										// 武器のマトリックス
		D3DXMATRIX mtxPlayer;

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtx);

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtxPlayer);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, rot.y, rot.x, rot.z);
		D3DXMatrixMultiply(&mtxPlayer, &mtxPlayer, &mtxMeshRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxMeshTrans, vecPos.x, vecPos.y, vecPos.z);
		D3DXMatrixMultiply(&mtxPlayer, &mtxPlayer, &mtxMeshTrans);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, 0.0f, 0.0f, 0.0f);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxMeshTrans, petalPos.x, petalPos.y, petalPos.z);
		D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshTrans);

		D3DXMatrixMultiply(&mtx, &mtx, &mtxPlayer);

		D3DXVECTOR3 petalRot;
		petalRot = D3DXVECTOR3(sinf(fAngle) * 10, cosf(fAngle) * 10, 0.0f);

		CEffect::SetEffect("data/tex/effect/rose_01.png",							// エフェクトタイプ
			pos,	// 発生位置										// 発生位置
			D3DXVECTOR3(10.0f, 10.0f, 0.0f),						// サイズ
			D3DXVECTOR3(mtx._41, mtx._42, mtx._43),					// 移動方向
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// サイズのフレーム当たりの加算値
			D3DXVECTOR3(0.02f, 0.02f, 0.0f),						// 回転の変化量
			D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),						// 色の変化量
			EASINGTYPE_NONE,										// イージングタイプ
			petalRot,												// テクスチャの回転
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),						// カラー
			50,														// ライフ
			false,													// 重力
			0,														// 抵抗
			true,													// ビルボード
			0,														// 表示する箇所(横)
			0,														// 表示する箇所(縦)
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),							// 中心位置
			0,														// 角度
			0,														// 距離
			0,														// 回転速度
			D3DXVECTOR2(1.0f, 1.0f),								// 分割数
			false,													// 加算合成の有無
			false,													// Zバッファの比較有無
			false													// フェードの有無
		);
	}
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void CEffect::MakeVertex(void)
{
	VERTEX_3D *pVtx;

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-m_size.x, m_size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_size.x, m_size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_size.x, -m_size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_size.x, -m_size.y, 0.0f);

	//法線ベクトル
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	//頂点カラー
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	//テクスチャ描写の位置
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
// アニメーション処理
//=============================================================================
void CEffect::Animation(void)
{
	// アニメーション
	if (m_AnimCount >= m_nMaxLife / m_sprite.x)
	{
		m_AnimPage++;		// ページに1加算する

		SpriteAnimation(m_sprite, m_AnimPage, 0);		// 描画範囲
		m_AnimCount = 0;		// 初期化
	}

	m_AnimCount++;				// カウンタに1プラスする
}

//=============================================================================
// 位置のランダム処理
//=============================================================================
D3DXVECTOR3 CEffect::GetRandomPosWithSphere(void)
{
	// 位置のランダム取得
	float fAngle = float(CManager::GetRand(314)) - float(CManager::GetRand(314));
	float fAngle_x = float(CManager::GetRand(314)) - float(CManager::GetRand(314));

	D3DXVECTOR3 pos;

	// 位置の計算
	pos.x = cosf(D3DX_PI + fAngle) * cosf(D3DX_PI + fAngle_x);
	pos.y = sinf(D3DX_PI + fAngle_x);
	pos.z = sinf(D3DX_PI + fAngle) * cosf(D3DX_PI + fAngle_x);

	// 計算結果を返す
	return pos;
}


//=============================================================================
// 位置のランダム処理
//=============================================================================
D3DXVECTOR3 CEffect::GetRandomPosWithCone(float &fRadius)
{
	// 位置のランダム取得
	float fAngle = float(CManager::GetRand(314)) / 100.0f - float(CManager::GetRand(314)) / 100.0f;
	fRadius = float(CManager::GetRand(10 * 100)) / 100.0f - float(CManager::GetRand(10 * 100)) / 100.0f;

	D3DXVECTOR3 pos;

	// 位置の計算
	pos.x = sinf(D3DX_PI * fAngle) * (fRadius);
	pos.y = 1.0f;

	fAngle = float(CManager::GetRand(314)) / 100.0f - float(CManager::GetRand(314)) / 100.0f;
	pos.z = cosf(D3DX_PI * fAngle) * (fRadius);

	return pos;
}

//=============================================================================
// 角度のランダム処理
//=============================================================================
float CEffect::GetRandomAngle(void)
{
	// 角度のランダム取得
	return float(CManager::GetRand(314)) / 100.0f -
		float(CManager::GetRand(314)) / 100.0f;		// ランダムに取得
}

//=============================================================================
// エフェクト情報の読み込み
//=============================================================================
void CEffect::LoadScript(std::string Add)
{
	std::map<std::string, EFFECT>::const_iterator it = m_effectMap.find(Add);

	if (it == m_effectMap.end())
	{// 見つからなかったとき
	 // 検索した文字列をキーとして
	 // 新しくマップにエフェクトデータを追加する
		EFFECT effect;
		effect.bAlpha = false;
		effect.bBillboard = false;
		effect.bFadeOut = false;
		effect.bZBuffer = false;

		FILE *pFile;
		char cReadText[128];		//文字
		char cHeadText[128];		//比較
		char cDie[128];
		int nCntPointer = 0;			//ポインターの数値

		char aTag[64];					//モデルのアドレス
		char aAdd[64];					//モデルのアドレス

		//テキストデータロード
		pFile = fopen(Add.c_str(), "r");

		if (pFile != NULL)
		{
			//ポインターのリセット
			nCntPointer = 0;

			//スクリプトが来るまでループ
			while (strcmp(cHeadText, "SCRIPT") != 0)
			{
				fgets(cReadText, sizeof(cReadText), pFile);
				sscanf(cReadText, "%s", &cHeadText);
			}

			//スクリプトだったら
			if (strcmp(cHeadText, "SCRIPT") == 0)
			{
				//エンドスクリプトが来るまで
				while (strcmp(cHeadText, "END_SCRIPT") != 0)
				{
					fgets(cReadText, sizeof(cReadText), pFile);
					sscanf(cReadText, "%s", &cHeadText);

					if (strcmp(cHeadText, "EFFECTSET") == 0)
					{//キャラクターの初期情報のとき
							//エンドキャラクターセットが来るまでループ
						while (strcmp(cHeadText, "END_EFFECTSET") != 0)
						{
							fgets(cReadText, sizeof(cReadText), pFile);
							sscanf(cReadText, "%s", &cHeadText);

							//改行
							if (strcmp(cReadText, "\n") != 0)
							{
								if (strcmp(cHeadText, "EFFECT_FILENAME") == 0)
								{// テクスチャアドレス情報
									sscanf(cReadText, "%s %s %s", &cDie, &cDie, &aAdd[0]);						//アドレスの取得
									effect.texAdd = aAdd;
								}
								else if (strcmp(cHeadText, "TAG") == 0)
								{// タグの情報
									sscanf(cReadText, "%s %s %s", &cDie, &cDie, &aTag[0]);						//アドレスの取得
								}
								else if (strcmp(cHeadText, "BILLBOARD") == 0)
								{// ビルボードの有無
									effect.bBillboard = true;
								}
								else if (strcmp(cHeadText, "ALPHABLENDING") == 0)
								{// アルファブレンディングの有無
									effect.bAlpha = true;
								}
								else if (strcmp(cHeadText, "ZBUFFER") == 0)
								{// Zバッファの有無
									effect.bZBuffer = true;
								}
								else if (strcmp(cHeadText, "FADE_OUT") == 0)
								{// フェードアウトの有無
									effect.bFadeOut = true;
								}
								else if (strcmp(cHeadText, "SIZE") == 0)
								{// サイズ
									D3DXVECTOR3 size;
									sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
										&size.x,
										&size.y,
										&size.z);

									// サイズ情報を設定する
									effect.size = size;
								}
								else if (strcmp(cHeadText, "COL") == 0)
								{// 色
									D3DXCOLOR col;
									sscanf(cReadText, "%s %s %f %f %f %f", &cDie, &cDie,
										&col.r,
										&col.g,
										&col.b,
										&col.a);

									// 色情報を設定する
									effect.col = col;
								}
								else if (strcmp(cHeadText, "MOVE_SIZE") == 0)
								{// サイズの変化量
									D3DXVECTOR3 moveSize;
									sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
										&moveSize.x,
										&moveSize.y,
										&moveSize.z);

									// サイズの変化量情報を設定する
									effect.moveSize = moveSize;
								}
								else if (strcmp(cHeadText, "MOVE_ROT") == 0)
								{// 回転の変化量
									D3DXVECTOR3 moveRot;
									sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
										&moveRot.x,
										&moveRot.y,
										&moveRot.z);

									// 回転の変化量情報を設定する
									effect.moveRot = moveRot;
								}
								else if (strcmp(cHeadText, "MOVE_COL") == 0)
								{// 色の変化量
									D3DXCOLOR moveCol;
									sscanf(cReadText, "%s %s %f %f %f %f", &cDie, &cDie,
										&moveCol.r,
										&moveCol.g,
										&moveCol.b,
										&moveCol.a);

									// 色の変化量情報を設定する
									effect.moveCol = moveCol;
								}
								else if (strcmp(cHeadText, "LIFE") == 0)
								{//ライフ
									int nLife;
									sscanf(cReadText, "%s %s %d", &cDie, &cDie,
										&nLife);

									// サイズ情報を設定する
									effect.nLife = nLife;
								}
								else if (strcmp(cHeadText, "GRAVITY") == 0)
								{// 重力加速度
									float fGravity;
									sscanf(cReadText, "%s %s %f", &cDie, &cDie,
										&fGravity);

									// サイズ情報を設定する
									effect.fGravity = fGravity;
								}
								else if (strcmp(cHeadText, "RESISTANCE") == 0)
								{// 抵抗値
									float fResistance;
									sscanf(cReadText, "%s %s %f", &cDie, &cDie,
										&fResistance);

									// サイズ情報を設定する
									effect.fResistance = fResistance;
								}
								else if (strcmp(cHeadText, "SPEED") == 0)
								{// 抵抗値
									float fMaxSpeed;
									sscanf(cReadText, "%s %s %f", &cDie, &cDie,
										&fMaxSpeed);

									// サイズ情報を設定する
									effect.fMaxSpeed = fMaxSpeed;
								}
								else if (strcmp(cHeadText, "CENTER_POS") == 0)
								{// 回転の変化量
									D3DXVECTOR3 centerPos;
									sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
										&centerPos.x,
										&centerPos.y,
										&centerPos.z);

									// 回転の変化量情報を設定する
									effect.centerPos = centerPos;
								}
								else if (strcmp(cHeadText, "ANGLE") == 0)
								{// 重力加速度
									float fAngle;
									sscanf(cReadText, "%s %s %f", &cDie, &cDie,
										&fAngle);

									// サイズ情報を設定する
									effect.fAngle = fAngle;
								}
								else if (strcmp(cHeadText, "DISTANCE") == 0)
								{// 重力加速度
									float fDistance;
									sscanf(cReadText, "%s %s %f", &cDie, &cDie,
										&fDistance);

									// サイズ情報を設定する
									effect.fDistance = fDistance;
								}
								else if (strcmp(cHeadText, "ROTATION_SPEED") == 0)
								{// 重力加速度
									float fRotationSpeed;
									sscanf(cReadText, "%s %s %f", &cDie, &cDie,
										&fRotationSpeed);

									// サイズ情報を設定する
									effect.fSpeedRot = fRotationSpeed;
								}
								else if (strcmp(cHeadText, "SPRITE") == 0)
								{// サイズ
									D3DXVECTOR2 sprite;
									sscanf(cReadText, "%s %s %f %f", &cDie, &cDie,
										&sprite.x,
										&sprite.y);

									// サイズ情報を設定する
									effect.sprite = sprite;
								}
							}
						}
					}
				}
			}

			// マップへ挿入する
			m_effectMap.insert(std::map<std::string, EFFECT>::value_type(aTag, effect));

			//ファイル閉
			fclose(pFile);
		}
		else
		{

		}

		return;				// 結果を返す
	}
}

//=============================================================================
// エフェクトデータの取得
//=============================================================================
EFFECT CEffect::GetEffectData(std::string Tag)
{
	std::map<std::string, EFFECT>::const_iterator it = m_effectMap.find(Tag);

	EFFECT effect;

	if (it == m_effectMap.end())
	{// 見つからなかったとき
	 // 検索した文字列をキーとして
	 // 新しくマップにテクスチャを追加する
		return effect;
	}
	else
	{
		return it->second;
	}
}

//=============================================================================
// エフェクトの作成処理
//=============================================================================
void CEffect::CreateEffect(std::string Tag, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	EFFECT effect;
	effect = GetEffectData(Tag);

	float fAngle = float(CManager::GetRand(314)) - float(CManager::GetRand(314));
	float fAngle_x = float(CManager::GetRand(314)) - float(CManager::GetRand(314));

	D3DXVECTOR3 particlePos;
	float fRadius;

	float fSpeed = 0.0f;
	//if (false)
	{// スピードがランダムじゃなかったとき
		fSpeed = effect.fMaxSpeed;
	}

	switch (PARTICLESHAPE_SPHERE)
	{
	case PARTICLESHAPE_CONE:
		fRadius = float(CManager::GetRand(10 * 100)) / 100.0f - float(CManager::GetRand(10 * 100)) / 100.0f;

		particlePos = CEffect::GetRandomPosWithCone(fRadius);

		// 位置の計算
		particlePos.x = sinf(D3DX_PI * fAngle) * (fRadius);
		particlePos.y = effect.fMaxSpeed;
		particlePos.z = cosf(D3DX_PI * fAngle) * (fRadius);
		break;
	case PARTICLESHAPE_SPHERE:
		particlePos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		particlePos.x = cosf(D3DX_PI + fAngle) * cosf(D3DX_PI + fAngle_x);
		particlePos.y = sinf(D3DX_PI + fAngle_x);
		particlePos.z = sinf(D3DX_PI + fAngle) * cosf(D3DX_PI + fAngle_x);

		fAngle = float(CManager::GetRand(314)) / 100.0f - float(CManager::GetRand(314)) / 100.0f;
		break;
	case PARTICLESHAPE_BOX:
		break;
	}

	//D3DXVECTOR3 rot = D3DXVECTOR3_ZERO;
	//rot = D3DXVECTOR3(sinf(fAngle) * 10, cosf(fAngle) * 10, 0.0f);

	// パーティクル全体の位置計算
	D3DXVECTOR3 vecPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMATRIX mtxMeshRot, mtxMeshTrans;				// 計算用マトリックス
	D3DXMATRIX mtx;										// 武器のマトリックス
	D3DXMATRIX mtxPlayer;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtx);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxPlayer);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxPlayer, &mtxPlayer, &mtxMeshRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxMeshTrans, vecPos.x, vecPos.y, vecPos.z);
	D3DXMatrixMultiply(&mtxPlayer, &mtxPlayer, &mtxMeshTrans);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, 0.0f, 0.0f, 0.0f);
	D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxMeshTrans, particlePos.x, particlePos.y, particlePos.z);
	D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshTrans);

	D3DXMatrixMultiply(&mtx, &mtx, &mtxPlayer);

	CEffect::SetEffect
	(
		effect.texAdd,		// パーティクルのタイプ
		pos,			// 発生位置
		effect.size,					// サイズ
		D3DXVECTOR3(mtx._41, mtx._42, mtx._43) * fSpeed,	// 方向ベクトル
		effect.moveSize,				// 大きさの変化量
		effect.moveRot,				// 回転の変化量
		effect.moveCol,				// 色の変化量
		EASINGTYPE_NONE,
		rot,					// テクスチャの回転
		effect.col,					// カラー
		effect.nLife,			// パーティクルの生存カウント数
		effect.fGravity,				// 重力
		effect.fResistance,			// 抵抗
		effect.bBillboard,			// ビルボード
		0,						// 表示する箇所(横)
		0,						// 表示する箇所(縦)
		effect.centerPos,			// 中心位置
		effect.fAngle,				// 角度
		effect.fDistance,			// 距離
		effect.fSpeedRot,		// 回転速度
		effect.sprite,				// 画像の分割数
		effect.bAlpha,				// 加算合成の有無
		effect.bZBuffer,				// Zバッファの比較有無
		effect.bFadeOut				// フェード
	);

}

//=============================================================================
// パーティクルスクリプトアセットのロード処理
//=============================================================================
void CEffect::LoadParticleScript(void)
{
	FILE *pFile;
	char cReadText[128];		//文字
	char cHeadText[128];		//比較
	char cDie[128];
	int nCntPointer = 0;		//ポインターの数値

	char sAdd[64];				//モデルのアドレス
	std::string Add;

	int nCntMotion = 0;			//参照するポインタの値を初期化
	int nCntKey = 0;

	int nMaxModel = 0;

	//テキストデータロード
	pFile = fopen(MANAGER_PARTICLE, "r");

	if (pFile != NULL)
	{
		//ポインターのリセット
		nCntPointer = 0;

		//スクリプトが来るまでループ
		while (strcmp(cHeadText, "SCRIPT") != 0)
		{
			fgets(cReadText, sizeof(cReadText), pFile);
			sscanf(cReadText, "%s", &cHeadText);
		}

		//スクリプトだったら
		if (strcmp(cHeadText, "SCRIPT") == 0)
		{
			//エンドスクリプトが来るまで
			while (strcmp(cHeadText, "END_SCRIPT") != 0)
			{
				fgets(cReadText, sizeof(cReadText), pFile);
				sscanf(cReadText, "%s", &cHeadText);

				//改行
				if (strcmp(cReadText, "\n") != 0)
				{
					if (strcmp(cHeadText, "MODEL_FILENAME") == 0)
					{//パーツモデルのアドレス情報のとき
						sscanf(cReadText, "%s %s %s", &cDie, &cDie, &sAdd[0]);						//アドレスの取得
						Add = sAdd;
						CEffect::LoadScript(Add);
					}
				}
			}
		}

		//ファイル閉
		fclose(pFile);
	}
	else
	{
		MessageBox(NULL, "テクスチャマネージャが開けませんでした！", "WARNING", MB_ICONWARNING);	// メッセージボックスの生成
	}
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CEffect::Debug(void)
{

}
#endif