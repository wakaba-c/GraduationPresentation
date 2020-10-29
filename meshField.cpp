//=============================================================================
//
// メッシュフィールド処理 [meshField.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "meshField.h"
#include "manager.h"
#include "renderer.h"
#include "inputMouse.h"
#include "inputKeyboard.h"
#include "camera.h"
#include "circle.h"
#include "tree.h"
#include "collider.h"
#include "write.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define FLOOR_INCREASE (5.0f)

//=============================================================================
// コンストラクタ
//=============================================================================
CMeshField::CMeshField(CScene::PRIORITY obj = CScene::PRIORITY_FLOOR) : CScene(obj)
{
	SetObjType(CScene::PRIORITY_FLOOR);

	// 位置・回転の初期設定
	m_rot = D3DXVECTOR3(0.0f, 1.57f, 0.0f);
	m_col = D3DXCOLOR(0.0f, 1.0f, 0.4f, 1.0f);
	m_bDebug = false;
	m_bCalculation = false;

	for (int nCount = 0; nCount < DEPTH_FIELD * WIDE_FIELD * 2; nCount++)
	{
		m_randType[nCount] = RANDTYPE_NONE;
	}
}

//=============================================================================
// デストラクタ
//=============================================================================
CMeshField::~CMeshField()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CMeshField::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得

   //総頂点数
	m_nNumVertex = (DEPTH_FIELD + 1) * (WIDE_FIELD + 1);

	//インデックス数
	m_nNumIndex = ((DEPTH_FIELD + 1) * 2) * DEPTH_FIELD + ((WIDE_FIELD - 1) * 2);

	//総ポリゴン数
	m_nNumPolygon = DEPTH_FIELD * WIDE_FIELD * 2 + 4 * (DEPTH_FIELD - 1);

	// 頂点情報の作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVertex, D3DUSAGE_WRITEONLY, FVF_VERTEX_3D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);

	//インデックスバッファを生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIndex, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIdxBuff, NULL);

	int nPolygon = DEPTH_FIELD * WIDE_FIELD * 2;

	// 頂点情報の作成
	MakeVertex(NULL, false);

	// 頂点インデックスの作成
	MakeIndex();

	// 法線の計算
	CalculationNormalize();

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CMeshField::Uninit(void)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CMeshField::Update(void)
{
#ifdef _DEBUG
	// デバッグ処理
	Debug();
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CMeshField::Draw(void)
{
	D3DXVECTOR3 pos = GetPosition();
	CRenderer *pRenderer = CManager::GetRenderer();

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得
	D3DXMATRIX	mtxRot, mtxTrans;				//計算用マトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点バッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//頂点フォーマットの設定
	pDevice->SetTexture(0, NULL);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVertex, 0, m_nNumPolygon);

	//頂点フォーマットの設定
	pDevice->SetTexture(0, NULL);
}

//=============================================================================
// クリエイト処理
//=============================================================================
CMeshField *CMeshField::Create(void)
{
	CMeshField *pMeshField;
	pMeshField = new CMeshField(CScene::PRIORITY_FLOOR);

	if (pMeshField != NULL)
	{// メッシュフィールドが存在していたとき
		pMeshField->Init();			// 初期化処理
	}
	return pMeshField;
}

//=============================================================================
// ロード処理
//=============================================================================
HRESULT CMeshField::Load(void)
{
	return S_OK;
}

