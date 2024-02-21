/****************************************************************************
 * apps/examples/hello/hello_main.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include "xmc4_scu.h"
#include "xmc4_ccu4.h"
#include <nuttx/irq.h>
#include <debug.h>
/****************************************************************************
 * Public Functions
 ****************************************************************************/

#define putreg32(v, a) (*(volatile uint32_t *)(a) = (v))
#define getreg32(a) (*(volatile uint32_t *)(a))
/****************************************************************************
 * hello_main
 ****************************************************************************/
volatile bool pending = false;
static void xmc4_interval_handler(void)
{
  // xmc4_clrpend(XMC4_IRQ_CCU41_SR0);
  // putreg32(CCU4_CC4_INTS_CMUS_MASK, XMC4_CCU41_CC40SWR);
  // uint32_t rc = getreg32(XMC4_CCU41_CC40CR);
  // printf("interval  :%d\n", rc);
  tmrinfo("interval_handler\n");
  pending = false;
  // nxsched_timer_expiration();
}
void test_init()
{
  putreg32(0xFFFF, XMC4_CCU41_CC41TIMER);

  putreg32(SCU_PR0_CCU41RS, XMC4_SCU_PRSET0);
  putreg32(SCU_PR0_CCU41RS, XMC4_SCU_PRCLR0);

  /* Set clock source of the timer and enable free-run mode */
  putreg32(SCU_CLK_CCUC, XMC4_SCU_CLKSET);
  putreg32(CCU4_GIDLC_CS0I_MASK, XMC4_CCU41_GIDLC);
  putreg32(CCU4_GIDLC_CS1I_MASK, XMC4_CCU41_GIDLC);

  /* Enable prescaler*/
  putreg32(CCU4_GIDLC_SPRB_MASK, XMC4_CCU41_GIDLC);

  putreg32(CCU4_CC4_TCCLR_TRBC_MASK, XMC4_CCU41_CC40TCCLR);
  putreg32(CCU4_CC4_TCCLR_TRBC_MASK, XMC4_CCU41_CC41TCCLR);
  putreg32(CCU4_CC4_CMC_TCE_MASK, XMC4_CCU41_CC41CMC);

  up_enable_irq(XMC4_IRQ_CCU41_SR0);
  irq_attach(XMC4_IRQ_CCU41_SR0, (xcpt_t)xmc4_interval_handler, NULL);

  /* Enable Single shot mode*/
  // putreg32(CCU4_CC4_TC_TSSM_MASK, XMC4_CCU41_CC40TC);
  putreg32(CCU4_CC4_TC_TSSM_MASK, XMC4_CCU41_CC41TC);

}

void test_sleep()
{
  irqstate_t flags;
  flags = enter_critical_section();
  /* Set compare value for output compare channel */
  //  tmrinfo("up_alarm_start : sec:%ld nsec :%ld\n", ts->tv_sec, ts->tv_nsec);
  putreg32(15, XMC4_CCU41_CC40PSC);
  putreg32(1000, XMC4_CCU41_CC40PRS);
  putreg32(15, XMC4_CCU41_CC41PSC);
  putreg32(60000, XMC4_CCU41_CC41PRS);
  putreg32(2, XMC4_CCU41_CC41PRS);

  /* Enable interrupts */
  // putreg32(CCU4_CC4_INTE_CMUE_MASK, XMC4_CCU41_CC40INTE);
  putreg32(CCU4_CC4_INTE_PME_MASK, XMC4_CCU41_CC41INTE);

  // Enable shadow transfer
  putreg32(CCU4_GCSS_S0SE_MASK, XMC4_CCU41_GCSS);
  putreg32(CCU4_GCSS_S1SE_MASK, XMC4_CCU41_GCSS);

  putreg32(CCU4_CC4_TCSET_TRBS_MASK, XMC4_CCU41_CC40TCSET);
  putreg32(CCU4_CC4_TCSET_TRBS_MASK, XMC4_CCU41_CC41TCSET);

  leave_critical_section(flags);
  static uint32_t cc41 = 0;
  cc41 = 0;
  pending = true;
  while (pending)
  {
    uint32_t tmp = getreg32(XMC4_CCU41_CC41TIMER);
    if (tmp != cc41)
    {
      cc41 = tmp;
      tmrinfo("%d\n", cc41);
      //  break;
    }
  }
}

int main(int argc, FAR char *argv[])
{
  tmrinfo("Helllllo, World!!\n");
  // while (true)
  //{
  //   irqstate_t flags = enter_critical_section();
  //   clock_systime_ticks();
  //   //leave_critical_section(NULL);
  // }

  test_init();
  test_sleep();
  test_sleep();
  // usleep(300000);
  tmrinfo("Helllllo, World2!!\n");
  //   while(1){}
  // printf("Hello, Wor2!\n\n\n");
  //  sleep(1);

  // uint8_t *error = NULL;
  // *error = 4;

  return 0;
}
