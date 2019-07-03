#pragma once
#include "TokenStream.h"
#include"MetisFormulaContainer.h"
#include "Render\RenderProxy.h"
#include "Render\CoreType.h"
#include "Render\D3D11HRI.h"
#include "Render\WinLaunch.h"
#include "MetisRegionResolver.h"
#include "AICase.h"
#include "CaseMatch/MetisRegionFeature.h"
#include "CaseMatch/MetisRegionMatcher.h"
#include <fstream>
#define FTest(ClassName) \
class Unit##ClassName\
{\
public:\
	static void Test();\
};\
void Unit##ClassName::Test()

FTest(TokenStream)
{
	TokenStream Stream;
	std::string formular =
		"= "
		" "
		"- "
		"* "
		"+ "
		"/ "
		"; "
		"BWall "
		"BWin "
		"BDoor "
		"ODoor "
		"OWin "
		"BWallWithWin "
		"BWallWithDoor "
		"BWallWithWinDoor "
		"BWallWithNothing "
		"WallCorner "
		"BBedSizeEdge "
		"HeadOfBed "
		"HeadOfBedLeft "
		"HeadOfBedRight "
		"SWallWithWin "
		"SWallWithDoor "
		"STailOfBed "
		"100 STailOfBed";
	Stream.ProductTokenStream(formular);
}

FTest(MetisFormulaContainer)
{
	std::string formular = "#rule=+2#WallParallelWithDoor*1000*1$";

	//MetisFormulaContainer Container;
	//MetisRegionResolver N;
	//Container.Test(formular);
}


void GeneratorRect(const MetisRegionResolver& Resolver, std::vector<FMetisRect> &Ref) {
	Ref.reserve(Resolver.max_row*Resolver.max_rank);
	auto  p = Resolver.independent_regulation_map.find(0);
	auto &sssss = p->second[0];
    float MAX = -FLT_MAX;
	for (int i = 0; i < Resolver.max_row; ++i) {
		for (int j = 0; j < Resolver.max_rank; ++j) {
			if (sssss[i][j]->Weight > MAX)
				MAX = sssss[i][j]->Weight;
		}
	}
	for (int i = 0; i < Resolver.max_row; ++i) {
		for (int j = 0; j < Resolver.max_rank; ++j) {
			if (!sssss[i][j]->bInside)
				continue;
			FMetisRect rect;
			rect.LeftTop = XMFLOAT3(sssss[i][j]->top_left_point.x, sssss[i][j]->top_left_point.y, 0.f);
			rect.RightTop = XMFLOAT3(sssss[i][j]->top_right_point.x, sssss[i][j]->top_right_point.y, 0.f);
			rect.LeftBottom = XMFLOAT3(sssss[i][j]->bottom_left_point.x, sssss[i][j]->bottom_left_point.y, 0.f);
			rect.RightBottom = XMFLOAT3(sssss[i][j]->bottom_right_point.x, sssss[i][j]->bottom_right_point.y, 0.f);
			rect.Color = XMFLOAT4(0, 1, 0, sssss[i][j]->Weight / MAX);
			Ref.push_back(rect);
		}
	}
}

void GeneratorRectForAttachedLayout(const MetisRegionResolver& Resolver, std::vector<FMetisRect> &Ref) {
	Ref.reserve(Resolver.max_row*Resolver.max_rank);
	auto  p = Resolver.attach_regulation_map[0].find(0);
	//第1套布局的第一套规则
	auto &sssss = p->second[0];
    float MAX = -FLT_MAX;
	float Min = FLT_MAX;
	for (int i = 0; i < Resolver.max_row; ++i) {
		for (int j = 0; j < Resolver.max_rank; ++j) {
			if (sssss[i][j]->Weight > MAX)
				MAX = sssss[i][j]->Weight;
		}
	}
	for (int i = 0; i < Resolver.max_row; ++i) {
		for (int j = 0; j < Resolver.max_rank; ++j) {
			if (sssss[i][j]->Weight < Min)
				Min = sssss[i][j]->Weight;
		}
	}
	MAX -= Min;
	for (int i = 0; i < Resolver.max_row; ++i) {
		for (int j = 0; j < Resolver.max_rank; ++j) {
			FMetisRect rect;
			rect.LeftTop = XMFLOAT3(sssss[i][j]->top_left_point.x, sssss[i][j]->top_left_point.y, 0.f);
			rect.RightTop = XMFLOAT3(sssss[i][j]->top_right_point.x, sssss[i][j]->top_right_point.y, 0.f);
			rect.LeftBottom = XMFLOAT3(sssss[i][j]->bottom_left_point.x, sssss[i][j]->bottom_left_point.y, 0.f);
			rect.RightBottom = XMFLOAT3(sssss[i][j]->bottom_right_point.x, sssss[i][j]->bottom_right_point.y, 0.f);
			rect.Color = XMFLOAT4(0, 1, 0, (sssss[i][j]->Weight-Min) / MAX);
			Ref.push_back(rect);
		}
	}
 
}

