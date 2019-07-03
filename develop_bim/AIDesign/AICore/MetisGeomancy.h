/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  �ļ�����:��ˮ����
*  ��Ҫ����:
*
*  ��ǰ�汾:V1.0.0
*  ����:jirenze��niuzongyao
*  ��������:2018-11-07
*  ˵��:
**************************************************/
#pragma once
#include "../AICase.h"
#include "../Point3f.h"
#include "../Door.h"


/** �ŵ�����*/
enum RoomDoorType
{
	NOMEN=0,  // û������
	RUHUMEN = 1,  // �뻧��
	WOSHIMEN = 2,    // ������
	CHUFANGMEN = 3, // ������
	WEISHENGJIANMEN = 4     // ��������
};

struct GeomancyData
{
	int id;  // ���
	int count; // ����
	int score; // ��λ����
};
struct DoorData
{
	string room_no;
	int room_door_type;
	Door door;
	string link_room_no;
};

class MetisGeomancy
{
public:

	MetisGeomancy() {};
	MetisGeomancy(const shared_ptr<AICase> &ai_case);
	~MetisGeomancy() {};
	Json::Value GetGeomancyJson();

private:
	// Ѱ����Ч����
	void PickEffectiveDoors();
	//�ж������ŵ��Ƿ��ཻ
	bool IsDoorIntersect(DoorData src_door, DoorData des_door);
	// ͨ���������ͺ������ͻ�÷���������
	int GetRoomDoorType(int space_id, int door_type);
	// ��ʼ�����������
	void InitDoors();
	// �뻧�Ŷ���������10
	GeomancyData EntranceToToilet();
	// �뻧�ŶԳ�����11
	GeomancyData EntranceToKitchen();
	// �����Ŷ���������12
	GeomancyData BedroomToToilet();
	// �����ŶԳ�����13
	GeomancyData BedroomToKitchen();
	// �����Ŷ��뻧��14
	GeomancyData BedroomToEntrance();
	// �������ŶԳ�����15
	GeomancyData ToiletToKitchen();
	// ���ڼ��16
	GeomancyData CheckSharpAngle();
	// ����ɳ������������ֱ����Ӧ���������ţ�������ֱ������֮�� 17
	GeomancyData CheckSoftToDoor();
	// ��������ֻ�Դ��ţ��������ױ�ɷ����ɢ���Ѳ������ߣ���2�֣�18
	GeomancyData CheckTableToDoor();
	// ���Ҵ�ͷ�����ŷ��ţ������������أ����������˽�������3�֣�19
	GeomancyData CheckBedHeadToDoor();
	// ���Ӳ���ֱ������ͷλ���������׷�ɷ����3�֣�20
	GeomancyData CheckBedHeadToMirror();
	// ���������泯�����������������գ���1�֣�21
	GeomancyData CheckDeskToWindow();
	// �����������Դ��ţ����������ܸ��ţ���1�֣�22
	GeomancyData CheckDeskToDoor();
	// ��������Ӧ��ǽ������û�п�ɽ����1��23
	GeomancyData CheckDeskToWall();
	// ��ˮ�۷ֽṹ��json��
	Json::Value GeomancyDataToJson(GeomancyData data);
	//��÷����б�
	Json::Value GetGeomancyScoreList();
private:
	// ��Ч����
	vector<DoorData> effective_doors;
	// ������
	vector<DoorData> bedroom_doors;
	// �뻧��
	DoorData entrance_door;
	// ������
	vector<DoorData> kitchen_doors;
	// ��������
	vector<DoorData> toilet_doors;
	// �������
	const shared_ptr<AICase> ai_case;
};
