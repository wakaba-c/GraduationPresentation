//=============================================================================
//
// キャラクター処理 [character.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "character.h"
#include "game.h"
#include "manager.h"
#include "renderer.h"
#include "inputKeyboard.h"
#include "inputController.h"
#include "scene3D.h"
#include "meshField.h"
#include "model.h"
#include "meshCapsule.h"
#include "colliderSphere.h"
#include "meshCube.h"
#include "enemy.h"
#include "meshOrbit.h"
#include "camera.h"
#include "colliderBox.h"
#include "stage.h"
#include "sound.h"
#include "scene2D.h"
#include "effect.h"
#include "gauge2D.h"
#include "messageWindow.h"
#include "result.h"
#include "fade.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CCharacter::CCharacter(CScene::PRIORITY obj) : CScene(obj)
{
	m_pAnimation = NULL;
	m_apCurrentPos = NULL;								// 現在位置 の初期化
	m_apCurrentRot = NULL;								// 現在回転 の初期化
	m_pModel = NULL;

	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 回転の初期化
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動量の初期化
	m_dest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動先の初期化
	m_difference = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 差の初期化
	m_nNumParts = 0;									// パーツ総数の初期化
	m_nCurrentFrame = 0;								// フレーム数の初期化
	m_nCurrentKey = 0;									// キー数の初期化
	m_bAnimSwitch = false;								// アニメーションスイッチの初期化
	m_bAnimation = true;								// アニメーションフラグの初期化
	m_nActionCount = 0;									// アクションカウンタの初期化

}