//=============================================================================
// 地形高低読込関数
//=============================================================================
void CMeshField::LoadRand(char *add, bool bDebug)
{
	FILE *pFile = NULL;																	// ファイル
	char cReadText[128];															// 文字
	char cHeadText[128];															// 比較
	char cDie[128];

	pFile = fopen(add, "r");				// ファイルを開くまたは作る

	if (pFile != NULL)						//ファイルが読み込めた場合
	{
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
					if (strcmp(cHeadText, "FIELDSET") == 0)
					{//キャラクターの初期情報のとき
						CMeshField *pField;
						pField = CMeshField::Create();										// 床の作成

						if (pField != NULL)
						{
							//エンドキャラクターセットが来るまでループ
							while (strcmp(cHeadText, "END_FIELDSET") != 0)
							{
								fgets(cReadText, sizeof(cReadText), pFile);
								sscanf(cReadText, "%s", &cHeadText);

								if (strcmp(cHeadText, "POS") == 0)
								{//パーツ総数のとき
									D3DXVECTOR3 pos;
									sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
										&pos.x,
										&pos.y,
										&pos.z);

									// 位置の設定
									pField->SetPosition(pos);
								}
								else if (strcmp(cHeadText, "CHECK_HEIGHT") == 0)
								{// 計算する必要のあるメッシュかどうか
									pField->SetCalculation(true);
								}
								else if (strcmp(cHeadText, "VERTEXINFO") == 0)
								{//パーツ総数のとき
									pField->MakeVertex(pFile, bDebug);			// 頂点情報の作成(ファイルから)
									pField->CalculationNormalize();				// 法線の計算
								}
							}
						}
					}
				}
			}
		}
		fclose(pFile);				// ファイルを閉じる
	}
	else
	{
		MessageBox(NULL, "地面情報のアクセス失敗！", "WARNING", MB_ICONWARNING);	// メッセージボックスの生成
	}
}

//=============================================================================
// 地面の種類を変更
//=============================================================================
void CMeshField::SetTexType(int nValue, RANDTYPE Type)
{
	m_randType[nValue] = Type;
}