void GeneratorResolverForAttachedLayout(MetisRegionResolver&Resolver) {
	
	SingleFurnitureSingleRuleLayoutData data;
	data.rotation_degree =90;
	data.furniture_vertex_points.push_back(Point3f(3000, 2000, 0));
	data.furniture_vertex_points.push_back(Point3f(3000, 3000, 0));
	data.furniture_vertex_points.push_back(Point3f(2000, 3000, 0));
	data.furniture_vertex_points.push_back(Point3f(2000, 2000, 0));
	std::map<int, SingleFurnitureSingleRuleLayoutData> Data;
	Data.insert(std::make_pair(1, data));
	Resolver.all_furniture_weight_combo_data.push_back(Data);
}

void GeneratorRegion(const SingleCase&Case, FMetisRegion& Region) {
	for (auto wall : Case.wall_list) {
		FMetisLine Line;
		Line.Point1.x = wall.start_corner.point.x;
		Line.Point1.y = wall.start_corner.point.y;
		Line.Point1.z= wall.start_corner.point.z;
 
		Line.Point2.x = wall.end_corner.point.x;
		Line.Point2.y = wall.end_corner.point.y;
		Line.Point2.z = wall.end_corner.point.z;
		Region.Lines.push_back(Line);
		/* 
		for (auto door : wall.door_list) {
			if (door.start_point.x == door.end_point.x) {
				FMetisRect rect;
				rect.LeftTop = XMFLOAT3(door.start_point.x+10, door.start_point.y, 0.f);
				rect.RightTop = XMFLOAT3(door.end_point.x+ 10, door.end_point.y, 0.f);
				rect.LeftBottom = XMFLOAT3(door.start_point.x, door.start_point.y, 0.f);
				rect.RightBottom = XMFLOAT3(door.end_point.x, door.end_point.y, 0.f);
				rect.Color = XMFLOAT4(1,0,0,1);
				Region.Layouts.push_back(rect);
			}
			else
			{
				FMetisRect rect;
				rect.LeftTop = XMFLOAT3(door.start_point.x, door.start_point.y+ 10, 0.f);
				rect.RightTop = XMFLOAT3(door.end_point.x, door.end_point.y+ 10, 0.f);
				rect.LeftBottom = XMFLOAT3(door.start_point.x, door.start_point.y, 0.f);
				rect.RightBottom = XMFLOAT3(door.end_point.x, door.end_point.y, 0.f);
				rect.Color = XMFLOAT4(1, 0, 0, 1);
				Region.Layouts.push_back(rect);
			}
		}*/
		for (auto win : wall.window_list) {
			if (win.start_point.x == win.end_point.x) {
				FMetisRect rect;
				rect.LeftTop = XMFLOAT3(win.start_point.x + 10, win.start_point.y, 0.f);
				rect.RightTop = XMFLOAT3(win.end_point.x + 10, win.end_point.y, 0.f);
				rect.LeftBottom = XMFLOAT3(win.start_point.x, win.start_point.y, 0.f);
				rect.RightBottom = XMFLOAT3(win.end_point.x, win.end_point.y, 0.f);
				rect.Color = XMFLOAT4(0, 0, 1, 1);
				Region.Layouts.push_back(rect);
			}
			else
			{
				FMetisRect rect;
				rect.LeftTop = XMFLOAT3(win.start_point.x, win.start_point.y + 10, 0.f);
				rect.RightTop = XMFLOAT3(win.end_point.x, win.end_point.y + 10, 0.f);
				rect.LeftBottom = XMFLOAT3(win.start_point.x, win.start_point.y, 0.f);
				rect.RightBottom = XMFLOAT3(win.end_point.x, win.end_point.y, 0.f);
				rect.Color = XMFLOAT4(0, 0, 1, 1);
				Region.Layouts.push_back(rect);
			}
		}
	}
	//add layout
	FMetisLayout MetisRect(0,0,0);
	MetisRect.LeftTop = XMFLOAT3(3000, 2000, 0);
	MetisRect.RightTop = XMFLOAT3(3000, 3000, 0);
	MetisRect.LeftBottom= XMFLOAT3(2000, 2000, 0);
	MetisRect.RightBottom = XMFLOAT3(2000, 3000, 0);
	MetisRect.Color = XMFLOAT4(1, 1, 1,0.2f);
	Region.AddLayout(&MetisRect);

}

