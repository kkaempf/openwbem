/*-------------------------------------------------------------------------
 *
 * scansup.h
 *	  scanner support routines.  used by both the bootstrap lexer
 * as well as the normal lexer
 *
 * Portions Copyright (c) 1996-2001, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * $Id: OW_WQLScanUtils.hpp,v 1.1.1.1 2002-03-01 16:58:19 nuffer Exp $
 *
 *-------------------------------------------------------------------------
 */

#ifndef OW_WQL_SCAN_UTILS_HPP_
#define OW_WQL_SCAN_UTILS_HPP_

#include "OW_config.h"

class OW_String;

OW_String RemoveEscapes(const OW_String& str);


#endif	 /* SCANSUP_H */