//=============================================================================
// ポリゴンの床の高さを求める
//=============================================================================
float CMeshField::GetHeight(D3DXVECTOR3 pos)
{
	VERTEX_3D *pVtx;										//頂点情報へのポインタ

	D3DXVECTOR3 FieldPos = GetPosition();

	D3DXVECTOR3 AB;
	D3DXVECTOR3 BC;

	D3DXVECTOR3 point1;
	D3DXVECTOR3 point2;
	D3DXVECTOR3 point3;

	D3DXVECTOR3 aWork[3];
	D3DXVECTOR3 aPlayer[3];
	float fHeight = 0;
	float fAnswer[3];

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//法線求める
	for (int nDepth = 0; nDepth < DEPTH_FIELD; nDepth++)
	{
		for (int nWide = 0; nWide < WIDE_FIELD; nWide++)
		{
			point1 = pVtx[WIDE_FIELD + nWide + 1 + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos;
			point1.y = 0.0f;
			point2 = pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos;
			point2.y = 0.0f;
			point3 = pVtx[WIDE_FIELD + nWide + 2 + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos;
			point3.y = 0.0f;

			if (CManager::GetDistance(point1, D3DXVECTOR3(pos.x, 0.0f, pos.z)) <= 4000.0f ||
				CManager::GetDistance(point2, D3DXVECTOR3(pos.x, 0.0f, pos.z)) <= 4000.0f ||
				CManager::GetDistance(point3, D3DXVECTOR3(pos.x, 0.0f, pos.z)) <= 4000.0f)
			{
				//差分の計算
				aWork[0] = (pVtx[WIDE_FIELD + nWide + 1 + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos) - (pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos);
				aPlayer[0] = pos - (pVtx[WIDE_FIELD + nWide + 1 + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos);

				aWork[1] = (pVtx[WIDE_FIELD + nWide + 2 + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos) - (pVtx[WIDE_FIELD + nWide + 1 + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos);
				aPlayer[1] = pos - (pVtx[WIDE_FIELD + nWide + 2 + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos);

				aWork[2] = (pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos) - (pVtx[WIDE_FIELD + nWide + 2 + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos);
				aPlayer[2] = pos - (pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos);

				//法線
				fAnswer[0] = aWork[0].x * aPlayer[0].z - aWork[0].z * aPlayer[0].x;
				fAnswer[1] = aWork[1].x * aPlayer[1].z - aWork[1].z * aPlayer[1].x;
				fAnswer[2] = aWork[2].x * aPlayer[2].z - aWork[2].z * aPlayer[2].x;

				if ((fAnswer[0] > 0 && fAnswer[1] > 0 && fAnswer[2] > 0) || (fAnswer[0] <= 0 && fAnswer[1] <= 0 && fAnswer[2] <= 0))
				{
					//３点測量法
					m_type = m_randType[((WIDE_FIELD * 2) * nDepth) + (2 * nWide)];
					return fHeight = (pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos.y + FieldPos.y) - (1 / apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide)].y) * (apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide)].x * (pos.x - (pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos.x + FieldPos.x)) + apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide)].z * (pos.z - (pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos.z + FieldPos.z)));
				}

				//差分の計算
				aWork[0] = (pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos) - (pVtx[1 + nWide + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos);
				aPlayer[0] = pos - (pVtx[1 + nWide + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos);

				aWork[1] = (pVtx[1 + nWide + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos) - (pVtx[WIDE_FIELD + nWide + 2 + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos);
				aPlayer[1] = pos - (pVtx[WIDE_FIELD + nWide + 2 + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos);

				aWork[2] = (pVtx[WIDE_FIELD + nWide + 2 + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos) - (pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos);
				aPlayer[2] = pos - (pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos + FieldPos);

				//外積
				fAnswer[0] = aWork[0].x * aPlayer[0].z - aWork[0].z * aPlayer[0].x;
				fAnswer[1] = aWork[1].x * aPlayer[1].z - aWork[1].z * aPlayer[1].x;
				fAnswer[2] = aWork[2].x * aPlayer[2].z - aWork[2].z * aPlayer[2].x;

				//符号が一致したとき
				if ((fAnswer[0] >= 0 && fAnswer[1] >= 0 && fAnswer[2] >= 0) || (fAnswer[0] < 0 && fAnswer[1] < 0 && fAnswer[2] < 0))
				{
					//３点測量法
					m_type = m_randType[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1];
					return fHeight = (pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos.y + FieldPos.y) - (1 / apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1].y) * (apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1].x * (pos.x - (pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos.x + FieldPos.x)) + apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1].z * (pos.z - (pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos.z + FieldPos.z)));
				}
			}
		}
	}
	//頂点データのアンロック
	m_pVtxBuff->Unlock();

	m_type = RANDTYPE_NONE;
	return fHeight;
}

//=============================================================================
// X軸とZ軸の当たり判定
//=============================================================================
bool CMeshField::SphereModel(D3DXVECTOR3 C1, D3DXVECTOR3 C2, float R1)
{
	bool bDetection = false;
	//D3DXVECTOR3 C3;							//計算用
	D3DXVECTOR3 save = D3DXVECTOR3(0.0f, 0.0f, 0.0f);						//保存用
	D3DXVECTOR3 vec = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if ((C1.x - C2.x) * (C1.x - C2.x) + (C1.z - C2.z) * (C1.z - C2.z) <= R1 * R1)
	{
		bDetection = true;
	}
	return bDetection;
}

//====================================================================
// フィールド範囲内かどうか
//====================================================================
bool CMeshField::CollisionRange(D3DXVECTOR3 pos)
{
	D3DXVECTOR3 FieldPos = GetPosition();
	bool bIn = false;

	if (FieldPos.x - (WIDE_FIELD * SIZE) / 2 < pos.x)
	{
		if (FieldPos.x + (WIDE_FIELD * SIZE) / 2 > pos.x)
		{
			if (FieldPos.z - (WIDE_FIELD * SIZE) / 2 < pos.z)
			{
				if (FieldPos.z + (WIDE_FIELD * SIZE) / 2 > pos.z)
				{
					bIn = true;
				}
			}
		}
	}

	return bIn;
}

//=============================================================================
// 円の範囲内判定
//=============================================================================
bool CMeshField::CollisionRangeToCircle(D3DXVECTOR3 pos, float fRadius)
{
	D3DXVECTOR3 FieldPos = GetPosition();
	bool bIn = false;																// 当たり判定 の初期化

	// 左右の半径込の当たり判定
	if ((FieldPos.x - (WIDE_FIELD * SIZE) / 2) - fRadius < pos.x)
	{// 左と点
		if ((FieldPos.x + (WIDE_FIELD * SIZE) / 2) + fRadius > pos.x)
		{// 右と点
			if (FieldPos.z - (WIDE_FIELD * SIZE) / 2 < pos.z)
			{// 奥と点
				if (FieldPos.z + (WIDE_FIELD * SIZE) / 2 > pos.z)
				{// 手前と点
					bIn = true;														// 当たり判定 のフラグを立てる
				}
			}
		}
	}
	if (!bIn)
	{// 当たり判定 のフラグが立っていなかったとき
		// 奥行き半径込の当たり判定
		if (FieldPos.x - (WIDE_FIELD * SIZE) / 2 < pos.x)
		{// 左と点
			if (FieldPos.x + (WIDE_FIELD * SIZE) / 2 > pos.x)
			{// 右と点
				if ((FieldPos.z - (WIDE_FIELD * SIZE) / 2) - fRadius < pos.z)
				{// 奥と点
					if ((FieldPos.z + (WIDE_FIELD * SIZE) / 2) + fRadius > pos.z)
					{// 手前と点
						bIn = true;													// 当たり判定 のフラグを立てる
					}
				}
			}
		}
	}
	if (!bIn)
	{// 当たり判定 のフラグが立っていなかったとき
		// 角の円と点による当たり判定
		if (CollisionCircle(pos, fRadius))
		{// 当たり判定 のフラグが立ったとき
			bIn = true;
		}
	}

	return bIn;																		// 当たり判定 のフラグ状態を返す
}

//=============================================================================
// 点と円の当たり判定
//=============================================================================
bool CMeshField::CollisionCircle(D3DXVECTOR3 pos, float fRadius)
{
	D3DXVECTOR3 FieldPos = GetPosition();
	bool bIn = false;		// 当たり判定の初期化

	if (((FieldPos.x - (WIDE_FIELD * SIZE) / 2 - pos.x) * (FieldPos.x - (WIDE_FIELD * SIZE) / 2 - pos.x)) +
		((FieldPos.z - (DEPTH_FIELD * SIZE) / 2 - pos.z) * (FieldPos.z - (DEPTH_FIELD * SIZE) / 2 - pos.z)) <= fRadius * fRadius)
	{// 左上と点
		bIn = true;			// 当たり判定 のフラグを立てる
	}
	else if (((FieldPos.x + (WIDE_FIELD * SIZE) / 2 - pos.x) * (FieldPos.x + (WIDE_FIELD * SIZE) / 2 - pos.x)) +
		((FieldPos.z - (DEPTH_FIELD * SIZE) / 2 - pos.z) * (FieldPos.z - (DEPTH_FIELD * SIZE) / 2 - pos.z)) <= fRadius * fRadius)
	{// 右上と点
		bIn = true;			// 当たり判定 のフラグを立てる
	}
	else if (((FieldPos.x - (WIDE_FIELD * SIZE) / 2 - pos.x) * (FieldPos.x - (WIDE_FIELD * SIZE) / 2 - pos.x)) +
		((FieldPos.z + (DEPTH_FIELD * SIZE) / 2 - pos.z) * (FieldPos.z + (DEPTH_FIELD * SIZE) / 2 - pos.z)) <= fRadius * fRadius)
	{// 左下と点
		bIn = true;			// 当たり判定 のフラグを立てる
	}
	else if (((FieldPos.x + (WIDE_FIELD * SIZE) / 2 - pos.x) * (FieldPos.x + (WIDE_FIELD * SIZE) / 2 - pos.x)) +
		((FieldPos.z + (DEPTH_FIELD * SIZE) / 2 - pos.z) * (FieldPos.z + (DEPTH_FIELD * SIZE) / 2 - pos.z)) <= fRadius * fRadius)
	{// 右下と点
		bIn = true;			// 当たり判定 のフラグを立てる
	}

	return bIn;			// 当たり判定 フラグの状態を返す
}

//=============================================================================
// 地形の高低をリセット
//=============================================================================
void CMeshField::Reset(void)
{
	MakeVertex(NULL, false);									// 頂点情報の再作成
	CalculationNormalize();						// 法線ベクトルの計算
}

//=============================================================================
// 法線の計算
//=============================================================================
void CMeshField::CalculationNormalize(void)
{
	VERTEX_3D *pVtx;
	int nSand = 0;						// 砂カウンター
	int nGrass = 0;						// 草カウンター

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 法線求める
	for (int nDepth = 0; nDepth < DEPTH_FIELD; nDepth++)
	{
		for (int nWide = 0; nWide < WIDE_FIELD; nWide++)
		{
			D3DXVECTOR3 AB = pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos - pVtx[WIDE_FIELD + 1 + nWide + ((WIDE_FIELD + 1) * nDepth)].pos;
			D3DXVECTOR3 BC = pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos - pVtx[WIDE_FIELD + nWide + 2 + ((WIDE_FIELD + 1) * nDepth)].pos;

			D3DXVec3Cross(&apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide)], &BC, &AB);
			D3DXVec3Normalize(&apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide)], &apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide)]);

			AB = pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos - pVtx[WIDE_FIELD + nWide + 2 + ((WIDE_FIELD + 1) * nDepth)].pos;
			BC = pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].pos - pVtx[1 + nWide + ((WIDE_FIELD + 1) * nDepth)].pos;

			D3DXVec3Cross(&apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1], &BC, &AB);
			D3DXVec3Normalize(&apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1], &apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1]);

			// 地面のタイプを選択
			if (pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].col == D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f))
			{
				nSand++;
			}
			else
			{
				nGrass++;
			}

			if (pVtx[WIDE_FIELD + 1 + nWide + ((WIDE_FIELD + 1) * nDepth)].col == D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f))
			{
				nSand++;
			}
			else
			{
				nGrass++;
			}

			if (pVtx[WIDE_FIELD + nWide + 2 + ((WIDE_FIELD + 1) * nDepth)].col == D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f))
			{
				nSand++;
			}
			else
			{
				nGrass++;
			}

			if (nSand > nGrass)
			{
				m_randType[((WIDE_FIELD * 2) * nDepth) + (2 * nWide)] = RANDTYPE_SAND;
			}
			else
			{
				m_randType[((WIDE_FIELD * 2) * nDepth) + (2 * nWide)] = RANDTYPE_GRASS;
			}

			nSand = 0;
			nGrass = 0;

			// 地面のタイプを選択
			if (pVtx[nWide + ((WIDE_FIELD + 1) * nDepth)].col == D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f))
			{
				nSand++;
			}
			else
			{
				nGrass++;
			}

			if (pVtx[WIDE_FIELD + nWide + 2 + ((WIDE_FIELD + 1) * nDepth)].col == D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f))
			{
				nSand++;
			}
			else
			{
				nGrass++;
			}

			if (pVtx[1 + nWide + ((WIDE_FIELD + 1) * nDepth)].col == D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f))
			{
				nSand++;
			}
			else
			{
				nGrass++;
			}

			if (nSand > nGrass)
			{
				m_randType[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1] = RANDTYPE_SAND;
			}
			else
			{
				m_randType[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1] = RANDTYPE_GRASS;
			}

			nSand = 0;
			nGrass = 0;
		}
	}

	// 頂点データのアンロック
	m_pVtxBuff->Unlock();

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 法線平均求める
	for (int nDepth = 0; nDepth < DEPTH_FIELD + 1; nDepth++)
	{
		for (int nWide = 0; nWide < WIDE_FIELD + 1; nWide++)
		{
			// 一番上の段
			if (nDepth == 0)
			{
				if (nWide != WIDE_FIELD)
				{//最後尾ではない場合
					if (nWide == 0)
					{// 一番最初
					 // よこ
						pVtx[(WIDE_FIELD + 1) * nDepth + nWide].nor =
							(apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide)] +
								apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1])
							/ 2;
					}
					else if (nWide != WIDE_FIELD)
					{
						pVtx[(WIDE_FIELD + 1) * nDepth + nWide].nor =
							(apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) - 1] +
								apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide)] +
								apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1])
							/ 3;
					}
				}
			}
			else if (nDepth != DEPTH_FIELD)
			{// 一番下の段ではない場合
				if (nWide == 0)
				{// 一番端っこだったとき
					pVtx[(WIDE_FIELD + 1) * nDepth + nWide].nor =
						(apNor[(nDepth - 1) * 2 * WIDE_FIELD + (2 * nWide)] +
							apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide)] +
							apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1])
						/ 3;
				}
				else if (nWide == WIDE_FIELD)
				{// 横最後
					pVtx[(WIDE_FIELD + 1) * nDepth + nWide].nor =
						(apNor[(nDepth - 1) * 2 * WIDE_FIELD + (2 * nWide) - 2] +
							apNor[(nDepth - 1) * 2 * WIDE_FIELD + (2 * nWide) - 1] +
							apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) - 1])
						/ 3;
				}
				else
				{// 真ん中
					pVtx[(WIDE_FIELD + 1) * nDepth + nWide].nor =
						(apNor[(nDepth - 1) * 2 * WIDE_FIELD + (2 * nWide) - 2] +
							apNor[(nDepth - 1) * 2 * WIDE_FIELD + (2 * nWide) - 1] +
							apNor[(nDepth - 1) * 2 * WIDE_FIELD + (2 * nWide)] +
							apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) - 1] +
							apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide)] +
							apNor[((WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1])
						/ 6;
				}
			}
			else
			{
				if (nWide == 0)
				{// 一番最初ではない場合

				}
				else if (nWide == WIDE_FIELD)
				{
					pVtx[(WIDE_FIELD + 1) * nDepth + nWide].nor =
						(apNor[(nDepth - 1) * 2 * WIDE_FIELD + (2 * nWide) - 2] +
							apNor[(nDepth - 1) * 2 * WIDE_FIELD + (2 * nWide) - 1])
						/ 2;
				}
				else
				{
					pVtx[(WIDE_FIELD + 1) * nDepth + nWide].nor =
						(apNor[(nDepth - 1) * 2 * WIDE_FIELD + (2 * nWide) - 2] +
							apNor[(nDepth - 1) * 2 * WIDE_FIELD + (2 * nWide) - 1] +
							apNor[(nDepth - 1) * 2 * WIDE_FIELD + (2 * nWide)])
						/ 3;
				}
			}
		}
	}

	// 頂点データのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
