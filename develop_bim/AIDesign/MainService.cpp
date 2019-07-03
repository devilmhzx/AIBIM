/*************************************************
*  Copyright(c) 2018-2020 DaBanJia
*  All rights reserved.
*
*  文件名称:启动程序文件
*  简要描述:
*
*  当前版本:
*  作者:常远
*  创建日期:2018-11-22
*  说明:
**************************************************/

//#include "Log/easylogging++.h"
//#include "HttpServer/MetisHttpServer.h"
//
//// 初始化日志
//void LogInit()
//{
//	el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
//	el::Configurations conf(LOG_INIT_PATH);
//	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::MaxLogFileSize,
//		"100000000");
//	/// 也可以设置全部logger的配置  
//	el::Loggers::reconfigureAllLoggers(conf);
//}
//
//int main()
//{
//	TryDump(
//	LogInit();
//	DatabaseHelper * p_db = DatabaseHelper::initance();
//	if (!p_db->initDB(DB_PATH))
//	{
//		LOG(ERROR) << "database error!";
//		return 0;
//	}
//
//	LOG(INFO) << "start service......";
//	MetisHttpServer *http_server = new MetisHttpServer();
//	http_server->InitializeMetisHttpServer(10);
//	);
//}