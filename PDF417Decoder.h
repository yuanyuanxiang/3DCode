#include "ZXingDeCoder.h"
#include "zxing/pdf417/PDF417Reader.h"
using namespace zxing::pdf417;

/**
* @class PDF417Decoder
* @brief PDF417½âÂëÆ÷
*/
class PDF417Decoder : public ZXingDecoder<PDF417Reader>
{
public:
	PDF417Decoder(DecodeHints hint = DecodeHints::PDF_417_HINT) 
		: ZXingDecoder(hint)
	{

	}
	~PDF417Decoder() { }
};