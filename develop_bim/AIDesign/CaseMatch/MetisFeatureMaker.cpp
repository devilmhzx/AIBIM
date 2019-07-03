#include "MetisFeatureMaker.h"
#include "MetisRegionFeature.h"
#include "../SingleCase.h"
#include "../ComUtil.h"
#include "../DatabaseHelper.h"
bool MetisFeatureMaker::ExtractRegionFeatureToFileAndRecordToServer( SingleCase*single_case)
{
	shared_ptr<DatabaseHelper> p_db = std::make_shared<DatabaseHelper>();
	if (!p_db || !single_case)
		return false;
	SampleRoom sample_room;
	sample_room.no = ComUtil::getGuuidNo();
	sample_room.design_no = single_case->single_room.GetNo();
	sample_room.data_path = ComUtil::GetSampleDataPath(sample_room.no);
	sample_room.room_type = single_case->single_room.getSpaceId();
	sample_room.data = single_case->json_file_name;
	std::shared_ptr<MetisRegionFeature> feature = std::make_shared<MetisRegionFeature>();
 
	if (!feature->AnalyseCaseFeature(single_case, 36))
		return false;
	if (!feature->Serialization(sample_room.data_path.c_str()))
		return false;
	if (!p_db->insertSampleRoom(sample_room))
		return false;
	return true;
}
