/*
 * types.h - Basic types definition.
 *
 * The confidential and proprietary information contained in this file may
 * only be used by a person authorised under and to the extent permitted
 * by a subsisting licensing agreement from ARM Limited.
 *
 * Copyright 2007-2009 ARM Limited.
 * All rights reserved.
 *
 * This entire notice must be reproduced on all copies of this file
 * and copies of this file may only be made by a person if such person is
 * permitted to do so under the terms of a subsisting license agreement
 * from ARM Limited.
 */

/*
 * Revision $Revision: 27837 $
 * Checkin $Date: 2009-04-22 18:13:06 +0100 (Wed, 22 Apr 2009) $
 * Revising $Author: pgotch $
 */

#ifndef TYPES__H
#define TYPES__H

/* Includes */
#include <amba_pv.h>

/* Datatypes */

/*
 * Basic address and data types.
 */
typedef sc_dt::uint64 addr_t;
typedef sc_dt::uint64 data_t;

/*
 * Bus width in bits.
 */
static const unsigned int BUSWIDTH = sizeof(data_t) * 8;

#endif  /* defined(TYPES__H) */
