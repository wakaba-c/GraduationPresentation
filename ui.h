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

//=============================================================================
// 前方宣言
//=============================================================================
class CScene2D;

//=============================================================================
// クラス定義
//=============================================================================
class CUi
{
public:
	CUi();			// コンストラクタ
	~CUi();						// デストラクタ
	HRESULT Init(void);			// 初期化処理
	void Uninit(void);			// 開放処理
	void Update(void);			// 更新処理
	void Draw(void);			// 描画処理

	static CUi *Create(void);	// クリエイト処理
	static HRESULT Load(void);	// ロード処理
	bool LoadScript(const std::string &add);		// スクリプトデータロード処理

	void ShowInspector(void) {};

	void CreateTexture(std::string Add);
	void DeleteTexture(int nIndex);
	void SceneDebug(void);
	void SaveScript(std::string Add);

	void SetPosition(D3DXVECTOR3 pos);

	D3DXVECTOR3 GetPosition(void) { return m_pos; }

private:
	D3DXVECTOR3 m_pos;
	std::vector<CScene2D*> m_Asset;
};
#endif