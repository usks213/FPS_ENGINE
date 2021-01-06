#pragma once

//===== インクルード =====
#include <list>
#include <Windows.h>

//===== マクロ定義 =====
#define MAX_CELL (1+4+16+64)


//===== クラス定義 =====
// 四分木空間
template<class T>
class CCell
{
public:
	CCell()
	{

	}
	~CCell()
	{
		m_list.clear();
	}

	void ClearList()
	{
		m_list.clear();
	}

	std::list<T*>& GetList() { return m_list; }

	static void SetMapSize(float width, float height)
	{
		m_fUnit_W = width / (float)(1 << (m_uiLevel - 1));
		m_fUnit_H = height / (float)(1 << (m_uiLevel - 1));

		m_fLeft = -m_fUnit_W;
		m_fTop = -m_fUnit_H;
	}

	static void SetOffSet(float left, float top)
	{
		m_fLeftOffSet = left;
		m_fTopOffSet = top;
	}

	static float GetUnitW() { return m_fUnit_W; }
	static float GetUnitH() { return m_fUnit_H; }
	static unsigned int GetUnitLevel() { return m_uiLevel; }

	std::list<T*> m_list;

	// ビット分割関数
	static DWORD BitSeparate32(DWORD n)
	{
		n = (n | (n << 8)) & 0x00ff00ff;
		n = (n | (n << 4)) & 0x0f0f0f0f;
		n = (n | (n << 2)) & 0x33333333;
		return (n | (n << 1)) & 0x55555555;
	}

	// 2Dモートン空間番号算出関数
	static WORD Get2DMortonNumber(WORD x, WORD y)
	{
		return (WORD)(BitSeparate32(x) | (BitSeparate32(y) << 1));
	}

	// 座標→線形4分木要素番号変換関数
	static DWORD GetPointElem(float pos_x, float pos_y)
	{
		pos_x += CCell::GetUnitW() * 3;
		pos_y += CCell::GetUnitH() * 3;

		if (pos_x - m_fLeft - m_fLeftOffSet < 0 || pos_y - m_fTop - m_fTopOffSet < 0)
			return 0xffffffff;

		// 本当はフィールドの大きさとか
		return Get2DMortonNumber((WORD)((pos_x - m_fLeft - m_fLeftOffSet) / CCell::GetUnitW()),
			(WORD)((pos_y - m_fTop - m_fTopOffSet) / CCell::GetUnitH()));
	}

private:
	static float m_fUnit_W;		// 最小レベル空間の幅単位
	static float m_fUnit_H;		// 最小レベル空間の高単位
	static float m_fLeft;	// 領域の左側（X軸最小値）
	static float m_fTop;	// 領域の上側（Y軸最小値）

	static float m_fLeftOffSet;
	static float m_fTopOffSet;

	static const unsigned int m_uiLevel = 3;			// 最下位レベル
};

// 初期サイズ
template<class T>
float CCell<T>::m_fUnit_W = 100.0f;		// 最小レベル空間の幅単位
template<class T>
float CCell<T>::m_fUnit_H = 100.0f;		// 最小レベル空間の高単位
template<class T>
float CCell<T>::m_fLeft = 100.0f;		// 領域の左側（X軸最小値）
template<class T>
float CCell<T>::m_fTop = 100.0f;		// 領域の上側（Y軸最小値）
template<class T>
float CCell<T>::m_fLeftOffSet = 0.0f;		// 領域の左側オフセット（X軸最小値）
template<class T>
float CCell<T>::m_fTopOffSet = 0.0f;		// 領域の上側オフセット（Y軸最小値）