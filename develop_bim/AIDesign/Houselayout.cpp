#include "Houselayout.h"



Houselayout::Houselayout()
{
	version = 10000;
}


Houselayout::~Houselayout()
{
}

/*****************************************************************
*  函数名:BIM的户型json数据初始化户型
*  功能描述:
*
*  @param data json数据
*
*****************************************************************/
void Houselayout::LoadBimData(Json::Value data)
{
	if (data.isMember("layoutMode") && data["layoutMode"].isObject())
	{
		Json::Value homeMode = data["layoutMode"];

		// 初始化版本号
		if (homeMode.isMember("version") && homeMode["version"].isInt())
		{
			this->version = homeMode["version"].asInt();
		}

		// 初始化房间列表
		if (homeMode.isMember("roomList") && homeMode["roomList"].isArray())
		{
			Json::Value rooms = homeMode["roomList"];
			for (unsigned int i = 0; i < rooms.size(); i++)
			{
				if (rooms[i].isObject())
				{
					Json::Value room = rooms[i];
					Room tem_room = Room();
					tem_room.LoadBimData(room);
					if (tem_room.getIsOutroom() == true)
					{
						continue;
					}
					if (tem_room.point_list.size() < 3)
					{
						continue;
					}
					this->room_list.push_back(tem_room);
					//Room* temp_room = Room(room);
				}
			}
		}

		// 初始化门列表-singleDoorList
		if (homeMode.isMember("singleDoorList") && homeMode["singleDoorList"].isArray())
		{
			Json::Value signle_doors = homeMode["singleDoorList"];
			for (unsigned int i = 0; i < signle_doors.size(); i++)
			{
				if (signle_doors[i].isObject())
				{
					Json::Value signle_door = signle_doors[i];
					Door tem_door = Door();
					tem_door.LoadBimDoor(signle_door);
					tem_door.door_type = D_NORMAL;
					tem_door.opening_type = -1;
					this->door_list.push_back(tem_door);
					//Room* temp_room = Room(room);
				}
			}
		}

		// 初始化门列表-slidingDoorList
		if (homeMode.isMember("slidingDoorList") && homeMode["slidingDoorList"].isArray())
		{
			Json::Value sliding_doors = homeMode["slidingDoorList"];
			for (unsigned int i = 0; i < sliding_doors.size(); i++)
			{
				if (sliding_doors[i].isObject())
				{
					Json::Value sliding_door = sliding_doors[i];
					Door tem_door = Door();
					tem_door.LoadBimDoor(sliding_door);
					tem_door.door_type = D_SLIDING;
					tem_door.opening_type = -1;
					this->door_list.push_back(tem_door);
					//Room* temp_room = Room(room);
				}
			}
		}

		// 初始化门列表-securityDoorList
		if (homeMode.isMember("securityDoorList") && homeMode["securityDoorList"].isArray())
		{
			Json::Value security_doors = homeMode["securityDoorList"];
			for (unsigned int i = 0; i < security_doors.size(); i++)
			{
				if (security_doors[i].isObject())
				{
					Json::Value security_door = security_doors[i];
					Door tem_door = Door();
					tem_door.LoadBimDoor(security_door);
					tem_door.door_type = D_SECURITY;
					tem_door.opening_type = -1;
					this->door_list.push_back(tem_door);
					//Room* temp_room = Room(room);
				}
			}
		}

		// 初始化门列表-passList
		if (homeMode.isMember("passList") && homeMode["passList"].isArray())
		{
			Json::Value pass_list = homeMode["passList"];
			for (unsigned int i = 0; i < pass_list.size(); i++)
			{
				if (pass_list[i].isObject())
				{
					Json::Value pass = pass_list[i];
					Door tem_door = Door();
					tem_door.LoadBimDoor(pass);
					if (tem_door.door_type == D_PASS)
					{
						tem_door.opening_type = -1;
						this->door_list.push_back(tem_door);
					}
					//Room* temp_room = Room(room);
				}
			}
		}

		// 初始化窗户列表-windowList 标准窗户
		if (homeMode.isMember("windowList") && homeMode["windowList"].isArray())
		{
			Json::Value normal_windows = homeMode["windowList"];
			for (unsigned int i = 0; i < normal_windows.size(); i++)
			{
				if (normal_windows[i].isObject())
				{
					Json::Value value_window = normal_windows[i];
					Window tem_window = Window();
					tem_window.LoadBimWindow(value_window);
					tem_window.window_type = WIN_NORMAL;
					tem_window.opening_type = -1;
					this->window_list.push_back(tem_window);
					//Room* temp_room = Room(room);
				}
			}
		}

		// 初始化窗户列表-floorWindowList（落地窗）
		if (homeMode.isMember("floorWindowList") && homeMode["floorWindowList"].isArray())
		{
			Json::Value floor_windows = homeMode["floorWindowList"];
			for (unsigned int i = 0; i < floor_windows.size(); i++)
			{
				if (floor_windows[i].isObject())
				{
					Json::Value value_window = floor_windows[i];
					Window tem_window = Window();
					tem_window.LoadBimWindow(value_window);
					tem_window.window_type = WIN_FLOOR;
					tem_window.opening_type = -1;
					this->window_list.push_back(tem_window);
					//Room* temp_room = Room(room);
				}
			}
		}
		// 初始化窗户列表-bayFloorWindowList（标准飘窗）
		if (homeMode.isMember("bayFloorWindowList") && homeMode["bayFloorWindowList"].isArray())
		{
			Json::Value bay_floor_windows = homeMode["bayFloorWindowList"];
			for (unsigned int i = 0; i < bay_floor_windows.size(); i++)
			{
				if (bay_floor_windows[i].isObject())
				{
					Json::Value value_window = bay_floor_windows[i];
					Window tem_window = Window();
					tem_window.LoadBimWindow(value_window);
					tem_window.window_type = WIN_BAY_FLOOR;
					tem_window.opening_type = -1;
					this->window_list.push_back(tem_window);
					//Room* temp_room = Room(room);
				}
			}
		}

		// 初始化窗户列表-TrapeBayWindowList(梯形飘窗）
		if (homeMode.isMember("TrapeBayWindowList") && homeMode["TrapeBayWindowList"].isArray())
		{
			Json::Value trape_bay_windows = homeMode["TrapeBayWindowList"];
			for (unsigned int i = 0; i < trape_bay_windows.size(); i++)
			{
				if (trape_bay_windows[i].isObject())
				{
					Json::Value value_window = trape_bay_windows[i];
					Window tem_window = Window();
					tem_window.LoadBimWindow(value_window);
					tem_window.window_type = WIN_TRAPE_BAY;
					tem_window.opening_type = -1;
					this->window_list.push_back(tem_window);
					//Room* temp_room = Room(room);
				}
			}
		}

		// 临时初始化地标-floorBoxList（收到创建的数据）
		if (homeMode.isMember("floorBoxList") && homeMode["floorBoxList"].isArray())
		{
			Json::Value floor_box_list = homeMode["floorBoxList"];
			for (unsigned int i = 0; i < floor_box_list.size(); i++)
			{
				if (floor_box_list[i].isObject())
				{
					Json::Value floorbox = floor_box_list[i];
					FloorBox tem_floor = FloorBox();
					tem_floor.LoadBimDataTest(floorbox);
					tem_floor.floor_box_type = FB_STOOL;
					this->floor_box_list.push_back(tem_floor);
					
				}
			}
		}

		// 初始化区域数据
		if (homeMode.isMember("regionList") && homeMode["regionList"].isArray())
		{
			Json::Value regions = homeMode["regionList"];
			for (unsigned int i = 0; i < regions.size(); i++)
			{
				if (regions[i].isObject())
				{
					Json::Value regionObj = regions[i];
					RoomRegion tmpRoomRegion = RoomRegion();
					tmpRoomRegion.LoadBimData(regionObj);
					this->region_list.push_back(tmpRoomRegion);
				}
			}
		}

	}

	// 初始化相关数据
	InitCorner();
	InitOpening();

	// 初始化地标和墙标
	InitBox();

	// 初始化区域

}