// 床の生成処理
//=============================================================================
void CMeshField::CreateRand(const int &nWide, const int &nDepth, const D3DXVECTOR3 &createPos)
{
	for (int nCntDepth = 0; nCntDepth < nDepth; nCntDepth++)
	{
		for (int nCntWide = 0; nCntWide < nWide; nCntWide++)
		{
			CMeshField *pMeshField = CMeshField::Create();

			if (pMeshField != NULL)
			{
				D3DXVECTOR3 pos;

				pos.x = (WIDE_FIELD * SIZE) * nCntWide - (((WIDE_FIELD * SIZE) * nWide) / 2) + ((WIDE_FIELD * SIZE) / 2) + createPos.x;
				pos.y = createPos.y;
				pos.z = (DEPTH_FIELD * SIZE) * nCntDepth - (((DEPTH_FIELD * SIZE) * nDepth) / 2) + ((DEPTH_FIELD * SIZE) / 2) + createPos.z;

				pMeshField->SetPosition(pos);
			}
		}
	}
}

//=============================================================================
// 木配置読込関数
//=============================================================================
void CMeshField::LoadTree(void)
{
	FILE *pFile;																	// ファイル
	char cReadText[128];															// 文字
	char cHeadText[128];															// 比較
	float aData[4];																	// 答え

	pFile = fopen("data/stage/object.csv", "r");									// ファイルを開くまたは作る

	if (pFile != NULL)																//ファイルが読み込めた場合
	{
		fgets(cReadText, sizeof(cReadText), pFile);									//最初の行を飛ばす
		fgets(cReadText, sizeof(cReadText), pFile);									//2行目を飛ばす

		while (fgets(cReadText, sizeof(cReadText), pFile) != NULL)
		{
			//fgets(cReadText, sizeof(cReadText), pFile);							//最初の行を飛ばす
			sscanf(cReadText, "%s", &cHeadText);

			CManager::ConvertStringToFloat(cReadText, ",", aData);

			CTree *pTree = CTree::Create(TREETYPE_NORMAL);
			pTree->SetPosition(D3DXVECTOR3(aData[0], aData[1], aData[2]));
		}

		fclose(pFile);																// ファイルを閉じる
	}
	else
	{
		MessageBox(NULL, "オブジェクト情報のアクセス失敗！", "WARNING", MB_ICONWARNING);	// メッセージボックスの生成
	}
}

