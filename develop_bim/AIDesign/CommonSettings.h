/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:CommonSettings.h
*  简要描述:全局配置
*
*  当前版本:
*  作者:常远
*  创建日期:2018-08-29
*  说明:
**************************************************/

#pragma once

#define  FLOAT_LIMIT 0.0001f
#define  OPENING_LIMIT 1.0f


//#define  BASE_PATH  "D://test//sample_data/"
//#define  DB_PATH  "D://test//aidesign.db"
//#define  SAMPLE_DATA_PATH  "D://test//sample_data/"
//#define  LOG_INIT_PATH  "D://test//my_log.ini"
//#define  DUMP_PATH  "D://test//dump"


#define DEFAULT_NO "00000"
#define M_PI  3.1415926f

/** 区域类型 */
enum RegionType
{
	R_FLOOR = 0,  // 地面
	R_WALL = 1,    // 墙面
	R_CEILING = 2,   // 顶面
};

/** 墙标类型 */
enum WallBoxType
{
	WB_STRONG_ELECTRI = 0,  // 强电箱
	WB_WEAK_ELECTRI = 1,    // 弱电箱
	WB_AIR_CONDITIONER = 2,     // 空调口
	WB_RADIATOR = 3,
};

/** 地标类型 */
enum FloorBoxType
{
	FB_PILLAR = 0,          // 柱子
	FB_STOOL = 1,		    // 马桶点位
	FB_FLOOR_DRAIN = 2,     // 地漏
	FB_UP_WATER_GAP = 3,    // 上水口
	FB_DOWN_WATER_GAP = 4,  // 下水口
	FB_PIPING = 5,			// 管道
	FB_CHIMNEY_PIPING=6,
	FB_PACKPIPE = 7,        // 包立管
};

/** BIM_Component类型 */
enum ComponentJType
{
	CJ_EleBoxL_Point = 15,    // 弱电箱
	CJ_EleBoxH_Point = 16,    // 强电箱
	CJ_Closestool = 31,       // 马桶下水
	CJ_Basin = 60,            // 地漏下水
	CJ_Drain_Point = 61,      // 下水主管道
	CJ_Water_Supply = 62,     // 入户水点位 
	CJ_Hot_Water = 55,        // 热水点位
	CJ_Cold_Water = 17,       // 冷水点位
	CJ_Middle_Water = 1202,   // 中水点位 Warning：BIM数据枚举无赋值
	CJ_Gas_Meter = 1201,      // 燃气表
	CJ_Gas_Supervisor = 1200, // 燃气主管 
	CJ_Water_Collector = 1203,// 分集水器 Warning：BIM数据枚举无赋值

	CJ_Default_NO = 11021013  // 没有添加
};

enum AirPillarType
{
	Pi_Pillar,    // 柱子
	Air_AirFlue,  //烟道
	NO_Type,      //无类型
};

/** 窗户类型 */
enum WindonType
{
	WIN_NORMAL = 0,  // 平开窗
	WIN_FLOOR = 1,   // 落地窗
	WIN_BAY_FLOOR = 2,// 普通飘窗
	WIN_TRAPE_BAY =3 // 梯形飘窗
};

/** 门类型 */
enum DoorType
{
	D_NORMAL =0,  // 标准门
	D_SLIDING = 1,   // 推拉门
	D_PASS = 2,     // 垭口-新垭口
	D_SECURITY =3     // 防盗门
};

/** 门类型 */
enum DoorLayoutType
{
	D_NORMAL_LAYOUT = 0,  // 正常布局
	D_SUN_LAYOUT = 1,   // 阳光门
};

/** 墙类型 */
enum WallType
{
	WALL_NORMAL = 0,
	WALL_LOW = 1, //矮墙
	WALL_ARC = 2 // 户型墙
};

/** 区域类型 */
enum SpaceId
{
	NOT_NAME = 1,  // 未命名
	ZHUWO = 2,
	CIWO = 3,
	ERTONGFANG = 4,
	SHUFANG = 5,
	KETING = 6,
	CHUWUJIAN = 7,
	WEISHENGJIAN = 8,
	YANGTAI = 9,
	CHUFANG = 10,
	CANTING = 11,
	XUANGUAN = 12,
	XIYIJIAN = 13,
	ZOULANG = 14,
	GUODAO = 15,
	KEWO = 16,
	ZHUWEISHENGJIAN = 17,
	KEWEISHENGJIAN = 18,
	WOSHIYANGTAI = 19,
	HOUYANGTAI = 20,
	CHAIGAIJIAN = 21,
	SHUIDIAN = 22,
	GEXING = 23,
	KECANTING = 24
};

/** 动线类型 */
// a.动线交叉1点，扣8分
// b.入户门进卧室需要穿过沙发组和视听组所构成的区域，扣10分
// c.卧室进卫生间需要穿过沙发组、视听组和餐桌椅所构成的区域，扣10分
enum MoveLineType
{
	CORSS_A = 10,
	CORSS_B = 11, 
	CORSS_C = 12 
};

/**---------------------------------------
*    构建组别  
*	 2019-02-20更新为最新的构件组别 
*    @纪仁泽
*-----------------------------------*/
enum ModelGroup
{
	GROUP_BED = 1002,             //双人床 
	GROUP_RIGHT_BEDSIDE = 1004,   //右床头柜       
	GROUP_LEFT_BEDSIDE = 1003,    //左床头柜
	GROUP_BED_STOOL = 1007,       //床尾凳
	GROUP_WARDROBE = 1006,        //定制衣柜
	GROUP_DESK = 1007,            //书桌
	GROUP_BOOKCASE = 1012,        //定制书柜
	GROUP_DRESSING_TABLE = 1008,  //梳妆台
	//GROUP_RIGHT_CHAIR = 109,      //右休闲椅
	//GROUP_LEFT_CHAIR = 137,	      //左休闲椅
	GROUP_SIDE = 1020,            //边几   
	GROUP_MAIN_SOFA = 1016,       //主位沙发
	GROUP_ASSIST_SOFA_L = 1017,   //辅位沙发-左
	GROUP_ASSIST_SOFA_R = 1018,   //辅位沙发-右
	GROUP_TEA_TABLE = 1019,       //茶几
	GROUP_DINING_TABLE = 1028,    //餐桌
	//GROUP_DINING_CHAIR = 117,     //餐椅
	GROUP_CABINET_MEAL = 1029,    //餐边柜
	GROUP_CABINET_TV = 1010,      //视听柜
	GROUP_CABINET_BALCONY = 1014, //定制阳台柜
	GROUP_CABINET_ENTRY = 1027,    //定制入户柜
	GROUP_CABINET_HALL = 1037,	 //间厅柜
	GROUP_ENTRANCE_DESK = 1038,   //玄关案台   

	GROUP_REFIGERATOR = 1031,     //冰箱
	GROUP_COMMODE = 1025,          //马桶
	GROUP_CABINET_BATH = 1024,    //浴室柜
	GROUP_SPRINKLER= 1021,          //花洒
	GROUP_BATH_ROOM = 1026,       // 淋浴房
	GROUP_BATHTUB = 1023,         //浴缸
	GROUP_WASHER= 1030,           //洗衣机
	GROUP_CABINET_LAND = 1034,    //厨房地柜
	GROUP_CABINET_HANG = 1035,    //厨房吊柜
	GROUP_CABINET_HIGH = 1036,    //厨房高柜
	GROUP_SINK = 1033,            //水槽
	GROUP_STOVE= 1032,            //炉灶

	//inner id 
	VIRTUAL_VISITOR_ROOM = 90001, // 客厅模拟包


};
