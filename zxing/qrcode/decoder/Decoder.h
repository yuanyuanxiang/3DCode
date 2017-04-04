#ifndef __DECODER_H__
#define __DECODER_H__

/*
 *  Decoder.h
 *  zxing
 *
 *  Copyright 2010 ZXing authors All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <zxing/common/reedsolomon/ReedSolomonDecoder.h>
#include <zxing/common/Counted.h>
#include <zxing/common/Array.h>
#include <zxing/common/DecoderResult.h>
#include <zxing/common/BitMatrix.h>

namespace zxing {
namespace qrcode {

class Decoder {
private:
  ReedSolomonDecoder rsDecoder_;

  // EcLevel, added by yuanyuanxiang
  int m_nLevel;

  // Version, added by yuanyuanxiang
  int m_nVersion;

  // MaskingNo, added by yuanyuanxiang
  int m_nMaskingNo;

  void correctErrors(ArrayRef<char> bytes, int numDataCodewords);

public:
  Decoder();
  Ref<DecoderResult> decode(Ref<BitMatrix> bits);

  // Get EcLevel, added by yuanyuanxiang
  int GetErrorCorrectLevel() const;

  // Get Version, added by yuanyuanxiang
  int GetQRCodeVersion() const;

  // Get MsakingNo, added by yuanyuanxiang
  int GetMaskingNo() const;
};

}
}

#endif // __DECODER_H__
