/*****************************************************************************
 *
 *   Portable Xerox AltoII cursor task
 *
 *   Copyright: Juergen Buchmueller <pullmoll@t-online.de>
 *
 *   Licenses: MAME, GPLv2
 *
 *****************************************************************************/
#include "alto2.h"

/**
 * @brief f1_curt_block early: disable the cursor task and set the curt_blocks flag
 */
void alto2_cpu_device::f1_curt_block_0()
{
	m_task_wakeup &= ~(1 << m_task);
	LOG((LOG_CURT,2,"	BLOCK %s\n", task_name(m_task)));
	m_dsp.curt_blocks = 1;
}

/**
 * @brief f2_load_xpreg late: load the x position register from BUS[6-15]
 */
void alto2_cpu_device::f2_load_xpreg_1()
{
	m_dsp.xpreg = A2_GET32(m_bus,16,6,15);
	LOG((LOG_CURT,2,"	XPREG← BUS[6-15] (%#o)\n", m_dsp.xpreg));
}

/**
 * @brief f2_load_csr late: load the cursor shift register from BUS[0-15]
 *
 * Shift CSR to xpreg % 16 position to make it easier to
 * to handle the word xor in unload_word().
 * <PRE>
 * xpreg % 16   cursor bits
 *              [ first word   ][  second word ]
 * ----------------------------------------------
 *     0        xxxxxxxxxxxxxxxx0000000000000000
 *     1        0xxxxxxxxxxxxxxxx000000000000000
 *     2        00xxxxxxxxxxxxxxxx00000000000000
 * ...
 *    14        00000000000000xxxxxxxxxxxxxxxx00
 *    15        000000000000000xxxxxxxxxxxxxxxx0
 * </PRE>
 */
void alto2_cpu_device::f2_load_csr_1()
{
	m_dsp.csr = m_bus;
	LOG((LOG_CURT,2,"	CSR← BUS (%#o)\n", m_dsp.csr));
	int x = 1023 - m_dsp.xpreg; \
	m_dsp.curdata = m_dsp.csr << (16 - (x & 15)); \
	m_dsp.curword = x / 16; \
}

/**
 * @brief curt_activate: called by the CPU when the cursor task becomes active
 */
void alto2_cpu_device::activate_curt()
{
	m_task_wakeup &= ~(1 << m_task);
	m_dsp.curt_wakeup = 0;
}

/** @brief initialize the cursor task F1 and F2 functions */
void alto2_cpu_device::init_curt(int task)
{
	set_f1(task, f1_block,				&alto2_cpu_device::f1_curt_block_0, 0);
	set_f2(task, f2_curt_load_xpreg,	0, &alto2_cpu_device::f2_load_xpreg_1);
	set_f2(task, f2_curt_load_csr,		0, &alto2_cpu_device::f2_load_csr_1);
	m_active_callback[task] = &alto2_cpu_device::activate_curt;
}

void alto2_cpu_device::exit_curt()
{
	// nothing to do yet
}