//=============================================================================
// インスペクターの表示
//=============================================================================
void CMeshField::ShowInspector(void)
{

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void CMeshField::MakeVertex(FILE *pFile, bool bDebug)
{
	VERTEX_3D *pVtx;
	char cReadText[128];															// 文字
	char cHeadText[128];															// 比較
	float aData[7];																	// 答え

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nDepth = 0; nDepth < DEPTH_FIELD + 1; nDepth++)
	{
		for (int nWide = 0; nWide < WIDE_FIELD + 1; nWide++)
		{
			if (pFile != NULL)
			{

				fgets(cReadText, sizeof(cReadText), pFile);							// データから1行取得
				sscanf(cReadText, "%s", &cHeadText);								// 行を読み込む

				// データを扱える形式に分解
				CManager::ConvertStringToFloat(cReadText, ",", aData);

				//頂点座標の設定
				pVtx[0].pos = D3DXVECTOR3(aData[0], aData[1], aData[2]);			// データを当てはめる

				D3DXCOLOR col = D3DXCOLOR(aData[3], aData[4], aData[5], aData[6]);

				//頂点カラー
				pVtx[0].col = col;	// データを当てはめる

				if (bDebug)
				{
					pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// データを当てはめる
				}

			}
			else
			{
				//頂点座標の設定
				pVtx[0].pos = D3DXVECTOR3((-SIZE * WIDE_FIELD) / 2 + SIZE * nWide, 0.0f, (SIZE * DEPTH_FIELD) / 2 - SIZE * nDepth);

				if (!bDebug)
				{
					//頂点カラー
					pVtx[0].col = m_col;
				}
				else
				{
					if (nDepth >= DEPTH_FIELD || nWide >= WIDE_FIELD)
					{
						pVtx[0].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);	// データを当てはめる
					}
					else
					{
						pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);	// データを当てはめる
					}
				}
			}

			//法線
			pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			//テクスチャ描写の位置
			pVtx[0].tex = D3DXVECTOR2(1.0f * nWide, 1.0f * nDepth);

			pVtx++;
		}
	}

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
// インデックス割り当て
//=============================================================================
void CMeshField::MakeIndex(void)
{
	WORD *pIdx;									//インデックスデータへポインタ
	int nCount = 0;

	//インデックスバッファをロックし、インデックスデータへのポインタ
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	for (int nCntY = 0; nCntY < DEPTH_FIELD; nCntY++)
	{
		if (nCntY != 0)
		{
			pIdx[0] = (WIDE_FIELD + 1) + nCntY * (WIDE_FIELD + 1);
			pIdx++;
			nCount++;
		}

		for (int nCntX = 0; nCntX < WIDE_FIELD + 1; nCntX++)
		{
			pIdx[0] = (WIDE_FIELD + 1) + nCntX + nCntY * (WIDE_FIELD + 1);
			pIdx[1] = nCntX + nCntY * (WIDE_FIELD + 1);
			pIdx += 2;
			nCount += 2;
		}

		if (DEPTH_FIELD > nCntY + 1)
		{
			pIdx[0] = WIDE_FIELD + nCntY * (WIDE_FIELD + 1);
			pIdx++;
			nCount++;
		}
	}

	//インデックスバッファをロックし、インデックスデータへのポインタ
	m_pIdxBuff->Unlock();
}