// 解析BIM单房间数据
void Houselayout::LoadBimSingleData(Json::Value data)
{
	if (data.isMember("layoutMode") && data["layoutMode"].isObject())
	{
		Json::Value homeMode = data["layoutMode"];

		// 初始化版本号
		if (homeMode.isMember("version") && homeMode["version"].isInt())
		{
			this->version = homeMode["version"].asInt();
		}

		// 初始化窗户列表-windowList 标准窗户
		if (homeMode.isMember("windowList") && homeMode["windowList"].isArray())
		{
			Json::Value normal_windows = homeMode["windowList"];
			for (unsigned int i = 0; i < normal_windows.size(); i++)
			{
				if (normal_windows[i].isObject())
				{
					Json::Value value_window = normal_windows[i];
					Window tem_window = Window();
					tem_window.LoadBimWindow(value_window);
					tem_window.window_type = WIN_NORMAL;
					tem_window.opening_type = -1;
					this->window_list.push_back(tem_window);
					//Room* temp_room = Room(room);
				}
			}
		}

		// 初始化窗户列表-floorWindowList（落地窗）
		if (homeMode.isMember("floorWindowList") && homeMode["floorWindowList"].isArray())
		{
			Json::Value floor_windows = homeMode["floorWindowList"];
			for (unsigned int i = 0; i < floor_windows.size(); i++)
			{
				if (floor_windows[i].isObject())
				{
					Json::Value value_window = floor_windows[i];
					Window tem_window = Window();
					tem_window.LoadBimWindow(value_window);
					tem_window.window_type = WIN_FLOOR;
					tem_window.opening_type = -1;
					this->window_list.push_back(tem_window);
					//Room* temp_room = Room(room);
				}
			}
		}
		// 初始化窗户列表-bayFloorWindowList（标准飘窗）
		if (homeMode.isMember("bayFloorWindowList") && homeMode["bayFloorWindowList"].isArray())
		{
			Json::Value bay_floor_windows = homeMode["bayFloorWindowList"];
			for (unsigned int i = 0; i < bay_floor_windows.size(); i++)
			{
				if (bay_floor_windows[i].isObject())
				{
					Json::Value value_window = bay_floor_windows[i];
					Window tem_window = Window();
					tem_window.LoadBimWindow(value_window);
					tem_window.window_type = WIN_BAY_FLOOR;
					tem_window.opening_type = -1;
					this->window_list.push_back(tem_window);
					//Room* temp_room = Room(room);
				}
			}
		}

		// 初始化窗户列表-TrapeBayWindowList(梯形飘窗）
		if (homeMode.isMember("TrapeBayWindowList") && homeMode["TrapeBayWindowList"].isArray())
		{
			Json::Value trape_bay_windows = homeMode["TrapeBayWindowList"];
			for (unsigned int i = 0; i < trape_bay_windows.size(); i++)
			{
				if (trape_bay_windows[i].isObject())
				{
					Json::Value value_window = trape_bay_windows[i];
					Window tem_window = Window();
					tem_window.LoadBimWindow(value_window);
					tem_window.window_type = WIN_TRAPE_BAY;
					tem_window.opening_type = -1;
					this->window_list.push_back(tem_window);
					//Room* temp_room = Room(room);
				}
			}
		}

		// 初始化门列表-singleDoorList
		if (homeMode.isMember("singleDoorList") && homeMode["singleDoorList"].isArray())
		{
			Json::Value signle_doors = homeMode["singleDoorList"];
			for (unsigned int i = 0; i < signle_doors.size(); i++)
			{
				if (signle_doors[i].isObject())
				{
					Json::Value signle_door = signle_doors[i];
					Door tem_door = Door();
					tem_door.LoadBimDoor(signle_door);
					tem_door.door_type = D_NORMAL;
					tem_door.opening_type = -1;
					this->door_list.push_back(tem_door);
					//Room* temp_room = Room(room);
				}
			}
		}
		// 初始化门列表-slidingDoorList
		if (homeMode.isMember("slidingDoorList") && homeMode["slidingDoorList"].isArray())
		{
			Json::Value sliding_doors = homeMode["slidingDoorList"];
			for (unsigned int i = 0; i < sliding_doors.size(); i++)
			{
				if (sliding_doors[i].isObject())
				{
					Json::Value sliding_door = sliding_doors[i];
					Door tem_door = Door();
					tem_door.LoadBimDoor(sliding_door);
					tem_door.door_type = D_SLIDING;
					tem_door.opening_type = -1;
					this->door_list.push_back(tem_door);
					//Room* temp_room = Room(room);
				}
			}
		}

		// 初始化门列表-securityDoorList
		if (homeMode.isMember("securityDoorList") && homeMode["securityDoorList"].isArray())
		{
			Json::Value security_doors = homeMode["securityDoorList"];
			for (unsigned int i = 0; i < security_doors.size(); i++)
			{
				if (security_doors[i].isObject())
				{
					Json::Value security_door = security_doors[i];
					Door tem_door = Door();
					tem_door.LoadBimDoor(security_door);
					tem_door.door_type = D_SECURITY;
					tem_door.opening_type = -1;
					this->door_list.push_back(tem_door);
					//Room* temp_room = Room(room);
					// 入户门
					this->entrance_door = tem_door;
				}
			}
		}

		// 初始化门列表-passList
		if (homeMode.isMember("passList") && homeMode["passList"].isArray())
		{
			Json::Value pass_list = homeMode["passList"];
			for (unsigned int i = 0; i < pass_list.size(); i++)
			{
				if (pass_list[i].isObject())
				{
					Json::Value pass = pass_list[i];
					Door tem_door = Door();
					tem_door.LoadBimDoor(pass);
					if (tem_door.door_type == D_PASS)
					{
						tem_door.opening_type = -1;
						this->door_list.push_back(tem_door);
					}
					//Room* temp_room = Room(room);
				}
			}
		}

		/** 初始化门洞 NewPass_list @jirenze */
		if (homeMode.isMember("newPassList") && homeMode["newPassList"].isArray())
		{
			Json::Value new_pass_list = homeMode["newPassList"];
			for (unsigned int i = 0; i < new_pass_list.size(); i++)
			{
				if (new_pass_list[i].isObject())
				{
					Json::Value new_pass = new_pass_list[i];
					Door temp_door = Door();
					temp_door.LoadBimDoor(new_pass);
					if (temp_door.door_type == D_PASS)
					{
						temp_door.opening_type = -1;
						this->door_list.push_back(temp_door);
					}
				}
			}
		}

		// 初始化房间列表
		if (homeMode.isMember("roomList") && homeMode["roomList"].isArray())
		{
			Json::Value rooms = homeMode["roomList"];
			for (unsigned int i = 0; i < rooms.size(); i++)
			{
				if (rooms[i].isObject())
				{
					Json::Value room = rooms[i];
					Room tem_room = Room();
					tem_room.LoadBimData(room);
					if (tem_room.getIsOutroom() == true)
					{
						continue;
					}
					if (tem_room.point_list.size() < 3)
					{
						continue;
					}
					this->room_list.push_back(tem_room);
					//Room* temp_room = Room(room);
				}
			}
		}

		/** component_list @jirenze */
		/*if (homeMode.isMember("componentList") && homeMode["componentList"].isArray())
		{
			Json::Value new_component_list = homeMode["componentList"];
			for (int i = 0; i < new_component_list.size(); i++)
			{
				if (new_component_list[i].isObject())
				{
					Json::Value new_component = new_component_list[i];
					ComponentObject temp_component = ComponentObject();
					temp_component.LoadBimComponentObject(new_component);
					this->componentj_list.push_back(temp_component);
				}
			}
		}*/


		/** 柱子 pillar_list */
		if (homeMode.isMember("pillarList") && homeMode["pillarList"].isArray())
		{
			Json::Value new_pillar_list = homeMode["pillarList"];
			for (unsigned int i = 0; i < new_pillar_list.size(); i++)
			{
				if (new_pillar_list[i].isObject())
				{
					Json::Value new_pillar = new_pillar_list[i];
					AirPillarBaseJ temp_pillar = AirPillarBaseJ();
					temp_pillar.LoadBimAirPillar(new_pillar);
					FloorBox temp_floor_box = temp_pillar.GetNewFloorBox(temp_pillar);
					this->floor_box_list.push_back(temp_floor_box);
				}
			}
		}

		/** 烟道 airflue_list */
		if (homeMode.isMember("airFlueList") && homeMode["airFlueList"].isArray())
		{
			Json::Value new_airflue_list = homeMode["airFlueList"];
			for (unsigned int i = 0; i < new_airflue_list.size(); i++)
			{
				if (new_airflue_list[i].isObject())
				{
					Json::Value new_airflue = new_airflue_list[i];
					AirPillarBaseJ temp_airflue = AirPillarBaseJ();
					temp_airflue.LoadBimAirPillar(new_airflue);
					FloorBox temp_floor_box = temp_airflue.GetNewFloorBox(temp_airflue);
					this->floor_box_list.push_back(temp_floor_box);
				}
			}
		}

		/** 包立管 */
		if (homeMode.isMember("packPipeList") && homeMode["packPipeList"].isArray())
		{
			Json::Value new_packpipe_list = homeMode["packPipeList"];
			for (unsigned int i = 0; i < new_packpipe_list.size(); i++)
			{
				if (new_packpipe_list[i].isObject())
				{
					Json::Value new_packpipe = new_packpipe_list[i];
					PackPipeJ temp_packpipe = PackPipeJ();
					temp_packpipe.LoadBimPackPipe(new_packpipe);
					FloorBox temp_floor_box = temp_packpipe.GetNewFloorBox(temp_packpipe);
					this->floor_box_list.push_back(temp_floor_box);
				}
			}
		}

		// 初始化区域数据
		if (homeMode.isMember("regionList") && homeMode["regionList"].isArray())
		{
			Json::Value regions = homeMode["regionList"];
			for (unsigned int i = 0; i < regions.size(); i++)
			{
				if (regions[i].isObject())
				{
					Json::Value regionObj = regions[i];
					RoomRegion tmpRoomRegion = RoomRegion();
					tmpRoomRegion.LoadBimData(regionObj);
					this->region_list.push_back(tmpRoomRegion);
				}
			}
		}

		// 初始化入光口
		string sunny_no = "";
		if (homeMode.isMember("sunnyOpeningList") && homeMode["sunnyOpeningList"].isArray())
		{
			Json::Value sunnyOpenings = homeMode["sunnyOpeningList"];
			for (unsigned int i = 0; i < sunnyOpenings.size(); i++)
			{
				if (sunnyOpenings[i].isObject())
				{
					Json::Value sunnyObj = sunnyOpenings[i];
					Window tem_window = Window();
					tem_window.LoadBimWindow(sunnyObj);
					sunny_no = tem_window.GetNo();
					break;
				}
			}

			for (auto &tmp : window_list)
			{
				if (tmp.GetNo() == sunny_no)
				{
					tmp.is_light = true;
				}
			}

			for (auto &tmp : door_list)
			{
				if (tmp.GetNo() == sunny_no)
				{
					tmp.is_light = true;
				}
			}

		}
	}

	// 初始化相关数据
	InitCorner();
	InitOpening();

	// 初始化地标和墙标
	InitBox();
}
/*****************************************************************
*  函数名:解析APP整个户型数据
*  功能描述:
*
*  @param data json数据
*
*****************************************************************/
void Houselayout::LoadAppData(Json::Value data)
{
	if (data.isMember("homeMode") && data["homeMode"].isObject())
	{
		Json::Value homeMode = data["homeMode"];

		// 初始化版本号
		if (homeMode.isMember("version") && homeMode["version"].isInt())
		{
			this->version = homeMode["version"].asInt();
		}

		// 初始化房间列表
		if (homeMode.isMember("roomList") && homeMode["roomList"].isArray())
		{
			Json::Value rooms = homeMode["roomList"];
			for (unsigned int i = 0; i < rooms.size(); i++)
			{
				if (rooms[i].isObject())
				{
					Json::Value room = rooms[i];
					Room tem_room = Room();
					tem_room.LoadBimData(room);
					if (tem_room.getIsOutroom() == true)
					{
						continue;
					}
					if (tem_room.point_list.size() < 3)
					{
						continue;
					}
					this->room_list.push_back(tem_room);
				}
			}
		}

		// 初始化门列表-doors
		if (homeMode.isMember("doors") && homeMode["doors"].isArray())
		{
			Json::Value doors = homeMode["doors"];
			for (int i = 0; i < int(doors.size()); i++)
			{
				if (doors[i].isObject())
				{
					Json::Value door = doors[i];
					Door tem_door = Door();
					tem_door.LoadBimDoor(door);
					this->door_list.push_back(tem_door);
				}
			}
		}

		// 初始化窗户列表-windowList
		if (homeMode.isMember("windows") && homeMode["windows"].isArray())
		{
			Json::Value windows = homeMode["windows"];
			for (int i = 0; i < int(windows.size()); i++)
			{
				if (windows[i].isObject())
				{
					Json::Value value_window = windows[i];
					Window tem_window = Window();
					tem_window.LoadBimWindow(value_window);
					tem_window.window_type = WIN_NORMAL;
					tem_window.opening_type = -1;
					this->window_list.push_back(tem_window);
				}
			}
		}
	}

	// 初始化相关数据
	InitCorner();
	InitOpening();
	// 初始化地标和墙标
	InitBox();

}

