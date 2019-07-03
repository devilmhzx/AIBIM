/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:
*  简要描述:
*
*  当前版本: V1.0.0
*  作者:冯小翼
*  创建日期:2018-9-5
*  说明:
*      语法解析中Token类型，已经加工
**************************************************/
#pragma once
#include <string>
/*
* ETokenType 算数运算的操作中,所有类型全部默认为为浮点类型
*/
 
enum ETokenType
{
	/*空格*/
	E_BLANK,
	/*#号*/
	E_POUND,
    /*@号*/
    E_AT,
	/*$号*/
	E_DOLLAR,
	/*等于*/
	E_EQUL,
	/*加号(+)*/
	E_Plus,
	/*减号(-)*/
	E_Minus,
	/*乘号(*) */
	E_Mul,
	/*除号(/)*/
	E_Div,
	/*分号*/
	E_Semicolon,
	/*身份标识*/
	E_IDENTTY,
	/*靠墙*/
	E_CloseWall,
	/*靠窗*/
	E_CloseWin,
	/*靠门*/
	E_CloseDoor,
	/*闭门*/
	E_DivergeDoor,
	/*闭窗*/
	E_DivergeWin,
	/*靠有窗的墙*/
	E_CloseWallWithWin,
	/*靠有门的墙*/
	E_CloseWallWithDoor,
	/*靠有门有窗的墙*/
	E_CloseWallWithWinDoor,
	/*靠无门无窗的强*/
	E_CloseWallWithNothing,
	/*靠角落*/
	E_CloseCorner,
	/*间隔墙*/
	E_IntervalWall,
	E_IntervalWallWithWin,
	E_IntervalWallWithDoor,
	E_IntervalWallWithWinDoor,
	E_IntervalWallWithNothing,
	/*正对门的墙(Wall)*/
	E_WallParallelWithDoor,
	E_WallSectionOnLeftOfDoor,
	E_WallSectionOnRightOfDoor,
	E_WallOnOrientationOfDoor,
	E_WallOnReverseOfDoor,
	/*扇形区域*/
	E_SectorRegionWithDoor,
	E_SectorRegionOnOrientationOfDoor,
	E_SectorRegionOnReverseOfDoor,
	/*窗*/
	E_WallParallelWithWin,
	E_WallSectionOnLeftOfWin,
	E_WallSectionOnRightOfWin,
	E_WallOnLeftOfWin,
	E_WallOnRightOfWin,
	/*区域与窗户的关系*/
	E_SectorRegionWithWin,
	/*距离门的墙角*/
	E_CornerWithNearestDistToDoor,
	E_CornerWithFarestDistToDoor,
	E_ClosePositiveCorner,
	E_CloseNegativeCorner,
	E_CloseActualWall,
	 

	E_FOrientationWall,
	E_FLeftWall,
	E_FRightWall,
	E_FReverseWall,
	E_FOrientationWallSection,
	E_FLeftWallSection,
	E_FRightWallSection,
	E_FReverseWallSection,
	E_FOrientationWallRadiation,
	E_FOrientationWallSectionOnLeft,
	E_FOrientationWallSectionOnRight,
	E_FReverseWallSectionOnLeft,
	E_FReverseWallSectionOnRight,

	E_FOrientationLineRegion,
	E_FLeftLineRegion,
	E_FRightLineRegion,
	E_FReverseLineRegion,