void GeneratorRegionFromAiCase(AICase*ai_case, FMetisRegion& Region)
{

	for (auto &ref : ai_case->single_case_list) {
		GeneratorRegion(ref, Region);
	}
}


#if 0
FTest(MetisWeight)
{
	WinLaunch::InitializeWindows();
	D3D11HRI::InitializeHRI(WinLaunch::hWnd, 800, 600);
	RenderProxy Proxy;
	FMetisRegion Region;
	SingleCase Case;
	GeneratorRoom(Case);
	GeneratorRegion(Case, Region);
	MetisRegionResolver  Resolver;
	std::vector<FMetisRect>  Ref;
	GeneratorResolver(Resolver);
	{
		MetisFormulaContainer Container;
		
		Container.ConstructContainer(
			"Name=DWin*0*1;"
			/*Name=CWall*2000*1;*/
			/*Name=CWallWithWin*2000*1;*/
			/*Name=CWallWithWinDoor*2000*1;*/
			/*Name=CWallWithDoor*2000*1;*/
			/*Name=CWallWithNothing*2000*1;*/
			/*Name=DDoor*200*1;*/
			/*Name=DWin*200*1;*/
			, &Resolver, &Case
		);
		for (auto j : Container.m_weight_map) {
			for (auto m :j.second) {
				for (auto i : m) {
					FMetisRect rect;
					rect.LeftTop = XMFLOAT3(i->top_left_point.x, i->top_left_point.y, 0.f);
					rect.RightTop = XMFLOAT3(i->top_right_point.x, i->top_right_point.y, 0.f);
					rect.LeftBottom = XMFLOAT3(i->bottom_left_point.x, i->bottom_left_point.y, 0.f);
					rect.RightBottom = XMFLOAT3(i->bottom_right_point.x, i->bottom_right_point.y, 0.f);
					rect.Color = XMFLOAT4(0, 1, 0, i->Weight);
					Ref.push_back(rect);
				}
				break;
			}
		}
	}
	//GeneratorRect(Resolver, Ref);
	Proxy.InitializeProxy();
	Proxy.RegisterProxy(&Region,nullptr);
	Proxy.RegisterGrid(&Ref[0], Ref.size());

	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		Proxy.Render();
	}

}
#endif
FTest(SingleCase)
{
	AICase* src_case = new AICase();
	string src_houselayout = "C:\\Users\\Administrator\\Desktop\\BIMDATA\\2\\DrawHome\\h.json";
	// 初始化模板数据
	Json::Reader reader;
	Json::Value root;
	fstream f;
	// 初始化模板户型
	f.open(src_houselayout, ios::in);
	if (!f.is_open()) {
		cout << "Open src_houselayout file error!" << endl;
		return;
	}
	else
	{
		if (reader.parse(f, root))
		{
			src_case->house.LoadBimData(root);
			f.close();
		}
		else
		{
			cout << "parse src_houselayout error\n" << endl;
			return;
		}
	}

	// 初始化模板设计
	src_case->Init();
	src_case->InitSingleCase();
	WinLaunch::InitializeWindows();
	D3D11HRI::InitializeHRI(WinLaunch::hWnd, 600, 800);
	RenderProxy Proxy;
	std::vector<FMetisRegion> Region;
	Region.resize(src_case->single_case_list.size());
	// 进行单房间匹配
    std::vector<FMetisRect> ref;
	for (auto &single_case : src_case->single_case_list)
	{
		//std::shared_ptr<SingleCase> ptr_single_case(&single_case);
		//shared_ptr<MetisRegionResolver> region_resolver = make_shared<MetisRegionResolver>(ptr_single_case);
		GeneratorRegion(single_case, Region[0]);
		std::shared_ptr<SingleCase> scase = std::make_shared<SingleCase>(single_case);
		
        MetisRegionResolver resolver(scase);
        MetisFormulaContainer container;
        container.ConstructWeightMapWithAllTypeTest(&resolver, &single_case);
    
        GeneratorRectForAttachedLayout(resolver, ref);


	}
	Proxy.InitializeProxy();
	Proxy.RegisterProxy(&Region[0], nullptr);
    Proxy.RegisterGrid(&ref[0], (unsigned)ref.size());
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
		   TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
		Proxy.Render();
	}

	// í?2?éè??êy?Y
	src_case->SyncAICaseBySingle();



}
SingleCase*  Generatecase(const char*path) {
	AICase* src_case = new AICase();
	string src_houselayout = path;
	Json::Reader reader;
	Json::Value root;
	fstream f;
	f.open(src_houselayout, ios::in);
	if (!f.is_open()) {
		cout << "Open src_houselayout file error!" << endl;
		return nullptr;
	}
	else
	{
		if (reader.parse(f, root))
		{
			src_case->house.LoadBimData(root);
			f.close();
		}
		else
		{
			cout << "parse src_houselayout error\n" << endl;
			return nullptr;
		}
	}
	src_case->InitSingleCase();
	return &src_case->single_case_list[0];
}
FTest(MetisHouseMatcher) {
	std::string path = "C:\\Users\\Administrator\\Desktop\\Template\\";
	std::vector<std::string> smatcher;
	for (int i = 1; i <= 9; ++i) {
		char psd[24];
		sprintf_s(psd, "%d", i);
		auto c = path;
		c += psd;
		smatcher.push_back(c);
	}
	MetisRegionMatcher Mathcer;
	Mathcer.FillTemplateFeaure(smatcher);



	auto sngleCase = Generatecase("C:\\Program Files (x86)\\BIM\\XR\\Content\\DBJCache\\DrawHome\\1.json");

	auto c = Mathcer.FindHighSimilarityTemplate(sngleCase, 1|2|4);

	
	auto b = Mathcer.GetSimilarityTemplateIndex();
	printf("%d\n", b);
	system("pause");
}
///////////////////////////////////////////////////
#include "HttpServer/MetisHttpServer.h"
#include "KitchenAutomaticalDesign.h"