/*****************************************************************
*  函数名:解析APP单房间数据
*  功能描述:
*
*  @param data json数据
*
*****************************************************************/
void Houselayout::LoadAppSingleData(Json::Value data)
{
	if (data.isMember("homeMode") && data["homeMode"].isObject())
	{
		Json::Value homeMode = data["homeMode"];

		// 初始化版本号
		if (homeMode.isMember("version") && homeMode["version"].isInt())
		{
			this->version = homeMode["version"].asInt();
		}

		// 初始化房间
		if (homeMode.isMember("room") && homeMode["room"].isObject())
		{
			Json::Value room = homeMode["room"];
			Room tem_room = Room();
			tem_room.LoadBimData(room);
			this->room_list.push_back(tem_room);
		}

		// 初始化门列表-doors
		if (homeMode.isMember("doors") && homeMode["doors"].isArray())
		{
			Json::Value doors = homeMode["doors"];
			for (int i = 0; i < int(doors.size()); i++)
			{
				if (doors[i].isObject())
				{
					Json::Value door = doors[i];
					Door tem_door = Door();
					tem_door.LoadBimDoor(door);
					this->door_list.push_back(tem_door);
				}
			}
		}

		// 初始化入户门entrance_door
		if (homeMode.isMember("entrance_door") && homeMode["entrance_door"].isObject())
		{
			Json::Value door = homeMode["entrance_door"];
			this->entrance_door.LoadBimDoor(door);
		}

		// 初始化窗户列表-windowList
		if (homeMode.isMember("windows") && homeMode["windows"].isArray())
		{
			Json::Value windows = homeMode["windows"];
			for (int i = 0; i < int(windows.size()); i++)
			{
				if (windows[i].isObject())
				{
					Json::Value value_window = windows[i];
					Window tem_window = Window();
					tem_window.LoadBimWindow(value_window);
					//tem_window.window_type = WIN_NORMAL;
					tem_window.opening_type = -1;
					this->window_list.push_back(tem_window);
				}
			}
		}

		// 临时初始化地标-floorBoxList（收到创建的数据）
		if (homeMode.isMember("floorBoxList") && homeMode["floorBoxList"].isArray())
		{
			Json::Value floor_box_list = homeMode["floorBoxList"];
			for (int i = 0; i < int(floor_box_list.size()); i++)
			{
				if (floor_box_list[i].isObject())
				{
					Json::Value floorbox = floor_box_list[i];
					FloorBox tem_floor = FloorBox();
					tem_floor.LoadBimDataTest(floorbox);
					tem_floor.floor_box_type = FB_STOOL;
					this->floor_box_list.push_back(tem_floor);

				}
			}
		}

		// 初始化区域
		if (homeMode.isMember("region") && homeMode["region"].isObject())
		{
			Json::Value region = homeMode["region"];
			RoomRegion tmpRegion = RoomRegion();
			tmpRegion.LoadBimData(region);
			this->region_list.push_back(tmpRegion);
		}

	}

	// 针对卧室中的推拉门，把推拉门变换为直窗进行布局
	// 房间变换
	if (room_list.size() > 0)
	{
		int change_room_id = ComUtil::ChangeRoomId(room_list[0].getSpaceId());
		if (change_room_id == ZHUWO)
		{
			int sliding_count = 0;
			Door *p_door = new Door();
			int door_index = -1;
			for (int i = 0; i < door_list.size(); i++)
			{
				if (door_list[i].door_type == D_SLIDING)
				{
					sliding_count++;
					*p_door = door_list[i];
					door_index = i;
				}
			}

			// 如果房间只有一个推拉门
			if (sliding_count == 1)
			{
				Window tmp_window = Window(p_door);
				window_list.push_back(tmp_window);
				// 移除推拉门
				vector<Door>::iterator   iter = door_list.begin() + door_index;
				door_list.erase(iter);
			}

			delete p_door;
		}
	}
	// 初始化相关数据
	InitCorner();
	InitOpening();
	// 初始化地标和墙标
	InitBox();
}
// 获得房间编号获得房间对象
Room* Houselayout::GetRoomByRoomNo(string room_no)
{
	for (int i = 0; i < room_list.size();i++)
	{
		Room* p_room = &room_list[i];
		if (p_room->GetNo() == room_no)
		{
			return p_room;
		}
	}

	return NULL;
}

