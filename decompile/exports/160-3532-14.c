
/* WARNING: Unable to track spacebase fully for stack */
/* scale_and_plot_point_default (confidence: Confirmed mechanism; what the reciprocal represents
   physically not confirmed)
   
   Evidence: Pre-loads `dx:ax` from a global default reciprocal constant at `[0x6D2]`, then falls
   through into `scale_and_plot_point` below */

void scale_and_plot_point_default(void)

{
  char *pcVar1;
  undefined2 uVar2;
  undefined2 in_AX;
  undefined2 uVar3;
  undefined2 in_DX;
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined4 uVar4;
  int iVar5;
  undefined1 uVar6;
  
  pcVar1 = (char *)(unaff_BP + unaff_SI + -0x19);
  *pcVar1 = *pcVar1 + (char)((uint)in_DX >> 8);
  *(undefined2 *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP) = in_AX;
  *(undefined2 *)((char *)s_ACQ_RAM_even_000f_fef8 + 0xc + unaff_BP) = in_DX;
  iVar5 = *(int *)0x6d2;
  uVar6 = (undefined1)(iVar5 >> 0xf);
  uVar4 = func_0x000e777d();
  *(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 6 + unaff_BP) = (int)uVar4;
  *(undefined2 *)(unaff_BP + -0x10) = (int)((ulong)uVar4 >> 0x10);
  uVar4 = func_0x000e7764(0xe772,6,0,iVar5,uVar6);
  uVar3 = (undefined2)uVar4;
  *(undefined2 *)((char *)s_ACQ_RAM_even_000f_fef8 + 6 + unaff_BP) = uVar3;
  *(undefined2 *)(unaff_BP + -0x10) = (int)((ulong)uVar4 >> 0x10);
  uVar2 = *(undefined2 *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP);
  *(undefined2 *)(unaff_BP + -8) = uVar2;
  *(undefined2 *)((char *)s_ACQ_RAM_odd_000f_ff05 + 1 + unaff_BP) = uVar3;
  func_0x000e7f39(0xe772,*(undefined2 *)0x6e6,*(undefined2 *)0x6e8,uVar2,uVar3,1);
  *(undefined2 *)0x6e6 = *(undefined2 *)(unaff_BP + -8);
  *(undefined2 *)0x6e8 = *(undefined2 *)((char *)s_ACQ_RAM_odd_000f_ff05 + 1 + unaff_BP);
  return;
}



/* WARNING: Unknown calling convention -- yet parameter storage is locked */
/* scale_and_plot_point (confidence: Confirmed mechanism; the physical quantity being scaled
   (timebase? voltage?) not confirmed)
   
   Evidence: `(raw_value dx:ax, reciprocal [bp+8])` - fixed-point scale via `mul32` + `ashr32`
   (multiply by a precomputed reciprocal, then shift right - **not a real divide**, an earlier
   session misread `mul32` as a divide), then calls `plot_scaled_point` to draw the scaled value and
   cache it in `[0x6E6]`/`[0x6E8]` for the next line segment - the per-sample step of an
   acquisition-to-plotter rendering pipeline */

void scale_and_plot_point(word reciprocal)

{
  undefined2 uVar1;
  undefined2 uVar2;
  int unaff_BP;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined4 uVar3;
  
  uVar3 = func_0x000e777d();
  *(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 6 + unaff_BP) = (int)uVar3;
  *(undefined2 *)(unaff_BP + -0x10) = (int)((ulong)uVar3 >> 0x10);
  uVar3 = func_0x000e7764(0xe772,6,0);
  uVar2 = (undefined2)uVar3;
  *(undefined2 *)((char *)s_ACQ_RAM_even_000f_fef8 + 6 + unaff_BP) = uVar2;
  *(undefined2 *)(unaff_BP + -0x10) = (int)((ulong)uVar3 >> 0x10);
  uVar1 = *(undefined2 *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP);
  *(undefined2 *)(unaff_BP + -8) = uVar1;
  *(undefined2 *)((char *)s_ACQ_RAM_odd_000f_ff05 + 1 + unaff_BP) = uVar2;
  func_0x000e7f39(0xe772,*(undefined2 *)0x6e6,*(undefined2 *)0x6e8,uVar1,uVar2,1);
  *(undefined2 *)0x6e6 = *(undefined2 *)(unaff_BP + -8);
  *(undefined2 *)0x6e8 = *(undefined2 *)((char *)s_ACQ_RAM_odd_000f_ff05 + 1 + unaff_BP);
  return;
}



void __stdcall16far FUN_000f_00e4(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 unaff_DS;
  int iVar4;
  
  iVar4 = *(int *)0x6d0;
  iVar3 = iVar4 >> 0xf;
  func_0x000e777d();
  uVar1 = func_0x000e7764(0xe772,0xc,0,iVar4,iVar3);
  func_0x000e777d(0xe772,*(int *)0x6d2,*(int *)0x6d2 >> 0xf);
  uVar2 = func_0x000e77ae(0xe772,0x40,0);
  func_0x000e7f39(0xe772,*(undefined2 *)0x6e6,*(undefined2 *)0x6e8,uVar1,uVar2,0);
  *(undefined2 *)0x6e6 = uVar1;
  *(undefined2 *)0x6e8 = uVar2;
  return;
}



void __stdcall16far FUN_000f_02fd(byte param_1)

{
  if (((param_1 & 0x7f) < 0x1a) || (0x1f < (param_1 & 0x7f))) {
    if ((param_1 & 0x7f) == 10) {
      func_0x00083259();
    }
    else {
      func_0x00083259();
    }
  }
  else {
    func_0x00083259();
  }
  return;
}



/* reset_acq_buffers_stub (confidence: Mechanism confirmed; the "acquisition buffer" interpretation
   is a plausible but unconfirmed inference from the buffer count/size)
   
   Evidence: A tiny stub that reaches a shared tail block initializing **8 separate buffer-size
   variables to `0x800` (2048 bytes) each**, plus a handful of other fixed setup values - see
   `docs/self-test/hardware-probes.md` "Possible waveform acquisition buffer init" */

void reset_acq_buffers_stub(void)

{
  char in_AH;
  int in_BX;
  int unaff_SI;
  undefined2 unaff_DS;
  
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + in_AH;
  *(undefined2 *)0x6f8 = 0x800;
  *(undefined2 *)0x6f6 = 0x800;
  *(undefined2 *)0x6f4 = 0x800;
  *(undefined2 *)0x6f2 = 0x800;
  *(undefined1 *)*(undefined4 *)0x6fa = 0xd5;
  *(undefined2 *)0x700 = 0;
  *(undefined2 *)0x702 = 0;
  *(undefined2 *)0x704 = 0x14;
  *(undefined2 *)0x6e8 = 0x800;
  *(undefined2 *)0x6e6 = 0x800;
  *(undefined2 *)0x6b4 = 0x800;
  *(undefined2 *)0x6b2 = 0x800;
  *(undefined2 *)0x70e = 0;
  func_0x000e7d7d();
  return;
}



/* print_and_reset_acq_buffers (confidence: Same confidence caveat as `reset_acq_buffers_stub`)
   
   Evidence: Prints a status string (`0xFF7B:0x362`) and does a scale computation, then falls
   through into the same shared buffer-init tail as `reset_acq_buffers_stub` */

void print_and_reset_acq_buffers(void)

{
  undefined2 unaff_DS;
  undefined2 uStack0002;
  
  uStack0002 = 0xb;
  func_0x000e7911();
  *(uint *)0x6d6 = *(int *)0x6d4 * (int)(0x1fff / (long)*(int *)0x6cc) & 0xfff0;
  *(undefined2 *)0x6f8 = 0x800;
  *(undefined2 *)0x6f6 = 0x800;
  *(undefined2 *)0x6f4 = 0x800;
  *(undefined2 *)0x6f2 = 0x800;
  *(undefined1 *)*(undefined4 *)0x6fa = 0xd5;
  *(undefined2 *)0x700 = 0;
  *(undefined2 *)0x702 = 0;
  *(undefined2 *)0x704 = 0x14;
  *(undefined2 *)0x6e8 = 0x800;
  *(undefined2 *)0x6e6 = 0x800;
  *(undefined2 *)0x6b4 = 0x800;
  *(undefined2 *)0x6b2 = 0x800;
  *(undefined2 *)0x70e = 0;
  func_0x000e7d7d(0xe791,0x200,0x200);
  return;
}



void __stdcall16far FUN_000f_06db(int param_1,uint param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined4 uStack_16;
  byte bStack_12;
  undefined4 uStack_10;
  uint *puStack_c;
  
  iVar1 = *(int *)0x6ca;
  if (iVar1 == 0) {
    uVar3 = *(uint *)&DAT_000f_1b12 & 0xfffe;
    _puStack_c = (uint *)CONCAT22(unaff_DS,(uint *)(uVar3 + 0x6f2));
    uVar2 = param_3 * 0x101;
    *(uint *)(uVar3 + 0x6f4) = uVar2;
    *_puStack_c = uVar2;
    *(uint *)(uVar3 + 0x6f8) = uVar2 | 0xc;
    *(uint *)(uVar3 + 0x6f6) = uVar2 | 0xc;
    uStack_10 = (undefined1 *)
                CONCAT22((int)((ulong)*(undefined4 *)&DAT_000f_1cec >> 0x10),
                         (undefined1 *)((int)*(undefined4 *)&DAT_000f_1cec + 0x2c7));
    *uStack_10 = 0xf5;
    if (*(uint *)0x700 == uVar2) {
      *(undefined2 *)0x704 = 1;
    }
    else {
      *(undefined2 *)0x792 = 0x78;
      unaff_CS = 0xe60b;
      func_0x000e6224();
      *(undefined2 *)0x704 = 0x78;
    }
    *(undefined2 *)0x702 = *(undefined2 *)0x700;
    *(uint *)0x700 = uVar2;
    *(int *)0x6f6 = param_2 << 4;
    *(int *)0x6f2 = param_2 << 4;
    *(int *)0x6f8 = param_1 << 4;
    *(int *)0x6f4 = param_1 << 4;
    *(int *)0x6e6 = param_1;
    *(uint *)0x6e8 = param_2;
    *(undefined2 *)0x792 = *(undefined2 *)0x704;
    func_0x000e6224(unaff_CS);
  }
  else if (iVar1 != 1) {
    if (iVar1 == 2) {
      *(undefined2 *)0x6d8 = 1;
      if (6 < (int)param_2) {
        param_2 = param_2 - 7;
        param_1 = param_1 + 0x400;
      }
      uStack_16 = (byte *)CONCAT22((int)((ulong)*(undefined4 *)0x6ea >> 0x10),
                                   (byte *)((int)*(undefined4 *)0x6ea + param_1 * 2));
      *uStack_16 = *uStack_16 | (byte)(1 << ((byte)param_2 & 7));
    }
    else {
      if (iVar1 != 3) {
        if (iVar1 == 4) {
          *(undefined2 *)0x6d8 = 1;
          uStack_16 = (byte *)CONCAT22((int)((ulong)*(undefined4 *)0x6ea >> 0x10),
                                       (byte *)((int)*(undefined4 *)0x6ea +
                                               (param_1 / 8 +
                                               (0xf - param_2) * (*(int *)0x6ce / 8 + 1)) * 2));
          bStack_12 = (byte)(0x80 >> ((byte)param_1 & 7));
          *uStack_16 = *uStack_16 | bStack_12;
          return;
        }
        if (iVar1 != 8) {
          return;
        }
      }
      *(undefined2 *)0x6d8 = 1;
      if ((param_2 & 0xf8) != 0) {
        param_1 = param_1 + 0x400;
      }
      uStack_16 = (byte *)CONCAT22((int)((ulong)*(undefined4 *)0x6ea >> 0x10),
                                   (byte *)((int)*(undefined4 *)0x6ea + param_1 * 2));
      bStack_12 = (byte)(1 << ((byte)param_2 & 7));
      *uStack_16 = *uStack_16 | bStack_12;
    }
  }
  return;
}



void __cdecl16far FUN_000f_0ca1(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  uVar2 = 0xe7f3;
  func_0x000e7fa6();
  iVar1 = *(int *)0x6ca;
  if (iVar1 == 0) {
    uVar2 = 0xe90a;
    func_0x000e90a5(0xe7f3,*(undefined2 *)0x70a,*(undefined2 *)0x70c,2000,2000,0);
  }
  else if (iVar1 == 1) {
    uVar2 = 0xe791;
    func_0x000e7911(0xe7f3,800,(char *)s_reformated_000f_fe8a + 1);
  }
  else if (iVar1 == 2) {
    func_0x000e7911(0xe7f3,0x326,(char *)s_reformated_000f_fe8a + 1,0xc);
    func_0x000e8e03(0xe791,2);
    uVar2 = 0xe791;
    func_0x000e7911(0xe8ca,0x329,(char *)s_reformated_000f_fe8a + 1,0x1b,0x40);
  }
  else if (iVar1 == 3) {
    func_0x000e7911(0xe7f3,0x336,(char *)s_reformated_000f_fe8a + 1,0xc);
    func_0x000e8e03(0xe791,2);
    uVar2 = 0xe791;
    func_0x000e7911(0xe8ca,0x339,(char *)s_reformated_000f_fe8a + 1,0x1b,0x40);
  }
  else if (iVar1 == 4) {
    func_0x000e7911(0xe7f3,0x33e,(char *)s_reformated_000f_fe8a + 1,0x1b);
    func_0x000e7911(0xe791,0x344,(char *)s_reformated_000f_fe8a + 1,0xc);
    uVar2 = 0x82c9;
    func_0x00083259(0xe791,10);
  }
  else if (iVar1 == 8) {
    func_0x000e7911(0xe7f3,0x32e,(char *)s_reformated_000f_fe8a + 1,0xc);
    func_0x000e8e03(0xe791,2);
    uVar2 = 0xe791;
    func_0x000e7911(0xe8ca,0x331,(char *)s_reformated_000f_fe8a + 1,0x1b,0x40);
  }
  *(undefined2 *)0x6ca = 0;
  *(undefined2 *)0x792 = 100;
  *(undefined1 *)0x78d = 1;
  func_0x000e6224(uVar2);
  return;
}



void __stdcall16far FUN_000f_0e03(void)

{
  undefined2 unaff_CS;
  undefined2 uStack_a;
  
  for (uStack_a = 0; uStack_a < 0x46; uStack_a = uStack_a + 1) {
    func_0x00083259(unaff_CS,0);
    unaff_CS = 0x82c9;
  }
  return;
}



/* WARNING: Instruction at (ram,0x000010f2) overlaps instruction at (ram,0x000010f1)
    */
/* convert_sample_value (confidence: Mechanism confirmed; exact conversion (voltage? time?) not
   confirmed)
   
   Evidence: Starts with a real x87 `fmul` (entered via fallthrough, no own prologue), then calls
   `mul32` and `sdiv32` - a mixed float/integer value conversion, likely
   ADC-count-to-engineering-unit scaling; possible evidence of a real 8087 coprocessor. **Loops on
   itself**: its own tail (`L_F1047`'s predecessor) `jmp`s back ~0x500 bytes to `L_F0E50` (a `push
   [bp+0x10]` stub that falls straight back into its own `fmul` entry) when `[bp-0xE]==0` - a manual
   iterate-without-recursing pattern, not a call to a different function, found via
   `disasm/analyze_loops_vs_functions.py` */

void convert_sample_value(void)

{
  bool bVar1;
  undefined2 uVar2;
  uint uVar3;
  int iVar4;
  int unaff_BP;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  
  do {
    func_0x000e777d(unaff_CS);
    unaff_CS = 0xe772;
    iVar4 = func_0x000e77ae(0xe772);
    *(int *)(unaff_BP + 8) = *(int *)(unaff_BP + 8) + iVar4;
    *(undefined2 *)(unaff_BP + 6) = 0;
    do {
      while( true ) {
        while( true ) {
          while( true ) {
            while( true ) {
              while( true ) {
                if (*(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 0xc + unaff_BP) == 1) {
                  func_0x000e80e4();
                  unaff_CS = 0xe804;
                  func_0x000e804f();
                }
                if (*(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP) != 0) {
                  return;
                }
                uVar2 = func_0x000e904d(unaff_CS);
                *(undefined2 *)(unaff_BP + -8) = uVar2;
                unaff_CS = 0xe8e2;
                uVar3 = func_0x000e904d(0xe8e2,*(undefined2 *)(unaff_BP + 10));
                *(uint *)((char *)s_ACQ_RAM_odd_000f_ff05 + 1 + unaff_BP) = uVar3;
                if ((*(uint *)(unaff_BP + -8) & uVar3) == 0) break;
                ((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP)[0] = '\x01';
                ((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP)[1] = '\0';
              }
              if ((*(int *)(unaff_BP + -8) == 0) &&
                 (*(int *)((char *)s_ACQ_RAM_odd_000f_ff05 + 1 + unaff_BP) == 0)) {
                bVar1 = false;
              }
              else {
                bVar1 = true;
              }
              if (bVar1) break;
              ((char *)s_ACQ_RAM_even_000f_fef8 + 0xc + unaff_BP)[0] = '\x01';
              ((char *)s_ACQ_RAM_even_000f_fef8 + 0xc + unaff_BP)[1] = '\0';
              ((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP)[0] = '\x01';
              ((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP)[1] = '\0';
            }
            ((char *)s_ACQ_RAM_even_000f_fef8 + 0xc + unaff_BP)[0] = '\0';
            ((char *)s_ACQ_RAM_even_000f_fef8 + 0xc + unaff_BP)[1] = '\0';
            if (*(int *)(unaff_BP + -8) == 0) {
              uVar2 = *(undefined2 *)(unaff_BP + 6);
              *(undefined2 *)((char *)s_ACQ_RAM_even_000f_fef8 + 6 + unaff_BP) = uVar2;
              *(undefined2 *)(unaff_BP + 6) = *(undefined2 *)(unaff_BP + 10);
              *(undefined2 *)(unaff_BP + 10) = uVar2;
              uVar2 = *(undefined2 *)(unaff_BP + 8);
              *(undefined2 *)((char *)s_ACQ_RAM_even_000f_fef8 + 6 + unaff_BP) = uVar2;
              *(undefined2 *)(unaff_BP + 8) = *(undefined2 *)(unaff_BP + 0xc);
              *(undefined2 *)(unaff_BP + 0xc) = uVar2;
              uVar2 = *(undefined2 *)(unaff_BP + -8);
              *(undefined2 *)((char *)s_ACQ_RAM_even_000f_fef8 + 4 + unaff_BP) = uVar2;
              *(undefined2 *)(unaff_BP + -8) =
                   *(undefined2 *)((char *)s_ACQ_RAM_odd_000f_ff05 + 1 + unaff_BP);
              *(undefined2 *)((char *)s_ACQ_RAM_odd_000f_ff05 + 1 + unaff_BP) = uVar2;
            }
            iVar4 = *(int *)(unaff_BP + 10) - *(int *)(unaff_BP + 6);
            *(int *)((char *)s_ACQ_RAM_even_000f_fef8 + unaff_BP) = iVar4;
            *(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 2 + unaff_BP) = iVar4 >> 0xf;
            iVar4 = *(int *)(unaff_BP + 0xc) - *(int *)(unaff_BP + 8);
            *(int *)((undefined *)&DAT_000f_fef4 + unaff_BP) = iVar4;
            *(int *)((undefined *)&DAT_000f_fef6 + unaff_BP) = iVar4 >> 0xf;
            iVar4 = 0xffe - *(int *)(unaff_BP + 6);
            *(int *)(unaff_BP + -0x20) = iVar4;
            *(int *)((undefined *)&DAT_000f_fef2 + unaff_BP) = iVar4 >> 0xf;
            iVar4 = *(int *)(unaff_BP + 6);
            *(int *)((char *)s_SYS_RAM_000f_fee8 + 4 + unaff_BP) = -iVar4;
            *(int *)((char *)s_SYS_RAM_000f_fee8 + 6 + unaff_BP) = -iVar4 >> 0xf;
            iVar4 = (*(int *)(unaff_BP + 0x10) + -1) - *(int *)(unaff_BP + 8);
            *(int *)((char *)s_SYS_RAM_000f_fee8 + unaff_BP) = iVar4;
            *(int *)((char *)s_SYS_RAM_000f_fee8 + 2 + unaff_BP) = iVar4 >> 0xf;
            iVar4 = *(int *)(unaff_BP + 0xe) - *(int *)(unaff_BP + 8);
            *(int *)((char *)s_OMM_RB_000f_fee0 + 4 + unaff_BP) = iVar4;
            *(int *)((char *)s_OMM_RB_000f_fee0 + 6 + unaff_BP) = iVar4 >> 0xf;
            if ((*(uint *)(unaff_BP + -8) & 1) == 0) break;
            func_0x000e777d(0xe8e2);
            unaff_CS = 0xe772;
            iVar4 = func_0x000e77ae(0xe772);
            *(int *)(unaff_BP + 6) = *(int *)(unaff_BP + 6) + iVar4;
            *(int *)(unaff_BP + 8) = *(int *)(unaff_BP + 0x10) + -1;
          }
          if ((*(uint *)(unaff_BP + -8) & 2) == 0) break;
          func_0x000e777d(0xe8e2);
          unaff_CS = 0xe772;
          iVar4 = func_0x000e77ae(0xe772);
          *(int *)(unaff_BP + 6) = *(int *)(unaff_BP + 6) + iVar4;
          *(undefined2 *)(unaff_BP + 8) = *(undefined2 *)(unaff_BP + 0xe);
        }
        if ((*(uint *)(unaff_BP + -8) & 4) == 0) break;
        func_0x000e777d(0xe8e2);
        unaff_CS = 0xe772;
        iVar4 = func_0x000e77ae(0xe772);
        *(int *)(unaff_BP + 8) = *(int *)(unaff_BP + 8) + iVar4;
        *(undefined2 *)(unaff_BP + 6) = 0xffe;
      }
    } while ((*(uint *)(unaff_BP + -8) & 8) == 0);
  } while( true );
}



byte __stdcall16far FUN_000f_104d(int param_1,int param_2,int param_3,int param_4)

{
  byte bVar1;
  
  bVar1 = param_4 < param_2;
  if (param_2 < param_3) {
    bVar1 = bVar1 | 2;
  }
  if (0xfff < param_1) {
    bVar1 = bVar1 | 4;
  }
  if (param_1 < 0) {
    bVar1 = bVar1 | 8;
  }
  return bVar1;
}



void __stdcall16far
FUN_000f_10a5(int param_1,int param_2,int param_3,int param_4,undefined2 param_5)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 unaff_CS;
  undefined2 uVar6;
  undefined2 unaff_DS;
  int iStack_16;
  int iStack_14;
  int iStack_12;
  
  iStack_16 = 1;
  if ((*(int *)0x6ca == 0) || (*(int *)0x6ca == 1)) {
    iStack_16 = (*(int *)0x3e0 >> 6 & 0xfU) + 1;
  }
  iStack_14 = 0;
  iVar3 = param_3 - param_1;
  if (iVar3 < 0) {
    iVar4 = -1;
  }
  else {
    iVar4 = 1;
  }
  iVar1 = param_4 - param_2;
  if (iVar1 < 0) {
    iVar2 = -1;
  }
  else {
    iVar2 = 1;
  }
  if (iVar3 < 0) {
    iVar3 = -iVar3;
  }
  if (iVar1 < 0) {
    iVar1 = -iVar1;
  }
  if (iVar3 < iVar1) {
    iStack_12 = iVar3 * 2 - iVar1;
    iVar1 = iStack_12 - iVar1;
    for (; param_4 != param_2; param_2 = param_2 + iVar2) {
      iStack_14 = (iStack_14 + 1) % iStack_16;
      uVar6 = unaff_CS;
      if (iStack_14 == 0) {
        uVar6 = 0xe86d;
        func_0x000e86db(unaff_CS,param_1,param_2,param_5);
      }
      iVar5 = iVar3 * 2;
      if (-1 < iStack_12) {
        param_1 = param_1 + iVar4;
        iVar5 = iVar1;
      }
      iStack_12 = iStack_12 + iVar5;
      unaff_CS = uVar6;
    }
  }
  else {
    iStack_12 = iVar1 * 2 - iVar3;
    iVar3 = iStack_12 - iVar3;
    for (; param_3 != param_1; param_1 = param_1 + iVar4) {
      iStack_14 = (iStack_14 + 1) % iStack_16;
      uVar6 = unaff_CS;
      if (iStack_14 == 0) {
        uVar6 = 0xe86d;
        func_0x000e86db(unaff_CS,param_1,param_2,param_5);
      }
      iVar5 = iVar1 * 2;
      if (-1 < iStack_12) {
        param_2 = param_2 + iVar2;
        iVar5 = iVar3;
      }
      iStack_12 = iStack_12 + iVar5;
      unaff_CS = uVar6;
    }
  }
  func_0x000e86db(unaff_CS,param_1,param_2,param_5);
  return;
}



void __cdecl16far FUN_000f_121b(void)

{
  char cVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  cVar1 = *(char *)&DAT_000f_1b08;
  *(char *)0x6b6 = cVar1;
  if ((cVar1 != '\0') || (*(char *)0x400 == '\x01')) {
    *(undefined1 *)0x706 = 0;
    func_0x000e92b0();
    *(undefined *)&DAT_000f_1b07 = 1;
    uVar2 = 0xe925;
    while (*(char *)&DAT_000f_1b07 != '\0') {
      func_0x000e693c(uVar2);
      uVar2 = 0xe693;
    }
  }
  return;
}



void __cdecl16far FUN_000f_12b0(void)

{
  func_0x000e9255();
  return;
}



void __cdecl16far FUN_000f_12bf(void)

{
  char cVar1;
  bool bVar2;
  int iVar3;
  undefined2 unaff_CS;
  undefined2 uVar4;
  undefined2 unaff_DS;
  bool bVar5;
  int iStack_12;
  int iStack_10;
  undefined2 uStack_c;
  
  *(undefined2 *)0x6ea = 0x8000;
  *(undefined2 *)0x6ec = 0x4000;
  cVar1 = *(char *)0x403;
  if (*(char *)&DAT_000f_1b09 == '\0') {
    iStack_10 = 0;
    iStack_12 = 0;
  }
  else {
    iStack_10 = func_0x0008392b();
    unaff_CS = 0x82c9;
    iStack_12 = func_0x0008397e(0x82c9);
  }
  if (cVar1 == '\0') {
    bVar5 = true;
    goto LAB_000f_13d4;
  }
  if ((cVar1 == '\x01') || (cVar1 == '\x02')) {
LAB_000f_1309:
    if (*(char *)&DAT_000f_1b09 != '\0') {
      if ((iStack_10 == 0) || (*(char *)0x6b6 == '\0')) {
        bVar5 = true;
      }
      else {
        bVar5 = false;
      }
      if ((bVar5) && (iStack_12 == 0)) {
        bVar5 = true;
        goto LAB_000f_13d4;
      }
    }
    bVar5 = false;
  }
  else {
    if ((cVar1 != '\x03') && (cVar1 != '\x04')) {
      if (cVar1 == '\x05') goto LAB_000f_1309;
      if (cVar1 != '\b') {
        bVar5 = false;
        goto LAB_000f_13d4;
      }
    }
    if ((*(char *)&DAT_000f_1b09 == '\0') ||
       (((iStack_10 != 0 && (*(char *)0x6b6 != '\0')) || (iStack_12 != 0)))) {
      bVar2 = false;
    }
    else {
      bVar2 = true;
    }
    bVar5 = false;
    if (bVar2) {
      iVar3 = func_0x000838e3(unaff_CS);
      bVar5 = iVar3 != 0;
      unaff_CS = 0x82c9;
    }
  }
LAB_000f_13d4:
  *(undefined1 *)0x6b6 = 0;
  if (bVar5) {
    if (*(int *)0x6ca != 5) {
      func_0x000e8363(unaff_CS,cVar1);
      *(undefined2 *)0x710 = 0xfff;
      uVar4 = 0xe7c2;
      func_0x000e7c27(0xe836,0);
      uStack_c = 1;
      *(int *)0x6c4 = *(int *)0x710 - *(int *)0x6d6;
      while (0 < *(int *)0x6c4 + *(int *)0x6d6) {
        func_0x000e7ebf(uVar4);
        func_0x000e7aa7(0xe7eb,uStack_c);
        uVar4 = 0xe817;
        func_0x000e8179(0xe7aa);
        uStack_c = 0;
        *(int *)0x6c4 = *(int *)0x6c4 - *(int *)0x6d6;
      }
      unaff_CS = 0xe8ca;
      func_0x000e8ca1(uVar4);
    }
    if (((*(int *)0x6ca != 0) && (*(char *)&DAT_000f_1b09 != '\0')) && (iStack_10 == 0)) {
      func_0x0009470e(unaff_CS,(undefined1 *)&LAB_000f_8e90);
      func_0x00096c5f(38000);
      func_0x0009470e(0x9687,0x41);
    }
  }
  return;
}



undefined2 __stdcall16far FUN_000f_14cc(int param_1)

{
  return (int)(1000000 / (long)param_1);
}



undefined2 __stdcall16far FUN_000f_14e6(int param_1)

{
  return (int)(10000000 / (long)param_1);
}



undefined2 __stdcall16far FUN_000f_1500(int param_1)

{
  return (int)(100000000 / (long)param_1);
}



void __stdcall16far FUN_000f_151a(void)

{
  int in_stack_00000010;
  
  if (in_stack_00000010 == 0) {
    func_0x000e95c8();
  }
  else if (in_stack_00000010 == 1) {
    func_0x000e9704();
  }
  else if (in_stack_00000010 == 2) {
    func_0x000e962c();
  }
  else if (in_stack_00000010 == 3) {
    func_0x000e9778();
  }
  else {
    func_0x000e95c8();
  }
  return;
}



void __stdcall16far FUN_000f_15c8(char *param_1,undefined4 param_2,undefined2 *param_3)

{
  char *pcVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  char *pcStack_e;
  
  *(undefined2 *)0x6aa = *(undefined2 *)0x678;
  *(undefined2 *)0x6ac = 0;
  *(undefined2 *)0x6ae = (int)param_2;
  *(undefined2 *)0x6b0 = (int)((ulong)param_2 >> 0x10);
  pcStack_e = param_1;
  uVar3 = 0;
  iVar2 = func_0x000ea13b();
  *(int *)0x6aa = *(int *)0x6aa + iVar2;
  while( true ) {
    pcVar1 = pcStack_e;
    pcStack_e = (char *)CONCAT22(pcStack_e._2_2_,(char *)pcStack_e + 1);
    if (*pcVar1 == '\0') break;
    iVar2 = func_0x000ea13b(0xea13,*pcVar1,uVar3);
    *(int *)0x6aa = *(int *)0x6aa + iVar2;
  }
  *param_3 = *(undefined2 *)0x6aa;
  return;
}



void __stdcall16far FUN_000f_162c(char *param_1,undefined4 param_2,undefined2 *param_3)

{
  char *pcVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  char *pcStack_e;
  
  *(undefined2 *)0x6aa = *(undefined2 *)0x678;
  *(undefined2 *)0x6ac = 0;
  *(undefined2 *)0x6ae = (int)param_2;
  *(undefined2 *)0x6b0 = (int)((ulong)param_2 >> 0x10);
  pcStack_e = param_1;
  uVar3 = 0;
  iVar2 = func_0x000ea13b();
  *(int *)0x6aa = *(int *)0x6aa + iVar2;
  while( true ) {
    pcVar1 = pcStack_e;
    pcStack_e = (char *)CONCAT22(pcStack_e._2_2_,(char *)pcStack_e + 1);
    if (*pcVar1 == '\0') break;
    iVar2 = func_0x000ea1c0(0xea13,*pcVar1,uVar3);
    *(int *)0x6aa = *(int *)0x6aa + iVar2;
  }
  *param_3 = *(undefined2 *)0x6aa;
  return;
}



void __stdcall16far FUN_000f_1690(char *param_1,undefined4 param_2,undefined2 *param_3)

{
  char *pcVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  char *pcStack_e;
  
  *(undefined2 *)0x6aa = *(undefined2 *)0x678;
  *(undefined2 *)0x6ac = 0;
  *(undefined2 *)0x6ae = (int)param_2;
  *(undefined2 *)0x6b0 = (int)((ulong)param_2 >> 0x10);
  pcStack_e = param_1;
  uVar3 = 0;
  iVar2 = func_0x000ea13b();
  *(int *)0x6aa = *(int *)0x6aa + iVar2;
  while( true ) {
    pcVar1 = pcStack_e;
    pcStack_e = (char *)CONCAT22(pcStack_e._2_2_,(char *)pcStack_e + 1);
    if (*pcVar1 == '\0') break;
    iVar2 = func_0x000ea1c0(0xea13,*pcVar1,uVar3);
    *(int *)0x6aa = *(int *)0x6aa + iVar2;
  }
  func_0x000ea2d6(0xea13,*(undefined2 *)0x678,*(undefined2 *)0x6aa);
  *param_3 = *(undefined2 *)0x6aa;
  return;
}



void __stdcall16far FUN_000f_1704(char *param_1,undefined4 param_2,undefined2 *param_3)

{
  char *pcVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  char *pcStack_e;
  
  *(undefined2 *)0x6aa = *(undefined2 *)0x678;
  *(undefined2 *)0x6ac = 0;
  *(undefined2 *)0x6ae = (int)param_2;
  *(undefined2 *)0x6b0 = (int)((ulong)param_2 >> 0x10);
  pcStack_e = param_1;
  uVar3 = 0;
  iVar2 = func_0x000ea13b();
  *(int *)0x6aa = *(int *)0x6aa + iVar2;
  while( true ) {
    pcVar1 = pcStack_e;
    pcStack_e = (char *)CONCAT22(pcStack_e._2_2_,(char *)pcStack_e + 1);
    if (*pcVar1 == '\0') break;
    iVar2 = func_0x000ea13b(0xea13,*pcVar1,uVar3);
    *(int *)0x6aa = *(int *)0x6aa + iVar2;
  }
  func_0x000ea2d6(0xea13,*(undefined2 *)0x678,*(undefined2 *)0x6aa);
  *param_3 = *(undefined2 *)0x6aa;
  return;
}



void __stdcall16far FUN_000f_1778(char *param_1,undefined4 param_2,undefined2 *param_3)

{
  char *pcVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  char *pcStack_e;
  
  *(undefined2 *)0x6aa = *(undefined2 *)0x678;
  *(undefined2 *)0x6ac = 0;
  *(undefined2 *)0x6ae = (int)param_2;
  *(undefined2 *)0x6b0 = (int)((ulong)param_2 >> 0x10);
  pcStack_e = param_1;
  uVar3 = 0;
  iVar2 = func_0x000ea13b();
  *(int *)0x6aa = *(int *)0x6aa + iVar2;
  while( true ) {
    pcVar1 = pcStack_e;
    pcStack_e = (char *)CONCAT22(pcStack_e._2_2_,(char *)pcStack_e + 1);
    if (*pcVar1 == '\0') break;
    iVar2 = func_0x000ea251(0xea13,*pcVar1,uVar3);
    *(int *)0x6aa = *(int *)0x6aa + iVar2;
  }
  *param_3 = *(undefined2 *)0x6aa;
  return;
}



void __stdcall16far FUN_000f_17dc(byte param_1,int param_2)

{
  undefined2 unaff_DS;
  
  if (((*(char *)&DAT_000f_1a88 == '\0') && (*(char *)&DAT_000f_1a8a == '\0')) &&
     (*(char *)0x686 == '\0')) {
    param_1 = param_1 | 0x82;
  }
  if (*(char *)&DAT_000f_1b06 == '\0') {
    *(byte *)((int)*(undefined4 *)&DAT_000f_1cec + 0x144) = param_1 & 0xfe | 0x80;
  }
  else {
    *(byte *)(param_2 * 9 + (int)*(undefined4 *)&DAT_000f_1cec) = param_1;
  }
  if (param_2 == 0x38) {
    *(byte *)((int)*(undefined4 *)&DAT_000f_1cec + 0x1f8) = param_1;
  }
  return;
}



void __stdcall16far
FUN_000f_1858(undefined4 param_1,int param_2,int param_3,byte param_4,byte param_5,int param_6)

{
  undefined1 uVar1;
  int iVar2;
  undefined1 uVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  
  iVar2 = (int)param_1 + (int)((ulong)param_1 >> 0x10) * 0x10;
  uVar5 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1cec >> 0x10);
  iVar4 = (int)*(undefined4 *)&DAT_000f_1cec + param_6 * 9;
  uVar1 = (undefined1)iVar2;
  *(undefined1 *)(iVar4 + 5) = uVar1;
  uVar3 = (undefined1)((uint)iVar2 >> 8);
  *(undefined1 *)(iVar4 + 6) = uVar3;
  *(undefined1 *)(iVar4 + 7) = uVar1;
  *(undefined1 *)(iVar4 + 8) = uVar3;
  *(byte *)(iVar4 + 1) = param_5 | (byte)(param_3 << 5);
  *(undefined1 *)(iVar4 + 2) = (char)(param_3 >> 3);
  *(byte *)(iVar4 + 3) = param_4 | (byte)(param_2 << 5);
  *(undefined1 *)(iVar4 + 4) = (char)(param_2 >> 3);
  func_0x000e994b();
  return;
}



void __stdcall16far FUN_000f_194b(int param_1,int param_2,int param_3)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  uVar2 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1cec >> 0x10);
  iVar1 = (int)*(undefined4 *)&DAT_000f_1cec + param_3 * 9;
  *(byte *)(iVar1 + 1) =
       *(byte *)(param_3 * 9 + (int)*(undefined4 *)&DAT_000f_1cec + 1) | (byte)(param_2 << 5);
  *(undefined1 *)(iVar1 + 2) = (char)(param_2 >> 3);
  *(byte *)(iVar1 + 3) =
       *(byte *)(param_3 * 9 + (int)*(undefined4 *)&DAT_000f_1cec + 3) | (byte)(param_1 << 5);
  *(undefined1 *)(iVar1 + 4) = (char)(param_1 >> 3);
  return;
}



void __stdcall16far FUN_000f_19df(void)

{
  func_0x000e951a();
  return;
}



void __stdcall16far FUN_000f_1a05(void)

{
  func_0x000e951a();
  return;
}



uint __stdcall16far FUN_000f_21c0(byte param_1)

{
  byte bVar1;
  int iVar2;
  byte *pbVar3;
  char cVar4;
  char cVar5;
  byte bVar6;
  uint in_DX;
  uint uVar7;
  char cVar8;
  byte *pbVar9;
  byte *pbVar10;
  byte *pbVar11;
  int unaff_DS;
  
  pbVar10 = (byte *)((int)*(undefined4 *)0x6ae +
                    ((int)((ulong)*(undefined4 *)0x6ae >> 0x10) - unaff_DS) * 0x10);
  pbVar3 = (byte *)*(undefined4 *)((uint)param_1 * 4 + (int)*(undefined4 *)&DAT_000f_1cc0);
  pbVar9 = (byte *)pbVar3;
  uVar7 = in_DX & 0xff;
  while( true ) {
    bVar1 = *pbVar9;
    pbVar9 = pbVar9 + 1;
    if (bVar1 == 0) break;
    *pbVar10 = bVar1 & 0xf;
    pbVar11 = pbVar10 + 1;
    uVar7 = (bVar1 & 0x70) >> 4;
    cVar4 = (char)((bVar1 & 0xf) - 7) >> 2;
    cVar8 = (char)uVar7;
    cVar5 = cVar4 + cVar8;
    if (SCARRY1(cVar4,cVar8) == cVar5 < '\0') {
      cVar8 = cVar5;
    }
    *pbVar11 = cVar8 + (char)*(undefined2 *)0x6aa;
    iVar2 = *(int *)&DAT_000f_1b12;
    pbVar11[iVar2 + 1] = 2;
    pbVar11[iVar2 + 2] = 2;
    bVar6 = 8;
    if ((bVar1 & 0x80) != 0) {
      bVar6 = 9;
    }
    pbVar11[iVar2] = bVar6;
    pbVar11[iVar2 + -1] = bVar6;
    *(int *)0x6ae = *(int *)0x6ae + 2;
    pbVar10 = pbVar10 + 2;
  }
  return uVar7;
}



uint __stdcall16far FUN_000f_2251(byte param_1)

{
  byte bVar1;
  int iVar2;
  byte *pbVar3;
  byte bVar4;
  uint in_DX;
  uint uVar5;
  byte *pbVar6;
  byte *pbVar7;
  byte *pbVar8;
  int unaff_DS;
  
  pbVar7 = (byte *)((int)*(undefined4 *)0x6ae +
                   ((int)((ulong)*(undefined4 *)0x6ae >> 0x10) - unaff_DS) * 0x10);
  pbVar3 = (byte *)*(undefined4 *)((uint)param_1 * 4 + (int)*(undefined4 *)&DAT_000f_1cc0);
  pbVar6 = (byte *)pbVar3;
  uVar5 = in_DX & 0xff;
  while( true ) {
    bVar1 = *pbVar6;
    pbVar6 = pbVar6 + 1;
    if (bVar1 == 0) break;
    *pbVar7 = bVar1 & 0xf;
    pbVar8 = pbVar7 + 1;
    uVar5 = (bVar1 & 0x70) >> 4;
    *pbVar8 = (char)uVar5 + (char)*(undefined2 *)0x6aa;
    iVar2 = *(int *)&DAT_000f_1b12;
    pbVar8[iVar2 + 1] = 2;
    pbVar8[iVar2 + 2] = 2;
    bVar4 = 8;
    if ((bVar1 & 0x80) != 0) {
      bVar4 = 0xb;
    }
    pbVar8[iVar2] = bVar4;
    pbVar8[iVar2 + -1] = bVar4;
    *(int *)0x6ae = *(int *)0x6ae + 2;
    pbVar7 = pbVar7 + 2;
  }
  return uVar5;
}



void __stdcall16far FUN_000f_22d6(byte param_1,byte param_2)

{
  int iVar1;
  undefined1 *puVar2;
  int unaff_DS;
  bool bVar3;
  
  puVar2 = (undefined1 *)
           ((int)*(undefined4 *)0x6ae +
           ((int)((ulong)*(undefined4 *)0x6ae >> 0x10) - unaff_DS) * 0x10);
  iVar1 = *(int *)&DAT_000f_1b12;
  *puVar2 = 4;
  puVar2[1] = param_1;
  puVar2 = puVar2 + 2;
  *(int *)0x6ae = *(int *)0x6ae + 2;
  puVar2[iVar1] = 2;
  puVar2[iVar1 + 1] = 2;
  puVar2[iVar1 + -1] = 8;
  puVar2[iVar1 + -2] = 8;
  do {
    if (param_2 <= param_1) {
      return;
    }
    *puVar2 = 4;
    puVar2[1] = param_1;
    puVar2 = puVar2 + 2;
    *(int *)0x6ae = *(int *)0x6ae + 2;
    puVar2[iVar1] = 2;
    puVar2[iVar1 + 1] = 2;
    puVar2[iVar1 + -1] = 9;
    puVar2[iVar1 + -2] = 9;
    bVar3 = param_1 < 0xfd;
    param_1 = param_1 + 3;
  } while (bVar3);
  return;
}



void __cdecl16far FUN_000f_2344(void)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  int iStack_a;
  
  if (*(char *)&DAT_000f_1b09 != '\0') {
    unaff_CS = 0xe951;
    func_0x000e9858();
  }
  for (iStack_a = 0; iStack_a < 0x15; iStack_a = iStack_a + 1) {
    if ((*(char *)&DAT_000f_1a93 == '\x14') &&
       (((iStack_a == 10 || (iStack_a == 0xf)) || (iStack_a == 0x12)))) {
      if (iStack_a == 0x12) {
        func_0x000e9858(unaff_CS,(int)*(undefined4 *)&DAT_000f_1bd4 +
                                 *(int *)((int)*(undefined4 *)&DAT_000f_1ca0 + 0x24),
                        (int)((ulong)*(undefined4 *)&DAT_000f_1bd4 >> 0x10),
                        *(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cac + 0x24),0x334,0x10,0x10,
                        0x36);
      }
      else {
        func_0x000e9858(unaff_CS,(int)*(undefined4 *)&DAT_000f_1bd4 +
                                 *(int *)((int)*(undefined4 *)&DAT_000f_1ca0 + iStack_a * 2),
                        (int)((ulong)*(undefined4 *)&DAT_000f_1bd4 >> 0x10),
                        *(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cac + iStack_a * 2),0x33e,
                        0x10,0x10,iStack_a + 0x24);
      }
    }
    else {
      iVar2 = iStack_a * 2;
      func_0x000e9858(unaff_CS,(int)*(undefined4 *)&DAT_000f_1bd4 +
                               *(int *)((int)*(undefined4 *)&DAT_000f_1ca0 + iVar2),
                      (int)((ulong)*(undefined4 *)&DAT_000f_1bd4 >> 0x10),
                      *(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cac + iVar2),
                      *(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cb0 + iVar2),0x10,0x10,
                      iStack_a + 0x24);
    }
    unaff_CS = 0xe951;
  }
  func_0x000e9858(unaff_CS,(int)*(undefined4 *)&DAT_000f_1bd4 +
                           *(int *)((int)*(undefined4 *)&DAT_000f_1ca0 + 0x16),
                  (int)((ulong)*(undefined4 *)&DAT_000f_1bd4 >> 0x10),
                  *(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cac + 0x16),
                  *(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cb0 + 0x16),9,0x10,0x2f);
  *(undefined2 *)0x548 = 0xffff;
  for (iStack_a = 0; iStack_a < 0x12; iStack_a = iStack_a + 1) {
    func_0x000e97dc(0xe951,0x10,iStack_a + 0x24);
  }
  func_0x000e97dc(0xe951,0x90,0x37);
  func_0x000e97dc(0xe951,0x93,0x38);
  func_0x000e97dc(0xe951,0x80,0x36);
  for (iStack_a = 0; iStack_a < 0x13; iStack_a = iStack_a + 1) {
    *(int *)(iStack_a * 2 + 0x60a) =
         (int)*(undefined4 *)&DAT_000f_1cec + (iStack_a + 0x24) * 9 +
         (int)((ulong)*(undefined4 *)&DAT_000f_1cec >> 0x10) * 0x10;
  }
  uVar1 = *(undefined4 *)&DAT_000f_1cec;
  *(int *)0x62e = (int)uVar1 + 0x144;
  *(undefined2 *)0x630 = (int)((ulong)uVar1 >> 0x10);
  *(undefined2 *)0x682 = 0xffff;
  *(undefined2 *)0x684 = 0xffff;
  *(undefined2 *)0x67c = 0xffff;
  *(undefined2 *)0x67d = 0xffff;
  return;
}



void __stdcall16far FUN_000f_2566(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined2 unaff_DS;
  int iStack_a;
  
  if (param_1 == 4) {
    func_0x000e951a();
    uVar1 = *(undefined4 *)0x6ae;
    *(undefined2 *)0x6a6 = (int)uVar1;
    *(undefined2 *)0x6a8 = (int)((ulong)uVar1 >> 0x10);
  }
  else if (param_1 == 6) {
    *(undefined *)&DAT_000f_056e = 0x54;
    *(undefined1 *)0x65f = 0;
    func_0x000e95c8();
    *(undefined *)&DAT_000f_056e = 0x65;
    *(undefined1 *)0x65f = 0x6b;
    *(undefined1 *)&DAT_000f_0570 = 0;
    *(undefined2 *)0x678 = 5;
    func_0x000e95c8(0xe951,0x65e);
    *(undefined2 *)0x678 = 0;
    *(undefined1 *)
     (*(int *)((int)*(undefined4 *)&DAT_000f_1ca0 + 0xc) + (int)*(undefined4 *)&DAT_000f_1bd4 + 0x2c
     ) = 7;
    *(undefined1 *)
     (*(int *)((int)*(undefined4 *)&DAT_000f_1ca0 + 0xc) + (int)*(undefined4 *)&DAT_000f_1bd4 + 0x2d
     ) = 10;
    *(undefined1 *)
     (*(int *)((int)*(undefined4 *)&DAT_000f_1ca0 + 0xc) + *(int *)&DAT_000f_1b12 +
      (int)*(undefined4 *)&DAT_000f_1bd4 + 0x2e) = 9;
    *(undefined1 *)
     (*(int *)((int)*(undefined4 *)&DAT_000f_1ca0 + 0xc) + *(int *)&DAT_000f_1b12 +
      (int)*(undefined4 *)&DAT_000f_1bd4 + 0x2f) = 9;
  }
  else if (param_1 == 7) {
    if ((((undefined *)&DAT_000f_1b2b)[*(int *)0x570 * 0x14] == '\0') ||
       ((*(int *)(*(int *)0x570 * 0x1c + 0x576) != 0 &&
        (*(int *)((*(int *)0x570 * 2 + 1) * 0xe + 0x576) != 0)))) {
      *(undefined *)&DAT_000f_056e = 4;
    }
    else {
      *(undefined *)&DAT_000f_056e = 7;
    }
    *(undefined1 *)0x65f = 0x56;
    *(undefined *)&DAT_000f_0570 = 0x31;
    *(undefined1 *)0x661 = 0x3d;
    *(undefined1 *)0x662 = 0;
    func_0x000e99df();
    uVar1 = *(undefined4 *)0x6ae;
    *(undefined2 *)0x69e = (int)uVar1;
    *(undefined2 *)0x6a0 = (int)((ulong)uVar1 >> 0x10);
  }
  else if (param_1 == 8) {
    if ((((undefined *)&DAT_000f_1b2b)[(*(int *)0x570 * 2 + 1) * 10] == '\0') ||
       ((*(int *)(*(int *)0x570 * 0x1c + 0x576) != 0 &&
        (*(int *)((*(int *)0x570 * 2 + 1) * 0xe + 0x576) != 0)))) {
      *(undefined *)&DAT_000f_056e = 4;
    }
    else {
      *(undefined *)&DAT_000f_056e = 7;
    }
    *(undefined1 *)0x65f = 0x56;
    *(undefined *)&DAT_000f_0570 = 0x32;
    *(undefined1 *)0x661 = 0x3d;
    *(undefined1 *)0x662 = 0;
    func_0x000e99df();
    uVar1 = *(undefined4 *)0x6ae;
    *(undefined2 *)0x6a2 = (int)uVar1;
    *(undefined2 *)0x6a4 = (int)((ulong)uVar1 >> 0x10);
  }
  else if (param_1 == 9) {
    if (*(char *)0x3ea == '\0') {
      *(undefined *)&DAT_000f_056e = 4;
      *(undefined1 *)0x65f = 0x54;
      *(undefined *)&DAT_000f_0570 = 0x3d;
      *(undefined1 *)0x661 = 0;
    }
    else {
      *(undefined *)&DAT_000f_056e = 0xf;
      *(undefined1 *)0x65f = 0x10;
      *(undefined *)&DAT_000f_0570 = 0x3d;
      *(undefined1 *)0x661 = 0;
    }
    func_0x000e99df();
    uVar1 = *(undefined4 *)0x6ae;
    *(undefined2 *)0x69a = (int)uVar1;
    *(undefined2 *)0x69c = (int)((ulong)uVar1 >> 0x10);
  }
  else if (param_1 == 10) {
    uVar6 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1bd4 >> 0x10);
    uVar7 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1ca0 >> 0x10);
    iVar4 = (int)*(undefined4 *)&DAT_000f_1ca0;
    iVar3 = (int)*(undefined4 *)&DAT_000f_1bd4 + *(int *)(iVar4 + 0x14);
    iVar2 = iVar3;
    uVar5 = uVar6;
    func_0x000eac86();
    func_0x000eac86(0xea34,0x2c0,(char *)s_reformated_000f_fe8a + 1,iVar3 + *(int *)&DAT_000f_1b12,
                    uVar6,iVar2,uVar5,iVar4,uVar7);
    for (iStack_a = 6; iStack_a < 0x85; iStack_a = iStack_a + 2) {
      *(undefined1 *)(iStack_a + iVar3) = 7;
      *(char *)(iStack_a + iVar3 + 1) = (char)iStack_a + -4;
      *(undefined1 *)(iVar3 + iStack_a + *(int *)&DAT_000f_1b12) = 0xf9;
      *(undefined1 *)(iVar3 + iStack_a + *(int *)&DAT_000f_1b12 + 1) = 0xf9;
    }
    func_0x000eac86(0xea34,0x2c8,(char *)s_reformated_000f_fe8a + 1,iVar3 + 0x86,uVar6,iVar2,uVar5,
                    iVar4,uVar7);
    func_0x000eac86(0xea34,0x2ce,(char *)s_reformated_000f_fe8a + 1,
                    iVar3 + *(int *)&DAT_000f_1b12 + 0x86,uVar6);
  }
  else if (param_1 == 0xd) {
    func_0x000e95c8();
    uVar1 = *(undefined4 *)0x6ae;
    *(int *)0x696 = (int)uVar1 + 2;
    *(undefined2 *)0x698 = (int)((ulong)uVar1 >> 0x10);
    func_0x000e95c8(0xe951,0x311,(char *)s_reformated_000f_fe8a + 1,(int)*(undefined4 *)0x696,
                    (int)((ulong)*(undefined4 *)0x696 >> 0x10),0x674);
  }
  else if (param_1 == 0xe) {
    func_0x000e95c8();
    uVar1 = *(undefined4 *)0x6ae;
    *(int *)0x68a = (int)uVar1 + 2;
    *(undefined2 *)0x68c = (int)((ulong)uVar1 >> 0x10);
    func_0x000e95c8(0xe951,0x2de,(char *)s_reformated_000f_fe8a + 1,(int)*(undefined4 *)0x68a,
                    (int)((ulong)*(undefined4 *)0x68a >> 0x10),0x674);
    uVar1 = *(undefined4 *)0x6ae;
    *(int *)0x68e = (int)uVar1 + 2;
    *(undefined2 *)0x690 = (int)((ulong)uVar1 >> 0x10);
    if (*(char *)0x3f0 == '\0') {
      func_0x000e95c8(0xe951,0x2e7,(char *)s_reformated_000f_fe8a + 1,(int)*(undefined4 *)0x68e,
                      (int)((ulong)*(undefined4 *)0x68e >> 0x10),0x674);
    }
    else {
      func_0x000e9704(0xe951,0x2f0,(char *)s_reformated_000f_fe8a + 1,(int)*(undefined4 *)0x68e,
                      (int)((ulong)*(undefined4 *)0x68e >> 0x10),0x674);
    }
    uVar1 = *(undefined4 *)0x6ae;
    *(int *)0x692 = (int)uVar1 + 2;
    *(undefined2 *)0x694 = (int)((ulong)uVar1 >> 0x10);
    if (*(char *)0x3f0 == '\0') {
      func_0x000e95c8(0xe951,0x2f9,(char *)s_reformated_000f_fe8a + 1,(int)*(undefined4 *)0x692,
                      (int)((ulong)*(undefined4 *)0x692 >> 0x10),0x674);
    }
    else {
      func_0x000e9704(0xe951,0x302,(char *)s_reformated_000f_fe8a + 1,(int)*(undefined4 *)0x692,
                      (int)((ulong)*(undefined4 *)0x692 >> 0x10),0x674);
    }
  }
  else if (param_1 == 0xf) {
    uVar5 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1bd4 >> 0x10);
    uVar6 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1ca0 >> 0x10);
    iVar3 = (int)*(undefined4 *)&DAT_000f_1ca0;
    iVar2 = (int)*(undefined4 *)&DAT_000f_1bd4 + *(int *)(iVar3 + 0x1e);
    func_0x000eac86();
    if ((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 8) == 0) {
      func_0x000eac86(0xea34,0x293,(char *)s_reformated_000f_fe8a + 1,iVar2 + 0xd,uVar5,iVar3,uVar6)
      ;
    }
    else if ((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 0x41) == 1) {
      func_0x000eac86(0xea34,0x27d,(char *)s_reformated_000f_fe8a + 1,iVar2 + 0xd,uVar5,iVar3,uVar6)
      ;
    }
    else {
      func_0x000eac86(0xea34,0x288,(char *)s_reformated_000f_fe8a + 1,iVar2 + 0xd,uVar5,iVar3,uVar6)
      ;
    }
    func_0x000eac86(0xea34,0x29e,(char *)s_reformated_000f_fe8a + 1,iVar2 + *(int *)&DAT_000f_1b12,
                    uVar5);
  }
  else if (param_1 == 0x10) {
    *(undefined *)&DAT_000f_056e = 3;
    *(undefined1 *)0x65f = 0;
    func_0x000e95c8();
  }
  else if (param_1 == 0x11) {
    func_0x000e95c8();
  }
  else if (param_1 == 0x12) {
    func_0x000e95c8();
  }
  else if (param_1 == 0x13) {
    if (*(char *)&DAT_000f_1a93 == '\x14') {
      func_0x000e951a();
    }
    else {
      func_0x000e951a();
    }
  }
  else if (param_1 == 0x14) {
    func_0x000eac86();
  }
  return;
}



void __stdcall16far FUN_000f_2c86(char *param_1,char *param_2)

{
  while (*param_1 != -1) {
    *param_2 = *param_1;
    param_2 = (char *)CONCAT22(param_2._2_2_,(char *)param_2 + 1);
    param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
  }
  return;
}



void __cdecl16far FUN_000f_3325(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 uVar1;
  int iStack_a;
  
  if (*(char *)&DAT_000f_1a93 == '\x14') {
    uVar1 = 5;
    func_0x000ea566();
    func_0x000ea566(0xea34,10,uVar1);
    for (iStack_a = 0xf; iStack_a < 0x15; iStack_a = iStack_a + 1) {
      func_0x000ea566(0xea34,iStack_a);
    }
  }
  else {
    for (iStack_a = 0; iStack_a < 0x15; iStack_a = iStack_a + 1) {
      func_0x000ea566(unaff_CS,iStack_a);
      unaff_CS = 0xea34;
    }
  }
  return;
}



void __cdecl16far FUN_000f_337d(void)

{
  int iVar1;
  undefined2 in_BX;
  int iVar2;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined1 uStack_9;
  
  iVar1 = *(int *)0x570;
  if ((*(int *)0x570 == 0) || (*(char *)&DAT_000f_1a88 == '\0')) {
    *(undefined2 *)0x65c = 0;
    if ((*(byte *)&DAT_000f_1a78 & 1) == 0) {
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 0x3e;
      *(int *)0x65c = *(int *)0x65c + 1;
    }
    if ((*(byte *)&DAT_000f_1a78 & 2) != 0) {
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 7;
      *(int *)0x65c = *(int *)0x65c + 1;
    }
    uVar4 = 0;
    uVar3 = 0x56;
    func_0x000eceda();
    *(undefined2 *)0x634 = in_BX;
    *(undefined2 *)0x636 = unaff_ES;
    if ((*(byte *)&DAT_000f_1a78 & 8) != 0) {
      *(undefined1 *)((int)*(undefined4 *)0x634 + -1) = 0x16;
    }
    if (((*(byte *)&DAT_000f_1b28 + 3 <
          (uint)*(byte *)&DAT_000f_1a7a + (uint)*(byte *)&DAT_000f_1a79) ||
        ((int)((uint)*(byte *)&DAT_000f_1a7a + (uint)*(byte *)&DAT_000f_1a79) <
         (int)(*(byte *)&DAT_000f_1b28 - 3))) && (*(char *)&DAT_000f_1a88 != '\0')) {
      uStack_9 = 2;
    }
    else if ((((uint)*(byte *)&DAT_000f_1a7a + (uint)*(byte *)&DAT_000f_1a79 ==
               (uint)*(byte *)&DAT_000f_1b28) || (*(char *)&DAT_000f_1a88 == '\0')) ||
            ((*(byte *)&DAT_000f_1a85 & 8) == 0)) {
      uStack_9 = 0;
    }
    else {
      uStack_9 = 2;
    }
    if (*(char *)&DAT_000f_1a79 == -1) {
      uStack_9 = 3;
    }
  }
  else {
    *(undefined2 *)0x65c = 0;
    if ((((undefined *)&DAT_000f_1b2a)[iVar1 * 0x14] & 1) == 0) {
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 0x3e;
      *(int *)0x65c = *(int *)0x65c + 1;
    }
    if ((((undefined *)&DAT_000f_1b2a)[iVar1 * 0x14] & 2) != 0) {
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 7;
      *(int *)0x65c = *(int *)0x65c + 1;
    }
    uVar4 = 0;
    uVar3 = 0x56;
    iVar2 = *(int *)0x65c + 0x65e;
    func_0x000eceda();
    *(int *)0x634 = iVar2;
    *(undefined2 *)0x636 = unaff_ES;
    if ((((undefined *)&DAT_000f_1b2a)[iVar1 * 0x14] & 8) != 0) {
      *(undefined1 *)((int)*(undefined4 *)0x634 + -1) = 0x16;
    }
    uStack_9 = 0;
  }
  func_0x000e97dc(0xead0,0x11,0x24,uVar3,uVar4);
  func_0x000e99df(0xe951,(int)*(undefined4 *)&DAT_000f_1bd4 + *(int *)*(undefined4 *)&DAT_000f_1ca0,
                  (int)((ulong)*(undefined4 *)&DAT_000f_1bd4 >> 0x10),uStack_9);
  return;
}



void __cdecl16far FUN_000f_356b(void)

{
  int iVar1;
  undefined2 in_BX;
  int iVar2;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined1 uStack_9;
  
  iVar1 = *(int *)0x570 * 2 + 1;
  if ((*(int *)0x570 == 0) || (*(char *)&DAT_000f_1a88 == '\0')) {
    *(undefined2 *)0x65c = 0;
    if ((*(byte *)&DAT_000f_1a7b & 1) == 0) {
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 0x3e;
      *(int *)0x65c = *(int *)0x65c + 1;
    }
    if ((*(byte *)&DAT_000f_1a7b & 2) != 0) {
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 7;
      *(int *)0x65c = *(int *)0x65c + 1;
    }
    if (*(char *)&DAT_000f_1a7c != '\0') {
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 0x17;
      *(int *)0x65c = *(int *)0x65c + 1;
    }
    uVar4 = 0;
    uVar3 = 0x56;
    func_0x000eceda();
    *(undefined2 *)0x634 = in_BX;
    *(undefined2 *)0x636 = unaff_ES;
    if ((*(byte *)&DAT_000f_1a7b & 8) != 0) {
      *(undefined1 *)((int)*(undefined4 *)0x634 + -1) = 0x16;
    }
    if (((*(byte *)&DAT_000f_1b32 + 3 <
          (uint)*(byte *)&DAT_000f_1a7e + (uint)*(byte *)&DAT_000f_1a7d) ||
        ((int)((uint)*(byte *)&DAT_000f_1a7e + (uint)*(byte *)&DAT_000f_1a7d) <
         (int)(*(byte *)&DAT_000f_1b32 - 3))) && (*(char *)&DAT_000f_1a88 != '\0')) {
      uStack_9 = 2;
    }
    else if ((((uint)*(byte *)&DAT_000f_1a7e + (uint)*(byte *)&DAT_000f_1a7d ==
               (uint)*(byte *)&DAT_000f_1b32) || (*(char *)&DAT_000f_1a88 == '\0')) ||
            ((*(byte *)&DAT_000f_1a85 & 8) == 0)) {
      uStack_9 = 0;
    }
    else {
      uStack_9 = 2;
    }
    if (*(char *)&DAT_000f_1a7d == -1) {
      uStack_9 = 3;
    }
  }
  else {
    *(undefined2 *)0x65c = 0;
    if ((((undefined *)&DAT_000f_1b2a)[iVar1 * 10] & 1) == 0) {
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 0x3e;
      *(int *)0x65c = *(int *)0x65c + 1;
    }
    if ((((undefined *)&DAT_000f_1b2a)[iVar1 * 10] & 2) != 0) {
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 7;
      *(int *)0x65c = *(int *)0x65c + 1;
    }
    if ((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 0x80) != 0) {
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 0x17;
      *(int *)0x65c = *(int *)0x65c + 1;
    }
    uVar4 = 0;
    uVar3 = 0x56;
    iVar2 = *(int *)0x65c + 0x65e;
    func_0x000eceda();
    *(int *)0x634 = iVar2;
    *(undefined2 *)0x636 = unaff_ES;
    if ((((undefined *)&DAT_000f_1b2a)[iVar1 * 10] & 8) != 0) {
      *(undefined1 *)((int)*(undefined4 *)0x634 + -1) = 0x16;
    }
    uStack_9 = 0;
  }
  func_0x000e97dc(0xead0,0x11,0x25,uVar3,uVar4);
  func_0x000e99df(0xe951,(int)*(undefined4 *)&DAT_000f_1bd4 +
                         *(int *)((int)*(undefined4 *)&DAT_000f_1ca0 + 2),
                  (int)((ulong)*(undefined4 *)&DAT_000f_1bd4 >> 0x10),uStack_9);
  return;
}



void __cdecl16far FUN_000f_3799(void)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined1 uStack_9;
  
  uVar6 = 0x26;
  uVar5 = 0x10;
  uVar3 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1bd4 >> 0x10);
  uVar4 = 0xe951;
  uVar1 = uVar3;
  func_0x000e9858();
  *(undefined2 *)0x65c = 0;
  if ((*(int *)0x570 == 0) || (*(char *)&DAT_000f_1a88 == '\0')) {
    if (*(char *)&DAT_000f_1a88 == '\0') {
      if ((*(byte *)&DAT_000f_1a82 & 0x10) != 0) {
        *(undefined1 *)(*(int *)0x65c + 0x65e) = 0x3e;
        *(int *)0x65c = *(int *)0x65c + 1;
      }
    }
    else if (((*(byte *)&DAT_000f_1a82 & 0x10) != 0) && (*(char *)&DAT_000f_1a80 == '\x04')) {
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 99;
      *(int *)0x65c = *(int *)0x65c + 1;
    }
    if (*(char *)&DAT_000f_1a74 == '\x16') {
      iVar2 = *(int *)0x65c;
      *(int *)0x65c = *(int *)0x65c + 1;
      *(undefined1 *)(iVar2 + 0x65e) = 0x45;
      iVar2 = *(int *)0x65c;
      *(int *)0x65c = *(int *)0x65c + 1;
      *(undefined1 *)(iVar2 + 0x65e) = 0x58;
      iVar2 = *(int *)0x65c;
      *(int *)0x65c = *(int *)0x65c + 1;
      *(undefined1 *)(iVar2 + 0x65e) = 0x54;
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 0;
      *(int *)0x634 = *(int *)0x65c + 0x65e;
      *(undefined2 *)0x636 = unaff_DS;
    }
    else if (((*(char *)&DAT_000f_1a88 == '\0') || ((*(byte *)&DAT_000f_1a82 & 0x40) == 0)) ||
            (*(byte *)&DAT_000f_1a74 < 0x13)) {
      uVar4 = 0xead0;
      func_0x000eceda(0xe951,*(int *)0x65c + 0x65e);
      *(undefined2 *)0x634 = uVar1;
      *(undefined2 *)0x636 = uVar3;
    }
    else {
      uVar4 = 0xead0;
      func_0x000eceda(0xe951,*(int *)0x65c + 0x65e);
      *(undefined2 *)0x634 = uVar1;
      *(undefined2 *)0x636 = uVar3;
    }
    if ((*(char *)&DAT_000f_1a9c == *(char *)0x18e) || (*(char *)&DAT_000f_1a88 == '\0')) {
      uStack_9 = 0;
    }
    else {
      uStack_9 = 2;
    }
  }
  else {
    if ((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 0x40) != 0) {
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 99;
      *(int *)0x65c = *(int *)0x65c + 1;
    }
    if (*(char *)(*(int *)0x570 * 0x10 + 0x18e) == '\x19') {
      iVar2 = *(int *)0x65c;
      *(int *)0x65c = *(int *)0x65c + 1;
      *(undefined1 *)(iVar2 + 0x65e) = 0x45;
      iVar2 = *(int *)0x65c;
      *(int *)0x65c = *(int *)0x65c + 1;
      *(undefined1 *)(iVar2 + 0x65e) = 0x58;
      iVar2 = *(int *)0x65c;
      *(int *)0x65c = *(int *)0x65c + 1;
      *(undefined1 *)(iVar2 + 0x65e) = 0x54;
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 0;
      *(int *)0x634 = *(int *)0x65c + 0x65e;
      *(undefined2 *)0x636 = unaff_DS;
    }
    else {
      iVar2 = *(int *)0x65c + 0x65e;
      uVar4 = 0xead0;
      func_0x000eceda(0xe951,iVar2);
      *(int *)0x634 = iVar2;
      *(undefined2 *)0x636 = uVar3;
    }
    uStack_9 = 0;
  }
  func_0x000e97dc(uVar4,0x11,0x26,uVar5,uVar6);
  func_0x000e99df(0xe951,(int)*(undefined4 *)&DAT_000f_1bd4 +
                         *(int *)((int)*(undefined4 *)&DAT_000f_1ca0 + 4),
                  (int)((ulong)*(undefined4 *)&DAT_000f_1bd4 >> 0x10),uStack_9);
  return;
}



void __cdecl16far FUN_000f_3a12(void)

{
  undefined2 in_BX;
  int iVar1;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined1 uStack_9;
  
  *(undefined2 *)0x65c = 0;
  if ((*(int *)0x570 == 0) || (*(char *)&DAT_000f_1a88 == '\0')) {
    if (*(char *)&DAT_000f_1a88 == '\0') {
      if ((*(byte *)&DAT_000f_1a82 & 0x10) != 0) {
        *(undefined1 *)(*(int *)0x65c + 0x65e) = 0x3e;
        *(int *)0x65c = *(int *)0x65c + 1;
      }
    }
    else if (((*(byte *)&DAT_000f_1a82 & 0x10) != 0) && (*(char *)&DAT_000f_1a80 == '\x04')) {
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 99;
      *(int *)0x65c = *(int *)0x65c + 1;
    }
    uVar3 = 0;
    uVar2 = 0x73;
    func_0x000eceda();
    *(undefined2 *)0x634 = in_BX;
    *(undefined2 *)0x636 = unaff_ES;
    if ((*(char *)&DAT_000f_1a9c == *(char *)0x18e) || (*(char *)&DAT_000f_1a88 == '\0')) {
      uStack_9 = 0;
    }
    else {
      uStack_9 = 2;
    }
  }
  else {
    if ((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 0x40) != 0) {
      *(undefined1 *)(*(int *)0x65c + 0x65e) = 99;
      *(int *)0x65c = *(int *)0x65c + 1;
    }
    uVar3 = *(undefined2 *)0x570;
    uVar2 = 0x73;
    iVar1 = *(int *)0x65c + 0x65e;
    func_0x000eceda();
    *(int *)0x634 = iVar1;
    *(undefined2 *)0x636 = unaff_ES;
    uStack_9 = 0;
  }
  func_0x000e97dc(0xead0,0x11,0x27,uVar2,uVar3);
  func_0x000e99df(0xe951,(int)*(undefined4 *)&DAT_000f_1bd4 +
                         *(int *)((int)*(undefined4 *)&DAT_000f_1ca0 + 6),
                  (int)((ulong)*(undefined4 *)&DAT_000f_1bd4 >> 0x10),uStack_9);
  return;
}



void __cdecl16far FUN_000f_3b6b(void)

{
  undefined2 uVar1;
  undefined2 in_BX;
  undefined2 unaff_ES;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  *(undefined2 *)0x65c = 0;
  if (*(char *)&DAT_000f_1a74 == '\x16') {
    *(undefined *)&DAT_000f_056e = 0x31;
    *(undefined1 *)0x65f = 100;
    *(undefined1 *)&DAT_000f_0570 = 0;
    *(undefined2 *)0x634 = 0x660;
    *(undefined2 *)0x636 = unaff_DS;
  }
  else {
    unaff_CS = 0xead0;
    func_0x000eceda();
    *(undefined2 *)0x634 = in_BX;
    *(undefined2 *)0x636 = unaff_ES;
  }
  if (((*(char *)((int)*(undefined4 *)0x634 + -2) == 'm') ||
      (*(char *)((int)*(undefined4 *)0x634 + -2) == 'n')) ||
     (*(char *)((int)*(undefined4 *)0x634 + -2) == '\x15')) {
    *(undefined1 *)0x655 = *(undefined1 *)((int)*(undefined4 *)0x634 + -2);
  }
  else {
    *(undefined1 *)0x655 = 0;
  }
  *(undefined1 *)0x654 = *(undefined1 *)((int)*(undefined4 *)0x634 + -1);
  uVar1 = func_0x000ece82(unaff_CS,*(int *)0x65c + 0x65e);
  *(undefined2 *)0x650 = uVar1;
  if ((*(byte *)&DAT_000f_1a82 & 0x20) != 0) {
    *(int *)0x652 = *(int *)0x652 + 1;
  }
  return;
}



void __cdecl16far FUN_000f_3c3a(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_CS;
  undefined2 uVar5;
  undefined2 unaff_DS;
  int iStack_20;
  int iStack_16;
  int iStack_12;
  int iStack_c;
  int iStack_a;
  
  iVar3 = *(int *)0x570;
  iStack_c = iVar3 * 2;
  iVar1 = iVar3 * 0x1c;
  iStack_12 = iVar1 + 0x574;
  iVar2 = (iStack_c + 1) * 0xe;
  iStack_16 = iVar2 + 0x574;
  iVar3 = iVar3 * 0x14;
  iVar4 = (iStack_c + 1) * 10;
  if ((*(uint *)0x676 & 0x401) != 0) {
    if (((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 1) == 0) ||
       ((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 0x40) != 0)) {
      iStack_a = (*(int *)(iVar1 + 0x576) >> 2) - (*(int *)(iVar2 + 0x576) >> 2);
    }
    else {
      iStack_a = *(int *)(iVar1 + 0x576) - *(int *)(iVar2 + 0x576);
    }
    if (iStack_a < 0) {
      iStack_a = -iStack_a;
    }
    uVar5 = 0xead0;
    func_0x000ed0ae();
    if (*(char *)&DAT_000f_1b09 != '\0') {
      uVar5 = 0x91ba;
      func_0x000924d2(0xead0,0);
    }
    unaff_CS = uVar5;
    if (*(char *)0x3ea != '\0') {
      unaff_CS = 0xead0;
      func_0x000ed1d9(uVar5,iStack_a,3,0x65e);
    }
    if (*(char *)0x686 == '\0') {
      func_0x000e97dc(unaff_CS,0x11,0x2d);
      *(undefined2 *)0x678 = 0x15;
      unaff_CS = 0xe951;
      func_0x000e99df(0xe951,(int)*(undefined4 *)0x69a,(int)((ulong)*(undefined4 *)0x69a >> 0x10),0)
      ;
    }
    *(undefined2 *)0x678 = 0;
  }
  if ((*(uint *)0x676 & 0x801) != 0) {
    if ((*(int *)(iVar1 + 0x576) == 0) || (*(int *)(iVar2 + 0x576) == 0)) {
      uVar5 = unaff_CS;
      if (((undefined *)&DAT_000f_1b2b)[iVar3] == '\0') {
        if (*(char *)0x67c == '\0') {
          uVar5 = 0xea34;
          func_0x000ea566(unaff_CS,7);
          *(undefined1 *)0x67c = 1;
          *(undefined2 *)0x682 = 0xffff;
        }
      }
      else if (*(char *)0x67c != '\0') {
        func_0x000ea566(unaff_CS,7);
        *(undefined1 *)0x67c = 0;
        *(undefined2 *)0x682 = 0xffff;
        uVar5 = 0xea34;
      }
      unaff_CS = uVar5;
      if (((undefined *)&DAT_000f_1b2b)[iVar4] == '\0') {
        if (*(char *)0x67d == '\0') {
          unaff_CS = 0xea34;
          func_0x000ea566(uVar5,8);
          *(undefined1 *)0x67d = 1;
          *(undefined2 *)0x684 = 0xffff;
        }
      }
      else if (*(char *)0x67d != '\0') {
        func_0x000ea566(uVar5,8);
        *(undefined1 *)0x67d = 0;
        *(undefined2 *)0x684 = 0xffff;
        unaff_CS = 0xea34;
      }
    }
    else if ((*(char *)0x67d == '\0') || (*(char *)0x67c == '\0')) {
      func_0x000ea566(unaff_CS,7);
      func_0x000ea566(0xea34,8);
      *(undefined1 *)0x67c = 1;
      *(undefined1 *)0x67d = 1;
      *(undefined2 *)0x682 = 0xffff;
      *(undefined2 *)0x684 = 0xffff;
      unaff_CS = 0xea34;
    }
  }
  iVar1 = *(int *)(iVar2 + 0x576);
  if (((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 0x40) != 0) && (iStack_c == 0)) {
    iStack_c = 8;
    iStack_12 = 0x5e4;
    iStack_16 = 0x5f2;
  }
  if (((*(byte *)(*(int *)0x570 * 0x10 + 399) & 9) == 0) &&
     (((*(byte *)(*(int *)0x570 * 0x10 + 399) & 2) != 0 ||
      (((undefined *)&DAT_000f_1b28)[*(int *)0x570 * 0x14] !=
       ((undefined *)&DAT_000f_1b28)[(*(int *)0x570 * 2 + 1) * 10])))) {
    func_0x000e97dc(unaff_CS,0x10,0x2b);
    *(undefined2 *)0x682 = 0xffff;
    unaff_CS = 0xe951;
  }
  else if ((*(uint *)0x676 & 0x805) != 0) {
    if (*(char *)0x67c == '\0') {
      if (iVar1 == 0) {
        iVar3 = *(int *)((undefined *)&DAT_000f_1b2c + iVar3);
        iStack_a = func_0x000f830e(unaff_CS,(int)*(undefined4 *)&DAT_000f_1b90,
                                   (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iStack_c,
                                   *(undefined2 *)(iStack_12 + 6));
        iStack_a = iStack_a - iVar3;
      }
      else {
        iVar3 = *(int *)((undefined *)&DAT_000f_1b2c + iVar3);
        iStack_a = func_0x000f830e(unaff_CS,(int)*(undefined4 *)&DAT_000f_1b90,
                                   (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iStack_c,
                                   *(undefined2 *)(iStack_16 + 6));
        iStack_a = iStack_a - iVar3;
      }
    }
    else if ((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 0x20) == 0) {
      iVar3 = *(int *)(iStack_16 + 6) + 1;
      uVar5 = func_0x000f830e(unaff_CS,(int)*(undefined4 *)&DAT_000f_1b90,
                              (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iStack_c);
      iStack_a = func_0x000f830e(0xf80a,(int)*(undefined4 *)&DAT_000f_1b90,
                                 (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iStack_c,
                                 *(int *)(iStack_12 + 6) + 1,uVar5);
      iStack_a = iStack_a - iVar3;
    }
    else {
      iVar3 = *(int *)(iStack_16 + 6);
      uVar5 = func_0x000f830e(unaff_CS,(int)*(undefined4 *)&DAT_000f_1b90,
                              (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iStack_c);
      iStack_a = func_0x000f830e(0xf80a,(int)*(undefined4 *)&DAT_000f_1b90,
                                 (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iStack_c,
                                 *(undefined2 *)(iStack_12 + 6),uVar5);
      iStack_a = iStack_a - iVar3;
    }
    iStack_20 = 0;
    if ((iStack_a < 0) && (iStack_a = -iStack_a, *(char *)0x67c == '\0')) {
      *(undefined *)&DAT_000f_056e = 0x2d;
      iStack_20 = 1;
    }
    if (((*(uint *)0x676 & 4) != 0) || (unaff_CS = 0xf80a, *(int *)0x682 != iStack_a)) {
      *(int *)0x682 = iStack_a;
      uVar5 = 0xead0;
      func_0x000ed0ae(0xf80a,iStack_a,0,iStack_20 + 0x65e);
      if (*(char *)0x686 == '\0') {
        func_0x000e97dc(0xead0,0x11,0x2b);
        *(undefined2 *)0x678 = 0x1c;
        uVar5 = 0xe951;
        func_0x000e99df(0xe951,(int)*(undefined4 *)0x69e,(int)((ulong)*(undefined4 *)0x69e >> 0x10),
                        0);
        *(undefined2 *)0x678 = 0;
      }
      unaff_CS = uVar5;
      if (*(char *)&DAT_000f_1b09 != '\0') {
        unaff_CS = 0x91ba;
        func_0x000924d2(uVar5,1);
      }
    }
  }
  if ((*(byte *)(*(int *)0x570 * 0x10 + 399) & 10) == 0) {
    func_0x000e97dc(unaff_CS,0x10,0x2c);
    *(undefined2 *)0x684 = 0xffff;
  }
  else if ((*(uint *)0x676 & 0x809) != 0) {
    if (*(char *)0x67d == '\0') {
      if (iVar1 == 0) {
        iStack_c = iStack_c + 1;
        iStack_a = func_0x000f830e(unaff_CS,(int)*(undefined4 *)&DAT_000f_1b90,
                                   (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iStack_c,
                                   *(undefined2 *)(iStack_12 + 6),
                                   *(undefined2 *)((undefined *)&DAT_000f_1b2c + iVar4));
        iStack_a = iStack_a - iStack_c;
      }
      else {
        iStack_c = iStack_c + 1;
        iStack_a = func_0x000f830e(unaff_CS,(int)*(undefined4 *)&DAT_000f_1b90,
                                   (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iStack_c,
                                   *(undefined2 *)(iStack_16 + 6),
                                   *(undefined2 *)((undefined *)&DAT_000f_1b2c + iVar4));
        iStack_a = iStack_a - iStack_c;
      }
    }
    else if ((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 0x20) == 0) {
      iVar3 = func_0x000f830e(unaff_CS,(int)*(undefined4 *)&DAT_000f_1b90,
                              (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iStack_c,
                              *(undefined2 *)(iStack_16 + 6));
      iStack_a = func_0x000f830e(0xf80a,(int)*(undefined4 *)&DAT_000f_1b90,
                                 (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iStack_c,
                                 *(undefined2 *)(iStack_12 + 6));
      iStack_a = iStack_a - iVar3;
    }
    else {
      iVar3 = func_0x000f830e(unaff_CS,(int)*(undefined4 *)&DAT_000f_1b90,
                              (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iStack_c + 1,
                              *(undefined2 *)(iStack_16 + 6));
      iStack_a = func_0x000f830e(0xf80a,(int)*(undefined4 *)&DAT_000f_1b90,
                                 (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iStack_c + 1,
                                 *(undefined2 *)(iStack_12 + 6));
      iStack_a = iStack_a - iVar3;
    }
    iStack_20 = 0;
    if (iStack_a < 0) {
      iStack_a = -iStack_a;
      if ((*(char *)0x67d == '\0') && ((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 0x80) == 0)) {
        *(undefined *)&DAT_000f_056e = 0x2d;
        iStack_20 = 1;
      }
    }
    else if ((*(char *)0x67d == '\0') && ((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 0x80) != 0)) {
      *(undefined *)&DAT_000f_056e = 0x2d;
      iStack_20 = 1;
    }
    if (((*(uint *)0x676 & 8) != 0) || (*(int *)0x684 != iStack_a)) {
      *(int *)0x684 = iStack_a;
      uVar5 = 0xead0;
      func_0x000ed0ae(0xf80a,iStack_a,1,iStack_20 + 0x65e);
      if (*(char *)0x686 == '\0') {
        func_0x000e97dc(0xead0,0x11,0x2c);
        *(undefined2 *)0x678 = 0x1c;
        uVar5 = 0xe951;
        func_0x000e99df(0xe951,(int)*(undefined4 *)0x6a2,(int)((ulong)*(undefined4 *)0x6a2 >> 0x10),
                        0);
        *(undefined2 *)0x678 = 0;
      }
      if (*(char *)&DAT_000f_1b09 != '\0') {
        func_0x000924d2(uVar5,2);
      }
    }
  }
  return;
}



void __cdecl16far FUN_000f_42e6(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined1 uStack_9;
  
  if ((((*(byte *)&DAT_000f_1a82 & 0x10) == 0) && (*(char *)&DAT_000f_1a80 == '\x04')) &&
     (*(char *)&DAT_000f_1a88 != '\0')) {
    func_0x000ed0ae();
  }
  else if ((((*(byte *)&DAT_000f_1a82 & 0x10) == 0) || (*(char *)&DAT_000f_1a80 != '\x04')) ||
          (*(char *)&DAT_000f_1a88 == '\0')) {
    func_0x000ed0ae();
  }
  else {
    func_0x000ed0ae();
  }
  uVar1 = 0xead0;
  if (*(char *)0x686 == '\0') {
    func_0x000e97dc(0xead0,0x11,0x28);
    *(undefined2 *)0x678 = 0x23;
    if ((*(char *)&DAT_000f_1a9c == *(char *)0x18e) || (*(char *)&DAT_000f_1a88 == '\0')) {
      uStack_9 = 0;
    }
    else {
      uStack_9 = 2;
    }
    uVar1 = 0xe951;
    func_0x000e99df(0xe951,(int)*(undefined4 *)0x6a6,(int)((ulong)*(undefined4 *)0x6a6 >> 0x10),
                    uStack_9);
    *(undefined2 *)0x678 = 0;
  }
  if (*(char *)&DAT_000f_1b09 != '\0') {
    func_0x000924d2(uVar1,3);
  }
  return;
}



void __cdecl16far FUN_000f_43ee(void)

{
  undefined2 unaff_DS;
  
  *(int *)0x672 = (*(int *)(*(int *)0x570 * 0x1c + 0x578) >> 3) + 0xf6;
  if (*(char *)&DAT_000f_1a93 == '\x14') {
    func_0x000e994b();
  }
  else {
    func_0x000e994b();
  }
  return;
}



void __cdecl16far FUN_000f_444f(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_DS;
  undefined2 uVar4;
  undefined2 uVar5;
  
  iVar1 = *(int *)(*(int *)0x570 * 0x1c + 0x576);
  iVar2 = *(int *)((*(int *)0x570 * 2 + 1) * 0xe + 0x576);
  iVar3 = *(int *)(*(int *)0x570 * 0x10 + 0x192);
  if (iVar3 < 1) {
    uVar5 = 0x36;
    uVar4 = 0x90;
    func_0x000e97dc();
  }
  else {
    iVar3 = (iVar3 >> 3) + 0xea;
    uVar5 = 0x36;
    func_0x000e97dc();
    if (*(char *)&DAT_000f_1a93 == '\x14') {
      uVar4 = 0x36;
      func_0x000e994b(0xe951,iVar3,0x334,0x36,uVar5);
    }
    else {
      uVar4 = 0x36;
      func_0x000e994b(0xe951,iVar3,*(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cb0 + 0x24),0x36,
                      uVar5);
    }
  }
  func_0x000e994b(0xe951,(iVar1 >> 3) + 0xee,
                  *(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cb0 + 0x20),0x34,uVar4,uVar5,iVar3)
  ;
  func_0x000e994b(0xe951,(iVar2 >> 3) + 0xee,
                  *(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cb0 + 0x22),0x35);
  return;
}



void __cdecl16far FUN_000f_4542(void)

{
  byte bVar1;
  undefined2 unaff_DS;
  
  bVar1 = *(byte *)(*(int *)0x570 * 0x10 + 0x18d) & 0xf;
  if (bVar1 == 0) {
    func_0x000e9858();
  }
  else if (bVar1 == 1) {
    func_0x000e9858();
  }
  else if (bVar1 == 2) {
    func_0x000e9858();
  }
  else {
    func_0x000e9858();
  }
  return;
}



void __cdecl16far FUN_000f_4629(void)

{
  char cVar1;
  undefined2 unaff_CS;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  undefined1 uVar4;
  bool bVar5;
  undefined4 uVar6;
  uint uStack_a;
  
  uVar6 = CONCAT22(*(int *)&DAT_000f_1b00 >> 0xf,*(int *)&DAT_000f_1b00 >> 4);
  for (uStack_a = *(uint *)&DAT_000f_1b00 & 0xf; 0 < (int)uStack_a; uStack_a = uStack_a - 1) {
    uVar6 = func_0x000e777d(unaff_CS,10,0,uVar6);
    unaff_CS = 0xe772;
  }
  uVar2 = unaff_CS;
  if ((*(byte *)&DAT_000f_1a86 & 2) == 0) {
    uVar4 = 0;
    bVar5 = true;
    uVar2 = 0xe772;
    func_0x000e7753(unaff_CS,0,0);
    if (!bVar5) {
      uVar2 = 0xe772;
      func_0x000e7753(0xe772,*(undefined2 *)&DAT_000f_1b02,*(undefined2 *)&DAT_000f_1b04);
      if (((((bool)uVar4) && (*(int *)0x54a == 0)) && (*(char *)0x1f != '\x03')) &&
         (bVar5 = *(char *)0x1f == '\x04', !bVar5)) {
        func_0x000e7753(0xe772,*(undefined2 *)0x67e,*(undefined2 *)0x680);
        if (bVar5) {
          return;
        }
        uVar2 = *(undefined2 *)&DAT_000f_1b04;
        *(undefined2 *)0x67e = *(undefined2 *)&DAT_000f_1b02;
        *(undefined2 *)0x680 = uVar2;
        uStack_a = 0;
        while ((bVar5 = uStack_a == 7, (int)uStack_a < 7 && (func_0x000e774c(0xe772), !bVar5))) {
          cVar1 = func_0x000e7866(0xe772,10,0);
          *(char *)(0x664 - uStack_a) = cVar1 + '0';
          func_0x000e783d(0xe772,10,0);
          uStack_a = uStack_a + 1;
        }
        *(undefined1 *)(0x664 - uStack_a) = 0x23;
        *(undefined1 *)0x665 = 0;
        func_0x000e951a(0xe772,0x664 - uStack_a);
        func_0x000e9858(0xe951,(int)*(undefined4 *)&DAT_000f_1bd4 + 0xd70,
                        (int)((ulong)*(undefined4 *)&DAT_000f_1bd4 >> 0x10),
                        *(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cac + 0x1a),
                        *(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cb0 + 0x1a),0x10,0x10,0x31);
        func_0x000e97dc(0xe951,0x11,0x31);
        return;
      }
    }
  }
  uVar3 = uVar2;
  if ((*(byte *)&DAT_000f_1a86 & 2) == 0) {
    uVar4 = 0;
    bVar5 = true;
    uVar3 = 0xe772;
    func_0x000e7753(uVar2,0,0);
    if (bVar5) goto LAB_000f_4843;
    uVar3 = 0xe772;
    func_0x000e7753(0xe772,*(undefined2 *)&DAT_000f_1b02,*(undefined2 *)&DAT_000f_1b04);
    if ((((bool)uVar4) || (*(char *)0x1f == '\x03')) || (*(char *)0x1f == '\x04'))
    goto LAB_000f_4843;
  }
  if (*(int *)0x54a == 0) {
    if ((*(byte *)&DAT_000f_1a86 & 2) == 0) {
      *(undefined2 *)0x67e = 0xffff;
      *(undefined2 *)0x680 = 0xffff;
      func_0x000e9858(uVar3,(int)*(undefined4 *)0x696,(int)((ulong)*(undefined4 *)0x696 >> 0x10),
                      *(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cac + 0x1a),
                      *(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cb0 + 0x1a),0x10,0x10,0x31);
    }
    else {
      func_0x000e9858(uVar3,(int)*(undefined4 *)&DAT_000f_1bd4 +
                            *(int *)((int)*(undefined4 *)&DAT_000f_1ca0 + 0x1a),
                      (int)((ulong)*(undefined4 *)&DAT_000f_1bd4 >> 0x10),
                      *(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cac + 0x1a),
                      *(undefined2 *)((int)*(undefined4 *)&DAT_000f_1cb0 + 0x1a),0x10,0x10,0x31);
    }
    func_0x000e97dc(0xe951,0x11,0x31);
    return;
  }
LAB_000f_4843:
  func_0x000e97dc(uVar3,0x10,0x31);
  return;
}



void __cdecl16far FUN_000f_48a0(void)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  char *pcVar4;
  undefined2 uVar5;
  
  uVar1 = *(undefined4 *)&DAT_000f_1bd4;
  uVar3 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1ca0 >> 0x10);
  iVar2 = (int)*(undefined4 *)&DAT_000f_1ca0;
  *(int *)0x6ae = (int)uVar1 + *(int *)(iVar2 + 0x16);
  *(undefined2 *)0x6b0 = (int)((ulong)uVar1 >> 0x10);
  *(undefined2 *)0x65c = 0;
  if (*(int *)0x54e == 0) {
    if (*(int *)0x54a == 0) {
      if (((*(byte *)&DAT_000f_1af4 |
           *(byte *)&DAT_000f_1af5 |
           *(byte *)&DAT_000f_1af6 |
           *(byte *)&DAT_000f_1af7 | *(byte *)&DAT_000f_1af8 | *(byte *)&DAT_000f_1af9) & 1) == 0) {
        if (*(char *)&DAT_000f_1a87 == '\0') {
          uVar3 = 2;
          pcVar4 = (char *)s_reformated_000f_fe8a + 1;
          func_0x000e9a05();
          func_0x000e97dc(0xe951,0x11,0x30,pcVar4,uVar3);
          func_0x000e97dc(0xe951,0x10,0x2f);
        }
        else {
          uVar3 = 0x2f;
          func_0x000e97dc();
          func_0x000e97dc(0xe951,0x10,0x30,uVar3);
        }
      }
      else {
        uVar3 = 0;
        pcVar4 = (char *)s_reformated_000f_fe8a + 1;
        func_0x000e9a05();
        *(undefined2 *)0x678 = 0x2a;
        if (((*(byte *)0x19c & 1) == 0) ||
           (*(char *)&DAT_000f_1af4 == '\0' && *(char *)&DAT_000f_1af5 == '\0')) {
          if (((*(byte *)&DAT_000f_1af4 | *(byte *)&DAT_000f_1af5) & 1) != 0) {
            if (*(int *)0x570 == 1) {
              func_0x000e9a05(0xe951,0x1b3,(char *)s_reformated_000f_fe8a + 1,1,pcVar4,uVar3);
            }
            else {
              func_0x000e9a05(0xe951,0x1b5,(char *)s_reformated_000f_fe8a + 1,0,pcVar4,uVar3);
            }
            *(int *)0x678 = *(int *)0x678 + 0x13;
          }
          if (((*(byte *)&DAT_000f_1af6 | *(byte *)&DAT_000f_1af7) & 1) != 0) {
            if (*(int *)0x570 == 2) {
              func_0x000e9a05(0xe951,0x1b7,(char *)s_reformated_000f_fe8a + 1,1,pcVar4,uVar3);
            }
            else {
              func_0x000e9a05(0xe951,0x1b9,(char *)s_reformated_000f_fe8a + 1,0,pcVar4,uVar3);
            }
            *(int *)0x678 = *(int *)0x678 + 0x13;
          }
          if (((*(byte *)&DAT_000f_1af8 | *(byte *)&DAT_000f_1af9) & 1) != 0) {
            if (*(int *)0x570 == 3) {
              func_0x000e9a05(0xe951,0x1bb,(char *)s_reformated_000f_fe8a + 1,1,pcVar4,uVar3);
            }
            else {
              func_0x000e9a05(0xe951,0x1bd,(char *)s_reformated_000f_fe8a + 1,0,pcVar4,uVar3);
            }
            *(int *)0x678 = *(int *)0x678 + 0x13;
          }
          if (*(int *)0x570 == 0) {
            func_0x000e9a05(0xe951,0x1bf,(char *)s_reformated_000f_fe8a + 1,1,pcVar4,uVar3);
          }
          else {
            func_0x000e9a05(0xe951,0x1c1,(char *)s_reformated_000f_fe8a + 1,0,pcVar4,uVar3);
          }
        }
        else {
          if (*(int *)0x570 == 1) {
            func_0x000e9a05(0xe951,0x1a3,(char *)s_reformated_000f_fe8a + 1,1,pcVar4,uVar3);
          }
          else {
            func_0x000e9a05(0xe951,0x1a9,(char *)s_reformated_000f_fe8a + 1,0,pcVar4,uVar3);
          }
          *(int *)0x678 = *(int *)0x678 + 0x2f;
          if (*(int *)0x570 == 0) {
            func_0x000e9a05(0xe951,0x1af,(char *)s_reformated_000f_fe8a + 1,1);
          }
          else {
            func_0x000e9a05(0xe951,0x1b1,(char *)s_reformated_000f_fe8a + 1,0);
          }
        }
        func_0x000e97dc(0xe951,0x11,0x30);
        func_0x000e97dc(0xe951,0x10,0x2f);
      }
      *(undefined2 *)0x678 = 0;
    }
    else if (*(int *)0x54c == 0) {
      if (((*(byte *)0x19c & 1) == 0) ||
         (*(char *)&DAT_000f_1af4 == '\0' && *(char *)&DAT_000f_1af5 == '\0')) {
        uVar3 = 0;
        pcVar4 = (char *)s_reformated_000f_fe8a + 1;
        func_0x000e9a05();
        *(undefined2 *)0x678 = 0x23;
        if ((*(char *)&DAT_000f_1af4 == '\0') && (*(char *)&DAT_000f_1af5 == '\0')) {
          func_0x000e9a05(0xe951,(int)*(undefined4 *)&DAT_000f_1c94,
                          (int)((ulong)*(undefined4 *)&DAT_000f_1c94 >> 0x10),0,pcVar4,uVar3);
          *(int *)0x678 = *(int *)0x678 + 0x38;
        }
        else {
          func_0x000e9a05(0xe951,(int)*(undefined4 *)&DAT_000f_1c84,
                          (int)((ulong)*(undefined4 *)&DAT_000f_1c84 >> 0x10),0,pcVar4,uVar3);
          *(int *)0x678 = *(int *)0x678 + 0x31;
        }
        if ((*(char *)&DAT_000f_1af6 == '\0') && (*(char *)&DAT_000f_1af7 == '\0')) {
          func_0x000e9a05(0xe951,(int)*(undefined4 *)&DAT_000f_1c98,
                          (int)((ulong)*(undefined4 *)&DAT_000f_1c98 >> 0x10),0);
          *(int *)0x678 = *(int *)0x678 + 0x38;
        }
        else {
          func_0x000e9a05(0xe951,(int)*(undefined4 *)&DAT_000f_1c88,
                          (int)((ulong)*(undefined4 *)&DAT_000f_1c88 >> 0x10),0);
          *(int *)0x678 = *(int *)0x678 + 0x31;
        }
        if ((*(char *)&DAT_000f_1af8 == '\0') && (*(char *)&DAT_000f_1af9 == '\0')) {
          func_0x000e9a05(0xe951,(int)*(undefined4 *)&DAT_000f_1c9c,
                          (int)((ulong)*(undefined4 *)&DAT_000f_1c9c >> 0x10),0);
          *(int *)0x678 = *(int *)0x678 + 0x38;
        }
        else {
          func_0x000e9a05(0xe951,(int)*(undefined4 *)&DAT_000f_1c8c,
                          (int)((ulong)*(undefined4 *)&DAT_000f_1c8c >> 0x10),0);
          *(int *)0x678 = *(int *)0x678 + 0x31;
        }
        *(undefined2 *)0x678 = 0;
        func_0x000e97dc(0xe951,0x11,0x2f);
        func_0x000e97dc(0xe951,0x10,0x30);
      }
      else {
        uVar5 = 0;
        uVar3 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1c90 >> 0x10);
        func_0x000e9a05();
        func_0x000e97dc(0xe951,0x11,0x2f,uVar3,uVar5);
        func_0x000e97dc(0xe951,0x10,0x30);
      }
    }
    else {
      uVar5 = 0;
      uVar3 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1c80 >> 0x10);
      func_0x000e9a05();
      func_0x000e97dc(0xe951,0x11,0x2f,uVar3,uVar5);
      func_0x000e97dc(0xe951,0x10,0x30);
    }
  }
  else {
    uVar5 = 0;
    pcVar4 = (char *)s_reformated_000f_fe8a + 1;
    func_0x000e9a05();
    func_0x000e97dc(0xe951,0x11,0x30,pcVar4,uVar5,iVar2,uVar3);
    func_0x000e97dc(0xe951,0x10,0x2f);
  }
  return;
}



void __stdcall16far FUN_000f_4ced(char *param_1,int param_2)

{
  char cVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 *puStack_c;
  
  cVar1 = *(char *)((param_2 + -1) * 6 + 0x63d);
  iVar2 = param_2 * 6;
  _puStack_c = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(iVar2 + 0x638));
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  if (((char *)param_1)[1] == 'd') {
    *(undefined1 *)(iVar2 + 0x63c) = 0x20;
  }
  else {
    *(undefined1 *)(iVar2 + 0x63c) = 0xe;
  }
  uVar3 = func_0x000ece82();
  *_puStack_c = uVar3;
  if (*param_1 == '0') {
    *(undefined2 *)(iVar2 + 0x63a) = 2;
  }
  else if (((char *)param_1)[1] == '0') {
    *(undefined2 *)(iVar2 + 0x63a) = 0;
  }
  else {
    *(undefined2 *)(iVar2 + 0x63a) = 1;
  }
  if (cVar1 == 'm') {
    *(undefined1 *)(iVar2 + 0x63d) = 0x6b;
  }
  else if (cVar1 == '\x15') {
    *(undefined1 *)(iVar2 + 0x63d) = 0x4d;
  }
  else if (cVar1 == 'n') {
    *(undefined1 *)(iVar2 + 0x63d) = 0x47;
  }
  else {
    *(undefined1 *)(iVar2 + 0x63d) = 0;
  }
  return;
}



void __stdcall16far FUN_000f_4dc7(char *param_1,int *param_2)

{
  if (*param_2 < 1) {
    *param_2 = *param_2 + 3;
    if (*param_1 == 'k') {
      *param_1 = '\0';
    }
    else if (*param_1 == 'M') {
      *param_1 = 'k';
    }
    else if (*param_1 == 'G') {
      *param_1 = 'M';
    }
    else {
      *param_1 = 'm';
    }
    func_0x000ecdc7();
  }
  else if (3 < *param_2) {
    *param_2 = *param_2 + -3;
    if (*param_1 == 'k') {
      *param_1 = 'M';
    }
    else if (*param_1 == 'M') {
      *param_1 = 'G';
    }
    else if (*param_1 == 'm') {
      *param_1 = '\0';
    }
    else {
      *param_1 = 'k';
    }
    func_0x000ecdc7();
  }
  return;
}



char * __stdcall16far FUN_000f_4e82(byte *param_1,undefined2 *param_2)

{
  byte bVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  if (*param_1 == 0x30) {
    *param_2 = 2;
    bVar1 = ((byte *)param_1)[2];
  }
  else {
    if (((byte *)param_1)[1] == 0x30) {
      *param_2 = 4;
    }
    else {
      *param_2 = 3;
    }
    bVar1 = *param_1;
  }
  return (char *)s_OMM_RB_000f_fee0 + bVar1;
}



void __stdcall16far FUN_000f_4eda(char *param_1,byte param_2,uint param_3,char param_4,byte param_5)

{
  int iVar1;
  char *pcVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  char cStack_d;
  uint uStack_c;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  pcVar2 = (char *)param_1;
  if (param_4 == 'V') {
    param_2 = param_2 + 0xd;
  }
  if ((uint)param_2 % 3 == 0) {
    cStack_d = '5';
  }
  else if ((uint)param_2 % 3 == 1) {
    cStack_d = '1';
  }
  else {
    cStack_d = '2';
  }
  uStack_c = (param_2 + 2) / 3;
  if (param_4 == 's') {
    if ((param_3 & 0x20) != 0) {
      uStack_c = uStack_c - 1;
    }
  }
  else if (param_3 == 9) {
    uStack_c = uStack_c + 3;
  }
  else if (param_3 == 6) {
    uStack_c = uStack_c + 2;
  }
  else if (param_3 == 3) {
    uStack_c = uStack_c + 1;
  }
  iVar1 = uStack_c + 2;
  if (iVar1 % 3 == 2) {
    *param_1 = cStack_d;
    pcVar2[1] = '0';
    param_1 = (char *)CONCAT22(uVar3,pcVar2 + 2);
  }
  else if (iVar1 % 3 == 0) {
    *param_1 = '0';
    pcVar2[1] = '.';
    pcVar2[2] = cStack_d;
    param_1 = (char *)CONCAT22(uVar3,pcVar2 + 3);
  }
  else {
    *param_1 = cStack_d;
    param_1 = (char *)CONCAT22(uVar3,pcVar2 + 1);
  }
  if (iVar1 / 3 == 0) {
    *param_1 = 'n';
    param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
  }
  else if (iVar1 / 3 == 1) {
    *param_1 = '\x15';
    param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
  }
  else {
    if (iVar1 / 3 == 2) {
      *param_1 = 'm';
      param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
    }
    if (iVar1 / 3 == 4) {
      *param_1 = 'k';
      param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
    }
  }
  *param_1 = param_4;
  param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
  *param_1 = '\0';
  if ((param_5 & 0x80) == 0) {
    if ((((*(byte *)((uint)param_5 * 0x10 + 0x18c) & 0x40) != 0) && (param_4 == 's')) &&
       (*(char *)&DAT_000f_1a88 != '\0')) {
      func_0x000ed64e();
    }
  }
  else if (((param_5 & 0x40) != 0) && (param_4 == 's')) {
    func_0x000ed64e();
  }
  return;
}



void __stdcall16far FUN_000f_50ae(int param_1,int param_2,char *param_3)

{
  bool bVar1;
  int iVar2;
  undefined2 unaff_DS;
  int iStack_e;
  int iStack_c;
  int iStack_a;
  
  bVar1 = true;
  iStack_e = 10000;
  if (*(char *)(param_2 * 6 + 0x63c) == '%') {
    iStack_c = param_1 << 1;
  }
  else {
    iStack_c = param_1 * *(int *)(param_2 * 6 + 0x638);
  }
  for (iStack_a = 0; iStack_a < 5; iStack_a = iStack_a + 1) {
    if (*(int *)(param_2 * 6 + 0x63a) == iStack_a) {
      *param_3 = '.';
      param_3 = (char *)CONCAT22(param_3._2_2_,(char *)param_3 + 1);
    }
    iVar2 = iStack_c / iStack_e;
    iStack_c = iStack_c % iStack_e;
    iStack_e = iStack_e / 10;
    if (iVar2 == 0) {
      if ((!bVar1) || (*(int *)(param_2 * 6 + 0x63a) + -2 < iStack_a)) {
        *param_3 = '0';
        param_3 = (char *)CONCAT22(param_3._2_2_,(char *)param_3 + 1);
        bVar1 = false;
      }
    }
    else {
      *param_3 = (char)iVar2 + '0';
      param_3 = (char *)CONCAT22(param_3._2_2_,(char *)param_3 + 1);
      bVar1 = false;
    }
  }
  if (*(char *)(param_2 * 6 + 0x63d) != '\0') {
    *param_3 = *(char *)(param_2 * 6 + 0x63d);
    param_3 = (char *)CONCAT22(param_3._2_2_,(char *)param_3 + 1);
  }
  *param_3 = *(char *)(param_2 * 6 + 0x63c);
  param_3 = (char *)CONCAT22(param_3._2_2_,(char *)param_3 + 1);
  *param_3 = '\0';
  return;
}



uint __stdcall16far FUN_000f_51d9(int param_1,int param_2,byte *param_3)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined2 unaff_CS;
  uint unaff_DS;
  int *piStack_1a;
  uint local_14;
  int local_12;
  uint uStack_e;
  uint uStack_c;
  int iStack_a;
  
  bVar1 = true;
  uStack_e = 10000;
  iVar2 = param_2 * 6;
  _piStack_1a = (int *)CONCAT22(unaff_DS,(int *)(iVar2 + 0x638));
  iVar4 = param_1 * *_piStack_1a;
  local_14 = (uint)*(byte *)(iVar2 + 0x63d);
  *param_3 = 0;
  if (iVar4 < 10000) {
    if (iVar4 < 1000) {
      if (iVar4 < 100) {
        if (iVar4 < 10) {
          *param_3 = 0x3f;
          ((byte *)param_3)[1] = 0;
          uVar3 = unaff_DS;
        }
        else {
          unaff_CS = 0xe94b;
          uVar3 = func_0x000e94b2();
          local_12 = *(int *)(iVar2 + 0x63a) + 2;
          uStack_c = uVar3;
        }
      }
      else {
        unaff_CS = 0xe94b;
        uVar3 = func_0x000e94cc();
        local_12 = *(int *)(iVar2 + 0x63a);
        uStack_c = uVar3;
      }
    }
    else {
      unaff_CS = 0xe94b;
      uVar3 = func_0x000e94e6();
      local_12 = *(int *)(iVar2 + 0x63a) + -1;
      uStack_c = uVar3;
    }
  }
  else {
    unaff_CS = 0xe94b;
    uVar3 = func_0x000e9500();
    local_12 = *(int *)(iVar2 + 0x63a) + -2;
    uStack_c = uVar3;
  }
  if (*param_3 != 0x3f) {
    if (*(char *)(iVar2 + 0x63c) != ' ') {
      uVar3 = func_0x000ecdc7(unaff_CS,&local_14);
    }
    iStack_a = 0;
    while( true ) {
      if (4 < iStack_a) break;
      if (local_12 == iStack_a) {
        *param_3 = 0x2e;
        param_3 = (byte *)CONCAT22(param_3._2_2_,(byte *)param_3 + 1);
      }
      iVar4 = (int)uStack_c / (int)uStack_e;
      uStack_c = (int)uStack_c % (int)uStack_e;
      uVar3 = (int)uStack_e / 10;
      if (iVar4 == 0) {
        if ((!bVar1) || (local_12 + -2 < iStack_a)) {
          *param_3 = 0x30;
          param_3 = (byte *)CONCAT22(param_3._2_2_,(byte *)param_3 + 1);
          bVar1 = false;
        }
      }
      else {
        *param_3 = (char)iVar4 + 0x30;
        param_3 = (byte *)CONCAT22(param_3._2_2_,(byte *)param_3 + 1);
        bVar1 = false;
      }
      iStack_a = iStack_a + 1;
      uStack_e = uVar3;
    }
    if (local_14 != 0) {
      *param_3 = (byte)local_14;
      param_3 = (byte *)CONCAT22(param_3._2_2_,(byte *)param_3 + 1);
    }
    uVar3 = CONCAT11((char)(uVar3 >> 8),*(byte *)(iVar2 + 0x63c));
    *param_3 = *(byte *)(iVar2 + 0x63c);
    param_3 = (byte *)CONCAT22(param_3._2_2_,(byte *)param_3 + 1);
    *param_3 = 0;
  }
  return uVar3;
}



void __cdecl16far FUN_000f_5396(void)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined *puVar4;
  undefined *puVar5;
  uint uVar6;
  undefined2 unaff_CS;
  undefined2 uVar7;
  undefined2 unaff_DS;
  
  iVar1 = *(int *)0x570 * 0x10;
  puVar4 = (undefined *)&DAT_000f_1b24 + *(int *)0x570 * 0x14;
  iVar2 = (*(int *)0x570 * 2 + 1) * 10;
  puVar5 = (undefined *)&DAT_000f_1b24 + iVar2;
  if (((((undefined *)&DAT_000f_1b2a)[*(int *)0x570 * 0x14] & 1) == 0) ||
     ((((undefined *)&DAT_000f_1b2a)[iVar2] & 1) == 0 && (*(byte *)(iVar1 + 399) | 0xf4) == 0xf4)) {
    *(undefined1 *)&DAT_000f_054d = 0;
    *(undefined *)&DAT_000f_054c = 0x25;
    *(undefined2 *)&DAT_000f_0548 = 1;
    *(undefined2 *)&DAT_000f_054a = 4;
  }
  else {
    unaff_CS = 0xead0;
    func_0x000eceda();
    *(undefined2 *)0x634 = puVar4;
    *(undefined2 *)0x636 = unaff_DS;
    if ((*(int *)0x570 != 0) || ((*(byte *)&DAT_000f_1a86 & 2) == 0)) {
      if (((*(char *)((int)*(undefined4 *)0x634 + -2) == 'm') ||
          (*(char *)((int)*(undefined4 *)0x634 + -2) == 'n')) ||
         (*(char *)((int)*(undefined4 *)0x634 + -2) == '\x15')) {
        *(undefined *)&DAT_000f_054d = *(undefined1 *)((int)*(undefined4 *)0x634 + -2);
      }
      else {
        *(undefined1 *)&DAT_000f_054d = 0;
      }
      *(undefined *)&DAT_000f_054c = *(undefined1 *)((int)*(undefined4 *)0x634 + -1);
      unaff_CS = 0xead0;
      uVar3 = func_0x000ece82(0xead0,0x65e);
      *(undefined2 *)&DAT_000f_0548 = uVar3;
    }
  }
  if ((((undefined *)&DAT_000f_1b2a)[iVar2] & 1) == 0) {
    *(undefined1 *)0x643 = 0;
    *(undefined1 *)0x642 = 0x25;
    *(undefined2 *)0x63e = 1;
    *(undefined2 *)0x640 = 4;
    uVar3 = unaff_CS;
  }
  else {
    uVar3 = 0xead0;
    func_0x000eceda(unaff_CS,0x65e);
    *(undefined2 *)0x634 = puVar5;
    *(undefined2 *)0x636 = unaff_DS;
    if ((*(int *)0x570 != 0) || ((*(byte *)&DAT_000f_1a86 & 2) == 0)) {
      if ((*(char *)((int)*(undefined4 *)0x634 + -2) == 'm') ||
         ((*(char *)((int)*(undefined4 *)0x634 + -2) == 'n' ||
          (*(char *)((int)*(undefined4 *)0x634 + -2) == '\x15')))) {
        *(undefined1 *)0x643 = *(undefined1 *)((int)*(undefined4 *)0x634 + -2);
      }
      else {
        *(undefined1 *)0x643 = 0;
      }
      *(undefined1 *)0x642 = *(undefined1 *)((int)*(undefined4 *)0x634 + -1);
      uVar3 = 0xead0;
      uVar7 = func_0x000ece82(0xead0,0x65e);
      *(undefined2 *)0x63e = uVar7;
    }
  }
  if (*(char *)(iVar1 + 0x18e) == '\x19') {
    *(undefined *)&DAT_000f_056e = 0x31;
    *(undefined1 *)0x65f = 100;
    *(undefined1 *)&DAT_000f_0570 = 0;
    *(undefined2 *)0x634 = 0x660;
    *(undefined2 *)0x636 = unaff_DS;
    uVar7 = uVar3;
  }
  else {
    uVar6 = (uint)*(byte *)(iVar1 + 0x18e);
    uVar7 = 0xead0;
    func_0x000eceda(uVar3,0x65e);
    *(uint *)0x634 = uVar6;
    *(undefined2 *)0x636 = unaff_DS;
  }
  if (((*(char *)((int)*(undefined4 *)0x634 + -2) == 'm') ||
      (*(char *)((int)*(undefined4 *)0x634 + -2) == 'n')) ||
     (*(char *)((int)*(undefined4 *)0x634 + -2) == '\x15')) {
    *(undefined1 *)0x649 = *(undefined1 *)((int)*(undefined4 *)0x634 + -2);
  }
  else {
    *(undefined1 *)0x649 = 0;
  }
  *(undefined1 *)0x648 = *(undefined1 *)((int)*(undefined4 *)0x634 + -1);
  uVar3 = func_0x000ece82(uVar7,0x65e);
  *(undefined2 *)0x644 = uVar3;
  func_0x000ecced(0xead0,0x65e);
  return;
}



uint __stdcall16far FUN_000f_564e(char *param_1)

{
  char *pcVar1;
  char *pcVar2;
  uint uVar3;
  char *pcVar4;
  undefined2 uVar5;
  
  if (*param_1 == '0') {
    param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 2);
  }
  pcVar1 = param_1;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  pcVar4 = (char *)param_1;
  if (*param_1 == '5') {
    if (pcVar4[1] == '0') {
      uVar3 = (uint)(byte)pcVar4[2];
      if (uVar3 == 0x15) {
        param_1 = (char *)CONCAT22(uVar5,pcVar4 + 1);
        pcVar2 = param_1;
        *pcVar1 = '0';
        param_1 = (char *)CONCAT22(uVar5,pcVar4 + 2);
        pcVar1 = param_1;
        *pcVar2 = '.';
        param_1 = (char *)CONCAT22(uVar5,pcVar4 + 3);
        pcVar2 = param_1;
        *pcVar1 = '2';
        param_1 = (char *)CONCAT22(uVar5,pcVar4 + 4);
        pcVar1 = param_1;
        *pcVar2 = 'm';
        param_1 = (char *)CONCAT22(uVar5,pcVar4 + 5);
        *pcVar1 = 's';
        *param_1 = '\0';
        uVar3 = 1;
      }
      else if (uVar3 == 0x6d) {
        param_1 = (char *)CONCAT22(uVar5,pcVar4 + 1);
        pcVar2 = param_1;
        *pcVar1 = '0';
        param_1 = (char *)CONCAT22(uVar5,pcVar4 + 2);
        pcVar1 = param_1;
        *pcVar2 = '.';
        param_1 = (char *)CONCAT22(uVar5,pcVar4 + 3);
        pcVar2 = param_1;
        *pcVar1 = '2';
        param_1 = (char *)CONCAT22(uVar5,pcVar4 + 4);
        *pcVar2 = 's';
        *param_1 = '\0';
        uVar3 = 0;
      }
      else if (uVar3 == 0x6e) {
        param_1 = (char *)CONCAT22(uVar5,pcVar4 + 1);
        pcVar2 = param_1;
        *pcVar1 = '0';
        param_1 = (char *)CONCAT22(uVar5,pcVar4 + 2);
        pcVar1 = param_1;
        *pcVar2 = '.';
        param_1 = (char *)CONCAT22(uVar5,pcVar4 + 3);
        pcVar2 = param_1;
        *pcVar1 = '2';
        param_1 = (char *)CONCAT22(uVar5,pcVar4 + 4);
        pcVar1 = param_1;
        *pcVar2 = '\x15';
        param_1 = (char *)CONCAT22(uVar5,pcVar4 + 5);
        *pcVar1 = 's';
        *param_1 = '\0';
        uVar3 = 1;
      }
    }
    else if (pcVar4[-1] == '.') {
      pcVar4[-2] = '2';
      pcVar4[-1] = pcVar4[1];
      *param_1 = pcVar4[2];
      pcVar4[1] = pcVar4[3];
      uVar3 = 0xfffe;
    }
    else {
      pcVar4[4] = pcVar4[3];
      pcVar4[3] = pcVar4[2];
      pcVar4[2] = pcVar4[1];
      pcVar4[1] = '0';
      *param_1 = '2';
      uVar3 = 1;
    }
  }
  else {
    *param_1 = (*param_1 + -0x30) * '\x04' + '0';
    uVar3 = 0;
  }
  return uVar3;
}



void __cdecl16far FUN_000f_57df(void)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  int iStack_e;
  
  if ((*(char *)&DAT_000f_1a8a == '\0') || (*(char *)&DAT_000f_1b07 != '\0')) {
    iStack_e = 0;
    uVar3 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1bd4 >> 0x10);
    uVar4 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1ca0 >> 0x10);
    iVar2 = (int)*(undefined4 *)&DAT_000f_1ca0;
    iVar1 = (int)*(undefined4 *)&DAT_000f_1bd4 + *(int *)(iVar2 + 10);
    func_0x000eac86();
    if ((*(byte *)&DAT_000f_1b0b & 4) != 0) {
      func_0x000eac86(0xea34,0x1d9,(char *)s_reformated_000f_fe8a + 1,iVar1,uVar3,iVar2,uVar4);
      func_0x000eac86(0xea34,0x1eb,(char *)s_reformated_000f_fe8a + 1,iVar1 + *(int *)&DAT_000f_1b12
                      ,uVar3);
      iStack_e = 0xe;
    }
    if ((*(byte *)&DAT_000f_1b0b & 2) != 0) {
      func_0x000eac86(0xea34,0x1fd,(char *)s_reformated_000f_fe8a + 1,iVar1 + iStack_e,uVar3,iVar2,
                      uVar4);
      func_0x000eac86(0xea34,0x20f,(char *)s_reformated_000f_fe8a + 1,
                      iVar1 + *(int *)&DAT_000f_1b12 + iStack_e,uVar3);
      iStack_e = iStack_e + 0xe;
    }
    if ((*(byte *)&DAT_000f_1b0b & 1) != 0) {
      func_0x000eac86(0xea34,0x221,(char *)s_reformated_000f_fe8a + 1,iVar1 + iStack_e,uVar3,iVar2,
                      uVar4);
      func_0x000eac86(0xea34,0x233,(char *)s_reformated_000f_fe8a + 1,
                      iVar1 + *(int *)&DAT_000f_1b12 + iStack_e,uVar3);
    }
    func_0x000e97dc(0xea34,0x11,0x29);
  }
  return;
}



/* tag_position_marker_and_dispatch (confidence: Mechanism confirmed)
   
   Evidence: Called from `compute_and_draw_scale_marker` with `(far ptr record, value)`: tags 2 byte
   fields of the record (`[+0xC]`/`[+0x1A]`) with a flag byte, and if the record's own 2 word fields
   (`[0]` vs `[2]`) differ, calls `dispatch_item_handler_if_enabled` and ORs its result into
   `value`; then sets global flags `[0x548]` bit 1/2 from `value` bits `0x100`/`0x200` and returns
   `value` */

undefined2 tag_position_marker_and_dispatch(void)

{
  undefined1 uVar1;
  int *piVar2;
  uint uVar3;
  int unaff_BP;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  uVar1 = *(undefined1 *)(unaff_BP + -7);
  uVar5 = (undefined2)
          ((ulong)*(undefined4 *)((char *)s_ACQ_RAM_even_000f_fef8 + 0xc + unaff_BP) >> 0x10);
  iVar4 = (int)*(undefined4 *)((char *)s_ACQ_RAM_even_000f_fef8 + 0xc + unaff_BP);
  *(undefined1 *)(iVar4 + 0xc) = uVar1;
  *(undefined1 *)(iVar4 + 0x1a) = uVar1;
  piVar2 = *(int **)((char *)s_ACQ_RAM_even_000f_fef8 + 0xc + unaff_BP);
  if (*piVar2 != ((int *)piVar2)[1]) {
    uVar3 = func_0x000edffd();
    *(uint *)(unaff_BP + 8) = *(uint *)(unaff_BP + 8) | uVar3;
  }
  if ((*(uint *)(unaff_BP + 8) & 0x100) != 0) {
    *(uint *)0x548 = *(uint *)0x548 | 1;
  }
  if ((*(uint *)(unaff_BP + 8) & 0x200) != 0) {
    *(uint *)0x548 = *(uint *)0x548 | 2;
  }
  return *(undefined2 *)(unaff_BP + 8);
}



undefined2 __stdcall16far FUN_000f_5de2(int param_1,undefined2 param_2,int param_3)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  undefined2 uVar2;
  
  if (param_3 == 0) {
    uVar1 = 10;
    uVar2 = param_2;
    func_0x000ede76();
    func_0x000ede76(0xeda2,param_1 * 0x1c + 0x582,unaff_DS,10,param_2,uVar1,uVar2);
  }
  else {
    uVar1 = 10;
    uVar2 = param_2;
    func_0x000edf50();
    func_0x000edf50(0xeda2,param_1 * 0x1c + 0x582,unaff_DS,10,param_2,uVar1,uVar2);
  }
  return 0x200;
}



void __stdcall16far FUN_000f_5e76(int *param_1,int param_2,int param_3)

{
  int *piVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  int iStack_c;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  piVar1 = (int *)param_1;
  iStack_c = piVar1[1] - (piVar1[1] - piVar1[2]) / param_2;
  if (piVar1[1] < iStack_c) {
    iStack_c = piVar1[1];
  }
  if (iStack_c < piVar1[1] - param_3) {
    iStack_c = piVar1[1] - param_3;
  }
  if (iStack_c < 0) {
    iStack_c = 0;
  }
  if (0xfff - param_3 < iStack_c) {
    iStack_c = 0xfff - param_3;
  }
  if ((*(char *)&DAT_000f_1a93 == '\x14') && (piVar1[1] == *(int *)&DAT_000f_0486)) {
    *param_1 = (param_3 >> 1) + iStack_c;
    piVar1[1] = iStack_c + (param_3 >> 1);
    *(int *)0x5c8 = piVar1[1];
    *(uint *)0x560 = *(uint *)0x560 | 0x100;
    *(int *)0x590 = piVar1[1];
    *(uint *)0x55c = *(uint *)0x55c | 0x100;
  }
  piVar1[2] = iStack_c;
  *(byte *)(piVar1 + 6) = *(byte *)(piVar1 + 6) | 1;
  return;
}



void __stdcall16far FUN_000f_5f50(int *param_1,int param_2,int param_3)

{
  int *piVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  int iStack_c;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  piVar1 = (int *)param_1;
  iStack_c = piVar1[1] - param_2 * (piVar1[1] - piVar1[2]);
  if (iStack_c < 0) {
    iStack_c = 0;
  }
  if (0xfff - param_3 < iStack_c) {
    iStack_c = 0xfff - param_3;
  }
  if ((*(char *)&DAT_000f_1a93 == '\x14') && (piVar1[1] == *(int *)&DAT_000f_0486)) {
    *param_1 = (param_3 >> 1) + iStack_c;
    piVar1[1] = iStack_c + (param_3 >> 1);
    *(int *)0x5c8 = piVar1[1];
    *(uint *)0x560 = *(uint *)0x560 | 0x100;
    *(int *)0x590 = piVar1[1];
    *(uint *)0x55c = *(uint *)0x55c | 0x100;
  }
  piVar1[2] = iStack_c;
  *(byte *)(piVar1 + 6) = *(byte *)(piVar1 + 6) & 0xfe;
  return;
}



undefined2 __stdcall16far FUN_000f_5ffd(int param_1)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 uStack_1a;
  int iStack_16;
  undefined2 *puStack_e;
  
  iVar1 = param_1 * 0x1c;
  _puStack_e = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(iVar1 + 0x574));
  iVar2 = func_0x000ee879();
  if (*(char *)&DAT_000f_1a93 == '\x14') {
    iStack_16 = iVar2 / 2;
  }
  else {
    iStack_16 = iVar2 / 10;
  }
  *(undefined2 *)(iVar1 + 0x576) = *_puStack_e;
  uStack_1a = 0x100;
  if ((*(int *)(iVar1 + 0x578) + iVar2) - iStack_16 < *(int *)(iVar1 + 0x576)) {
    *(int *)(iVar1 + 0x578) = (*(int *)(iVar1 + 0x576) + iStack_16) - iVar2;
    uStack_1a = 0x300;
  }
  else if (*(int *)(iVar1 + 0x576) < *(int *)(iVar1 + 0x578) + iStack_16) {
    *(int *)(iVar1 + 0x578) = *(int *)(iVar1 + 0x576) - iStack_16;
    uStack_1a = 0x300;
  }
  if (0xfff - iVar2 < *(int *)(iVar1 + 0x578)) {
    *(int *)(iVar1 + 0x578) = 0xfff - iVar2;
  }
  else if (*(int *)(iVar1 + 0x578) < 0) {
    *(undefined2 *)(iVar1 + 0x578) = 0;
  }
  if (*(int *)(iVar1 + 0x578) < *(int *)(iVar1 + 0x584)) {
    if (*(int *)(iVar1 + 0x584) < *(int *)(iVar1 + 0x578) + iVar2) {
      *(undefined2 *)(iVar1 + 0x586) = *(undefined2 *)(iVar1 + 0x578);
    }
  }
  return uStack_1a;
}



void __stdcall16far FUN_000f_613b(int param_1)

{
  uint *puVar1;
  undefined2 unaff_DS;
  
  *(int *)0x570 = param_1;
  puVar1 = (uint *)(param_1 * 2 + 0x550);
  *puVar1 = *puVar1 | 0x800;
  *(uint *)0x53a = *(uint *)0x53a | 0x467f;
  return;
}



int __stdcall16far FUN_000f_615c(int param_1)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined2 uStack_10;
  
  iVar1 = param_1 * 0x1c;
  if ((*(byte *)(param_1 * 0x10 + 0x18c) & 8) == 0) {
    uStack_10 = *(int *)(iVar1 + 0x578);
  }
  else {
    uStack_10 = *(int *)(iVar1 + 0x576) + (*(int *)(iVar1 + 0x576) - *(int *)(iVar1 + 0x578)) * -10;
    if (uStack_10 < 0) {
      uStack_10 = 0;
    }
    if (0xc00 < uStack_10) {
      uStack_10 = 0xc00;
    }
  }
  return uStack_10;
}



undefined2 __stdcall16far FUN_000f_61dc(int param_1,int param_2)

{
  uint uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  uint uVar3;
  
  uVar1 = *(byte *)(param_1 * 0x10 + 0x18c) & 8;
  if (param_2 == 0) {
    uVar2 = unaff_DS;
    uVar3 = uVar1;
    func_0x000ee319();
    func_0x000ee319(0xeda2,param_1 * 0x1c + 0x582,unaff_DS,uVar1,uVar2,uVar3);
  }
  else {
    uVar2 = unaff_DS;
    uVar3 = uVar1;
    func_0x000ee286();
    func_0x000ee286(0xeda2,param_1 * 0x1c + 0x582,unaff_DS,uVar1,uVar2,uVar3);
  }
  return 0x240;
}



void __stdcall16far FUN_000f_6286(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  int iStack_c;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  iStack_c = *(int *)(iVar1 + 2) + (*(int *)(iVar1 + 2) - *(int *)(iVar1 + 4)) * -4;
  if (iStack_c < 0) {
    iStack_c = 0;
  }
  if (param_2 == 0) {
    if (0 < iStack_c) {
      iStack_c = 0;
    }
  }
  else if (0xe68 < iStack_c) {
    iStack_c = 0xe68;
  }
  if (*(char *)&DAT_000f_1a93 == '\x14') {
    if (*(int *)(iVar1 + 2) == *(int *)&DAT_000f_0486) {
      *(undefined2 *)0x5c8 = *(undefined2 *)(iVar1 + 2);
      *(uint *)0x560 = *(uint *)0x560 | 0x100;
      *(undefined2 *)0x590 = *(undefined2 *)(iVar1 + 2);
      *(uint *)0x55c = *(uint *)0x55c | 0x100;
    }
  }
  *(int *)(iVar1 + 4) = iStack_c;
  *(byte *)(iVar1 + 0xc) = *(byte *)(iVar1 + 0xc) | 2;
  return;
}



void __stdcall16far FUN_000f_6319(int *param_1,int param_2)

{
  int *piVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  int iStack_c;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  piVar1 = (int *)param_1;
  iStack_c = piVar1[1] - (piVar1[1] - piVar1[2]) / 4;
  if (iStack_c < 0) {
    iStack_c = 0;
  }
  if (param_2 == 0) {
    if (0xc00 < iStack_c) {
      iStack_c = 0xc00;
    }
    if (*(char *)&DAT_000f_1a93 == '\x14') {
      *param_1 = iStack_c + 0x1ff;
      piVar1[1] = iStack_c + 0x1ff;
    }
  }
  else {
    if (0xf9a < iStack_c) {
      iStack_c = 0xf9a;
    }
    if (*(char *)&DAT_000f_1a93 == '\x14') {
      *param_1 = iStack_c + 0x32;
      piVar1[1] = iStack_c + 0x32;
    }
  }
  if (*(char *)&DAT_000f_1a93 == '\x14') {
    if (piVar1[1] == *(int *)&DAT_000f_0486) {
      *(int *)0x5c8 = piVar1[1];
      *(uint *)0x560 = *(uint *)0x560 | 0x100;
      *(int *)0x590 = piVar1[1];
      *(uint *)0x55c = *(uint *)0x55c | 0x100;
    }
  }
  piVar1[2] = iStack_c;
  *(byte *)(piVar1 + 6) = *(byte *)(piVar1 + 6) & 0xfd;
  return;
}



undefined2 __stdcall16far FUN_000f_63ed(int param_1,int param_2)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_DS;
  undefined2 uStack_16;
  undefined2 uStack_14;
  undefined2 uStack_12;
  undefined2 uStack_10;
  undefined2 uStack_e;
  
  iVar3 = param_1 * 2 + 1;
  uStack_14 = *(int *)(param_1 * 0x10 + 0x192);
  uStack_14 = uStack_14 - (uStack_14 >> 2);
  if (param_2 == 4) {
    if ((*(byte *)(param_1 * 0x1c + 0x580) & 2) == 0) {
      if ((*(byte *)(param_1 * 0x1c + 0x580) & 1) == 0) {
        uStack_16 = 0x3ff;
      }
      else {
        uStack_16 = 0x65;
      }
      uStack_e = (*(int *)(param_1 * 0x1c + 0x576) >> 2) + uStack_14;
      uStack_12 = (*(int *)(param_1 * 0x1c + 0x578) >> 2) + uStack_14;
      uStack_10 = (*(int *)(iVar3 * 0xe + 0x576) >> 2) + uStack_14;
      uStack_14 = (*(int *)(iVar3 * 0xe + 0x578) >> 2) + uStack_14;
      if (uStack_12 < 0) {
        uStack_12 = 0;
      }
      else if (0xfff < uStack_12 + uStack_16) {
        uStack_12 = 0xfff - uStack_16;
      }
      if (uStack_14 < 0) {
        uStack_14 = 0;
      }
      else if (0xfff < uStack_14 + uStack_16) {
        uStack_14 = 0xfff - uStack_16;
      }
      if (uStack_e < uStack_12) {
        uStack_e = uStack_12;
      }
      else if (uStack_12 + uStack_16 < uStack_e) {
        uStack_e = uStack_12 + uStack_16;
      }
      if (uStack_10 < uStack_14) {
        uStack_10 = uStack_14;
      }
      else if (uStack_14 + uStack_16 < uStack_10) {
        uStack_10 = uStack_14 + uStack_16;
      }
    }
    else {
      iVar2 = func_0x000ee879();
      uStack_e = (*(int *)(param_1 * 0x1c + 0x576) >> 2) + uStack_14;
      uStack_12 = (*(int *)(param_1 * 0x1c + 0x578) >> 2) + uStack_14;
      uStack_10 = (*(int *)(iVar3 * 0xe + 0x576) >> 2) + uStack_14;
      uStack_14 = (*(int *)(iVar3 * 0xe + 0x578) >> 2) + uStack_14;
      if (uStack_12 < 0) {
        uStack_12 = 0;
      }
      else if (0xfff < uStack_12 + iVar2) {
        uStack_12 = 0xfff - iVar2;
      }
      if (uStack_14 < 0) {
        uStack_14 = 0;
      }
      else if (0xfff < uStack_14 + iVar2) {
        uStack_14 = 0xfff - iVar2;
      }
      if (uStack_e < uStack_12) {
        uStack_e = uStack_12;
      }
      else if (uStack_12 + iVar2 < uStack_e) {
        uStack_e = uStack_12 + iVar2;
      }
      if (uStack_10 < uStack_14) {
        uStack_10 = uStack_14;
      }
      else if (uStack_14 + iVar2 < uStack_10) {
        uStack_10 = uStack_14 + iVar2;
      }
    }
    pbVar1 = (byte *)(param_1 * 0x1c + 0x580);
    *pbVar1 = *pbVar1 | 4;
    pbVar1 = (byte *)(iVar3 * 0xe + 0x580);
    *pbVar1 = *pbVar1 | 4;
  }
  else {
    iVar2 = func_0x000ee879();
    uStack_e = (*(int *)(param_1 * 0x1c + 0x576) - uStack_14) * 4;
    uStack_10 = (*(int *)(iVar3 * 0xe + 0x576) - uStack_14) * 4;
    uStack_12 = (*(int *)(param_1 * 0x1c + 0x578) - uStack_14) * 4;
    uStack_14 = (*(int *)(iVar3 * 0xe + 0x578) - uStack_14) * 4;
    if (uStack_e < 0) {
      uStack_e = 0;
    }
    else if (0xfff < uStack_e) {
      uStack_e = 0xfff;
    }
    if (uStack_10 < 0) {
      uStack_10 = 0;
    }
    else if (0xfff < uStack_10) {
      uStack_10 = 0xfff;
    }
    if (uStack_12 < 0) {
      uStack_12 = 0;
    }
    else if (0xfff < uStack_12 + iVar2) {
      uStack_12 = 0xfff - iVar2;
    }
    if (uStack_14 < 0) {
      uStack_14 = 0;
    }
    else if (0xfff < uStack_14 + iVar2) {
      uStack_14 = 0xfff - iVar2;
    }
    pbVar1 = (byte *)(param_1 * 0x1c + 0x580);
    *pbVar1 = *pbVar1 & 0xfb;
    pbVar1 = (byte *)(iVar3 * 0xe + 0x580);
    *pbVar1 = *pbVar1 & 0xfb;
  }
  iVar2 = param_1 * 0x1c;
  *(int *)(iVar2 + 0x578) = uStack_12;
  *(int *)(iVar2 + 0x576) = uStack_e;
  *(int *)(iVar2 + 0x574) = uStack_e;
  iVar3 = iVar3 * 0xe;
  *(int *)(iVar3 + 0x578) = uStack_14;
  *(int *)(iVar3 + 0x576) = uStack_10;
  *(int *)(iVar3 + 0x574) = uStack_10;
  *(uint *)0x53a = *(uint *)0x53a | 0x100;
  return 0x740;
}



undefined2 __stdcall16far FUN_000f_67a4(int param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined4 uStack_22;
  undefined2 *puStack_1e;
  undefined2 *puStack_1a;
  undefined2 local_16 [10];
  
  iVar3 = param_1 * 0x1c;
  _puStack_1e = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(iVar3 + 0x574));
  uStack_22 = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(iVar3 + 0x582));
  _puStack_1a = (undefined2 *)CONCAT22(unaff_SS,local_16);
  *_puStack_1a = *uStack_22;
  uVar1 = *(undefined2 *)(iVar3 + 0x584);
  uVar2 = *(undefined2 *)(iVar3 + 0x586);
  *uStack_22 = *_puStack_1e;
  *(undefined2 *)(iVar3 + 0x584) = *(undefined2 *)(iVar3 + 0x576);
  *(undefined2 *)(iVar3 + 0x586) = *(undefined2 *)(iVar3 + 0x578);
  *_puStack_1e = *_puStack_1a;
  *(undefined2 *)(iVar3 + 0x576) = uVar1;
  *(undefined2 *)(iVar3 + 0x578) = uVar2;
  *(uint *)0x548 = *(uint *)0x548 | 1;
  return 0x200;
}



undefined2 __stdcall16far FUN_000f_6879(char param_1)

{
  undefined2 uStack_a;
  
  if (param_1 == '\0') {
    uStack_a = 0xfff;
  }
  else if (param_1 == '\x01') {
    uStack_a = 0x197;
  }
  else if (param_1 == '\x02') {
    uStack_a = 0xfff;
  }
  else if (param_1 == '\x03') {
    uStack_a = 0x197;
  }
  else if (param_1 == '\x04') {
    uStack_a = 0x3ff;
  }
  else if (param_1 == '\x05') {
    uStack_a = 0x65;
  }
  else if (param_1 == '\x06') {
    uStack_a = 0xfff;
  }
  else if (param_1 == '\a') {
    uStack_a = 0x197;
  }
  return uStack_a;
}



undefined2 __stdcall16far FUN_000f_68fa(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_DS;
  undefined2 uStack_e;
  
  iVar3 = param_1;
  iVar1 = func_0x000ee15c();
  iVar2 = param_1 * 0x1c;
  _uStack_e = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(iVar2 + 0x574));
  *(int *)(iVar2 + 0x576) = *(int *)(iVar2 + 0x576) - iVar1;
  if (*(int *)(iVar2 + 0x576) < 0) {
    *(undefined2 *)(iVar2 + 0x576) = 0;
  }
  *(int *)(iVar2 + 0x576) = *(int *)(iVar2 + 0x576) << 2;
  *_uStack_e = *(undefined2 *)(iVar2 + 0x576);
  *(int *)(iVar2 + 0x578) = *(int *)(iVar2 + 0x578) - iVar1;
  if (*(int *)(iVar2 + 0x578) < 0) {
    *(undefined2 *)(iVar2 + 0x578) = 0;
  }
  *(int *)(iVar2 + 0x578) = *(int *)(iVar2 + 0x578) << 2;
  *(byte *)(iVar2 + 0x580) = *(byte *)(iVar2 + 0x580) & 0xf9;
  iVar3 = func_0x000ee879(0xeda2,*(byte *)(iVar2 + 0x580) & 0xf9,iVar3);
  _uStack_e = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(iVar2 + 0x582));
  *(int *)(iVar2 + 0x584) = *(int *)(iVar2 + 0x584) - iVar1;
  if (*(int *)(iVar2 + 0x584) < 0) {
    *(undefined2 *)(iVar2 + 0x584) = 0;
  }
  *(int *)(iVar2 + 0x584) = *(int *)(iVar2 + 0x584) << 2;
  if (*(int *)(iVar2 + 0x578) + iVar3 < *(int *)(iVar2 + 0x584)) {
    *(int *)(iVar2 + 0x584) = *(int *)(iVar2 + 0x578) + iVar3;
  }
  *_uStack_e = *(undefined2 *)(iVar2 + 0x584);
  *(int *)(iVar2 + 0x586) = *(int *)(iVar2 + 0x586) - iVar1;
  if (*(int *)(iVar2 + 0x586) < 0) {
    *(undefined2 *)(iVar2 + 0x586) = 0;
  }
  *(int *)(iVar2 + 0x586) = *(int *)(iVar2 + 0x586) << 2;
  if (0xfff - iVar3 < *(int *)(iVar2 + 0x586)) {
    *(int *)(iVar2 + 0x586) = 0xfff - iVar3;
  }
  *(byte *)(iVar2 + 0x58e) = *(byte *)(iVar2 + 0x58e) & 0xf9;
  *(uint *)0x548 = *(uint *)0x548 | 0x100;
  return 0x740;
}



void __cdecl16far FUN_000f_6a58(void)

{
  undefined2 uVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  int iStack_a;
  
  if (*(char *)&DAT_000f_1a86 != '\0') {
    if (*(char *)0x512 != '\0') {
      unaff_CS = 0xeea5;
      uVar1 = func_0x000eecb2();
      *(undefined2 *)&DAT_000f_1b26 = uVar1;
      *(uint *)&DAT_000f_0460 = *(uint *)&DAT_000f_0460 | 4;
    }
    if (*(char *)0x51a != '\0') {
      uVar1 = func_0x000eecb2(unaff_CS,*(undefined2 *)&DAT_000f_1b2e,*(undefined2 *)0x516,0xff);
      *(undefined2 *)&DAT_000f_1b30 = uVar1;
      *(uint *)&DAT_000f_0460 = *(uint *)&DAT_000f_0460 | 8;
    }
    if ((*(char *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x22) != '\0') ||
       (*(char *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x40) != '\0')) {
      if ((*(uint *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x3c) & 0x80) == 0) {
        iStack_a = (*(int *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x1e) +
                   *(int *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x3c)) -
                   (uint)*(byte *)&DAT_000f_1b28;
      }
      else {
        iStack_a = *(int *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x1e) -
                   (uint)*(byte *)&DAT_000f_1b28;
      }
      if (3 < iStack_a) {
        iStack_a = 3;
      }
      if (iStack_a < -3) {
        iStack_a = -3;
      }
      *(char *)&DAT_000f_1b29 = *(char *)&DAT_000f_1b28 + (char)iStack_a;
      *(uint *)&DAT_000f_0460 = *(uint *)&DAT_000f_0460 | 1;
    }
    if ((*(char *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x2e) != '\0') ||
       (*(char *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x3a) != '\0')) {
      if ((*(uint *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x36) & 0x80) == 0) {
        iStack_a = (*(int *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x2a) +
                   *(int *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x36)) -
                   (uint)*(byte *)&DAT_000f_1b32;
      }
      else {
        iStack_a = *(int *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x2a) -
                   (uint)*(byte *)&DAT_000f_1b32;
      }
      if (3 < iStack_a) {
        iStack_a = 3;
      }
      if (iStack_a < -3) {
        iStack_a = -3;
      }
      *(char *)&DAT_000f_1b33 = *(char *)&DAT_000f_1b32 + (char)iStack_a;
      *(uint *)&DAT_000f_0460 = *(uint *)&DAT_000f_0460 | 2;
    }
  }
  if (*(char *)&DAT_000f_1a93 == '\x14') {
    if ((*(byte *)0x4fc & 8) != 0) {
      if ((*(byte *)0x4fb & 8) == 0) {
        *(byte *)&DAT_000f_0474 = *(byte *)&DAT_000f_0474 & 0xfe;
      }
      else {
        *(byte *)&DAT_000f_0474 = *(byte *)&DAT_000f_0474 | 1;
      }
      *(uint *)0x53a = *(uint *)0x53a | 0x80;
      *(uint *)&DAT_000f_0460 = *(uint *)&DAT_000f_0460 | 0x210;
    }
    if ((*(byte *)0x4fc & 4) != 0) {
      if ((*(byte *)0x4fb & 4) == 0) {
        *(byte *)&DAT_000f_0474 = *(byte *)&DAT_000f_0474 & 0xfd;
      }
      else {
        *(byte *)&DAT_000f_0474 = *(byte *)&DAT_000f_0474 | 2;
      }
      if ((*(byte *)0x18c & 1) != 0) {
        *(uint *)&DAT_000f_0460 = *(uint *)&DAT_000f_0460 | 0x40;
      }
      *(uint *)0x53a = *(uint *)0x53a | 0x100;
    }
  }
  else {
    if ((*(byte *)0x4ec & 8) != 0) {
      if ((*(byte *)0x4eb & 8) == 0) {
        *(byte *)&DAT_000f_0474 = *(byte *)&DAT_000f_0474 & 0xfe;
      }
      else {
        *(byte *)&DAT_000f_0474 = *(byte *)&DAT_000f_0474 | 1;
      }
      *(uint *)0x53a = *(uint *)0x53a | 0x80;
      *(uint *)&DAT_000f_0460 = *(uint *)&DAT_000f_0460 | 0x210;
    }
    if ((*(byte *)0x4ec & 4) != 0) {
      if ((*(byte *)0x4eb & 4) == 0) {
        *(byte *)&DAT_000f_0474 = *(byte *)&DAT_000f_0474 & 0xfd;
      }
      else {
        *(byte *)&DAT_000f_0474 = *(byte *)&DAT_000f_0474 | 2;
      }
      if ((*(byte *)0x18c & 1) != 0) {
        *(uint *)&DAT_000f_0460 = *(uint *)&DAT_000f_0460 | 0x40;
      }
      *(uint *)0x53a = *(uint *)0x53a | 0x100;
    }
  }
  if (*(char *)0x512 != '\0') {
    *(byte *)0x601 = *(byte *)0x601 | 2;
    *(byte *)0x600 = *(byte *)0x600 | 1;
  }
  if (*(char *)0x51a != '\0') {
    *(byte *)0x601 = *(byte *)0x601 | 0x20;
    *(byte *)0x600 = *(byte *)0x600 | 2;
  }
  return;
}



int __stdcall16far FUN_000f_6cb2(int param_1,int param_2,int param_3)

{
  if (param_1 - param_3 < param_2) {
    if (param_1 + param_3 <= param_2) {
      param_2 = param_1 + param_3;
    }
  }
  else {
    param_2 = param_1 - param_3;
  }
  return param_2;
}



void __cdecl16far FUN_000f_6cea(void)

{
  byte bVar1;
  int *piVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  char cStack_26;
  undefined4 uStack_22;
  uint uStack_1a;
  undefined4 uStack_18;
  uint uStack_a;
  
  uStack_18 = (int *)*(undefined4 *)&DAT_000f_1c6c;
  for (uStack_a = 0; (int)uStack_a < 0x17; uStack_a = uStack_a + 1) {
    uVar3 = (undefined2)((ulong)uStack_18 >> 0x10);
    piVar2 = (int *)uStack_18;
    if ((((*(int *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x42) == *uStack_18) &&
         (*(int *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x48) == piVar2[1])) &&
        (((*(byte *)0x4f3 & 0x40) != 0) == (bool)(char)piVar2[2])) &&
       (((*(byte *)0x4f3 & 0x80) != 0) == (bool)*(char *)((int)piVar2 + 5))) break;
    uStack_18 = (int *)CONCAT22(uVar3,piVar2 + 3);
  }
  if (*(byte *)&DAT_000f_1a74 != uStack_a) {
    *(undefined *)&DAT_000f_1a74 = (char)uStack_a;
    *(uint *)0x53a = *(uint *)0x53a | 0x10;
  }
  uStack_22 = (int *)*(undefined4 *)&DAT_000f_1c70;
  for (uStack_1a = 0; (int)uStack_1a < 0x17; uStack_1a = uStack_1a + 1) {
    uVar3 = (undefined2)((ulong)uStack_22 >> 0x10);
    if ((*(int *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x4e) == *uStack_22) &&
       (*(int *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x54) == ((int *)uStack_22)[1])) break;
    uStack_22 = (int *)CONCAT22(uVar3,(int *)uStack_22 + 2);
  }
  if (*(byte *)&DAT_000f_1a77 != uStack_1a) {
    *(undefined *)&DAT_000f_1a77 = (char)uStack_1a;
    *(uint *)0x53a = *(uint *)0x53a | 0x20;
  }
  bVar1 = *(byte *)&DAT_000f_1a82 & 3;
  if (bVar1 != 1) {
    if (bVar1 == 2) {
      cStack_26 = *(char *)&DAT_000f_1a77;
      goto LAB_000f_6e6e;
    }
    if (bVar1 != 3) goto LAB_000f_6e6e;
  }
  cStack_26 = *(char *)&DAT_000f_1a74;
  if (cStack_26 == '\x16') {
    cStack_26 = '\x19';
  }
  else if (((*(byte *)&DAT_000f_1a82 & 0x40) != 0) && (0x12 < *(byte *)&DAT_000f_1a74)) {
    cStack_26 = cStack_26 + '\x03';
  }
LAB_000f_6e6e:
  *(char *)&DAT_000f_1a9c = cStack_26;
  return;
}



void __cdecl16far FUN_000f_7045(void)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  undefined2 unaff_CS;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  bool bVar6;
  
  bVar1 = *(byte *)0x4fb;
  if ((bVar1 & 4) == 0) {
    bVar3 = 0;
  }
  else {
    bVar3 = 0x10;
  }
  if ((bVar1 & 8) == 0) {
    bVar2 = 0;
  }
  else {
    bVar2 = 0x20;
  }
  bVar2 = bVar2 | bVar3 | *(byte *)&DAT_000f_1a82 & 0xcf;
  if (*(byte *)&DAT_000f_1a82 != bVar2) {
    *(byte *)&DAT_000f_1a82 = bVar2;
    *(uint *)0x53a = *(uint *)0x53a | 0x30;
  }
  if ((*(byte *)0x4fc & 0x12) != 0) {
    if (((bVar1 & 2) == 0) || ((bVar1 & 0x10) != 0)) {
      if (((bVar1 & 2) == 0) && ((bVar1 & 0x10) != 0)) {
        *(undefined *)&DAT_000f_1a92 = 1;
        *(undefined2 *)&DAT_000f_1afc = 0x400;
      }
      else {
        *(undefined *)&DAT_000f_1a92 = 1;
        *(undefined2 *)&DAT_000f_1afc = 0x1000;
      }
    }
    else {
      *(undefined1 *)&DAT_000f_1a92 = 0;
      *(undefined2 *)&DAT_000f_1afc = 0x400;
    }
    unaff_CS = 0xf0cc;
    func_0x000f0ccc();
  }
  uVar5 = unaff_CS;
  if ((*(byte *)0x4fc & 0x20) != 0) {
    bVar6 = (bVar1 & 0x20) == 0x20;
    *(bool *)0x1b79 = bVar6;
    if (bVar6) {
      if (*(char *)&DAT_000f_1a86 == '\0') {
        *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 | 2;
        func_0x000e6ce8(unaff_CS);
        uVar5 = 0xe6a8;
      }
      else {
        *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 | 2;
      }
    }
    else {
      *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 & 0xfd;
      uVar5 = 0xe6a8;
      func_0x000e6d04(unaff_CS);
    }
  }
  if ((*(byte *)0x4fc & 0x80) != 0) {
    *(bool *)0x1b78 = (bVar1 & 0x80) == 0x80;
    if (*(char *)&DAT_000f_1a88 == '\0') {
      if (*(char *)&DAT_000f_1a86 == '\0') {
        *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 | 1;
        func_0x000e6ce8(uVar5);
        uVar4 = 0xe6a8;
      }
      else {
        *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 | 1;
        uVar4 = uVar5;
      }
    }
    else {
      *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 & 0xfe;
      uVar4 = 0xe6a8;
      func_0x000e6d04(uVar5);
    }
    uVar5 = 0xf0cc;
    func_0x000f0ccc(uVar4);
  }
  bVar1 = *(byte *)0x4f7;
  *(undefined *)&DAT_000f_1a80 = 4;
  *(undefined *)&DAT_000f_1a76 = 4;
  *(undefined1 *)&DAT_000f_1a87 = 0;
  if (((bVar1 & 2) == 2) && (*(char *)&DAT_000f_1a88 != '\0')) {
    if (*(char *)&DAT_000f_1b07 == '\0') {
      func_0x000e92b0(uVar5);
      *(undefined1 *)0x6b6 = 1;
      *(undefined *)&DAT_000f_1b07 = 1;
      uVar5 = 0xe925;
    }
    else {
      *(int *)0x3e0 = *(int *)0x3e0 + -0x40;
      if (*(int *)0x3e0 < 0) {
        *(undefined2 *)0x3e0 = 0x25f;
      }
    }
  }
  if ((bVar1 & 0x10) == 0) {
    *(undefined1 *)0x4e0 = 0;
  }
  if ((*(char *)0x4e0 == '\0') && (*(bool *)0x1bf8 = (bVar1 & 0x10) == 0x10, (bVar1 & 0x10) == 0x10)
     ) {
    *(undefined1 *)0x3fd = 1;
    *(undefined1 *)0x608 = 1;
  }
  if (((*(byte *)0x4f8 & 0x20) != 0) && ((*(byte *)0x4f7 & 0x20) != 0)) {
    if (*(code *)FUN_000f_02fd == (code)0x1) {
      *(code *)FUN_000f_02fd = (code)0x0;
    }
    else {
      *(code *)FUN_000f_02fd = (code)0x1;
    }
    func_0x000f0ccc(uVar5);
  }
  return;
}



/* update_display_mode_flags (confidence: Mechanism confirmed; exact flags' meanings not confirmed)
   
   Evidence: Packs several bit-flags from a caller-given word into a combined status byte, stores it
   to `[0x1B72]` if changed (setting a "changed" flag at `[0x53A]`), then does mode-specific
   follow-up based on `[0x4FC]` - also sets the scale-clamp bound `[0x1BEC]` (`0x400` or `0x1000`
   depending on mode) before calling `start_plot_output_task` */

void update_display_mode_flags(void)

{
  byte bVar1;
  int unaff_BP;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  bool bVar2;
  byte in_stack_00000000;
  
  if ((*(uint *)(unaff_BP + -8) & 8) == 0) {
    bVar1 = 0;
  }
  else {
    bVar1 = 0x20;
  }
  bVar1 = bVar1 | in_stack_00000000;
  *(byte *)(unaff_BP + -9) = bVar1;
  if (*(byte *)&DAT_000f_1a82 != bVar1) {
    *(byte *)&DAT_000f_1a82 = bVar1;
    *(uint *)0x53a = *(uint *)0x53a | 0x30;
  }
  if ((*(byte *)0x4fc & 0x12) != 0) {
    if (((*(uint *)(unaff_BP + -8) & 2) == 0) || ((*(uint *)(unaff_BP + -8) & 0x10) != 0)) {
      if (((*(uint *)(unaff_BP + -8) & 2) == 0) && ((*(uint *)(unaff_BP + -8) & 0x10) != 0)) {
        *(undefined *)&DAT_000f_1a92 = 1;
        *(undefined2 *)&DAT_000f_1afc = 0x400;
      }
      else {
        *(undefined *)&DAT_000f_1a92 = 1;
        *(undefined2 *)&DAT_000f_1afc = 0x1000;
      }
    }
    else {
      *(undefined1 *)&DAT_000f_1a92 = 0;
      *(undefined2 *)&DAT_000f_1afc = 0x400;
    }
    func_0x000f0ccc();
  }
  if ((*(byte *)0x4fc & 0x20) != 0) {
    bVar2 = (*(uint *)(unaff_BP + -8) & 0x20) == 0x20;
    *(bool *)0x1b79 = bVar2;
    if (bVar2) {
      if (*(char *)&DAT_000f_1a86 == '\0') {
        *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 | 2;
        func_0x000e6ce8();
      }
      else {
        *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 | 2;
      }
    }
    else {
      *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 & 0xfd;
      func_0x000e6d04();
    }
  }
  if ((*(byte *)0x4fc & 0x80) != 0) {
    *(bool *)0x1b78 = (*(uint *)(unaff_BP + -8) & 0x80) == 0x80;
    if (*(char *)&DAT_000f_1a88 == '\0') {
      if (*(char *)&DAT_000f_1a86 == '\0') {
        *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 | 1;
        func_0x000e6ce8();
      }
      else {
        *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 | 1;
      }
    }
    else {
      *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 & 0xfe;
      func_0x000e6d04();
    }
    func_0x000f0ccc();
  }
  *(uint *)(unaff_BP + -8) = (uint)*(byte *)0x4f7;
  *(undefined *)&DAT_000f_1a80 = 4;
  *(undefined *)&DAT_000f_1a76 = 4;
  *(undefined1 *)&DAT_000f_1a87 = 0;
  if (((*(uint *)(unaff_BP + -8) & 2) == 2) && (*(char *)&DAT_000f_1a88 != '\0')) {
    if (*(char *)&DAT_000f_1b07 == '\0') {
      func_0x000e92b0();
      *(undefined1 *)0x6b6 = 1;
      *(undefined *)&DAT_000f_1b07 = 1;
    }
    else {
      *(int *)0x3e0 = *(int *)0x3e0 + -0x40;
      if (*(int *)0x3e0 < 0) {
        *(undefined2 *)0x3e0 = 0x25f;
      }
    }
  }
  if ((*(uint *)(unaff_BP + -8) & 0x10) == 0) {
    *(undefined1 *)0x4e0 = 0;
  }
  if ((*(char *)0x4e0 == '\0') &&
     (*(bool *)0x1bf8 = (*(uint *)(unaff_BP + -8) & 0x10) == 0x10,
     (*(uint *)(unaff_BP + -8) & 0x10) == 0x10)) {
    *(undefined1 *)0x3fd = 1;
    *(undefined1 *)0x608 = 1;
  }
  if (((*(byte *)0x4f8 & 0x20) != 0) && ((*(byte *)0x4f7 & 0x20) != 0)) {
    if (*(code *)FUN_000f_02fd == (code)0x1) {
      *(code *)FUN_000f_02fd = (code)0x0;
    }
    else {
      *(code *)FUN_000f_02fd = (code)0x1;
    }
    func_0x000f0ccc();
  }
  return;
}



void __cdecl16far FUN_000f_7273(void)

{
  byte bVar1;
  byte bVar2;
  undefined2 unaff_DS;
  
  bVar2 = *(byte *)0x4ef;
  if ((bVar2 & 8) == 0) {
    bVar1 = 0;
  }
  else {
    bVar1 = 0x40;
  }
  bVar1 = bVar1 | *(byte *)&DAT_000f_1a82 & 0xbf;
  if (*(byte *)&DAT_000f_1a82 != bVar1) {
    *(byte *)&DAT_000f_1a82 = bVar1;
    *(uint *)0x53a = *(uint *)0x53a | 0x30;
  }
  *(byte *)0x543 = bVar2 & *(byte *)0x4f0 & 0x80;
  if ((*(byte *)0x4f0 & 4) != 0) {
    if ((bVar2 & 4) == 0) {
      *(undefined1 *)&DAT_000f_1a7c = 0;
    }
    else {
      *(undefined *)&DAT_000f_1a7c = 1;
    }
    *(uint *)0x53a = *(uint *)0x53a | 8;
  }
  if ((bVar2 & 0x20) == 0) {
    if ((*(byte *)0x4f3 & 2) == 0) {
      *(undefined1 *)&DAT_000f_1a83 = 0;
    }
    else {
      if ((bVar2 & 0x10) == 0) {
        bVar2 = 0x10;
      }
      else {
        bVar2 = 0;
      }
      *(byte *)&DAT_000f_1a83 = bVar2 | 2;
    }
  }
  else {
    *(undefined *)&DAT_000f_1a83 = 1;
  }
  return;
}



void __stdcall16far FUN_000f_7339(void)

{
  return;
}



void __cdecl16far FUN_000f_7393(void)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined1 uStack_a;
  
  if (*(char *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x40) != '\0') {
    iVar1 = *(int *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x3c);
    if ((((iVar1 == 0) || (iVar1 == 3)) || (iVar1 == 6)) || ((iVar1 == 9 || (iVar1 == 0xff)))) {
      *(undefined *)&DAT_000f_1a79 = (char)iVar1;
    }
    else {
      *(undefined *)&DAT_000f_1a79 = 0xfe;
    }
    *(uint *)0x53a = *(uint *)0x53a | 4;
  }
  if (*(char *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x3a) != '\0') {
    iVar1 = *(int *)((int)*(undefined4 *)&DAT_000f_1c2c + 0x36);
    if (((iVar1 == 0) || (iVar1 == 3)) || ((iVar1 == 6 || ((iVar1 == 9 || (iVar1 == 0xff)))))) {
      uStack_a = (undefined1)iVar1;
      *(undefined *)&DAT_000f_1a7d = uStack_a;
    }
    else {
      *(undefined *)&DAT_000f_1a7d = 0xfe;
    }
    *(uint *)0x53a = *(uint *)0x53a | 8;
  }
  return;
}



void __cdecl16far FUN_000f_74ed(void)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_DS;
  int iStack_a;
  
  iVar3 = *(int *)0x570 << 1;
  iVar2 = *(int *)0x570;
  iStack_a = iVar3;
  do {
    iStack_a = iStack_a + 2;
    if (6 < iStack_a) {
      iStack_a = 0;
    }
  } while ((((((undefined *)&DAT_000f_1af2)[iStack_a] | ((undefined *)&DAT_000f_1af3)[iStack_a]) & 1
            ) == 0) && (iStack_a != iVar3));
  if (iStack_a == iVar3) {
    *(undefined2 *)0x570 = 0;
  }
  else {
    *(int *)0x570 = iStack_a >> 1;
  }
  *(undefined2 *)0x56e = 0;
  func_0x000ee13b();
  puVar1 = (uint *)(iVar2 * 2 + 0x550);
  *puVar1 = *puVar1 | 0x800;
  return;
}



void __cdecl16far FUN_000f_7567(void)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_DS;
  int iStack_a;
  
  iVar3 = *(int *)0x570 << 1;
  iVar2 = *(int *)0x570;
  iStack_a = iVar3;
  do {
    iStack_a = iStack_a + -2;
    if (iStack_a < 0) {
      iStack_a = 6;
    }
  } while ((((((undefined *)&DAT_000f_1af2)[iStack_a] | ((undefined *)&DAT_000f_1af3)[iStack_a]) & 1
            ) == 0) && (iStack_a != iVar3));
  if (iStack_a == iVar3) {
    *(undefined2 *)0x570 = 0;
  }
  else {
    *(int *)0x570 = iStack_a >> 1;
  }
  *(undefined2 *)0x56e = 0;
  func_0x000ee13b();
  puVar1 = (uint *)(iVar2 * 2 + 0x550);
  *puVar1 = *puVar1 | 0x800;
  return;
}



void __cdecl16far FUN_000f_75e1(void)

{
  uint *puVar1;
  int iVar2;
  undefined2 unaff_DS;
  int iStack_14;
  int iStack_10;
  int *piStack_e;
  int iStack_a;
  
  _piStack_e = (int *)CONCAT22(unaff_DS,(int *)(*(int *)0x570 * 0x1c + 0x574));
  *(int *)0x52c = *(int *)0x52c + *(int *)0x52a;
  if (*(int *)0x52c < 0) {
    iStack_14 = -1;
    iStack_10 = -*(int *)0x52c;
  }
  else {
    iStack_14 = 1;
    iStack_10 = *(int *)0x52c;
  }
  if (0x13 < iStack_10) {
    if (iStack_10 < 0x1e) {
      iStack_10 = 1;
    }
    else if (iStack_10 < 0x46) {
      iStack_10 = iStack_10 + -0x1d;
    }
    else if (0x78 < iStack_10) {
      iStack_10 = iStack_10 + (iStack_10 >> 1);
    }
    *(undefined2 *)0x52c = 0;
    if (iStack_14 == -1) {
      iStack_10 = -iStack_10;
    }
    iStack_a = *_piStack_e + iStack_10;
    if (iStack_a < 0) {
      iStack_a = 0;
    }
    if (0xfff < iStack_a) {
      iStack_a = 0xfff;
    }
    iVar2 = *_piStack_e;
    *_piStack_e = iStack_a;
    if (iStack_a != iVar2) {
      puVar1 = (uint *)(*(int *)0x570 * 2 + 0x550);
      *puVar1 = *puVar1 | 0x100;
    }
  }
  return;
}



/* apply_pending_position_delta (confidence: Mechanism confirmed)
   
   Evidence: Reads a signed pending adjustment from `[0x52C]` (always reset to 0 once consumed - a
   one-shot "apply this" flag, likely fed by a front-panel encoder/knob), clamps the resulting
   position to `[0,0xFFF]`, updates a stored position value via far ptr `[bp-0xC]` and computes the
   delta; if the position actually moved, flags bit `0x100` of `[([0x570]<<1)+0x550]` - same
   per-item flag table and channel/item index `[0x570]` that `sync_shift_register_output` uses - the
   scale-position-adjustment half of that same subsystem */

void apply_pending_position_delta(void)

{
  uint *puVar1;
  int *piVar2;
  int iVar3;
  int unaff_BP;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  if (*(int *)0x52c < 0) {
    ((char *)s_ACQ_RAM_even_000f_fef8 + 6 + unaff_BP)[0] = -1;
    ((char *)s_ACQ_RAM_even_000f_fef8 + 6 + unaff_BP)[1] = -1;
    *(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP) = -*(int *)0x52c;
  }
  else {
    ((char *)s_ACQ_RAM_even_000f_fef8 + 6 + unaff_BP)[0] = '\x01';
    ((char *)s_ACQ_RAM_even_000f_fef8 + 6 + unaff_BP)[1] = '\0';
    *(undefined2 *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP) = *(undefined2 *)0x52c;
  }
  if (0x13 < *(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP)) {
    if (*(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP) < 0x1e) {
      ((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP)[0] = '\x01';
      ((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP)[1] = '\0';
    }
    else if (*(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP) < 0x46) {
      *(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP) =
           *(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP) + -0x1d;
    }
    else if (0x78 < *(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP)) {
      *(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP) =
           *(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP) +
           (*(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP) >> 1);
    }
    *(undefined2 *)0x52c = 0;
    if (*(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 6 + unaff_BP) == -1) {
      *(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP) =
           -*(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP);
    }
    iVar3 = **(int **)((char *)s_ACQ_RAM_even_000f_fef8 + 0xc + unaff_BP) +
            *(int *)((char *)s_ACQ_RAM_even_000f_fef8 + 10 + unaff_BP);
    *(int *)(unaff_BP + -8) = iVar3;
    if (iVar3 < 0) {
      *(undefined2 *)(unaff_BP + -8) = 0;
    }
    if (0xfff < *(int *)(unaff_BP + -8)) {
      *(undefined2 *)(unaff_BP + -8) = 0xfff;
    }
    piVar2 = *(int **)((char *)s_ACQ_RAM_even_000f_fef8 + 0xc + unaff_BP);
    *(int *)(unaff_BP + -0x10) = *(int *)(unaff_BP + -8) - *piVar2;
    *piVar2 = *(int *)(unaff_BP + -8);
    if (*(int *)(unaff_BP + -0x10) != 0) {
      puVar1 = (uint *)(*(int *)0x570 * 2 + 0x550);
      *puVar1 = *puVar1 | 0x100;
    }
  }
  return;
}



void __cdecl16far FUN_000f_76bc(void)

{
  byte bVar1;
  int iVar2;
  undefined2 unaff_CS;
  undefined2 uVar3;
  undefined2 unaff_DS;
  int *piStack_14;
  int iStack_e;
  
  _piStack_14 = (int *)CONCAT22(unaff_DS,(int *)&DAT_000f_0484);
  if (((*(byte *)0x4f7 & 1) == 1) && ((*(byte *)0x4f8 & 1) == 1)) {
    unaff_CS = 0xef44;
    iStack_e = func_0x000ef819();
    *(undefined2 *)0x604 = *(undefined2 *)0x752;
    *(undefined2 *)0x606 = 200;
  }
  else if (((*(byte *)0x4f7 & 0x40) == 0x40) && ((*(byte *)0x4f8 & 0x40) == 0x40)) {
    unaff_CS = 0xef44;
    iStack_e = func_0x000ef8c8();
    *(undefined2 *)0x604 = *(undefined2 *)0x752;
    *(undefined2 *)0x606 = 200;
  }
  else if (((*(byte *)0x4f7 & 1) == 1) && ((*(byte *)0x4f8 & 1) == 0)) {
    unaff_CS = 0xef44;
    iStack_e = func_0x000ef819();
  }
  uVar3 = unaff_CS;
  if (((*(byte *)0x4f7 & 0x40) == 0x40) && ((*(byte *)0x4f8 & 0x40) == 0)) {
    uVar3 = 0xef44;
    iStack_e = func_0x000ef8c8(unaff_CS,*(undefined2 *)0x604,*(undefined2 *)0x752);
  }
  bVar1 = *(byte *)&DAT_000f_0490;
  iVar2 = func_0x000ee879(uVar3,bVar1);
  iVar2 = iVar2 / 2;
  if (iStack_e < iVar2) {
    iStack_e = iVar2;
  }
  if (0xfff - iVar2 < iStack_e) {
    iStack_e = 0xfff - iVar2;
  }
  iVar2 = *_piStack_14;
  *_piStack_14 = iStack_e;
  if (iStack_e != iVar2) {
    *(uint *)&DAT_000f_046a = *(uint *)&DAT_000f_046a | 0x100;
  }
  if ((bVar1 & 2) == 0) {
    *(int *)0x590 = iStack_e;
    *(uint *)0x55c = *(uint *)0x55c | 0x100;
  }
  else {
    *(int *)0x5c8 = iStack_e;
    *(uint *)0x560 = *(uint *)0x560 | 0x100;
  }
  return;
}



undefined * __stdcall16far FUN_000f_7819(uint param_1,uint param_2)

{
  undefined2 unaff_DS;
  int iStack_c;
  char *pcStack_a;
  
  if (param_2 == param_1) {
    if ((*(byte *)0x18c & 8) == 0) {
      pcStack_a = (char *)s_ACQ_RAM_even_000f_fef8 + 4 + *(int *)&DAT_000f_0484;
    }
    else {
      pcStack_a = (char *)(*(int *)&DAT_000f_0484 + -2);
    }
    *(undefined2 *)0x604 = *(undefined2 *)0x752;
  }
  else {
    if (param_2 < param_1) {
      iStack_c = -1 - param_1;
    }
    else {
      iStack_c = -param_1;
    }
    iStack_c = param_2 + iStack_c;
    if (*(int *)0x606 < iStack_c) {
      if ((*(byte *)0x18c & 8) == 0) {
        pcStack_a = (char *)(*(int *)&DAT_000f_0484 + (iStack_c - *(int *)0x606) * -2);
      }
      else {
        pcStack_a = (char *)(*(int *)&DAT_000f_0484 - (iStack_c - *(int *)0x606) / 4);
      }
      *(undefined2 *)0x606 = 0;
      *(undefined2 *)0x604 = *(undefined2 *)0x752;
    }
    else {
      pcStack_a = (char *)*(undefined2 *)&DAT_000f_0484;
    }
  }
  return pcStack_a;
}



int __stdcall16far FUN_000f_78c8(uint param_1,uint param_2)

{
  undefined2 unaff_DS;
  int iStack_c;
  int iStack_a;
  
  if (param_2 == param_1) {
    if ((*(byte *)0x18c & 8) == 0) {
      iStack_a = *(int *)&DAT_000f_0484 + 0x14;
    }
    else {
      iStack_a = *(int *)&DAT_000f_0484 + 2;
    }
    *(undefined2 *)0x604 = *(undefined2 *)0x752;
  }
  else {
    if (param_2 < param_1) {
      iStack_c = -1 - param_1;
    }
    else {
      iStack_c = -param_1;
    }
    iStack_c = param_2 + iStack_c;
    if (*(int *)0x606 < iStack_c) {
      if ((*(byte *)0x18c & 8) == 0) {
        iStack_a = *(int *)&DAT_000f_0484 + (iStack_c - *(int *)0x606) * 2;
      }
      else {
        iStack_a = *(int *)&DAT_000f_0484 + (iStack_c - *(int *)0x606) / 4;
      }
      *(undefined2 *)0x606 = 0;
      *(undefined2 *)0x604 = *(undefined2 *)0x752;
    }
    else {
      iStack_a = *(int *)&DAT_000f_0484;
    }
  }
  return iStack_a;
}



void __cdecl16far FUN_000f_7a3c(void)

{
  byte bVar1;
  uint uVar2;
  undefined2 unaff_DS;
  
  bVar1 = *(byte *)&DAT_000f_1a85;
  if ((*(byte *)0x4ef & 0x40) == 0) {
    uVar2 = *(byte *)0x4f3 & 0x39;
    if (uVar2 == 1) {
      *(undefined *)&DAT_000f_1a85 = 3;
    }
    else if (uVar2 == 8) {
      *(undefined *)&DAT_000f_1a85 = 7;
    }
    else if ((((uVar2 == 0x10) || (uVar2 == 0x11)) || (uVar2 == 0x18)) || (uVar2 == 0x19)) {
      *(undefined *)&DAT_000f_1a85 = 2;
    }
    else if (((uVar2 == 0x20) || (uVar2 == 0x21)) || ((uVar2 == 0x28 || (uVar2 == 0x29)))) {
      *(undefined *)&DAT_000f_1a85 = 1;
    }
    else if (uVar2 == 0x30) {
      *(undefined1 *)&DAT_000f_1a85 = 0;
    }
    else {
      *(undefined2 *)0x534 = 0xcc;
      *(undefined2 *)&reset_all_channel_plot_caches = 0xef97;
      *(uint *)0x538 = uVar2;
      *(undefined *)&DAT_000f_1a85 = 1;
    }
  }
  else {
    *(undefined *)&DAT_000f_1a85 = 8;
  }
  if (*(byte *)&DAT_000f_1a85 != bVar1) {
    *(uint *)0x53a = *(uint *)0x53a | 0xc;
    if (((*(char *)&DAT_000f_1a85 == '\0') || (bVar1 == 0)) ||
       ((*(char *)&DAT_000f_1a85 == '\b' || (bVar1 == 8)))) {
      *(byte *)0x601 = *(byte *)0x601 | 0x11;
    }
    if (((*(byte *)&DAT_000f_1a85 ^ bVar1) & 1) == 1) {
      *(byte *)0x601 = *(byte *)0x601 | 1;
    }
    if (((*(byte *)&DAT_000f_1a85 ^ bVar1) & 2) == 2) {
      *(byte *)0x601 = *(byte *)0x601 | 0x10;
    }
    if (((*(byte *)&DAT_000f_1a85 ^ bVar1) & 8) == 8) {
      *(uint *)0x53a = *(uint *)0x53a | 0x70;
    }
  }
  return;
}



void __cdecl16far FUN_000f_7ba5(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  *(undefined *)&DAT_000f_1a8a = 1;
  *(undefined2 *)0x53e = *(undefined2 *)0x752;
  *(undefined1 *)0x542 = 0;
  *(undefined1 *)0x467 = 0;
  *(undefined1 *)0x46a = 1;
  if (*(char *)&DAT_000f_1a86 == '\0') {
    *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 | 4;
    unaff_CS = 0xe6a8;
    func_0x000e6ce8();
  }
  else {
    *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 | 4;
  }
  func_0x000e7416(unaff_CS);
  func_0x000e6c85(0xe741);
  func_0x000e6f5c(0xe6a8);
  *(undefined1 *)0x3e2 = 0x73;
  func_0x000f4ce8(0xe6f5);
  func_0x000f5f56(0xf4ce);
  return;
}



void __cdecl16far FUN_000f_7c00(void)

{
  byte bVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (*(char *)0x467 != '\0') {
    *(undefined1 *)0x467 = 0;
    *(undefined1 *)0x468 = 1;
    *(undefined1 *)0x46a = 1;
  }
  bVar1 = *(byte *)0x4e9 & 99;
  if (((*(byte *)0x4e9 & 99) != 0) && (bVar1 == *(byte *)0x542)) {
    if ((bVar1 == 1) || (((bVar1 == 2 || (bVar1 == 0x20)) || (bVar1 == 0x40)))) {
      *(int *)0x540 = *(int *)0x53e + 0x180;
      if (*(uint *)0x53e < *(uint *)0x540) {
        if ((*(uint *)0x540 < *(uint *)0x752) || (*(uint *)0x752 < *(uint *)0x53e)) {
          *(byte *)0x4e8 = *(byte *)0x4e8 | bVar1;
          *(undefined2 *)0x532 = 1;
          *(undefined2 *)0x53e = *(undefined2 *)0x752;
        }
      }
      else if ((*(uint *)0x540 < *(uint *)0x752) && (*(uint *)0x752 < *(uint *)0x53e)) {
        *(byte *)0x4e8 = *(byte *)0x4e8 | bVar1;
        *(undefined2 *)0x532 = 1;
        *(undefined2 *)0x53e = *(undefined2 *)0x752;
      }
    }
    else {
      *(undefined1 *)0x542 = 0;
    }
  }
  bVar1 = *(byte *)0x4e7 & *(byte *)0x4e8;
  if ((bVar1 & 99) == 0) {
    if (((*(int *)0x532 != 0) || ((*(byte *)0x466 & 0x20) != 0)) || (*(char *)0x468 != '\0')) {
      *(undefined1 *)0x3e2 = 0x78;
      func_0x000f4ce8();
      func_0x000f5f56(0xf4ce);
    }
  }
  else {
    *(undefined2 *)0x53e = *(undefined2 *)0x752;
    *(byte *)0x542 = bVar1 & 99;
    if ((bVar1 & 0x20) != 0) {
      *(undefined1 *)0x3e2 = 0x75;
      func_0x000f4ce8();
      unaff_CS = 0xf5f5;
      func_0x000f5f56(0xf4ce);
    }
    if ((bVar1 & 1) != 0) {
      *(undefined1 *)0x3e2 = 100;
      func_0x000f4ce8(unaff_CS);
      unaff_CS = 0xf5f5;
      func_0x000f5f56(0xf4ce);
    }
    if ((bVar1 & 2) != 0) {
      *(undefined1 *)0x3e2 = 0x6c;
      func_0x000f4ce8(unaff_CS);
      unaff_CS = 0xf5f5;
      func_0x000f5f56(0xf4ce);
    }
    if ((bVar1 & 0x40) != 0) {
      *(undefined1 *)0x3e2 = 0x72;
      func_0x000f4ce8(unaff_CS);
      func_0x000f5f56(0xf4ce);
    }
  }
  if (*(char *)0x468 != '\0') {
    *(undefined1 *)0x468 = 0;
  }
  return;
}



void __cdecl16far FUN_000f_7d99(void)

{
  undefined2 unaff_DS;
  
  func_0x000f5146();
  func_0x000f408e(0xf4ce);
  *(undefined1 *)&DAT_000f_1a8a = 0;
  *(undefined2 *)0x53a = 0xffff;
  *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 & 0xfb;
  func_0x000e6c85(0xf408);
  return;
}



void __stdcall16far FUN_000f_7dc2(int param_1,int param_2)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  byte bVar5;
  int iVar6;
  undefined2 unaff_DS;
  int *piStack_14;
  int iStack_a;
  
  iVar6 = param_2 * 8;
  _piStack_14 = (int *)CONCAT22(unaff_DS,(int *)(iVar6 + 0x4fe));
  iVar2 = *(int *)((int)*(undefined4 *)&DAT_000f_1c64 + 2);
  iVar3 = *(int *)*(undefined4 *)&DAT_000f_1c64;
  iVar4 = *(int *)((int)*(undefined4 *)&DAT_000f_1c64 + 4);
  bVar5 = (byte)iVar2;
  if (iVar2 == 0) {
    *(int *)(iVar6 + 0x500) = param_1;
    iStack_a = param_1;
  }
  else {
    *(int *)(iVar6 + 0x500) =
         (*(int *)(iVar6 + 0x500) - (*(int *)(iVar6 + 0x500) >> (bVar5 & 0x1f))) + param_1;
    iStack_a = *(int *)(iVar6 + 0x500) >> (bVar5 & 0x1f);
  }
  if (*_piStack_14 + iVar3 < param_1) {
    *_piStack_14 = param_1;
    *(undefined1 *)(iVar6 + 0x502) = 1;
    *(undefined1 *)(iVar6 + 0x503) = 1;
    *(undefined1 *)(iVar6 + 0x504) = 0;
    *(int *)(iVar6 + 0x500) = param_1 << (bVar5 & 0x1f);
    *(undefined2 *)0x532 = 1;
  }
  else if (param_1 < *_piStack_14 - iVar3) {
    *_piStack_14 = param_1;
    *(undefined1 *)(iVar6 + 0x502) = 2;
    *(undefined1 *)(iVar6 + 0x503) = 2;
    *(undefined1 *)(iVar6 + 0x504) = 0;
    *(int *)(iVar6 + 0x500) = param_1 << (bVar5 & 0x1f);
    *(undefined2 *)0x532 = 1;
  }
  else {
    cVar1 = *(char *)(iVar6 + 0x503);
    if (cVar1 == '\0') {
      if (0 < iVar2) {
        if (*_piStack_14 + 1 < iStack_a) {
          *_piStack_14 = iStack_a;
          *(undefined1 *)(iVar6 + 0x502) = 1;
          *(undefined2 *)0x532 = 1;
        }
        else if (iStack_a < *_piStack_14 + -1) {
          *_piStack_14 = iStack_a;
          *(undefined1 *)(iVar6 + 0x502) = 2;
          *(undefined2 *)0x532 = 1;
        }
        else if (*(char *)(iVar6 + 0x502) != '\0') {
          *(undefined1 *)(iVar6 + 0x502) = 0;
        }
      }
    }
    else if (cVar1 == '\x01') {
      if (*_piStack_14 < param_1) {
        *_piStack_14 = param_1;
        *(undefined1 *)(iVar6 + 0x502) = 1;
        *(undefined2 *)0x532 = 1;
        *(undefined1 *)(iVar6 + 0x504) = 0;
      }
      else {
        *(undefined1 *)(iVar6 + 0x502) = 0;
        *(char *)(iVar6 + 0x504) = *(char *)(iVar6 + 0x504) + '\x01';
        if (iVar4 < (int)(uint)*(byte *)(iVar6 + 0x504)) {
          *(undefined1 *)(iVar6 + 0x504) = 0;
          *(undefined1 *)(iVar6 + 0x503) = 0;
        }
      }
    }
    else if (cVar1 == '\x02') {
      if (param_1 < *_piStack_14) {
        *_piStack_14 = param_1;
        *(undefined1 *)(iVar6 + 0x502) = 2;
        *(undefined2 *)0x532 = 1;
        *(undefined1 *)(iVar6 + 0x504) = 0;
      }
      else {
        *(undefined1 *)(iVar6 + 0x502) = 0;
        *(char *)(iVar6 + 0x504) = *(char *)(iVar6 + 0x504) + '\x01';
        if (iVar4 < (int)(uint)*(byte *)(iVar6 + 0x504)) {
          *(undefined1 *)(iVar6 + 0x504) = 0;
          *(undefined1 *)(iVar6 + 0x503) = 0;
        }
      }
    }
  }
  return;
}



void __stdcall16far FUN_000f_802b(void)

{
  func_0x000fb529();
  return;
}



/* build_print_record_sequence (confidence: Confirmed)
   
   Evidence: Builds a row of print/plot-job records via repeated `build_print_record_3532` calls,
   each reading from a growing offset into the templates loaded by `load_print_record_templates`
   (`[0x1E8]+0`, `+0x22`, ...) and writing to a growing offset into the destination array at
   `[0x1DDC]+0x5A` (`+9` per record) */

void build_print_record_sequence(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  int iStack_12;
  int iStack_10;
  
  uVar2 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1cec >> 0x10);
  iVar1 = (int)*(undefined4 *)&DAT_000f_1cec;
  uVar4 = 0x10;
  uVar3 = 0x10;
  func_0x000f8272();
  func_0x000f8272(0xf80a,iVar1 + 99,uVar2,(int)*(undefined4 *)0x1e8 + 0x22,
                  (int)((ulong)*(undefined4 *)0x1e8 >> 0x10),0x30,0,0,0x10,0x10,uVar3,uVar4);
  func_0x000f8272(0xf80a,iVar1 + 0x6c,uVar2,(int)*(undefined4 *)0x1e8,
                  (int)((ulong)*(undefined4 *)0x1e8 >> 0x10),0x30,0,0,0x10,0x10);
  func_0x000f8272(0xf80a,iVar1 + 0x75,uVar2,(int)*(undefined4 *)0x1e8 + 0x22,
                  (int)((ulong)*(undefined4 *)0x1e8 >> 0x10),0x30,0,0,0x10,0x10);
  iStack_10 = 0x1f1;
  uVar3 = 0xf80a;
  for (iStack_12 = 0; iStack_12 < 8; iStack_12 = iStack_12 + 1) {
    func_0x000fbc09(uVar3,iVar1 + 0x76,uVar2,iStack_10 + 2,unaff_DS,4);
    iStack_10 = iStack_10 + 6;
    uVar3 = 0xfbbe;
  }
  uVar2 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1cec >> 0x10);
  iVar1 = (int)*(undefined4 *)&DAT_000f_1cec;
  func_0x000f8272(uVar3,iVar1 + 0x7e,uVar2,(int)*(undefined4 *)0x1e8 + 0x44,
                  (int)((ulong)*(undefined4 *)0x1e8 >> 0x10),0x30,0,0,0x10,0x10);
  func_0x000f8272(0xf80a,iVar1 + 0x87,uVar2,(int)*(undefined4 *)0x1e8 + 0x66,
                  (int)((ulong)*(undefined4 *)0x1e8 >> 0x10),0x30,0,0,0x10,0x10);
  func_0x000f8272(0xf80a,iVar1 + 0x90,uVar2,(int)*(undefined4 *)0x1e8 + 0x44,
                  (int)((ulong)*(undefined4 *)0x1e8 >> 0x10),0x30,0,0,0x10,0x10);
  func_0x000f8272(0xf80a,iVar1 + 0x99,uVar2,(int)*(undefined4 *)0x1e8 + 0x88,
                  (int)((ulong)*(undefined4 *)0x1e8 >> 0x10),0x30,0,0,0x10,0x10);
  return;
}



/* load_print_record_templates (confidence: Confirmed)
   
   Evidence: Copies 2 fixed `0xAA` (170)-byte compiled-in template blocks (from
   `[0x1C9C]`/`[0x1CA0]`) via `memcpy_far` into the scratch far-pointer slots `[0x1E8]`/`[0x1EC]` -
   loads the source templates `build_print_record_sequence`'s `build_print_record_3532` calls read
   from */

void __cdecl16far load_print_record_templates(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  uVar2 = 0xaa;
  uVar1 = (undefined2)((ulong)*(undefined4 *)0x1e8 >> 0x10);
  func_0x000fbc09();
  func_0x000fbc09(0xfbbe,(int)*(undefined4 *)&DAT_000f_1bb0,
                  (int)((ulong)*(undefined4 *)&DAT_000f_1bb0 >> 0x10),(int)*(undefined4 *)0x1ec,
                  (int)((ulong)*(undefined4 *)0x1ec >> 0x10),0xaa,uVar1,uVar2);
  return;
}



/* build_print_record_3532 (confidence: Mechanism confirmed)
   
   Evidence: Takes many parameters and packs them into a record via
   `array_index_16`/`pack_low5_bits`/`set_position_record_3532`/`copy_word_far` - `160-3532`'s
   parallel implementation of `build_print_record` */

void __stdcall16far
build_print_record_3532
          (undefined1 *param_1,undefined4 param_2,undefined1 param_3,undefined2 param_4,
          undefined2 param_5,undefined1 param_6,byte param_7)

{
  uint uVar1;
  undefined1 *puVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 local_a [4];
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  puVar2 = (undefined1 *)param_1;
  *param_1 = param_3;
  uVar1 = (uint)param_7;
  uVar4 = uVar3;
  func_0x000fbc69();
  func_0x000fbc69(0xfbc2,puVar2 + 3,uVar3,param_6,uVar4,uVar1);
  func_0x000fbc84(0xfbc2,puVar2 + 4,uVar3,param_4);
  func_0x000fbc84(0xfbc2,puVar2 + 2,uVar3,param_5);
  local_a[0] = func_0x000fbc2f(0xfbc2,param_2);
  func_0x000fbc4d(0xfbc2,local_a);
  puVar2[7] = puVar2[5];
  puVar2[8] = puVar2[6];
  return;
}



/* read_acq_sample_with_wrap (confidence: Mechanism confirmed)
   
   Evidence: `(far_ptr record, type_idx, position)` - resolves `position` against a channel's
   circular acquisition buffer (bound `[0x2A]`, end pointer `[record+0x68]`), special-casing
   `type_idx==0` and `==4`/count `==8`; out-of-`[0,0x2000]` range sets clip flag `[0x3B0]` and
   returns 0; else reads the byte at the resolved offset, packing it as a 4-bit nibble when
   `[chan*16+0x18C]` bit `0x10` is set (matches the nibble-packing step preceding
   `convert_sample_value`) - called 4x consecutively from `0xF4186` computing
   `position(idx+1)-position(idx)` deltas for adjacent-sample line drawing */

int __stdcall16far
read_acq_sample_with_wrap(word record_off,word record_seg,word type_idx,word position)

{
  int iVar1;
  byte *pbVar2;
  int iVar3;
  undefined2 unaff_DS;
  int iStack_14;
  undefined4 uStack_12;
  uint uStack_c;
  
  iVar3 = (int)type_idx >> 1;
  if ((*(byte *)(type_idx * 0x14 + 0xa7) & 1) != 0) {
    iVar1 = *(int *)(iVar3 * 0x10 + 0x194);
    if (iVar3 == 0) {
      if (*(int *)0x2a < (int)((*(int *)(type_idx * 10 + 0x68) - iVar1) + position)) {
        position = (((*(int *)(type_idx * 10 + 0x68) - iVar1) + position) - *(int *)0x2a) - 1;
      }
      else if ((int)((*(int *)(type_idx * 10 + 0x68) - iVar1) + position) < 0) {
        position = (*(int *)(type_idx * 10 + 0x68) - iVar1) + position + *(int *)0x2a + 1;
      }
      else {
        position = (*(int *)(type_idx * 10 + 0x68) - iVar1) + position;
      }
    }
    else if (iVar3 == 4) {
      if (type_idx == 8) {
        iStack_14 = *(int *)0x68;
      }
      else {
        iStack_14 = *(int *)0x72;
      }
      iStack_14 = iStack_14 >> 2;
      if (*(int *)(code *)scale_and_plot_point + 1 < (int)((iStack_14 - iVar1) + position)) {
        position = ((iStack_14 - iVar1) + position) - *(int *)(code *)scale_and_plot_point;
      }
      else if ((int)((iStack_14 - iVar1) + position) < 0) {
        position = (iStack_14 - iVar1) + position + *(int *)(code *)scale_and_plot_point + 2;
      }
      else {
        position = (iStack_14 - iVar1) + position;
      }
    }
  }
  if (((int)position < 0) || (0x2000 < (int)position)) {
    *(undefined1 *)0x3b0 = 1;
    iVar3 = 0;
  }
  else {
    pbVar2 = (byte *)(record_off + position + *(int *)(type_idx * 4 + 0x3c));
    uStack_12 = (byte *)CONCAT22(record_seg,pbVar2);
    if ((*(byte *)(iVar3 * 0x10 + 0x18c) & 0x10) == 0) {
      uStack_c = (uint)*uStack_12 << 4;
    }
    else {
      uStack_c = (uint)pbVar2[1] << 4 | (int)(uint)*uStack_12 >> 4;
    }
    iVar3 = (int)uStack_c >> 2;
  }
  return iVar3;
}



void __stdcall16far FUN_000f_84e2(uint param_1,int param_2,int param_3)

{
  byte bVar1;
  undefined1 *puVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  int iStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined1 *puStack_c;
  
  uStack_10 = (undefined1 *)
              CONCAT22((int)((ulong)*(undefined4 *)&DAT_000f_1ba4 >> 0x10),
                       (undefined1 *)
                       ((int)*(undefined4 *)&DAT_000f_1ba4 + param_1 * 8 + param_2 + 4));
  if ((param_1 & 1) != 0) {
    param_2 = param_2 + 4;
    param_3 = param_3 + 4;
  }
  _puStack_c = (undefined1 *)CONCAT22(unaff_DS,(undefined1 *)(param_2 * 6 + 0x1f1));
  bVar1 = *(byte *)(param_1 * 0x14 + 0xb1);
  for (iStack_18 = param_2; iStack_18 <= param_3; iStack_18 = iStack_18 + 1) {
    uVar3 = (undefined2)((ulong)_puStack_c >> 0x10);
    puVar2 = (undefined1 *)_puStack_c;
    puVar2[1] = puVar2[1] | 1;
    *_puStack_c = *uStack_10;
    uStack_14 = (byte *)CONCAT22(uVar3,puVar2 + 2);
    *uStack_14 = bVar1 & 0x1f;
    _puStack_c = (undefined1 *)CONCAT22(uVar3,puVar2 + 6);
    uStack_10 = (undefined1 *)CONCAT22(uStack_10._2_2_,(undefined1 *)uStack_10 + 1);
  }
  return;
}



int __stdcall16far FUN_000f_858e(int param_1)

{
  int iVar1;
  undefined2 unaff_DS;
  int iStack_e;
  uint uStack_c;
  
  uStack_c = (uint)*(byte *)&DAT_000f_1a77;
  iVar1 = param_1 * 2;
  if ((int)(*(byte *)&DAT_000f_1a74 - uStack_c) < 0xd) {
    iStack_e = 2;
    for (; (int)uStack_c < (int)(uint)*(byte *)&DAT_000f_1a74; uStack_c = uStack_c + 1) {
      if ((int)(uStack_c + 1) % 3 == 0) {
        iStack_e = iStack_e * 5 >> 1;
      }
      else {
        iStack_e = iStack_e << 1;
      }
    }
    if ((iStack_e < iVar1) && (iVar1 != 0)) {
      iStack_e = (iVar1 + (iStack_e >> 1)) / iStack_e;
    }
    else {
      iStack_e = 1;
    }
  }
  else {
    iStack_e = 1;
  }
  return iStack_e;
}



void __cdecl16far FUN_000f_8624(void)

{
  undefined2 uVar1;
  uint uVar2;
  int iVar3;
  char *pcVar4;
  char *pcVar5;
  undefined2 unaff_DS;
  undefined2 uVar6;
  undefined2 uVar7;
  char cStack_25;
  uint uStack_1c;
  int iStack_1a;
  int iStack_18;
  int iStack_16;
  int iStack_10;
  int iStack_c;
  
  uVar7 = 7;
  func_0x000e7298();
  pcVar4 = (char *)s_SREF_LOCKED_000f_f93a + 6;
  func_0x000fa30e(0xe723,uVar7);
  iStack_16 = *(int *)0x570;
  if ((*(int *)(iStack_16 * 2 + 0x55a) == 0) || (*(char *)&DAT_000f_1a93 != '\x1e')) {
    iStack_1a = 0;
  }
  else {
    *(uint *)0x548 = *(uint *)0x548 | 0x88c3;
    iStack_1a = iStack_16;
  }
  for (iStack_18 = iStack_1a; iStack_18 < 5; iStack_18 = iStack_18 + 1) {
    if (*(int *)(iStack_18 * 2 + 0x55a) != 0) {
      func_0x000e5d2f(pcVar4);
      iVar3 = iStack_18 * 2;
      uStack_1c = *(uint *)(iVar3 + 0x55a);
      *(undefined2 *)0x3ae = *(undefined2 *)(iVar3 + 0x55a);
      *(undefined2 *)(iVar3 + 0x55a) = 0;
      pcVar4 = (char *)0xe5d1;
      func_0x000e5d28(0xe5d1);
      if ((uStack_1c & 0x9f0) != 0) {
        if ((*(uint *)0x22 & 0x2000) != 0) {
          *(uint *)0x22 = *(uint *)0x22 & 0xdfff;
          *(undefined *)&DAT_000f_0490 = *(undefined *)&DAT_000f_0474;
          *(undefined1 *)0x58e = *(undefined *)&DAT_000f_0474;
        }
        pcVar4 = (char *)0xeda2;
        uVar2 = func_0x000eda26(0xe5d1,iStack_18,uStack_1c);
        uStack_1c = uStack_1c | uVar2;
      }
      pcVar5 = pcVar4;
      if ((uStack_1c & 0x1f) != 0) {
        pcVar5 = (char *)0xf862;
        func_0x000f88f2(pcVar4,iStack_18,uStack_1c);
        *(uint *)0x548 = *(uint *)0x548 | 0x8000;
      }
      pcVar4 = pcVar5;
      if ((uStack_1c & 0x40) != 0) {
        pcVar4 = (char *)0xf862;
        func_0x000f8a78(pcVar5);
        *(uint *)0x548 = *(uint *)0x548 | 0x8000;
      }
      iVar3 = iStack_18 * 0x1c;
      iStack_c = iVar3 + 0x574;
      uVar7 = *(undefined2 *)(iVar3 + 0x576);
      uVar1 = *(undefined2 *)(iVar3 + 0x584);
      if (((*(uint *)0x186 & 0x40) == 0) || (iStack_18 != 0)) {
        iStack_10 = iStack_18 * 0x10 + 0x18c;
      }
      else {
        iStack_18 = 4;
        iStack_10 = 0x1cc;
        iStack_c = 0x5e4;
        if (iStack_16 == 0) {
          iStack_16 = 4;
        }
        cStack_25 = '\x01';
      }
      pcVar5 = pcVar4;
      if ((uStack_1c & 0xa00) != 0) {
        pcVar5 = "\x10<";
        func_0x000f79d0(pcVar4,iStack_10,unaff_DS,iStack_c,unaff_DS,iStack_18,
                        *(undefined2 *)(iVar3 + 0x578));
        uStack_1c = uStack_1c | 0x100;
      }
      if (((uStack_1c & 0x9c0) != 0) && (iStack_16 == iStack_18)) {
        uVar6 = *(undefined2 *)(iStack_c + 8);
        func_0x000fafc5(pcVar5,iStack_10,unaff_DS,iStack_c,unaff_DS,0x1f1);
        pcVar5 = (char *)s_TEKTRONIX_2230_000f_fa07 + 5;
        func_0x000fafc5((char *)s_TEKTRONIX_2230_000f_fa07 + 5,iStack_10,unaff_DS,iStack_c + 0xe,
                        unaff_DS,0x1f7,unaff_DS,uVar1,*(undefined2 *)(iStack_c + 8),uVar7,uVar6);
      }
      if (((uStack_1c & 0xa40) != 0) && (iStack_16 == iStack_18)) {
        func_0x000fb0d5(pcVar5,iStack_18);
        func_0x000f7e2a((char *)s_TEKTRONIX_2230_000f_fa07 + 5,iStack_18);
        func_0x000f7f72(0xf7bf,iStack_18);
        func_0x000f84e2(0xf7bf,iStack_18 * 2,0,3);
        pcVar5 = "";
        func_0x000f84e2(0xf80a,iStack_18 * 2 + 1,0,3);
      }
      pcVar4 = pcVar5;
      if (((uStack_1c & 0x1f) != 0) && (iStack_16 == iStack_18)) {
        pcVar4 = "";
        func_0x000f7f72(pcVar5,iStack_18);
      }
      *(uint *)0x186 = *(uint *)0x186 | 2;
      if (*(char *)&DAT_000f_1a93 == '\x1e') break;
      if (cStack_25 != '\0') {
        iStack_18 = 1;
        cStack_25 = '\0';
      }
    }
  }
  *(undefined2 *)0x3ae = 0;
  if ((*(uint *)0x186 & 4) != 0) {
    *(uint *)0x186 = *(uint *)0x186 & 0xfffb;
  }
  *(uint *)0x186 = *(uint *)0x186 | 0x10;
  func_0x000e7309(pcVar4,0,7);
  return;
}



void __stdcall16far FUN_000f_88f2(int param_1,uint param_2)

{
  byte bVar1;
  uint uVar2;
  undefined2 unaff_CS;
  undefined2 uVar3;
  undefined2 unaff_DS;
  undefined2 uStack_12;
  undefined2 uStack_e;
  
  uVar2 = param_1 << 1;
  _uStack_e = (byte *)CONCAT22(unaff_DS,(byte *)(param_1 * 0x10 + 0x18c));
  if ((param_2 & 0x10) != 0) {
    bVar1 = *_uStack_e;
    unaff_CS = 0xf7ae;
    func_0x000f7aea();
    if (param_1 == 0) {
      if ((bVar1 & 8) == 0) {
        *(byte *)0x1cc = *(byte *)0x1cc & 0xf7;
      }
      else {
        *(byte *)0x1cc = *(byte *)0x1cc | 8;
      }
    }
  }
  if ((param_2 & 5) != 0) {
    func_0x000f6f4a(unaff_CS,uVar2 & 0xff);
    unaff_CS = 0xf80a;
    func_0x000f84e2(0xf6f4,uVar2 & 0xff,2,3);
  }
  if ((param_2 & 10) != 0) {
    func_0x000f6f4a(unaff_CS,(uVar2 & 0xff) + 1);
    unaff_CS = 0xf80a;
    func_0x000f84e2(0xf6f4,(uVar2 & 0xff) + 1,2,3);
  }
  if (*(int *)0x570 == param_1) {
    uVar3 = unaff_CS;
    if ((param_2 & 5) != 0) {
      uVar3 = 0xf80a;
      func_0x000f84e2(unaff_CS,uVar2 & 0xff,0,1);
    }
    unaff_CS = uVar3;
    if ((param_2 & 10) != 0) {
      unaff_CS = 0xf80a;
      func_0x000f84e2(uVar3,(uVar2 & 0xff) + 1,0,1);
    }
  }
  if ((param_1 == 0) && ((*_uStack_e & 1) != 0)) {
    _uStack_12 = (byte *)CONCAT22(unaff_DS,(byte *)0xa6);
    func_0x000fbc09(unaff_CS,0xb1);
    *(byte *)0x146 = *(byte *)0x146 | *_uStack_12 | 4;
    _uStack_12 = (byte *)CONCAT22(unaff_DS,(byte *)0xba);
    func_0x000fbc09(0xfbbe,0xc5,unaff_DS,0x165,unaff_DS,4);
    *(byte *)0x15a = *(byte *)0x15a | *_uStack_12 | 4;
  }
  return;
}



void __cdecl16far FUN_000f_8a78(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  byte *pbStack_12;
  byte bStack_d;
  byte *pbStack_c;
  
  _pbStack_c = (byte *)CONCAT22(unaff_DS,(byte *)0x18c);
  if ((*(byte *)0x18c & 0x40) == 0) {
    *(uint *)0x186 = *(uint *)0x186 & 0xffbf;
    uVar1 = 0;
    func_0x000f8e66();
    func_0x000f8e98((char *)s_This_message_will_disappear__000f_f7e2 + 0x14,4,uVar1);
  }
  else {
    *(uint *)0x186 = *(uint *)0x186 | 0x40;
    uVar1 = 4;
    func_0x000f8e66();
    *(byte *)0xb0 = *(byte *)0xb0 & 0xfe;
    *(byte *)0xa6 = *(byte *)0xa6 | 1;
    _pbStack_12 = (byte *)CONCAT22(unaff_DS,(byte *)0xba);
    *(byte *)0xc4 = *(byte *)0xc4 & 0xfe;
    *_pbStack_12 = *_pbStack_12 | 1;
  }
  if ((*(char *)&DAT_000f_1a86 != '\0') && ((*(uint *)0x186 & 0x40) != 0)) {
    bStack_d = *(byte *)0x20;
    if ((*_pbStack_c & 4) != 0) {
      bStack_d = bStack_d | 3;
    }
    func_0x000f69a5((char *)s_This_message_will_disappear__000f_f7e2 + 0x14,bStack_d,uVar1);
  }
  return;
}



undefined2 __cdecl16far FUN_000f_8b16(void)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined2 uStack_c;
  
  iVar1 = *(int *)0x24 >> (*(byte *)0x16f & 0x1f);
  uStack_c = 0;
  if (((*(byte *)0x20 & 1) != 0) && (iVar1 < *(int *)0x6c)) {
    *(byte *)0xa7 = *(byte *)0xa7 | 1;
    uStack_c = 1;
  }
  if (((*(byte *)0x20 & 2) != 0) && (iVar1 < *(int *)0x76)) {
    *(byte *)0xbb = *(byte *)0xbb | 1;
    uStack_c = 1;
  }
  return uStack_c;
}



void __cdecl16far FUN_000f_8b71(void)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_DS;
  
  *(uint *)0x186 = *(uint *)0x186 | 0x80;
  iVar1 = *(int *)0x190;
  iVar2 = *(int *)0x1d0;
  if ((*(byte *)0x20 & 1) != 0) {
    *(int *)0x98 = iVar1 + -1;
    *(int *)0xa0 = iVar2 + -1;
    *(byte *)0xa7 = *(byte *)0xa7 & 0xfe;
  }
  if ((*(byte *)0x20 & 2) != 0) {
    *(int *)0x9c = iVar1 + -1;
    *(int *)0xa4 = iVar2 + -1;
    *(byte *)0xbb = *(byte *)0xbb & 0xfe;
  }
  *(uint *)&DAT_000f_046a = *(uint *)&DAT_000f_046a | 0x340;
  return;
}



void __cdecl16far FUN_000f_8bcd(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  int iVar3;
  char *unaff_CS;
  undefined2 unaff_DS;
  uint uStack_10;
  
  iVar3 = *(int *)0x28 - *(int *)0x24 >> (*(byte *)0x16f & 0x1f);
  if (*(int *)0x2a < *(int *)0x26 + iVar3) {
    uStack_10 = ((*(int *)0x26 + iVar3) - *(int *)0x2a) - 1;
  }
  else if (*(int *)0x26 + iVar3 < 0) {
    uStack_10 = *(int *)0x26 + iVar3 + *(int *)0x2a + 1;
  }
  else {
    uStack_10 = *(int *)0x26 + iVar3;
  }
  uVar1 = *(undefined2 *)0x194;
  if ((*(byte *)0x18c & 0x20) == 0) {
    uStack_10 = uStack_10 & 0xfffe;
  }
  uVar2 = *(undefined2 *)(code *)FUN_000f_00e4;
  if ((*(byte *)0x20 & 1) != 0) {
    unaff_CS = (char *)s_Press_cursor_select_button_to_st_000f_f7bc + 5;
    func_0x000f8ca0();
    *(undefined2 *)0xa0 = uVar2;
  }
  if ((*(byte *)0x20 & 2) != 0) {
    func_0x000f8ca0(unaff_CS,1,uVar1,uStack_10);
    *(undefined2 *)0xa4 = uVar2;
  }
  *(uint *)0x186 = *(uint *)0x186 & 0xff5f;
  *(uint *)&DAT_000f_046a = *(uint *)&DAT_000f_046a | 0x200;
  return;
}



void __stdcall16far FUN_000f_8ca0(int param_1,undefined2 param_2,int param_3)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iStack_c;
  int iStack_a;
  
  iVar1 = param_1 * 10;
  *(int *)(iVar1 + 0x68) = param_3;
  if (*(int *)0x2a < *(int *)(iVar1 + 0x66) + 4) {
    iStack_c = (*(int *)(iVar1 + 0x66) - *(int *)0x2a) + 3;
  }
  else if (*(int *)(param_1 * 10 + 0x66) + 4 < 0) {
    iStack_c = *(int *)(param_1 * 10 + 0x66) + *(int *)0x2a + 5;
  }
  else {
    iStack_c = *(int *)(param_1 * 10 + 0x66) + 4;
  }
  uVar5 = CONCAT22((int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),
                   (int)*(undefined4 *)&DAT_000f_1b90 + iStack_c + *(int *)(param_1 * 4 + 0x3c));
  uVar4 = CONCAT22((int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),
                   (int)*(undefined4 *)&DAT_000f_1b90 +
                   (iStack_c >> 2) + *(int *)((param_1 + 8) * 4 + 0x3c));
  if (param_3 < iStack_c) {
    iStack_a = (*(int *)0x2a - iStack_c) + param_3 + 1;
  }
  else {
    iStack_a = param_3 - iStack_c;
  }
  if (iStack_c < param_3) {
    uVar3 = 0;
    uVar2 = 0;
    func_0x000fbbe9();
    func_0x000fbbe9(0xfbbe,(int)uVar4,(int)((ulong)uVar4 >> 0x10),iStack_a >> 2,0,0,uVar2,uVar3,
                    uVar4,uVar5);
  }
  else {
    iVar1 = *(int *)0x2a;
    uVar3 = 0;
    uVar2 = 0;
    func_0x000fbbe9();
    func_0x000fbbe9(0xfbbe,(int)*(undefined4 *)&DAT_000f_1b90 + *(int *)(param_1 * 4 + 0x3c),
                    (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),param_3,0,0,param_1 * 4,
                    uVar2,uVar3);
    func_0x000fbbe9(0xfbbe,(int)uVar4,(int)((ulong)uVar4 >> 0x10),(iVar1 + 1) - iStack_c >> 2,0,0);
    iVar1 = (param_1 + 8) * 4;
    func_0x000fbbe9(0xfbbe,(int)*(undefined4 *)&DAT_000f_1b90 + *(int *)(iVar1 + 0x3c),
                    (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),param_3 >> 2,0,0,iVar1);
  }
  *(undefined2 *)(param_1 * 4 + 0x98) = param_2;
  return;
}



/* WARNING: Unknown calling convention -- yet parameter storage is locked */
/* set_item_pair_active (confidence: Confirmed)
   
   Evidence: `(pair_index)` - activates both items in a pair (indices `pair_index*2` and
   `pair_index*2+1`) via two calls to `set_item_active_flag` */

void set_item_pair_active(word pair_index)

{
  int in_stack_00000004;
  undefined2 uVar1;
  
  uVar1 = 1;
  func_0x000f8ec8();
  func_0x000f8ec8((char *)s_This_message_will_disappear__000f_f7e2 + 0x14,in_stack_00000004 * 2 + 1,
                  1,uVar1);
  return;
}



/* deselect_item_pair (confidence: Confirmed)
   
   Evidence: `(pair_index)` - calls `set_item_active_flag(2*pair_index, 0)` and
   `set_item_active_flag(2*pair_index+1, 0)`, clearing the "selected" bit for both items of a pair
    */

void __stdcall16far deselect_item_pair(word pair_index)

{
  undefined2 uVar1;
  
  uVar1 = 0;
  func_0x000f8ec8();
  func_0x000f8ec8((char *)s_This_message_will_disappear__000f_f7e2 + 0x14,pair_index * 2 + 1,0,uVar1
                 );
  return;
}



/* set_item_active_flag (confidence: Mechanism confirmed; exact item type not confirmed)
   
   Evidence: `(item_index, set_flag)` - sets or clears a bit in a byte-per-item flag array
   (`[item_index+0x1BE2]`) and the matching field in a parallel 20-byte-per-item record array
   (offsets `0xA6`/`0xB0`) */

void __stdcall16far set_item_active_flag(word item_index,byte set_flag)

{
  byte *pbVar1;
  undefined2 unaff_DS;
  
  if (set_flag == 0) {
    ((undefined *)&DAT_000f_1af2)[item_index] = ((undefined *)&DAT_000f_1af2)[item_index] & 0xfe;
    pbVar1 = (byte *)(item_index * 0x14 + 0xb0);
    *pbVar1 = *pbVar1 & 0xfe;
  }
  else {
    if ((((undefined *)&DAT_000f_1af2)[item_index] & 2) == 0) {
      return;
    }
    ((undefined *)&DAT_000f_1af2)[item_index] = ((undefined *)&DAT_000f_1af2)[item_index] | 1;
    pbVar1 = (byte *)(item_index * 0x14 + 0xb0);
    *pbVar1 = *pbVar1 | 1;
  }
  pbVar1 = (byte *)(item_index * 0x14 + 0xa6);
  *pbVar1 = *pbVar1 | 1;
  return;
}



/* disable_item_pair (confidence: Confirmed)
   
   Evidence: `(pair_index)` - calls `deselect_item_pair`, then also clears bit 1 ("enabled/valid")
   of `[item+0x1BE2]` for both items of the pair, removing them from selectability entirely */

void __stdcall16far disable_item_pair(word pair_index)

{
  undefined2 unaff_DS;
  
  func_0x000f8e98();
  ((undefined *)&DAT_000f_1af2)[pair_index * 2] =
       ((undefined *)&DAT_000f_1af2)[pair_index * 2] & 0xfd;
  ((undefined *)&DAT_000f_1af3)[pair_index * 2] =
       ((undefined *)&DAT_000f_1af3)[pair_index * 2] & 0xfd;
  return;
}



void __cdecl16far FUN_000f_8f50(void)

{
  int iVar1;
  undefined2 uVar2;
  uint uVar3;
  undefined2 unaff_DS;
  byte bStack_c;
  
  if (*(char *)&DAT_000f_1a9a == '\x02') {
    bStack_c = 0xff;
  }
  else {
    bStack_c = 0;
  }
  uVar2 = 0;
  uVar3 = (uint)bStack_c;
  func_0x000fbbe9();
  func_0x000fbbe9(0xfbbe,(int)*(undefined4 *)0x36,(int)((ulong)*(undefined4 *)0x36 >> 0x10),0x1fff,1
                  ,1,uVar3,uVar2);
  if ((*(byte *)0x18c & 1) != 0) {
    iVar1 = *(int *)((int)*(undefined4 *)&DAT_000f_1ba4 + 0x40);
    if (*(char *)&DAT_000f_1a93 == '\x1e') {
      func_0x000fbbe9(0xfbbe,(int)*(undefined4 *)0x36 + iVar1,
                      (int)((ulong)*(undefined4 *)0x36 >> 0x10),0x800,1,1);
      func_0x000fbbe9(0xfbbe,(int)*(undefined4 *)&DAT_000f_1b90 + iVar1,
                      (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),0x800,0,0);
    }
    else {
      func_0x000fbbe9(0xfbbe,(int)*(undefined4 *)0x36 + iVar1,
                      (int)((ulong)*(undefined4 *)0x36 >> 0x10),0x440,1,1);
      func_0x000fbbe9(0xfbbe,(int)*(undefined4 *)&DAT_000f_1b90 + iVar1,
                      (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),0x440,0,0);
    }
  }
  return;
}



void __stdcall16far FUN_000f_9042(int param_1,byte param_2)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  
  iVar1 = *(int *)(param_1 * 4 + 0x3c);
  iVar2 = *(int *)0x2a + 1;
  *(byte *)(param_1 + 0x1dc) = *(byte *)(param_1 + 0x1dc) & 0xfe;
  *(byte *)(param_1 + 0x1e4) = *(byte *)(param_1 + 0x1e4) & 0xfe;
  if ((param_2 & 1) != 0) {
    uVar4 = 0;
    uVar3 = 0;
    func_0x000fbbe9();
    unaff_CS = 0xfbbe;
    func_0x000fbbe9(0xfbbe,(int)*(undefined4 *)&DAT_000f_1b90 + iVar1,
                    (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iVar2,0,0,uVar3,uVar4);
  }
  if ((param_2 & 2) != 0) {
    func_0x000fa130(unaff_CS,(int)*(undefined4 *)0x36 + *(int *)((param_1 + 8) * 4 + 0x3c),
                    (int)((ulong)*(undefined4 *)0x36 >> 0x10),0,iVar2 >> 2,1);
    func_0x000fa130((char *)s_installed_in_this_instrument__000f_f8f8 + 0x1a,
                    (int)*(undefined4 *)0x36 + iVar1,(int)((ulong)*(undefined4 *)0x36 >> 0x10),0,
                    iVar2,1);
  }
  return;
}



/* pad_readout_buffer_remainder (confidence: Mechanism confirmed)
   
   Evidence: Computes 2 far pointers relative to the readout buffer base `[0x1C80]` (`+0x4851` and
   `-0x37AF`) into fixed scratch vars `[0x1E8]`/`[0x1EA]` and `[0x1EC]`/`[0x1EE]`, calls
   `load_print_record_templates`/`build_print_record_sequence`, then `memset_word_pattern_far`s
   `0x4800` bytes from the buffer's current end (far ptr `[0x36]`, set by
   `compute_readout_buffer_length_and_flag`) with a `0x0101` fill pattern - called only from
   `compute_readout_buffer_length_and_flag` */

void __cdecl16far pad_readout_buffer_remainder(void)

{
  undefined4 uVar1;
  undefined2 unaff_DS;
  
  uVar1 = *(undefined4 *)&DAT_000f_1b90;
  *(int *)0x1e8 = (int)uVar1 + 0x4851;
  *(undefined2 *)0x1ea = (int)((ulong)uVar1 >> 0x10);
  uVar1 = *(undefined4 *)&DAT_000f_1b90;
  *(int *)0x1ec = (int)uVar1 + -0x37af;
  *(undefined2 *)0x1ee = (int)((ulong)uVar1 >> 0x10);
  func_0x000f8234();
  func_0x000f80a6(0xf80a);
  func_0x000fbbe9(0xf80a,(int)*(undefined4 *)0x36,(int)((ulong)*(undefined4 *)0x36 >> 0x10),0x4800,1
                  ,1);
  return;
}



void __stdcall16far FUN_000f_9166(byte param_1,byte *param_2)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  byte *pbVar5;
  undefined2 uVar6;
  undefined2 unaff_DS;
  byte *pbStack_10;
  byte bStack_b;
  
  iVar2 = (uint)param_1 * 0x10;
  _pbStack_10 = (byte *)CONCAT22(unaff_DS,(byte *)(iVar2 + 0x18c));
  *_pbStack_10 = 0;
  if (*(char *)&DAT_000f_1a93 == '\x14') {
    *_pbStack_10 = *_pbStack_10 | *(byte *)0x18c & 8;
  }
  bVar1 = *param_2;
  if ((bVar1 & 1) == 0) {
    *(undefined2 *)(iVar2 + 0x194) = 0x400;
  }
  else {
    *_pbStack_10 = *_pbStack_10 | 1;
    *(undefined2 *)(iVar2 + 0x194) = 0x1000;
  }
  if ((bVar1 & 2) != 0) {
    *_pbStack_10 = *_pbStack_10 | 4;
    *(int *)(iVar2 + 0x194) = *(int *)(iVar2 + 0x194) >> 1;
  }
  if ((bVar1 & 4) != 0) {
    *_pbStack_10 = *_pbStack_10 | 0x10;
    *(int *)(iVar2 + 0x194) = *(int *)(iVar2 + 0x194) << 1;
  }
  if ((bVar1 & 8) != 0) {
    *_pbStack_10 = *_pbStack_10 | 0x20;
  }
  uVar6 = (undefined2)((ulong)param_2 >> 0x10);
  pbVar5 = (byte *)param_2;
  if (pbVar5[5] != 0) {
    *_pbStack_10 = *_pbStack_10 | 0x40;
  }
  if ((5 < pbVar5[4]) && ((bVar1 & 8) != 0)) {
    *_pbStack_10 = *_pbStack_10 | 2;
  }
  if ((bVar1 & 8) == 0) {
    bStack_b = 8;
  }
  else if ((bVar1 & 2) == 0) {
    if ((bVar1 & 0x20) == 0) {
      bStack_b = 2;
    }
    else {
      bStack_b = 1;
    }
  }
  else {
    bStack_b = 7;
  }
  uVar3 = (uint)pbVar5[4];
  uVar4 = (uint)bStack_b;
  func_0x000fadbf();
  func_0x000fac27(0xfaab,(byte *)(iVar2 + 0x18c),unaff_DS,0,1,uVar4,uVar3);
  func_0x000f9710(0xfaab,param_1,param_2);
  *(undefined2 *)(iVar2 + 400) = *(undefined2 *)(pbVar5 + 0xc);
  *(undefined2 *)(iVar2 + 0x192) = *(undefined2 *)(pbVar5 + 0x16);
  return;
}



void __stdcall16far FUN_000f_930d(byte *param_1,byte param_2,char param_3)

{
  byte *pbVar1;
  int iVar2;
  byte bVar3;
  byte *pbVar4;
  int *piVar5;
  undefined2 uVar6;
  undefined2 unaff_DS;
  undefined2 uVar7;
  byte local_26;
  undefined1 local_25;
  int iStack_24;
  undefined2 uStack_22;
  undefined4 uStack_20;
  int iStack_1c;
  undefined2 uStack_1a;
  int *piStack_18;
  undefined1 *puStack_c;
  
  bVar3 = param_2 * '\x02' + param_3;
  iVar2 = (uint)bVar3 * 0x14;
  piVar5 = (int *)((uint)bVar3 * 4 + 0x3c);
  _piStack_18 = (int *)CONCAT22(unaff_DS,piVar5);
  _puStack_c = (undefined1 *)CONCAT22(unaff_DS,(undefined1 *)((uint)param_2 * 0x10 + 0x18c));
  ((undefined *)&DAT_000f_1af2)[bVar3] = ((undefined *)&DAT_000f_1af2)[bVar3] | 2;
  _iStack_1c = CONCAT22((int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),
                        (int)*(undefined4 *)&DAT_000f_1b90 + *_piStack_18);
  uStack_20 = (undefined1 *)
              CONCAT22((int)((ulong)*(undefined4 *)0x36 >> 0x10),
                       (undefined1 *)((int)*(undefined4 *)0x36 + *_piStack_18));
  uVar6 = (undefined2)((ulong)param_1 >> 0x10);
  pbVar4 = (byte *)param_1;
  if ((*param_1 & 8) == 0) {
    piVar5 = (int *)(uint)pbVar4[7];
    uVar7 = 0;
    func_0x000fad6e();
    func_0x000fad6e(0xfaab,(undefined *)&DAT_000f_1b2e + (uint)bVar3 * 10,unaff_DS,0,pbVar4[8],0,
                    pbVar4[6],uVar7,piVar5);
  }
  else {
    uVar7 = unaff_DS;
    func_0x000fad6e();
  }
  func_0x000f9650(0xfaab,(undefined *)&DAT_000f_1b24 + (uint)bVar3 * 10,unaff_DS,param_1,uVar7,
                  piVar5);
  func_0x000fbbe9((char *)s_AUTO_needs_XY_or_talk_only__000f_f81c + 10,(int)_iStack_1c,
                  (int)((ulong)_iStack_1c >> 0x10),*(undefined2 *)(pbVar4 + 0xe),0,0);
  func_0x000fbbe9(0xfbbe,(undefined1 *)uStack_20,(int)((ulong)uStack_20 >> 0x10),
                  *(undefined2 *)(pbVar4 + 0xe),1,1);
  if (*(char *)&DAT_000f_1a93 != '\x1e') {
    iStack_24 = 0x4444;
    if ((*param_1 & 2) == 0) {
      uStack_22 = 0x400;
    }
    else {
      uStack_22 = 0x200;
      if (param_3 != '\0') {
        iStack_24 = 0x4644;
      }
    }
    func_0x000fbbe9(0xfbbe,(int)*(undefined4 *)&DAT_000f_1b90 + iStack_24,
                    (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),uStack_22,0,0);
    func_0x000fbbe9(0xfbbe,(int)*(undefined4 *)0x36 + iStack_24,
                    (int)((ulong)*(undefined4 *)0x36 >> 0x10),uStack_22,1,1);
  }
  *(undefined2 *)(iVar2 + 0xaa) = 0;
  *(undefined2 *)(iVar2 + 0xa8) = 1;
  *uStack_20 = 2;
  ((undefined1 *)uStack_20)[1] = 2;
  func_0x000f9925(0xfbbe,*_puStack_c,&local_26);
  local_26 = local_26 & 0xfe;
  func_0x000f9aab(0xf981,iVar2 + 0xa6,unaff_DS,local_26,local_25,(int)_iStack_1c,
                  (int)((ulong)_iStack_1c >> 0x10),1,0,4);
  if (((undefined *)&DAT_000f_1af2)[bVar3] == '\0') {
    *(byte *)(bVar3 + 0x1dc) = *(byte *)(bVar3 + 0x1dc) & 0xfe;
  }
  else {
    pbVar1 = (byte *)(bVar3 + 0x1dc);
    *pbVar1 = *pbVar1 | 1;
  }
  return;
}



void __stdcall16far FUN_000f_956b(byte param_1,char param_2)

{
  byte bVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined1 uStack_d;
  
  if (param_2 == '\0') {
    uStack_d = 0;
  }
  else {
    uStack_d = 4;
  }
  if (*(char *)&DAT_000f_1a93 == '\x1e') {
    iVar2 = (uint)param_1 * 0x1c;
    *(undefined2 *)(iVar2 + 0x576) = 0;
    *(undefined2 *)(iVar2 + 0x578) = 0;
    *(undefined1 *)(iVar2 + 0x580) = uStack_d;
    *(undefined2 *)(iVar2 + 0x584) = 0;
    *(undefined2 *)(iVar2 + 0x586) = 0;
    *(undefined1 *)(iVar2 + 0x58e) = uStack_d;
    *(undefined1 *)(param_1 + 0x564) = uStack_d;
  }
  else {
    bVar1 = *(byte *)&DAT_000f_0474 & 0xfd;
    iVar2 = (uint)param_1 * 0x1c;
    *(undefined2 *)(iVar2 + 0x576) = *(undefined2 *)&DAT_000f_0486;
    *(undefined2 *)(iVar2 + 0x578) = *(undefined2 *)&DAT_000f_0488;
    *(byte *)(iVar2 + 0x580) = bVar1;
    *(undefined2 *)(iVar2 + 0x584) = *(undefined2 *)0x584;
    *(undefined2 *)(iVar2 + 0x586) = *(undefined2 *)0x586;
    *(byte *)(iVar2 + 0x58e) = bVar1;
    *(byte *)(param_1 + 0x564) = bVar1;
  }
  return;
}



/* compute_print_cell_size (confidence: Mechanism confirmed)
   
   Evidence: `(far ptr dest, far ptr src)` - reads a flag byte from `src`; if bit `0x8` clear,
   checks `src`'s word field `[+0x12]`: if zero, marks `dest` "empty" (`[+7]=0`); else marks `dest`
   "used" (`[+7]=1`) and computes `dest[+8]` as `src[+0x12]` scaled by `>>6` (if `src` flag bit
   `0x4` set) or `<<2` (otherwise) - a print-record cell-size scaler */

void __stdcall16far compute_print_cell_size(word dest_off,word dest_seg,word src_off,word src_seg)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *_src_off;
  if ((bVar1 & 8) == 0) {
    if (*(int *)(src_off + 0x12) == 0) {
      *(undefined1 *)(dest_off + 7) = 0;
    }
    else {
      *(undefined1 *)(dest_off + 7) = 1;
      if ((bVar1 & 4) == 0) {
        iVar2 = *(int *)(src_off + 0x12) << 2;
      }
      else {
        iVar2 = *(int *)(src_off + 0x12) >> 6;
      }
      *(int *)(dest_off + 8) = iVar2;
    }
    dest_off = dest_off + 10;
  }
  if (*(int *)(src_off + 0x10) == 0) {
    *(undefined1 *)(dest_off + 7) = 0;
  }
  else {
    *(undefined1 *)(dest_off + 7) = 1;
    if ((bVar1 & 4) == 0) {
      iVar2 = *(int *)(src_off + 0x10) << 2;
    }
    else {
      iVar2 = *(int *)(src_off + 0x10) >> 6;
    }
    *(int *)(dest_off + 8) = iVar2;
  }
  return;
}



void __stdcall16far FUN_000f_9710(byte param_1,byte *param_2)

{
  undefined2 uVar1;
  byte *pbVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  undefined2 uVar4;
  int iStack_14;
  int iStack_12;
  byte *pbStack_c;
  
  uVar4 = 0x41;
  uVar1 = func_0x0009470e();
  if (param_1 == 4) {
    param_1 = 1;
  }
  _pbStack_c = (byte *)CONCAT22(unaff_DS,(byte *)((uint)param_1 * 0x10 + 0x18c));
  uVar3 = (undefined2)((ulong)param_2 >> 0x10);
  pbVar2 = (byte *)param_2;
  iStack_14 = *(int *)(pbVar2 + 0x14);
  if (((*param_2 & 0x10) != 0) || ((*param_2 & 8) == 0)) {
    iStack_14 = iStack_14 << 1;
  }
  iStack_12 = iStack_14;
  if ((*_pbStack_c & 0x10) != 0) {
    iStack_12 = iStack_14 << 1;
  }
  if ((*param_2 & 1) == 0) {
    iStack_14 = iStack_14 << 2;
  }
  if ((*_pbStack_c & 4) != 0) {
    iStack_14 = iStack_14 << 1;
  }
  if (0x1000 < iStack_14) {
    iStack_14 = 0x1000;
  }
  *(int *)(pbVar2 + 0xc) = iStack_12;
  *(int *)(pbVar2 + 0x16) = iStack_14;
  func_0x0009470e(38000,uVar1,uVar4);
  return;
}



void __stdcall16far FUN_000f_97d3(void)

{
  func_0x000fa2b0();
  return;
}



void __stdcall16far FUN_000f_9925(byte param_1,byte *param_2,undefined1 *param_3)

{
  undefined1 uStack_9;
  
  uStack_9 = 0x21;
  if ((param_1 & 0x20) != 0) {
    uStack_9 = 0x29;
  }
  if ((param_1 & 2) != 0) {
    uStack_9 = uStack_9 | 0x10;
  }
  if ((param_1 & 0x10) != 0) {
    uStack_9 = uStack_9 | 4;
  }
  *param_2 = uStack_9;
  if ((param_1 & 4) == 0) {
    if ((param_1 & 8) == 0) {
      *param_3 = 0x10;
    }
    else {
      *param_3 = 0x1a;
    }
  }
  else if ((param_1 & 8) == 0) {
    *param_3 = 0x11;
  }
  else {
    *param_3 = 0x1c;
  }
  return;
}



void __stdcall16far FUN_000f_99b9(undefined2 param_1,undefined1 param_2,undefined1 param_3)

{
  byte *pbVar1;
  undefined2 uVar2;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  int iStack_14;
  int iStack_12;
  byte *pbStack_c;
  
  if ((*(byte *)0x18c & 1) == 0) {
    iStack_14 = *(int *)0x194;
  }
  else {
    iStack_14 = *(int *)0x194 >> 2;
  }
  iStack_14 = iStack_14 + -1;
  _pbStack_c = (byte *)CONCAT22(unaff_DS,(byte *)0xa6);
  for (iStack_12 = 0; iStack_12 < 2; iStack_12 = iStack_12 + 1) {
    pbVar1 = (byte *)_pbStack_c;
    uVar2 = (undefined2)((ulong)_pbStack_c >> 0x10);
    if ((((undefined *)&DAT_000f_1af2)[iStack_12] & 2) == 0) {
      *_pbStack_c = 0;
      pbVar1[1] = 0;
      *_pbStack_c = *_pbStack_c | 1;
      pbVar1[10] = pbVar1[10] & 0x28;
    }
    else {
      func_0x000f9aab(unaff_CS,pbVar1,uVar2,param_2,param_3,
                      (int)*(undefined4 *)&DAT_000f_1b90 + *(int *)(iStack_12 * 4 + 0x3c),
                      (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iStack_14,
                      *(undefined2 *)0x186,6);
      unaff_CS = 0xf981;
    }
    _pbStack_c = (byte *)CONCAT22(uVar2,pbVar1 + 0x14);
  }
  _pbStack_c = (byte *)CONCAT22(unaff_DS,(undefined1 *)0x146);
  *_pbStack_c = 1;
  *(undefined1 *)0x147 = 0;
  *(byte *)0x150 = *(byte *)0x150 & 0x28;
  _pbStack_c = (byte *)CONCAT22(unaff_DS,(undefined1 *)0x15a);
  *_pbStack_c = 1;
  *(undefined1 *)0x15b = 0;
  *(byte *)0x164 = *(byte *)0x164 & 0x28;
  return;
}



void __stdcall16far
FUN_000f_9aab(undefined1 *param_1,undefined1 param_2,undefined1 param_3,undefined2 param_4,
             undefined2 param_5,undefined2 param_6,uint param_7,undefined1 param_8)

{
  undefined1 *puVar1;
  undefined2 uVar2;
  undefined2 local_a [4];
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  puVar1 = (undefined1 *)param_1;
  *param_1 = 0;
  puVar1[1] = 0;
  puVar1[10] = param_2;
  puVar1[0xd] = param_3;
  puVar1[0xe] = 0;
  puVar1[0xb] = 0x10;
  puVar1[0xc] = 0;
  if ((param_7 & 1) == 0) {
    puVar1[1] = puVar1[1] & 0xfe;
  }
  else {
    puVar1[1] = puVar1[1] | 1;
  }
  local_a[0] = func_0x000fbc2f();
  func_0x000fbc4d(0xfbc2,local_a);
  puVar1[0x11] = puVar1[0xf];
  puVar1[0x12] = puVar1[0x10];
  *(undefined2 *)(puVar1 + 6) = 0;
  *(undefined2 *)(puVar1 + 8) = param_6;
  *param_1 = param_8;
  return;
}



void __stdcall16far FUN_000f_9b56(char param_1)

{
  undefined2 unaff_DS;
  
  *(uint *)0x186 = *(uint *)0x186 | 4;
  *(uint *)0x186 = *(uint *)0x186 & 0xfb5e;
  if (((param_1 == '\x03') || (param_1 == '\x05')) || (param_1 == '\a')) {
    *(uint *)0x186 = *(uint *)0x186 | 1;
    *(uint *)0x186 = *(uint *)0x186 | 0x80;
  }
  if ((((*(byte *)&DAT_000f_1a82 & 3) == 3) && (param_1 != '\x04')) && (param_1 != '\x03')) {
    *(uint *)0x186 = *(uint *)0x186 | 0x100;
  }
  else {
    *(uint *)0x186 = *(uint *)0x186 & 0xfeff;
  }
  return;
}



void __stdcall16far FUN_000f_9bb4(byte param_1)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  if ((param_1 & 4) == 0) {
    *(undefined1 *)0x16f = 0;
  }
  else {
    *(undefined1 *)0x16f = 1;
  }
  if ((param_1 & 0x10) == 0) {
    *(undefined1 *)0x16e = 0;
  }
  else {
    *(undefined1 *)0x16e = 1;
  }
  uVar1 = func_0x000f858e();
  *(undefined2 *)0x170 = uVar1;
  if (((*(char *)0x1f == '\x03') || (*(char *)0x1f == '\a')) || (*(char *)0x1f == '\x05')) {
    if ((param_1 & 4) == 0) {
      *(undefined2 *)0x172 = 0x10fe;
      *(undefined2 *)(code *)scale_and_plot_point = 0x43e;
    }
    else {
      *(undefined2 *)0x172 = 0x87e;
      *(undefined2 *)(code *)scale_and_plot_point = 0x21e;
    }
  }
  return;
}



void __stdcall16far
FUN_000f_9cec(int param_1,int param_2,byte param_3,uint param_4,int param_5,int param_6)

{
  int iVar1;
  char *unaff_CS;
  undefined2 unaff_DS;
  int iStack_10;
  int iStack_e;
  int iStack_c;
  
  iVar1 = *(int *)((param_4 & 1) * 10 + 0x68) >> (param_3 & 0x1f);
  if ((*(uint *)0x186 & 0x20) != 0) {
    unaff_CS = (char *)s_Smoothing_000f_f8ce + 5;
    func_0x000f9fa5();
  }
  if (param_5 < (iVar1 - param_6) + param_1) {
    iStack_c = (((iVar1 - param_6) + param_1) - param_5) + -1;
  }
  else if ((iVar1 - param_6) + param_1 < 0) {
    iStack_c = (iVar1 - param_6) + param_1 + param_5 + 1;
  }
  else {
    iStack_c = (iVar1 - param_6) + param_1;
  }
  if (param_5 < iStack_c + -1) {
    iStack_c = ((iStack_c + -1) - param_5) + -1;
  }
  else if (iStack_c + -1 < 0) {
    iStack_c = iStack_c + param_5;
  }
  else {
    iStack_c = iStack_c + -1;
  }
  if (param_5 < (iVar1 - param_6) + param_2) {
    iStack_e = (((iVar1 - param_6) + param_2) - param_5) + -1;
  }
  else if ((iVar1 - param_6) + param_2 < 0) {
    iStack_e = (iVar1 - param_6) + param_2 + param_5 + 1;
  }
  else {
    iStack_e = (iVar1 - param_6) + param_2;
  }
  if (param_5 < iStack_e + -1) {
    iStack_e = ((iStack_e + -1) - param_5) + -1;
  }
  else if (iStack_e + -1 < 0) {
    iStack_e = iStack_e + param_5;
  }
  else {
    iStack_e = iStack_e + -1;
  }
  if (iStack_c == param_5) {
    if (iStack_c == iStack_e) {
      iStack_c = 0;
      iStack_e = 0;
    }
    else {
      iStack_c = 0;
    }
  }
  if ((*(char *)0x18e == '\x17') || (*(char *)0x18e == '\x18')) {
    if (*(char *)0x18e == '\x17') {
      iStack_10 = 2;
    }
    else {
      iStack_10 = 1;
    }
    *(int *)0x392 = param_4 * 2 + 0x37e;
    *(undefined2 *)0x394 = unaff_DS;
    *(int *)0x3aa = param_4 * 2 + 0x396;
    *(undefined2 *)0x3ac = unaff_DS;
    if (((iStack_e <= *(int *)*(undefined4 *)0x3aa + 0x14) &&
        ((int)((char *)s_ACQ_RAM_even_000f_fef8 + 4 + *(int *)*(undefined4 *)0x3aa) <= iStack_e)) &&
       ((*(int *)*(undefined4 *)0x3aa + 2 < iStack_e ||
        (iStack_e < *(int *)*(undefined4 *)0x3aa + -2)))) {
      if (*(int *)*(undefined4 *)0x3aa < iStack_e) {
        iStack_e = *(int *)*(undefined4 *)0x3aa + iStack_10;
      }
      else if (iStack_e < *(int *)*(undefined4 *)0x3aa) {
        iStack_e = *(int *)*(undefined4 *)0x3aa - iStack_10;
      }
    }
    if (((iStack_c <= *(int *)*(undefined4 *)0x392 + 0x14) &&
        ((int)((char *)s_ACQ_RAM_even_000f_fef8 + 4 + *(int *)*(undefined4 *)0x392) <= iStack_c)) &&
       ((*(int *)*(undefined4 *)0x392 + 2 < iStack_c ||
        (iStack_c < *(int *)*(undefined4 *)0x392 + -2)))) {
      if (*(int *)*(undefined4 *)0x392 < iStack_c) {
        iStack_c = *(int *)*(undefined4 *)0x392 + iStack_10;
      }
      else if (iStack_c < *(int *)*(undefined4 *)0x392) {
        iStack_c = *(int *)*(undefined4 *)0x392 - iStack_10;
      }
    }
    *(int *)*(undefined4 *)0x392 = iStack_c;
    *(int *)*(undefined4 *)0x3aa = iStack_e;
  }
  func_0x000fb847(unaff_CS,param_4,iStack_c,iStack_e);
  return;
}



void __stdcall16far FUN_000f_9fa5(int param_1,int *param_2,int *param_3)

{
  int iVar1;
  undefined2 unaff_DS;
  
  if (param_1 == 8) {
    param_1 = 2;
  }
  if (param_1 == 9) {
    param_1 = 3;
  }
  iVar1 = *(int *)(param_1 * 4 + 0x98);
  if (iVar1 < *param_2) {
    *param_2 = iVar1;
  }
  if (iVar1 < *param_3) {
    *param_3 = iVar1;
  }
  return;
}



/* clear_display_chip_frame_trigger (confidence: Confirmed mechanism and hardware identity)
   
   Evidence: Writes `0` to physical `0x42000` - see `clear_display_chip_int_reset`. Was
   `clear_display_chip_frame_trigger` */

void __cdecl16far clear_display_chip_frame_trigger(void)

{
  DAT_4000_2000 = 0;
  return;
}



/* clear_display_chip_int_reset (confidence: Confirmed mechanism and hardware identity)
   
   Evidence: Writes `0` to physical `0x41000` - the same fixed address `read_display_chip_int_reset`
   reads. Was `clear_display_chip_int_reset` */

void __cdecl16far clear_display_chip_int_reset(void)

{
  DAT_4000_1000 = 0;
  return;
}



void __stdcall16far
FUN_000f_a02a(undefined4 param_1,int param_2,int param_3,int param_4,int param_5,undefined1 param_6)

{
  int iVar1;
  uint uVar2;
  char *unaff_CS;
  int iVar3;
  undefined1 uStack_12;
  byte bStack_11;
  int iStack_10;
  int iStack_c;
  
  if (param_3 < param_2) {
    bStack_11 = 3;
    uStack_12 = 1;
    iVar1 = param_2;
  }
  else {
    bStack_11 = 1;
    uStack_12 = 3;
    iVar1 = param_3;
    param_3 = param_2;
  }
  iStack_c = param_3 + param_4;
  iStack_10 = iVar1 + param_4;
  if (param_5 < iStack_c) {
    iStack_c = param_5;
  }
  if (param_5 < iStack_10) {
    iStack_10 = param_5;
  }
  if (param_3 < iVar1) {
    if (iVar1 < iStack_c) {
      uVar2 = (uint)bStack_11;
      iVar3 = iVar1;
      func_0x000fa130();
    }
    else {
      uVar2 = (uint)bStack_11;
      iVar3 = iStack_c;
      func_0x000fa130();
    }
    if (iStack_c < iVar1) {
      unaff_CS = (char *)s_installed_in_this_instrument__000f_f8f8 + 0x1a;
      func_0x000fa130((char *)s_installed_in_this_instrument__000f_f8f8 + 0x1a,param_1,iVar1,
                      iStack_10,uStack_12,iVar3,uVar2);
    }
    else {
      unaff_CS = (char *)s_installed_in_this_instrument__000f_f8f8 + 0x1a;
      func_0x000fa130((char *)s_installed_in_this_instrument__000f_f8f8 + 0x1a,param_1,iStack_c,
                      iStack_10,uStack_12,iVar3,uVar2);
    }
  }
  if (iVar1 <= iStack_c) {
    func_0x000fa130(unaff_CS,param_1,iVar1,iStack_c,param_6);
  }
  return;
}



void __stdcall16far FUN_000f_a130(undefined4 param_1,int param_2,int param_3,byte param_4)

{
  undefined4 uStack_c;
  
  for (uStack_c = (byte *)CONCAT22((int)((ulong)param_1 >> 0x10),(byte *)((int)param_1 + param_2));
      (byte *)uStack_c <= (byte *)((int)param_1 + param_3);
      uStack_c = (byte *)CONCAT22(uStack_c._2_2_,(byte *)uStack_c + 1)) {
    if (((*uStack_c & 3) != 2) && ((*uStack_c & 3) != 0)) {
      if ((*uStack_c & 4) == 0) {
        *uStack_c = param_4;
      }
      else {
        *uStack_c = param_4 | 4;
      }
    }
  }
  return;
}



void __stdcall16far FUN_000f_a1a0(byte param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  int iStack_a;
  
  if (param_2 != 0) {
    uVar4 = *(undefined2 *)0x2a;
    iVar1 = func_0x000fa2b0();
    iStack_a = param_3 + *(int *)0x170;
    if (param_4 < iStack_a) {
      iStack_a = param_4;
    }
    if (*(int *)0x2a < iStack_a) {
      iStack_a = (iStack_a - *(int *)0x2a) + -1;
    }
    else if (iStack_a < 0) {
      iStack_a = iStack_a + *(int *)0x2a + 1;
    }
    iVar2 = func_0x000fa2b0((char *)s_installed_in_this_instrument__000f_f8f8 + 0x1a,iStack_a,
                            param_4,*(undefined2 *)((uint)param_1 * 10 + 0x68),*(undefined2 *)0x2a,
                            uVar4);
    uVar4 = (undefined2)((ulong)*(undefined4 *)0x36 >> 0x10);
    iVar3 = (int)*(undefined4 *)0x36 + *(int *)((uint)param_1 * 4 + 0x3c);
    if (iVar2 < iVar1) {
      func_0x000fa130((char *)s_installed_in_this_instrument__000f_f8f8 + 0x1a,iVar3,uVar4,iVar1,
                      *(undefined2 *)0x2a,3);
      func_0x000fa130((char *)s_installed_in_this_instrument__000f_f8f8 + 0x1a,iVar3,uVar4,0,iVar2,3
                     );
    }
    else {
      func_0x000fa130((char *)s_installed_in_this_instrument__000f_f8f8 + 0x1a,iVar3,uVar4,iVar1,
                      iVar2,3,(uint)param_1 * 4);
    }
  }
  return;
}



int __stdcall16far FUN_000f_a2b0(int param_1,int param_2,int param_3,int param_4)

{
  undefined2 uStack_a;
  
  if (param_4 < (param_3 - param_2) + param_1) {
    uStack_a = (((param_3 - param_2) + param_1) - param_4) + -1;
  }
  else if ((param_3 - param_2) + param_1 < 0) {
    uStack_a = (param_3 - param_2) + param_1 + param_4 + 1;
  }
  else {
    uStack_a = (param_3 - param_2) + param_1;
  }
  return uStack_a;
}



void __cdecl16far FUN_000f_a30e(void)

{
  char cVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_DS;
  bool bVar5;
  byte *pbStack_1e;
  int *piStack_1a;
  int iStack_16;
  char cStack_11;
  byte bStack_e;
  byte bStack_d;
  char cStack_9;
  
  cVar1 = *(char *)&DAT_000f_1b2b;
  iVar3 = *(int *)&DAT_000f_1b36;
  bVar5 = (*(byte *)0x18f & 0xb) == 0;
  if ((*(byte *)0x18f & 8) == 8) {
    *(undefined1 *)&DAT_000f_1b35 = 0;
    *(undefined1 *)&DAT_000f_1b2b = 0;
  }
  else {
    for (cStack_9 = '\0'; cStack_9 < '\x02'; cStack_9 = cStack_9 + '\x01') {
      if (cStack_9 == '\0') {
        pbStack_1e = (byte *)0x35a;
        piStack_1a = (int *)&DAT_000f_1b24;
        bStack_d = *(byte *)0x602 & 0xf;
      }
      else {
        pbStack_1e = (byte *)0x36c;
        piStack_1a = (int *)&DAT_000f_1b2e;
        bStack_d = *(byte *)0x602 & 0xf0;
      }
      _piStack_1a = (int *)CONCAT22(unaff_DS,piStack_1a);
      _pbStack_1e = (byte *)CONCAT22(unaff_DS,pbStack_1e);
      if ((bStack_d & 0x11) != 0) {
        pbStack_1e[10] = 0;
        *(undefined1 *)((int)piStack_1a + 7) = 0;
        pbStack_1e[0xb] = 0;
      }
      if ((((*(byte *)&DAT_000f_1b0c & 0x80) != 0) &&
          ((pbStack_1e[1] != 0 || ((bVar5 && (cStack_9 == '\x01')))))) &&
         (((!bVar5 && ((*(byte *)(piStack_1a + 3) & 3) == 3)) ||
          (((bVar5 && ((*(byte *)&DAT_000f_1b2a & 3) == 3)) && ((*(byte *)&DAT_000f_1b34 & 3) == 3))
          )))) {
        *(undefined2 *)(pbStack_1e + 8) = *(undefined2 *)(pbStack_1e + 6);
        *(undefined2 *)(pbStack_1e + 6) = *(undefined2 *)(pbStack_1e + 4);
        *(undefined2 *)(pbStack_1e + 4) = *(undefined2 *)(pbStack_1e + 2);
        pbStack_1e[10] = pbStack_1e[10] << 1 | 1;
        if ((((pbStack_1e[10] & 7) == 7) &&
            (*(int *)(pbStack_1e + 8) - *(int *)(pbStack_1e + 6) < 4)) &&
           (-4 < *(int *)(pbStack_1e + 8) - *(int *)(pbStack_1e + 6))) {
          iVar4 = *(int *)(pbStack_1e + 8);
          *(int *)(pbStack_1e + 0xe) = iVar4;
          piStack_1a[4] = iVar4;
          *(int *)(pbStack_1e + 0xc) = *_piStack_1a;
          pbStack_1e[0xb] = 0xff;
          *(undefined *)((int)piStack_1a + 7) = 0xff;
        }
      }
      pbStack_1e[1] = 0;
      if (((*_pbStack_1e & 1) != 0) && ((*(byte *)0x18d & 0xf) == 2)) {
        pbStack_1e[10] = 0;
        pbStack_1e[0xb] = 0;
        *(undefined1 *)((int)piStack_1a + 7) = 0;
      }
      if (((pbStack_1e[0xb] != 0) && (*_pbStack_1e != 0)) && (!bVar5)) {
        piStack_1a[4] = *(int *)(pbStack_1e + 0xe) + (*(int *)(pbStack_1e + 0xc) - *_piStack_1a) * 4
        ;
      }
    }
    if (bVar5) {
      *(undefined1 *)&DAT_000f_1b35 = 0;
      if (((*(char *)&DAT_000f_1b28 == *(char *)&DAT_000f_1b32) && (*(char *)0x365 != '\0')) &&
         (*(char *)0x377 != '\0')) {
        if ((*(char *)0x35a != '\0') || (*(char *)0x36c != '\0')) {
          *(int *)&DAT_000f_1b2c =
               *(int *)0x368 +
               ((*(int *)0x366 - *(int *)&DAT_000f_1b24) + (*(int *)0x378 - *(int *)&DAT_000f_1b2e))
               * 4;
        }
      }
      else {
        *(undefined1 *)&DAT_000f_1b2b = 0;
      }
    }
  }
  bStack_e = 0;
  *(undefined1 *)0x36c = 0;
  *(undefined1 *)0x35a = 0;
  cVar2 = *(char *)&DAT_000f_1b2b;
  *(char *)&DAT_000f_1b7b = cVar2;
  if ((cVar1 != cVar2) ||
     (cVar1 = *(char *)&DAT_000f_1b35, *(char *)&DAT_000f_1b85 = cVar1, cStack_11 != cVar1)) {
    bStack_e = 1;
  }
  iVar4 = *(int *)&DAT_000f_1b2c;
  *(int *)&DAT_000f_1b7c = iVar4;
  if ((iStack_16 != iVar4) ||
     (iVar4 = *(int *)&DAT_000f_1b36, *(int *)&DAT_000f_1b86 = iVar4, iVar3 != iVar4)) {
    bStack_e = bStack_e | 2;
  }
  if (bStack_e != 0) {
    func_0x000f7f2a();
  }
  if ((*(byte *)&DAT_000f_1b0c & 0x80) != 0) {
    *(undefined1 *)&DAT_000f_1b0c = 0;
  }
  *(undefined1 *)0x602 = 0;
  return;
}



void __stdcall16far FUN_000f_a60c(byte *param_1,int param_2)

{
  undefined2 unaff_CS;
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  if ((*param_1 & 4) != 0) {
    unaff_CS = 0xfb59;
    func_0x000fb593();
  }
  uVar1 = unaff_CS;
  if ((*param_1 & 1) != 0) {
    uVar1 = 0xfb59;
    func_0x000fb832(unaff_CS,((byte *)param_1)[10],(int)*(undefined4 *)&DAT_000f_1cec + param_2 * 9,
                    (int)((ulong)*(undefined4 *)&DAT_000f_1cec >> 0x10));
  }
  if ((*param_1 & 2) != 0) {
    func_0x000fb5fe(uVar1,param_2,param_1,(int)*(undefined4 *)0x36 + *(int *)(param_2 * 4 + 0x3c),
                    (int)((ulong)*(undefined4 *)0x36 >> 0x10),
                    (int)*(undefined4 *)&DAT_000f_1cec + param_2 * 9,
                    (int)((ulong)*(undefined4 *)&DAT_000f_1cec >> 0x10));
  }
  *param_1 = 0;
  return;
}



void __cdecl16far FUN_000f_a6c8(void)

{
  byte *pbVar1;
  char cVar2;
  byte bVar3;
  byte *pbVar4;
  int iVar5;
  byte *pbVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  char *unaff_CS;
  char *pcVar9;
  char *pcVar10;
  undefined2 unaff_DS;
  int iStack_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined2 uStack_14;
  int iStack_12;
  byte *pbStack_10;
  char *pcStack_c;
  
  if ((*(uint *)0x186 & 8) == 0) {
    if ((*(byte *)0x20 & 0x40) == 0) {
      if ((*(uint *)0x22 & 0x200) != 0) {
        *(uint *)0x22 = *(uint *)0x22 & 0xfdff;
        *(uint *)0x22 = *(uint *)0x22 | 0x400;
      }
    }
    else {
      unaff_CS = (char *)s_PEAKDET_000f_faa0 + 7;
      func_0x000fb975();
    }
    if (*(char *)0xa6 != '\0' || *(char *)0xba != '\0') {
      _pcStack_c = (char *)CONCAT22(unaff_DS,(char *)0xa6);
      for (iStack_12 = 0; iStack_12 < 2; iStack_12 = iStack_12 + 1) {
        cVar2 = *_pcStack_c;
        uVar7 = (undefined2)((ulong)_pcStack_c >> 0x10);
        if (cVar2 != '\0') {
          func_0x000fa60c(unaff_CS,pcStack_c,uVar7,iStack_12);
          unaff_CS = (char *)0xfa60;
        }
        _pcStack_c = (char *)CONCAT22(uVar7,pcStack_c + 0x14);
      }
    }
    if (*(char *)0xce != '\0' ||
        (*(char *)0xe2 != '\0' ||
        (*(char *)0xf6 != '\0' ||
        (((char *)s_Copyright__C__Tektronix__Inc___1_000f_000a)[0x10] != '\0' ||
        (((char *)s_Copyright__C__Tektronix__Inc___1_000f_000a)[0x24] != '\0' ||
        ((char *)s_Copyright__C__Tektronix__Inc___1_000f_000a)[0x38] != '\0'))))) {
      _pcStack_c = (char *)CONCAT22(unaff_DS,(char *)0xce);
      for (iStack_12 = 2; iStack_12 < 8; iStack_12 = iStack_12 + 1) {
        cVar2 = *_pcStack_c;
        uVar7 = (undefined2)((ulong)_pcStack_c >> 0x10);
        if (cVar2 != '\0') {
          func_0x000fa60c(unaff_CS,pcStack_c,uVar7,iStack_12);
          unaff_CS = (char *)0xfa60;
        }
        _pcStack_c = (char *)CONCAT22(uVar7,pcStack_c + 0x14);
      }
    }
    if (*(char *)0x146 != '\0' || *(char *)0x15a != '\0') {
      _pcStack_c = (char *)CONCAT22(unaff_DS,(char *)0x146);
      for (iStack_12 = 8; iStack_12 < 10; iStack_12 = iStack_12 + 1) {
        cVar2 = *_pcStack_c;
        uVar7 = (undefined2)((ulong)_pcStack_c >> 0x10);
        if (cVar2 != '\0') {
          if (*(char *)&DAT_000f_1a93 != '\x1e') {
            pcStack_c[10] = pcStack_c[10] & 0xfb;
          }
          func_0x000fa60c(unaff_CS,pcStack_c,uVar7,iStack_12);
          unaff_CS = (char *)0xfa60;
        }
        _pcStack_c = (char *)CONCAT22(uVar7,pcStack_c + 0x14);
      }
    }
    if ((*(uint *)0x186 & 2) != 0) {
      *(uint *)0x186 = *(uint *)0x186 & 0xfffd;
      _pbStack_10 = (byte *)CONCAT22(unaff_DS,(byte *)0x1f1);
      for (iStack_12 = 0; iStack_12 < 8; iStack_12 = iStack_12 + 1) {
        uVar7 = (undefined2)((ulong)_pbStack_10 >> 0x10);
        pbVar6 = (byte *)_pbStack_10;
        if (pbVar6[1] != 0) {
          pbVar6[1] = pbVar6[1] & 0xfe;
          uStack_18 = (byte *)CONCAT22(uVar7,pbVar6 + 2);
          uVar8 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1cec >> 0x10);
          iVar5 = (int)*(undefined4 *)&DAT_000f_1cec + (uint)*_pbStack_10 * 9;
          uStack_1c = (byte *)CONCAT22(uVar8,(byte *)(iVar5 + 1));
          *uStack_1c = *uStack_18 | *uStack_1c & 0xe0;
          uStack_1c = (byte *)CONCAT22(uVar8,(byte *)(iVar5 + 3));
          pbVar1 = uStack_1c;
          uStack_18 = (byte *)CONCAT22(uVar7,pbVar6 + 4);
          pbVar4 = uStack_18;
          uStack_1c = (byte *)CONCAT22(uVar8,(byte *)(iVar5 + 4));
          uStack_18 = (byte *)CONCAT22(uVar7,pbVar6 + 5);
          *pbVar1 = *pbVar4;
          *uStack_1c = *uStack_18;
        }
        _pbStack_10 = (byte *)CONCAT22(uVar7,pbVar6 + 6);
      }
      bVar3 = *(byte *)(*(int *)0x570 * 0x10 + 0x18c);
      if ((bVar3 & 8) == 0) {
        iStack_20 = 1;
      }
      else {
        iStack_20 = 10;
      }
      if ((bVar3 & 4) != 0) {
        iStack_20 = iStack_20 << 1;
      }
      func_0x000fbc84(unaff_CS,(int)*(undefined4 *)&DAT_000f_1cec + 0x8b,
                      (int)((ulong)*(undefined4 *)&DAT_000f_1cec >> 0x10),iStack_20);
      unaff_CS = (char *)0xfbc2;
      func_0x000fbc84(0xfbc2,(int)*(undefined4 *)&DAT_000f_1cec + 0x9d,
                      (int)((ulong)*(undefined4 *)&DAT_000f_1cec >> 0x10),iStack_20);
    }
    pcVar9 = unaff_CS;
    if ((*(uint *)0x186 & 0x10) != 0) {
      pcVar9 = (char *)0xe723;
      func_0x000e7371(unaff_CS);
      *(uint *)0x186 = *(uint *)0x186 & 0xffef;
    }
    if ((*(uint *)0x1e6 & 1) != 0) {
      *(uint *)0x1e6 = *(uint *)0x1e6 & 0xfffe;
    }
    *(char *)0x18a = *(char *)0x18a + '\x01';
    if (*(byte *)0x18b <= *(byte *)0x18a) {
      *(undefined1 *)0x18a = 0;
      uStack_14 = *(undefined2 *)0x570;
      if (((*(uint *)0x186 & 0x40) != 0) && (*(int *)0x570 == 0)) {
        uStack_14 = 4;
      }
      func_0x000fb21f(pcVar9,uStack_14);
      pcVar9 = "";
      func_0x000f7c94((char *)s_TEKTRONIX_2230_000f_fa07 + 5,uStack_14);
    }
    pcVar10 = pcVar9;
    if ((((*(byte *)0x21 & 1) != 0) && (*(char *)0x760 != '\0')) && ((*(uint *)0x22 & 0x400) != 0))
    {
      if ((*(uint *)*(undefined4 *)0x322 & 0x8000) == 0) {
        pcVar10 = (char *)0xe6a8;
        func_0x000e6a8e(pcVar9,8);
      }
      else {
        pcVar10 = (char *)s_unable_to_reset_mi_000f_fcba + 9;
        func_0x000fdc3e(pcVar9);
      }
    }
    if ((*(uint *)0x22 & 1) != 0) {
      func_0x000e6a8e(pcVar10,8);
    }
  }
  else {
    *(byte *)*(undefined4 *)&DAT_000f_1cec = *(byte *)*(undefined4 *)&DAT_000f_1cec & 0xfe;
    pbVar1 = (byte *)((int)*(undefined4 *)&DAT_000f_1cec + 9);
    *pbVar1 = *pbVar1 & 0xfe;
    func_0x000e726a();
  }
  return;
}



void __cdecl16far FUN_000f_aab7(void)

{
  uint uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  byte *pbStack_c;
  
  _pbStack_c = (byte *)CONCAT22(unaff_DS,(byte *)0x18c);
  *(undefined1 *)0x18c = 0;
  if (*(char *)&DAT_000f_1a80 == '\x04') {
    *(byte *)0x18c = *(byte *)0x18c | 1;
    *(undefined2 *)0x194 = 0x1000;
  }
  else {
    *(undefined2 *)0x194 = 0x400;
  }
  if ((*(byte *)0x20 & 0x10) != 0) {
    *_pbStack_c = *_pbStack_c | 4;
    *(int *)0x194 = *(int *)0x194 >> 1;
  }
  if ((*(byte *)&DAT_000f_1a82 & 0x20) != 0) {
    *_pbStack_c = *_pbStack_c | 8;
  }
  if ((*(char *)0x1e == '\x06') || (*(char *)0x1e == '\a')) {
    *_pbStack_c = *_pbStack_c | 0x10;
    *(int *)0x194 = *(int *)0x194 << 1;
  }
  if (((*_pbStack_c & 1) != 0) && ((*(byte *)&DAT_000f_1a82 & 0x10) != 0)) {
    *_pbStack_c = *_pbStack_c | 0x40;
  }
  if ((*(byte *)&DAT_000f_1a85 & 8) == 0) {
    *_pbStack_c = *_pbStack_c | 0x20;
  }
  if (*(char *)&DAT_000f_1a7c != '\0') {
    *_pbStack_c = *_pbStack_c | 0x80;
  }
  if (((*(char *)0x1f != '\0') && ((*_pbStack_c & 0x20) != 0)) &&
     (*(code *)FUN_000f_02fd == (code)0x1)) {
    *_pbStack_c = *_pbStack_c | 2;
  }
  uVar1 = (uint)*(byte *)0x244;
  uVar2 = (uint)*(byte *)0x240;
  func_0x000fadbf();
  func_0x000fac27(0xfaab,0x18c,unaff_DS,*(undefined2 *)0x24,*(undefined1 *)0x1f,uVar2,uVar1);
  *(byte *)&DAT_000f_1b88 = *(byte *)&DAT_000f_1b88 & 0xfc;
  func_0x000fad6e(0xfaab,(undefined *)&DAT_000f_1b24,unaff_DS,*(undefined2 *)&DAT_000f_1a94,
                  *(undefined *)&DAT_000f_1a78,*(undefined *)&DAT_000f_1a79,
                  *(undefined *)&DAT_000f_1a7a);
  func_0x000fad6e(0xfaab,(undefined *)&DAT_000f_1b2e,unaff_DS,*(undefined2 *)&DAT_000f_1a96,
                  *(undefined *)&DAT_000f_1a7b,*(undefined *)&DAT_000f_1a7d,
                  *(undefined *)&DAT_000f_1a7e);
  *(uint *)0x548 = *(uint *)0x548 | 0xc;
  return;
}



void __stdcall16far FUN_000f_ac27(byte *param_1,int param_2,char param_3)

{
  byte *pbVar1;
  undefined2 uVar2;
  int iStack_c;
  int iStack_a;
  
  iStack_a = 0;
  iStack_c = 0;
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  pbVar1 = (byte *)param_1;
  *(int *)(pbVar1 + 6) = param_2;
  if ((*param_1 & 1) == 0) {
    iStack_c = 2;
    *(int *)(pbVar1 + 6) = *(int *)(pbVar1 + 6) << 2;
  }
  if ((*param_1 & 4) != 0) {
    iStack_c = iStack_c + 1;
    iStack_a = -1;
    param_2 = param_2 >> 1;
  }
  if ((*param_1 & 0x10) != 0) {
    iStack_c = iStack_c + -1;
    iStack_a = iStack_a + 1;
    param_2 = param_2 << 1;
  }
  *(int *)(pbVar1 + 10) = iStack_c;
  *(int *)(pbVar1 + 0xc) = iStack_a;
  *(int *)(pbVar1 + 4) = param_2;
  *(int *)(pbVar1 + 0xe) = *(int *)(pbVar1 + 8) + -1;
  if ((param_3 == '\x03') || (param_3 == '\x04')) {
    (pbVar1 + 4)[0] = 0xff;
    (pbVar1 + 4)[1] = 0xff;
    (pbVar1 + 6)[0] = 0xff;
    (pbVar1 + 6)[1] = 0xff;
  }
  return;
}



void __cdecl16far FUN_000f_accc(void)

{
  undefined2 unaff_DS;
  
  if ((*(byte *)0x18c & 4) == 0) {
    if (((*(byte *)0x20 & 1) == 0) && ((*(byte *)0x18c & 0x20) != 0)) {
      *(undefined *)&DAT_000f_1af3 = 2;
      *(byte *)&DAT_000f_1af3 = *(byte *)&DAT_000f_1af3 | 1;
      *(undefined1 *)&DAT_000f_1af2 = 0;
    }
    else {
      *(undefined *)&DAT_000f_1af2 = 2;
      *(byte *)&DAT_000f_1af2 = *(byte *)&DAT_000f_1af2 | 1;
      *(undefined1 *)&DAT_000f_1af3 = 0;
    }
  }
  else {
    *(undefined *)&DAT_000f_1af2 = 2;
    *(byte *)&DAT_000f_1af2 = *(byte *)&DAT_000f_1af2 | 1;
    *(undefined *)&DAT_000f_1af3 = 2;
    *(byte *)&DAT_000f_1af3 = *(byte *)&DAT_000f_1af3 | 1;
  }
  return;
}



void __stdcall16far FUN_000f_ad2e(int param_1)

{
  undefined2 unaff_DS;
  
  if (*(char *)&DAT_000f_1af2 != '\0') {
    if (param_1 < 0) {
      *(byte *)0x1dc = *(byte *)0x1dc & 0xfe;
    }
    else {
      *(byte *)0x1dc = *(byte *)0x1dc | 1;
    }
  }
  if (*(char *)&DAT_000f_1af3 != '\0') {
    if (param_1 < 0) {
      *(byte *)0x1dd = *(byte *)0x1dd & 0xfe;
    }
    else {
      *(byte *)0x1dd = *(byte *)0x1dd | 1;
    }
  }
  return;
}



/* init_print_record_fields (confidence: Confirmed)
   
   Evidence: `(far ptr record, pos, type, attr1, attr2)` - builds a small record: `[0]`/`[2]`=pos
   (dup'd), `[6]`=type; `[4]`/`[5]` get `attr1+attr2` or just `attr2` depending on `attr1` bit
   `0x80` - matches `build_print_record_3532`'s field layout */

void __stdcall16far
init_print_record_fields(word record_off,word record_seg,word pos,byte type,byte attr1,byte attr2)

{
  undefined1 uStack_9;
  
  *_record_off = pos;
  *(word *)(record_off + 2) = pos;
  *(byte *)(record_off + 6) = type;
  if ((attr1 & 0x80) == 0) {
    uStack_9 = attr1 + attr2;
  }
  else {
    uStack_9 = attr2;
  }
  *(byte *)(record_off + 4) = uStack_9;
  *(byte *)(record_off + 5) = uStack_9;
  return;
}



void __stdcall16far
FUN_000f_adbf(undefined4 param_1,char param_2,byte param_3,char param_4,byte param_5,
             undefined1 param_6)

{
  int iVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  *(char *)(iVar1 + 1) = param_2 << 4;
  *(byte *)(iVar1 + 1) = *(byte *)(iVar1 + 1) | param_3;
  *(char *)(iVar1 + 3) = param_4 << 4;
  *(byte *)(iVar1 + 3) = *(byte *)(iVar1 + 3) | param_5;
  *(undefined1 *)(iVar1 + 2) = param_6;
  return;
}



void __stdcall16far FUN_000f_ae0b(byte param_1)

{
  int iVar1;
  undefined2 unaff_DS;
  int *piStack_c;
  
  iVar1 = *(int *)0x3a << (*(byte *)0x16e & 0x1f);
  _piStack_c = (int *)CONCAT22(unaff_DS,(int *)0x64);
  if ((param_1 & 1) != 0) {
    if (*(int *)0x2a < *(int *)0x64 + iVar1) {
      *(int *)0x64 = ((*(int *)0x64 + iVar1) - *(int *)0x2a) + -1;
    }
    else if (*_piStack_c + iVar1 < 0) {
      *_piStack_c = *_piStack_c + iVar1 + *(int *)0x2a + 1;
    }
    else {
      *_piStack_c = *_piStack_c + iVar1;
    }
    *(int *)0x6c = *_piStack_c;
    if ((*(uint *)0x186 & 0x80) != 0) {
      if (*(int *)0x2a < *(int *)0x68 + iVar1) {
        *(int *)0x68 = ((*(int *)0x68 + iVar1) - *(int *)0x2a) + -1;
      }
      else if (*(int *)0x68 + iVar1 < 0) {
        *(int *)0x68 = *(int *)0x68 + iVar1 + *(int *)0x2a + 1;
      }
      else {
        *(int *)0x68 = *(int *)0x68 + iVar1;
      }
    }
  }
  _piStack_c = (int *)CONCAT22(unaff_DS,(int *)0x6e);
  if ((param_1 & 2) != 0) {
    if (*(int *)0x2a < *_piStack_c + iVar1) {
      *_piStack_c = ((*_piStack_c + iVar1) - *(int *)0x2a) + -1;
    }
    else if (*_piStack_c + iVar1 < 0) {
      *_piStack_c = *_piStack_c + iVar1 + *(int *)0x2a + 1;
    }
    else {
      *_piStack_c = *_piStack_c + iVar1;
    }
    *(int *)0x76 = *_piStack_c;
    if ((*(uint *)0x186 & 0x80) != 0) {
      if (*(int *)0x2a < *(int *)0x72 + iVar1) {
        *(int *)0x72 = ((*(int *)0x72 + iVar1) - *(int *)0x2a) + -1;
      }
      else if (*(int *)0x72 + iVar1 < 0) {
        *(int *)0x72 = *(int *)0x72 + iVar1 + *(int *)0x2a + 1;
      }
      else {
        *(int *)0x72 = *(int *)0x72 + iVar1;
      }
    }
  }
  *(undefined2 *)0x3a = 0;
  return;
}



void __stdcall16far
FUN_000f_afc5(byte *param_1,undefined4 param_2,undefined4 param_3,int param_4,int param_5)

{
  byte bVar1;
  byte bVar2;
  byte *pbVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  char *pcVar7;
  uint uStack_a;
  
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  pbVar3 = (byte *)param_1;
  bVar1 = *param_1;
  bVar2 = (byte)*(int *)(pbVar3 + 10);
  if (*(int *)(pbVar3 + 10) < 0) {
    uStack_a = param_4 << (-bVar2 & 0x1f) & 0xfffe;
  }
  else {
    uStack_a = param_4 >> (bVar2 & 0x1f);
  }
  if (*(int *)(pbVar3 + 0xe) < (int)uStack_a) {
    uStack_a = *(uint *)(pbVar3 + 0xe);
  }
  if (((bVar1 & 0x10) != 0) || ((bVar1 & 0x20) == 0)) {
    uStack_a = uStack_a & 0xfffe;
  }
  *(uint *)((int)param_2 + 6) = uStack_a;
  uStack_a = uStack_a - (param_5 + -1);
  if ((bVar1 & 0x10) != 0) {
    uStack_a = ((int)uStack_a >> 1) + 1;
  }
  if ((bVar1 & 8) != 0) {
    uStack_a = uStack_a * 10;
  }
  if (0x3ff < (int)uStack_a) {
    uStack_a = 0x3ff;
  }
  if ((bVar1 & 4) != 0) {
    uStack_a = uStack_a << 1;
  }
  uVar6 = (undefined2)((ulong)param_3 >> 0x10);
  iVar4 = (int)param_3;
  *(byte *)(iVar4 + 1) = *(byte *)(iVar4 + 1) | 1;
  uVar5 = uVar6;
  pcVar7 = (char *)s_ACQ_RAM_even_000f_fef8 + 4 + uStack_a;
  func_0x000fbc84();
  *(byte *)(iVar4 + 0x19) = *(byte *)(iVar4 + 0x19) | 1;
  func_0x000fbc84(0xfbc2,iVar4 + 0x1d,uVar6,(char *)s_ACQ_RAM_even_000f_fef8 + 4 + uStack_a,uVar5,
                  pcVar7);
  return;
}



void __stdcall16far FUN_000f_b0d5(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_DS;
  uint uStack_1c;
  uint uStack_1a;
  int iStack_14;
  undefined4 uStack_10;
  
  if (*(char *)&DAT_000f_1a93 != '\x14') {
    uStack_10 = (byte *)CONCAT22((int)((ulong)*(undefined4 *)&DAT_000f_1cec >> 0x10),
                                 (byte *)((int)*(undefined4 *)&DAT_000f_1cec +
                                         (uint)*(byte *)((int)*(undefined4 *)&DAT_000f_1ba4 +
                                                         param_1 * 0x10 + 4) * 9));
    iVar4 = param_1 * 0x1c;
    iVar1 = *(int *)(iVar4 + 0x57c);
    iVar2 = *(int *)(iVar4 + 0x57e);
    iVar3 = *(int *)(iVar4 + 0x57a);
    iVar4 = *(int *)(iVar4 + 0x588);
    uStack_1a = 0x1000;
    uStack_1c = 0x2000;
    for (iStack_14 = 0; iStack_14 < 2; iStack_14 = iStack_14 + 1) {
      if (((undefined *)&DAT_000f_1af2)[param_1 * 2 + iStack_14] == '\0') {
        *(uint *)0x186 = *(uint *)0x186 & ~uStack_1a;
        *(uint *)0x186 = *(uint *)0x186 & ~uStack_1c;
        *uStack_10 = *uStack_10 & 0xfe;
        uStack_10 = (byte *)CONCAT22(uStack_10._2_2_,(byte *)uStack_10 + 9);
        *uStack_10 = *uStack_10 & 0xfe;
      }
      else {
        if ((iVar3 < iVar1) || (iVar2 < iVar3)) {
          *uStack_10 = *uStack_10 & 0xfe;
          *(uint *)0x186 = *(uint *)0x186 & ~uStack_1a;
        }
        else {
          *uStack_10 = *uStack_10 | 1;
          *(uint *)0x186 = *(uint *)0x186 | uStack_1a;
        }
        uStack_10 = (byte *)CONCAT22(uStack_10._2_2_,(byte *)uStack_10 + 9);
        if ((iVar4 < iVar1) || (iVar2 < iVar4)) {
          *uStack_10 = *uStack_10 & 0xfe;
          *(uint *)0x186 = *(uint *)0x186 & ~uStack_1c;
        }
        else {
          *uStack_10 = *uStack_10 | 1;
          *(uint *)0x186 = *(uint *)0x186 | uStack_1c;
        }
      }
      uStack_10 = (byte *)CONCAT22(uStack_10._2_2_,(byte *)uStack_10 + 9);
      uStack_1a = 0x4000;
      uStack_1c = 0x8000;
    }
  }
  return;
}



void __stdcall16far FUN_000f_b21f(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  char *unaff_CS;
  char *pcVar9;
  undefined2 unaff_DS;
  bool bVar10;
  int iStack_18;
  undefined1 uStack_13;
  undefined2 uStack_a;
  
  iVar7 = param_1 * 2;
  if (param_1 == 0) {
    uStack_13 = *(undefined *)&DAT_000f_1a86;
  }
  else {
    uStack_13 = 0xff;
  }
  iVar1 = *(int *)(param_1 * 0x1c + 0x57a);
  iVar2 = *(int *)((iVar7 + 1) * 0xe + 0x57a);
  bVar10 = (*(byte *)(param_1 * 0x10 + 0x18c) & 0x20) == 0;
  iVar3 = param_1 * 0x28;
  uVar8 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1cec >> 0x10);
  iVar6 = (int)*(undefined4 *)&DAT_000f_1cec;
  iVar4 = param_1 * 0x14;
  if ((*(uint *)0x186 & 0x1000) != 0) {
    if ((iVar1 == 0) && (((undefined *)&DAT_000f_1b2b)[iVar4] != '\0')) {
      uStack_a = *(undefined2 *)((undefined *)&DAT_000f_1b2c + iVar4);
      pcVar9 = unaff_CS;
    }
    else {
      pcVar9 = "";
      uStack_a = func_0x000f830e();
    }
    unaff_CS = (char *)s_TEKTRONIX_2230_000f_fa07 + 5;
    func_0x000fb529(pcVar9,uStack_13,uStack_a,iVar3 + 0xb1,unaff_DS,iVar6 + 0x5c,uVar8);
    if (bVar10) {
      uVar5 = func_0x000f830e((char *)s_TEKTRONIX_2230_000f_fa07 + 5,
                              (int)*(undefined4 *)&DAT_000f_1b90,
                              (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iVar7,iVar1 + 1);
      unaff_CS = (char *)s_TEKTRONIX_2230_000f_fa07 + 5;
      func_0x000fb529(0xf80a,uStack_13,uVar5,iVar3 + 0xb3,unaff_DS,iVar6 + 0x5e,uVar8);
    }
  }
  iStack_18 = iVar3 + 0xb1;
  if ((*(uint *)0x186 & 0x2000) != 0) {
    if ((iVar2 == 0) && (((undefined *)&DAT_000f_1b2b)[iVar4] != '\0')) {
      uStack_a = *(undefined2 *)((undefined *)&DAT_000f_1b2c + iVar4);
      pcVar9 = unaff_CS;
    }
    else {
      pcVar9 = "";
      uStack_a = func_0x000f830e(unaff_CS,(int)*(undefined4 *)&DAT_000f_1b90,
                                 (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iVar7,iVar2);
    }
    unaff_CS = (char *)s_TEKTRONIX_2230_000f_fa07 + 5;
    func_0x000fb529(pcVar9,uStack_13,uStack_a,iStack_18,unaff_DS,iVar6 + 0x65,uVar8);
    if (bVar10) {
      uVar5 = func_0x000f830e((char *)s_TEKTRONIX_2230_000f_fa07 + 5,
                              (int)*(undefined4 *)&DAT_000f_1b90,
                              (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iVar7,iVar2 + 1);
      unaff_CS = (char *)s_TEKTRONIX_2230_000f_fa07 + 5;
      func_0x000fb529(0xf80a,uStack_13,uVar5,iVar3 + 0xb3,unaff_DS,iVar6 + 0x67,uVar8);
    }
  }
  iVar7 = iVar7 + 1;
  iVar4 = iVar7 * 0x14 + 0xb1;
  iVar3 = iVar7 * 10;
  if ((*(uint *)0x186 & 0x4000) != 0) {
    if ((iVar1 == 0) && (((undefined *)&DAT_000f_1b2b)[iVar3] != '\0')) {
      uStack_a = *(undefined2 *)((undefined *)&DAT_000f_1b2c + iVar3);
      pcVar9 = unaff_CS;
    }
    else {
      pcVar9 = "";
      uStack_a = func_0x000f830e(unaff_CS,(int)*(undefined4 *)&DAT_000f_1b90,
                                 (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iVar7,iVar1);
    }
    unaff_CS = (char *)s_TEKTRONIX_2230_000f_fa07 + 5;
    func_0x000fb529(pcVar9,uStack_13,uStack_a,iVar4,unaff_DS,iVar6 + 0x6e,uVar8);
  }
  if ((*(uint *)0x186 & 0x8000) != 0) {
    if ((iVar2 == 0) && (((undefined *)&DAT_000f_1b2b)[iVar3] != '\0')) {
      uStack_a = *(undefined2 *)((undefined *)&DAT_000f_1b2c + iVar3);
      pcVar9 = unaff_CS;
    }
    else {
      pcVar9 = "";
      uStack_a = func_0x000f830e(unaff_CS,(int)*(undefined4 *)&DAT_000f_1b90,
                                 (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),iVar7,iVar2);
    }
    func_0x000fb529(pcVar9,uStack_13,uStack_a,iVar4,unaff_DS,iVar6 + 0x77,uVar8);
  }
  return;
}



void __stdcall16far FUN_000f_b529(char param_1,int param_2,byte *param_3,undefined4 param_4)

{
  undefined2 unaff_CS;
  char *pcStack_a;
  
  if (param_1 == '\0') {
    pcStack_a = (char *)s_ACQ_RAM_even_000f_fef8 + 4 +
                ((uint)((byte *)param_3)[1] << 3 | (int)(uint)*param_3 >> 5) + param_2;
  }
  else {
    unaff_CS = 0xfaa0;
    pcStack_a = (char *)func_0x000faa08();
  }
  func_0x000fbc84(unaff_CS,param_4,pcStack_a);
  return;
}



void __stdcall16far FUN_000f_b593(void)

{
  func_0x000fbc09();
  return;
}



bool __stdcall16far FUN_000f_b5b7(int param_1)

{
  undefined2 unaff_DS;
  
  func_0x000fbc09();
  return *(int *)(param_1 * 4 + 0x3e) == 0;
}



void __stdcall16far FUN_000f_b5fe(int param_1,undefined4 param_2,undefined4 param_3,byte *param_4)

{
  byte *pbVar1;
  byte bVar2;
  int iVar3;
  byte *pbVar4;
  byte *pbVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
  undefined2 uVar11;
  undefined2 uVar12;
  undefined2 unaff_DS;
  undefined4 uStack_1a;
  undefined4 uStack_16;
  byte bStack_e;
  byte bStack_d;
  int iStack_c;
  
  iVar9 = param_1 >> 1;
  uVar11 = (undefined2)((ulong)param_2 >> 0x10);
  iVar7 = (int)param_2;
  uVar12 = (undefined2)((ulong)param_4 >> 0x10);
  ((byte *)param_4)[5] = *(byte *)(iVar7 + 0xf);
  ((byte *)param_4)[6] = *(byte *)(iVar7 + 0x10);
  uVar12 = (undefined2)((ulong)param_3 >> 0x10);
  iVar10 = (int)param_3;
  pbVar4 = (byte *)(iVar10 + *(int *)(iVar7 + 2));
  uStack_16 = (byte *)CONCAT22(uVar12,pbVar4);
  pbVar5 = (byte *)(iVar10 + *(int *)(iVar7 + 4));
  uStack_1a = (byte *)CONCAT22(uVar12,pbVar5);
  if ((*(uint *)0x186 & 0x100) == 0) {
    *uStack_16 = 9;
    *uStack_1a = 9;
  }
  else {
    iStack_c = 1;
    if (*(int *)(iVar7 + 6) - *(int *)(iVar7 + 2) < 1) {
      iStack_c = -1;
    }
    bVar2 = pbVar5[iStack_c];
    *uStack_1a = bVar2 & 3;
    if ((bVar2 & 3) == 2) {
      *uStack_16 = pbVar4[-iStack_c] & 3;
      *uStack_1a = pbVar5[-iStack_c] & 3;
    }
    else {
      *uStack_16 = pbVar4[iStack_c] & 3;
    }
    if (*uStack_16 == 2) {
      *uStack_16 = 9;
    }
    if (*uStack_1a == 2) {
      *uStack_1a = 9;
    }
  }
  if ((*param_4 & 0x10) == 0) {
    *(undefined1 *)(*(int *)(iVar7 + 6) + iVar10) = 9;
  }
  else {
    *(undefined1 *)(*(int *)(iVar7 + 6) + iVar10) = 8;
  }
  *(undefined1 *)(*(int *)(iVar7 + 8) + iVar10) = 10;
  if (((0 < iVar9) && (iVar9 < 4)) || ((*(byte *)&DAT_000f_1a86 & 0x86) != 0)) {
    if ((*param_4 & 4) == 0) {
      if ((*param_4 & 8) == 0) {
        bStack_d = 2;
        bStack_e = 2;
      }
      else {
        bStack_d = 1;
        bStack_e = 0;
      }
    }
    else {
      bStack_d = 2;
      bStack_e = 1;
    }
    iVar3 = *(int *)&DAT_000f_1b12;
    uVar6 = (uint)bStack_e;
    uVar8 = (uint)bStack_d;
    func_0x000fbd86();
    func_0x000fbd86(0xfbd8,(iVar10 - iVar3) + *(int *)(iVar7 + 4),uVar12,bStack_d,bStack_e,uVar8,
                    uVar6);
  }
  *(undefined2 *)(iVar7 + 2) = *(undefined2 *)(iVar7 + 6);
  *(undefined2 *)(iVar7 + 4) = *(undefined2 *)(iVar7 + 8);
  if ((*(uint *)0x186 & 1) != 0) {
    if (iVar9 == 0) {
      *(byte *)(*(int *)0x172 + iVar10) = *(byte *)(*(int *)0x172 + iVar10) | 4;
    }
    else if (iVar9 == 4) {
      pbVar1 = (byte *)(*(int *)(code *)scale_and_plot_point + iVar10);
      *pbVar1 = *pbVar1 | 4;
    }
  }
  return;
}



void __stdcall16far FUN_000f_b832(undefined1 param_1,undefined1 *param_2)

{
  *param_2 = param_1;
  return;
}



void __stdcall16far FUN_000f_b847(int param_1,int param_2,undefined2 param_3)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined2 local_e;
  byte *pbStack_c;
  
  iVar1 = param_1 * 0x14;
  _pbStack_c = (byte *)CONCAT22(unaff_DS,(byte *)(iVar1 + 0xa6));
  if ((*(byte *)((param_1 >> 1) * 0x10 + 0x18c) & 0x10) == 0) {
    *(int *)(iVar1 + 0xac) = param_2;
  }
  else {
    *(int *)(iVar1 + 0xac) = param_2 + 1;
  }
  *(undefined2 *)(iVar1 + 0xae) = param_3;
  local_e = func_0x000fbc2f();
  func_0x000fbc4d(0xfbc2,&local_e);
  *_pbStack_c = *_pbStack_c | 2;
  return;
}



/* clear_readout_attrs_and_flag_dirty (confidence: Confirmed)
   
   Evidence: `(skip_clear)` - unless `skip_clear!=0`, picks a mode (`0`/`1`/`2`) from `[0x18C]` bits
   `0x10`/`0x20` and calls `clear_attr_bits_at_prev_delimiter` twice (whole buffer, then one item's
   end position); either way, unconditionally ORs `[0x22]\|=0x1000` (sibling of the `0x2000` bit
   `compute_readout_buffer_length_and_flag` sets) to flag the readout content changed. Called with
   `skip_clear=0` from `finish_acq_reset_and_clear_mode` */

void __stdcall16far clear_readout_attrs_and_flag_dirty(word skip_clear)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 uStack_e;
  byte *pbStack_c;
  
  _pbStack_c = (byte *)CONCAT22(unaff_DS,(byte *)0x18c);
  if (skip_clear == 0) {
    if ((*(byte *)0x18c & 0x10) == 0) {
      if ((*_pbStack_c & 0x20) == 0) {
        uStack_e = 2;
      }
      else {
        uStack_e = 0;
      }
    }
    else {
      uStack_e = 1;
    }
    uVar1 = 0x2000;
    uVar2 = uStack_e;
    func_0x000fbd86();
    func_0x000fbd86(0xfbd8,(int)*(undefined4 *)&DAT_000f_1b90 +
                           *(int *)((int)*(undefined4 *)&DAT_000f_1ba4 + 0x40),
                    (int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),0x800,uStack_e,uVar1,uVar2);
  }
  else {
    *(uint *)0x22 = *(uint *)0x22 | 0x1000;
  }
  return;
}



void __stdcall16far FUN_000f_b9f3(byte param_1,undefined *param_2)

{
  int iVar1;
  undefined2 unaff_DS;
  int iStack_14;
  int iStack_e;
  undefined *puStack_a;
  
  if (((*(char *)0x1f == '\x03') || (*(char *)0x1f == '\a')) || (*(char *)0x1f == '\x05')) {
    if ((param_1 & 4) == 0) {
      puStack_a = (undefined *)0x1100;
    }
    else {
      puStack_a = (undefined *)0x880;
    }
  }
  else {
    puStack_a = param_2;
  }
  func_0x000fbb24();
  iVar1 = *(int *)((int)*(undefined4 *)&DAT_000f_1ba4 + 0x40);
  *(int *)0x5c = iVar1 + (*(int *)0x3c >> 2);
  if ((*(char *)&DAT_000f_1a93 == '\x1e') || ((param_1 & 0x10) == 0)) {
    *(int *)0x5e = *(int *)0x3e >> 2;
    *(int *)0x60 = iVar1 + (*(int *)0x40 >> 2);
    *(int *)0x62 = *(int *)0x42 >> 2;
  }
  else {
    *(int *)0x5e = *(int *)0x3e >> 3;
    *(int *)0x60 = iVar1 + (*(int *)0x40 >> 3);
    *(int *)0x62 = *(int *)0x42 >> 3;
  }
  func_0x000fbbe9(0xfb9f,100);
  *(int *)0x2a = (int)(puStack_a + -1);
  iStack_14 = 100;
  for (iStack_e = 0; iStack_e < 4; iStack_e = iStack_e + 1) {
    *(int *)(iStack_14 + 2) = (int)(puStack_a + -1);
    iStack_14 = iStack_14 + 10;
  }
  return;
}



void __stdcall16far
FUN_000f_bb24(byte param_1,char param_2,char param_3,int param_4,int param_5,undefined2 param_6)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined4 uStack_10;
  undefined2 uStack_c;
  
  iVar1 = (uint)param_1 * 8;
  _uStack_c = (int *)CONCAT22(unaff_DS,(int *)(iVar1 + 0x3c));
  uStack_10 = (int *)CONCAT22(unaff_DS,(int *)(iVar1 + 0x40));
  if (param_2 == '\0') {
    if (param_3 == '\0') {
      *_uStack_c = param_4;
      *(int *)(iVar1 + 0x3e) = param_4;
      *uStack_10 = param_4;
      *(undefined2 *)(iVar1 + 0x42) = param_6;
    }
    else {
      *_uStack_c = param_4;
      *(undefined2 *)(iVar1 + 0x3e) = param_6;
      *uStack_10 = param_4;
      *(int *)(iVar1 + 0x42) = param_4;
    }
  }
  else {
    *_uStack_c = param_4;
    *(undefined2 *)(iVar1 + 0x3e) = param_6;
    *uStack_10 = param_4 + param_5;
    *(undefined2 *)(iVar1 + 0x42) = param_6;
  }
  return;
}



/* memset_word_pattern_far (confidence: Confirmed)
   
   Evidence: `(far_ptr dest, byte_count, fill_lo, fill_hi)` - word-granularity sibling of the
   confirmed `memset_far`: `rep stosw` filling `byte_count/2` words with the packed
   `fill_hi:fill_lo` byte pair - called from `pad_readout_buffer_remainder` to pad the readout
   buffer's tail with a repeating `0x0101` pattern */

void __stdcall16far
memset_word_pattern_far(word dest_off,word dest_seg,word byte_count,byte fill_lo,byte fill_hi)

{
  undefined2 *puVar1;
  uint uVar2;
  
  for (uVar2 = byte_count >> 1; uVar2 != 0; uVar2 = uVar2 - 1) {
    puVar1 = (undefined2 *)dest_off;
    dest_off = dest_off + 2;
    *puVar1 = CONCAT11(fill_hi,fill_lo);
  }
  return;
}



/* memcpy_far (confidence: Confirmed)
   
   Evidence: `(src_off, src_seg, dest_off, dest_seg, count)` - generic memcpy-style utility: `lds
   si,[bp+6]; les di,[bp+0xa]; mov cx,[bp+0xe]; shr cx,1; rep movsw` */

void __stdcall16far memcpy_far(word src_off,word src_seg,word dest_off,word dest_seg,word count)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  uint uVar3;
  
  for (uVar3 = count >> 1; uVar3 != 0; uVar3 = uVar3 - 1) {
    puVar2 = (undefined2 *)dest_off;
    dest_off = dest_off + 2;
    puVar1 = (undefined2 *)src_off;
    src_off = src_off + 2;
    *puVar2 = *puVar1;
  }
  for (uVar3 = count & 1; uVar3 != 0; uVar3 = uVar3 - 1) {
    puVar2 = (undefined2 *)dest_off;
    dest_off = dest_off + 1;
    puVar1 = (undefined2 *)src_off;
    src_off = src_off + 1;
    *(undefined1 *)puVar2 = *(undefined1 *)puVar1;
  }
  return;
}



/* array_index_16 (confidence: Confirmed)
   
   Evidence: `(base, index) -> base + index*16` **(corrected 2026-09-16 - `base` is a plain word
   used directly, not a far pointer; `retf 4` proves only 2 words of arguments)** - 16-byte-record
   array indexing */

int __stdcall16far array_index_16(word base,word index)

{
  return base + index * 0x10;
}



/* copy_word_far (confidence: Confirmed)
   
   Evidence: `(src_off, src_seg, dest_off, dest_seg)` - copies one word from a far source pointer to
   a far destination pointer */

void __stdcall16far copy_word_far(word src_off,word src_seg,word dest_off,word dest_seg)

{
  *_dest_off = *_src_off;
  return;
}



/* pack_low5_bits (confidence: Confirmed)
   
   Evidence: `(target_off, target_seg, value)` - packs the low 5 bits of `value` into `*target`,
   preserving its high 3 bits - same pattern as `pack_row_col_bits` but a separate `160-3532`
   implementation */

void __stdcall16far pack_low5_bits(word target_off,word target_seg,byte value)

{
  *_target_off = *_target_off & 0xe0 | value;
  return;
}



/* set_position_record_3532 (confidence: Confirmed)
   
   Evidence: `(record_off, record_seg, coord)` - `coord>>3` (character-cell scaling) written across
   2 record bytes - same pattern as `set_position_record` but in `160-3532`, and only ever encodes
   one coordinate per call unlike its sibling's two */

void __stdcall16far set_position_record_3532(word record_off,word record_seg,word coord)

{
  undefined1 *puVar1;
  
  puVar1 = _record_off;
  _record_off = (byte *)CONCAT22(record_seg,(byte *)(record_off - 1));
  *puVar1 = (char)((int)coord >> 3);
  *_record_off = *_record_off & 0x1f;
  *_record_off = *_record_off | (byte)((coord & 7) << 5);
  return;
}



/* init_default_print_cell_dimensions (confidence: Mechanism confirmed; exact field meanings not
   confirmed)
   
   Evidence: Writes 5 fixed byte constants to `[0x30]`-`[0x34]` (`9, 0xA, 0x14, 0x10, 0xA`) -
   `[0x34]` specifically is read as a per-row `memcpy_far` stride (`*0xA`) by the print-record
   character-row copier near `0xEF3D6`-`0xEF440`, consistent with these being default print-record
   cell width/height fields; called from `compute_readout_buffer_length_and_flag` right before it
   sets up the far ptr `[0x36]`/`[0x38]` */

void __cdecl16far init_default_print_cell_dimensions(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x30 = 9;
  *(undefined1 *)0x31 = 10;
  *(undefined1 *)0x33 = 0x14;
  *(undefined1 *)0x32 = 0x10;
  *(undefined1 *)0x34 = 10;
  return;
}



/* compute_readout_buffer_length_and_flag (confidence: Mechanism confirmed; `SUB_FBCCF`/`SUB_F911C`
   not yet traced)
   
   Evidence: Computes the linear-address delta between far pointers `[0x1C84]` and `[0x1C80]` (via
   `seg_off_to_linear` x2) into `[0x1C02]` - the same running length/position value read by
   `read_acq_sample_with_wrap` and the "TEKTRONIX" logo builder `SUB_F5898`; builds a far ptr to the
   buffer's current end at `[0x36]`/`[0x38]`, calls `SUB_FBCCF`, then sets flag bit
   `[0x22]\|=0x2000` (sibling of the `0x1000` bit set elsewhere in the same report-screen flow) */

void compute_readout_buffer_length_and_flag(void)

{
  undefined4 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 uVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 unaff_DS;
  
  uVar6 = (undefined2)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10);
  uVar4 = (undefined2)*(undefined4 *)&DAT_000f_1b90;
  uVar2 = func_0x000e6d2f();
  iVar5 = (int)*(undefined4 *)0x1c84;
  iVar3 = func_0x000e6d2f(0xe6a8,iVar5,(int)((ulong)*(undefined4 *)0x1c84 >> 0x10),uVar2,uVar4,uVar6
                         );
  *(int *)&DAT_000f_1b12 = iVar3 - iVar5;
  uVar1 = *(undefined4 *)&DAT_000f_1b90;
  *(int *)0x36 = (int)uVar1 + *(int *)&DAT_000f_1b12;
  *(undefined2 *)0x38 = (int)((ulong)uVar1 >> 0x10);
  func_0x000fbccf(0xe6a8);
  *(uint *)0x22 = *(uint *)0x22 | 0x2000;
  func_0x000f911c(0xfbcc);
  return;
}



/* clear_attr_bits_at_prev_delimiter (confidence: Mechanism confirmed; exact meaning of the cleared
   attribute bits not confirmed)
   
   Evidence: `(far_ptr buf, start, mode)` - scans backward from `start` through the readout text
   buffer `[0x1C80]` for a `0`/`0xFF` delimiter byte, using a byte/word/dword stride selected by
   `mode` (`0`/`1`/else); once found, clears bits 0-1 of the paired attribute-plane byte (same
   offset, `ES=DS+0x800`) if bit 0 was set - called twice from `clear_readout_attrs_and_flag_dirty`,
   once scanning the whole buffer (`0x2000`) and once a specific item's end position from the
   `[0x1C94]` item table */

undefined4 __stdcall16far
clear_attr_bits_at_prev_delimiter(word buf_off,word buf_seg,word start,byte mode)

{
  byte *pbVar1;
  undefined2 in_AX;
  int iVar2;
  undefined2 in_DX;
  char *pcVar3;
  uint uVar4;
  bool bVar6;
  bool bVar7;
  uint uVar5;
  
  iVar2 = buf_seg + 0x800;
  if (mode == 0) {
    while (bVar6 = start != 0, start = start - 1, bVar6) {
      if (((*(char *)(buf_off + start) == '\0') || (*(char *)(buf_off + start) == -1)) &&
         ((*(byte *)(buf_off + start) & 1) != 0)) {
        *(byte *)(buf_off + start) = *(byte *)(buf_off + start) & 0xfc;
      }
    }
  }
  else {
    if (mode == 1) {
      buf_off = buf_off & 0xfffe;
      while( true ) {
        bVar6 = start < 2;
        start = start - 2;
        bVar7 = start != 0;
        if (bVar6) break;
        if (((*(char *)(buf_off + start + 1) == '\0') || (*(char *)(buf_off + start + 1) == -1)) &&
           ((*(byte *)(buf_off + start + 1) & 1) != 0)) {
          pbVar1 = (byte *)(buf_off + start + 1);
          *pbVar1 = *pbVar1 & 0xfc;
        }
      }
      start = 0;
      if (bVar7) goto LAB_000f_be59;
    }
    pcVar3 = (char *)(buf_off & 0xfffe);
    uVar4 = start - 2;
    while (uVar5 = uVar4, uVar4 = uVar5 - 2, 1 < uVar5) {
      if ((((((pcVar3[uVar4] == '\0') && (pcVar3[uVar5] == '\0')) ||
            ((pcVar3[uVar4] == -1 && (pcVar3[uVar5] == -1)))) ||
           ((pcVar3[uVar5 - 1] == '\0' && (pcVar3[uVar5 + 1] == '\0')))) ||
          ((pcVar3[uVar5 - 1] == -1 && (pcVar3[uVar5 + 1] == -1)))) &&
         ((pcVar3[uVar5 + 1] & 1U) != 0)) {
        pcVar3[uVar5 + 1] = pcVar3[uVar5 + 1] & 0xfc;
      }
    }
    if (((((*pcVar3 == '\0') || (*pcVar3 == -1)) || (pcVar3[1] == '\0')) || (pcVar3[1] == -1)) &&
       ((pcVar3[1] & 1U) != 0)) {
      pcVar3[1] = pcVar3[1] & 0xfc;
    }
  }
LAB_000f_be59:
  return CONCAT22(in_DX,in_AX);
}



int __stdcall16far FUN_000f_be64(int *param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  int iStack_a;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  piVar2 = (int *)param_1;
  if (piVar2[3] < *param_1) {
    iStack_a = (param_3 - *param_1) + piVar2[3] + 1;
  }
  else {
    iStack_a = piVar2[3] - *param_1;
  }
  *(int *)0x188 = iStack_a;
  iVar1 = (*(int *)0x2a + 1) - *(int *)((int)param_2 + 2) << (*(byte *)0x16f & 0x1f);
  if (iVar1 < iStack_a) {
    iStack_a = iVar1;
  }
  if (*(int *)0x2e < iStack_a) {
    iStack_a = *(int *)0x2e;
  }
  return iStack_a;
}



void __stdcall16far
FUN_000f_c2a9(byte *param_1,undefined2 param_2,byte *param_3,undefined2 param_4,int param_5,
             undefined2 param_6,undefined2 *param_7,undefined2 param_8,int param_9)

{
  undefined2 uVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  byte *pbVar5;
  undefined2 unaff_SS;
  
  pbVar5 = (byte *)*param_7;
  uVar1 = param_7[1];
  do {
    bVar2 = *param_1;
    bVar4 = param_1[param_9];
    bVar3 = bVar2;
    if (bVar4 < bVar2) {
      bVar3 = bVar4;
      bVar4 = bVar2;
    }
    if (bVar3 < *pbVar5) {
      *pbVar5 = bVar3;
    }
    if (pbVar5[1] < bVar4) {
      pbVar5[1] = bVar4;
    }
    pbVar5 = pbVar5 + 2;
    param_1 = param_1 + param_9 + param_5;
  } while (param_1 <= param_3);
  *param_7 = pbVar5;
  return;
}



void __stdcall16far
FUN_000f_c2f3(byte *param_1,undefined2 param_2,byte *param_3,undefined2 param_4,int param_5,
             uint param_6,uint *param_7,undefined2 param_8,char param_9)

{
  uint uVar1;
  byte bVar2;
  uint uVar3;
  byte *pbVar4;
  byte *pbVar5;
  undefined2 unaff_SS;
  
  uVar1 = param_7[1];
  pbVar4 = (byte *)(*param_7 & 0xfffe);
  pbVar5 = pbVar4;
  if (param_9 == '\0') {
    do {
      bVar2 = *param_1;
      if (bVar2 < *pbVar5) {
        *pbVar5 = bVar2;
      }
      if (pbVar5[1] < bVar2) {
        pbVar5[1] = bVar2;
      }
      pbVar4 = pbVar4 + param_6;
      pbVar5 = (byte *)((uint)pbVar4 & 0xfffe);
      param_1 = param_1 + param_5;
    } while (param_1 <= param_3);
  }
  else {
    uVar3 = 0;
    do {
      bVar2 = *param_1;
      if (bVar2 < *pbVar5) {
        *pbVar5 = bVar2;
      }
      if (pbVar5[1] < bVar2) {
        pbVar5[1] = bVar2;
      }
      param_6 = param_6 & 0xff;
      uVar3 = uVar3 + param_6;
      pbVar5 = (byte *)((uint)(pbVar4 + (uVar3 >> 1)) & 0xfffe);
      param_1 = param_1 + param_5;
    } while (param_1 <= param_3);
  }
  *param_7 = (uint)pbVar5;
  return;
}



void __stdcall16far
FUN_000f_c374(undefined1 *param_1,undefined2 param_2,undefined1 *param_3,undefined2 param_4,
             int param_5,int param_6,undefined2 *param_7,undefined2 param_8,int param_9)

{
  undefined2 uVar1;
  undefined1 *puVar2;
  undefined2 unaff_SS;
  
  puVar2 = (undefined1 *)*param_7;
  uVar1 = param_7[1];
  if (param_9 == 0) {
    while( true ) {
      if (((uint)puVar2 & 1) != 0) {
        *puVar2 = *param_1;
      }
      param_1 = param_1 + param_5;
      if (param_3 < param_1) break;
      puVar2 = puVar2 + param_6;
    }
  }
  else {
    while( true ) {
      if (((uint)puVar2 & 1) == 0) {
        *puVar2 = *param_1;
      }
      param_1 = param_1 + param_5;
      if (param_3 < param_1) break;
      puVar2 = puVar2 + param_6;
    }
  }
  *param_7 = puVar2;
  return;
}



undefined2 __stdcall16far
FUN_000f_c3e6(undefined2 *param_1,undefined2 param_2,undefined2 *param_3,undefined2 param_4,
             undefined2 param_5,undefined2 param_6,undefined2 *param_7)

{
  undefined2 uVar1;
  undefined2 *puVar2;
  undefined2 unaff_SS;
  
  puVar2 = (undefined2 *)*param_7;
  uVar1 = param_7[1];
  do {
    *puVar2 = CONCAT11((char)*param_1,(char)((uint)*param_1 >> 8));
    param_1 = param_1 + 1;
    puVar2 = puVar2 + 1;
  } while (param_1 <= param_3);
  *param_7 = puVar2;
  return param_5;
}



undefined2 __stdcall16far
FUN_000f_c420(undefined2 *param_1,undefined2 param_2,undefined2 *param_3,undefined2 param_4,
             int param_5,undefined2 param_6,undefined2 *param_7)

{
  undefined2 uVar1;
  undefined2 *puVar2;
  undefined2 *puVar3;
  undefined2 unaff_SS;
  
  puVar3 = (undefined2 *)*param_7;
  uVar1 = param_7[1];
  do {
    puVar2 = puVar3;
    puVar3 = puVar3 + 1;
    *puVar2 = *param_1;
    param_1 = (undefined2 *)((int)param_1 + param_5 + 1);
  } while (param_1 <= param_3);
  *param_7 = puVar3;
  return param_2;
}



undefined2 __stdcall16far
FUN_000f_c453(undefined1 *param_1,undefined2 param_2,undefined1 *param_3,undefined2 param_4,
             int param_5,uint param_6,undefined2 *param_7,undefined2 param_8,byte param_9)

{
  undefined2 uVar1;
  undefined1 *puVar2;
  undefined2 unaff_SS;
  
  puVar2 = (undefined1 *)*param_7;
  uVar1 = param_7[1];
  do {
    *puVar2 = *param_1;
    param_1 = param_1 + param_5;
    puVar2 = puVar2 + (param_6 >> (param_9 & 0x1f));
  } while (param_1 <= param_3);
  *param_7 = puVar2;
  return param_2;
}



void __stdcall16far
FUN_000f_c493(undefined1 *param_1,undefined2 param_2,undefined1 *param_3,undefined2 param_4,
             int param_5,int param_6,undefined2 *param_7,undefined2 param_8,byte param_9)

{
  undefined2 uVar1;
  undefined1 *puVar2;
  uint uVar3;
  undefined1 *puVar4;
  undefined2 unaff_SS;
  
  puVar2 = (undefined1 *)*param_7;
  uVar1 = param_7[1];
  uVar3 = 0;
  puVar4 = puVar2;
  do {
    *puVar4 = *param_1;
    param_1 = param_1 + param_5;
    uVar3 = uVar3 + param_6;
    puVar4 = puVar2 + (uVar3 >> (param_9 & 0x1f));
  } while (param_1 <= param_3);
  *param_7 = puVar4;
  return;
}



void __stdcall16far
FUN_000f_c4d8(byte *param_1,undefined2 param_2,byte *param_3,undefined2 param_4,int param_5,
             undefined2 param_6,undefined2 *param_7,undefined2 param_8,char param_9)

{
  undefined2 uVar1;
  uint uVar2;
  uint *puVar3;
  undefined2 unaff_SS;
  
  puVar3 = (uint *)*param_7;
  uVar1 = param_7[1];
  if ((char)(param_9 - 1U) < '\0') {
    do {
      *puVar3 = (uint)*param_1 << 8;
      puVar3 = puVar3 + 1;
      param_1 = param_1 + param_5;
    } while (param_1 <= param_3);
  }
  else {
    do {
      uVar2 = *puVar3;
      *puVar3 = ((int)(CONCAT12((uint)*param_1 * 0x100 < uVar2,(uint)*param_1 * 0x100 - uVar2) >> 1)
                >> (param_9 - 1U & 0x1f)) + uVar2;
      puVar3 = puVar3 + 1;
      param_1 = param_1 + param_5;
    } while (param_1 <= param_3);
  }
  *param_7 = puVar3;
  return;
}



void __stdcall16far
FUN_000f_c534(byte *param_1,undefined2 param_2,byte *param_3,undefined2 param_4,int param_5,
             int param_6,int *param_7,undefined2 param_8,char param_9,byte param_10)

{
  int iVar1;
  uint *puVar2;
  uint uVar3;
  uint *puVar4;
  uint uVar5;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  uVar5 = *(uint *)0x316;
  puVar2 = (uint *)*param_7;
  iVar1 = param_7[1];
  puVar4 = puVar2;
  if ((char)(param_9 - 1U) < '\0') {
    do {
      *puVar4 = (uint)*param_1 << 8;
      uVar5 = uVar5 + param_6;
      puVar4 = (uint *)((uVar5 >> (param_10 & 0x1f) & 0xfffe) + (int)puVar2);
      param_1 = param_1 + param_5;
    } while (param_1 <= param_3);
  }
  else {
    do {
      uVar3 = *puVar4;
      *puVar4 = ((int)(CONCAT12((uint)*param_1 * 0x100 < uVar3,(uint)*param_1 * 0x100 - uVar3) >> 1)
                >> (param_9 - 1U & 0x1f)) + uVar3;
      uVar5 = uVar5 + param_6;
      puVar4 = (uint *)((uVar5 >> (param_10 & 0x1f) & 0xfffe) + (int)puVar2);
      param_1 = param_1 + param_5;
    } while (param_1 <= param_3);
  }
  *(uint *)0x316 = uVar5 >> (param_10 & 0x1f) & 1;
  *param_7 = (int)puVar4;
  return;
}



void __stdcall16far
FUN_000f_c5d1(byte *param_1,undefined2 param_2,byte *param_3,undefined2 param_4,undefined2 *param_5)

{
  undefined2 uVar1;
  byte bVar2;
  byte *pbVar3;
  undefined2 unaff_SS;
  
  pbVar3 = (byte *)*param_5;
  uVar1 = param_5[1];
  do {
    bVar2 = *param_1;
    if (param_1[2] < bVar2) {
      bVar2 = param_1[2];
    }
    *pbVar3 = bVar2;
    bVar2 = param_1[1];
    if (bVar2 < param_1[3]) {
      bVar2 = param_1[3];
    }
    pbVar3[1] = bVar2;
    param_1 = param_1 + 4;
    pbVar3 = pbVar3 + 2;
  } while (param_1 <= param_3);
  *param_5 = pbVar3;
  return;
}



/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* ack_comm_hw_status_bits (confidence: Confirmed)
   
   Evidence: `(req_byte_ptr, out_status_ptr)` **(corrected argument order 2026-09-16 - was listed
   reversed)** - reads the comm-option hw status register at physical `0x40377E` into
   `*out_status_ptr`; for each of 3 request bits (`0x80`/`0x20`/`0x40`) in `*req_byte_ptr`, if both
   the request bit and the matching status bit are set, clears it and writes the updated byte to the
   comm-option hw control register at physical `0x407DE` (same pair as `detect_comm_option_hw`);
   returns a bitmask of acknowledged bits */

uint __stdcall16far ack_comm_hw_status_bits(word req_byte_ptr,word out_status_ptr)

{
  uint uVar1;
  uint uVar2;
  byte bVar3;
  undefined2 unaff_DS;
  uint *in_stack_00000008;
  
  uVar1 = _DAT_4000_377e;
  *in_stack_00000008 = _DAT_4000_377e;
  bVar3 = *(byte *)req_byte_ptr;
  uVar2 = 0;
  if (((bVar3 & 0x80) != 0) && ((uVar1 & 0x8000) == 0)) {
    bVar3 = bVar3 & 0x7f;
    uVar2 = 1;
    DAT_4000_07de = bVar3;
  }
  if (((bVar3 & 0x20) != 0) && ((uVar1 & 0x2000) != 0)) {
    bVar3 = bVar3 & 0xdf;
    uVar2 = uVar2 | 4;
    DAT_4000_07de = bVar3;
  }
  if (((bVar3 & 0x40) != 0) && ((uVar1 & 0x4000) != 0)) {
    bVar3 = bVar3 & 0xbf;
    uVar2 = uVar2 | 2;
    DAT_4000_07de = bVar3;
  }
  *(byte *)req_byte_ptr = bVar3;
  return uVar2;
}



void __cdecl16far FUN_000f_c680(void)

{
  undefined1 uVar1;
  undefined2 unaff_DS;
  
  func_0x000fe10a();
  func_0x000fc6c4((char *)s_not_installed_000f_fd1f + 1);
  *(undefined1 *)0x242 = *(undefined *)&DAT_000f_1a83;
  *(undefined1 *)0x243 = *(undefined *)&DAT_000f_1a82;
  *(undefined1 *)0x244 = *(undefined *)&DAT_000f_1a9c;
  *(undefined1 *)0x241 = *(undefined1 *)0x1f;
  uVar1 = func_0x000fc8c5(0xfc68,*(undefined *)&DAT_000f_1a85);
  *(undefined1 *)0x240 = uVar1;
  func_0x000fe1e1(0xfc68);
  func_0x000fc78e((char *)s_not_installed_000f_fd1f + 1);
  return;
}



void __cdecl16far FUN_000f_c6c4(void)

{
  undefined1 uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x20 = 0;
  *(undefined1 *)0x21 = 0;
  *(uint *)0x22 = *(uint *)0x22 & 0xf000;
  uVar2 = (uint)*(byte *)&DAT_000f_1a83;
  uVar1 = func_0x000fc7f4();
  *(undefined1 *)0x1f = uVar1;
  if ((*(byte *)&DAT_000f_1a85 & 8) == 0) {
    if (*(char *)0x1f == '\a') {
      *(undefined1 *)0x1e = 2;
    }
    else {
      uVar1 = func_0x000fc971(0xfc68,*(undefined1 *)0x1f,*(undefined *)&DAT_000f_1a9a,uVar2);
      *(undefined1 *)0x1e = uVar1;
    }
  }
  else if (*(char *)0x1f == '\0') {
    *(undefined1 *)0x1e = 9;
  }
  else {
    *(undefined1 *)0x1e = 8;
  }
  if (((*(char *)&DAT_000f_1a85 == '\x03') && (*(char *)&DAT_000f_1a9a == '\x01')) &&
     (*(char *)0x1f != '\x03')) {
    *(undefined1 *)0x1e = 10;
  }
  if (((*(char *)0x1e == '\x02') || (*(char *)0x1e == '\x05')) || (*(char *)0x1e == '\n')) {
    *(byte *)0x20 = *(byte *)0x20 | 4;
  }
  if ((2 < *(byte *)0x1f) && (*(byte *)0x1f < 8)) {
    *(byte *)0x20 = *(byte *)0x20 | 0x40;
  }
  return;
}



void __cdecl16far FUN_000f_c78e(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)&DAT_000f_1a9a == '\x03') {
    if ((*(byte *)0x20 & 0x10) == 0) {
      *(undefined2 *)0x2e = 0xc;
    }
    else {
      *(undefined2 *)0x2e = 0x18;
    }
  }
  else if (*(char *)&DAT_000f_1a9a == '\x02') {
    if ((*(byte *)0x20 & 0x10) == 0) {
      *(undefined2 *)0x2e = 0x18;
    }
    else {
      *(undefined2 *)0x2e = 0x24;
    }
  }
  else if ((*(byte *)0x20 & 0x10) == 0) {
    *(undefined2 *)0x2e = 0x30;
  }
  else {
    *(undefined2 *)0x2e = 0x50;
  }
  return;
}



int __stdcall16far FUN_000f_c7f4(char param_1,byte param_2,byte param_3)

{
  undefined2 unaff_DS;
  int iStack_a;
  
  if (param_2 < 0x13) {
    if (param_2 < 8) {
      if (param_2 < 6) {
        iStack_a = 0;
      }
      else {
        iStack_a = 1;
      }
    }
    else {
      iStack_a = 2;
    }
  }
  else if ((param_3 & 2) == 0) {
    if (param_1 == '\0') {
      if ((param_3 & 1) == 0) {
        iStack_a = 6;
      }
      else {
        iStack_a = 4;
      }
    }
    else {
      iStack_a = 3;
    }
  }
  else {
    iStack_a = 5;
  }
  if ((iStack_a == 5) &&
     ((*(char *)&DAT_000f_1a9a == '\x02' || (*(char *)&DAT_000f_1a9a == '\x03')))) {
    iStack_a = 6;
  }
  if (((*(byte *)&DAT_000f_1a85 & 8) != 0) && (iStack_a == 3)) {
    iStack_a = 4;
  }
  if (((*(byte *)&DAT_000f_1a85 & 8) != 0) && (iStack_a == 5)) {
    iStack_a = 6;
  }
  if ((iStack_a == 5) && ((*(char *)&DAT_000f_1a93 == '\x14' || (*(char *)0x1c == '\x14')))) {
    iStack_a = 7;
  }
  return iStack_a;
}



byte __stdcall16far FUN_000f_c8c5(byte param_1)

{
  undefined2 unaff_DS;
  byte bStack_9;
  
  bStack_9 = param_1;
  if ((param_1 & 8) != 0) {
    bStack_9 = 8;
  }
  if (((*(char *)0x241 == '\0') || (*(char *)0x244 == '\x06')) && (bStack_9 == 7)) {
    bStack_9 = 3;
  }
  if ((*(char *)0x241 == '\x03') && (bStack_9 == 3)) {
    bStack_9 = 7;
  }
  if ((bStack_9 == 0) || (bStack_9 == 1)) {
    *(byte *)0x20 = *(byte *)0x20 | 1;
  }
  else if (bStack_9 == 2) {
    *(byte *)0x20 = *(byte *)0x20 | 2;
  }
  else if (bStack_9 == 3) {
    *(byte *)0x20 = *(byte *)0x20 | 0x92;
  }
  else if (bStack_9 == 7) {
    *(byte *)0x20 = *(byte *)0x20 | 0x1b;
  }
  else if ((bStack_9 == 8) && (*(byte *)0x20 = *(byte *)0x20 | 1, *(char *)0x241 == '\0')) {
    *(byte *)0x20 = *(byte *)0x20 | 0x80;
  }
  return bStack_9;
}



void __stdcall16far FUN_000f_c971(void)

{
  return;
}



void __cdecl16far FUN_000f_ca35(void)

{
  undefined2 unaff_DS;
  undefined1 uStack_b;
  undefined1 uStack_a;
  undefined1 uStack_9;
  
  if ((*(byte *)0x20 & 2) == 0) {
    uStack_a = 0xfe;
    uStack_9 = 2;
    uStack_b = 5;
  }
  else {
    uStack_a = 0xfd;
    uStack_9 = 1;
    uStack_b = 9;
  }
  *(byte *)0x256 = *(byte *)0x256 & 0xf0;
  *(byte *)0x256 = *(byte *)0x256 | uStack_b;
  *(undefined1 *)*(undefined4 *)0x326 = *(undefined1 *)0x256;
  *(byte *)0x20 = *(byte *)0x20 & uStack_a;
  *(byte *)0x20 = *(byte *)0x20 | uStack_9;
  return;
}



undefined2 __stdcall16far FUN_000f_ca87(byte param_1,byte param_2,byte param_3)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  *(byte *)0x256 = *(byte *)0x256 & 0xf0;
  *(byte *)0x256 = *(byte *)0x256 | param_3;
  *(undefined1 *)*(undefined4 *)0x326 = *(undefined1 *)0x256;
  if ((*(byte *)0x242 & 2) == 0) {
    *(byte *)0x20 = *(byte *)0x20 & param_2;
    *(byte *)0x20 = *(byte *)0x20 | param_1;
    uVar1 = 1;
  }
  else if (((*(uint *)*(undefined4 *)0x322 & 0x8000) == 0) || ((*(byte *)0x226 & 0x20) != 0)) {
    *(byte *)0x20 = *(byte *)0x20 & param_2;
    *(byte *)0x20 = *(byte *)0x20 | param_1;
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}



void __cdecl16far FUN_000f_ccec(void)

{
  uint uVar1;
  char *pcVar2;
  char *pcVar3;
  undefined2 unaff_DS;
  undefined1 uVar4;
  bool bVar5;
  ulong uVar6;
  
  *(byte *)&DAT_000f_1a9e = *(byte *)&DAT_000f_1a9e & 0xfe;
  pcVar3 = (char *)s_cursor__a___000f_fd8c + 2;
  func_0x000fe7e0();
  if ((*(byte *)0x20 & 0x80) != 0) {
    pcVar3 = (char *)s_Acq_Addr_Buf_u3427_8_000f_fba5 + 4;
    func_0x000fc99c((char *)s_cursor__a___000f_fd8c + 2);
    if ((*(byte *)0x20 & 0x80) != 0) {
      pcVar3 = (char *)s_Acq_Addr_Buf_u3427_8_000f_fba5 + 4;
      func_0x000fca35((char *)s_Acq_Addr_Buf_u3427_8_000f_fba5 + 4);
    }
  }
  *(undefined1 *)*(undefined4 *)0x33a = *(undefined1 *)0x259;
  *(undefined1 *)*(undefined4 *)0x326 = *(undefined1 *)0x256;
  func_0x000e5d2f(pcVar3);
  *(byte *)*(undefined4 *)0x336 = *(byte *)0x258 & 0xed;
  *(undefined2 *)*(undefined4 *)0x32e = *(undefined2 *)0x25e;
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  func_0x000e5d28(0xe5d1);
  *(uint *)0x22 = *(uint *)0x22 | 0x400;
  uVar6 = (ulong)(*(int *)&DAT_000f_1b00 >> 4 & 0x7ff);
  pcVar3 = (char *)0xe5d1;
  for (uVar1 = *(uint *)&DAT_000f_1b00 & 0xf; 0 < (int)uVar1; uVar1 = uVar1 - 1) {
    uVar6 = func_0x000e777d(pcVar3,10,0,uVar1,uVar6);
    pcVar3 = (char *)0xe772;
  }
  *(uint *)0x548 = *(uint *)0x548 | 0x2000;
  while( true ) {
    uVar4 = 0;
    bVar5 = true;
    func_0x000e7753(pcVar3,0,0);
    if (((!bVar5) && (func_0x000e7753(0xe772,uVar6), (bool)uVar4 || bVar5)) ||
       (pcVar3 = (char *)0xe772, (*(uint *)0x22 & 0x80) != 0)) {
      func_0x000e004f(0xe772,2);
      return;
    }
    while ((*(uint *)0x22 & 0x400) == 0) {
      *(uint *)0x22 = *(uint *)0x22 | 1;
      func_0x000e6224(pcVar3);
      pcVar3 = (char *)0xe60b;
    }
    *(uint *)0x22 = *(uint *)0x22 & 0xfffe;
    uVar1 = *(int *)0x260 + 2U & 0xfff;
    *(uint *)0x264 = uVar1;
    *(uint *)0x92 = uVar1;
    *(uint *)0x8c = uVar1;
    pcVar2 = pcVar3;
    if ((*(byte *)0x20 & 0x80) != 0) {
      pcVar2 = (char *)s_Acq_Addr_Buf_u3427_8_000f_fba5 + 4;
      func_0x000fca35(pcVar3);
    }
    if ((*(byte *)0x20 & 1) != 0) {
      func_0x000f9042(pcVar2,0,3);
      pcVar2 = (char *)0xfbbe;
      func_0x000fbbe9((char *)s_Press_button_again_to_end__000f_f800 + 5,100);
    }
    if ((*(byte *)0x20 & 2) != 0) {
      func_0x000f9042(pcVar2,1,3);
      pcVar2 = (char *)0xfbbe;
      func_0x000fbbe9((char *)s_Press_button_again_to_end__000f_f800 + 5,0x6e);
    }
    *(uint *)&DAT_000f_046a = *(uint *)&DAT_000f_046a | 0x200;
    func_0x000fd40c(pcVar2);
    *(uint *)0x186 = *(uint *)0x186 | 0x400;
    func_0x000ff067((char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9);
    if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) break;
    pcVar3 = (char *)s_Clk_Delay_Reg_u4230_000f_fbbb + 4;
    func_0x000fcfd2((char *)s_not_implemented_000f_fe00 + 5);
    if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) {
      *(uint *)0x22 = *(uint *)0x22 | 0x80;
      func_0x000e5d15((char *)s_Clk_Delay_Reg_u4230_000f_fbbb + 4);
      return;
    }
  }
  *(uint *)0x22 = *(uint *)0x22 | 0x80;
  func_0x000e5d15((char *)s_not_implemented_000f_fe00 + 5);
  return;
}



void __cdecl16far FUN_000f_cedf(void)

{
  int iVar1;
  undefined2 unaff_DS;
  int iStack_a;
  
  if (*(int *)0x23c < *(int *)0x8c) {
    iStack_a = (*(int *)0x23c - *(int *)0x8c) + 0x1000;
  }
  else {
    iStack_a = *(int *)0x23c - *(int *)0x8c;
  }
  if ((*(byte *)0x20 & 0x10) != 0) {
    iStack_a = iStack_a >> 1;
  }
  if (*(char *)&DAT_000f_1a9b != '\0') {
    iStack_a = iStack_a + 3;
  }
  if ((*(byte *)0x20 & 1) != 0) {
    *(byte *)0x1dc = *(byte *)0x1dc | 1;
    *(byte *)0x1e4 = *(byte *)0x1e4 | 1;
    iVar1 = *(int *)0x64 + iStack_a;
    if (*(int *)0x2a < iVar1) {
      *(int *)0x26 = (iVar1 - *(int *)0x2a) + -1;
    }
    else if (iVar1 < 0) {
      *(int *)0x26 = iVar1 + *(int *)0x2a + 1;
    }
    else {
      *(int *)0x26 = iVar1;
    }
  }
  if ((*(byte *)0x20 & 2) != 0) {
    *(byte *)0x1dd = *(byte *)0x1dd | 1;
    *(byte *)0x1e5 = *(byte *)0x1e5 | 1;
    iStack_a = *(int *)0x6e + iStack_a;
    if (*(int *)0x2a < iStack_a) {
      *(int *)0x26 = (iStack_a - *(int *)0x2a) + -1;
    }
    else if (iStack_a < 0) {
      *(int *)0x26 = iStack_a + *(int *)0x2a + 1;
    }
    else {
      *(int *)0x26 = iStack_a;
    }
  }
  if (*(char *)0x241 == '\x05') {
    *(uint *)0x22 = *(uint *)0x22 | 8;
  }
  *(uint *)&DAT_000f_046a = *(uint *)&DAT_000f_046a | 0x200;
  return;
}



void __cdecl16far FUN_000f_cfd2(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  
  *(undefined2 *)0x222 = 1;
  *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 | 0x20;
  uVar1 = 2;
  func_0x000e004f();
  func_0x000e6224(0xe004,uVar1);
  *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 & 0xdf;
  func_0x000fd94f(0xe60b);
  *(undefined2 *)0x222 = 0;
  return;
}



void __cdecl16far FUN_000f_d28c(void)

{
  uint uVar1;
  char *pcVar2;
  undefined2 unaff_DS;
  
  *(byte *)&DAT_000f_1a9e = *(byte *)&DAT_000f_1a9e & 0xfe;
  func_0x000fe7e0();
  uVar1 = *(int *)0x260 + 2U & 0xfff;
  *(uint *)0x264 = uVar1;
  *(uint *)0x92 = uVar1;
  *(uint *)0x8c = uVar1;
  func_0x000fe023((char *)s_cursor__a___000f_fd8c + 2);
  *(uint *)0x186 = *(uint *)0x186 | 0x400;
  if ((*(char *)0x241 == '\x04') && (*(char *)0x240 == '\x03')) {
    pcVar2 = (char *)0xfd28;
    func_0x000fd2e8((char *)s_PRESS_MENU_KEYS_TO_CONTINUE_000f_fd00 + 8);
  }
  else {
    func_0x000fd40c((char *)s_PRESS_MENU_KEYS_TO_CONTINUE_000f_fd00 + 8);
    pcVar2 = (char *)s_not_implemented_000f_fe00 + 5;
    func_0x000ff067((char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9);
  }
  *(uint *)0x186 = *(uint *)0x186 & 0xfbff;
  func_0x000fdb3e(pcVar2);
  return;
}



/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_000f_d2e8(void)

{
  uint uVar1;
  char *unaff_CS;
  undefined2 uVar2;
  undefined2 uVar3;
  char *pcVar4;
  undefined2 unaff_DS;
  
  if ((*(byte *)0x20 & 0x80) != 0) {
    unaff_CS = (char *)s_Acq_Addr_Buf_u3427_8_000f_fba5 + 4;
    func_0x000fc99c();
    if ((*(byte *)0x20 & 0x80) != 0) {
      unaff_CS = (char *)s_Acq_Addr_Buf_u3427_8_000f_fba5 + 4;
      func_0x000fca35((char *)s_Acq_Addr_Buf_u3427_8_000f_fba5 + 4);
    }
  }
  *(uint *)0x22 = *(uint *)0x22 | 0x400;
  while ((*(uint *)0x22 & 0x80) == 0) {
    while ((*(uint *)0x22 & 0x400) == 0) {
      *(uint *)0x22 = *(uint *)0x22 | 1;
      *(undefined1 *)0x30d = 0x2a;
      func_0x000e6224(unaff_CS);
      unaff_CS = (char *)0xe60b;
    }
    *(uint *)0x22 = *(uint *)0x22 & 0xfffe;
    uVar1 = *(int *)0x260 + 2U & 0xfff;
    *(uint *)0x264 = uVar1;
    *(uint *)0x92 = uVar1;
    *(uint *)0x8c = uVar1;
    *(uint *)0x8e = uVar1;
    pcVar4 = unaff_CS;
    if ((*(byte *)0x20 & 0x80) != 0) {
      pcVar4 = (char *)s_Acq_Addr_Buf_u3427_8_000f_fba5 + 4;
      func_0x000fca35(unaff_CS);
    }
    uVar2 = 0xf670;
    func_0x000f6dc9(pcVar4,*(undefined1 *)0x20);
    *(byte *)0x226 = *(byte *)0x226 | 0x40;
    if ((*(byte *)0x20 & 1) != 0) {
      uVar2 = 0xfbbe;
      func_0x000fbbe9(0xf670,100);
    }
    uVar3 = uVar2;
    if ((*(byte *)0x20 & 2) != 0) {
      uVar3 = 0xfbbe;
      func_0x000fbbe9(uVar2,0x6e);
    }
    func_0x000fd40c(uVar3);
    *(uint *)0x22 = *(uint *)0x22 | 1;
    uVar1 = _DAT_4000_377e & 0xffe;
    pcVar4 = (char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9;
    while ((_DAT_4000_377e - uVar1 & 0xf80) == 0) {
      *(undefined1 *)0x30d = 0x2c;
      func_0x000e6224(pcVar4);
      pcVar4 = (char *)0xe60b;
    }
    *(uint *)0x22 = *(uint *)0x22 & 0xfffe;
    unaff_CS = (char *)s_not_implemented_000f_fe00 + 5;
    func_0x000ff067(pcVar4);
  }
  return;
}



void __cdecl16far FUN_000f_d40c(void)

{
  undefined2 unaff_DS;
  
  func_0x000e5d2f();
  *(byte *)*(undefined4 *)0x336 = *(byte *)0x258 & 0xed;
  *(undefined2 *)*(undefined4 *)0x32e = *(undefined2 *)0x25e;
  *(byte *)0x258 = *(byte *)0x257 | 0x10;
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  *(byte *)0x258 = *(byte *)0x258 | *(byte *)0x258 & 2;
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  *(undefined1 *)0x1a99 = 0x41;
  func_0x000e5d28(0xe5d1);
  return;
}



void __cdecl16far FUN_000f_d471(void)

{
  uint uVar1;
  int iVar2;
  undefined2 unaff_CS;
  char *pcVar3;
  char *pcVar4;
  char *pcVar5;
  undefined2 uVar6;
  undefined2 unaff_DS;
  undefined1 uVar7;
  bool bVar8;
  ulong uVar9;
  undefined2 uStack_e;
  int iStack_c;
  
  iStack_c = 0;
  *(byte *)&DAT_000f_1a9e = *(byte *)&DAT_000f_1a9e & 0xfe;
  uStack_e = 1;
  if (*(char *)0x241 == '\0') {
    unaff_CS = 0xfea2;
    func_0x000feaef();
    if ((*(byte *)0x240 & 8) != 0) {
      uStack_e = 0;
    }
  }
  pcVar3 = (char *)s_cursor__a___000f_fd8c + 2;
  func_0x000fe7e0(unaff_CS);
  if ((*(byte *)0x20 & 0x80) != 0) {
    pcVar3 = (char *)s_Acq_Addr_Buf_u3427_8_000f_fba5 + 4;
    func_0x000fc99c((char *)s_cursor__a___000f_fd8c + 2);
    if ((*(byte *)0x20 & 0x80) != 0) {
      pcVar3 = (char *)s_Acq_Addr_Buf_u3427_8_000f_fba5 + 4;
      func_0x000fca35((char *)s_Acq_Addr_Buf_u3427_8_000f_fba5 + 4);
    }
  }
  func_0x000fe023(pcVar3);
  *(undefined2 *)&DAT_000f_1b02 = 0;
  *(undefined2 *)&DAT_000f_1b04 = 0;
  uVar9 = (ulong)(*(int *)&DAT_000f_1b00 >> 4 & 0x7ff);
  pcVar3 = (char *)s_PRESS_MENU_KEYS_TO_CONTINUE_000f_fd00 + 8;
  for (uVar1 = *(uint *)&DAT_000f_1b00 & 0xf; 0 < (int)uVar1; uVar1 = uVar1 - 1) {
    uVar9 = func_0x000e777d(pcVar3,10,0,uVar1,uVar9);
    pcVar3 = (char *)0xe772;
  }
  *(uint *)0x548 = *(uint *)0x548 | 0x2000;
  while( true ) {
    uVar7 = 0;
    bVar8 = true;
    func_0x000e7753(pcVar3,0,0);
    if ((!bVar8) && (func_0x000e7753(0xe772,uVar9), (bool)uVar7 || bVar8)) {
      uVar6 = 0xe772;
      func_0x000e7753(0xe772,*(undefined2 *)&DAT_000f_1b02,*(undefined2 *)&DAT_000f_1b04);
      if (!(bool)uVar7) {
        uVar6 = 0xe921;
        func_0x000e921b(0xe772);
      }
      func_0x000e004f(uVar6,2);
      return;
    }
    pcVar3 = (char *)0xe772;
    func_0x000e7753(0xe772,*(undefined2 *)&DAT_000f_1b02,*(undefined2 *)&DAT_000f_1b04);
    if (!(bool)uVar7) {
      pcVar3 = (char *)0xe921;
      func_0x000e921b(0xe772);
    }
    pcVar4 = pcVar3;
    if ((*(byte *)0x20 & 0x80) != 0) {
      pcVar4 = (char *)s_Acq_Addr_Buf_u3427_8_000f_fba5 + 4;
      func_0x000fca35(pcVar3);
    }
    if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) break;
    func_0x000fd40c(pcVar4);
    *(undefined1 *)0x30c = 0x10;
    *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 | 0x20;
    func_0x000e6224((char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9);
    *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 & 0xdf;
    if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) {
      *(uint *)0x22 = *(uint *)0x22 | 0x80;
      func_0x000e6a8e(0xe60b,2);
      return;
    }
    pcVar3 = (char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9;
    iVar2 = func_0x000fd7f8(0xe60b,*(undefined1 *)0x20);
    if (iVar2 != -1) {
      pcVar4 = (char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9;
      func_0x000fd94f((char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9);
      if (*(char *)0x1e == '\a') {
        pcVar4 = (char *)0xff3c;
        func_0x000ff3c8((char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9);
      }
      if ((*(byte *)0x20 & 1) != 0) {
        pcVar3 = pcVar4;
        if (*(char *)0x1e == '\x06') {
          pcVar3 = (char *)0xff3c;
          func_0x000ff40a(pcVar4,*(undefined1 *)0x22f);
        }
        pcVar5 = pcVar3;
        if ((*(byte *)&DAT_000f_1b88 & 1) != 0) {
          *(byte *)&DAT_000f_1b88 = *(byte *)&DAT_000f_1b88 & 0xfe;
          pcVar5 = (char *)0xf6e5;
          func_0x000f6e5c(pcVar3,0,0);
          *(uint *)&DAT_000f_046a = *(uint *)&DAT_000f_046a | 4;
        }
        pcVar4 = (char *)s_uncaled___delta___000f_fd5c + 8;
        func_0x000fe54e(pcVar5,*(undefined1 *)0x22f,0,*(undefined1 *)0x22b,(int)*(undefined4 *)0x31e
                        ,(int)((ulong)*(undefined4 *)0x31e >> 0x10),0);
        *(byte *)0x233 = *(byte *)0x233 & 0xfe;
      }
      if ((*(byte *)0x20 & 2) != 0) {
        pcVar3 = pcVar4;
        if (*(char *)0x1e == '\x06') {
          pcVar3 = (char *)0xff3c;
          func_0x000ff40a(pcVar4,*(undefined1 *)0x230);
        }
        pcVar5 = pcVar3;
        if ((*(byte *)&DAT_000f_1b88 & 2) != 0) {
          *(byte *)&DAT_000f_1b88 = *(byte *)&DAT_000f_1b88 & 0xfd;
          pcVar5 = (char *)0xf6e5;
          func_0x000f6e5c(pcVar3,1,0);
          *(uint *)&DAT_000f_046a = *(uint *)&DAT_000f_046a | 8;
        }
        pcVar4 = (char *)s_uncaled___delta___000f_fd5c + 8;
        func_0x000fe54e(pcVar5,*(undefined1 *)0x230,uStack_e,*(undefined1 *)0x22c,
                        (int)*(undefined4 *)0x31e,(int)((ulong)*(undefined4 *)0x31e >> 0x10),1);
        *(byte *)0x233 = *(byte *)0x233 & 0xfd;
      }
      *(uint *)0x22 = *(uint *)0x22 | 0x100;
      pcVar3 = (char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9;
      func_0x000fda40(pcVar4);
      iStack_c = iStack_c + 1;
      if ((*(byte *)0x242 & 2) == 0) {
        if (*(int *)0x254 >> 3 <= iStack_c) {
          iStack_c = 0;
          func_0x000e6a8e((char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9,2);
          if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) {
            *(uint *)0x22 = *(uint *)0x22 | 0x80;
            func_0x000e6a8e(0xe6a8,2);
            return;
          }
          *(undefined2 *)0x222 = 2;
          pcVar3 = (char *)0xe60b;
          func_0x000e6224(0xe6a8);
          *(undefined2 *)0x222 = 0;
          if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) {
            *(uint *)0x22 = *(uint *)0x22 | 0x80;
            func_0x000e6a8e(0xe60b,2);
            return;
          }
        }
      }
      else {
        func_0x000e6a8e((char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9,2);
        if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) {
          *(uint *)0x22 = *(uint *)0x22 | 0x80;
          func_0x000e6a8e(0xe6a8,2);
          return;
        }
        *(undefined2 *)0x222 = 2;
        func_0x000e6224(0xe6a8);
        if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) {
          *(uint *)0x22 = *(uint *)0x22 | 0x80;
          func_0x000e6a8e(0xe60b,2);
          return;
        }
        pcVar3 = (char *)s_Clk_Delay_Reg_u4230_000f_fbbb + 4;
        func_0x000fcfd2(0xe60b);
        if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) {
          *(uint *)0x22 = *(uint *)0x22 | 0x80;
          func_0x000e6a8e((char *)s_Clk_Delay_Reg_u4230_000f_fbbb + 4,2);
          return;
        }
      }
      if ((*(uint *)&DAT_000f_1b04 | 0xf) == 0xf) {
        if (*(char *)0x241 == '\0') {
          func_0x000e693c(pcVar3);
          pcVar3 = (char *)0xe693;
          func_0x000e693c(0xe693);
        }
        else {
          while (((*(byte *)0x1a98 & 0xf) != 0 ||
                 ((*(int *)0x548 != 0 || *(int *)0x676 != 0 && (*(char *)&DAT_000f_1b06 != '\0')))))
          {
            func_0x000e693c(pcVar3);
            pcVar3 = (char *)0xe693;
          }
        }
      }
    }
  }
  *(uint *)0x22 = *(uint *)0x22 | 0x80;
  func_0x000e6a8e(pcVar4,2);
  return;
}



undefined2 __stdcall16far FUN_000f_d7f8(byte param_1)

{
  int iVar1;
  char *pcVar2;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x25a = *(undefined1 *)*(undefined4 *)0x32a;
  *(undefined2 *)0x25c = *(undefined2 *)*(undefined4 *)0x332;
  *(byte *)0x258 = *(byte *)0x258 & 0xef;
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  pcVar2 = (char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9;
  func_0x000fd863();
  if (*(char *)0x241 == '\0') {
    pcVar2 = (char *)s_read_back_000f_fd2e + 5;
    iVar1 = func_0x000fe234((char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9);
    if (iVar1 == -1) {
      return 0xffff;
    }
  }
  if ((param_1 & 8) != 0) {
    func_0x000fd8ef(pcVar2,*(undefined2 *)0x8c,param_1);
  }
  return 0;
}



void __cdecl16far FUN_000f_d863(void)

{
  undefined2 unaff_DS;
  undefined2 uStack_c;
  undefined2 uStack_a;
  
  uStack_a = func_0x000fe806();
  if (*(char *)0x241 == '\0') {
    uStack_a = uStack_a - *(byte *)0x245;
  }
  *(uint *)0x8e = uStack_a & 0xfff;
  if ((((*(uint *)0x262 & 1) == 0) && (*(char *)0x1e != '\x01')) && (*(char *)0x1e != '\a')) {
    uStack_c = *(int *)0x248 + -1;
  }
  else {
    uStack_c = *(int *)0x248;
  }
  *(uint *)0x8c = ((*(int *)0x8e - *(int *)0x24a) - uStack_c) + 1U & 0xfff;
  *(uint *)0x23c = (*(int *)0x8e - uStack_c) + 1U & 0xfff;
  return;
}



void __stdcall16far FUN_000f_d8ef(uint param_1,byte param_2)

{
  undefined2 unaff_DS;
  
  if ((param_2 & 4) == 0) {
    if ((param_1 & 1) == 0) {
      *(undefined1 *)0x22b = 0;
      *(undefined1 *)0x22c = 1;
    }
    else {
      *(undefined1 *)0x22b = 1;
      *(undefined1 *)0x22c = 0;
    }
  }
  else if ((param_1 & 2) == 0) {
    *(undefined1 *)0x22b = 0;
    *(undefined1 *)0x22c = 2;
  }
  else {
    *(undefined1 *)0x22b = 2;
    *(undefined1 *)0x22c = 0;
  }
  return;
}



void __cdecl16far FUN_000f_d94f(void)

{
  undefined2 unaff_CS;
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  if ((*(byte *)0x20 & 1) != 0) {
    uVar1 = unaff_CS;
    if (((*(byte *)0x600 & 1) != 0) && (*(char *)0x1e == '\x06')) {
      uVar1 = 0xff3c;
      func_0x000ff469();
    }
    if ((*(byte *)0x600 & 1) != 0) {
      *(byte *)0x600 = *(byte *)0x600 & 0xfe;
      *(byte *)&DAT_000f_1b88 = *(byte *)&DAT_000f_1b88 | 1;
      *(undefined2 *)&DAT_000f_1b24 = *(undefined2 *)&DAT_000f_1a94;
      *(undefined2 *)&DAT_000f_1b26 = *(undefined2 *)&DAT_000f_1a94;
      *(byte *)0x35a = *(byte *)0x35a | 1;
    }
    unaff_CS = uVar1;
    if ((*(char *)0x240 == '\0') || ((*(byte *)0x240 & 8) != 0)) {
      if ((*(byte *)0x600 & 2) != 0) {
        *(byte *)0x600 = *(byte *)0x600 & 0xfd;
        *(byte *)&DAT_000f_1b88 = *(byte *)&DAT_000f_1b88 | 2;
        *(undefined2 *)&DAT_000f_1b2e = *(undefined2 *)&DAT_000f_1a96;
        *(undefined2 *)&DAT_000f_1b30 = *(undefined2 *)&DAT_000f_1a96;
        *(byte *)0x36c = *(byte *)0x36c | 1;
      }
      if ((*(byte *)&DAT_000f_1b88 & 2) != 0) {
        *(byte *)&DAT_000f_1b88 = *(byte *)&DAT_000f_1b88 & 0xfd;
        unaff_CS = 0xf6e5;
        func_0x000f6e5c(uVar1,1,0);
        *(uint *)&DAT_000f_046a = *(uint *)&DAT_000f_046a | 8;
      }
    }
  }
  if ((*(byte *)0x20 & 2) != 0) {
    if (((*(byte *)0x600 & 2) != 0) && (*(char *)0x1e == '\x06')) {
      func_0x000ff469(unaff_CS,1);
    }
    if ((*(byte *)0x600 & 2) != 0) {
      *(byte *)0x600 = *(byte *)0x600 & 0xfd;
      *(byte *)&DAT_000f_1b88 = *(byte *)&DAT_000f_1b88 | 2;
      *(undefined2 *)&DAT_000f_1b2e = *(undefined2 *)&DAT_000f_1a96;
      *(undefined2 *)&DAT_000f_1b30 = *(undefined2 *)&DAT_000f_1a96;
      *(byte *)0x36c = *(byte *)0x36c | 1;
    }
  }
  return;
}



void __cdecl16far FUN_000f_da40(void)

{
  uint *puVar1;
  uint uVar2;
  char *unaff_CS;
  undefined2 unaff_DS;
  
  if ((*(byte *)0x243 & 3) == 3) {
    unaff_CS = (char *)s_cursor__a___000f_fd8c + 2;
    func_0x000fe83c();
  }
  if ((*(byte *)0x20 & 0x40) != 0) {
    func_0x000f69a5(unaff_CS,*(undefined1 *)0x20);
  }
  if (*(char *)0x240 == '\x03') {
    if ((*(byte *)0x318 & 1) != 0) {
      puVar1 = (uint *)&DAT_000f_1b02;
      uVar2 = *puVar1;
      *puVar1 = *puVar1 + 1;
      *(int *)&DAT_000f_1b04 = *(int *)&DAT_000f_1b04 + (uint)(0xfffe < uVar2);
    }
    *(char *)0x318 = *(char *)0x318 + '\x01';
  }
  else {
    puVar1 = (uint *)&DAT_000f_1b02;
    uVar2 = *puVar1;
    *puVar1 = *puVar1 + 1;
    *(int *)&DAT_000f_1b04 = *(int *)&DAT_000f_1b04 + (uint)(0xfffe < uVar2);
  }
  *(uint *)0x548 = *(uint *)0x548 | 0x2000;
  return;
}



/* clear_measurement_mode_bit (confidence: Confirmed mechanism)
   
   Evidence: Clears bit `0x10` of `[0x258]` and commits it to the shared hardware register cluster
   (`[0x336]`) - part of the `configure_measurement_hw` family */

void __cdecl16far clear_measurement_mode_bit(void)

{
  undefined2 unaff_DS;
  
  *(byte *)0x258 = *(byte *)0x258 & 0xef;
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  *(byte *)0x21 = *(byte *)0x21 & 0xfe;
  *(uint *)0x22 = *(uint *)0x22 & 0xfffe;
  return;
}



/* engage_selftest_mode_bit_and_sample (confidence: Mechanism confirmed)
   
   Evidence: Clears bit 4 and sets bit 1 of the shared comm self-test control byte `[0x258]`, writes
   it to hardware via far ptr `[0x336]` (the same register `sync_status_byte_to_hw` writes, one of
   `init_selftest_register_group`'s confirmed register family), then takes one readback sample from
   far ptr `[0x31E]+0x1000` */

void __cdecl16far engage_selftest_mode_bit_and_sample(void)

{
  undefined2 unaff_DS;
  
  *(byte *)0x258 = *(byte *)0x258 & 0xef | 2;
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  return;
}



/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* poll_comm_probe_until_match (confidence: Mechanism confirmed)
   
   Evidence: The fallback `run_continuous_selftest_tick` dispatches to when `[0x1B5E]` is clear:
   while `[0x258]` bit `0x10` is set, loops calling `ack_comm_hw_status_bits` and comparing
   `[0x262]` against a live read of the confirmed comm probe register (physical `0x40377E`) until
   they match, then updates `[0x266]`/`[0x1A]` and gates the result on `[0x1B76]` bit 2 */

uint __cdecl16far poll_comm_probe_until_match(void)

{
  uint uVar1;
  undefined2 unaff_DS;
  uint uStack_a;
  
  if ((*(byte *)0x258 & 0x10) == 0) {
    uStack_a = 0;
  }
  else {
    uStack_a = func_0x000fc624();
    if (uStack_a != 0) {
      while (*(int *)0x262 != _DAT_4000_377e) {
        uVar1 = func_0x000fc624(0xfc62,600);
        uStack_a = uStack_a | uVar1;
      }
    }
    if ((uStack_a & 4) != 0) {
      *(undefined2 *)0x266 = *(undefined2 *)0x262;
    }
    *(uint *)0x1a = uStack_a;
    if ((*(byte *)&DAT_000f_1a86 & 4) != 0) {
      uStack_a = 0;
    }
  }
  return uStack_a;
}



void __cdecl16far FUN_000f_dc27(void)

{
  func_0x000fdf88();
  return;
}



void __cdecl16far FUN_000f_dc3e(void)

{
  undefined2 unaff_DS;
  
  if ((*(uint *)*(undefined4 *)0x322 & 0x8000) == 0) {
    func_0x000e6a8e();
  }
  else {
    *(byte *)0x258 = *(byte *)0x258 | 0x40;
    *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  }
  return;
}



/* sync_status_byte_to_hw (confidence: Confirmed mechanism)
   
   Evidence: Writes `[0x258]` into the far-pointer hardware register `[0x336]` - called from
   `switch_to_next_task`, so likely a general "flush pending status" step rather than
   measurement-specific */

void __cdecl16far sync_status_byte_to_hw(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  return;
}



void __cdecl16far FUN_000f_dd38(void)

{
  char cVar1;
  undefined2 unaff_DS;
  
  cVar1 = *(char *)0x240;
  if (cVar1 == '\0') {
    *(undefined1 *)0x256 = 0xb;
  }
  else if (cVar1 == '\x01') {
    *(undefined1 *)0x256 = 9;
  }
  else if (cVar1 == '\a') {
    *(undefined1 *)0x256 = 0xd;
  }
  else if (cVar1 == '\b') {
    if (*(char *)0x244 == '\a') {
      *(undefined1 *)0x256 = 0xd;
    }
    else {
      *(undefined1 *)0x256 = 0xc;
    }
  }
  else {
    *(undefined1 *)0x256 = 5;
  }
  *(byte *)0x256 = *(byte *)0x256 | 0x10;
  if (((*(char *)0x1e == '\x02') || (*(char *)0x1e == '\n')) || (*(char *)0x1e == '\x05')) {
    *(byte *)0x256 = *(byte *)0x256 | 0x20;
  }
  if (*(char *)&DAT_000f_1a9b == '\x01') {
    *(byte *)0x256 = *(byte *)0x256 | 0x40;
  }
  return;
}



void __stdcall16far FUN_000f_ddc3(char param_1,char param_2)

{
  char cVar1;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x257 = 0;
  if (param_1 == '\x01') {
    *(byte *)0x257 = *(byte *)0x257 | 4;
  }
  if (param_2 == '\x01') {
    *(byte *)0x257 = *(byte *)0x257 | 8;
  }
  if (((*(byte *)0x243 & 2) != 0) && ((*(byte *)0x243 & 1) == 0)) {
    *(byte *)0x257 = *(byte *)0x257 | 1;
  }
  cVar1 = *(char *)0x241;
  if ((cVar1 == '\x03') || (cVar1 == '\x04')) {
    *(byte *)0x257 = *(byte *)0x257 | 0x40;
    goto LAB_000f_de3f;
  }
  if (cVar1 != '\x05') {
    if (cVar1 == '\x06') {
      *(byte *)0x257 = *(byte *)0x257 | 0x22;
      goto LAB_000f_de3f;
    }
    if (cVar1 != '\a') {
      *(byte *)0x257 = *(byte *)0x257 | 0x82;
      goto LAB_000f_de3f;
    }
  }
  *(byte *)0x257 = *(byte *)0x257 | 0x22;
LAB_000f_de3f:
  if ((*(byte *)0x242 & 2) != 0) {
    *(byte *)0x257 = *(byte *)0x257 | 0x20;
  }
  *(undefined1 *)0x258 = *(undefined1 *)0x257;
  return;
}



/* WARNING: Removing unreachable block (ram,0x0000e06c) */

void __stdcall16far FUN_000f_de59(int param_1)

{
  undefined2 unaff_DS;
  uint uStack_c;
  uint uStack_a;
  
  uStack_a = *(int *)&DAT_000f_1afc >> 3 & 0x3fe;
  if (*(char *)&DAT_000f_1a92 == '\0') {
    uStack_c = 0x400 - uStack_a;
  }
  else {
    uStack_c = uStack_a;
    uStack_a = 0x400 - uStack_a;
  }
  if (*(char *)&DAT_000f_1a80 == '\x04') {
    uStack_a = uStack_a << 2;
    uStack_c = uStack_c << 2;
  }
  else {
    uStack_a = uStack_a & 0xfffc;
    uStack_c = uStack_c & 0xfffc;
  }
  *(uint *)0x24 = uStack_a;
  if (*(char *)0x1f == '\0') {
    uStack_a = (int)(uStack_a << (*(byte *)0x231 & 0x1f)) / param_1 + 2U & 0xfffe;
    uStack_c = (int)(uStack_c << (*(byte *)0x231 & 0x1f)) / param_1 + 2U & 0xfffe;
  }
  *(uint *)0x24a = uStack_a;
  *(uint *)0x248 = uStack_c;
  *(int *)0x260 = 0xffe - uStack_a;
  *(int *)0x25e = 0xff0 - uStack_c;
  if (*(char *)0x1f != '\0') {
    *(int *)0x25e = *(int *)0x25e + (uint)*(byte *)0x245;
  }
  if ((5 < *(uint *)0x25e) &&
     (((*(char *)0x1f == '\x05' || (*(char *)0x1f == '\x06')) || (*(char *)0x1f == '\a')))) {
    if ((*(byte *)0x20 & 0x10) == 0) {
      *(int *)0x25e = *(int *)0x25e + -2;
    }
    else {
      *(int *)0x25e = (int)((char *)s_ACQ_RAM_odd_000f_ff05 + 5 + *(int *)0x25e);
    }
  }
  return;
}



void __cdecl16far FUN_000f_e023(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)*(undefined4 *)0x33a = *(undefined1 *)0x259;
  *(undefined1 *)*(undefined4 *)0x326 = *(undefined1 *)0x256;
  func_0x000e5d2f();
  *(byte *)*(undefined4 *)0x336 = *(byte *)0x258 & 0xed;
  *(undefined2 *)*(undefined4 *)0x32e = *(undefined2 *)0x25e;
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  func_0x000e5d28(0xe5d1);
  return;
}



undefined2 __cdecl16far FUN_000f_e07c(void)

{
  undefined2 uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  int iStack_c;
  
  if ((*(byte *)0x20 & 0x40) == 0) {
    uVar1 = 0;
  }
  else if ((*(byte *)&DAT_000f_1a86 & 0x97) == 0) {
    if ((*(uint *)0x186 & 0x400) == 0) {
      uVar1 = 0;
    }
    else {
      uVar2 = *(uint *)*(undefined4 *)0x322 & 0xffe;
      if ((int)uVar2 < *(int *)0x8c) {
        iStack_c = (uVar2 - *(int *)0x8c) + 0x1000;
      }
      else {
        iStack_c = uVar2 - *(int *)0x8c;
      }
      if (iStack_c < 0x401) {
        uVar1 = 0;
      }
      else if ((*(uint *)*(undefined4 *)0x322 & 0x3000) == 0) {
        *(uint *)0x186 = *(uint *)0x186 & 0xfbff;
        uVar1 = 1;
      }
      else {
        uVar1 = 0;
      }
    }
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}



void __cdecl16far FUN_000f_e1e1(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)&DAT_000f_1a9b = 0;
  if ((*(char *)0x3f0 != '\0') &&
     ((*(char *)&DAT_000f_1a9a == '\x01' || (*(char *)&DAT_000f_1a9a == '\x02')))) {
    if (*(char *)0x240 == '\a') {
      *(undefined *)&DAT_000f_1a9b = 3;
    }
    else {
      *(undefined *)&DAT_000f_1a9b = 2;
    }
  }
  if ((*(char *)0x1f == '\x06') && (*(char *)&DAT_000f_1a9a == '\x02')) {
    *(undefined1 *)&DAT_000f_1a9b = 0;
  }
  return;
}



undefined2 __cdecl16far FUN_000f_e234(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  uint uStack_18;
  int iStack_16;
  undefined1 local_14 [2];
  undefined2 local_12;
  uint uStack_10;
  uint local_e;
  int iStack_c;
  uint local_a;
  
  iVar1 = func_0x000fe3ca();
  if (iVar1 == -1) {
    uVar2 = 0xffff;
  }
  else {
    if (*(int *)0x23c < *(int *)0x8c) {
      iStack_c = (*(int *)0x23c - *(int *)0x8c) + 0x1000;
    }
    else {
      iStack_c = *(int *)0x23c - *(int *)0x8c;
    }
    uStack_10 = *(int *)0x23c - 2U & 0xfff;
    func_0x000fe475((char *)s_read_back_000f_fd2e + 5,*(undefined2 *)0x24,uStack_10,iStack_c,
                    local_14);
    func_0x000fe475((char *)s_read_back_000f_fd2e + 5,0,*(undefined2 *)0x8c,iStack_c,&local_e);
    *(undefined2 *)0x23c = local_12;
    *(int *)0x8c = local_a;
    if (*(char *)0x244 == '\x05') {
      iStack_16 = 1;
      if (((*(byte *)0x20 & 0x10) == 0) && ((*(char *)0x1e == '\0' || (*(char *)0x1e == '\x06')))) {
        if (*(int *)0x28 == 0x1000) {
          uStack_18 = 1;
        }
        else {
          uStack_18 = 4;
        }
        if (local_e != uStack_18) {
          iStack_16 = 0;
        }
      }
    }
    else {
      iStack_16 = 0;
    }
    if ((int)local_e < *(int *)0x236) {
      local_a = (local_a + *(int *)0x246) - iStack_16;
    }
    else {
      local_a = ((local_a + *(int *)0x246) - iStack_16) - 1;
    }
    *(uint *)0x8e = local_a & 0xfff;
    if ((*(byte *)0x20 & 0x10) != 0) {
      local_e = (int)local_e >> 1;
    }
    if (*(char *)0x1e == '\x06') {
      local_e = local_e << 1;
    }
    if ((*(byte *)0x240 & 8) != 0) {
      if ((*(byte *)0x20 & 1) == 0) {
        local_e = local_e & 0xfffe;
      }
      else {
        local_e = local_e | 1;
      }
    }
    if ((*(byte *)0x20 & 1) == 0) {
      if ((*(byte *)0x240 & 8) == 0) {
        *(uint *)0x8a = local_e;
        *(uint *)0x76 = local_e;
      }
      else {
        *(uint *)0x80 = local_e;
        *(uint *)0x6c = local_e;
      }
    }
    else {
      *(uint *)0x80 = local_e;
      *(uint *)0x6c = local_e;
    }
    uVar2 = 0;
  }
  return uVar2;
}



undefined2 __cdecl16far FUN_000f_e3ca(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  uint uStack_a;
  
  uStack_a = (uint)*(byte *)0x25a;
  if ((*(uint *)0x262 & 0x4000) != 0) {
    uStack_a = uStack_a | 0x100;
  }
  *(uint *)0x18 = uStack_a;
  if ((*(int *)0x24c + 0x50 < (int)uStack_a) ||
     ((int)uStack_a < (int)((undefined *)&DAT_000f_fed4 + *(int *)0x24e))) {
    uVar1 = 0xffff;
  }
  else {
    *(int *)0x238 = (int)((*(int *)0x24c - uStack_a) * 100) / *(int *)0x250 + 200;
    *(int *)0x238 = *(int *)0x238 / (int)(100 / (long)*(int *)0x252) + *(int *)0x252 * -2;
    if (*(char *)0x244 == '\x05') {
      if ((*(uint *)0x238 & 1) == 0) {
        *(undefined1 *)0x232 = 0;
      }
      else {
        *(undefined1 *)0x232 = 1;
        *(int *)0x238 = *(int *)0x238 + 5;
      }
    }
    uVar1 = 0;
  }
  return uVar1;
}



void __stdcall16far FUN_000f_e475(int param_1,int param_2,int param_3,int *param_4,uint *param_5)

{
  undefined2 unaff_DS;
  undefined2 uStack_10;
  undefined2 uStack_e;
  undefined2 uStack_c;
  undefined2 uStack_a;
  
  uStack_a = -1;
  if (*(char *)0x244 == '\x05') {
    uStack_e = 2;
  }
  else {
    uStack_e = 1;
  }
  uStack_c = param_2 - uStack_e;
  while (uStack_a < param_1 * uStack_e) {
    uStack_c = uStack_c + uStack_e & 0xfff;
    if ((int)uStack_c < *(int *)0x8c) {
      uStack_10 = (uStack_c - *(int *)0x8c) + 0x1000;
    }
    else {
      uStack_10 = uStack_c - *(int *)0x8c;
    }
    uStack_a = *(int *)0x24 * uStack_e - (*(int *)0x252 * (param_3 - uStack_10) + *(int *)0x238);
  }
  *param_4 = uStack_a / uStack_e;
  if ((*(char *)0x244 == '\x05') && (*(char *)0x232 == '\x01')) {
    *param_5 = uStack_c;
  }
  else if (*(char *)0x244 == '\x05') {
    *param_5 = uStack_c + 1;
  }
  else {
    *param_5 = uStack_c;
  }
  return;
}



void __cdecl16far FUN_000f_e74a(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)*(undefined4 *)0x326 = 0x19;
  *(undefined1 *)*(undefined4 *)0x336 = 0;
  *(undefined2 *)*(undefined4 *)0x32e = 0;
  *(undefined1 *)*(undefined4 *)0x32a = 0;
  *(undefined1 *)0x258 = 0xe;
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  *(undefined1 *)0x258 = 0x1e;
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  *(uint *)0x262 = *(uint *)0x262 | 0x8000;
  while ((*(uint *)0x262 & 0x8000) != 0) {
    *(undefined2 *)0x262 = *(undefined2 *)*(undefined4 *)0x322;
  }
  *(byte *)0x258 = *(byte *)0x258 & 0xef;
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  return;
}



void __cdecl16far FUN_000f_e7e0(void)

{
  uint uVar1;
  undefined2 unaff_DS;
  
  *(uint *)0x186 = *(uint *)0x186 | 8;
  uVar1 = (uint)*(byte *)0x241;
  func_0x000f9812();
  func_0x000f6708(0xf981,uVar1);
  *(uint *)0x186 = *(uint *)0x186 & 0xfff7;
  return;
}



uint __cdecl16far FUN_000f_e806(void)

{
  undefined2 unaff_DS;
  
  if ((*(uint *)0x262 & 1) != 0) {
    *(byte *)0x20 = *(byte *)0x20 | 0x20;
  }
  if ((*(byte *)0x226 & 1) != 0) {
    *(uint *)0x262 = *(uint *)0x262 | 1;
  }
  return (*(uint *)0x262 & 0xffe) - 1 & 0xfff;
}



void __cdecl16far FUN_000f_e83c(void)

{
  int iVar1;
  char *unaff_CS;
  char *pcVar2;
  undefined2 unaff_DS;
  undefined2 uStack_c;
  int iStack_a;
  
  iVar1 = *(int *)0x194;
  if ((*(uint *)0x262 & 0x1000) == 0) {
    uStack_c = 0;
    iStack_a = 0;
  }
  else {
    uStack_c = 1;
    iStack_a = (int)((*(uint *)0x25c & 0xfff) - *(int *)0x25e << (*(byte *)0x16e & 0x1f)) >>
               (*(byte *)0x16f & 0x1f);
    if (*(char *)0x244 == '\x05') {
      iStack_a = iStack_a >> 1;
    }
    iStack_a = iStack_a *
               *(int *)((uint)*(byte *)0x244 * 10 + (int)*(undefined4 *)&DAT_000f_1bbc + 4) +
               *(int *)0x190;
    if ((iStack_a < 0) || (iVar1 < iStack_a)) {
      uStack_c = 0;
      iStack_a = 0;
      unaff_CS = (char *)0xfbcc;
      func_0x000fbcc3();
    }
  }
  if (*(char *)0x1f == '\x05') {
    pcVar2 = unaff_CS;
    if ((*(byte *)0x20 & 1) != 0) {
      pcVar2 = (char *)s_installed_in_this_instrument__000f_f8f8 + 0x1a;
      func_0x000fa1a0(unaff_CS,0,uStack_c,iStack_a,iVar1);
    }
    if ((*(byte *)0x20 & 2) != 0) {
      func_0x000fa1a0(pcVar2,1,uStack_c,iStack_a,iVar1);
    }
  }
  else {
    pcVar2 = unaff_CS;
    if ((*(byte *)0x20 & 1) != 0) {
      pcVar2 = (char *)s_cursor__a___000f_fd8c + 2;
      func_0x000fe963(unaff_CS,0,uStack_c,iStack_a,iVar1);
    }
    if ((*(byte *)0x20 & 2) != 0) {
      func_0x000fe963(pcVar2,1,uStack_c,iStack_a,iVar1);
    }
  }
  return;
}



void __stdcall16far FUN_000f_e963(int param_1,int param_2,undefined2 param_3)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined2 uStack_c;
  
  iVar1 = param_1 * 6;
  _uStack_c = (byte *)CONCAT22(unaff_DS,(byte *)(iVar1 + 0x17a));
  if (param_2 == 0) {
    if ((*_uStack_c & 1) == 0) {
      *_uStack_c = *_uStack_c & 0xfd;
    }
    else {
      *_uStack_c = *_uStack_c & 0xfe;
      *_uStack_c = *_uStack_c | 2;
      *(undefined2 *)(iVar1 + 0x17c) = *(undefined2 *)(iVar1 + 0x17e);
      *(undefined1 *)(iVar1 + 0x17b) = 1;
    }
  }
  else {
    *_uStack_c = *_uStack_c | 1;
    *_uStack_c = *_uStack_c | 2;
    *(undefined2 *)(iVar1 + 0x17c) = *(undefined2 *)(iVar1 + 0x17e);
    *(undefined2 *)(iVar1 + 0x17e) = param_3;
    *(undefined1 *)(iVar1 + 0x17b) = 3;
  }
  if ((*_uStack_c & 2) != 0) {
    func_0x000fa02a();
  }
  return;
}



uint __stdcall16far FUN_000f_ea2e(undefined1 param_1,undefined1 param_2)

{
  undefined2 unaff_DS;
  undefined2 uStack_e;
  undefined1 uStack_b;
  undefined1 uStack_a;
  
  *(undefined1 *)*(undefined4 *)0x326 = param_2;
  *(undefined1 *)0x258 = 0;
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  *(undefined2 *)*(undefined4 *)0x32e = 0xce0;
  *(undefined1 *)0x258 = param_1;
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  *(byte *)0x258 = *(byte *)0x258 | 0x10;
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  func_0x000e6224();
  uStack_a = '\0';
  uStack_b = -1;
  while ((uStack_a != uStack_b || ((*(uint *)*(undefined4 *)0x322 & 0x2000) == 0))) {
    uStack_a = *(char *)*(undefined4 *)0x32a;
    uStack_b = *(char *)*(undefined4 *)0x32a;
  }
  uStack_e = (uint)*(byte *)*(undefined4 *)0x32a;
  if ((*(uint *)*(undefined4 *)0x322 & 0x4000) != 0) {
    uStack_e = uStack_e | 0x100;
  }
  return uStack_e;
}



void __cdecl16far FUN_000f_eaef(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  uVar2 = 0x99;
  uVar1 = func_0x000fea2e();
  *(undefined2 *)0x24c = uVar1;
  uVar1 = func_0x000fea2e(0xfea2,0x8e,0x89,uVar2);
  *(undefined2 *)0x24e = uVar1;
  *(undefined1 *)0x258 = 0;
  *(undefined1 *)*(undefined4 *)0x336 = *(undefined1 *)0x258;
  *(int *)0x250 = *(int *)0x24c - *(int *)0x24e;
  *(int *)0x250 = *(int *)0x250 + -2;
  return;
}



void __stdcall16far FUN_000f_eb40(undefined4 param_1,int param_2,uint *param_3)

{
  uint uVar1;
  code *pcVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  uint *puVar6;
  uint uVar7;
  undefined2 uVar8;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  int iStack_16;
  code *pcStack_12;
  code *pcStack_10;
  undefined1 uStack_d;
  int iStack_c;
  undefined2 uStack_a;
  
  iStack_16 = 2;
  while (iStack_16 != 0) {
    iStack_16 = iStack_16 + -1;
    uVar7 = (uint)((ulong)param_1 >> 0x10);
    uVar4 = (uint)param_1;
    puVar6 = (uint *)param_3;
    uVar8 = (undefined2)((ulong)param_3 >> 0x10);
    if (((int)*(undefined4 *)0x31e + param_2 == uVar4 - 1) &&
       ((uint)((ulong)*(undefined4 *)0x31e >> 0x10) == uVar7)) {
      *param_3 = uVar4;
      puVar6[1] = uVar7;
    }
    else {
      if ((uint)(*(int *)0x31e + param_2) < uVar4) {
        uStack_a = (undefined2)((ulong)*(undefined4 *)0x31e >> 0x10);
        iStack_c = (int)*(undefined4 *)0x31e + 0xfff;
      }
      else {
        uStack_a = (undefined2)((ulong)*(undefined4 *)0x31e >> 0x10);
        iStack_c = (int)*(undefined4 *)0x31e + param_2;
        iStack_16 = 0;
      }
      *(uint *)0xe = uVar4;
      *(uint *)0x10 = uVar7;
      *(int *)0x12 = iStack_c;
      *(undefined2 *)0x14 = uStack_a;
      func_0x000fdc3e(unaff_CS);
      unaff_CS = 0xe60b;
      func_0x000e6224((char *)s_unable_to_reset_mi_000f_fcba + 9);
      uVar1 = *(uint *)*(undefined4 *)0x322;
      pcVar2 = (code *)*(undefined4 *)((int)*(undefined4 *)&DAT_000f_1bc4 + (uint)*(byte *)0x1e * 4)
      ;
      pcStack_10 = (code *)((ulong)pcVar2 >> 0x10);
      pcStack_12 = (code *)pcVar2;
      if (*(char *)0x1e == '\a') {
        unaff_CS = 0xfc4d;
        uVar5 = uVar4;
        func_0x000fc4d8(0xe60b,uVar4,uVar7,iStack_c,uStack_a,*(undefined1 *)0x22a,
                        *(undefined2 *)0x23a,10);
        *param_3 = uVar5;
        puVar6[1] = uVar7;
        pcStack_12 = (code *)0x0;
        pcStack_10 = (code *)0x0;
      }
      else if (*(char *)0x1e == '\n') {
        unaff_CS = 0xfc5d;
        uVar5 = uVar4;
        func_0x000fc5d1(0xe60b,uVar4,uVar7,iStack_c,uStack_a,10);
        *param_3 = uVar5;
        puVar6[1] = uVar7;
        pcStack_12 = (code *)0x0;
        pcStack_10 = (code *)0x0;
      }
      if (pcStack_12 != (code *)0x0 || pcStack_10 != (code *)0x0) {
        (*pcStack_12)(unaff_CS,uVar4,uVar7,iStack_c,uStack_a,*(undefined1 *)0x22a,
                      *(undefined2 *)0x23a,10);
        *param_3 = uVar4;
        puVar6[1] = uVar7;
      }
      uStack_d = *(undefined1 *)((uVar1 & 0xffe) + (int)*(undefined4 *)0x31e);
    }
    *(undefined1 *)0x30d = 0x1e;
    if ((int)*(undefined4 *)0x31e + 0xfffU < (uint)*(undefined4 *)param_3) {
      uVar3 = *(undefined4 *)param_3;
      *param_3 = (int)uVar3 - 0x1000;
      puVar6[1] = (uint)((ulong)uVar3 >> 0x10);
    }
    param_1 = *(undefined4 *)param_3;
    if ((uint)(*(int *)0x31e + param_2) < (uint)param_1) {
      iStack_16 = 0;
    }
  }
  return;
}



void __cdecl16far FUN_000f_ed1d(void)

{
  uint uVar1;
  char *pcVar2;
  char *pcVar3;
  undefined2 unaff_DS;
  undefined4 uStack_1c;
  int iStack_18;
  undefined2 uStack_16;
  undefined4 uStack_14;
  int iStack_10;
  undefined2 uStack_e;
  undefined1 local_c [10];
  
  *(undefined1 *)0x30d = 0x10;
  func_0x000e5d21();
  pcVar3 = (char *)0xe5d1;
  do {
    if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) {
      *(uint *)0x22 = *(uint *)0x22 | 0x80;
      func_0x000e5d15(pcVar3);
      return;
    }
    *(undefined1 *)0x30d = 0x10;
    *(undefined1 *)0x30c = 3;
    pcVar2 = (char *)0xe60b;
    func_0x000e6224(pcVar3);
    if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) {
      *(uint *)0x22 = *(uint *)0x22 | 0x80;
      func_0x000e5d15(0xe60b);
      return;
    }
    pcVar3 = pcVar2;
  } while (((*(uint *)0x22 & 0x400) == 0) || (*(int *)0x3a != 0));
  *(undefined1 *)0x30c = 4;
  if (((*(byte *)0x226 & 0x20) != 0) && ((*(byte *)0x226 & 0x40) == 0)) {
    if (((*(byte *)0x20 & 1) != 0) && (*(int *)0x6c == 0)) {
      *(undefined1 *)0x319 = 1;
      return;
    }
    if (((*(byte *)0x20 & 2) != 0) && (*(int *)0x76 == 0)) {
      *(undefined1 *)0x319 = 1;
      return;
    }
  }
  *(undefined1 *)0x30c = 5;
  uVar1 = *(int *)0x8e - 1U & 0xfff;
  if ((*(byte *)0x20 & 1) != 0) {
    _iStack_10 = CONCAT22((int)((ulong)*(undefined4 *)0x31e >> 0x10),
                          (int)*(undefined4 *)0x31e + *(int *)0x8c + (uint)*(byte *)0x22b);
    if (*(char *)&DAT_000f_1a9b == '\0') {
      uStack_14 = CONCAT22((int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),
                           (int)*(undefined4 *)&DAT_000f_1b90 + *(int *)0x6c);
    }
    else {
      uStack_14 = *(undefined4 *)0x30e;
    }
  }
  if ((*(byte *)0x20 & 2) != 0) {
    _iStack_18 = CONCAT22((int)((ulong)*(undefined4 *)0x31e >> 0x10),
                          (int)*(undefined4 *)0x31e + *(int *)0x8c + (uint)*(byte *)0x22c);
    if (*(char *)&DAT_000f_1a9b == '\0') {
      uStack_1c = CONCAT22((int)((ulong)*(undefined4 *)&DAT_000f_1b90 >> 0x10),
                           (int)*(undefined4 *)&DAT_000f_1b90 + *(int *)0x76 + *(int *)0x40);
    }
    else {
      uStack_1c = *(undefined4 *)0x312;
    }
  }
  *(undefined2 *)0x3a = *(undefined2 *)0x2c;
  if ((*(char *)&DAT_000f_1a9b != '\0') && ((*(uint *)0x22 & 2) != 0)) {
    *(uint *)0x22 = *(uint *)0x22 & 0xfffd;
    *(int *)0x3a = *(int *)0x3a + -4;
    *(byte *)0x226 = *(byte *)0x226 | 0x40;
  }
  *(undefined1 *)0x30d = 0x11;
  *(undefined1 *)0x30c = 6;
  if ((*(byte *)0x20 & 1) != 0) {
    *(undefined2 *)0xa = (int)uStack_14;
    *(undefined2 *)0xc = (int)((ulong)uStack_14 >> 0x10);
    if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) {
      *(uint *)0x22 = *(uint *)0x22 | 0x80;
      func_0x000e5d15(0xe60b);
      return;
    }
    pcVar2 = (char *)s_TIME_OUT_000f_fdbe + 6;
    func_0x000feb40(0xe60b,(int)_iStack_10,(int)((ulong)_iStack_10 >> 0x10),uVar1,local_c);
  }
  pcVar3 = pcVar2;
  if ((*(byte *)0x20 & 2) != 0) {
    *(undefined2 *)0xa = (int)uStack_1c;
    *(undefined2 *)0xc = (int)((ulong)uStack_1c >> 0x10);
    if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) {
      *(uint *)0x22 = *(uint *)0x22 | 0x80;
      func_0x000e5d15(pcVar2);
      return;
    }
    pcVar3 = (char *)s_TIME_OUT_000f_fdbe + 6;
    func_0x000feb40(pcVar2,(int)_iStack_18,(int)((ulong)_iStack_18 >> 0x10),uVar1,local_c);
  }
  pcVar2 = pcVar3;
  if ((*(byte *)0x226 & 8) != 0) {
    pcVar2 = (char *)s_Clk_Delay_Reg_u4230_000f_fbbb + 4;
    func_0x000fcedf(pcVar3);
    *(byte *)0x226 = *(byte *)0x226 & 0xf7;
  }
  if (0 < *(int *)0x2c) {
    *(byte *)0x226 = *(byte *)0x226 & 0xbf;
  }
  *(uint *)0x22 = *(uint *)0x22 & 0xfbff;
  func_0x000e5d15(pcVar2);
  *(undefined1 *)0x30c = 7;
  return;
}



uint __cdecl16far FUN_000f_ef54(void)

{
  uint uVar1;
  undefined2 unaff_DS;
  uint uStack_e;
  int iStack_c;
  uint uStack_a;
  
  if (*(char *)0x0 == '\0') {
    uStack_e = *(uint *)0x262 & 0xffe;
    *(uint *)0x264 = uStack_e;
    if (uStack_e < *(uint *)0x8c) {
      uStack_a = (uStack_e - *(int *)0x8c) + 0x1000;
    }
    else {
      uStack_a = uStack_e - *(int *)0x8c;
    }
    if ((((*(byte *)0x226 & 4) != 0) && ((*(uint *)*(undefined4 *)0x322 & 0x8000) == 0)) &&
       ((uStack_a & *(uint *)0x224) == 0)) {
      if (uStack_e < *(uint *)0x266) {
        iStack_c = (uStack_e - *(int *)0x266) + 0x1000;
      }
      else {
        iStack_c = uStack_e - *(int *)0x266;
      }
      if ((*(int *)0x248 <= iStack_c + 8) && (*(undefined1 *)0x0 = 1, *(int *)0x25e == 0)) {
        if ((*(byte *)0x20 & 0x10) == 0) {
          uStack_e = uStack_e + 2;
        }
        else {
          uStack_e = uStack_e + 6;
        }
        uStack_e = uStack_e & 0xfff;
        if (uStack_e < *(uint *)0x8c) {
          uStack_a = (uStack_e - *(int *)0x8c) + 0x1000;
        }
        else {
          uStack_a = uStack_e - *(int *)0x8c;
        }
      }
    }
    if ((*(char *)&DAT_000f_1a9b == '\0') || ((*(uint *)0x22 & 2) == 0)) {
      uVar1 = *(uint *)0x224;
    }
    else {
      uVar1 = *(int *)0x224 << 1;
    }
    uStack_a = uStack_a & uVar1;
    *(uint *)0x92 = uStack_e;
  }
  else {
    *(undefined1 *)0x319 = 1;
    uStack_a = 0;
  }
  return uStack_a;
}



void __cdecl16far FUN_000f_f067(void)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  undefined2 uVar6;
  uint uVar7;
  char *pcVar8;
  char *pcVar9;
  undefined2 unaff_DS;
  int *piStack_18;
  
  iVar2 = (uint)*(byte *)0x22f * 10;
  iVar3 = (uint)*(byte *)0x230 * 10;
  _piStack_18 = (int *)CONCAT22(unaff_DS,(int *)0x8c);
  iVar4 = iVar3;
  if ((*(byte *)0x20 & 1) != 0) {
    iVar4 = iVar2;
  }
  bVar1 = *(byte *)0x20;
  *(byte *)0x21 = *(byte *)0x21 | 1;
  *(uint *)0x22 = *(uint *)0x22 | 0x402;
  *(byte *)0x226 = *(byte *)0x226 | 0x40;
  *(undefined1 *)0x319 = 0;
  *(undefined2 *)0x3a = 0;
  *(undefined2 *)0x2c = 0;
  *(undefined2 *)0x2 = 0;
  *(undefined1 *)0x0 = 0;
  uVar5 = (uint)*(byte *)0x20;
  func_0x000f6dc9();
  if ((*(byte *)0x20 & 1) != 0) {
    *(undefined2 *)(iVar2 + 0x66) = 0;
  }
  if ((*(byte *)0x20 & 2) != 0) {
    *(undefined2 *)(iVar3 + 0x66) = 0;
  }
  if ((*(byte *)&DAT_000f_1a86 & 0xdf) == 0) {
    func_0x000e5d21(0xf670,uVar5);
    func_0x000e6224(0xe5d1);
    pcVar8 = (char *)0xe5d1;
    func_0x000e5d15(0xe60b);
    if ((*(byte *)&DAT_000f_1a86 & 0xdf) == 0) {
      while ((pcVar9 = pcVar8, (*(uint *)0x22 & 0x80) == 0 && (*(char *)0x319 == '\0'))) {
        pcVar9 = (char *)s_not_implemented_000f_fe00 + 5;
        uVar6 = func_0x000fef54(pcVar8);
        *(undefined2 *)0x6 = uVar6;
        if (*(char *)0x319 != '\0') break;
        while ((pcVar9 = (char *)s_not_implemented_000f_fe00 + 5, *(int *)0x6 == 0 &&
               (*(char *)0x319 == '\0'))) {
          if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) {
            *(uint *)0x22 = *(uint *)0x22 | 0x80;
            func_0x000e5d15((char *)s_not_implemented_000f_fe00 + 5);
            return;
          }
          func_0x000e5d21((char *)s_not_implemented_000f_fe00 + 5);
          func_0x000e6224(0xe5d1);
          func_0x000e5d15(0xe60b);
          uVar6 = func_0x000fef54(0xe5d1);
          *(undefined2 *)0x6 = uVar6;
          if ((*(byte *)&DAT_000f_1a86 & 0xdf) != 0) {
            *(uint *)0x22 = *(uint *)0x22 | 0x80;
            func_0x000e5d15((char *)s_not_implemented_000f_fe00 + 5);
            return;
          }
        }
        if (*(char *)0x319 != '\0') break;
        pcVar9 = (char *)0xfbe6;
        uVar7 = func_0x000fbe64((char *)s_not_implemented_000f_fe00 + 5,0x8c,unaff_DS,iVar4 + 100,
                                unaff_DS,0xfff);
        uVar5 = *(uint *)0x224;
        *(int *)0x2c = (int)(uVar7 & uVar5) >> ((bVar1 & 0x10) != 0);
        if (*(int *)0x2c == 0) break;
        *(uint *)0x8e = *_piStack_18 + (uVar7 & uVar5) & 0xfff;
        pcVar9 = (char *)s_TIME_OUT_000f_fdbe + 6;
        func_0x000fed1d(0xfbe6);
        if ((*(char *)0x319 != '\0') || ((*(uint *)0x22 & 0x80) != 0)) break;
        pcVar9 = (char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9;
        func_0x000fd94f((char *)s_TIME_OUT_000f_fdbe + 6);
        if ((*(byte *)0x20 & 1) != 0) {
          if ((*(byte *)&DAT_000f_1b88 & 1) != 0) {
            *(byte *)&DAT_000f_1b88 = *(byte *)&DAT_000f_1b88 & 0xfe;
            pcVar9 = (char *)0xf6e5;
            func_0x000f6e5c((char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9,0,0);
            *(uint *)&DAT_000f_046a = *(uint *)&DAT_000f_046a | 4;
          }
          if (*(int *)0x2a < *(int *)(iVar2 + 0x66) + *(int *)0x3a) {
            *(int *)(iVar2 + 0x66) = ((*(int *)(iVar2 + 0x66) + *(int *)0x3a) - *(int *)0x2a) + -1;
          }
          else if (*(int *)(iVar2 + 0x66) + *(int *)0x3a < 0) {
            *(int *)(iVar2 + 0x66) = *(int *)(iVar2 + 0x66) + *(int *)0x3a + *(int *)0x2a + 1;
          }
          else {
            *(int *)(iVar2 + 0x66) = *(int *)(iVar2 + 0x66) + *(int *)0x3a;
          }
          *(byte *)0x233 = *(byte *)0x233 & 0xfe;
        }
        if ((*(byte *)0x20 & 2) != 0) {
          pcVar8 = pcVar9;
          if ((*(byte *)&DAT_000f_1b88 & 2) != 0) {
            *(byte *)&DAT_000f_1b88 = *(byte *)&DAT_000f_1b88 & 0xfd;
            pcVar8 = (char *)0xf6e5;
            func_0x000f6e5c(pcVar9,1,0);
            *(uint *)&DAT_000f_046a = *(uint *)&DAT_000f_046a | 8;
          }
          if (*(int *)0x2a < *(int *)(iVar3 + 0x66) + *(int *)0x3a) {
            *(int *)(iVar3 + 0x66) = ((*(int *)(iVar3 + 0x66) + *(int *)0x3a) - *(int *)0x2a) + -1;
          }
          else if (*(int *)(iVar3 + 0x66) + *(int *)0x3a < 0) {
            *(int *)(iVar3 + 0x66) = *(int *)(iVar3 + 0x66) + *(int *)0x3a + *(int *)0x2a + 1;
          }
          else {
            *(int *)(iVar3 + 0x66) = *(int *)(iVar3 + 0x66) + *(int *)0x3a;
          }
          *(byte *)0x233 = *(byte *)0x233 & 0xfd;
          pcVar9 = pcVar8;
        }
        *_piStack_18 = *(int *)0x8e;
        *(uint *)0x22 = *(uint *)0x22 | 0x100;
        pcVar8 = (char *)0xe6a8;
        func_0x000e6a8e(pcVar9,2);
      }
      if ((*(char *)&DAT_000f_1a9b != '\0') && (*(char *)0x241 != '\x04')) {
        func_0x000e5d21(pcVar9);
        func_0x000e6224(0xe5d1);
        *(undefined2 *)0x3a = 4;
        *(uint *)0x22 = *(uint *)0x22 | 0x100;
        func_0x000e6a8e(0xe60b,2);
        func_0x000e6224(0xe6a8);
        func_0x000e5d15(0xe60b);
      }
    }
    else {
      *(uint *)0x22 = *(uint *)0x22 | 0x80;
      func_0x000e5d15(0xe5d1);
    }
  }
  else {
    *(uint *)0x22 = *(uint *)0x22 | 0x80;
    func_0x000e5d15(0xf670);
  }
  return;
}



void __cdecl16far FUN_000f_f3c8(void)

{
  undefined2 unaff_DS;
  
  if ((*(byte *)0x226 & 2) != 0) {
    if (*(char *)0x240 == '\x03') {
      *(undefined1 *)0x227 = (char)(*(int *)0x234 >> 1);
    }
    else {
      *(undefined1 *)0x227 = *(undefined1 *)0x234;
    }
    if (*(byte *)0x228 <= *(byte *)0x227) {
      *(byte *)0x226 = *(byte *)0x226 & 0xfd;
    }
    *(int *)0x234 = *(int *)0x234 + 1;
  }
  return;
}



void __stdcall16far FUN_000f_f40a(int param_1)

{
  byte *pbVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined1 uStack_b;
  
  if ((*(byte *)0x20 & 1) == 0) {
    uStack_b = 2;
  }
  else {
    uStack_b = 1;
  }
  iVar2 = *(int *)(param_1 * 10 + 0x6c) >> 1;
  if ((*(byte *)(iVar2 + 0x268) & uStack_b) == 0) {
    pbVar1 = (byte *)(iVar2 + 0x268);
    *pbVar1 = *pbVar1 | uStack_b;
    *(undefined1 *)0x227 = 0;
  }
  else {
    *(undefined1 *)0x227 = *(undefined1 *)0x228;
  }
  return;
}



/* clear_item_flag_bit_all (confidence: Mechanism confirmed)
   
   Evidence: `(which_bit)` - clears bit 0 (if `which_bit==0`) or bit 1 (otherwise) of `[i+0x268]`
   for all 165 (`0xA5`) entries `i` - a bulk per-item flag clear, called with `di=1` from the
   boot/reinit sequence */

void __stdcall16far clear_item_flag_bit_all(word which_bit)

{
  undefined2 unaff_DS;
  undefined2 uStack_c;
  undefined1 uStack_9;
  
  if (which_bit == 0) {
    uStack_9 = 0xfe;
  }
  else {
    uStack_9 = 0xfd;
  }
  for (uStack_c = 0; uStack_c < 0xa5; uStack_c = uStack_c + 1) {
    *(byte *)(uStack_c + 0x268) = *(byte *)(uStack_c + 0x268) & uStack_9;
  }
  return;
}



void __cdecl16far FUN_000f_f4a2(void)

{
  char cVar1;
  undefined4 uVar2;
  undefined2 unaff_DS;
  int iStack_a;
  
  *(undefined1 *)0x226 = 0;
  *(undefined1 *)0x22c = 0;
  *(undefined1 *)0x22b = 0;
  *(undefined1 *)0x318 = 0;
  *(undefined1 *)0x228 = 0;
  *(undefined1 *)0x227 = 0;
  *(undefined2 *)0x234 = 0;
  if ((*(char *)0x1e == '\a') || (*(char *)0x1e == '\x06')) {
    *(byte *)0x226 = *(byte *)0x226 | 2;
    *(byte *)0x228 = (byte)((uint)*(undefined2 *)&DAT_000f_1afe >> 8) & 0xf;
  }
  if (*(char *)0x1e == '\x06') {
    for (iStack_a = 0; iStack_a < 0xa5; iStack_a = iStack_a + 1) {
      *(undefined1 *)(iStack_a + 0x268) = 0;
    }
  }
  iStack_a = (uint)*(byte *)0x1e * 2;
  if ((*(byte *)0x20 & 0x10) != 0) {
    iStack_a = iStack_a + 1;
  }
  *(undefined1 *)0x22a = *(undefined1 *)((int)*(undefined4 *)&DAT_000f_1bb4 + iStack_a);
  if (*(char *)0x244 == '\x05') {
    *(undefined1 *)0x22a = 2;
  }
  if ((*(byte *)0x20 & 0x10) == 0) {
    *(undefined2 *)0x224 = 0xfffc;
  }
  else {
    *(undefined2 *)0x224 = 0xfff8;
  }
  if ((*(byte *)0x240 & 8) != 0) {
    *(int *)0x224 = *(int *)0x224 << 1;
  }
  if ((*(char *)0x1e == '\x06') || (*(char *)0x1e == '\a')) {
    *(int *)0x23a = *(int *)0x252 << 1;
  }
  else {
    *(undefined2 *)0x23a = *(undefined2 *)0x252;
  }
  if (*(char *)&DAT_000f_1a9b == '\0') {
    *(undefined2 *)0x23e = 0;
    *(undefined1 *)0x22f = 0;
    if ((*(byte *)0x240 & 8) == 0) {
      *(undefined1 *)0x230 = 1;
    }
    else {
      *(undefined1 *)0x230 = 0;
    }
  }
  else {
    *(undefined2 *)0x23e = 0x1000;
    *(undefined1 *)0x22f = 0;
    *(undefined1 *)0x230 = 1;
  }
  if (*(byte *)0x241 < 3) {
    uVar2 = *(undefined4 *)&DAT_000f_1b90;
    *(int *)0x30e = (int)uVar2 + 0x1000;
    *(undefined2 *)0x310 = (int)((ulong)uVar2 >> 0x10);
  }
  else {
    *(undefined2 *)0x30e = 0x268;
    *(undefined2 *)0x310 = unaff_DS;
    if ((*(char *)0x240 == '\b') || (*(char *)0x240 == '\a')) {
      uVar2 = *(undefined4 *)0x30e;
      *(int *)0x312 = (int)uVar2 + 0x52;
      *(undefined2 *)0x314 = (int)((ulong)uVar2 >> 0x10);
    }
    else {
      uVar2 = *(undefined4 *)0x30e;
      *(undefined2 *)0x312 = (int)uVar2;
      *(undefined2 *)0x314 = (int)((ulong)uVar2 >> 0x10);
    }
  }
  if ((((*(char *)0x1e == '\x05') || (*(char *)0x1e == '\x04')) || (*(char *)0x1e == '\n')) ||
     (*(char *)0x1e == '\x02')) {
    *(byte *)0x226 = *(byte *)0x226 | 1;
  }
  if ((*(char *)0x240 == '\b') && (*(char *)0x241 != '\0')) {
    *(byte *)0x226 = *(byte *)0x226 | 1;
  }
  if ((*(char *)0x241 == '\x04') && (*(char *)0x240 == '\x03')) {
    *(byte *)0x226 = *(byte *)0x226 | 0x20;
  }
  if ((*(byte *)0x20 & 8) != 0) {
    func_0x000fd8ef();
  }
  cVar1 = *(char *)0x1e;
  if (cVar1 != '\0') {
    if (cVar1 == '\x01') {
      *(undefined1 *)0x22e = 0;
      return;
    }
    if (cVar1 == '\x02') {
      *(undefined1 *)0x22e = 0xff;
      return;
    }
    if (cVar1 != '\x03') {
      if (cVar1 == '\x04') {
        if ((*(byte *)0x20 & 0x10) != 0) {
          *(undefined1 *)0x22e = 2;
          return;
        }
        *(undefined1 *)0x22e = 1;
        return;
      }
      if (cVar1 == '\x05') {
        *(undefined1 *)0x22e = 1;
        return;
      }
      if ((cVar1 != '\x06') && (cVar1 != '\t')) {
        return;
      }
    }
  }
  if ((*(byte *)0x20 & 0x10) == 0) {
    *(undefined1 *)0x22e = 0;
  }
  else {
    *(undefined1 *)0x22e = 1;
  }
  return;
}



void __cdecl16far FUN_000f_f704(void)

{
  char cVar1;
  undefined2 unaff_CS;
  char *pcVar2;
  undefined2 unaff_DS;
  
  if ((*(byte *)&DAT_000f_1a86 & 0xdf) == 0) {
    *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 & 0xdf;
    while ((*(byte *)0x1a98 & 0xf) != 0) {
      func_0x000e693c(unaff_CS);
      unaff_CS = 0xe693;
    }
    func_0x000e5d15(unaff_CS);
    *(undefined1 *)0x233 = 3;
    func_0x000f8234(0xe5d1);
    func_0x000f80a6(0xf80a);
    func_0x000fc680(0xf80a);
    func_0x000fdc88(0xfc68);
    pcVar2 = (char *)0xff4a;
    func_0x000ff4a2((char *)s_2230_2220_boot___160__000f_fcd3 + 5);
    cVar1 = *(char *)0x241;
    if (((cVar1 == '\0') || (cVar1 == '\x01')) || (cVar1 == '\x02')) {
      pcVar2 = (char *)s_2230_2220_Power_up_tests_complet_000f_fc47 + 9;
      func_0x000fd471(0xff4a);
    }
    else if ((cVar1 == '\x03') || (cVar1 == '\x04')) {
      pcVar2 = (char *)0xfd28;
      func_0x000fd28c(0xff4a);
    }
    else if (cVar1 == '\x05') {
      pcVar2 = (char *)s_Clk_Delay_Reg_u4230_000f_fbbb + 4;
      func_0x000fcaf6(0xff4a);
    }
    else if (cVar1 == '\x06') {
      pcVar2 = (char *)s_comm_stat_u1x23_000f_fc0b + 5;
      func_0x000fd006(0xff4a);
    }
    else if (cVar1 == '\a') {
      pcVar2 = (char *)s_Clk_Delay_Reg_u4230_000f_fbbb + 4;
      func_0x000fccec(0xff4a);
    }
    func_0x000fdb3e(pcVar2);
    *(byte *)&DAT_000f_1a86 = *(byte *)&DAT_000f_1a86 | 0x20;
  }
  return;
}