void SGGGG(std::vector< std::vector<KitchenDesign::CabinetWall>>&DSD, std::vector<FMetisRect>&ref) {
	auto &DDDD = DSD[1];
	for (auto &casss : DDDD)
		for (int i = 0; i < casss.furnitureBeg.size(); ++i)
		{	 
				auto beg = casss.furnitureBeg[i];
				auto end = casss.furnitureEnd[i];
				FMetisRect rect;
				rect.Color = XMFLOAT4(1, 1,0, 1.f);
				if (beg.x == end.x) {
					rect.LeftTop = XMFLOAT3(beg.x + 40, beg.y, 20.f);
					rect.RightTop = XMFLOAT3(end.x + 40, end.y, 20.f);
					rect.LeftBottom = XMFLOAT3(beg.x, beg.y, 20.f);
					rect.RightBottom = XMFLOAT3(end.x, end.y, 20.f);
					ref.push_back(rect);
				}
				else
				{
					rect.LeftTop = XMFLOAT3(beg.x, beg.y + 40, 20.f);
					rect.RightTop = XMFLOAT3(end.x, end.y + 40, 20.f);
					rect.LeftBottom = XMFLOAT3(beg.x, beg.y, 20.f);
					rect.RightBottom = XMFLOAT3(end.x, end.y, 20.f);
					ref.push_back(rect);
				} 
		}
	for (auto &casss : DDDD)
		for (int i = 0; i < casss.subLayout.size();++i)
		{
			for (int j = 0; j < casss.subLayout[i].furnitureBeg.size(); ++j)
			{
				auto beg = casss.subLayout[i].furnitureBeg[j];
				auto end = casss.subLayout[i].furnitureEnd[j];
				FMetisRect rect;
			 	if(casss.subLayout[i].furnitureType[j]== KitchenDesign::E_Cooker)
			 		rect.Color = XMFLOAT4(1, 0, 0, 1.f);
			 	if (casss.subLayout[i].furnitureType[j] == KitchenDesign::E_Predinner)
			 		rect.Color = XMFLOAT4(1, 1, 1, 1.f);
			 	if (casss.subLayout[i].furnitureType[j] == KitchenDesign::E_Water_Solt)
			 		rect.Color = XMFLOAT4(0, 0, 1, 1.f);
			 
				if (beg.x == end.x) {
					rect.LeftTop = XMFLOAT3(beg.x + 20, beg.y, 30.f);
					rect.RightTop = XMFLOAT3(end.x + 20, end.y, 30.f);
					rect.LeftBottom = XMFLOAT3(beg.x, beg.y, 30.f);
					rect.RightBottom = XMFLOAT3(end.x, end.y, 30.f);		 
					ref.push_back(rect);
				}
				else
				{
					rect.LeftTop = XMFLOAT3(beg.x, beg.y + 20, 30.f);
					rect.RightTop = XMFLOAT3(end.x, end.y + 20, 30.f);
					rect.LeftBottom = XMFLOAT3(beg.x, beg.y, 30.f);
					rect.RightBottom = XMFLOAT3(end.x, end.y, 30.f);	 
					ref.push_back(rect);
				}
			}
			if (casss.subLayout[i].fridgeType.size())
			{
				FMetisRect rect;
				auto beg = casss.subLayout[i].fridgeBeg;
				auto end = casss.subLayout[i].fridgeEnd;
				if (beg.x == end.x) {
					rect.LeftTop = XMFLOAT3(beg.x + 20, beg.y, -10.f);
					rect.RightTop = XMFLOAT3(end.x + 20, end.y, -10.f);
					rect.LeftBottom = XMFLOAT3(beg.x, beg.y, -10.f);
					rect.RightBottom = XMFLOAT3(end.x, end.y, -10.f);
					rect.Color = XMFLOAT4(1, 1, 1, 1.f);
					ref.push_back(rect);
				}
				else
				{
					rect.LeftTop = XMFLOAT3(beg.x, beg.y + 20, -10.f);
					rect.RightTop = XMFLOAT3(end.x, end.y + 20, -10.f);
					rect.LeftBottom = XMFLOAT3(beg.x, beg.y, -10.f);
					rect.RightBottom = XMFLOAT3(end.x, end.y, -10.f);
					rect.Color = XMFLOAT4(1, 1, 1, 1.f);
					ref.push_back(rect);
				}
			}
		 }
}