// 获得房间的墙列表
vector<Wall> Houselayout::GetWallsByRoomNo(string room_no)
{
	vector<Wall> temp_list;
	for (vector<Wall>::iterator wall = wall_list.begin(); wall != wall_list.end(); wall++)
	{
		if (room_no == wall->room_no)
		{
			temp_list.push_back(*wall);
		}
	}

	return temp_list;
}

// 获得房间的门列表
vector<Door> Houselayout::GetDoorsByRoomNo(string room_no)
{
	vector<Door> temp_list;
	vector<Wall> temp_wall_list = GetWallsByRoomNo(room_no);
	if (temp_wall_list.size() < 1)
	{
		return temp_list;
	}

	for (vector<Wall>::iterator wall = temp_wall_list.begin(); wall != temp_wall_list.end(); wall++)
	{
		for (int i = 0; i < wall->door_list.size(); i++)
		{
			temp_list.push_back(wall->door_list[i]);
		}
	}

	return temp_list;
}

// 获得房间的窗列表
vector<Window> Houselayout::GetWindowsByRoomNo(string room_no)
{
	vector<Window> temp_list;
	vector<Wall> temp_wall_list = GetWallsByRoomNo(room_no);
	if (temp_wall_list.size() < 1)
	{
		return temp_list;
	}

	for (vector<Wall>::iterator wall = temp_wall_list.begin(); wall != temp_wall_list.end(); wall++)
	{
		for (int i = 0; i < wall->window_list.size(); i++)
		{
			temp_list.push_back(wall->window_list[i]);
		}
	}

	return temp_list;
}