//=============================================================================
// デバッグ処理
//=============================================================================
void CMeshField::Debug(void)
{

}

//=============================================================================
// 地形編集関数
//=============================================================================
void CMeshField::CreateRand(D3DXVECTOR3 &worldPos, float fRadius)
{
	VERTEX_3D *pVtx;																// 頂点情報へのポインタ
	CInputMouse *pMouse = CManager::GetInputMouse();								// マウスの取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();						// キーボードの取得
	D3DXVECTOR3 pos = GetPosition();

	if (!pKeyboard->GetPressKeyboard(DIK_LALT))
	{// 左Altキーが押されていないとき
		if (pKeyboard->GetPressKeyboard(DIK_R))
		{// Rキーが押されているとき
			if (pMouse->GetReleaseMouse(MOUSE_LEFT))
			{// マウスの左ボタンが離されたとき
				CalculationNormalize();														// 法線の計算
			}
			else if (pMouse->GetPressMouse(MOUSE_LEFT))
			{// マウスの左ボタンが押されているとき
					// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
				m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

				//床の頂点全部を参照
				for (int nCount = 0; nCount < m_nNumVertex; nCount++)
				{
					if (SphereModel(worldPos, pVtx[nCount].pos + pos, fRadius))
					{// マウスのワールド座標から指定した半径までで当てはまる頂点があったとき
						pVtx[nCount].pos.y += FLOOR_INCREASE;
					}
				}

				//頂点データのアンロック
				m_pVtxBuff->Unlock();
			}
			else if (pMouse->GetPressMouse(MOUSE_RIGHT))
			{// マウスの右ボタンが押されているとき
				// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
				m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

				// 床の頂点全部を参照
				for (int nCount = 0; nCount < m_nNumVertex; nCount++)
				{
					if (SphereModel(worldPos, pVtx[nCount].pos + pos, fRadius))
					{// マウスのワールド座標から指定した半径までで当てはまる頂点があったとき
						pVtx[nCount].pos.y -= FLOOR_INCREASE;									// 頂点の高さを下げる
					}
				}

				// 頂点データのアンロック
				m_pVtxBuff->Unlock();
			}
		}
	}
}

