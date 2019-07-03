#include "Token.h"


bool Token::Compare(const Token*left, const Token*right) {
	if (abs(left->Type - right->Type) <= 1)
		return true;
	if (left->Type > right->Type)
			return true;
	else
			return false;
	return true;
}
std::string TokenResever::equl = "=";
std::string TokenResever::pound = "#";
std::string TokenResever::at = "@";
std::string TokenResever::dollar = "$";
std::string TokenResever::blank = " ";
std::string TokenResever::minus = "-";
std::string TokenResever::mul = "*";
std::string TokenResever::plus = "+";
std::string TokenResever::div = "/";
std::string TokenResever::semicolon = ";";
std::string TokenResever::closeWall = "CWall";
std::string TokenResever::closeWin = "CWin";
std::string TokenResever::closeDoor = "CDoor";
std::string TokenResever::divergeDoor = "DDoor";
std::string TokenResever::divergeWin = "DWin";
std::string TokenResever::closeWallWithWin = "CWallWithWin";
std::string TokenResever::closeWallWithDoor = "CWallWithDoor";
std::string TokenResever::closeWallWithWinDoor = "CWallWithWinDoor";
std::string TokenResever::closeWallWithNothing = "CWallWithNothing";
std::string TokenResever::sWall = "IWall";
std::string  TokenResever::sWallWithWin = "IWallWithWin";
std::string  TokenResever::sWallWithDoor= "IWallWithDoor";
std::string  TokenResever::sWallWithWinDoor= "IWallWithWinDoor";
std::string  TokenResever::sWallWithNothing = "IWallWithNothing";
std::string TokenResever::closeCorner = "CCorner";
std::string TokenResever::wallParallelWithDoor = "WallParallelWithDoor";
std::string TokenResever::wallSectionOnLeftOfDoor = "WallSectionOnLeftOfDoor";
std::string TokenResever::wallSectionOnRightOfDoor = "WallSectionOnRightOfDoor";
std::string TokenResever::wallOnOrientationOfDoor = "WallOnOrientationOfDoor";
std::string TokenResever::wallOnReverseOfDoor = "WallOnReverseOfDoor";
std::string TokenResever::sectorRegionWithDoor = "SectorRegionWithDoor";
std::string TokenResever::sectorRegionOnOrientationOfDoor = "SectorRegionOnOrientationOfDoor";
std::string TokenResever::sectorRegionOnReverseOfDoor = "SectorRegionOnReverseOfDoor";
std::string TokenResever::sectorRegionWithWin = "SectorRegionWithWin";
std::string TokenResever::wallParallelWithWin = "WallParallelWithWin";
std::string TokenResever::wallSectionOnLeftOfWin= "WallSectionOnLeftOfWin";
std::string TokenResever::wallSectionOnRightOfWin = "WallSectionOnRightOfWin";
std::string TokenResever::wallsOnLeftOfWin = "WallOnLeftOfWin";
std::string TokenResever::wallsOnRightOfWin = "WallOnRightOfWin";

std::string TokenResever::cornerWithNearestDistToDoor = "CornerWithNearestDistToDoor";
std::string TokenResever::cornerWithFarsetDistToDoor = "CornerWithFarestDistToDoor";
std::string TokenResever::closePositiveCorner = "ClosePositiveCorner";
std::string TokenResever::closeNegativeCorner = "CloseNegativeCorner";
std::string TokenResever::closeActualWall = "CloseActualWall";


std::string  TokenResever::fOrientationWall = "FOrientationWall";
std::string  TokenResever::fLeftWall = "FLeftWall";
std::string  TokenResever::fRightWall = "FRightWall";
std::string  TokenResever::fReverseWall ="FReverseWall";

std::string  TokenResever::fOrientationWallSection = "FOrientationWallSection";
std::string  TokenResever::fLeftWallSection = "FLeftWallSection";
std::string  TokenResever::fRightWallSection = "FRightWallSection";
std::string  TokenResever::fReverseWallSection = "FReverseWallSection";
std::string  TokenResever::fOrientationWallRadiation= "FOrientationWallRadiation";


std::string TokenResever::fOrientationWallSectionOnLeft = "FOrientationWallSectionOnLeft";
std::string TokenResever::fOrientationWallSectionOnRight = "FOrientationWallSectionOnRight";
std::string TokenResever::fReverseWallSectionOnLeft = "FReverseWallSectionOnLeft";
std::string TokenResever::fReverseWallSectionOnRight = "FReverseWallSectionOnRight";


std::string TokenResever::fOrientationLineRegion = "FOrientationLineRegion";
 std::string TokenResever::fLeftLineRegion = "FLeftLineRegion";
 std::string TokenResever::fRightLineRegion = "FRightLineRegion";
 std::string TokenResever::fReverseLineRegion = "FReverseLineRegion";

 std::string TokenResever::fOrientationSectorRegion = "FOrientataionSectorRegion";
 std::string TokenResever::fLeftSectorRegion = "FLeftSectorRegion";
 std::string TokenResever::fRightSectorRegion = "FRightSectorRegion";
 std::string TokenResever::fReverseSectorRegion = "FReverseSectorRegion";

std::string TokenResever::toiletCircleRegion = "ToiletCircleRegion";

std::string TokenResever::closeVirtualWall = "CloseVirtualWall";
std::string TokenResever::closeEntranceDoor = "CloseEntranceDoor";
std::string TokenResever::closeRadiation = "CloseRadiation";
std::string TokenResever::divergeSunshineDoor = "DivergeSunshineDoor";
std::string TokenResever::divergeNormalDoor = "DivergeNormalDoor";

std::string TokenResever::closeWallOnLeftOfLightEntrance = "CloseWallOnLeftOfLightEntrance";
std::string TokenResever::closeWallOnRightOfLightEntrance = "CloseWallOnRightOfLightEntrance";
std::string TokenResever::closeWallWithLightEntrance = "CloseWallWithLightEntrance";
std::string TokenResever::weightStripBetweenMaxWallWithCentre = "WeightStripBetweenMaxWallWithCentre";