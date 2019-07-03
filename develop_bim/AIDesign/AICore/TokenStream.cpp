#include "TokenStream.h"
#include <string.h>


TokenStream::TokenStream()
{
	m_stream.reserve(INIT_MAX_TOKEN_STREAM);
	AddAllReseverKeyword();
}


TokenStream::~TokenStream()
{
}

bool TokenStream::ProductTokenStream(const std::string& formular) 
{	
	//清理
	m_stream.clear();
	const char* ref = formular.data();
	unsigned index = 0;
	while (index< formular.size())
	{
		if (ref[index] == '\t')
		{
			++index;
			continue;
		}
		if (isalpha(ref[index])) {
			std::string token_string;
			//first variable [a-z][A-Z]
			token_string+= ref[index];
			++index;
			while (isalnum(ref[index]))
			{
				token_string += ref[index];
				++index;
			} 
			if (!ParseTokenType(token_string))
				return false;
		 }
		else if (isdigit(ref[index]) || ref[index]=='.')
		{
			bool point = false;
			if (ref[index] == '.')
				point = true;
			std::string token_string;
			//first variable [0-9]
			token_string += ref[index];
			++index;
			while (isdigit(ref[index])|| (!point&&ref[index]=='.'))
			{
				if (ref[index] == '.')
					point = true;
				token_string += ref[index];
				++index;
			}
			if (!ParseTokenNumber(token_string))
				return false;
		}
		else if(ref[index] != '\n')
		{
			std::string token_string;
			token_string += ref[index];
			++index;
			if (!ParseTokenOperator(token_string))
				return false;
		}
	}
	return true;
}

bool TokenStream::ParseTokenType(const std::string& identity) 
{
	auto find = m_token_resever.find(identity);
	if (find == m_token_resever.end())
		m_stream.push_back(Token(ETokenType::E_IDENTTY, identity));
	else
		m_stream.push_back(Token(find->second));
	return true;
}

bool TokenStream::ParseTokenNumber(const std::string&identity)
{
	float value = (float)std::atof(identity.data());
	m_stream.push_back(Token(ETokenType::E_NUMBER, value));
	return true;
}

bool TokenStream::ParseTokenOperator(const std::string&identity)
{
	if (TokenResever::equl == identity)
		m_stream.push_back(Token(ETokenType::E_EQUL));
	else if (TokenResever::minus == identity)
		m_stream.push_back(Token(ETokenType::E_Minus));
	else if (TokenResever::mul == identity)
		m_stream.push_back(Token(ETokenType::E_Mul));
	else if (TokenResever::plus == identity)
		m_stream.push_back(Token(ETokenType::E_Plus));
	else if (TokenResever::div == identity)
		m_stream.push_back(Token(ETokenType::E_Div));
	else if (TokenResever::semicolon == identity)
		m_stream.push_back(Token(ETokenType::E_Semicolon));
	else if(TokenResever::dollar==identity)
		m_stream.push_back(Token(ETokenType::E_DOLLAR));
    else if(TokenResever::at == identity)
        m_stream.push_back(Token(ETokenType::E_AT));
	else if (TokenResever::pound == identity)
		m_stream.push_back(Token(ETokenType::E_POUND));
	else if (TokenResever::blank == identity)
		return true;
	else 
		return false;
	return true;
}