// 获得房间的点列表
vector<Corner> Houselayout::GetCornersByRoomNo(string room_no)
{
	vector<Corner> temp_list;

	for (vector<Corner>::iterator corner = corner_list.begin(); corner != corner_list.end(); corner++)
	{
		if (corner->room_no == room_no)
		{
			temp_list.push_back(*corner);
		}
	}

	return temp_list;
}

// 获得房间的地标
vector<FloorBox> Houselayout::GetFloorBoxesByRoomNo(string room_no)
{
	vector<FloorBox> temp_list;

	for (vector<FloorBox>::iterator floor_box = floor_box_list.begin(); floor_box != floor_box_list.end(); floor_box++)
	{
		if (floor_box->room_no == room_no)
		{
			temp_list.push_back(*floor_box);
		}
	}

	return temp_list;
}

// 获得房间的区域
vector<RoomRegion> Houselayout::GetRoomRegionsByRoomNo(string room_no)
{
	vector<RoomRegion> temp_list;
	for (vector<RoomRegion>::iterator region = region_list.begin(); region != region_list.end(); region++)
	{
		if (region->getRegionRoomNo() == room_no)
		{
			temp_list.push_back(*region);
		}
	}

	return temp_list;
}

// 获得门编号获得门对象
Door* Houselayout::GetDoorByDoorNo(string door_no)
{
	for (int i = 0; i < door_list.size(); i++)
	{
		Door* p_door = &door_list[i];
		if (p_door->GetNo() == door_no)
		{
			return p_door;
		}
	}

	return NULL;
}

