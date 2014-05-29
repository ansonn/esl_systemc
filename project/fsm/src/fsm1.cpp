/*
 * fsm1.cpp
 *
 *  Created on: 2014年5月29日
 *      Author: SPREADTRUM\ansonn.wang
 */

#include "fsm1.h"

void fsm_1::update_state()
{
	if (rst.read() == true)
		state = SO;
	else
		state = next_state;
}

void fsm_1::ns_op_logic()
{
	switch (state)
	{
	case SO:
		b.write(0);
		if (input1.read() || input2.read())
			a.write(1);
		else
			a.write(0);

		if (input1.read() == 1)
			next_state = S1;
		else
			next_state = SO;

		break;
	case S1:
		a.write(0);
		b.write(1);
		if (input2.read() == 1)
			next_state = S2;
		else
			next_state = SO;

		break;
	case S2:
		a.write(0);
		b.write(0);
		next_state = SO;

		break;
	default:
		a.write(0);
		b.write(1);
		next_state = SO;

		break;
	}
}

