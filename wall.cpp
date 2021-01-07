//=============================================================================
//
// 壁処理 [wall.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "wall.h"
#include "manager.h"
#include "renderer.h"
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
CMeshWall::CMeshWall(CScene::PRIORITY obj = CScene::PRIORITY_WALL) : CScene(obj)
{
	SetObjType(CScene::PRIORITY_WALL);

	m_pVtxBuff = NULL;

	// 位置・回転の初期設定
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_bDebug = false;
	m_bCalculation = false;
}

//=============================================================================
// デストラクタ
//=============================================================================
CMeshWall::~CMeshWall()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CMeshWall::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得

	//総頂点数
	m_nNumVertex = (WALL_DEPTH_FIELD + 1) * (WALL_WIDE_FIELD + 1);

	//インデックス数
	m_nNumIndex = ((WALL_DEPTH_FIELD + 1) * 2) * WALL_DEPTH_FIELD + ((WALL_WIDE_FIELD - 1) * 2);

	//総ポリゴン数
	m_nNumPolygon = WALL_DEPTH_FIELD * WALL_WIDE_FIELD * 2 + 4 * (WALL_DEPTH_FIELD - 1);

	// 頂点情報の作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVertex, D3DUSAGE_WRITEONLY, FVF_VERTEX_3D, D3DPOOL_MANAGED, &m_pVtxBuff, NULL);

	//インデックスバッファを生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIndex, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIdxBuff, NULL);

	int nPolygon = WALL_DEPTH_FIELD * WALL_WIDE_FIELD * 2;

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
void CMeshWall::Uninit(void)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CMeshWall::Update(void)
{
#ifdef _DEBUG
	// デバッグ処理
	Debug();
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CMeshWall::Draw(void)
{
#ifdef _DEBUG
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
	//pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVertex, 0, m_nNumPolygon);

	//頂点フォーマットの設定
	pDevice->SetTexture(0, NULL);
#endif
}

//=============================================================================
// クリエイト処理
//=============================================================================
CMeshWall *CMeshWall::Create(void)
{
	CMeshWall *pMeshWall;
	pMeshWall = new CMeshWall(CScene::PRIORITY_WALL);

	if (pMeshWall != NULL)
	{// メッシュフィールドが存在していたとき
		pMeshWall->Init();			// 初期化処理
	}
	return pMeshWall;
}

//=============================================================================
// ロード処理
//=============================================================================
HRESULT CMeshWall::Load(void)
{
	return S_OK;
}

//=============================================================================
// 地形高低読込関数
//=============================================================================
void CMeshWall::LoadWall(char *add, bool bDebug)
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
						CMeshWall *pWall;
						pWall = CMeshWall::Create();										// 床の作成

						if (pWall != NULL)
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
									pWall->SetPosition(pos);
								}
								else if (strcmp(cHeadText, "CHECK_HEIGHT") == 0)
								{// 計算する必要のあるメッシュかどうか
									pWall->SetCalculation(true);
								}
								else if (strcmp(cHeadText, "VERTEXINFO") == 0)
								{//パーツ総数のとき
									pWall->MakeVertex(pFile, bDebug);			// 頂点情報の作成(ファイルから)
									pWall->CalculationNormalize();				// 法線の計算
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
// ポリゴンの床の高さを求める
//=============================================================================
bool CMeshWall::GetWallHit(CScene *pTarget, D3DXVECTOR3 &nol)
{
	VERTEX_3D *pVtx;										//頂点情報へのポインタ
	bool bHit = false;			// ヒットフラグ

	D3DXVECTOR3 FieldPos = GetPosition();

	D3DXVECTOR3 AB;
	D3DXVECTOR3 BC;

	D3DXVECTOR3 point1;
	D3DXVECTOR3 point2;
	D3DXVECTOR3 point3;

	D3DXVECTOR3 aWork[3];
	D3DXVECTOR3 aPlayer[3];
	float fHeight = 0;

	D3DXVECTOR3 pointA, pointB;

	pointA = pTarget->GetPosition();
	pointB = pTarget->GetPosOld();

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	////法線求める
	//for (int nDepth = 0; nDepth < WALL_DEPTH_FIELD + 1; nDepth++)
	//{
	//	for (int nWide = 0; nWide < WALL_WIDE_FIELD + 1; nWide++)
	//	{
	//		// ①線分と平面の衝突
	//		D3DXVECTOR3 v1, v2;
	//		v1 = pTarget->GetPosition() - pVtx[WALL_WIDE_FIELD + nWide + 1 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;
	//		v2 = pTarget->GetPosOld() - pVtx[WALL_WIDE_FIELD + nWide + 1 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;

	//		if (D3DXVec3Dot(&v1, &pVtx[(WALL_WIDE_FIELD + 1) * nDepth + nWide].nor) * D3DXVec3Dot(&v2, &pVtx[(WALL_WIDE_FIELD + 1) * nDepth + nWide].nor) <= 0)
	//		{
	//			// ②ある点から平面までの距離
	//			float d1 = 0.0f, d2 = 0.0f;
	//			d1 = CManager::DistanceForDotAndPlane(pointA, pVtx[WALL_WIDE_FIELD + nWide + 1 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos, pVtx[(WALL_WIDE_FIELD + 1) * nDepth + nWide].nor);
	//			d2 = CManager::DistanceForDotAndPlane(pointB, pVtx[WALL_WIDE_FIELD + nWide + 1 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos, pVtx[(WALL_WIDE_FIELD + 1) * nDepth + nWide].nor);

	//			float a = 0.0f;
	//			if (d1 != 0 && d2 != 0)
	//			{
	//				a = fabs(d1) / (fabs(d1) + fabs(d2));
	//			}

	//			D3DXVECTOR3 v3 = D3DXVECTOR3_ZERO;

	//			D3DXVECTOR3 AB = pTarget->GetPosOld() - pTarget->GetPosition();
	//			v3 = pTarget->GetPosition() + (AB * a);
	//			//v3 = (1 - a) * v1 + a * v2;

	//			pointA = v3;

	//			//差分の計算
	//			aWork[0] = (pVtx[WALL_WIDE_FIELD + nWide + 1 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos + FieldPos) - (pVtx[nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos + FieldPos);
	//			aPlayer[0] = v3 - (pVtx[WALL_WIDE_FIELD + nWide + 1 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos + FieldPos);

	//			aWork[1] = (pVtx[WALL_WIDE_FIELD + nWide + 2 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos + FieldPos) - (pVtx[WALL_WIDE_FIELD + nWide + 1 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos + FieldPos);
	//			aPlayer[1] = v3 - (pVtx[WALL_WIDE_FIELD + nWide + 2 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos + FieldPos);

	//			aWork[2] = (pVtx[nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos + FieldPos) - (pVtx[WALL_WIDE_FIELD + nWide + 2 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos + FieldPos);
	//			aPlayer[2] = v3 - (pVtx[nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos + FieldPos);

	//			float fAnswer[3];

	//			//法線
	//			fAnswer[0] = aWork[0].x * aPlayer[0].z - aWork[0].z * aPlayer[0].x;
	//			fAnswer[1] = aWork[1].x * aPlayer[1].z - aWork[1].z * aPlayer[1].x;
	//			fAnswer[2] = aWork[2].x * aPlayer[2].z - aWork[2].z * aPlayer[2].x;

	//			if ((fAnswer[0] > 0 && fAnswer[1] > 0 && fAnswer[2] > 0) || (fAnswer[0] <= 0 && fAnswer[1] <= 0 && fAnswer[2] <= 0))
	//			{
	//				OutputDebugString("FinalPhase\n");
	//				pTarget->SetPosition(v3);
	//				pTarget->SetPosOld(v3);
	//				return v3;
	//			}
	//		}
	//	}
	//}

	{
		for (int nDepth = 0; nDepth < WALL_DEPTH_FIELD; nDepth++)
		{
			for (int nWide = 0; nWide < WALL_WIDE_FIELD; nWide++)
			{

				D3DXVECTOR3 P = pVtx[nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;	// 平面上の点P
				D3DXVECTOR3 A = pTarget->GetPosition();		// 始点
				D3DXVECTOR3 B = pTarget->GetPosOld();		// 終点

				P.y = 0.0f;
				A.y = 0.0f;
				B.y = 0.0f;

				// PA PBベクトル
				D3DXVECTOR3 PA = D3DXVECTOR3(P.x - A.x, P.y - A.y, P.z - A.z);
				D3DXVECTOR3 PB = D3DXVECTOR3(P.x - B.x, P.y - B.y, P.z - B.z);

				D3DXVec3Normalize(&PA, &PA);
				D3DXVec3Normalize(&PB, &PB);

				// PA PBそれぞれを平面法線と内積
				float dot_PA = D3DXVec3Dot(&PA, &apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)]);
				float dot_PB = D3DXVec3Dot(&PB, &apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)]);

				// 誤差を吸収するための判定文
				if (fabs(dot_PA) < 0.000001) { dot_PA = 0.0f; }
				if (fabs(dot_PB) < 0.000001) { dot_PB = 0.0f; }

				// 交差判定
				if (dot_PA == 0.0f && dot_PB == 0.0f)
				{
					//両端が平面上にあり、交点を計算できない。
					continue;
				}
				else
				{
					if ((dot_PA >= 0.0f && dot_PB <= 0.0f))
						//(dot_PA <= 0.0f && dot_PB >= 0.0f))
					{

					}
					else
					{
						//交差していない
						continue;
					}
				}

				// 以下交点を求める

				D3DXVECTOR3 AB = B - A;

				// 交点とAの距離 : 交点とBの距離 = dot_PA : dot_PB
				float hiritu = fabs(dot_PA) / (fabs(dot_PA) + fabs(dot_PB));
				D3DXVECTOR3 Ans = A + (AB * hiritu);

				D3DXVECTOR3 aVertex[4];
				aVertex[0] = pVtx[WALL_WIDE_FIELD + nWide + 1 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;
				aVertex[1] = pVtx[WALL_WIDE_FIELD + nWide + 2 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;
				aVertex[2] = pVtx[nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;
				aVertex[3] = pVtx[1 + nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;

				//差分の計算
				aWork[0] = (aVertex[0] + FieldPos) - (aVertex[2] + FieldPos);
				aPlayer[0] = Ans - (aVertex[0] + FieldPos);

				aWork[1] = (aVertex[1] + FieldPos) - (aVertex[0] + FieldPos);
				aPlayer[1] = Ans - (aVertex[1] + FieldPos);

				aWork[2] = (aVertex[2] + FieldPos) - (aVertex[1] + FieldPos);
				aPlayer[2] = Ans - (aVertex[2] + FieldPos);

				float fAnswer[3];
				D3DXVECTOR3 clossAns[3];

				//法線
				fAnswer[0] = aWork[0].x * aPlayer[0].z - aWork[0].z * aPlayer[0].x - aWork[0].y * aPlayer[0].y;
				fAnswer[1] = aWork[1].x * aPlayer[1].z - aWork[1].z * aPlayer[1].x - aWork[1].y * aPlayer[1].y;
				fAnswer[2] = aWork[2].x * aPlayer[2].z - aWork[2].z * aPlayer[2].x - aWork[2].y * aPlayer[2].y;

				D3DXVec3Cross(&clossAns[0], &aWork[0], &aPlayer[0]);
				D3DXVec3Cross(&clossAns[1], &aWork[1], &aPlayer[1]);
				D3DXVec3Cross(&clossAns[2], &aWork[2], &aPlayer[2]);

				float dot_12 = D3DXVec3Dot(&clossAns[0], &clossAns[1]);
				float dot_13 = D3DXVec3Dot(&clossAns[0], &clossAns[3]);

				float fDistance = CManager::GetDistance(pTarget->GetPosOld(), Ans);

				{
					//D3DXPLANE p;
					//D3DXPlaneFromPoints(&p, &aVertex[0], &aVertex[1], &aVertex[2]);
					////パラメトリック方程式の媒介変数” t "を解く。
					//FLOAT t = -((p.a * vP[1].x) + (p.b*vP[1].y) + (p.c*vP[1].z) + p.d) /
					//	(((p.a*vP[0].x) + (p.b*vP[0].y) + (p.c*vP[0].z)) - ((p.a*vP[1].x) + (p.b*vP[1].y) + (p.c*vP[1].z)));
					//// t が０から１の間であるなら交差していることになる（この時点では、まだ無限遠平面との交差）
					//if (t >= 0 && t <= 1.0)
					//{
					//	//交点座標を得る　ｔが分かっていれば両端点から簡単に求まる
					//	D3DXVECTOR3 v;
					//	v.x = t*vP[0].x + (1 - t)*vP[1].x;
					//	v.y = t*vP[0].y + (1 - t)*vP[1].y;
					//	v.z = t*vP[0].z + (1 - t)*vP[1].z;
					//	//交点が三角形の内か外かを判定　（ここで内部となれば、完全な交差となる）
					//	if (CCollider::IsInside(&v, &aVertex[0], &aVertex[1], &aVertex[2]))
					//	{
					//		return true;
					//	}
					//	return false;
					//}
				}

				if (CCollider::IsInside(&Ans, &aVertex[0], &aVertex[1], &aVertex[2]))
				{
					OutputDebugString("FinalPhase\n");
					D3DXVECTOR3 old = pTarget->GetPosOld();
					bHit = true;
					pTarget->SetPosition(D3DXVECTOR3(Ans.x, pTarget->GetPosition().y, Ans.z));
					pTarget->SetPosOld(old);
					nol = apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)];
					break;
					break;
				}

				////if ((fAnswer[0] >= 0 && fAnswer[1] >= 0 && fAnswer[2] >= 0) || (fAnswer[0] < 0 && fAnswer[1] < 0 && fAnswer[2] < 0))
				//{
				//	if ((clossAns[0] > 0 && clossAns[1] > 0 && clossAns[2] > 0) || (clossAns[0] <= 0 && clossAns[1] <= 0 && clossAns[2] <= 0))
				//	{
				//		if (dot_12 > 0 && dot_13 > 0)
				//		{
				//			OutputDebugString("FinalPhase\n");
				//			D3DXVECTOR3 old = pTarget->GetPosOld();
				//			bHit = true;
				//			pTarget->SetPosition(Ans);
				//			pTarget->SetPosOld(old);
				//			nol = apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)];
				//			break;
				//			break;
				//		}
				//	}
				//}

				P = pVtx[nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;	// 平面上の点P
				A = pTarget->GetPosition();		// 始点
				B = pTarget->GetPosOld();		// 終点

				// PA PBベクトル
				PA = D3DXVECTOR3(P.x - A.x, P.y - A.y, P.z - A.z);
				PB = D3DXVECTOR3(P.x - B.x, P.y - B.y, P.z - B.z);

				// PA PBそれぞれを平面法線と内積
				dot_PA = D3DXVec3Dot(&PA, &apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1]);
				dot_PB = D3DXVec3Dot(&PB, &apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1]);

				// 誤差を吸収するための判定文
				if (fabs(dot_PA) < 0.000001) { dot_PA = 0.0f; }
				if (fabs(dot_PB) < 0.000001) { dot_PB = 0.0f; }

				// 交差判定
				if (dot_PA == 0.0f && dot_PB == 0.0f)
				{
					//両端が平面上にあり、交点を計算できない。
					continue;
				}
				else
				{
					if ((dot_PA >= 0.0f && dot_PB <= 0.0f) ||
						(dot_PA <= 0.0f && dot_PB >= 0.0f))
					{

					}
					else
					{
						//交差していない
						continue;
					}
				}

				// 交点とAの距離 : 交点とBの距離 = dot_PA : dot_PB
				hiritu = fabs(dot_PA) / (fabs(dot_PA) + fabs(dot_PB));
				Ans = A + (AB * hiritu);

				//差分の計算
				aWork[0] = (aVertex[2] + FieldPos) - (aVertex[3] + FieldPos);
				aPlayer[0] = Ans - (aVertex[3] + FieldPos);

				aWork[1] = (aVertex[3] + FieldPos) - (aVertex[1] + FieldPos);
				aPlayer[1] = Ans - (aVertex[1] + FieldPos);

				aWork[2] = (aVertex[1] + FieldPos) - (aVertex[2] + FieldPos);
				aPlayer[2] = Ans - (aVertex[2] + FieldPos);

				//法線
				fAnswer[0] = aWork[0].x * aPlayer[0].z - aWork[0].z * aPlayer[0].x - aWork[0].y * aPlayer[0].y;
				fAnswer[1] = aWork[1].x * aPlayer[1].z - aWork[1].z * aPlayer[1].x - aWork[1].y * aPlayer[1].y;
				fAnswer[2] = aWork[2].x * aPlayer[2].z - aWork[2].z * aPlayer[2].x - aWork[2].y * aPlayer[2].y;

				D3DXVec3Cross(&clossAns[0], &aWork[0], &aPlayer[0]);
				D3DXVec3Cross(&clossAns[1], &aWork[1], &aPlayer[1]);
				D3DXVec3Cross(&clossAns[2], &aWork[2], &aPlayer[2]);

				dot_12 = D3DXVec3Dot(&clossAns[0], &clossAns[1]);
				dot_13 = D3DXVec3Dot(&clossAns[0], &clossAns[3]);

				if (CCollider::IsInside(&Ans, &aVertex[1], &aVertex[2], &aVertex[3]))
				{
					OutputDebugString("FinalPhase\n");
					D3DXVECTOR3 old = pTarget->GetPosOld();
					bHit = true;
					pTarget->SetPosition(D3DXVECTOR3(Ans.x, pTarget->GetPosition().y, Ans.z));
					pTarget->SetPosOld(old);
					nol = apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)];
					break;
					break;
				}

				////if ((fAnswer[0] >= 0 && fAnswer[1] >= 0 && fAnswer[2] >= 0) || (fAnswer[0] < 0 && fAnswer[1] < 0 && fAnswer[2] < 0))
				//{
				//	if ((clossAns[0] > 0 && clossAns[1] > 0 && clossAns[2] > 0) || (clossAns[0] <= 0 && clossAns[1] <= 0 && clossAns[2] <= 0))
				//	{
				//		if (dot_12 > 0 && dot_13 > 0)
				//		{
				//			//OutputDebugString("FinalPhase\n");
				//			//D3DXVECTOR3 old = pTarget->GetPosOld();
				//			//bHit = true;
				//			//pTarget->SetPosition(Ans);
				//			//pTarget->SetPosOld(old);
				//			//nol = apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)];
				//			break;
				//			break;
				//		}
				//	}
				//}
			}
		}
	}

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
	return bHit;
}