// 根据空间id获得空间列表
vector<Room*> Houselayout::GetRoomsBySpaceId(int space_id)
{
	vector<Room*> rooms;
	for (int i = 0; i < room_list.size(); i++)
	{
		if (room_list[i].getSpaceId() == space_id)
		{
			rooms.push_back(&room_list[i]);
		}
	}

	return rooms;
}

// 得到入户门的墙体
Wall * Houselayout::GetWallByEntranceDoor()
{
	for (int i = 0; i < wall_list.size(); i++)
	{
		Wall tmp_wall = wall_list[i];
		for (auto &door : tmp_wall.door_list)
		{
			if (door.door_type == D_SECURITY)
			{
				return &wall_list[i];
			}
		}
	}

	return NULL;
}

// 得到入户门
Door *Houselayout::GetEntranceDoor()
{
	for (int i = 0; i < door_list.size(); i++)
	{
		Door tmp_door = door_list[i];
		if (tmp_door.door_type == D_SECURITY)
		{
			return &door_list[i];
		}
	}

	return NULL;
}

// 计算墙体的朝向向量
Point3f Houselayout::CalculateWallNormal(Wall*wall)
{
	auto wall_direction = wall->start_corner.point - wall->end_corner.point;

	// 旋转90度
	auto rotate = 0.25f*6.283185307f;//2*pi/4
	auto x = cosf(rotate)*wall_direction.x - sinf(rotate)*wall_direction.y;
	wall_direction.y = sinf(rotate)*wall_direction.x + cosf(rotate)*wall_direction.y;
	wall_direction.x = x;

	wall_direction = ComMath::Normalize(wall_direction);
	auto extend = (wall->start_corner.point + wall->end_corner.point)*0.5f + wall_direction * 5.f;

	// 根据墙体找到房间
	Room* p_room = GetRoomByRoomNo(wall->room_no);
	if (p_room == NULL)
	{
		return wall_direction;
	}

	if (ComMath::PointInPolygon(extend, p_room->getPointList()))
		return wall_direction;
	return -wall_direction;
}

