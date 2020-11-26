//=============================================================================
//
// ユーザーインターフェース処理 [ui.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _UI_H_
#define _UI_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene.h"

//=============================================================================
// 前方宣言
//=============================================================================
class CScene2D;
class CCounter;

//=============================================================================
// クラス定義
//=============================================================================
class CUi : public CScene
{
public:
	typedef enum
	{
		OPERATIONTYPE_NONE = -1,
		OPERATIONTYPE_MOVE,
		OPERATIONTYPE_DELETE,
		OPERATIONTYPE_MAX
	} OPERATIONTYPE;

	CUi(PRIORITY obj);			// コンストラクタ
	~CUi();						// デストラクタ
	HRESULT Init(void);			// 初期化処理
	void Uninit(void);			// 開放処理
	void Update(void);			// 更新処理
	void Draw(void);			// 描画処理

	static CUi *Create(void);	// クリエイト処理
	static HRESULT Load(void);	// ロード処理
	bool LoadScript(const std::string &add);		// スクリプトデータロード処理

	void CreateTexture(std::string Add);
	void DeleteTexture(int nIndex);
	void SceneDebug(void);
	void SaveScript(std::string Add);

	void SetPosition(D3DXVECTOR3 pos);
	void SetColor(D3DXCOLOR col);
	void SetNextOperation(void);

	CCounter *GetCounter(std::string Tag);
	OPERATIONTYPE GetOpType(void) { return m_vOpType[m_nOpTypeNow]; }

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
	void PointMove(void);
	void Delete(void);

	typedef struct
	{
		D3DXVECTOR3 pos;			// 位置
		D3DXCOLOR col;				// 色
		int nFrame;					// 何フレームかけて処理するか
	} MOVEPOINT;

	D3DXCOLOR m_col;
	D3DXVECTOR3 m_currentPos;					// 1フレーム当たりの移動量
	D3DXCOLOR m_currentCol;						// 1フレーム当たりの色
	int m_nCurrentFrame;						// 現在のフレーム数
	int m_nCurrentPoint;						// 現在のポイント

	unsigned int m_nOpTypeNow;					// 現在の動作タイプ
	std::vector<OPERATIONTYPE> m_vOpType;		// 動作タイプ
	std::vector<MOVEPOINT> m_vPoint;			// 移動先
	std::vector<CScene2D*> m_vAsset;			// アセット
	std::map<std::string, CCounter*> m_CntMap;	// カウンタマップ
};
#endif