//=============================================================================
// デストラクタ
//=============================================================================
CCharacter::~CCharacter()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CCharacter::Init(void)
{
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CCharacter::Uninit(void)
{
	if (m_apCurrentPos != NULL)
	{// パーツの1フレームごとの移動量を格納する変数がNULLじゃなかったとき
		delete[] m_apCurrentPos;
		m_apCurrentPos = NULL;
	}

	if (m_apCurrentRot != NULL)
	{// パーツの1フレームごとの回転量を格納する変数がNULLじゃなかったとき
		delete[] m_apCurrentRot;
		m_apCurrentRot = NULL;
	}

	if (m_pModel != NULL)
	{
		for (int nCount = 0; nCount < m_nNumParts; nCount++)
		{
			m_pModel[nCount].Uninit();
		}

		delete[] m_pModel;
		m_pModel = NULL;
	}

	if (m_pAnimation != NULL)
	{
		for (int nCount = 0; nCount < m_nMaxAnimationType; nCount++)
		{
			if (m_pAnimation[nCount].apKeyInfo != NULL)
			{//NULLではないとき
				delete[] m_pAnimation[nCount].apKeyInfo;										//パーツの総数分メモリを取得する
				m_pAnimation[nCount].apKeyInfo = NULL;											//パーツ分のアドレスにNULLを入れる
			}
		}

		delete[] m_pAnimation;
		m_pAnimation = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CCharacter::Update(void)
{
	// アニメーション再生停止のフラグが立っていた時
	if (m_bAnimation) Animation();

#ifdef _DEBUG
	Debug();
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CCharacter::Draw(void)
{
	D3DXVECTOR3 pos = GetPosition();
	D3DXMATRIX		mtxRot, mtxTrans;				//計算用マトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	for (int nCount = 0; nCount < m_nNumParts; nCount++)
	{
		if (m_pModel != NULL)
		{//パーツが存在する場合
			m_pModel[nCount].Draw(&m_mtxWorld);							//描画
		}
	}
}

//=============================================================================
// ロード処理
//=============================================================================
HRESULT CCharacter::Load(void)
{
	return S_OK;
}

//=============================================================================
// 当たり判定(trigger)
//=============================================================================
void CCharacter::OnTriggerEnter(CCollider *col)
{

}

//=============================================================================
// 当たり判定(collider)
//=============================================================================
void CCharacter::OnCollisionEnter(CCollider *col)
{

}

//========================================================================================
// パーツのロード
//========================================================================================
void CCharacter::LoadScript(std::string add, const int nMaxAnim)
{
	FILE *pFile;
	char cReadText[128];		//文字
	char cHeadText[128];		//比較
	char cDie[128];
	int nCntPointer = 0;			//ポインターの数値

	char sAdd[64];					//モデルのアドレス
	int nIndex;
	int nParent;
	int nCntMotion = 0;				//参照するポインタの値を初期化
	int nCntKey = 0;
	int nMaxModel = 0;
	int nCntMaya = 0;				//mayaのデータ参照

	if (!m_pAnimation)
	{// メモリ取得
		m_pAnimation = new ANIMATION[nMaxAnim];
		m_nMaxAnimationType = nMaxAnim;

		if (m_pAnimation != NULL)
		{
			// アニメーション情報の初期化
			for (int nCount = 0; nCount < nMaxAnim; nCount++)
			{
				m_pAnimation[nCount].apKeyInfo = NULL;			// アニメーション情報の初期化
			}
		}
	}

	//テキストデータロード
	pFile = fopen(add.c_str(), "r");

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
					if (strcmp(cHeadText, "NUM_MODEL") == 0)
					{//パーツ総数のとき
						if (m_pModel == NULL)
						{
							sscanf(cReadText, "%s %s %d", &cDie, &cDie, &nMaxModel);					//パーツの数を取得

							const int nNumParts = nMaxModel;											//パーツの数を定数として定義

							if (m_pModel == NULL)
							{//モデルを格納する変数がNULLだったとき
								m_pModel = new CModel[nNumParts];											//パーツの総数分メモリを取得する
							}

							if (m_apCurrentPos == NULL)
							{// パーツの1フレームごとの移動量を格納する変数がNULLだったとき
								m_apCurrentPos = new D3DXVECTOR3[nNumParts];
							}

							if (m_apCurrentRot == NULL)
							{// パーツの1フレームごとの回転量を格納する変数がNULLだったとき
								m_apCurrentRot = new D3DXVECTOR3[nNumParts];
							}

							for (int nCount = 0; nCount < nMaxAnim; nCount++)
							{
								if (m_pAnimation[nCount].apKeyInfo == NULL)
								{
									m_pAnimation[nCount].apKeyInfo = new KEY_INFO[nNumParts];			//パーツの総数分メモリを取得する
								}
							}

							m_nNumParts = nNumParts;													//パーツの総数を格納する
						}
					}
					else if (strcmp(cHeadText, "MODEL_FILENAME") == 0)
					{//パーツモデルのアドレス情報のとき
						sscanf(cReadText, "%s %s %s", &cDie, &cDie, &sAdd[0]);						//アドレスの取得

						if (m_pModel != NULL)
						{
							m_pModel[nCntPointer].Load(sAdd);										//パーツモデルのロード
							m_pModel[nCntPointer].Init();											//パーツモデルの初期化
							nCntPointer++;																//ポインターを4Bプラス
						}
					}

					else if (strcmp(cHeadText, "CHARACTERSET") == 0)
					{//キャラクターの初期情報のとき
						nCntPointer = 0;															//参照するポインタの値を初期化

																									//エンドキャラクターセットが来るまでループ
						while (strcmp(cHeadText, "END_CHARACTERSET") != 0)
						{
							fgets(cReadText, sizeof(cReadText), pFile);
							sscanf(cReadText, "%s", &cHeadText);

							//パーツセットだったら
							if (strcmp(cHeadText, "PARTSSET") == 0)
							{
								//エンドパーツセットが来るまでループ
								while (strcmp(cHeadText, "END_PARTSSET") != 0)
								{
									fgets(cReadText, sizeof(cReadText), pFile);
									sscanf(cReadText, "%s", &cHeadText);

									//インデックス
									if (strcmp(cHeadText, "INDEX") == 0)
									{
										sscanf(cReadText, "%s %s %d", &cDie, &cDie, &nIndex);			//インデックスの値を取得
										m_pModel[nCntPointer].SetIndex(nIndex);							//インデックスの値を現在参照中のパーツモデルに格納
									}

									//親
									if (strcmp(cHeadText, "PARENT") == 0)
									{
										sscanf(cReadText, "%s %s %d", &cDie, &cDie, &nParent);			//親の値を取得

										if (nParent == -1)
										{//親の値が-1だったとき
											m_pModel[nCntPointer].SetParent(NULL);				//NULLを格納する
										}
										else
										{//親の値が-1ではないとき
											if (m_pModel != NULL)
											{//モデルの中身がNULLではないとき
											 //現在ロードされているモデル分の中から対象が見つかるまで回す
												for (int nCount = 0; nCount < nMaxModel; nCount++)
												{
													if (nParent == m_pModel[nCount].GetIndex())
													{//パーツモデルのインデック値と自分の親の値が一致したとき
														m_pModel[nCntPointer].SetParent(&m_pModel[nCount]);				//親のポインタを格納する
														break;
													}
												}
											}
										}
									}

									if (strcmp(cHeadText, "POS") == 0)
									{//位置
										D3DXVECTOR3 originPos;
										sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
											&originPos.x,
											&originPos.y,
											&originPos.z);

										m_pModel[nCntPointer].SetPosPreset(originPos);
										m_pModel[nCntPointer].SetPosition(originPos);
									}

									if (strcmp(cHeadText, "ROT") == 0)
									{//回転
										D3DXVECTOR3 originRot;
										sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
											&originRot.x,
											&originRot.y,
											&originRot.z);

										m_pModel[nCntPointer].SetRotPreset(originRot);
										m_pModel[nCntPointer].SetRotation(originRot);
									}
								}

								nCntPointer++;											//参照するポインタの値を進める
							}
						}
					}
					else if (strcmp(cHeadText, "MOTIONSET") == 0)
					{//キャラクターセットだったら

					 //エンドキャラクターセットが来るまでループ
						while (strcmp(cHeadText, "END_MOTIONSET") != 0)
						{
							fgets(cReadText, sizeof(cReadText), pFile);
							sscanf(cReadText, "%s", &cHeadText);

							if (strcmp(cHeadText, "LOOP") == 0)
							{//ループするかどうか
								sscanf(cReadText, "%s %s %d", &cDie, &cDie, &m_pAnimation[nCntMotion].nLoop);
							}
							else if (strcmp(cHeadText, "NUM_KEY") == 0)
							{//キー数
								sscanf(cReadText, "%s %s %d", &cDie, &cDie, &m_pAnimation[nCntMotion].nMaxKey);
							}
							else if (strcmp(cHeadText, "SWITCHFRAME") == 0)
							{// フレーム間の移動フレーム数
								sscanf(cReadText, "%s %s %d", &cDie, &cDie, &m_pAnimation[nCntMotion].nSwitchFrame);
							}

							if (strcmp(cHeadText, "KEYSET") == 0)
							{//キーセットだったら

							 //参照するポインタの値を初期化
								nCntPointer = 0;
								//エンドキーセットが来るまでループ
								while (strcmp(cHeadText, "END_KEYSET") != 0)
								{
									fgets(cReadText, sizeof(cReadText), pFile);
									sscanf(cReadText, "%s", &cHeadText);

									//フレーム数
									if (strcmp(cHeadText, "FRAME") == 0)
									{
										sscanf(cReadText, "%s %s %d", &cDie, &cDie, &m_pAnimation[nCntMotion].apKeyInfo[nCntKey].nFrame);
									}

									//キーだったら
									if (strcmp(cHeadText, "KEY") == 0)
									{
										//エンドキーが来るまでループ
										while (strcmp(cHeadText, "END_KEY") != 0)
										{
											fgets(cReadText, sizeof(cReadText), pFile);
											sscanf(cReadText, "%s", &cHeadText);

											//位置
											if (strcmp(cHeadText, "POS") == 0)
											{
												sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].pos.x,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].pos.y,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].pos.z);
											}

											//回転
											if (strcmp(cHeadText, "ROT") == 0)
											{
												sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].rot.x,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].rot.y,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].rot.z);
											}
										}

										//パーツのキー情報を格納する場所を進める
										nCntPointer++;
									}
								}

								//keyのカウンターに1プラス
								nCntKey++;
								nCntPointer = 0;
							}
						}

						//アニメーションの種類を1プラスする
						nCntMotion++;
						nCntKey = 0;
					}
				}
			}
		}

		//ファイル閉
		fclose(pFile);
	}
	else
	{

	}
}

