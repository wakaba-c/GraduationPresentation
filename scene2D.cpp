//=============================================================================
//
// scene2D処理 [Scene2D.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "scene2D.h"
#include "manager.h"
#include "renderer.h"
#include "inputKeyboard.h"
#include "inputMouse.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CScene2D::CScene2D(CScene::PRIORITY obj) : CScene(obj)
{
	// 優先度 の設定
	SetObjType(obj);

	// 値の初期化
	m_pTexture = NULL;								// テクスチャの初期化
	m_pVtxBuff = NULL;								// 頂点バッファの初期化
	m_Vtx = TEXTUREVTX_CENTER;						// 中心位置の初期化
	m_size = D3DXVECTOR3(10.0f, 10.0f, 0.0f);		// サイズの初期化
	m_DrawPos = D3DXVECTOR2(0.0f, 0.0f);			// 描画位置の初期化
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 回転の初期化
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);		// 色の初期化
}

//=============================================================================
// デストラクタ
//=============================================================================
CScene2D::~CScene2D()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CScene2D::Init(void)
{
	CRenderer *Renderer = CManager::GetRenderer();
	VERTEX_2D *pVtx;
	LPDIRECT3DDEVICE9 pDevice;
	pDevice = Renderer->GetDevice();

	D3DXVECTOR3 pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);				// 位置の取得
	SetPosition(pos);			// 位置情報の設定

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	m_fAngle = atan2f(m_size.x, m_size.y);
	m_fLength = sqrtf((m_size.x * m_size.x) + (m_size.y * m_size.y)) / 2;


	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(pos.x - m_size.x / 2, pos.y - m_size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(pos.x + m_size.x / 2, pos.y - m_size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(pos.x - m_size.x / 2, pos.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(pos.x + m_size.x / 2, pos.y, 0.0f);

	// 1.0で固定、同次座標
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラー
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CScene2D::Uninit(void)
{
	// 頂点バッファの開放
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CScene2D::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void CScene2D::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice;
	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスを取得する
	pDevice = pRenderer->GetDevice();

	//頂点バッファをデバイスのデータにバインド
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	//テクスチャの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	//頂点フォーマットの設定
	pDevice->SetTexture(0, m_pTexture);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//=============================================================================
// クリエイト処理
//=============================================================================
CScene2D *CScene2D::Create(CScene::PRIORITY obj)
{
	CScene2D *pScene2D;
	pScene2D = new CScene2D(obj);
	pScene2D->Init();
	return pScene2D;
}

//=============================================================================
// テクスチャの設定
//=============================================================================
void CScene2D::BindTexture(std::string Add)
{
	m_pTexture = CManager::GetResource(Add);
	m_TexAdd = Add;
}

//=============================================================================
// 頂点情報の更新
//=============================================================================
void CScene2D::SetTransform(void)
{
	VERTEX_2D *pVtx;										// 頂点情報へのポインタ
	D3DXVECTOR3 pos = GetPosition();						// 位置の取得

	m_fAngle = atan2f(m_size.x, m_size.y);
	m_fLength = sqrtf((m_size.x * m_size.x) + (m_size.y * m_size.y)) / 2;

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	switch (m_Vtx)
	{
	case TEXTUREVTX_CENTER:
		//頂点座標の設定
		pVtx[0].pos.x = pos.x + sinf(-D3DX_PI + m_fAngle + (D3DX_PI * m_rot.z)) * m_fLength;
		pVtx[0].pos.y = pos.y + cosf(-D3DX_PI + m_fAngle + (D3DX_PI * m_rot.z)) * m_fLength;
		pVtx[0].pos.z = 0.0f;

		pVtx[1].pos.x = pos.x + sinf(D3DX_PI - m_fAngle + (D3DX_PI * m_rot.z)) * m_fLength;
		pVtx[1].pos.y = pos.y + cosf(D3DX_PI - m_fAngle + (D3DX_PI * m_rot.z)) * m_fLength;
		pVtx[1].pos.z = 0.0f;

		pVtx[2].pos.x = pos.x + sinf(-m_fAngle + (D3DX_PI * m_rot.z)) * m_fLength;
		pVtx[2].pos.y = pos.y + cosf(-m_fAngle + (D3DX_PI * m_rot.z)) * m_fLength;
		pVtx[2].pos.z = 0.0f;

		pVtx[3].pos.x = pos.x + sinf(m_fAngle + (D3DX_PI * m_rot.z)) * m_fLength;
		pVtx[3].pos.y = pos.y + cosf(m_fAngle + (D3DX_PI * m_rot.z)) * m_fLength;
		pVtx[3].pos.z = 0.0f;
		break;

	case TEXTUREVTX_LEFT:
		//頂点座標の設定
		pVtx[0].pos.x = cosf(D3DX_PI * m_rot.z) - -m_size.y * sinf(D3DX_PI * m_rot.z) + pos.x;
		pVtx[0].pos.y = sinf(D3DX_PI * m_rot.z) + -m_size.y * cosf(D3DX_PI * m_rot.z) + pos.y;
		pVtx[0].pos.z = 0.0f;

		pVtx[1].pos.x = m_size.x / 2 * cosf(D3DX_PI * m_rot.z) - -m_size.y * sinf(D3DX_PI * m_rot.z) + pos.x;
		pVtx[1].pos.y = m_size.x / 2 * sinf(D3DX_PI * m_rot.z) + -m_size.y * cosf(D3DX_PI * m_rot.z) + pos.y;
		pVtx[1].pos.z = 0.0f;

		pVtx[2].pos.x = cosf(D3DX_PI * m_rot.z) - sinf(D3DX_PI * m_rot.z) + pos.x;
		pVtx[2].pos.y = sinf(D3DX_PI * m_rot.z) + cosf(D3DX_PI * m_rot.z) + pos.y;
		pVtx[2].pos.z = 0.0f;

		pVtx[3].pos.x = m_size.x / 2 * cosf(D3DX_PI * m_rot.z) - sinf(D3DX_PI * m_rot.z) + pos.x;
		pVtx[3].pos.y = m_size.x / 2 * sinf(D3DX_PI * m_rot.z) + cosf(D3DX_PI * m_rot.z) + pos.y;
		pVtx[3].pos.z = 0.0f;
		break;
	}

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
// 画像の反転
//=============================================================================
void CScene2D::SetTransformTurnOver(void)
{
	VERTEX_2D *pVtx;										//頂点情報へのポインタ
	D3DXVECTOR3 pos[4];

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//テクスチャ座標の更新
	pos[0] = pVtx[0].pos;
	pos[1] = pVtx[1].pos;
	pos[2] = pVtx[2].pos;
	pos[3] = pVtx[3].pos;

	pVtx[0].pos = pos[1];
	pVtx[1].pos = pos[0];
	pVtx[2].pos = pos[3];
	pVtx[3].pos = pos[2];

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
// サイズ設定処理
//=============================================================================
void CScene2D::SetSize(D3DXVECTOR3 size)
{
	m_size = size;
}

//=============================================================================
// テクスチャ描画位置の更新
//=============================================================================
void CScene2D::SetDrawPos(D3DXVECTOR2 tex)
{
	VERTEX_2D *pVtx;										//頂点情報へのポインタ

	m_DrawPos = tex;

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//テクスチャ座標の更新
	pVtx[0].tex = D3DXVECTOR2(0.0f + tex.x, 0.0f + tex.y);
	pVtx[1].tex = D3DXVECTOR2(1.0f + tex.x, 0.0f + tex.y);
	pVtx[2].tex = D3DXVECTOR2(0.0f + tex.x, 1.0f + tex.y);
	pVtx[3].tex = D3DXVECTOR2(1.0f + tex.x, 1.0f + tex.y);

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
// テクスチャ描画位置の更新
//=============================================================================
void CScene2D::SpriteAnimation(D3DXVECTOR2 patternNo, int nPatternAnim, int nHeight)
{//縦と横のパターン数と現在のページと行

	VERTEX_2D *pVtx;										//頂点情報へのポインタ

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//テクスチャ座標の更新
	pVtx[0].tex = D3DXVECTOR2(0.0f + nPatternAnim * 1.0f / patternNo.x, 0.0f + (1.0f / patternNo.y) * nHeight);
	pVtx[1].tex = D3DXVECTOR2(1.0f / patternNo.x + nPatternAnim * 1.0f / patternNo.x, 0.0f + (1.0f / patternNo.y) * nHeight);
	pVtx[2].tex = D3DXVECTOR2(0.0f + nPatternAnim * 1.0f / patternNo.x, 1.0f / patternNo.y + (1.0f / patternNo.y) * nHeight);
	pVtx[3].tex = D3DXVECTOR2(1.0f / patternNo.x + nPatternAnim * 1.0f / patternNo.x, 1.0f / patternNo.y + (1.0f / patternNo.y) * nHeight);

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
// テクスチャーの頂点カラーの適応
//=============================================================================
void CScene2D::SetColor(D3DXCOLOR col)
{
	VERTEX_2D *pVtx;										//頂点情報へのポインタ

	m_col = col;

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//テクスチャ座標の更新
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
// テクスチャ中心位置の更新
//=============================================================================
void CScene2D::SetCenter(TEXTUREVTX vtx)
{
	//中心位置の設定
	m_Vtx = vtx;
}

//=============================================================================
// テクスチャ描画位置の更新
//=============================================================================
void CScene2D::SetRotation(D3DXVECTOR3 rot)
{
	m_rot = rot;
}

//=============================================================================
// テクスチャの当たり判定(Hit)
//=============================================================================
bool CScene2D::HitCollision2D(CScene2D *obj, bool bTrigger)
{
	bool bJudge = false;
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 posOld = GetPosOld();

	D3DXVECTOR3 objPos = obj->GetPosition();
	D3DXVECTOR3 objPosOld = obj->GetPosOld();

	switch (m_Vtx)
	{
		//画像の中心が中央だったとき
	case TEXTUREVTX_CENTER:
		//上下判定
		if ((objPosOld.x + obj->m_size.x / 2 >= posOld.x - m_size.x / 2) &&
			(objPosOld.x - obj->m_size.x / 2 <= posOld.x + m_size.x / 2))
		{
			//下
			if ((objPos.y + obj->m_size.y / 2 > pos.y - m_size.y / 2) &&
				(objPosOld.y + obj->m_size.y / 2 <= posOld.y - m_size.y / 2))
			{
				bJudge = true;

				//トリガーでは無いとき
				if (!bTrigger)
				{
					objPos.y = pos.y - m_size.y / 2;
					obj->SetPosition(objPos);
				}
			}

			//上
			else if ((objPos.y - obj->m_size.y / 2 < pos.y + m_size.y / 2) &&
				(objPosOld.y - obj->m_size.y / 2 >= posOld.y + m_size.y / 2))
			{
				OutputDebugString("当たり判定\n");
				bJudge = true;
				//トリガーでは無いとき
				if (!bTrigger)
				{
					objPos.y = pos.y - m_size.y / 2;
					obj->SetPosition(objPos);
				}
			}
		}

		if (!bJudge)
		{
			//左右判定
			if ((objPosOld.y + obj->m_size.y / 2 > posOld.y - m_size.y / 2) &&
				(objPosOld.y - obj->m_size.y / 2 <= posOld.y + m_size.y / 2))
			{
				//プレイヤーの判定
				if (objPos.x + obj->m_size.x / 2 > pos.x - m_size.x / 2 &&
					(objPosOld.x + obj->m_size.x / 2 <= posOld.x - m_size.x / 2))
				{
					bJudge = true;
					//トリガーでは無いとき
					if (!bTrigger)
					{
						objPos.y = pos.y - m_size.y / 2;
						obj->SetPosition(objPos);
					}
				}
				else if (objPos.x - obj->m_size.x / 2 < pos.x + m_size.x / 2 &&
					(objPos.x - obj->m_size.x / 2 >= posOld.x + m_size.x / 2))
				{
					bJudge = true;

					//トリガーでは無いとき
					if (!bTrigger)
					{
						objPos.y = pos.y - m_size.y / 2;
						obj->SetPosition(objPos);
					}
				}
			}
		}
		break;

		//画像の中心が左詰めだったとき
	case TEXTUREVTX_LEFT:
		//上下判定
		if ((objPosOld.x + obj->m_size.x / 2 >= posOld.x - m_size.x / 2) &&
			(objPosOld.x - obj->m_size.x / 2 <= posOld.x + m_size.x / 2))
		{
			//上
			if ((objPos.y + obj->m_size.y / 2 > pos.y - m_size.y) &&
				(objPosOld.y + obj->m_size.y / 2 <= posOld.y - m_size.y))
			{
				bJudge = true;

				//トリガーでは無いとき
				if (!bTrigger)
				{
					//プレイヤーの位置を補正
					objPos.y = pos.y - m_size.y - obj->m_size.y / 2;
					obj->SetPosition(objPos);
					objPosOld.y = pos.y - m_size.y - obj->m_size.y / 2;
					obj->SetPosOld(objPosOld);
				}
			}

			//下
			else if ((objPos.y - obj->m_size.y / 2 < pos.y) &&
				(objPosOld.y - obj->m_size.y / 2 >= posOld.y))
			{
				OutputDebugString("当たり判定\n");
				bJudge = true;
				//トリガーでは無いとき
				if (!bTrigger)
				{
					//プレイヤーの位置を補正
					objPos.y = pos.y + obj->m_size.y / 2;
					obj->SetPosition(objPos);
					objPosOld.y = pos.y + obj->m_size.y / 2;
					SetPosOld(objPosOld);
				}
			}
		}

		if (!bJudge)
		{
			//左右判定
			if ((objPosOld.y + obj->m_size.y / 2 > posOld.y - m_size.y) &&
				(objPosOld.y - obj->m_size.y / 2 <= posOld.y))
			{
				//プレイヤーの判定
				if (objPos.x + obj->m_size.x / 2 > pos.x - m_size.x / 2 &&
					(objPosOld.x + obj->m_size.x / 2 <= posOld.x - m_size.x / 2))
				{
					bJudge = true;
					//トリガーでは無いとき
					if (!bTrigger)
					{
						objPos.x = pos.x - m_size.x / 2 - obj->m_size.x / 2;
						obj->SetPosition(objPos);
						objPosOld.x = pos.x - m_size.x / 2 - obj->m_size.x / 2;
						SetPosOld(objPosOld);
					}
				}
				else if (objPos.x - obj->m_size.x / 2 < pos.x + m_size.x / 2 &&
					(objPosOld.x - obj->m_size.x / 2 >= posOld.x + m_size.x / 2))
				{
					bJudge = true;

					//トリガーでは無いとき
					if (!bTrigger)
					{
						objPos.x = pos.x + m_size.x / 2 + obj->m_size.x / 2;
						obj->SetPosition(objPos);
						objPosOld.x = pos.x + m_size.x / 2 + obj->m_size.x / 2;
						SetPosOld(objPosOld);
					}
				}
			}
		}
		break;
	}

	//結果を返す
	return bJudge;
}

//=============================================================================
// テクスチャの当たり判定
//=============================================================================
bool CScene2D::InCollision2D(CScene2D *obj)
{
	bool bJudge = false;

	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 objPos = obj->GetPosition();

	//横の当たり判定 (pos1の下側)
	if (objPos.y - obj->m_size.y / 2 < pos.y + m_size.y / 2)
	{
		if (objPos.y - obj->m_size.y / 2 > pos.y - m_size.y / 2)
		{
			//横の当たり判定 (pos1の右側)
			if (objPos.x + obj->m_size.x / 2 < pos.x + m_size.x / 2)
			{
				if (objPos.x + obj->m_size.x / 2 > pos.x - m_size.x / 2)
				{
					bJudge = true;
				}
			}
			//横の当たり判定 (pos1の左側)
			else if (objPos.x - obj->m_size.x / 2 < pos.x + m_size.x / 2)
			{
				if (objPos.x - obj->m_size.x / 2 > pos.x - m_size.x / 2)
				{
					bJudge = true;
				}
			}
		}
	}
	if (!bJudge)
	{//まだ当たり判定がfalseの場合
		//横の当たり判定 (pos1の上側)
		if (objPos.y + obj->m_size.y / 2 < pos.y + m_size.y / 2)
		{
			if (objPos.y + obj->m_size.y / 2 > pos.y - m_size.y / 2)
			{
				//横の当たり判定 (pos1の右側)
				if (objPos.x + obj->m_size.x / 2 < pos.x + m_size.x / 2)
				{
					if (objPos.x + obj->m_size.x / 2 > pos.x - m_size.x / 2)
					{
						bJudge = true;
					}
				}
				//横の当たり判定 (pos1の左側)
				else if (objPos.x - obj->m_size.x / 2 < pos.x + m_size.x / 2)
				{
					if (objPos.x - obj->m_size.x / 2 > pos.x - m_size.x / 2)
					{
						bJudge = true;
					}
				}
			}
		}
	}

	//結果を返す
	return bJudge;
}