	E_FOrientationSectorRegion,
	E_FLeftSectorRegion,
	E_FRightSectorRegion,
	E_FReverseSectorRegion,
	E_ToiletCircleRegion,
	//客厅布局
	E_CloseVirtualWall,
	E_CloseEntranceDoor,
	E_CentreRadiation,
	E_DivergeSunshineDoor,
	E_DivergeNormalDoor,
	E_CloseWallOnLeftOfLightEntrance,
	E_CloseWallOnRightOfLightEntrance,
	E_CloseWallWithLightEntrance,
	E_WeightStripBetweenMaxWallWithCentre,
	E_NUMBER
};
struct Token
{
	Token(ETokenType type):
		Type(type), Number(0.f)
	{
	}
	Token(ETokenType type,float number) :
		Type(type),Number(number)
	{
	}
	Token(ETokenType type, const std::string& name) :
		Type(type), Name(name)
	{
	}
	/*是不是等于*/
	inline bool IsEqul()
	{
		return Type == E_EQUL;
	}
	/*是不是规则*/
	inline bool IsEntity()const
	{
		return Type >= E_CloseWall && Type < E_NUMBER;
	}
	/*是不是数字*/
	inline bool IsNumber()const
	{
		return Type == E_NUMBER;
	}
	/*是不是id*/
	inline bool IsIdentity()const
	{
		return Type == E_IDENTTY;
	}
	/*是不是操作符*/
	inline bool IsOperator()const
	{
		return Type >= E_EQUL && Type <= E_Div;
	}
	/*比较*/
	static bool Compare(const Token*left, const Token*right);
	ETokenType  Type;
	float		Number;
	std::string	Name;
};
/*--------------------------------------------------
* TokenResever  所有有关公式解析的关键字
*	(对应TokenType)
----------------------------------------------------*/
class TokenResever
{
public:
	/*对应枚举类型*/
	static std::string equl;
	static std::string blank;
	static std::string pound;
    static std::string at;
	static std::string dollar;
	static std::string mul;
	static std::string minus;
	static std::string plus;
	static std::string div;
	static std::string semicolon;
	static std::string closeWall;
	static std::string closeWin;
	static std::string closeDoor;
	static std::string divergeDoor;
	static std::string divergeWin;
	static std::string closeWallWithWin;
	static std::string closeWallWithDoor;
	static std::string closeWallWithWinDoor;
	static std::string closeWallWithNothing;
	static std::string sWall;
	static std::string sWallWithWin;
	static std::string sWallWithDoor;
	static std::string sWallWithWinDoor;
	static std::string sWallWithNothing;
	static std::string closeCorner;

	static std::string wallParallelWithDoor;
	static std::string wallSectionOnLeftOfDoor;
	static std::string wallSectionOnRightOfDoor;
	static std::string wallOnOrientationOfDoor;
	static std::string wallOnReverseOfDoor;

	static std::string sectorRegionWithDoor;
	static std::string sectorRegionOnOrientationOfDoor;
	static std::string sectorRegionOnReverseOfDoor;
	static std::string sectorRegionWithWin;

	static std::string wallParallelWithWin;
	static std::string wallSectionOnLeftOfWin;
	static std::string wallSectionOnRightOfWin;
	static std::string wallsOnLeftOfWin;
	static std::string wallsOnRightOfWin;
 
	static std::string cornerWithNearestDistToDoor;
	static std::string cornerWithFarsetDistToDoor;

	static std::string closePositiveCorner;
	static std::string closeNegativeCorner;
	static std::string closeActualWall;


	static std::string  fOrientationWall;
	static std::string  fLeftWall;
	static std::string  fRightWall;
	static std::string  fReverseWall;

	static std::string  fOrientationWallSection;
	static std::string  fLeftWallSection;
	static std::string  fRightWallSection;
	static std::string  fReverseWallSection;
	static std::string  fOrientationWallRadiation;

	static std::string fOrientationWallSectionOnLeft;
	static std::string fOrientationWallSectionOnRight;
	static std::string fReverseWallSectionOnLeft;
	static std::string fReverseWallSectionOnRight;


	static std::string fOrientationLineRegion;
	static std::string fLeftLineRegion;
	static std::string fRightLineRegion;
	static std::string fReverseLineRegion;

	static std::string fOrientationSectorRegion;
	static std::string fLeftSectorRegion;
	static std::string fRightSectorRegion;
	static std::string fReverseSectorRegion;

	static std::string toiletCircleRegion;
	static std::string closeVirtualWall;
	static std::string closeEntranceDoor;
	static std::string closeRadiation;
	static std::string divergeSunshineDoor;
	static std::string divergeNormalDoor;

	static std::string closeWallOnLeftOfLightEntrance;
	static std::string closeWallOnRightOfLightEntrance;
	static std::string closeWallWithLightEntrance;
	static std::string weightStripBetweenMaxWallWithCentre;
 
};