//=============================================================================
// アニメーション処理
//=============================================================================
void CCharacter::Animation(void)
{
	//最初のフレームの時
	if (m_nCurrentFrame == 0)
	{
		for (int nCount = 0; nCount < m_nNumParts; nCount++)
		{
			D3DXVECTOR3 pos = m_pModel[nCount].GetPosition();
			D3DXVECTOR3 rot = m_pModel[nCount].GetRotation();

			if (m_bAnimSwitch)
			{
				//posの計算((目標のkey + プリセットの配置) - 現在のキー)
				m_apCurrentPos[nCount] = ((m_pAnimation[m_nAnimationType].apKeyInfo[nCount].aKey[m_nCurrentKey].pos + m_pModel[nCount].GetPosPreset()) - pos) / 0.1f * (float)m_pAnimation[m_nAnimationType].nSwitchFrame;

				//rotの計算((目標のkey + プリセットの配置) - 現在のキー)
				m_apCurrentRot[nCount] = ((m_pAnimation[m_nAnimationType].apKeyInfo[nCount].aKey[m_nCurrentKey].rot + m_pModel[nCount].GetRotPreset()) - rot) / 0.1f * (float)m_pAnimation[m_nAnimationType].nSwitchFrame;
			}
			else
			{
				//posの計算((目標のkey + プリセットの配置) - 現在のキー)
				m_apCurrentPos[nCount] = ((m_pAnimation[m_nAnimationType].apKeyInfo[nCount].aKey[m_nCurrentKey].pos + m_pModel[nCount].GetPosPreset()) - pos) / (float)m_pAnimation[m_nAnimationType].apKeyInfo[m_nCurrentKey].nFrame;

				//rotの計算((目標のkey + プリセットの配置) - 現在のキー)
				m_apCurrentRot[nCount] = ((m_pAnimation[m_nAnimationType].apKeyInfo[nCount].aKey[m_nCurrentKey].rot + m_pModel[nCount].GetRotPreset()) - rot) / (float)m_pAnimation[m_nAnimationType].apKeyInfo[m_nCurrentKey].nFrame;
			}
		}
	}
	else
	{//それ以外のフレーム
		for (int nCount = 0; nCount < m_nNumParts; nCount++)
		{
			D3DXVECTOR3 pos = m_pModel[nCount].GetPosition();
			D3DXVECTOR3 rot = m_pModel[nCount].GetRotation();

			//rotの移動
			m_pModel[nCount].SetRotation(rot + m_apCurrentRot[nCount]);

			//posの移動
			m_pModel[nCount].SetPosition(pos + m_apCurrentPos[nCount]);
		}
	}

	if (m_pAnimation[m_nAnimationType].apKeyInfo != NULL)
	{// キー情報がNULLではないとき
		// フレームの最大数に満たない場合
		if (m_nCurrentFrame < m_pAnimation[m_nAnimationType].apKeyInfo[m_nCurrentKey].nFrame)
		{
			m_nCurrentFrame++;
		}
		//フレーム数の最大値に達した場合
		else if (m_nCurrentFrame >= m_pAnimation[m_nAnimationType].apKeyInfo[m_nCurrentKey].nFrame)
		{
			// 最大フレーム到達時の挙動
			BehaviorForMaxFrame();

			m_nCurrentKey++;			// キーの値に1プラス
			m_nCurrentFrame = 0;		// フレームのリセット
		}
	}

	// キー数の最大値に達した場合
	if (m_nCurrentKey >= m_pAnimation[m_nAnimationType].nMaxKey)
	{
		BehaviorForMaxKey();
	}
}

//=============================================================================
// アニメーションの切り替え
//=============================================================================
void CCharacter::AnimationSwitch(int nType)
{
	m_nAnimationTypeOld = m_nAnimationType;								// 前のアニメーションに今のアニメーションを入れる
	m_nAnimationType = nType;												// アニメーションの切り替え
	m_nCurrentFrame = 0;												// 現在のフレーム数をリセット
	m_nCurrentKey = 0;													// 現在のキー数をリセット
	m_bAnimation = true;
}

//=============================================================================
// キー数とフレーム数のリセット
//=============================================================================
void CCharacter::ResetKeyAndFrame(void)
{
	m_nCurrentKey = 0;
	m_nCurrentFrame = 0;
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CCharacter::Debug(void)
{

}
#endif