//=============================================================================
// ポリゴンの壁通過判定
//=============================================================================
bool CMeshWall::GetWallTest(CScene * pTarget, D3DXVECTOR3 & nol, D3DXVECTOR3 &rot)
{
	VERTEX_3D *pVtx;										//頂点情報へのポインタ
	bool bHit = false;			// ヒットフラグ

	D3DXVECTOR3 FieldPos = GetPosition();

	D3DXVECTOR3 AB;
	D3DXVECTOR3 BC;

	D3DXVECTOR3 point1;
	D3DXVECTOR3 point2;
	D3DXVECTOR3 point3;

	D3DXVECTOR3 aWork[3];
	D3DXVECTOR3 aPlayer[3];
	float fHeight = 0;

	D3DXVECTOR3 pointA, pointB;

	pointA = pTarget->GetPosition();
	pointB = pTarget->GetPosOld();

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nDepth = 0; nDepth < WALL_DEPTH_FIELD; nDepth++)
	{
		for (int nWide = 0; nWide < WALL_WIDE_FIELD; nWide++)
		{

			D3DXVECTOR3 P = pVtx[nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;	// 平面上の点P
			D3DXVECTOR3 A = pTarget->GetPosition();		// 始点
			D3DXVECTOR3 B = pTarget->GetPosOld();		// 終点

			P.y = 0.0f;
			A.y = 0.0f;
			B.y = 0.0f;

			// PA PBベクトル
			D3DXVECTOR3 PA = D3DXVECTOR3(P.x - A.x, P.y - A.y, P.z - A.z);
			D3DXVECTOR3 PB = D3DXVECTOR3(P.x - B.x, P.y - B.y, P.z - B.z);

			D3DXVec3Normalize(&PA, &PA);
			D3DXVec3Normalize(&PB, &PB);

			// PA PBそれぞれを平面法線と内積
			float dot_PA = D3DXVec3Dot(&PA, &apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)]);
			float dot_PB = D3DXVec3Dot(&PB, &apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)]);

			// 誤差を吸収するための判定文
			if (fabs(dot_PA) < 0.000001) { dot_PA = 0.0f; }
			if (fabs(dot_PB) < 0.000001) { dot_PB = 0.0f; }

			// 交差判定
			if (dot_PA == 0.0f && dot_PB == 0.0f)
			{
				//両端が平面上にあり、交点を計算できない。
				continue;
			}
			else
			{
				if ((dot_PA >= 0.0f && dot_PB <= 0.0f))
					//(dot_PA <= 0.0f && dot_PB >= 0.0f))
				{

				}
				else
				{
					//交差していない
					continue;
				}
			}

			// 以下交点を求める

			D3DXVECTOR3 AB = B - A;

			// 交点とAの距離 : 交点とBの距離 = dot_PA : dot_PB
			float hiritu = fabs(dot_PA) / (fabs(dot_PA) + fabs(dot_PB));
			D3DXVECTOR3 Ans = A + (AB * hiritu);

			D3DXVECTOR3 aVertex[4];
			aVertex[0] = pVtx[WALL_WIDE_FIELD + nWide + 1 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;
			aVertex[1] = pVtx[WALL_WIDE_FIELD + nWide + 2 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;
			aVertex[2] = pVtx[nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;
			aVertex[3] = pVtx[1 + nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;

			//差分の計算
			aWork[0] = (aVertex[0] + FieldPos) - (aVertex[2] + FieldPos);
			aPlayer[0] = Ans - (aVertex[0] + FieldPos);

			aWork[1] = (aVertex[1] + FieldPos) - (aVertex[0] + FieldPos);
			aPlayer[1] = Ans - (aVertex[1] + FieldPos);

			aWork[2] = (aVertex[2] + FieldPos) - (aVertex[1] + FieldPos);
			aPlayer[2] = Ans - (aVertex[2] + FieldPos);

			float fAnswer[3];
			D3DXVECTOR3 clossAns[3];

			//法線
			fAnswer[0] = aWork[0].x * aPlayer[0].z - aWork[0].z * aPlayer[0].x - aWork[0].y * aPlayer[0].y;
			fAnswer[1] = aWork[1].x * aPlayer[1].z - aWork[1].z * aPlayer[1].x - aWork[1].y * aPlayer[1].y;
			fAnswer[2] = aWork[2].x * aPlayer[2].z - aWork[2].z * aPlayer[2].x - aWork[2].y * aPlayer[2].y;

			D3DXVec3Cross(&clossAns[0], &aWork[0], &aPlayer[0]);
			D3DXVec3Cross(&clossAns[1], &aWork[1], &aPlayer[1]);
			D3DXVec3Cross(&clossAns[2], &aWork[2], &aPlayer[2]);

			float dot_12 = D3DXVec3Dot(&clossAns[0], &clossAns[1]);
			float dot_13 = D3DXVec3Dot(&clossAns[0], &clossAns[3]);

			float fDistance = CManager::GetDistance(pTarget->GetPosOld(), Ans);

			RAY ray;
			ray.vPoint[0] = pTarget->GetPosOld();
			ray.vPoint[1] = pTarget->GetPosition();
			ray.fYaw = 0.0f;
			ray.vPos = D3DXVECTOR3_ZERO;

			POLYGON poly;
			poly.vPoint[0] = aVertex[0];
			poly.vPoint[1] = aVertex[1];
			poly.vPoint[2] = aVertex[2];

			if (Intersect(&ray, &poly))
			{
				OutputDebugString("FinalPhase\n");
				D3DXVECTOR3 old = pTarget->GetPosOld();
				bHit = true;
				pTarget->SetPosition(D3DXVECTOR3(Ans.x, pTarget->GetPosition().y, Ans.z));
				pTarget->SetPosOld(old);
				nol = apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)];
				break;
				break;
			}

			////if ((fAnswer[0] >= 0 && fAnswer[1] >= 0 && fAnswer[2] >= 0) || (fAnswer[0] < 0 && fAnswer[1] < 0 && fAnswer[2] < 0))
			//{
			//	if ((clossAns[0] > 0 && clossAns[1] > 0 && clossAns[2] > 0) || (clossAns[0] <= 0 && clossAns[1] <= 0 && clossAns[2] <= 0))
			//	{
			//		if (dot_12 > 0 && dot_13 > 0)
			//		{
			//			OutputDebugString("FinalPhase\n");
			//			D3DXVECTOR3 old = pTarget->GetPosOld();
			//			bHit = true;
			//			pTarget->SetPosition(Ans);
			//			pTarget->SetPosOld(old);
			//			nol = apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)];
			//			break;
			//			break;
			//		}
			//	}
			//}

			P = pVtx[nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;	// 平面上の点P
			A = pTarget->GetPosition();		// 始点
			B = pTarget->GetPosOld();		// 終点

											// PA PBベクトル
			PA = D3DXVECTOR3(P.x - A.x, P.y - A.y, P.z - A.z);
			PB = D3DXVECTOR3(P.x - B.x, P.y - B.y, P.z - B.z);

			// PA PBそれぞれを平面法線と内積
			dot_PA = D3DXVec3Dot(&PA, &apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1]);
			dot_PB = D3DXVec3Dot(&PB, &apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1]);

			// 誤差を吸収するための判定文
			if (fabs(dot_PA) < 0.000001) { dot_PA = 0.0f; }
			if (fabs(dot_PB) < 0.000001) { dot_PB = 0.0f; }

			// 交差判定
			if (dot_PA == 0.0f && dot_PB == 0.0f)
			{
				//両端が平面上にあり、交点を計算できない。
				continue;
			}
			else
			{
				if ((dot_PA >= 0.0f && dot_PB <= 0.0f) ||
					(dot_PA <= 0.0f && dot_PB >= 0.0f))
				{

				}
				else
				{
					//交差していない
					continue;
				}
			}

			// 交点とAの距離 : 交点とBの距離 = dot_PA : dot_PB
			hiritu = fabs(dot_PA) / (fabs(dot_PA) + fabs(dot_PB));
			Ans = A + (AB * hiritu);

			//差分の計算
			aWork[0] = (aVertex[2] + FieldPos) - (aVertex[3] + FieldPos);
			aPlayer[0] = Ans - (aVertex[3] + FieldPos);

			aWork[1] = (aVertex[3] + FieldPos) - (aVertex[1] + FieldPos);
			aPlayer[1] = Ans - (aVertex[1] + FieldPos);

			aWork[2] = (aVertex[1] + FieldPos) - (aVertex[2] + FieldPos);
			aPlayer[2] = Ans - (aVertex[2] + FieldPos);

			//法線
			fAnswer[0] = aWork[0].x * aPlayer[0].z - aWork[0].z * aPlayer[0].x - aWork[0].y * aPlayer[0].y;
			fAnswer[1] = aWork[1].x * aPlayer[1].z - aWork[1].z * aPlayer[1].x - aWork[1].y * aPlayer[1].y;
			fAnswer[2] = aWork[2].x * aPlayer[2].z - aWork[2].z * aPlayer[2].x - aWork[2].y * aPlayer[2].y;

			D3DXVec3Cross(&clossAns[0], &aWork[0], &aPlayer[0]);
			D3DXVec3Cross(&clossAns[1], &aWork[1], &aPlayer[1]);
			D3DXVec3Cross(&clossAns[2], &aWork[2], &aPlayer[2]);

			dot_12 = D3DXVec3Dot(&clossAns[0], &clossAns[1]);
			dot_13 = D3DXVec3Dot(&clossAns[0], &clossAns[3]);

			ray.vPoint[0] = pTarget->GetPosOld();
			ray.vPoint[1] = pTarget->GetPosition();
			ray.fYaw = 0.0f;
			ray.vPos = D3DXVECTOR3_ZERO;

			poly.vPoint[0] = aVertex[1];
			poly.vPoint[1] = aVertex[2];
			poly.vPoint[2] = aVertex[3];

			if (Intersect(&ray, &poly))
			{
				OutputDebugString("FinalPhase\n");
				D3DXVECTOR3 old = pTarget->GetPosOld();
				bHit = true;
				pTarget->SetPosition(D3DXVECTOR3(Ans.x, pTarget->GetPosition().y, Ans.z));
				pTarget->SetPosOld(old);
				nol = apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)];
				break;
				break;
			}

			////if ((fAnswer[0] >= 0 && fAnswer[1] >= 0 && fAnswer[2] >= 0) || (fAnswer[0] < 0 && fAnswer[1] < 0 && fAnswer[2] < 0))
			//{
			//	if ((clossAns[0] > 0 && clossAns[1] > 0 && clossAns[2] > 0) || (clossAns[0] <= 0 && clossAns[1] <= 0 && clossAns[2] <= 0))
			//	{
			//		if (dot_12 > 0 && dot_13 > 0)
			//		{
			//			//OutputDebugString("FinalPhase\n");
			//			//D3DXVECTOR3 old = pTarget->GetPosOld();
			//			//bHit = true;
			//			//pTarget->SetPosition(Ans);
			//			//pTarget->SetPosOld(old);
			//			//nol = apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)];
			//			break;
			//			break;
			//		}
			//	}
			//}
		}
	}

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
	return bHit;
}