void TokenStream::AddAllReseverKeyword()
{
#define INSERT(X,Y) m_token_resever.insert(std::make_pair(X,Y))
	INSERT(TokenResever::closeWall, ETokenType::E_CloseWall);
	INSERT(TokenResever::closeWin, ETokenType::E_CloseWin);
	INSERT(TokenResever::closeDoor, ETokenType::E_CloseDoor);
	INSERT(TokenResever::divergeDoor, ETokenType::E_DivergeDoor);
	INSERT(TokenResever::divergeWin, ETokenType::E_DivergeWin);
	INSERT(TokenResever::closeWallWithWin, ETokenType::E_CloseWallWithWin);
	INSERT(TokenResever::closeWallWithDoor, ETokenType::E_CloseWallWithDoor);
	INSERT(TokenResever::closeWallWithWinDoor, ETokenType::E_CloseWallWithWinDoor);
	INSERT(TokenResever::closeWallWithNothing, ETokenType::E_CloseWallWithNothing);
	INSERT(TokenResever::closeCorner, ETokenType::E_CloseCorner);
	INSERT(TokenResever::sWall, ETokenType::E_IntervalWall);
	INSERT(TokenResever::sWallWithWin, ETokenType::E_IntervalWallWithWin);
	INSERT(TokenResever::sWallWithDoor, ETokenType::E_IntervalWallWithDoor);
	INSERT(TokenResever::sWallWithWinDoor, ETokenType::E_IntervalWallWithWinDoor);
	INSERT(TokenResever::sWallWithNothing, ETokenType::E_IntervalWallWithNothing);
	INSERT(TokenResever::wallParallelWithDoor, ETokenType::E_WallParallelWithDoor);
	INSERT(TokenResever::wallSectionOnLeftOfDoor, ETokenType::E_WallSectionOnLeftOfDoor);
	INSERT(TokenResever::wallSectionOnRightOfDoor, ETokenType::E_WallSectionOnRightOfDoor);
	INSERT(TokenResever::wallOnOrientationOfDoor, ETokenType::E_WallOnOrientationOfDoor);
	INSERT(TokenResever::wallOnReverseOfDoor, ETokenType::E_WallOnReverseOfDoor);
	INSERT(TokenResever::sectorRegionWithDoor, ETokenType::E_SectorRegionWithDoor);
	INSERT(TokenResever::sectorRegionOnOrientationOfDoor, ETokenType::E_SectorRegionOnOrientationOfDoor);
	INSERT(TokenResever::sectorRegionOnReverseOfDoor, ETokenType::E_SectorRegionOnReverseOfDoor);
	INSERT(TokenResever::sectorRegionWithWin, ETokenType::E_SectorRegionWithWin);
	INSERT(TokenResever::wallParallelWithWin, ETokenType::E_WallParallelWithWin);
	INSERT(TokenResever::wallSectionOnLeftOfWin, ETokenType::E_WallSectionOnLeftOfWin);
	INSERT(TokenResever::wallSectionOnRightOfWin, ETokenType::E_WallSectionOnRightOfWin);
	INSERT(TokenResever::wallsOnLeftOfWin, ETokenType::E_WallOnLeftOfWin);
	INSERT(TokenResever::wallsOnRightOfWin, ETokenType::E_WallOnRightOfWin);
	INSERT(TokenResever::cornerWithNearestDistToDoor, ETokenType::E_CornerWithNearestDistToDoor);
	INSERT(TokenResever::cornerWithFarsetDistToDoor, ETokenType::E_CornerWithFarestDistToDoor);
	INSERT(TokenResever::closePositiveCorner, ETokenType::E_ClosePositiveCorner);
	INSERT(TokenResever::closeNegativeCorner, ETokenType::E_CloseNegativeCorner);
	INSERT(TokenResever::closeActualWall, ETokenType::E_CloseActualWall);

	INSERT(TokenResever::fOrientationWall, ETokenType::E_FOrientationWall);
	INSERT(TokenResever::fLeftWall, ETokenType::E_FLeftWall);
	INSERT(TokenResever::fRightWall, ETokenType::E_FRightWall);
	INSERT(TokenResever::fReverseWall, ETokenType::E_FReverseWall);

	INSERT(TokenResever::fOrientationWallSection, ETokenType::E_FOrientationWallSection);
	INSERT(TokenResever::fLeftWallSection, ETokenType::E_FLeftWallSection);
	INSERT(TokenResever::fRightWallSection, ETokenType::E_FRightWallSection);
	INSERT(TokenResever::fReverseWallSection, ETokenType::E_FReverseWallSection);
	INSERT(TokenResever::fOrientationWallRadiation, ETokenType::E_FOrientationWallRadiation);


	INSERT(TokenResever::fOrientationWallSectionOnLeft, ETokenType::E_FOrientationWallSectionOnLeft);
	INSERT(TokenResever::fOrientationWallSectionOnRight, ETokenType::E_FOrientationWallSectionOnRight);
	INSERT(TokenResever::fReverseWallSectionOnLeft, ETokenType::E_FReverseWallSectionOnLeft);
	INSERT(TokenResever::fReverseWallSectionOnRight, ETokenType::E_FReverseWallSectionOnRight);

	INSERT(TokenResever::fOrientationLineRegion, ETokenType::E_FOrientationLineRegion);
	INSERT(TokenResever::fLeftLineRegion, ETokenType::E_FLeftLineRegion);
	INSERT(TokenResever::fRightLineRegion, ETokenType::E_FRightLineRegion);
	INSERT(TokenResever::fReverseLineRegion, ETokenType::E_FReverseLineRegion);

	INSERT(TokenResever::fOrientationSectorRegion, ETokenType::E_FOrientationSectorRegion);
	INSERT(TokenResever::fLeftSectorRegion, ETokenType::E_FLeftSectorRegion);
	INSERT(TokenResever::fRightSectorRegion, ETokenType::E_FRightSectorRegion);
	INSERT(TokenResever::fReverseSectorRegion, ETokenType::E_FReverseSectorRegion);
	INSERT(TokenResever::toiletCircleRegion, ETokenType::E_ToiletCircleRegion);
	INSERT(TokenResever::closeVirtualWall, ETokenType::E_CloseVirtualWall);
	INSERT(TokenResever::closeRadiation, ETokenType::E_CentreRadiation);
	INSERT(TokenResever::closeEntranceDoor, ETokenType::E_CloseEntranceDoor);
	INSERT(TokenResever::divergeSunshineDoor, ETokenType::E_DivergeSunshineDoor);
	INSERT(TokenResever::divergeNormalDoor, ETokenType::E_DivergeNormalDoor);
	INSERT(TokenResever::closeWallOnLeftOfLightEntrance, E_CloseWallOnLeftOfLightEntrance);
	INSERT(TokenResever::closeWallOnRightOfLightEntrance, E_CloseWallOnRightOfLightEntrance);
	INSERT(TokenResever::closeWallWithLightEntrance , E_CloseWallWithLightEntrance);
	INSERT(TokenResever::weightStripBetweenMaxWallWithCentre, E_WeightStripBetweenMaxWallWithCentre);
#undef INSERT
}