/////////////////////////////////私有方法//////////////////////
// 初始化点和墙体相关的数据
void Houselayout::InitCorner()
{
	for (vector<Room>::iterator room = room_list.begin(); room != room_list.end(); room++)
	{
		vector<Point3f> point_list = room->getPointList();
		vector<Corner> tmp_corner_list;

		// 生成corner点
		for (vector<Point3f>::iterator point = point_list.begin(); point != point_list.end(); point++)
		{
			Corner temp_corner;
			temp_corner.point = *point;
			//temp_corner->setByPoint(*point);
			temp_corner.SetNo(ComUtil::getGuuidNo());
			temp_corner.room_no = room->GetNo();
			this->corner_list.push_back(temp_corner);
			tmp_corner_list.push_back(temp_corner);
		}

		// 生成墙体
		for (int i = 0; i < tmp_corner_list.size(); i++)
		{
			int j = (i + 1) % tmp_corner_list.size();
			Wall tmp_wall = Wall(&tmp_corner_list[i], &tmp_corner_list[j]);
			tmp_wall.SetNo(ComUtil::getGuuidNo());
			tmp_wall.room_no = room->GetNo();
			this->wall_list.push_back(tmp_wall);
		}

	}
}

// 初始化洞相关的数据
void Houselayout::InitOpening()
{
	// 初始化门
	for (int i = 0; i < door_list.size(); i++)
	{
		InitDoorOnWall(&door_list[i]);
	}

	// 初始化窗户
	for (int j = 0; j < window_list.size(); j++)
	{
		InitWindowOnWall(&window_list[j]);
	}

}