void GGGG(SingleCase*Case,std::vector<FMetisRect>&ref) {
	for (auto model : Case->layout_list[1].model_list) {	 
		FMetisRect rect;	 
		auto loc = XMFLOAT3(model.location.x, model.location.y, 0);
		rect.LeftTop = XMFLOAT3(-model.width*0.5f, model.length*0.5f, 20.f);
		rect.RightTop = XMFLOAT3(model.width*0.5f, model.length*0.5f,  20.f);
		rect.LeftBottom = XMFLOAT3(-model.width*0.5f, -model.length*0.5f, 20.f);
		rect.RightBottom = XMFLOAT3(model.width*0.5f, -model.length*0.5f, 20.f);
		rect.Color = XMFLOAT4(1.f, 0.f, 0.f, 0.3f);
		if (model.id == 20003)
			rect.Color = XMFLOAT4(0, 0, 1, 0.3f);
		auto cross = ComMath::Cross(Point3f(1.f, 0.f, 0.f), model.rotation);
		float angle = model.rotation.y / 360.f*(3.1415926f * 2);
	 

		auto x = cosf(angle)*rect.LeftTop.x - sinf(angle)*rect.LeftTop.y;
		rect.LeftTop.y = sinf(angle)*rect.LeftTop.x + cosf(angle)*rect.LeftTop.y;
		rect.LeftTop.x = x;

 		rect.RightTop.y = sinf(angle)*rect.RightTop.x + cosf(angle)*rect.RightTop.y;
		rect.RightTop.x = x;

		 x = cosf(angle)*rect.LeftBottom.x - sinf(angle)*rect.LeftBottom.y;
		rect.LeftBottom.y = sinf(angle)*rect.LeftBottom.x + cosf(angle)*rect.LeftBottom.y;
		rect.LeftBottom.x = x;

		 x = cosf(angle)*rect.RightBottom.x - sinf(angle)*rect.RightBottom.y;
		rect.RightBottom.y = sinf(angle)*rect.RightBottom.x + cosf(angle)*rect.RightBottom.y;
		rect.RightBottom.x = x;

		rect.LeftTop = rect.LeftTop+loc;
		rect.RightTop = rect.RightTop + loc;
		rect.LeftBottom = rect.LeftBottom + loc;
		rect.RightBottom = rect.RightBottom + loc;
		ref.push_back(rect);	 
	}
}
FTest(Server)
{
	//KitchenAutomaticalDesign Design;
	KitchenDesign Design;
  
	auto single_case = Generatecase("C:\\Users\\40451\\Desktop\\1.json");
	Design.AutoDesign(single_case);

	auto result = Design.Get();

	std::vector<FMetisRect> Ref;
	//GGGG(single_case, Ref);
	// GenerateRectforKitchen3(result, Ref);
	 //GenerateRectforKitchen(result, Ref);
	WinLaunch::InitializeWindows();
	D3D11HRI::InitializeHRI(WinLaunch::hWnd, 600, 600);
	RenderProxy Proxy;
	FMetisRegion Region;
	GeneratorRegion(*single_case, Region);
 
	Proxy.InitializeProxy();
	Proxy.RegisterProxy(&Region, nullptr);
	//Proxy.RegisterGrid(&Ref[0], Ref.size());
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		Proxy.Render();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
#include "MoveLinesEstimator.h"

void BuildRectFromEstimator(std::vector<std::vector<EstimatorNode>>	&base_leaf_node, std::vector<FMetisRect>&rxef)
{

	for (auto &ref : base_leaf_node)
		for (auto &node : ref) {
			FMetisRect Rect;
			auto x = node.centre_point.x;
			auto y = node.centre_point.y;

			Rect.LeftTop = XMFLOAT3(x - 8, y + 8, 10);
			Rect.RightTop = XMFLOAT3(x + 8, y + 8, 10);
			Rect.LeftBottom = XMFLOAT3(x - 8, y - 8, 10);
			Rect.RightBottom = XMFLOAT3(x +8, y - 8, 10);
			 if (node.flag&E_Entity)
			{
				Rect.Color = XMFLOAT4(1.f, 0.f, 0.f, 0.4f);
				rxef.push_back(Rect);
			}
			else   if (node.flag&E_Outside) {
				Rect.Color = XMFLOAT4(1.f, 1.f, 1.f, 0.4f);
				rxef.push_back(Rect);
			}
			else if (node.flag&E_Debug) {
				Rect.Color = XMFLOAT4(0.f, 0.f,1.f, 0.4f);
				rxef.push_back(Rect);
			}
			else{
				Rect.Color = XMFLOAT4(0, 1, 0, 0.4f);
				rxef.push_back(Rect);
			}
	}
}

void BuildRectFromAicase(AICase*ai_case, std::vector<FMetisRect>&rxef) {

	for (auto &Model : ai_case->design.model_list) {
		FMetisRect Rect;
		auto x = Model.location.x;
		auto y = Model.location.y;
		Rect.LeftTop = XMFLOAT3(x - 10, y + 10, 10);
		Rect.RightTop = XMFLOAT3(x + 10, y + 10, 10);
		Rect.LeftBottom = XMFLOAT3(x - 10, y - 10, 10);
		Rect.RightBottom = XMFLOAT3(x + 10, y - 10, 10);
		Rect.Color = XMFLOAT4(1, 1, 1, 1);
		rxef.push_back(Rect);
	}
}

AICase * getAiCase(string hosue_name, string design_name)
{
 
	AICase* src_case = new AICase();

	Json::Reader reader;
	Json::Value root;
	fstream f;

	// 初始化模板户型
	f.open(hosue_name, ios::in);
	if (!f.is_open())
	{
		cout << "Open hosue_name file error!=" << hosue_name << endl;
		return NULL;
	}
	else
	{
		if (reader.parse(f, root))
		{
			src_case->house.LoadAppData(root);
			f.close();
		}
		else
		{
			cout << "Open hosue_name file error!=" << hosue_name << endl;
			return NULL;
		}
	}

	// 初始化模板设计
	f.open(design_name, ios::in);
	if (!f.is_open())
	{
		cout << "Open design_name file error!=" << design_name << endl;
		return NULL;
	}
	else
	{
		if (reader.parse(f, root))
		{
			src_case->design.LoadAppData(root);
			f.close();
		}
		else
		{
			cout << "Open design_name file error!=" << design_name << endl;
			return NULL;
		}
	}

	src_case->Init();
	// 初始化单房间数据
	src_case->InitSingleCase();
 

	return src_case;
}
 
FTest(AICase)
{

	auto ai_case = getAiCase("C:\\Users\\40451\\Desktop\\huxing.json",
		"C:\\Users\\40451\\Desktop\\sheji.json"
	);
	vector<int> house_work_id{ 30019, 30022, 10001, 20002 };
	//vector<unsigned> effective_house_work_id = ai_case->GetEffectiveModelList(house_work_id);
	vector<Model> model_list;
	for (int i = 0; i < ai_case->design.model_list.size(); i++)
	{
		Model tmp = ai_case->design.model_list[i];
		vector<int>::iterator ret;
		ret = find(house_work_id.begin(), house_work_id.end(), tmp.id);
		if (ret == house_work_id.end())
		{
			continue;
		}
		else
		{
			model_list.push_back(tmp);
		}
	}
	MoveLinesEstimator estimator;
	estimator.SetHouseworkModelId(model_list);
	estimator.EstimateModelMoveLines(ai_case);
	auto lines=estimator.GetMoveLines();
	 
	for (auto i : lines)
	{
		auto &room= ai_case->single_case_list[0];
		
		Wall wall;
		wall.start_corner.point = i.start;
		wall.end_corner.point = i.end;
		room.wall_list.push_back(wall);
	}


 	WinLaunch::InitializeWindows();
	D3D11HRI::InitializeHRI(WinLaunch::hWnd, 600, 600);
	RenderProxy Proxy;
	FMetisRegion Region;
	std::vector<FMetisRect> rect;
	GeneratorRegionFromAiCase(ai_case,Region);
	 //BuildRectFromEstimator(ref, rect);
	 BuildRectFromAicase(ai_case, rect);
	Proxy.InitializeProxy();
	Proxy.RegisterProxy(&Region, nullptr);
	Proxy.RegisterGrid(&rect[0],(unsigned)rect.size());
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		Proxy.Render();
	}

}

