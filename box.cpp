//==================================================================================================================
//
// box[box.cpp]
// Author:Ryouma Inoue
//
//==================================================================================================================
#include "manager.h"
#include "renderer.h"
#include "Box.h"
#include "inputKeyboard.h"
#include "player.h"
#include "game.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define WhileX 50.0f																// イチマスの長さ横
#define WhileY 50.0f																// イチマスの長さ高さ
#define WhileZ 50.0f																// イチマスの長さ縦

//==================================================================================================================
// 静的メンバ変数の初期化
//==================================================================================================================
LPDIRECT3DTEXTURE9 CBox::m_pTexture = NULL;			// テクスチャ変数
CBox *CBox::m_pBox = NULL;							// メッシュフィールドの情報
CPlayer *CBox::m_pPlayer = NULL;					// プレイヤーの情報

//==================================================================================================================
// グローバル変数
//==================================================================================================================

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CBox::CBox(PRIORITY type = CScene::PRIORITY_FLOOR) :CScene(type)
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CBox::~CBox()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
HRESULT CBox::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();						// レンダラー情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();					// デバイスの取得

																		// 位置・回転の初期設定
	m_pos = D3DXVECTOR3(100.0f, 100.0f, 0.0f);								// 位置
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);								// 回転
	m_nNumVertex = ((Box_Width + 1) * Box_Depth * 2) + (Box_Depth - 1) * 2;			// 総頂点数
	m_nNumIndex = (Box_Depth + 1) * (Box_Width + 1);					// 総インデックス数
	m_nNumPolygon = m_nNumVertex - 2;									// 三角形の数
	m_aVecA[Box_Depth * Box_Width] = {};								// 法線ベクトルを面の数分一時的に格納
	m_aVecB[Box_Depth * Box_Width] = {};								// 法線ベクトルを面の数分一時的に格納
	m_vectorA = D3DXVECTOR3(0, 0, 0);									// 一つ目の外積用変数
	m_vectorB = D3DXVECTOR3(0, 0, 0);									// 二つ目の外積用変数
	m_vectorC = D3DXVECTOR3(0, 0, 0);									// 三つ目の外積用変数
	nNumber = 0;														// 配列の番号
	StartBox = Box_Width + 1;												// 始まる箱
	EndBox = 0;															// 引かれる箱
	fDivide = 0;														// sinの中身を割る変数

																		// 頂点情報の作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * m_nNumIndex,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	//インデックスバッファを生成
	pDevice->CreateIndexBuffer(
		sizeof(WORD) * m_nNumVertex,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	// 頂点データの範囲をロックし、頂点バッファへのポインタ取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	// 縦をカウント
	for (int nDepth = 0; nDepth < Box_Depth + 1; nDepth++)
	{
		// 横をカウント
		for (int nWide = 0; nWide < Box_Width + 1; nWide++)
		{
			///////////////////////////////////////////////ナミナミ～～///////////////////////////////////////////////
			// 頂点座標の設定
			//m_pVtx[0].pos = D3DXVECTOR3((-WhileX * Width) / 2 + WhileX * nWide, cosf(D3DX_PI / 6 * nDepth + fDivide) * WhileY, (WhileZ / 2 * Depth) - WhileZ * nDepth);

			//// 頂点座標の設定
			//m_pVtx[0].pos.x = (-WhileX * Box_Width) / 2 + WhileX * nWide;
			//m_pVtx[0].pos.y = (WhileZ / 2 * Box_Depth) - WhileZ * nDepth;
			//m_pVtx[0].pos.z = 0.0f;
			// 座標の設定
			m_pVtx[0].pos.x = m_pos.x + WhileX * nWide;
			m_pVtx[0].pos.y = m_pos.y + WhileZ * nDepth;
			m_pVtx[0].pos.z = 0.0f;

			// 1.0で固定、同次座標
			m_pVtx[0].rhw = 1.0f;


			// 頂点カラー
			m_pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ描写の位置
			m_pVtx[0].tex = D3DXVECTOR2(1.0f * nWide, 1.0f * nDepth);

			m_pVtx++;
		}
	}

	// 頂点データをアンロック
	m_pVtxBuff->Unlock();

	// 頂点データの範囲をロックし、頂点バッファへのポインタ取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	//////////////////////////////法線ベクトル//////////////////////////////
	// 縦をカウント
	for (int nDepth = 0; nDepth < Box_Depth + 1; nDepth++)
	{
		// 横をカウント
		for (int nWide = 0; nWide < Box_Width + 1; nWide++)
		{
			// インデックス番号
			int nIdx = nDepth * (Box_Width + 1) + nWide;

			// 端は計算させない
			if (nWide != Box_Width && nDepth != Box_Depth)
			{
				m_vectorA = m_pVtx[StartBox].pos - m_pVtx[EndBox].pos;										// 一つ目のベクトル
				m_vectorB = m_pVtx[StartBox + 1].pos - m_pVtx[EndBox].pos;									// 二つ目のベクトル
				m_vectorC = m_pVtx[EndBox + 1].pos - m_pVtx[EndBox].pos;									// 三つ目のベクトル
				D3DXVec3Cross(&m_aVecA[nNumber], &m_vectorA, &m_vectorB);									// 二つのベクトルの直交ベクトル
				D3DXVec3Normalize(&m_aVecA[nNumber], &m_aVecA[nNumber]);									// 正規化する
				D3DXVec3Cross(&m_aVecB[nNumber], &m_vectorB, &m_vectorC);									// 二つのベクトルの直交ベクトル
				D3DXVec3Normalize(&m_aVecB[nNumber], &m_aVecB[nNumber]);									// 正規化する

																											// 配列の番号1プラスする
				StartBox++;
				EndBox++;
				nNumber++;
			}

		}
		// 配列の番号1プラスする
		StartBox++;
		EndBox++;
	}

	// 頂点データをアンロック
	m_pVtxBuff->Unlock();

	WORD *pIdx;				// インデックスバッファへのポインタ

							// インデックスバッファをロックしインデックスデータへのポインタを取得
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	// 縦をカウント
	for (int nCntY = 0; nCntY < Box_Depth; nCntY++)
	{
		// 一行目じゃないとき
		if (nCntY != 0)
		{
			pIdx[0] = (Box_Width + 1) + nCntY * (Box_Width + 1);
			pIdx++;
		}
		// 横をカウント
		for (int nCntX = 0; nCntX < Box_Width + 1; nCntX++)
		{
			pIdx[0] = (Box_Width + 1) + nCntX + nCntY * (Box_Width + 1);
			pIdx[1] = nCntX + nCntY * (Box_Width + 1);
			pIdx += 2;
		}

		// 縮退ポリゴン
		if (nCntY + 1 < Box_Depth)
		{
			pIdx[0] = Box_Width + nCntY * (Box_Width + 1);
			pIdx++;
		}
	}

	// インデックスバッファをアンロックする
	m_pIdxBuff->Unlock();

	return S_OK;
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CBox::Uninit(void)
{
	// 頂点バッファの開放
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// インデックスバッファの開放
	if (m_pIdxBuff != NULL)
	{
		m_pIdxBuff->Release();
		m_pIdxBuff = NULL;
	}
}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CBox::Update(void)
{
	VERTEX_2D *m_pVtx;

	// キーボード取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// 再初期化
	nNumber = 0;										// 配列の番号
	StartBox = Box_Width + 1;							// 始まる箱
	EndBox = 0;											// 引かれる箱

	// 動かす
	fDivide -= 0.05f;

	// 頂点データの範囲をロックし、頂点バッファへのポインタ取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	// 縦をカウント
	for (int nDepth = 0; nDepth < Box_Depth + 1; nDepth++)
	{
		// 横をカウント
		for (int nWide = 0; nWide < Box_Width + 1; nWide++)
		{
			///////////////////////////////////////////////ナミナミ～～///////////////////////////////////////////////
			// 頂点座標の設定
			//m_pVtx[0].pos = D3DXVECTOR3((-WhileX * Box_Width) / 2 + WhileX * nWide,
			//	(sinf(D3DX_PI / 6 * nDepth + fDivide) + 1) / 2 * WhileY,
			//	(WhileZ / 2 * Box_Depth) - WhileZ * nDepth);

			/////////////////////////////////////普通///////////////////////////////////////////////
		/*	 座標の設定
			m_pVtx[0].pos.x = WhileX * nWide;
			m_pVtx[0].pos.y = WhileZ * nDepth;
			m_pVtx[0].pos.z = 0.0f;
*/
			// 頂点カラー
			m_pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ描写の位置
			m_pVtx[0].tex = D3DXVECTOR2(1.0f * nWide, 1.0f * nDepth);

			m_pVtx++;
		}
	}

	// 頂点データをアンロック
	m_pVtxBuff->Unlock();

	// 頂点データの範囲をロックし、頂点バッファへのポインタ取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	//////////////////////////////法線ベクトル//////////////////////////////
	// 縦をカウント
	for (int nDepth = 0; nDepth < Box_Depth + 1; nDepth++)
	{
		// 横をカウント
		for (int nWide = 0; nWide < Box_Width + 1; nWide++)
		{
			// インデックス番号
			int nIdx = nDepth * (Box_Width + 1) + nWide;

			// 端は計算させない
			if (nWide != Box_Width && nDepth != Box_Depth)
			{
				m_vectorA = m_pVtx[StartBox].pos - m_pVtx[EndBox].pos;										// 一つ目のベクトル
				m_vectorB = m_pVtx[StartBox + 1].pos - m_pVtx[EndBox].pos;									// 二つ目のベクトル
				m_vectorC = m_pVtx[EndBox + 1].pos - m_pVtx[EndBox].pos;									// 三つ目のベクトル
				D3DXVec3Cross(&m_aVecA[nNumber], &m_vectorA, &m_vectorB);									// 二つのベクトルの直交ベクトル
				D3DXVec3Normalize(&m_aVecA[nNumber], &m_aVecA[nNumber]);									// 正規化する
				D3DXVec3Cross(&m_aVecB[nNumber], &m_vectorB, &m_vectorC);									// 二つのベクトルの直交ベクトル
				D3DXVec3Normalize(&m_aVecB[nNumber], &m_aVecB[nNumber]);									// 正規化する

																											// 配列の番号1プラスする
				StartBox++;
				EndBox++;
				nNumber++;
			}
		}
		// 配列の番号を1プラスする
		StartBox++;
		EndBox++;
	}

	// 頂点データをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CBox::Draw(void)
{
	// レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();			// レンダラーの情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans;							// 計算用マトリックス

															// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, m_pTexture);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVertex, 0, m_nNumPolygon);
}

//==================================================================================================================
// 位置設定
//==================================================================================================================
void CBox::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//==================================================================================================================
// ポリゴン生成
//==================================================================================================================
CBox *CBox::Create(void)
{
	// シーン動的に確保
	m_pBox = new CBox(CScene::PRIORITY_FLOOR);

	if (m_pBox != NULL)
	{
		// シーン初期化
		m_pBox->Init();
	}

	// 値を返す
	return m_pBox;
}

//==================================================================================================================
// テクスチャ情報ロード
//==================================================================================================================
HRESULT CBox::Load(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();						// レンダラーの情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();					// デバイスを取得する

																		// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,									// デバイスへのポインタ
		"data/TEXTURE/water.jpg",										// ファイルの名前
		&m_pTexture);													// 読み込むメモリー

	return S_OK;
}

//==================================================================================================================
// テクスチャ情報破棄
//==================================================================================================================
void CBox::Unload(void)
{
	// テクスチャの開放
	if (m_pTexture != NULL)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
}

//==================================================================================================================
// メッシュフィールド情報取得
//==================================================================================================================
CBox * CBox::GetBox(void)
{
	return m_pBox;
}

//==================================================================================================================
// 位置取得
//==================================================================================================================
D3DXVECTOR3 CBox::GetPos(void)
{
	return m_pos;
}

//==================================================================================================================
// 移動量取得
//==================================================================================================================
D3DXVECTOR3 CBox::GetMove(void)
{
	return m_move;
}

//==================================================================================================================
// 高さ算出用関数
//==================================================================================================================
float CBox::GetHeight(D3DXVECTOR3 pos)
{
	float Height = 0.0f;
	bool bRange = false;

	// 再初期化
	nNumber = 0;										// 配列の番号
	StartBox = Box_Width + 1;								// 始まる箱
	EndBox = 0;											// 引かれる箱

														// 頂点データの範囲をロックし、頂点バッファへのポインタ取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	//縦をカウント
	for (int nDepth = 0; nDepth < Box_Depth + 1; nDepth++)
	{
		// 横をカウント
		for (int nWide = 0; nWide < Box_Width + 1; nWide++)
		{
			// 範囲外にいるとき
			if (!bRange)
			{
				// 端は計算させない
				if (nWide != Box_Width && nDepth != Box_Depth)
				{
					// 全てのベクトルを出す
					D3DXVECTOR3 vecA = m_pVtx[StartBox].pos - m_pVtx[EndBox].pos;
					D3DXVECTOR3 vecB = m_pVtx[StartBox + 1].pos - m_pVtx[EndBox].pos;
					D3DXVECTOR3 vecC = m_pVtx[StartBox + 1].pos - m_pVtx[StartBox].pos;
					D3DXVECTOR3 vecD = m_pVtx[EndBox + 1].pos - m_pVtx[EndBox].pos;
					D3DXVECTOR3 vecE = m_pVtx[StartBox + 1].pos - m_pVtx[EndBox + 1].pos;

					// 引数のposから頂点までをベクトルにする
					D3DXVECTOR3 vec0 = pos - m_pVtx[EndBox].pos;
					D3DXVECTOR3 vec1 = pos - m_pVtx[EndBox + 1].pos;
					D3DXVECTOR3 vec2 = pos - m_pVtx[StartBox].pos;
					D3DXVECTOR3 vec3 = pos - m_pVtx[StartBox + 1].pos;

					// 直交ベクトル用変数
					D3DXVECTOR3 norB = D3DXVECTOR3(0, 0, 0);
					D3DXVECTOR3 norA = D3DXVECTOR3(0, 0, 0);
					D3DXVECTOR3 norC = D3DXVECTOR3(0, 0, 0);
					D3DXVECTOR3 norD = D3DXVECTOR3(0, 0, 0);
					D3DXVECTOR3 norE = D3DXVECTOR3(0, 0, 0);
					D3DXVECTOR3 norF = D3DXVECTOR3(0, 0, 0);

					//////直交ベクトル算出//////
					// 一つ目の三角形
					D3DXVec3Cross(&norA, &vecA, &vec0);										// 直交ベクトル算出
					D3DXVec3Cross(&norB, &vec0, &vecB);										// 直交ベクトル算出
					D3DXVec3Cross(&norC, &vecC, &vec2);										// 直交ベクトル算出

																							// 二つ目の三角形
					D3DXVec3Cross(&norD, &vecB, &vec0);										// 直交ベクトル算出
					D3DXVec3Cross(&norE, &vec0, &vecD);										// 直交ベクトル算出
					D3DXVec3Cross(&norF, &vec1, &vecE);										// 直交ベクトル算出

																							// 法線が面の表より上だったとき
					if (m_aVecA[nNumber] >= 0)
					{
						// 三角形の中にいるとき
						if (norA.y <= 0 && norB.y <= 0 && norC.y <= 0)
						{
							// メッシュフィールドの中にいる
							bRange = true;

							// 引数のposが頂点と同じであるとき
							if (pos == m_pVtx[StartBox].pos)
							{
								// 別な頂点を使って高さを算出
								// 指定された位置での高さを算出
								Height = m_pVtx[StartBox + 1].pos.y + (-m_aVecA[nNumber].x * vec3.x - m_aVecA[nNumber].z * vec3.z) / m_aVecA[nNumber].y;
							}
							else
							{
								// 指定された位置での高さを算出
								Height = m_pVtx[StartBox].pos.y + (-m_aVecA[nNumber].x * vec2.x - m_aVecA[nNumber].z * vec2.z) / m_aVecA[nNumber].y;
							}
						}
					}

					// 法線が面の表より上だったとき
					if (m_aVecB[nNumber] >= 0)
					{
						// 三角形の中にいるとき
						if (norD.y <= 0 && norE.y <= 0 && norF.y <= 0)
						{
							// メッシュフィールドの中にいる
							bRange = true;

							// 引数のposが頂点と同じであるとき
							if (pos == m_pVtx[EndBox + 1].pos)
							{
								// 別な頂点を使って高さを算出
								// 指定された位置での高さを算出
								Height = m_pVtx[EndBox].pos.y + (-m_aVecB[nNumber].x * (vec0.x) - m_aVecB[nNumber].z * (vec0.z)) / m_aVecB[nNumber].y;
							}
							else
							{
								// 指定された位置での高さを算出
								Height = m_pVtx[EndBox + 1].pos.y + (-m_aVecB[nNumber].x * (vec1.x) - m_aVecB[nNumber].z * (vec1.z)) / m_aVecB[nNumber].y;
							}
						}
					}

					// 配列の番号1プラスする
					StartBox++;
					EndBox++;
					nNumber++;
				}
			}
		}
		// 横終わったら配列の番号を1プラスする
		StartBox++;
		EndBox++;
	}

	// 頂点データをアンロック
	m_pVtxBuff->Unlock();

	return Height;
}

//==================================================================================================================
// 移動量設定
//==================================================================================================================
void CBox::SetMove(D3DXVECTOR3 move)
{
	m_move = move;
}