// 把门放到墙上面
void Houselayout::InitDoorOnWall(Door* door)
{
	Point3f point = door->pos;
	for (vector<Wall>::iterator wall = wall_list.begin(); wall != wall_list.end(); wall++)
	{
		Point3f start_point = wall->start_corner.point;
		Point3f end_point = wall->end_corner.point;
		float d_distance = ComMath::getPointToLineDis(point, start_point, end_point);
		float door_dis = door->width / 2.0f;
		if (d_distance > (door_dis - OPENING_LIMIT) && d_distance < (door_dis + OPENING_LIMIT))
		{
			vector<Point3f> door_point;
			if (ComMath::getPointOnLine(point, start_point, end_point, door->length / 2.0f, door_point))
			{
				if (door_point.size() == 2)
				{
					Door temp_door = Door(*door);
					temp_door.start_point = door_point[0];
					temp_door.end_point = door_point[1];
					wall->door_list.push_back(temp_door);
				}
			}
		}
	}
}


// 把窗户放到墙上面
void Houselayout::InitWindowOnWall(Window *window)
{
	Point3f point = window->pos;
	for (vector<Wall>::iterator wall = wall_list.begin(); wall != wall_list.end(); wall++)
	{
		Point3f start_point = wall->start_corner.point;
		Point3f end_point = wall->end_corner.point;
		float d_distance = ComMath::getPointToLineDis(point, start_point, end_point);
		float window_dis = window->width / 2.0f;
		if (d_distance > (window_dis - OPENING_LIMIT) && d_distance < (window_dis + OPENING_LIMIT))
		{
			vector<Point3f> window_point;
			if (ComMath::getPointOnLine(point, start_point, end_point, window->length / 2.0f, window_point))
			{
				if (window_point.size() == 2)
				{
					Window temp_window = Window(*window);
					temp_window.start_point = window_point[0];
					temp_window.end_point = window_point[1];
					wall->window_list.push_back(temp_window);
				}
			}
		}
	}
}

// 初始化Box,把Box与墙体产生关系
void Houselayout::InitBox()
{
	// 初始化地标
	for (int i = 0; i < floor_box_list.size(); i++)
	{
		InitFloorBox(&floor_box_list[i]);
	}

	// 初始化墙标
	for (int j = 0; j < wall_box_list.size(); j++)
	{
		//暂不处理
		;
	}
}

// 地标与房间关系
void Houselayout::InitFloorBox(FloorBox* box)
{
	for (vector<Room>::iterator room = room_list.begin(); room != room_list.end(); room++)
	{
		if (ComMath::PointInPolygon(box->center, room->point_list))
		{
			box->room_no = room->GetNo();
			return;
		}
	}
}