FTest(AICase2) {
	auto ai_case = getAiCase("C:\\Users\\40451\\Desktop\\house2.json",
		"C:\\Users\\40451\\Desktop\\design2.json"
	);
	std::vector<Model> temp;
	for (auto model : ai_case->design.model_list) {
		if (model.id == 193)
			continue;
		temp.push_back(model);
	}
	ai_case->design.model_list = temp;

 
	MoveLinesEstimator estimator;
	std::vector<std::string> model_is;
	model_is.push_back("sdsd");
	model_is.push_back("sdsd");
	model_is.push_back("sdsd");
	model_is.push_back("sdsd");
	estimator.SetHouseworkModelId(temp);
	auto &ref = estimator.EstimateMoveLines(ai_case);




	WinLaunch::InitializeWindows();
	D3D11HRI::InitializeHRI(WinLaunch::hWnd, 600, 600);
	RenderProxy Proxy;
	FMetisRegion Region;
	std::vector<FMetisRect> rect;
	GeneratorRegionFromAiCase(ai_case, Region);
	BuildRectFromEstimator(ref, rect);
	BuildRectFromAicase(ai_case, rect);
	Proxy.InitializeProxy();
	Proxy.RegisterProxy(&Region, nullptr);
	Proxy.RegisterGrid(&rect[0], (unsigned)rect.size());
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		Proxy.Render();
	}

}

FTest(HTTP)
{
	MetisHttpServer server;
	server.InitializeMetisHttpServer(1);
}

 