//=============================================================================
// X軸とZ軸の当たり判定
//=============================================================================
bool CMeshWall::SphereModel(D3DXVECTOR3 C1, D3DXVECTOR3 C2, float R1)
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
bool CMeshWall::CollisionRange(D3DXVECTOR3 pos)
{
	D3DXVECTOR3 FieldPos = GetPosition();
	bool bIn = false;

	if (FieldPos.x - (WALL_WIDE_FIELD * SIZE) / 2 < pos.x)
	{
		if (FieldPos.x + (WALL_WIDE_FIELD * SIZE) / 2 > pos.x)
		{
			if (FieldPos.z - (WALL_WIDE_FIELD * SIZE) / 2 < pos.z)
			{
				if (FieldPos.z + (WALL_WIDE_FIELD * SIZE) / 2 > pos.z)
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
bool CMeshWall::CollisionRangeToCircle(D3DXVECTOR3 pos, float fRadius)
{
	D3DXVECTOR3 FieldPos = GetPosition();
	bool bIn = false;																// 当たり判定 の初期化

	// 左右の半径込の当たり判定
	if ((FieldPos.x - (WALL_WIDE_FIELD * SIZE) / 2) - fRadius < pos.x)
	{// 左と点
		if ((FieldPos.x + (WALL_WIDE_FIELD * SIZE) / 2) + fRadius > pos.x)
		{// 右と点
			if (FieldPos.z - (WALL_WIDE_FIELD * SIZE) / 2 < pos.z)
			{// 奥と点
				if (FieldPos.z + (WALL_WIDE_FIELD * SIZE) / 2 > pos.z)
				{// 手前と点
					bIn = true;														// 当たり判定 のフラグを立てる
				}
			}
		}
	}
	if (!bIn)
	{// 当たり判定 のフラグが立っていなかったとき
	 // 奥行き半径込の当たり判定
		if (FieldPos.x - (WALL_WIDE_FIELD * SIZE) / 2 < pos.x)
		{// 左と点
			if (FieldPos.x + (WALL_WIDE_FIELD * SIZE) / 2 > pos.x)
			{// 右と点
				if ((FieldPos.z - (WALL_WIDE_FIELD * SIZE) / 2) - fRadius < pos.z)
				{// 奥と点
					if ((FieldPos.z + (WALL_WIDE_FIELD * SIZE) / 2) + fRadius > pos.z)
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
bool CMeshWall::CollisionCircle(D3DXVECTOR3 pos, float fRadius)
{
	D3DXVECTOR3 FieldPos = GetPosition();
	bool bIn = false;		// 当たり判定の初期化

	if (((FieldPos.x - (WALL_WIDE_FIELD * SIZE) / 2 - pos.x) * (FieldPos.x - (WALL_WIDE_FIELD * SIZE) / 2 - pos.x)) +
		((FieldPos.z - (WALL_DEPTH_FIELD * SIZE) / 2 - pos.z) * (FieldPos.z - (WALL_DEPTH_FIELD * SIZE) / 2 - pos.z)) <= fRadius * fRadius)
	{// 左上と点
		bIn = true;			// 当たり判定 のフラグを立てる
	}
	else if (((FieldPos.x + (WALL_WIDE_FIELD * SIZE) / 2 - pos.x) * (FieldPos.x + (WALL_WIDE_FIELD * SIZE) / 2 - pos.x)) +
		((FieldPos.z - (WALL_DEPTH_FIELD * SIZE) / 2 - pos.z) * (FieldPos.z - (WALL_DEPTH_FIELD * SIZE) / 2 - pos.z)) <= fRadius * fRadius)
	{// 右上と点
		bIn = true;			// 当たり判定 のフラグを立てる
	}
	else if (((FieldPos.x - (WALL_WIDE_FIELD * SIZE) / 2 - pos.x) * (FieldPos.x - (WALL_WIDE_FIELD * SIZE) / 2 - pos.x)) +
		((FieldPos.z + (WALL_DEPTH_FIELD * SIZE) / 2 - pos.z) * (FieldPos.z + (WALL_DEPTH_FIELD * SIZE) / 2 - pos.z)) <= fRadius * fRadius)
	{// 左下と点
		bIn = true;			// 当たり判定 のフラグを立てる
	}
	else if (((FieldPos.x + (WALL_WIDE_FIELD * SIZE) / 2 - pos.x) * (FieldPos.x + (WALL_WIDE_FIELD * SIZE) / 2 - pos.x)) +
		((FieldPos.z + (WALL_DEPTH_FIELD * SIZE) / 2 - pos.z) * (FieldPos.z + (WALL_DEPTH_FIELD * SIZE) / 2 - pos.z)) <= fRadius * fRadius)
	{// 右下と点
		bIn = true;			// 当たり判定 のフラグを立てる
	}

	return bIn;			// 当たり判定 フラグの状態を返す
}

//=============================================================================
// 法線の計算
//=============================================================================
void CMeshWall::CalculationNormalize(void)
{
	VERTEX_3D *pVtx;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 法線求める
	for (int nDepth = 0; nDepth < WALL_DEPTH_FIELD; nDepth++)
	{
		for (int nWide = 0; nWide < WALL_WIDE_FIELD; nWide++)
		{
			D3DXVECTOR3 AB = pVtx[nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos - pVtx[WALL_WIDE_FIELD + 1 + nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;
			D3DXVECTOR3 BC = pVtx[nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos - pVtx[WALL_WIDE_FIELD + nWide + 2 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;

			D3DXVec3Cross(&apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)], &BC, &AB);
			D3DXVec3Normalize(&apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)], &apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)]);

			AB = pVtx[nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos - pVtx[WALL_WIDE_FIELD + nWide + 2 + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;
			BC = pVtx[nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos - pVtx[1 + nWide + ((WALL_WIDE_FIELD + 1) * nDepth)].pos;

			D3DXVec3Cross(&apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1], &BC, &AB);
			D3DXVec3Normalize(&apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1], &apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1]);
		}
	}

	// 頂点データのアンロック
	m_pVtxBuff->Unlock();

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 法線平均求める
	for (int nDepth = 0; nDepth < WALL_DEPTH_FIELD + 1; nDepth++)
	{
		for (int nWide = 0; nWide < WALL_WIDE_FIELD + 1; nWide++)
		{
			// 一番上の段
			if (nDepth == 0)
			{
				if (nWide != WALL_WIDE_FIELD)
				{//最後尾ではない場合
					if (nWide == 0)
					{// 一番最初
					 // よこ
						pVtx[(WALL_WIDE_FIELD + 1) * nDepth + nWide].nor =
							(apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)] +
								apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1])
							/ 2;
					}
					else if (nWide != WALL_WIDE_FIELD)
					{
						pVtx[(WALL_WIDE_FIELD + 1) * nDepth + nWide].nor =
							(apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide) - 1] +
								apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)] +
								apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1])
							/ 3;
					}
				}
			}
			else if (nDepth != WALL_DEPTH_FIELD)
			{// 一番下の段ではない場合
				if (nWide == 0)
				{// 一番端っこだったとき
					pVtx[(WALL_WIDE_FIELD + 1) * nDepth + nWide].nor =
						(apNor[(nDepth - 1) * 2 * WALL_WIDE_FIELD + (2 * nWide)] +
							apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)] +
							apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1])
						/ 3;
				}
				else if (nWide == WALL_WIDE_FIELD)
				{// 横最後
					pVtx[(WALL_WIDE_FIELD + 1) * nDepth + nWide].nor =
						(apNor[(nDepth - 1) * 2 * WALL_WIDE_FIELD + (2 * nWide) - 2] +
							apNor[(nDepth - 1) * 2 * WALL_WIDE_FIELD + (2 * nWide) - 1] +
							apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide) - 1])
						/ 3;
				}
				else
				{// 真ん中
					pVtx[(WALL_WIDE_FIELD + 1) * nDepth + nWide].nor =
						(apNor[(nDepth - 1) * 2 * WALL_WIDE_FIELD + (2 * nWide) - 2] +
							apNor[(nDepth - 1) * 2 * WALL_WIDE_FIELD + (2 * nWide) - 1] +
							apNor[(nDepth - 1) * 2 * WALL_WIDE_FIELD + (2 * nWide)] +
							apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide) - 1] +
							apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide)] +
							apNor[((WALL_WIDE_FIELD * 2) * nDepth) + (2 * nWide) + 1])
						/ 6;
				}
			}
			else
			{
				if (nWide == 0)
				{// 一番最初ではない場合

				}
				else if (nWide == WALL_WIDE_FIELD)
				{
					pVtx[(WALL_WIDE_FIELD + 1) * nDepth + nWide].nor =
						(apNor[(nDepth - 1) * 2 * WALL_WIDE_FIELD + (2 * nWide) - 2] +
							apNor[(nDepth - 1) * 2 * WALL_WIDE_FIELD + (2 * nWide) - 1])
						/ 2;
				}
				else
				{
					pVtx[(WALL_WIDE_FIELD + 1) * nDepth + nWide].nor =
						(apNor[(nDepth - 1) * 2 * WALL_WIDE_FIELD + (2 * nWide) - 2] +
							apNor[(nDepth - 1) * 2 * WALL_WIDE_FIELD + (2 * nWide) - 1] +
							apNor[(nDepth - 1) * 2 * WALL_WIDE_FIELD + (2 * nWide)])
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
void CMeshWall::CreateRand(const int &nWide, const int &nDepth, const D3DXVECTOR3 &createPos)
{
	for (int nCntDepth = 0; nCntDepth < nDepth; nCntDepth++)
	{
		for (int nCntWide = 0; nCntWide < nWide; nCntWide++)
		{
			CMeshWall *pMeshWall = CMeshWall::Create();

			if (pMeshWall != NULL)
			{
				D3DXVECTOR3 pos;

				pos.x = (WALL_WIDE_FIELD * SIZE) * nCntWide - (((WALL_WIDE_FIELD * SIZE) * nWide) / 2) + ((WALL_WIDE_FIELD * SIZE) / 2) + createPos.x;
				pos.y = createPos.y;
				pos.z = (WALL_DEPTH_FIELD * SIZE) * nCntDepth - (((WALL_DEPTH_FIELD * SIZE) * nDepth) / 2) + ((WALL_DEPTH_FIELD * SIZE) / 2) + createPos.z;

				pMeshWall->SetPosition(pos);
			}
		}
	}
}

//=============================================================================
// インスペクターの表示
//=============================================================================
void CMeshWall::ShowInspector(void)
{

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
void CMeshWall::MakeVertex(FILE *pFile, bool bDebug)
{
	VERTEX_3D *pVtx;
	char cReadText[128];															// 文字
	char cHeadText[128];															// 比較
	float aData[7];																	// 答え

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nDepth = 0; nDepth < WALL_DEPTH_FIELD + 1; nDepth++)
	{
		for (int nWide = 0; nWide < WALL_WIDE_FIELD + 1; nWide++)
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
				pVtx[0].pos = D3DXVECTOR3((-SIZE * WALL_WIDE_FIELD) / 2 + SIZE * nWide, 0.0f, (SIZE * WALL_DEPTH_FIELD) / 2 - SIZE * nDepth);

				if (!bDebug)
				{
					//頂点カラー
					pVtx[0].col = m_col;
				}
				else
				{
					if (nDepth >= WALL_DEPTH_FIELD || nWide >= WALL_WIDE_FIELD)
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
void CMeshWall::MakeIndex(void)
{
	WORD *pIdx;									//インデックスデータへポインタ
	int nCount = 0;

	//インデックスバッファをロックし、インデックスデータへのポインタ
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	for (int nCntY = 0; nCntY < WALL_DEPTH_FIELD; nCntY++)
	{
		if (nCntY != 0)
		{
			pIdx[0] = (WALL_WIDE_FIELD + 1) + nCntY * (WALL_WIDE_FIELD + 1);
			pIdx++;
			nCount++;
		}

		for (int nCntX = 0; nCntX < WALL_WIDE_FIELD + 1; nCntX++)
		{
			pIdx[0] = (WALL_WIDE_FIELD + 1) + nCntX + nCntY * (WALL_WIDE_FIELD + 1);
			pIdx[1] = nCntX + nCntY * (WALL_WIDE_FIELD + 1);
			pIdx += 2;
			nCount += 2;
		}

		if (WALL_DEPTH_FIELD > nCntY + 1)
		{
			pIdx[0] = WALL_WIDE_FIELD + nCntY * (WALL_WIDE_FIELD + 1);
			pIdx++;
			nCount++;
		}
	}

	//インデックスバッファをロックし、インデックスデータへのポインタ
	m_pIdxBuff->Unlock();
}

//=============================================================================
// 地形高低保存関数
//=============================================================================
void CMeshWall::SaveWall(CWrite *pWrite)
{
	VERTEX_3D *pVtx;				// 頂点情報へのポインタ

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nDepth = 0; nDepth < WALL_DEPTH_FIELD + 1; nDepth++)
	{
		for (int nWide = 0; nWide < WALL_WIDE_FIELD + 1; nWide++)
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

//=============================================================================
// 壁の頂点情報編集処理
//=============================================================================
void CMeshWall::EditWallVertex(void)
{
	VERTEX_3D *pVtx;				// 頂点情報へのポインタ

	//頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nDepth = 0; nDepth < WALL_DEPTH_FIELD + 1; nDepth++)
	{
		for (int nWide = 0; nWide < WALL_WIDE_FIELD + 1; nWide++)
		{
			//頂点座標の取得
			char label[16];
			sprintf(label, "pos %d", nWide + WALL_DEPTH_FIELD * nDepth);
			D3DXVECTOR3 pos = pVtx[0].pos;
			ImGui::DragFloat3(label, (float*)&pos);
			pVtx[0].pos = pos;
			pVtx++;				// ポインタをシフト
		}
	}

	//頂点データのアンロック
	m_pVtxBuff->Unlock();
}

//
//BOOL Intersect(RAY* pRay,POLYGON* pPoly)
//無限遠平面と線分の交差を判定 （交差している場合はtrueを、していない場合はfalseを返す）
bool CMeshWall::Intersect(RAY* pRay, POLYGON* pPoly)
{
	D3DXVECTOR3 vP[2];

	//線分（レイ）の端点を得る
	{
		vP[0] = pRay->vPoint[0];
		vP[1] = pRay->vPoint[1];

		D3DXMATRIX mRotation;
		D3DXMatrixRotationY(&mRotation, pRay->fYaw);
		D3DXVec3TransformCoord(&vP[0], &vP[0], &mRotation);
		D3DXVec3TransformCoord(&vP[1], &vP[1], &mRotation);
		vP[0] += pRay->vPos;
		vP[1] += pRay->vPos;
	}
	//判定部分
	{
		D3DXPLANE p;
		D3DXPlaneFromPoints(&p, &pPoly->vPoint[0], &pPoly->vPoint[1], &pPoly->vPoint[2]);
		//パラメトリック方程式の媒介変数” t "を解く。
		FLOAT t = -((p.a * vP[1].x) + (p.b*vP[1].y) + (p.c*vP[1].z) + p.d) /
			(((p.a*vP[0].x) + (p.b*vP[0].y) + (p.c*vP[0].z)) - ((p.a*vP[1].x) + (p.b*vP[1].y) + (p.c*vP[1].z)));
		// t が０から１の間であるなら交差していることになる（この時点では、まだ無限遠平面との交差）
		if (t >= 0 && t <= 1.0)
		{
			//交点座標を得る　ｔが分かっていれば両端点から簡単に求まる
			D3DXVECTOR3 v;
			v.x = t*vP[0].x + (1 - t)*vP[1].x;
			v.y = t*vP[0].y + (1 - t)*vP[1].y;
			v.z = t*vP[0].z + (1 - t)*vP[1].z;
			//交点が三角形の内か外かを判定　（ここで内部となれば、完全な交差となる）
			if (CCollider::IsInside(&v, &pPoly->vPoint[0], &pPoly->vPoint[1], &pPoly->vPoint[2]) == true)
			{
				return true;
			}
			return false;
		}
	}
	return false;
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CMeshWall::Debug(void)
{

}
#endif