//=============================================================================
// 頂点色変更関数
//=============================================================================
void CMeshField::Paint(D3DXVECTOR3 &worldPos, float fRadius)
{
	VERTEX_3D *pVtx;																// 頂点情報へのポインタ
	CInputMouse *pMouse = CManager::GetInputMouse();								// マウスの取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();						// キーボードの取得
	D3DXVECTOR3 pos = GetPosition();

	if (!pKeyboard->GetPressKeyboard(DIK_LALT))
	{// 左Altキーが押されていないとき
		if (pMouse->GetPressMouse(MOUSE_LEFT))
		{// マウスの左ボタンが押されているとき
		 // 頂点データの範囲をロックし、頂点バッファへのポインタを取得
			m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			//床の頂点全部を参照
			for (int nCount = 0; nCount < m_nNumVertex; nCount++)
			{
				if (SphereModel(worldPos, pVtx[nCount].pos + pos, fRadius))
				{// マウスのワールド座標から指定した半径までで当てはまる頂点があったとき
					pVtx[nCount].col = D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f);
				}
			}

			//頂点データのアンロック
			m_pVtxBuff->Unlock();
		}
		else if (pMouse->GetPressMouse(MOUSE_RIGHT))
		{// マウスの右ボタンが押されているとき
		 // 頂点データの範囲をロックし、頂点バッファへのポインタを取得
			m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// 床の頂点全部を参照
			for (int nCount = 0; nCount < m_nNumVertex; nCount++)
			{
				if (SphereModel(worldPos, pVtx[nCount].pos + pos, fRadius))
				{// マウスのワールド座標から指定した半径までで当てはまる頂点があったとき
					pVtx[nCount].col = D3DXCOLOR(0.0f, 1.0f, 0.4f, 1.0f);
				}
			}

			// 頂点データのアンロック
			m_pVtxBuff->Unlock();
		}
	}
}

//=============================================================================
// 地形高低保存関数
//=============================================================================
void CMeshField::SaveRand(CWrite *pWrite)
{
	VERTEX_3D *pVtx;				// 頂点情報へのポインタ

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nDepth = 0; nDepth < DEPTH_FIELD + 1; nDepth++)
	{
		for (int nWide = 0; nWide < WIDE_FIELD + 1; nWide++)
		{
			//頂点座標の取得
			D3DXVECTOR3 pos = pVtx[0].pos;
			D3DXCOLOR col = pVtx[0].col;

			// 頂点情報の書き込み
			pWrite->Write("		%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n", pos.x, pos.y, pos.z, col.r, col.g, col.b, col.a);
			pVtx++;				// ポインタをシフト
		}
	}

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
}