/*
 * DetectorException.h
 *
 *  Created on: Aug 26, 2011
 *      Author: luiz
 */

#ifndef DETECTOREXCEPTION_H_
#define DETECTOREXCEPTION_H_

#include "zxing/ZXingException.h"

namespace zxing {
namespace datamatrix {

class DetectorException : public ZXingException {
  public:
    DetectorException(const char *msg);
    virtual ~DetectorException() throw();
};
} /* namespace nexxera */
} /* namespace zxing */
#endif /* DETECTOREXCEPTION_H_ */
