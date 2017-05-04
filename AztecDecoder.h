#include "ZXingDeCoder.h"
#include "zxing/aztec/AztecReader.h"
using namespace zxing::aztec;

/**
* @class AztecDecoder
* @brief Aztec½âÂëÆ÷
*/
class AztecDecoder : public ZXingDecoder<AztecReader>
{
public:
	AztecDecoder(DecodeHints hint = DecodeHints::AZTEC_HINT) 
		: ZXingDecoder(hint)
	{

	}
	~AztecDecoder() { }
};