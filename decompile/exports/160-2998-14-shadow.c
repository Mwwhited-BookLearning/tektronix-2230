
/* WARNING: Control flow encountered bad instruction data */

void FUN_0009_0060(void)

{
  uint *puVar1;
  int *piVar2;
  uint uVar3;
  char cVar4;
  char in_AL;
  byte in_DL;
  byte bVar5;
  int in_BX;
  int iVar6;
  uint uVar7;
  undefined1 uVar8;
  int unaff_BP;
  int iVar9;
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  cVar4 = *(char *)(in_BX + unaff_DI);
  piVar2 = (int *)(unaff_BP + unaff_DI + 1);
  *piVar2 = *piVar2 + -0x61;
  puVar1 = (uint *)(unaff_BP + unaff_SI + 1);
  *puVar1 = *puVar1 & 0xff9f;
  *(int *)(unaff_BP + -0x60ff) = *(int *)(unaff_BP + -0x60ff) + -0x7d;
  iVar9 = unaff_BP + 1;
  iVar6 = CONCAT11((char)((uint)in_BX >> 8),(char)in_BX + *(char *)(in_BX + 0x3c83));
  iVar6 = iVar6 + *(int *)(iVar6 + 0x5c83);
  iVar6 = iVar6 + *(int *)(iVar6 + -0x1b7d);
  uVar7 = iVar6 + *(int *)(iVar6 + -0x767d);
  *(uint *)(iVar9 + unaff_SI) = *(uint *)(iVar9 + unaff_SI) ^ 5;
  *(uint *)(unaff_DI + -0x60fa) = *(uint *)(unaff_DI + -0x60fa) & 0xff83;
  uVar3 = unaff_DI + 2;
  *(byte *)(uVar7 + 0x483) = *(byte *)(uVar7 + 0x483) | (byte)uVar7;
  *(uint *)(uVar7 + 0x83) = *(uint *)(uVar7 + 0x83) | uVar7;
  uVar8 = (undefined1)(uVar7 >> 8);
  bVar5 = (byte)uVar7 | *(byte *)(uVar7 + 0xe083);
  iVar6 = CONCAT11(uVar8,bVar5 | *(byte *)(CONCAT11(uVar8,bVar5) + -0x567d));
  piVar2 = (int *)(iVar6 + uVar3 + 0x15);
  *piVar2 = *piVar2 + -0x61;
  puVar1 = (uint *)(iVar9 + uVar3 + 0x15);
  *puVar1 = *puVar1 | 0xff9f;
  *(uint *)(iVar9 + unaff_SI) = *(uint *)(iVar9 + unaff_SI) ^ 0x16;
  piVar2 = (int *)(iVar6 + unaff_SI + -0x60ea);
  *piVar2 = *piVar2 + 0x7d;
  piVar2 = (int *)(iVar6 + (uVar3 | 0x15));
  *piVar2 = *piVar2 - (uint)(0x60 < (byte)((in_AL - cVar4) + 0x3eU));
  *(byte *)0xfe14 = *(byte *)0xfe14 & in_DL;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



int FUN_0009_009b(void)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  byte in_AL;
  uint uVar4;
  int in_BX;
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_DS;
  char in_AF;
  
  *(int *)(in_BX + unaff_SI) = *(int *)(in_BX + unaff_SI) + 10;
  uVar4 = CONCAT11(((in_AL & 10) == 0) << 6 | in_AF << 4 | ((POPCOUNT(in_AL & 10) & 1U) == 0) << 2,
                   (char)(in_AL & 10)) | 0x200;
  puVar1 = (uint *)(in_BX + unaff_DI + -0x60ec);
  uVar2 = *puVar1;
  *puVar1 = *puVar1 + 0x7d;
  uVar3 = uVar4 + 0x839f;
  return uVar3 + (uVar2 < 0xff83) + -0x7c61 +
         (uint)(0x7c60 < uVar4 || CARRY2(uVar3,(uint)(uVar2 < 0xff83)));
}



/* WARNING: Control flow encountered bad instruction data */

void FUN_0009_00fc(int param_1,int param_2)

{
  undefined1 uVar1;
  undefined2 uVar2;
  char *pcVar3;
  char in_AL;
  char cVar4;
  undefined4 in_EDX;
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  char in_stack_0000000e;
  char *in_stack_0000001a;
  char in_stack_0000001e;
  undefined2 uStack002e;
  int in_stack_00000030;
  int in_stack_00000032;
  int in_stack_00000036;
  undefined2 uStack003c;
  int in_stack_0000003e;
  int in_stack_00000040;
  undefined2 in_stack_00000046;
  char in_stack_0000004a;
  undefined2 *in_stack_0000004c;
  int in_stack_0000004e;
  int in_stack_00000050;
  undefined2 in_stack_00000056;
  char in_stack_0000005a;
  char in_stack_0000005c;
  int in_stack_0000005e;
  int in_stack_00000060;
  int in_stack_0000006e;
  int in_stack_00000070;
  char in_stack_0000007a;
  int in_stack_0000007e;
  int in_stack_00000080;
  char in_stack_0000008a;
  int in_stack_0000008e;
  int in_stack_00000090;
  undefined2 uStack0098;
  int in_stack_0000009e;
  int in_stack_000000a0;
  char in_stack_000000aa;
  int in_stack_000000ac;
  int in_stack_000000ae;
  char in_stack_000000b8;
  
  *(char *)(unaff_BP + unaff_SI) = *(char *)(unaff_BP + unaff_SI) + in_AL;
  *(char *)(param_2 + param_1) = *(char *)(param_2 + param_1) + in_stack_0000000e;
  pcVar3 = (char *)CONCAT22((int)((ulong)in_EDX >> 0x10),in_stack_0000001a);
  *pcVar3 = *pcVar3 + in_stack_0000001e;
  uStack002e = 0x200;
  uStack003c = 0;
  *(char *)(in_stack_00000032 + in_stack_00000030) =
       *(char *)(in_stack_00000032 + in_stack_00000030) +
       (char)*(undefined2 *)(in_stack_00000036 + in_stack_00000030) * '\x02';
  uVar1 = in(in_stack_00000046);
  *(undefined1 *)0x0 = uVar1;
  *(char *)(in_stack_00000040 + in_stack_0000003e) =
       *(char *)(in_stack_00000040 + in_stack_0000003e) + in_stack_0000004a;
  uVar2 = in(in_stack_00000056);
  *in_stack_0000004c = uVar2;
  *(char *)(in_stack_00000050 + in_stack_0000004e) =
       *(char *)(in_stack_00000050 + in_stack_0000004e) + in_stack_0000005a;
  *(char *)(in_stack_00000060 + in_stack_0000005e) =
       *(char *)(in_stack_00000060 + in_stack_0000005e) + in_stack_0000005c;
  *(char *)(in_stack_00000070 + in_stack_0000006e) =
       *(char *)(in_stack_00000070 + in_stack_0000006e) + in_stack_0000007a;
  *(char *)(in_stack_00000080 + in_stack_0000007e) =
       *(char *)(in_stack_00000080 + in_stack_0000007e) + in_stack_0000008a;
  uStack0098 = 0x1c3;
  cVar4 = func_0x0009b810();
  *(char *)(in_stack_00000090 + in_stack_0000008e) =
       *(char *)(in_stack_00000090 + in_stack_0000008e) + cVar4;
  *(char *)(in_stack_000000a0 + in_stack_0000009e) =
       *(char *)(in_stack_000000a0 + in_stack_0000009e) + in_stack_000000aa;
  *(char *)(in_stack_000000ae + in_stack_000000ac) =
       *(char *)(in_stack_000000ae + in_stack_000000ac) + in_stack_000000b8;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



/* WARNING: Control flow encountered bad instruction data */

void FUN_0009_0133(int param_1,int param_2)

{
  char in_AL;
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_SS;
  char in_stack_0000000e;
  int in_stack_00000010;
  int in_stack_00000012;
  char in_stack_0000001c;
  
  *(char *)(unaff_BP + unaff_SI) = *(char *)(unaff_BP + unaff_SI) + in_AL;
  *(char *)(param_2 + param_1) = *(char *)(param_2 + param_1) + in_stack_0000000e;
  *(char *)(in_stack_00000012 + in_stack_00000010) =
       *(char *)(in_stack_00000012 + in_stack_00000010) + in_stack_0000001c;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



void __stdcall16far FUN_0009_12cd(undefined4 param_1,undefined2 param_2)

{
  byte *pbVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  undefined2 uVar6;
  
  uVar6 = 0x8f80;
  uVar5 = 38000;
  uVar2 = func_0x0009470e();
  *(int *)0x6d2 = *(int *)0x6d2 + -1;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  if ((*(byte *)(*(int *)0x6d2 * 9 + iVar3) & 1) != 0) {
    pbVar1 = (byte *)(*(int *)0x6d2 * 9 + iVar3);
    *pbVar1 = *pbVar1 & 0xfe;
    uVar5 = 0x911e;
    func_0x000914c2(38000,*(undefined2 *)0x6d2,iVar3,uVar4,param_2,uVar6);
  }
  uVar4 = uVar5;
  if (*(char *)*(undefined4 *)0x6ce != '\0') {
    uVar4 = 0x911e;
    func_0x0009134c(uVar5,(int)*(undefined4 *)0x6ce,(int)((ulong)*(undefined4 *)0x6ce >> 0x10),
                    *(undefined2 *)0x6d2);
  }
  *(undefined1 *)0x4f3 = 0;
  func_0x0009470e(uVar4,uVar2);
  return;
}



void __stdcall16far FUN_0009_134c(byte *param_1,int param_2)

{
  bool bVar1;
  byte *pbVar2;
  undefined2 uVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  undefined2 uVar6;
  undefined4 local_16;
  int local_a;
  
  uVar6 = 0x41;
  uVar3 = func_0x0009470e();
  bVar1 = false;
  local_16 = (undefined1 *)
             CONCAT22((int)((ulong)*(undefined4 *)0x1ddc >> 0x10),
                      (undefined1 *)((int)*(undefined4 *)0x1ddc + (param_2 + 0x13) * 9));
  uVar5 = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
  iVar4 = (int)*(undefined4 *)0x1ddc;
  *(int *)0x6ae = (uint)*(byte *)(iVar4 + 0x141) * 0x100 + (uint)*(byte *)(iVar4 + 0x140);
  *(undefined2 *)0x6b0 = 0;
  func_0x000e9858(38000,(int)*(undefined4 *)0x6ae,(int)((ulong)*(undefined4 *)0x6ae >> 0x10),8,
                  param_2 * 0x32 + 0x55,0x10,0x10,param_2 + 0x13,uVar6);
  *(undefined2 *)0x6aa = 0;
  iVar4 = func_0x000ea13b(0xe951,0);
  *(int *)0x6aa = *(int *)0x6aa + iVar4;
  while( true ) {
    pbVar2 = param_1;
    param_1 = (byte *)CONCAT22(param_1._2_2_,(byte *)param_1 + 1);
    if (*pbVar2 == 0) break;
    if ((*(int *)0x6aa + 7 < 0x100) && (*(uint *)0x6ae < 0x359c)) {
      iVar4 = func_0x000ea13b(0xea13,*pbVar2 & 0x7f);
      *(int *)0x6aa = *(int *)0x6aa + iVar4;
    }
    else {
      bVar1 = true;
      *param_1 = 0;
    }
  }
  func_0x000e9858(0xea13,(int)*(undefined4 *)0x6ae + 2,(int)((ulong)*(undefined4 *)0x6ae >> 0x10),0,
                  0,0x10,0x10,0x23);
  *local_16 = 0x11;
  uVar5 = 38000;
  func_0x0009470e(0xe951,0x8f80);
  if (bVar1) {
    uVar5 = 0x8511;
    func_0x00085301(38000,0x38);
  }
  local_a = -1;
  iVar4 = local_a;
  do {
    local_a = iVar4;
    iVar4 = local_a + 1;
    if (*(char *)(local_a + 0x6bd) == '\x10') break;
  } while (iVar4 < 0x10);
  if (iVar4 < 0x10) {
    *(undefined1 *)(local_a + 0x6bd) = (char)param_2;
  }
  *(undefined1 *)(param_2 + 0x6bc) = 0x10;
  func_0x0009470e(uVar5,uVar3);
  return;
}



void __stdcall16far FUN_0009_14c2(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  
  uVar4 = (undefined2)((ulong)param_2 >> 0x10);
  iVar1 = (int)param_2;
  iVar3 = iVar1 + param_1 * 9;
  iVar2 = iVar1 + (uint)*(byte *)(param_1 + 0x6bc) * 9;
  iVar2 = (uint)*(byte *)(iVar2 + 6) * 0x100 + (uint)*(byte *)(iVar2 + 5);
  iVar1 = ((uint)*(byte *)(iVar1 + 0x96) * 0x100 + (uint)*(byte *)(iVar1 + 0x95)) - iVar2;
  iVar3 = (uint)*(byte *)(iVar3 + 6) * 0x100 + (uint)*(byte *)(iVar3 + 5);
  func_0x000915ee();
  func_0x000fbc09(0x911e,iVar2,0,iVar3,0,iVar1,uVar4);
  func_0x000fbc09(0xfbbe,iVar2 + param_3,0,iVar3 + param_3,0,iVar1);
  return;
}



void __stdcall16far FUN_0009_15ee(uint param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  byte local_f;
  int local_a;
  
  for (local_a = 0; (*(byte *)(local_a + 0x6bc) != param_1 && (local_a < 0x10));
      local_a = local_a + 1) {
  }
  *(undefined1 *)(local_a + 0x6bc) = *(undefined1 *)(param_1 + 0x6bc);
  local_f = (byte)param_1;
  while (local_f != 0x10) {
    local_f = *(byte *)(local_f + 0x6bc);
    if (0x10 < local_f) {
      local_f = 0x10;
      *(undefined1 *)0x6d4 = 1;
    }
    uVar3 = (undefined2)((ulong)param_3 >> 0x10);
    iVar1 = (int)param_3 + (uint)local_f * 9;
    iVar2 = ((uint)*(byte *)(iVar1 + 6) * 0x100 + (uint)*(byte *)(iVar1 + 5)) - param_2;
    *(undefined1 *)(iVar1 + 5) = (char)iVar2;
    *(undefined1 *)(iVar1 + 6) = (char)((uint)iVar2 >> 8);
  }
  *(undefined1 *)(param_1 + 0x6bc) = 0xff;
  return;
}



/* handle_gpib_device_clear (confidence: Mechanism confirmed; shape matches a GPIB Device Clear
   (DCL) handler)
   
   Evidence: Clears a 36-entry address/status table, dispatches on command code `[0x686]`,
   busy-waits via `restart_current_task` for a pending operation to finish, then calls the main
   ROM's `decimate_peakdet_samples` (cross-ROM)
   
   (physical 0x0916D6, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far handle_gpib_device_clear(void)

{
  ulong uVar1;
  undefined *puVar2;
  undefined1 *puVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  undefined2 uVar6;
  undefined *local_14;
  undefined4 local_10;
  int local_a;
  
  uVar6 = 0x8f80;
  uVar4 = func_0x0009470e();
  _local_14 = (undefined *)CONCAT22(unaff_DS,(undefined *)&DAT_0009_062c);
  uVar5 = 38000;
  func_0x0009470e(38000,0x41,uVar6);
  local_10 = (undefined1 *)
             CONCAT22((int)((ulong)*(undefined4 *)0x1ddc >> 0x10),
                      (undefined1 *)((int)*(undefined4 *)0x1ddc + 0xa2));
  for (local_a = 0x12; puVar3 = local_10, puVar2 = _local_14, local_a < 0x24; local_a = local_a + 1)
  {
    local_10 = (undefined1 *)CONCAT22(local_10._2_2_,(undefined1 *)local_10 + 9);
    *puVar3 = 0;
    uVar1 = (ulong)_local_14 >> 0x10;
    _local_14 = (undefined *)CONCAT22((int)uVar1,local_14 + 1);
    *puVar2 = 0xff;
  }
  if (*(char *)0x686 == '\x11') {
    *(undefined1 *)0x468 = 1;
  }
  else if (*(char *)0x686 == '!') {
    *(undefined2 *)0x548 = 0xffff;
  }
  else if (*(char *)0x1b78 == '\0') {
    *(undefined1 *)0x686 = 0;
    *(undefined2 *)0x548 = 0xffff;
    *(undefined1 *)0x1bf6 = 0xff;
    while (*(int *)0x548 != 0) {
      func_0x000e693c(uVar5);
      uVar5 = 0xe693;
    }
    *(undefined1 *)0x1bf6 = 0;
    *(undefined2 *)0x548 = 0xffff;
  }
  *(undefined1 *)0x686 = 0;
  func_0x000e9858(uVar5,0x2264,0,0,0,0x10,0x10,0x23);
  func_0x0009470e(0xe951,0x8f80);
  *(undefined1 *)0x4f3 = 0;
  func_0x0009470e(38000,uVar4);
  return;
}



void __cdecl16far FUN_0009_17ba(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2);
  return;
}



void __cdecl16far FUN_0009_1806(void)

{
  return;
}



void __cdecl16far FUN_0009_1830(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  
  uVar1 = 0x41;
  func_0x0009470e();
  *(undefined1 *)0x7b4 = 0x10;
  func_0x0009470e(38000,0x8f80,uVar1);
  *(byte *)0x603 = *(byte *)0x603 | 1;
  return;
}



void __cdecl16far FUN_0009_1856(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  
  uVar1 = 0x41;
  func_0x0009470e();
  *(undefined1 *)0x7b4 = 0;
  func_0x0009470e(38000,0x8f80,uVar1);
  *(byte *)0x603 = *(byte *)0x603 | 1;
  return;
}



void __cdecl16far FUN_0009_187c(void)

{
  char cVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  undefined2 uVar6;
  byte local_d;
  int local_a;
  
  cVar1 = *(char *)0x5fe;
  if (cVar1 == '\x18') {
    local_d = 1;
  }
  else if (cVar1 == '-') {
    local_d = 2;
  }
  else if (cVar1 == '<') {
    local_d = 0x40;
  }
  else {
    if (cVar1 != 'R') {
      *(undefined1 *)0x623 = 4;
      return;
    }
    local_d = 0x20;
  }
  cVar1 = *(char *)((int)*(undefined4 *)0x732 + 0x16);
  uVar6 = 0x41;
  func_0x0009470e();
  if (cVar1 == '\0') {
    *(undefined1 *)0x7b4 = 0x10;
  }
  local_a = 0;
  iVar2 = *(int *)0x464;
  uVar5 = 38000;
  do {
    uVar4 = uVar5;
    if (0x1d < local_a) break;
    *(byte *)0x7b4 = *(byte *)0x7b4 | local_d;
    uVar4 = 0xe693;
    func_0x000e693c(uVar5,uVar6);
    uVar5 = uVar4;
    local_a = local_a + 1;
  } while (iVar2 == *(int *)0x464);
  *(byte *)0x7b4 = *(byte *)0x7b4 & ~local_d;
  iVar3 = *(int *)0x464;
  func_0x0009470e(uVar4,0x8f80);
  if (iVar2 == iVar3) {
    func_0x00085301(38000,0x36);
  }
  else {
    *(byte *)0x603 = *(byte *)0x603 | 1;
  }
  return;
}



void __cdecl16far FUN_0009_1948(void)

{
  undefined4 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  
  uVar3 = 0x41;
  func_0x0009470e();
  uVar1 = *(undefined4 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10);
  uVar2 = (undefined2)((ulong)uVar1 >> 0x10);
  uVar4 = (undefined2)uVar1;
  func_0x0009470e(38000,0x8f80,uVar3,uVar4,uVar2);
  func_0x00083241(38000,0x22);
  func_0x00082d01(0x82c9,uVar4,uVar2);
  func_0x00083241(0x82c9,0x22);
  return;
}



void __cdecl16far FUN_0009_199e(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 local_a;
  
  local_a = 0;
  uVar3 = 0x41;
  uVar2 = 38000;
  uVar1 = func_0x0009470e();
  if ((*(byte *)0x1b73 & 2) == 0) {
    local_a = 0x1b;
  }
  else if (*(char *)0x1b78 == '\0') {
    if ((*(byte *)0x1b73 & 0x10) == 0) {
      DAT_4000_07de = *(undefined1 *)0x257;
    }
    else {
      local_a = 0x33;
    }
  }
  else if ((*(uint *)0x222 & 1) == 0) {
    local_a = 0x33;
  }
  else {
    uVar2 = 0xe6a8;
    func_0x000e6e60(38000,uVar3);
  }
  func_0x0009470e(uVar2,uVar1);
  if (local_a == 0x1b) {
    *(undefined1 *)0x623 = 0x1b;
  }
  else if (local_a != 0) {
    func_0x00085301(38000,local_a);
  }
  return;
}



void __cdecl16far FUN_0009_1a47(void)

{
  byte bVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  uVar3 = 0x41;
  uVar2 = func_0x0009470e();
  bVar1 = *(byte *)0x1b73;
  func_0x0009470e(38000,uVar2,uVar3);
  if ((bVar1 & 2) == 0) {
    func_0x00085301(38000,0x36);
  }
  return;
}



void __cdecl16far FUN_0009_1afa(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2);
  return;
}



void __cdecl16far FUN_0009_1b62(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2);
  return;
}



void __cdecl16far FUN_0009_1bae(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  undefined1 local_14 [10];
  undefined2 local_a;
  
  uVar2 = 0x41;
  local_a = func_0x0009470e();
  if (*(char *)0x1b78 == '\0') {
    uVar1 = 38000;
    func_0x0009470e(38000,local_a,uVar2);
    *(undefined1 *)0x623 = 0x1b;
    local_14[0] = 0;
  }
  else {
    func_0x0009470e(38000,local_a,uVar2);
    uVar2 = 38000;
    while (*(char *)0x6bb != '\0') {
      func_0x000838c0(uVar2);
      uVar2 = 0x82c9;
    }
    func_0x000eac86(uVar2,0x693);
    uVar1 = 0x823f;
    func_0x000827db(0xea34,local_14);
  }
  func_0x00082d01(uVar1,local_14);
  return;
}



void __cdecl16far FUN_0009_1c1e(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  if (*(char *)0x1b78 == '\0') {
    func_0x0009470e(38000,uVar1,uVar2);
    *(undefined1 *)0x623 = 0x1b;
  }
  else if (*(char *)(*(int *)0x570 * 0x10 + 0x18e) == '\x19') {
    func_0x0009470e(38000,uVar1,uVar2);
  }
  else {
    func_0x0009470e(38000,uVar1,uVar2);
  }
  return;
}



void __cdecl16far FUN_0009_1c9b(void)

{
  int iVar1;
  char cVar2;
  char cVar3;
  undefined2 uVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  undefined2 unaff_DS;
  undefined2 uVar8;
  int local_1c;
  int local_18;
  char local_11;
  
  local_11 = *(char *)0x692;
  uVar8 = 0x41;
  uVar7 = 38000;
  uVar4 = func_0x0009470e();
  iVar1 = *(int *)0x570;
  iVar5 = iVar1 * 2;
  iVar6 = *(int *)0x570 * 0x10;
  cVar2 = *(char *)(iVar1 * 0x14 + 0x1c18);
  cVar3 = *(char *)((iVar5 + 1) * 10 + 0x1c18);
  if (((*(byte *)(iVar6 + 399) | 0xf4) == 0xf4) || ((*(byte *)(iVar6 + 399) & 0xb) == 1)) {
    local_1c = (iVar5 + 1) * 10 + 0x1c14;
    local_18 = iVar1 * 0x14 + 0x1c14;
    if ((local_11 != '\0') && ((*(byte *)(iVar6 + 399) | 0xf4) != 0xf4)) {
      local_11 = '\0';
      uVar7 = 0x8511;
      func_0x00085301(38000,0x36,uVar8);
    }
  }
  else if ((*(byte *)(iVar6 + 399) & 0xb) == 2) {
    local_18 = (iVar5 + 1) * 10 + 0x1c14;
    local_1c = iVar1 * 0x14 + 0x1c14;
    if (local_11 == '\0') {
      local_11 = '\x01';
      uVar7 = 0x8511;
      func_0x00085301(38000,0x36,uVar8);
    }
  }
  else if (local_11 == '\0') {
    local_1c = (iVar5 + 1) * 10 + 0x1c14;
    local_18 = iVar1 * 0x14 + 0x1c14;
  }
  else {
    local_18 = (iVar5 + 1) * 10 + 0x1c14;
    local_1c = iVar1 * 0x14 + 0x1c14;
  }
  if ((*(char *)0x1b78 == '\0') || (((*(byte *)(iVar6 + 399) | 0xf4) == 0xf4 && (cVar2 != cVar3))))
  {
    func_0x0009470e(uVar7,uVar4);
    *(undefined1 *)0x623 = 0x1b;
  }
  else if (((*(byte *)(local_18 + 6) & 1) == 0) ||
          (((*(byte *)(local_1c + 6) & 1) == 0 && ((*(byte *)(iVar6 + 399) | 0xf4) == 0xf4)))) {
    func_0x0009470e(uVar7,uVar4);
  }
  else {
    func_0x0009470e(uVar7,uVar4);
  }
  *(char *)0x692 = local_11;
  return;
}



void __cdecl16far FUN_0009_1ec3(void)

{
  int iVar1;
  char cVar2;
  char cVar3;
  char cVar4;
  undefined2 uVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 unaff_DS;
  undefined1 *puVar9;
  undefined2 uVar10;
  byte local_25;
  undefined1 local_24 [10];
  int local_1a;
  
  local_25 = *(byte *)0x692;
  uVar10 = 0x41;
  uVar5 = func_0x0009470e();
  iVar6 = *(int *)0x570 * 0x10;
  iVar1 = *(int *)0x570;
  iVar7 = iVar1 * 2 + 1;
  cVar2 = *(char *)(iVar1 * 0x14 + 0x1c18);
  cVar3 = *(char *)(iVar7 * 10 + 0x1c18);
  func_0x0009470e(38000,0x8f80,uVar10);
  uVar10 = 38000;
  while (*(char *)0x6bb != '\0') {
    func_0x000838c0(uVar10);
    uVar10 = 0x82c9;
  }
  if (((*(byte *)(iVar6 + 399) | 0xf4) == 0xf4) || ((*(byte *)(iVar6 + 399) & 0xb) == 1)) {
    puVar9 = local_24;
    uVar8 = 0xea34;
    func_0x000eac86(uVar10,0x69d);
    if ((local_25 != 0) && ((*(byte *)(iVar6 + 399) | 0xf4) != 0xf4)) {
      local_25 = 0;
      uVar8 = 0x8511;
      func_0x00085301(0xea34,0x36,puVar9);
    }
  }
  else if ((*(byte *)(iVar6 + 399) & 0xb) == 2) {
    puVar9 = local_24;
    uVar8 = 0xea34;
    func_0x000eac86(uVar10,0x6a7);
    if (local_25 == 0) {
      local_25 = 1;
      uVar8 = 0x8511;
      func_0x00085301(0xea34,0x36,puVar9);
    }
  }
  else if (local_25 == 0) {
    puVar9 = local_24;
    uVar8 = 0xea34;
    func_0x000eac86(uVar10,0x69d);
  }
  else {
    puVar9 = local_24;
    uVar8 = 0xea34;
    func_0x000eac86(uVar10,0x6a7);
  }
  uVar10 = 38000;
  func_0x0009470e(uVar8,0x41,puVar9);
  cVar4 = '\0';
  if ((*(char *)0x1b78 == '\0') || (((*(byte *)(iVar6 + 399) | 0xf4) == 0xf4 && (cVar2 != cVar3))))
  {
    cVar4 = '\x1b';
    local_24[0] = 0;
  }
  else {
    iVar6 = (iVar1 * 2 + (uint)local_25) * 10;
    _local_1a = CONCAT22(unaff_DS,iVar6 + 0x1c14);
    if (((*(int *)(iVar1 * 0x1c + 0x576) == 0) || (*(int *)(iVar7 * 0xe + 0x576) == 0)) &&
       (*(char *)(iVar6 + 0x1c1b) == '\0')) {
      uVar10 = 0x8511;
      func_0x00085301(38000,0x34);
    }
    func_0x000827db(uVar10,local_24);
    uVar10 = 0x823f;
  }
  func_0x0009470e(uVar10,uVar5);
  if (cVar4 != '\0') {
    *(char *)0x623 = cVar4;
  }
  *(byte *)0x692 = local_25;
  func_0x00082d01(38000,local_24);
  return;
}



void __cdecl16far FUN_0009_20db(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x692 = 0;
  return;
}



void __cdecl16far FUN_0009_20ea(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x692 = 1;
  return;
}



void __cdecl16far FUN_0009_20f9(void)

{
  return;
}



void __cdecl16far FUN_0009_212a(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  undefined2 local_e;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  if (((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 1) == 0) ||
     ((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 0x40) != 0)) {
    local_e = *(int *)(*(int *)0x570 * 0x1c + 0x576) >> 2;
  }
  else {
    local_e = *(int *)(*(int *)0x570 * 0x1c + 0x576);
  }
  func_0x0009470e(38000,uVar1,uVar2);
  func_0x00081404(38000,local_e);
  return;
}



void __cdecl16far FUN_0009_21b9(void)

{
  uint *puVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  int *local_14;
  int local_10;
  undefined1 local_c;
  
  local_10 = func_0x000836aa();
  if (*(char *)0x623 == '\0') {
    uVar2 = func_0x0009470e(0x82c9,0x41);
    _local_14 = (int *)CONCAT22(unaff_DS,(int *)(*(int *)0x570 * 0x1c + 0x574));
    local_c = 0;
    if (((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 1) == 0) ||
       ((*(byte *)(*(int *)0x570 * 0x10 + 0x18c) & 0x40) != 0)) {
      if (local_10 < 0x400) {
        local_10 = local_10 << 2;
      }
      else {
        local_10 = 0xffc;
      }
    }
    if ((local_10 < 0) || (0xfff < local_10)) {
      local_c = 0x15;
    }
    else {
      puVar1 = (uint *)(*(int *)0x570 * 2 + 0x55a);
      *puVar1 = *puVar1 | 0x100;
      *_local_14 = local_10;
    }
    func_0x00092581(38000,0x100);
    func_0x0009470e(0x91ba,uVar2);
    *(undefined1 *)0x623 = local_c;
  }
  return;
}



void __cdecl16far FUN_0009_228a(void)

{
  uint *puVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  undefined2 uVar4;
  
  uVar4 = 0x41;
  uVar3 = 38000;
  uVar2 = func_0x0009470e();
  if (*(char *)(*(int *)0x570 + 0x569) != '\0') {
    puVar1 = (uint *)(*(int *)0x570 * 2 + 0x55a);
    *puVar1 = *puVar1 | 0x80;
    uVar3 = 0x91ba;
    func_0x00092581(38000,0x80,uVar4);
    *(undefined1 *)(*(int *)0x570 + 0x569) = 0;
  }
  func_0x0009470e(uVar3,uVar2);
  return;
}



void __cdecl16far FUN_0009_22d2(void)

{
  uint *puVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  undefined2 uVar4;
  
  uVar4 = 0x41;
  uVar3 = 38000;
  uVar2 = func_0x0009470e();
  if (*(char *)(*(int *)0x570 + 0x569) == '\0') {
    puVar1 = (uint *)(*(int *)0x570 * 2 + 0x55a);
    *puVar1 = *puVar1 | 0x80;
    uVar3 = 0x91ba;
    func_0x00092581(38000,0x80,uVar4);
    *(undefined1 *)(*(int *)0x570 + 0x569) = 1;
  }
  func_0x0009470e(uVar3,uVar2);
  return;
}



void __cdecl16far FUN_0009_231a(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2,*(undefined1 *)0x1b78);
  return;
}



void __cdecl16far FUN_0009_2366(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  uVar3 = 0x41;
  uVar2 = func_0x0009470e();
  iVar1 = *(int *)0x570;
  func_0x0009470e(38000,uVar2,uVar3);
  if ((((iVar1 == 1) || (iVar1 == 2)) || (iVar1 == 3)) || (iVar1 == 4)) {
    func_0x00082d9b(38000,(int)*(undefined4 *)0x6f2 + 0x17b,
                    (int)((ulong)*(undefined4 *)0x6f2 >> 0x10));
    func_0x00083241(0x82c9,iVar1 + 0x30);
  }
  return;
}



void __cdecl16far FUN_0009_23d3(void)

{
  uint *puVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  undefined2 uVar4;
  
  uVar4 = 0x41;
  uVar3 = func_0x0009470e();
  iVar2 = *(int *)0x570;
  *(undefined2 *)0x570 = 0;
  puVar1 = (uint *)(*(int *)0x570 * 2 + 0x55a);
  *puVar1 = *puVar1 | 0x800;
  puVar1 = (uint *)(iVar2 * 2 + 0x55a);
  *puVar1 = *puVar1 | 0x800;
  *(uint *)0x548 = *(uint *)0x548 | 0x467f;
  func_0x00092581(38000,0x800,uVar4,iVar2,uVar3);
  func_0x0009470e(0x91ba,uVar3);
  return;
}



void __cdecl16far FUN_0009_2426(void)

{
  uint *puVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  undefined2 local_10;
  undefined1 local_c;
  
  local_10 = *(int *)0x600;
  uVar5 = 0x41;
  uVar4 = 38000;
  uVar3 = func_0x0009470e();
  if ((local_10 == 4) && ((*(byte *)0x19c & 0x41) == 1)) {
    local_10 = 1;
  }
  local_c = 0;
  if (((local_10 < 1) || (3 < local_10)) ||
     (((*(byte *)(local_10 * 2 + 0x1be2) | *(byte *)(local_10 * 2 + 0x1be3)) & 1) == 0)) {
    local_c = 0x1b;
  }
  else {
    iVar2 = *(int *)0x570;
    *(int *)0x570 = local_10;
    puVar1 = (uint *)(*(int *)0x570 * 2 + 0x55a);
    *puVar1 = *puVar1 | 0x800;
    puVar1 = (uint *)(iVar2 * 2 + 0x55a);
    *puVar1 = *puVar1 | 0x800;
    *(uint *)0x548 = *(uint *)0x548 | 0x467f;
    uVar4 = 0x91ba;
    func_0x00092581(38000,0x800,uVar5);
  }
  func_0x0009470e(uVar4,uVar3);
  *(undefined1 *)0x623 = local_c;
  return;
}



/* build_comm_status_message (confidence: Mechanism confirmed - see
   `docs/decode-anomalies/dual-entry-points.md`'s `SUB_EAC86` entry for the new calling-convention
   evidence this surfaced)
   
   Evidence: `(index 0-3)` - picks one of 4 message chunks by index, appends it onto a base template
   via `SUB_EAC86(dest, src)`, terminates with `0xFF`
   
   (physical 0x0924D2, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __stdcall16far build_comm_status_message(word index)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  undefined2 local_c;
  
  uVar3 = 0x8f80;
  uVar1 = func_0x0009470e();
  if (index == 0) {
    _local_c = CONCAT22(unaff_DS,0x693);
  }
  else if (index == 1) {
    _local_c = CONCAT22(unaff_DS,0x69d);
  }
  else if (index == 2) {
    _local_c = CONCAT22(unaff_DS,0x6a7);
  }
  else if (index == 3) {
    _local_c = CONCAT22(unaff_DS,0x6b1);
  }
  *(undefined1 *)0x6bb = 1;
  iVar5 = 0;
  do {
    iVar4 = iVar5;
    iVar5 = iVar4 + 1;
  } while (*(char *)(iVar4 + 0x65e) != '\0');
  *(undefined1 *)(iVar4 + 0x65f) = 0xff;
  uVar2 = (undefined2)((ulong)_local_c >> 0x10);
  func_0x000eac86(38000,0x65e,unaff_DS,(int)_local_c,uVar2,uVar3,iVar5,uVar1);
  *(undefined1 *)(iVar5 + (int)_local_c) = 0xff;
  *(undefined1 *)0x6bb = 0;
  func_0x0009470e(0xea34,uVar1);
  return;
}



void __stdcall16far FUN_0009_2581(uint param_1)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (*(char *)0x1b78 != '\0') {
    while (((*(uint *)(*(int *)0x570 * 2 + 0x55a) | *(uint *)0x3ae) & param_1) != 0) {
      func_0x000e693c(unaff_CS);
      unaff_CS = 0xe693;
    }
  }
  return;
}



void __cdecl16far FUN_0009_2609(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x600 = *(undefined2 *)0x4fa;
  return;
}



void __cdecl16far FUN_0009_2621(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x5fe == '\x0e') {
    *(uint *)0x4f8 = *(uint *)0x4f8 & 0xfffe;
  }
  else if (*(char *)0x5fe == '\x0f') {
    *(uint *)0x4f8 = *(uint *)0x4f8 | 1;
  }
  else {
    *(undefined1 *)0x623 = 0x15;
  }
  return;
}



void __cdecl16far FUN_0009_2650(void)

{
  return;
}



int __cdecl16far FUN_0009_2676(void)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_DS;
  undefined1 local_5d;
  
  uVar1 = *(undefined2 *)0x4f8;
  iVar2 = func_0x00086b4b();
  if (iVar2 == 0) {
    if ((local_5d & 8) == 0) {
      iVar2 = func_0x0008373a(0x85eb,0);
      if ((iVar2 < 0) || (*(char *)0x57e != '\0')) {
        *(undefined1 *)0x623 = 0x15;
      }
      else {
        func_0x0009470e(0x82c9,0x41);
        iVar3 = func_0x000f3495(38000,uVar1,iVar2);
        iVar2 = func_0x0009470e(0xf313,0x8f80);
        if (iVar3 != 0) {
          *(undefined1 *)0x623 = 0x1b;
        }
        *(byte *)0x603 = *(byte *)0x603 | 1;
      }
    }
    else {
      *(undefined1 *)0x623 = 0x1b;
      iVar2 = 0;
    }
  }
  else {
    *(undefined1 *)0x623 = 0x1b;
    iVar2 = 0;
  }
  return iVar2;
}



void __cdecl16far FUN_0009_2719(void)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined1 local_3f;
  undefined1 local_35;
  
  iVar1 = func_0x00086b4b();
  if (iVar1 == 0) {
    if ((*(uint *)0x4f8 & 1) == 0) {
      local_35 = local_3f;
    }
    func_0x00081470(0x85eb,local_35,9);
  }
  else {
    *(undefined1 *)0x623 = 0x1b;
  }
  return;
}



void __cdecl16far FUN_0009_277e(void)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined1 local_40;
  undefined1 local_36;
  
  iVar1 = func_0x00086b4b();
  if (iVar1 == 0) {
    if ((*(uint *)0x4f8 & 1) == 0) {
      local_36 = local_40;
    }
    func_0x00081470(0x85eb,local_36,9);
  }
  else {
    *(undefined1 *)0x623 = 0x1b;
  }
  return;
}



void __cdecl16far FUN_0009_27e3(void)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined1 local_55;
  
  iVar1 = *(int *)0x4f8;
  iVar2 = func_0x00086b4b();
  if (((local_55 & 8) == 0) && (iVar2 == 0)) {
    func_0x000f3660(0x85eb,iVar1 >> 1,10);
    *(byte *)0x603 = *(byte *)0x603 | 1;
  }
  else {
    *(undefined1 *)0x623 = 0x1b;
  }
  return;
}



void __cdecl16far FUN_0009_2832(void)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined1 local_55;
  
  iVar1 = *(int *)0x4f8;
  iVar2 = func_0x00086b4b();
  if (((local_55 & 8) == 0) && (iVar2 == 0)) {
    func_0x000f3660(0x85eb,iVar1 >> 1,1);
    *(byte *)0x603 = *(byte *)0x603 | 1;
  }
  else {
    *(undefined1 *)0x623 = 0x1b;
  }
  return;
}



void __cdecl16far FUN_0009_2881(void)

{
  int iVar1;
  undefined2 unaff_DS;
  
  iVar1 = func_0x00086b4b();
  if (iVar1 != 0) {
    *(undefined1 *)0x623 = 0x1b;
  }
  return;
}



void __cdecl16far FUN_0009_28d3(void)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 unaff_DS;
  int local_68;
  uint local_66;
  int local_62;
  undefined1 local_5c [18];
  int local_4a;
  byte local_46;
  byte local_45;
  int local_40;
  byte local_3c;
  byte local_3b;
  undefined2 local_e;
  int local_c;
  uint local_a;
  
  iVar2 = *(int *)0x4f8;
  local_a = *(uint *)0x4f8 & 1;
  uVar1 = *(undefined2 *)0x4f8;
  local_62 = func_0x0008380b();
  if (local_62 < 0x2711) {
    iVar2 = func_0x00086b4b(0x82c9,iVar2 >> 1,local_5c);
    if (iVar2 == 0) {
      if (local_a == 0) {
        local_c = local_4a;
        local_66 = (uint)local_46;
        local_68 = local_66 - local_45;
      }
      else {
        local_c = local_40;
        local_66 = (uint)local_3c;
        local_68 = local_66 - local_3b;
      }
      if (local_68 == 1) {
        if (local_66 % 3 == 1) {
          local_62 = (local_62 << 1) / 5;
        }
        else {
          local_62 = local_62 / 2;
        }
      }
      else if (local_68 == 2) {
        if (local_66 % 3 == 1) {
          local_62 = local_62 / 5;
        }
        else {
          local_62 = local_62 / 4;
        }
      }
      else if (local_68 == 3) {
        local_62 = local_62 / 10;
      }
      if ((local_62 < 0x100) && (-0x100 < local_62)) {
        local_c = local_c - local_62;
        local_e = func_0x0009470e(0x85eb,0x41);
        func_0x000f32ec(38000,uVar1,local_c);
        func_0x0009470e(0xf313,0x8f80);
        *(byte *)0x603 = *(byte *)0x603 | 1;
      }
      else {
        *(undefined1 *)0x623 = 0x15;
      }
    }
    else {
      *(undefined1 *)0x623 = 0x1b;
    }
  }
  else {
    *(undefined1 *)0x623 = 0x15;
  }
  return;
}



void __cdecl16far FUN_0009_2a2f(void)

{
  char *pcVar1;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  int local_72;
  uint local_70;
  char local_6e [10];
  uint local_64;
  int local_62;
  int local_60;
  uint local_5e;
  int local_5c;
  int local_5a;
  int local_58;
  int local_44;
  int local_42;
  byte local_40;
  byte local_3f;
  int local_3a;
  int local_38;
  byte local_36;
  byte local_35;
  
  local_5a = *(int *)0x4f8 >> 1;
  local_5e = *(uint *)0x4f8 & 1;
  local_58 = func_0x00086b4b();
  if (local_58 == 0) {
    if (local_5e == 0) {
      local_5c = local_42 - local_44;
      local_70 = (uint)local_40;
      local_72 = local_70 - local_3f;
    }
    else {
      local_5c = local_38 - local_3a;
      local_70 = (uint)local_36;
      local_72 = local_70 - local_35;
    }
    if (local_72 == 1) {
      if (local_70 % 3 == 1) {
        local_5c = (local_5c * 5) / 2;
      }
      else {
        local_5c = local_5c << 1;
      }
    }
    else if (local_72 == 2) {
      if (local_70 % 3 == 1) {
        local_5c = local_5c * 5;
      }
      else {
        local_5c = local_5c << 2;
      }
    }
    else if (local_72 == 3) {
      local_5c = local_5c * 10;
    }
    if (local_5c < 1) {
      local_5c = -local_5c;
      local_6e[0] = '+';
    }
    else {
      local_6e[0] = '-';
    }
    local_60 = local_5c / 0x19;
    local_62 = local_5c % 0x19 << 2;
    local_64 = 1;
    if (99 < local_60) {
      local_6e[1] = (char)(local_60 / 100) + '0';
      local_64 = 2;
      local_60 = local_60 % 100;
    }
    if ((9 < local_60) || (1 < local_64)) {
      pcVar1 = local_6e + local_64;
      local_64 = local_64 + 1;
      *pcVar1 = (char)(local_60 / 10) + '0';
    }
    pcVar1 = local_6e + local_64;
    local_64 = local_64 + 1;
    *pcVar1 = (char)(local_60 % 10) + '0';
    pcVar1 = local_6e + local_64;
    local_64 = local_64 + 1;
    *pcVar1 = '.';
    pcVar1 = local_6e + local_64;
    local_64 = local_64 + 1;
    *pcVar1 = (char)(local_62 / 10) + '0';
    pcVar1 = local_6e + local_64;
    local_64 = local_64 + 1;
    *pcVar1 = (char)(local_62 % 10) + '0';
    pcVar1 = local_6e + local_64;
    local_64 = local_64 + 1;
    *pcVar1 = '\0';
    func_0x00082d01(0x85eb,local_6e);
  }
  else {
    *(undefined1 *)0x623 = 0x1b;
  }
  return;
}



void __cdecl16far FUN_0009_2c17(void)

{
  int iVar1;
  undefined2 unaff_DS;
  
  iVar1 = func_0x00086b4b();
  if (iVar1 != 0) {
    *(undefined1 *)0x623 = 0x1b;
  }
  return;
}



void __cdecl16far FUN_0009_2cf2(void)

{
  func_0x00092d17();
  return;
}



void FUN_0009_2d01(void)

{
  return;
}



void __cdecl16far FUN_0009_2d05(void)

{
  func_0x00092d17();
  return;
}



void __stdcall16far FUN_0009_2d17(int param_1)

{
  int iVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 uVar2;
  byte local_5a;
  int local_c;
  
  if (*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x14') {
    *(undefined1 *)0x623 = 0x1e;
    return;
  }
  if (*(int *)0x5f8 == 0) {
    if (*(char *)0x5ff == '\0') {
      *(undefined1 *)0x623 = 7;
      return;
    }
    *(undefined1 *)0x623 = 0x15;
    return;
  }
  local_c = 1;
  iVar1 = *(int *)0x5f8;
  if (iVar1 != 1) {
    if ((iVar1 == 2) || (iVar1 == 3)) {
      iVar1 = *(int *)0x5f8;
      if (((*(byte *)(iVar1 * 2 + (int)*(undefined4 *)0x732 + 0x7e) |
           *(byte *)(iVar1 * 2 + (int)*(undefined4 *)0x732 + 0x7f)) & 2) == 0) {
        *(undefined1 *)0x623 = 0x1b;
      }
      else {
        if (param_1 == 0) {
          uVar2 = 0x41;
          func_0x0009470e();
          func_0x000f0414(38000,iVar1,uVar2);
          func_0x0009470e(0xf008,0x8f80);
        }
        else {
          uVar2 = 0x41;
          func_0x0009470e();
          func_0x000f8e66(38000,iVar1,uVar2);
          func_0x0009470e(0xf8e6,0x8f80);
        }
        unaff_CS = 38000;
      }
      goto LAB_0009_2eab;
    }
    if (iVar1 != 4) {
      *(undefined1 *)0x623 = 0x15;
      goto LAB_0009_2eab;
    }
    local_c = 4;
  }
  unaff_CS = 0x85eb;
  iVar1 = func_0x00086b4b();
  if (iVar1 != 0) {
    *(undefined1 *)0x623 = 0x1b;
    return;
  }
  if (((local_c == 4) && ((local_5a & 1) != 0)) || ((local_c == 1 && ((local_5a & 1) == 0)))) {
    if (param_1 == 0) {
      func_0x0009470e(0x85eb,0x41);
      func_0x000f0414(38000,1);
      unaff_CS = 38000;
      func_0x0009470e(0xf008,0x8f80);
    }
    else {
      func_0x0009470e(0x85eb,0x41);
      func_0x000f8e66(38000,1);
      unaff_CS = 38000;
      func_0x0009470e(0xf8e6,0x8f80);
    }
  }
  else {
    *(undefined1 *)0x623 = 0x22;
  }
LAB_0009_2eab:
  *(uint *)*(undefined4 *)0x736 = *(uint *)*(undefined4 *)0x736 | 0x200;
  func_0x0009470e(unaff_CS,0x41);
  *(uint *)&DAT_0009_04ca = *(uint *)&DAT_0009_04ca | 0x200;
  func_0x0009470e(38000,0x8f80);
  return;
}



void __cdecl16far FUN_0009_2ed2(void)

{
  undefined1 uVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  if (*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x14') {
    *(undefined1 *)0x623 = 0x1e;
  }
  else if ((*(int *)0x5f8 < 1) || (3 < *(int *)0x5f8)) {
    if (*(int *)0x5f8 == 4) {
      func_0x00092f8a();
    }
    else if ((*(int *)0x5f8 < 5) || (0x1e < *(int *)0x5f8)) {
      if (*(char *)0x5ff == '\0') {
        *(undefined1 *)0x623 = 7;
      }
      else {
        *(undefined1 *)0x623 = 0x15;
      }
    }
    else {
      uVar3 = *(undefined2 *)0x5f8;
      uVar1 = func_0x00093b1c();
      iVar2 = func_0x00094ef5(0x92cf,uVar1,uVar3);
      if (iVar2 == 0) {
        *(byte *)0x603 = *(byte *)0x603 | 1;
      }
      else {
        *(undefined1 *)0x623 = 0x1b;
      }
    }
  }
  else {
    func_0x00092f8a();
  }
  return;
}



void FUN_0009_2f14(void)

{
  undefined1 uVar1;
  int iVar2;
  int unaff_BP;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  if (*(int *)0x5f8 == 4) {
    *(undefined2 *)(unaff_BP + -8) = 1;
    *(undefined2 *)(unaff_BP + -0xc) = 4;
    func_0x00092f8a();
  }
  else if ((*(int *)0x5f8 < 5) || (0x1e < *(int *)0x5f8)) {
    if (*(char *)0x5ff == '\0') {
      *(undefined1 *)0x623 = 7;
    }
    else {
      *(undefined1 *)0x623 = 0x15;
    }
  }
  else {
    uVar3 = *(undefined2 *)0x5f8;
    uVar1 = func_0x00093b1c();
    *(undefined1 *)(unaff_BP + -9) = uVar1;
    iVar2 = func_0x00094ef5(0x92cf,uVar1,uVar3);
    *(int *)(unaff_BP + -0xe) = iVar2;
    if (iVar2 == 0) {
      *(byte *)0x603 = *(byte *)0x603 | 1;
    }
    else {
      *(undefined1 *)0x623 = 0x1b;
    }
  }
  return;
}



void __stdcall16far FUN_0009_2f8a(int param_1,int param_2)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  byte local_56;
  
  if ((param_1 == 2) || (param_1 == 3)) {
    uVar2 = 0x41;
    uVar1 = func_0x0009470e();
    func_0x000f0414(38000,param_1,uVar2);
    func_0x000f8f24(0xf008,param_1);
    func_0x0009470e(0xf8e6,uVar1);
  }
  else {
    func_0x00086b4b();
    if (((param_2 != 4) || ((local_56 & 1) == 0)) && ((param_2 != 1 || ((local_56 & 1) != 0)))) {
      *(undefined1 *)0x623 = 0x22;
      return;
    }
    uVar1 = func_0x0009470e(0x85eb,0x41);
    func_0x000f0414(38000,param_1);
    func_0x000f8f24(0xf008,param_1);
    func_0x0009470e(0xf8e6,uVar1);
  }
  *(uint *)*(undefined4 *)0x736 = *(uint *)*(undefined4 *)0x736 | 0x200;
  func_0x0009470e(38000,0x41);
  *(uint *)&DAT_0009_04ca = *(uint *)&DAT_0009_04ca | 0x200;
  func_0x0009470e(38000,0x8f80);
  return;
}



void __cdecl16far FUN_0009_3051(void)

{
  int iVar1;
  undefined1 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  if (*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x1e') {
    if (*(char *)0x5f7 == '\0') {
      *(undefined2 *)0x5f8 = 1;
    }
  }
  else {
    if (*(char *)0x5f7 == '\0') {
      *(undefined2 *)0x5f8 = 4;
    }
    if (*(int *)0x5f8 != 4) {
      *(undefined1 *)0x623 = 0x15;
      return;
    }
  }
  if (0x1e < *(int *)0x5f8) {
    *(undefined1 *)0x623 = 0x15;
    return;
  }
  iVar1 = *(int *)0x5f8;
  if (iVar1 != 1) {
    if (iVar1 == 2) {
      return;
    }
    if (iVar1 == 3) {
      return;
    }
    if (iVar1 != 4) {
      uVar3 = *(undefined2 *)0x5f8;
      uVar2 = func_0x00093b1c();
      func_0x00095067(0x92cf,uVar2,uVar3);
      return;
    }
  }
  func_0x00086b4b();
  return;
}



void __cdecl16far FUN_0009_31e8(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x4f6 = 0;
  *(byte *)0x603 = *(byte *)0x603 | 1;
  return;
}



void __cdecl16far FUN_0009_31fc(void)

{
  undefined2 unaff_DS;
  
  if ((*(int *)0x5f8 < 0x1f) && (0 < *(int *)0x5f8)) {
    *(undefined1 *)0x4f6 = *(undefined1 *)0x5f8;
  }
  else {
    *(undefined1 *)0x623 = 0x15;
  }
  *(byte *)0x603 = *(byte *)0x603 | 1;
  return;
}



void __cdecl16far FUN_0009_3227(void)

{
  undefined2 unaff_DS;
  
  *(uint *)0x5f8 = (uint)*(byte *)0x4f6;
  return;
}



void __cdecl16far FUN_0009_3252(void)

{
  byte *pbVar1;
  undefined1 uVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  
  if ((*(int *)0x5f8 < 0x1f) && (4 < *(int *)0x5f8)) {
    uVar5 = *(undefined2 *)0x5f8;
    uVar2 = func_0x00093b1c();
    iVar3 = func_0x00093b7e(0x92cf,uVar2,uVar5);
    iVar4 = func_0x00096126(0x92cf);
    if (iVar4 == 0) {
      pbVar1 = (byte *)(iVar3 * 0xc + (int)*(undefined4 *)0x73e + 10);
      *pbVar1 = *pbVar1 & 0xbb;
      *(undefined2 *)*(undefined4 *)0x746 = 0;
      *(byte *)0x603 = *(byte *)0x603 | 1;
    }
    else {
      *(undefined1 *)0x623 = 0x21;
    }
  }
  else {
    *(undefined1 *)0x623 = 0x15;
  }
  return;
}



void __cdecl16far FUN_0009_32bc(void)

{
  byte *pbVar1;
  undefined1 uVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  
  if ((*(int *)0x5f8 < 0x1f) && (4 < *(int *)0x5f8)) {
    uVar5 = *(undefined2 *)0x5f8;
    uVar2 = func_0x00093b1c();
    iVar3 = func_0x00093b7e(0x92cf,uVar2,uVar5);
    iVar4 = func_0x00096126(0x92cf);
    if (iVar4 == 0) {
      pbVar1 = (byte *)(iVar3 * 0xc + (int)*(undefined4 *)0x73e + 10);
      *pbVar1 = *pbVar1 & 0xbf;
      pbVar1 = (byte *)(iVar3 * 0xc + (int)*(undefined4 *)0x73e + 10);
      *pbVar1 = *pbVar1 | 4;
      *(undefined2 *)*(undefined4 *)0x746 = 0;
      *(byte *)0x603 = *(byte *)0x603 | 1;
    }
    else {
      *(undefined1 *)0x623 = 0x21;
    }
  }
  else {
    *(undefined1 *)0x623 = 0x15;
  }
  return;
}



void __cdecl16far FUN_0009_3339(void)

{
  byte *pbVar1;
  undefined1 uVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  
  if ((*(int *)0x5f8 < 0x1f) && (4 < *(int *)0x5f8)) {
    uVar5 = *(undefined2 *)0x5f8;
    uVar2 = func_0x00093b1c();
    iVar3 = func_0x00093b7e(0x92cf,uVar2,uVar5);
    iVar4 = func_0x00096126(0x92cf);
    if (iVar4 == 0) {
      pbVar1 = (byte *)(iVar3 * 0xc + (int)*(undefined4 *)0x73e + 10);
      *pbVar1 = *pbVar1 | 0x44;
      *(undefined2 *)*(undefined4 *)0x746 = 0;
      *(byte *)0x603 = *(byte *)0x603 | 1;
    }
    else {
      *(undefined1 *)0x623 = 0x21;
    }
  }
  else {
    *(undefined1 *)0x623 = 0x15;
  }
  return;
}



void __cdecl16far FUN_0009_33a3(void)

{
  undefined1 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  if ((*(int *)0x5f8 < 0x1f) && (0 < *(int *)0x5f8)) {
    if ((*(int *)0x5f8 < 1) || (4 < *(int *)0x5f8)) {
      uVar2 = *(undefined2 *)0x5f8;
      uVar1 = func_0x00093b1c();
      func_0x00093b7e(0x92cf,uVar1,uVar2);
    }
  }
  else {
    *(undefined1 *)0x623 = 0x15;
  }
  return;
}



void __cdecl16far FUN_0009_3440(void)

{
  int iVar1;
  undefined2 unaff_SS;
  byte local_78 [58];
  int local_3e;
  int local_38;
  int local_2a;
  char local_28 [30];
  undefined1 local_a;
  
  iVar1 = func_0x00086b4b();
  if (iVar1 == 0) {
    if ((local_78[0] & 1) == 0) {
      local_28[0] = (char)(local_3e + local_38 >> 10) + '0';
      local_28[3] = '0';
    }
    else {
      local_28[3] = (char)(local_3e + local_38 >> 10) + '0';
      local_28[0] = '0';
    }
  }
  else {
    local_28[3] = '0';
    local_28[0] = '0';
  }
  for (local_2a = 1; local_2a < 3; local_2a = local_2a + 1) {
    iVar1 = func_0x00086b4b(0x85eb,local_2a + 1,local_78);
    if (iVar1 == 0) {
      local_28[local_2a] = (char)(local_3e + local_38 >> 10) + '0';
    }
    else {
      local_28[local_2a] = '0';
    }
  }
  func_0x000960ab(0x85eb,local_28 + 4);
  local_a = 0;
  func_0x00083241(0x9471,0x22);
  func_0x00082d01(0x82c9,local_28);
  func_0x00083241(0x82c9,0x22);
  return;
}



void __cdecl16far FUN_0009_3568(void)

{
  uint uVar1;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  int local_2c;
  char local_29;
  undefined1 local_28 [30];
  undefined1 local_a;
  
  for (local_2c = 0; local_2c < 4; local_2c = local_2c + 1) {
    local_28[local_2c] = 0x55;
  }
  local_29 = 'a';
  for (local_2c = 4; local_2c < 0x1e; local_2c = local_2c + 1) {
    uVar1 = func_0x00095067(unaff_CS,local_29);
    if ((uVar1 & 0x44) == 0) {
      local_28[local_2c] = 0x55;
    }
    else if ((uVar1 & 0x44) == 4) {
      local_28[local_2c] = 0x4c;
    }
    else {
      local_28[local_2c] = 0x50;
    }
    local_29 = local_29 + '\x01';
    unaff_CS = 0x9471;
  }
  local_a = 0;
  func_0x00083241(unaff_CS,0x22);
  func_0x00082d01(0x82c9,local_28);
  func_0x00083241(0x82c9,0x22);
  return;
}



void __cdecl16far FUN_0009_3600(void)

{
  func_0x00081404();
  return;
}



void __cdecl16far FUN_0009_3622(void)

{
  byte bVar1;
  uint uVar2;
  byte *in_BX;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  int local_20;
  int local_1c;
  int local_1a;
  undefined4 local_18;
  int local_14;
  undefined1 local_11;
  int local_10;
  int local_e;
  uint local_c;
  undefined1 local_9;
  
  iVar3 = (int)((ulong)*(undefined4 *)0x732 >> 0x10);
  if (*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x1e') {
    if (((*(char *)0x5f7 != '\x04') || (0x1e < *(int *)0x5f8)) || (*(int *)0x5f8 < 1)) {
      if (*(char *)0x5f7 == '\0') {
        *(undefined1 *)0x623 = 7;
        return;
      }
      *(undefined1 *)0x623 = 0x15;
      return;
    }
  }
  else {
    if (((*(char *)0x5f7 != '\0') && (*(int *)0x5f8 != 4)) &&
       ((*(int *)0x5f8 != 1 && (*(int *)0x5f8 != 0)))) {
      *(undefined1 *)0x623 = 0x15;
      return;
    }
    *(undefined2 *)0x5f8 = 4;
    *(undefined1 *)0x4f6 = 0;
  }
  if (((*(char *)0x4f6 == '\x01') && (*(int *)0x5f8 == 4)) ||
     ((*(char *)0x4f6 == '\x04' && (*(int *)0x5f8 == 1)))) {
    *(undefined1 *)0x623 = 0x15;
  }
  else {
    uVar2 = (uint)*(byte *)0x4f6;
    local_9 = func_0x00093b1c();
    local_11 = func_0x00093b1c(0x92cf,*(undefined2 *)0x5f8,uVar2);
    if (*(byte *)0x4f6 < 4) {
      local_c = (uint)*(byte *)0x4f6;
    }
    else if (*(char *)0x4f6 == '\x04') {
      local_c = 1;
    }
    else {
      local_c = 0;
    }
    if (*(int *)0x5f8 < 4) {
      local_14 = *(int *)0x5f8;
    }
    else if (*(int *)0x5f8 == 4) {
      local_14 = 1;
    }
    else {
      local_14 = 0;
    }
    if ((*(char *)0x4f6 == '\x01') || (*(int *)0x5f8 == 1)) {
      local_20 = 1;
    }
    else if ((*(char *)0x4f6 == '\x04') || (*(int *)0x5f8 == 4)) {
      local_20 = 4;
    }
    else {
      local_20 = 0;
    }
    *(byte *)0x603 = *(byte *)0x603 | 1;
    if (*(char *)0x4f6 == '\0') {
      if ((*(int *)0x5f8 < 1) || (4 < *(int *)0x5f8)) {
        *(undefined1 *)0x623 = 0x15;
      }
      else {
        iVar3 = func_0x000f0c2a(0x92cf,3);
        if (iVar3 == 0) {
          func_0x00093925(0xf0c2,local_20,local_14);
          func_0x000f0c81(0x92cf,3);
        }
        else {
          *(undefined1 *)0x623 = 0x1b;
        }
      }
    }
    else {
      if (local_c == 0) {
        func_0x0009471c(0x92cf,local_9,&local_1c);
        local_18 = (byte *)CONCAT22(iVar3,in_BX);
        if ((iVar3 == 0 && in_BX == (byte *)0x0) || (local_1c != 0)) {
          *(undefined1 *)0x623 = 0x1b;
          return;
        }
        if ((*local_18 & 1) == 0) {
          local_1a = 1;
        }
        else {
          local_1a = 4;
        }
        uVar4 = 0x9471;
        func_0x00094d9c(0x9471,local_9);
      }
      else {
        local_e = local_c * 2;
        local_10 = local_e + 1;
        if (((*(byte *)((int)*(undefined4 *)0x732 + local_e + 0x7e) |
             *(byte *)((int)*(undefined4 *)0x732 + local_10 + 0x7e)) & 2) == 0) {
          *(undefined1 *)0x623 = 0x1b;
          return;
        }
        func_0x0009470e(0x92cf,0x41);
        bVar1 = *(byte *)(local_c * 0x10 + 0x18c);
        uVar4 = 38000;
        func_0x0009470e(38000,0x8f80);
        if ((bVar1 & 1) == 0) {
          local_1a = 1;
        }
        else {
          local_1a = 4;
        }
      }
      if (local_20 == 0) {
        local_1c = func_0x000f0c2a(uVar4,3);
        if (local_1c == 0) {
          func_0x00087609(0xf0c2,local_9,local_11,&local_1c);
          func_0x000f0c81(0x85eb,3);
        }
      }
      else {
        if (((local_1a != 4) || (local_20 != 4)) && ((local_1a != 1 || (local_20 != 1)))) {
          *(undefined1 *)0x623 = 0x1b;
          return;
        }
        local_1c = func_0x000f0c2a(uVar4,3);
        if (local_1c == 0) {
          func_0x00087609(0xf0c2,local_9,local_11,&local_1c);
          func_0x000f0c81(0x85eb,3);
        }
      }
      if (local_1c == 0) {
        if (local_14 != 0) {
          *(uint *)*(undefined4 *)0x736 = *(uint *)*(undefined4 *)0x736 | 0x200;
        }
      }
      else {
        *(undefined1 *)0x623 = 0x1b;
      }
    }
  }
  return;
}



void __stdcall16far FUN_0009_3925(int param_1,int param_2)

{
  code cVar1;
  byte bVar2;
  int iVar3;
  undefined2 unaff_DS;
  undefined2 uVar4;
  
  uVar4 = 0x41;
  func_0x0009470e();
  cVar1 = *(code *)FUN_0009_00fc;
  func_0x0009470e(38000,0x8f80,uVar4);
  if ((((param_1 == 4) && (((byte)cVar1 & 1) != 0)) || (param_1 == 1)) || (param_1 == 0)) {
    if ((param_1 == 1) && (param_2 == 1)) {
      func_0x0009470e(38000,0x41);
      func_0x000f706e(38000,0,1,1);
      func_0x0009470e(0xf706,0x8f80);
      *(uint *)*(undefined4 *)0x736 = *(uint *)*(undefined4 *)0x736 | 0x200;
    }
    else if ((param_1 == 4) && (param_2 == 1)) {
      func_0x0009470e(38000,0x41);
      func_0x000f0414(38000,2);
      func_0x000f0414(0xf008,3);
      func_0x000f8f24(0xf008,2);
      func_0x000f8f24(0xf8e6,3);
      uVar4 = 0x97c6;
      func_0x00097c95(0xf8e6,1);
      while( true ) {
        iVar3 = func_0x00097c6e(uVar4,1);
        if (iVar3 == 0) break;
        uVar4 = 0xe693;
        func_0x000e693c(0x97c6);
      }
      func_0x000f706e(0x97c6,0,1,4);
      func_0x0009470e(0xf706,0x8f80);
      *(uint *)*(undefined4 *)0x736 = *(uint *)*(undefined4 *)0x736 | 0x200;
    }
    else {
      if (((*(byte *)((int)*(undefined4 *)0x732 + 0x80) & 2) != 0) ||
         ((*(byte *)((int)*(undefined4 *)0x732 + 0x81) & 2) != 0)) {
        func_0x0009470e(38000,0x41);
        bVar2 = *(byte *)0x19c;
        func_0x0009470e(38000,0x8f80);
        if ((bVar2 & 1) != 0) {
          func_0x0009470e(38000,0x41);
          func_0x000f0414(38000,1);
          func_0x000f8f24(0xf008,1);
          uVar4 = 0x97c6;
          func_0x00097c95(0xf8e6,1);
          while( true ) {
            iVar3 = func_0x00097c6e(uVar4,1);
            if (iVar3 == 0) break;
            uVar4 = 0xe693;
            func_0x000e693c(0x97c6);
          }
          func_0x0009470e(0x97c6,0x8f80);
        }
      }
      func_0x0009470e(38000,0x41);
      func_0x000f706e(38000,0,param_2,1);
      func_0x0009470e(0xf706,0x8f80);
      *(uint *)*(undefined4 *)0x736 = *(uint *)*(undefined4 *)0x736 | 0x200;
    }
  }
  else {
    *(undefined1 *)0x623 = 0x1b;
  }
  return;
}



char __stdcall16far FUN_0009_3b1c(int param_1)

{
  undefined1 local_9;
  
  if (param_1 == 0) {
    local_9 = '\0';
  }
  else if (param_1 == 1) {
    local_9 = '\x01';
  }
  else if (param_1 == 2) {
    local_9 = '\x02';
  }
  else if (param_1 == 3) {
    local_9 = '\x03';
  }
  else if (param_1 == 4) {
    local_9 = '\x01';
  }
  else {
    local_9 = (char)param_1 + '\\';
  }
  return local_9;
}



int __stdcall16far FUN_0009_3b7e(byte param_1)

{
  undefined2 local_a;
  
  if ((param_1 < 0x61) || (0x7a < param_1)) {
    local_a = -1;
  }
  else {
    local_a = param_1 - 0x61;
  }
  return local_a;
}



uint __stdcall16far FUN_0009_3bac(int param_1)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  undefined2 local_e;
  
  uVar2 = 0x8f80;
  uVar1 = func_0x0009470e();
  local_e = (uint)*(byte *)0x4f4 + param_1;
  if ((int)local_e < 0) {
    local_e = 4;
  }
  else if (4 < (int)local_e) {
    local_e = 0;
  }
  if (*(byte *)0x4f4 != local_e) {
    *(undefined1 *)0x4f4 = (char)local_e;
  }
  func_0x0009470e(38000,uVar1,uVar2);
  return local_e;
}



undefined2 __stdcall16far FUN_0009_3c07(int param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 local_e;
  
  uVar3 = 0x8f80;
  uVar2 = 38000;
  uVar1 = func_0x0009470e();
  if ((*(byte *)0x59a & 1) == 0) {
    local_e = 1;
  }
  else {
    local_e = 2;
  }
  if (param_1 != 0) {
    *(int *)0x5f8 = 3 - local_e;
    uVar2 = 0x856c;
    func_0x00085b2b(38000,uVar3);
  }
  if ((*(byte *)0x59a & 1) == 0) {
    local_e = 1;
  }
  else {
    local_e = 2;
  }
  func_0x0009470e(uVar2,uVar1);
  return local_e;
}



bool __stdcall16far FUN_0009_3c6e(int param_1)

{
  char cVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  uVar3 = 0x8f80;
  uVar2 = func_0x0009470e();
  if (param_1 != 0) {
    if (*(char *)0x45b == '\0') {
      *(undefined1 *)0x45b = 0xff;
    }
    else {
      *(undefined1 *)0x45b = 0;
    }
  }
  cVar1 = *(char *)0x45b;
  func_0x0009470e(38000,uVar2,uVar3);
  return cVar1 != '\0';
}



void __cdecl16far FUN_0009_3cc2(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0009_3cdb(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0009_3cf5(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0009_3d0e(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0009_3d28(void)

{
  int iVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  bool bVar2;
  undefined1 uVar3;
  undefined4 uVar4;
  int local_c;
  
  if (*(char *)0x5ff == '\0') {
    *(undefined2 *)((int)*(undefined4 *)0x732 + 0x8c) = 0;
  }
  else {
    if (*(char *)0x57e != '\0') {
      *(undefined1 *)0x623 = 0x15;
      return;
    }
    while( true ) {
      bVar2 = false;
      func_0x000e7753(unaff_CS,0x7ff,0);
      if ((!bVar2) && (-1 < *(int *)0x57c)) break;
      *(int *)0x57c = *(int *)0x57c + 1;
      uVar3 = 1;
      uVar4 = func_0x000e783d(0xe772,10,0,0,0);
      *(undefined2 *)0x578 = (int)uVar4;
      *(undefined2 *)0x57a = (int)((ulong)uVar4 >> 0x10);
      unaff_CS = 0xe772;
      func_0x000e7753(0xe772);
      if ((bool)uVar3) {
        *(undefined2 *)0x57c = 0;
      }
    }
    iVar1 = *(int *)0x578;
    local_c = *(int *)0x57c;
    while( true ) {
      bVar2 = false;
      if (local_c < 1) break;
      uVar4 = func_0x000e777d(0xe772,10,0);
      *(undefined2 *)0x578 = (int)uVar4;
      *(undefined2 *)0x57a = (int)((ulong)uVar4 >> 0x10);
      local_c = local_c + -1;
    }
    func_0x000e7753(0xe772,0x423f,0xf);
    if (bVar2) {
      *(undefined1 *)0x623 = 0x15;
      return;
    }
    *(uint *)((int)*(undefined4 *)0x732 + 0x8c) = iVar1 << 4 | *(uint *)0x57c;
  }
  *(byte *)0x603 = *(byte *)0x603 | 3;
  return;
}



void __cdecl16far FUN_0009_3e0c(void)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined2 local_c;
  undefined2 local_a;
  
  *(byte *)0x603 = *(byte *)0x603 | 3;
  if (*(char *)0x5ff == '\0') {
    *(undefined2 *)((int)*(undefined4 *)0x732 + 0x8a) = 0x200;
  }
  else {
    iVar1 = func_0x000836aa();
    if (*(char *)0x623 == '\0') {
      if ((iVar1 < 0x101) && (0 < iVar1)) {
        local_c = 0x100;
        for (local_a = 8; -1 < local_a; local_a = local_a + -1) {
          if (local_c == iVar1) {
            *(int *)((int)*(undefined4 *)0x732 + 0x8a) = local_a << 8;
            break;
          }
          local_c = local_c >> 1;
        }
        if (local_a < 0) {
          *(undefined1 *)0x623 = 0x15;
        }
      }
      else {
        *(undefined1 *)0x623 = 0x15;
      }
    }
  }
  return;
}



void __cdecl16far FUN_0009_3e9c(void)

{
  return;
}



void __cdecl16far FUN_0009_3ec6(void)

{
  func_0x00094488();
  return;
}



void __cdecl16far FUN_0009_3ef1(void)

{
  func_0x00094488();
  return;
}



void __cdecl16far FUN_0009_3f1c(void)

{
  return;
}



void __cdecl16far FUN_0009_3f46(void)

{
  return;
}



void __cdecl16far FUN_0009_3f75(void)

{
  byte *pbVar1;
  undefined2 unaff_DS;
  
  pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x12);
  *pbVar1 = *pbVar1 | 2;
  *(byte *)0x603 = *(byte *)0x603 | 1;
  return;
}



void __cdecl16far FUN_0009_3f8d(void)

{
  byte *pbVar1;
  undefined2 unaff_DS;
  
  pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x12);
  *pbVar1 = *pbVar1 & 0xfd;
  *(byte *)0x603 = *(byte *)0x603 | 3;
  return;
}



void __cdecl16far FUN_0009_3fa5(void)

{
  return;
}



/* WARNING: Unable to track spacebase fully for stack */

void __cdecl16far FUN_0009_3fcf(void)

{
  byte *pbVar1;
  byte bVar2;
  bool bVar3;
  char cVar4;
  int in_AX;
  undefined1 *puVar5;
  uint uVar6;
  byte bVar7;
  int in_CX;
  byte bVar8;
  int in_BX;
  undefined2 *puVar9;
  byte *unaff_SI;
  int iVar10;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  char acStack_160b [5619];
  int local_18;
  undefined1 auStack_d [3];
  int local_a;
  
  iVar10 = (int)*(undefined4 *)0x732;
  local_a = *(int *)(iVar10 + 0x8a) >> 8;
  cVar4 = (char)in_AX;
  if (local_a != 0) {
    in_BX = 1 << ((byte)((uint)*(int *)(iVar10 + 0x8a) >> 8) & 0x1f);
    unaff_SI[in_BX] = unaff_SI[in_BX] + cVar4;
    in_CX = local_a;
    local_18 = in_BX;
  }
  out(0xff,cVar4);
  *(int *)(unaff_SI + in_BX) = *(int *)(unaff_SI + in_BX) + in_AX;
  bVar8 = unaff_SI[in_BX];
  pbVar1 = (byte *)(in_BX + iVar10);
  bVar2 = *pbVar1;
  bVar7 = (byte)in_CX;
  *pbVar1 = *pbVar1 + bVar7;
  cVar4 = cVar4 + bVar8 + '\x02' + CARRY1(bVar2,bVar7);
  *unaff_SI = *unaff_SI + cVar4;
  unaff_SI[in_BX] = unaff_SI[in_BX] + bVar7;
  unaff_SI[in_BX] = unaff_SI[in_BX] + cVar4;
  *(char *)0x914 = *(char *)0x914 + bVar7;
  puVar5 = (undefined1 *)(CONCAT11((char)((uint)in_AX >> 8),cVar4) + -0x131);
  *(char *)(in_BX + iVar10) = *(char *)(in_BX + iVar10) + (char)((uint)puVar5 >> 8);
  *(undefined1 **)(unaff_SI + in_BX) = auStack_d + *(int *)(unaff_SI + in_BX);
  cVar4 = (char)auStack_d + unaff_SI[in_BX];
  uVar6 = CONCAT11((char)((uint)auStack_d >> 8),cVar4);
  pbVar1 = unaff_SI;
  bVar2 = *pbVar1;
  bVar8 = (byte)in_BX;
  *pbVar1 = *pbVar1 + bVar8;
  *(int *)(in_BX + iVar10) = (*(int *)(in_BX + iVar10) - uVar6) - (uint)CARRY1(bVar2,bVar8);
  (&stack0xfffe)[(int)unaff_SI] = (&stack0xfffe)[(int)unaff_SI] + cVar4;
  *unaff_SI = *unaff_SI + cVar4;
  unaff_SI[in_BX] = unaff_SI[in_BX] + cVar4;
  pbVar1 = &stack0xfffe + iVar10;
  bVar2 = *pbVar1;
  *pbVar1 = *pbVar1 + bVar8;
  *(uint *)(&stack0xfffe + (int)unaff_SI) =
       (*(int *)(&stack0xfffe + (int)unaff_SI) - uVar6) - (uint)CARRY1(bVar2,bVar8);
  *unaff_SI = *unaff_SI + cVar4;
  unaff_SI[in_BX] = unaff_SI[in_BX] + bVar7;
  unaff_SI[in_BX] = unaff_SI[in_BX] + cVar4;
  acStack_160b[iVar10] = acStack_160b[iVar10] + bVar7;
  *(uint *)(unaff_SI + in_BX) = *(uint *)(unaff_SI + in_BX) | uVar6;
  do {
    *(undefined2 *)(puVar5 + -2) = 0x30;
    *(undefined2 *)(puVar5 + -4) = unaff_CS;
    unaff_CS = 0x82c9;
    puVar9 = (undefined2 *)(puVar5 + -6);
    puVar5 = puVar5 + -6;
    *puVar9 = 0x40de;
    func_0x00083241();
    bVar3 = 0 < (int)unaff_SI;
    unaff_SI = unaff_SI + -1;
  } while (bVar3);
  return;
}



void __cdecl16far FUN_0009_405f(void)

{
  func_0x00082e5b();
  return;
}



void __cdecl16far FUN_0009_4082(void)

{
  func_0x00081404();
  return;
}



void __cdecl16far FUN_0009_40aa(void)

{
  func_0x00081404();
  return;
}



void __cdecl16far FUN_0009_414c(void)

{
  undefined2 unaff_DS;
  uint local_a;
  
  local_a = func_0x000836aa();
  if (*(char *)0x623 == '\0') {
    if (*(char *)((int)*(undefined4 *)0x732 + 0xc) == '\x04') {
      local_a = (int)local_a >> 2 & 0xfffe;
    }
    else {
      local_a = local_a & 0xfffc;
    }
    if (*(char *)((int)*(undefined4 *)0x732 + 0x1e) == '\0') {
      if ((0x200 < (int)local_a) || ((int)local_a < 0)) {
        *(undefined1 *)0x623 = 0x15;
        return;
      }
      *(int *)((int)*(undefined4 *)0x732 + 0x88) = local_a << 3;
    }
    else {
      if ((0x3ff < (int)local_a) || ((int)local_a < 0x200)) {
        *(undefined1 *)0x623 = 4;
        return;
      }
      *(int *)((int)*(undefined4 *)0x732 + 0x88) = (0x400 - local_a) * 8;
    }
    *(byte *)0x603 = *(byte *)0x603 | 3;
  }
  return;
}



void __cdecl16far FUN_0009_41fa(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0009_422f(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0009_4264(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0009_42a6(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0009_42e8(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0009_4301(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0009_431b(void)

{
  func_0x000946c2();
  return;
}



void __cdecl16far FUN_0009_4356(void)

{
  int iVar1;
  undefined2 local_c;
  
  iVar1 = func_0x00094559();
  if (iVar1 == 0x3a) {
    local_c = 2;
  }
  else if (iVar1 == 0x41) {
    local_c = 0;
  }
  else if (((iVar1 == 0x48) || (iVar1 == 0x50)) || (iVar1 == 0x58)) {
    local_c = 3;
  }
  else {
    local_c = 3;
  }
  func_0x000944a2(0x941f,iVar1,local_c);
  return;
}



void __cdecl16far FUN_0009_43b8(void)

{
  undefined2 uVar1;
  
  uVar1 = 2;
  func_0x000944a2();
  func_0x000944a2(0x941f,0x41,0,uVar1);
  func_0x000944a2(0x941f,0x48,3);
  func_0x000944a2(0x941f,0x58,1);
  func_0x000944a2(0x941f,0x50,3);
  return;
}



void __cdecl16far FUN_0009_4411(void)

{
  func_0x000944d5();
  return;
}



void __cdecl16far FUN_0009_4424(void)

{
  func_0x000944d5();
  return;
}



void __cdecl16far FUN_0009_4437(void)

{
  func_0x000944d5();
  return;
}



void __cdecl16far FUN_0009_444a(void)

{
  func_0x000944d5();
  return;
}



void __cdecl16far FUN_0009_445d(void)

{
  func_0x000944d5();
  return;
}



void __cdecl16far FUN_0009_4470(void)

{
  undefined2 uVar1;
  
  uVar1 = func_0x00094559();
  func_0x000944d5(0x941f,uVar1,uVar1);
  return;
}



/* get_comm_config_flag (confidence: Confirmed)
   
   Evidence: `(index)` - reads a byte from a config/flag array at far ptr `[0x73A]`
   
   (physical 0x094488, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

undefined1 __stdcall16far get_comm_config_flag(word index)

{
  undefined2 unaff_DS;
  
  return *(undefined1 *)(index + (int)*(undefined4 *)0x73a);
}



/* set_comm_config_flag (confidence: Confirmed)
   
   Evidence: `(index, value)` - writes into the config/flag array at `[0x73A]`, then invalidates a
   cache at `es:[0x736]` and sets flags in `[0x603]`
   
   (physical 0x0944A2, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __stdcall16far set_comm_config_flag(word index,byte value)

{
  undefined2 unaff_DS;
  
  *(byte *)(index + (int)*(undefined4 *)0x73a) = value;
  *(undefined2 *)*(undefined4 *)0x736 = 0xffff;
  *(byte *)0x603 = *(byte *)0x603 | 1;
  if (*(char *)0x604 == '\0') {
    *(byte *)0x603 = *(byte *)0x603 | 2;
  }
  return;
}



void __stdcall16far FUN_0009_44d5(void)

{
  func_0x00094488();
  return;
}



undefined2 __cdecl16far FUN_0009_4559(void)

{
  undefined2 unaff_DS;
  undefined2 local_a;
  
  if (*(byte *)((int)*(undefined4 *)0x732 + 0x28) < 0x13) {
    if (*(byte *)((int)*(undefined4 *)0x732 + 0x28) < 8) {
      if (*(byte *)((int)*(undefined4 *)0x732 + 0x28) < 6) {
        local_a = 0x3a;
      }
      else {
        local_a = 0x41;
      }
    }
    else {
      local_a = 0x48;
    }
  }
  else if ((*(char *)((int)*(undefined4 *)0x732 + 0x17) == '\0') &&
          ((*(byte *)((int)*(undefined4 *)0x732 + 0xf) & 3) != 1)) {
    local_a = 0x50;
  }
  else {
    local_a = 0x58;
  }
  return local_a;
}



byte __stdcall16far FUN_0009_45cf(int param_1,int param_2)

{
  undefined2 unaff_DS;
  byte local_9;
  
  if (param_2 == 0) {
    local_9 = 1;
  }
  else if (param_2 == 2) {
    local_9 = 4;
  }
  else if (param_2 == 3) {
    local_9 = 8;
  }
  else {
    local_9 = 2;
  }
  if (*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x14') {
    if (param_1 == 0x3a) {
      local_9 = local_9 & 4;
    }
    else if (param_1 == 0x41) {
      local_9 = local_9 & 1;
    }
    else if (((param_1 == 0x48) || (param_1 == 0x50)) || (param_1 == 0x58)) {
      local_9 = local_9 & 9;
    }
    else {
      local_9 = 0;
    }
  }
  else if ((param_1 == 0x3a) || (param_1 == 0x41)) {
    local_9 = local_9 & 7;
  }
  else if ((param_1 != 0x48) && (param_1 != 0x50)) {
    if (param_1 == 0x58) {
      local_9 = local_9 & 9;
    }
    else {
      local_9 = 0;
    }
  }
  return local_9;
}



void __stdcall16far FUN_0009_46c2(int param_1)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_DS;
  
  iVar1 = func_0x00094559();
  iVar2 = func_0x000945cf(0x941f,iVar1,param_1);
  if (iVar2 == 0) {
    *(undefined1 *)0x623 = 4;
  }
  else {
    if ((iVar1 == 0x58) && (param_1 == 3)) {
      param_1 = 1;
    }
    func_0x000944a2(0x941f,iVar1,param_1);
  }
  return;
}



/* set_ds_return_old (confidence: Confirmed)
   
   Evidence: `push ds; mov ds,[bp+6]; pop ax` - swaps `DS` to the caller-given segment word, returns
   the *old* `DS` in `ax` so a later call with that saved value restores it. Called 40x; every
   checked call site pairs a "swap in" with a matching "swap back"
   
   (physical 0x09470E, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

undefined2 __stdcall16far set_ds_return_old(word new_segment)

{
  undefined2 unaff_DS;
  
  return unaff_DS;
}



void __stdcall16far FUN_0009_471c(byte param_1,undefined2 *param_2)

{
  byte *pbVar1;
  int *piVar2;
  byte bVar3;
  undefined2 uVar4;
  int iVar5;
  undefined2 unaff_DS;
  undefined2 uVar6;
  
  uVar6 = 0x8f80;
  uVar4 = func_0x0009470e();
  iVar5 = param_1 - 0x61;
  bVar3 = *(byte *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e + 10);
  uVar6 = func_0x000951aa(38000,iVar5,uVar6);
  if ((bVar3 & 2) == 0) {
    *(undefined2 *)0x690 = 0x117;
    *param_2 = 2;
    func_0x0009470e(0x9471,uVar4);
  }
  else {
    func_0x0009470e(0x9471,0x41);
    func_0x000e5d2f(38000);
    func_0x0009470e(0xe5d1,0x8f80);
    if ((((bVar3 & 0x10) != 0) || (*(int *)((int)*(undefined4 *)0x746 + 4) != 0)) ||
       (*(int *)*(undefined4 *)0x746 != 0)) {
      *(undefined2 *)0x690 = 0xf4;
    }
    *(undefined2 *)0x690 = 0xfa;
    pbVar1 = (byte *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e + 10);
    *pbVar1 = *pbVar1 | 8;
    piVar2 = (int *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e + 8);
    *piVar2 = *piVar2 + 1;
    piVar2 = (int *)((int)*(undefined4 *)0x746 + 2);
    *piVar2 = *piVar2 + 1;
    func_0x0009470e(38000,0x41);
    func_0x000e5d28(38000);
    func_0x0009470e(0xe5d1,0x8f80);
    *param_2 = uVar6;
    func_0x0009470e(38000,uVar4);
  }
  return;
}



void __stdcall16far FUN_0009_485b(byte param_1,undefined4 param_2,int *param_3)

{
  byte *pbVar1;
  int *piVar2;
  byte bVar3;
  undefined2 uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  undefined2 unaff_DS;
  undefined2 uVar11;
  
  uVar11 = 0x41;
  uVar4 = func_0x0009470e();
  iVar5 = func_0x00096126(38000,uVar11);
  *param_3 = iVar5;
  if (*param_3 == 1) {
    *(undefined2 *)0x690 = 0x14d;
    func_0x0009470e(0x9471,uVar4);
  }
  else {
    iVar5 = param_1 - 0x61;
    uVar11 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10);
    iVar6 = (int)*(undefined4 *)0x742 + iVar5 * 0x4e;
    bVar3 = *(byte *)((int)*(undefined4 *)0x73e + iVar5 * 0xc + 10);
    uVar9 = (undefined2)((ulong)*(undefined4 *)0x74a >> 0x10);
    iVar7 = (int)*(undefined4 *)0x74a + *(int *)((int)*(undefined4 *)0x746 + 8);
    if ((bVar3 & 4) == 0) {
      iVar8 = (int)param_2;
      uVar10 = (undefined2)((ulong)param_2 >> 0x10);
      if ((bVar3 & 2) == 0) {
        *(undefined2 *)0x690 = 0x1ec;
        if (*(int *)((int)*(undefined4 *)0x746 + 6) <
            *(int *)(iVar8 + 0x3a) + *(int *)(iVar8 + 0x40)) {
          *(undefined2 *)0x690 = 0x21a;
          *param_3 = 4;
          *(undefined2 *)*(undefined4 *)0x746 = 0;
          func_0x0009470e(0x9471,uVar4);
        }
        else {
          *(undefined2 *)0x690 = 0x1f0;
          func_0x000fbc09(0x9471,iVar8,uVar10,iVar6,uVar11,0x4e,uVar10,iVar8);
          *(int *)(iVar6 + 0x36) = iVar7;
          *(undefined2 *)(iVar6 + 0x38) = uVar9;
          *(int *)(iVar6 + 0x3c) = *(int *)(iVar6 + 0x3a) + iVar7;
          *(undefined2 *)(iVar6 + 0x3e) = uVar9;
          pbVar1 = (byte *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e + 10);
          *pbVar1 = *pbVar1 | 0x10;
          pbVar1 = (byte *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e + 10);
          *pbVar1 = *pbVar1 & 0xfe;
          pbVar1 = (byte *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e + 10);
          *pbVar1 = *pbVar1 | 2;
          piVar2 = (int *)((int)*(undefined4 *)0x746 + 4);
          *piVar2 = *piVar2 + 1;
          *(undefined2 *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e) =
               *(undefined2 *)((int)*(undefined4 *)0x746 + 8);
          *(int *)((int)*(undefined4 *)0x73e + iVar5 * 0xc + 4) =
               *(int *)((int)*(undefined4 *)0x746 + 8) + *(int *)(iVar6 + 0x3a);
          *(undefined2 *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e + 2) =
               *(undefined2 *)(iVar6 + 0x3a);
          *(undefined2 *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e + 6) =
               *(undefined2 *)(iVar6 + 0x40);
          piVar2 = (int *)((int)*(undefined4 *)0x746 + 8);
          *piVar2 = *piVar2 + *(int *)(iVar6 + 0x3a) + *(int *)(iVar6 + 0x40);
          piVar2 = (int *)((int)*(undefined4 *)0x746 + 6);
          *piVar2 = *piVar2 - (*(int *)(iVar6 + 0x3a) + *(int *)(iVar6 + 0x40));
          *param_3 = 0;
          *(undefined2 *)*(undefined4 *)0x746 = 0;
          func_0x0009470e(0xfbbe,uVar4);
        }
      }
      else {
        *(undefined2 *)0x690 = 0x172;
        if (*(int *)(iVar8 + 0x3a) + *(int *)(iVar8 + 0x40) ==
            *(int *)(iVar6 + 0x3a) + *(int *)(iVar6 + 0x40)) {
          *(undefined2 *)0x690 = 0x177;
          uVar9 = (undefined2)((ulong)*(undefined4 *)(iVar6 + 0x36) >> 0x10);
          iVar7 = (int)*(undefined4 *)(iVar6 + 0x36);
          func_0x000fbc09(0x9471,iVar8,uVar10,iVar6,uVar11,0x4e);
          *(int *)(iVar6 + 0x36) = iVar7;
          *(undefined2 *)(iVar6 + 0x38) = uVar9;
          *(int *)(iVar6 + 0x3c) = *(int *)(iVar6 + 0x3a) + iVar7;
          *(undefined2 *)(iVar6 + 0x3e) = uVar9;
          iVar5 = iVar5 * 0xc;
          *(int *)((int)*(undefined4 *)0x73e + iVar5 + 4) =
               *(int *)(iVar5 + (int)*(undefined4 *)0x73e) + *(int *)(iVar6 + 0x3a);
          *(undefined2 *)(iVar5 + (int)*(undefined4 *)0x73e + 2) = *(undefined2 *)(iVar6 + 0x3a);
          *(undefined2 *)(iVar5 + (int)*(undefined4 *)0x73e + 6) = *(undefined2 *)(iVar6 + 0x40);
          *param_3 = 0;
          pbVar1 = (byte *)(iVar5 + (int)*(undefined4 *)0x73e + 10);
          *pbVar1 = *pbVar1 | 0x10;
          piVar2 = (int *)((int)*(undefined4 *)0x746 + 4);
          *piVar2 = *piVar2 + 1;
          *(undefined2 *)*(undefined4 *)0x746 = 0;
          func_0x0009470e(0xfbbe,uVar4);
        }
        else {
          *(undefined2 *)0x690 = 0x1a5;
          uVar9 = (undefined2)((ulong)*(undefined4 *)0x746 >> 0x10);
          iVar7 = (int)*(undefined4 *)0x746;
          if (*(int *)(iVar6 + 0x3a) + *(int *)(iVar6 + 0x40) + *(int *)(iVar7 + 6) <
              *(int *)(iVar8 + 0x3a) + *(int *)(iVar8 + 0x40)) {
            *(undefined2 *)0x690 = 0x1dd;
            *param_3 = 4;
            *(undefined2 *)*(undefined4 *)0x746 = 0;
            func_0x0009470e(0x9471,uVar4);
          }
          else {
            *(undefined2 *)0x690 = 0x1aa;
            func_0x00095d81(0x9471,iVar5,iVar7,iVar8,uVar9);
            uVar9 = (undefined2)((ulong)*(undefined4 *)0x74a >> 0x10);
            iVar7 = (int)*(undefined4 *)0x74a + *(int *)((int)*(undefined4 *)0x746 + 8);
            pbVar1 = (byte *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e + 10);
            *pbVar1 = *pbVar1 & 0xfe;
            pbVar1 = (byte *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e + 10);
            *pbVar1 = *pbVar1 | 2;
            func_0x000fbc09(0x9471,iVar8,uVar10,iVar6,uVar11,0x4e);
            *(int *)(iVar6 + 0x36) = iVar7;
            *(undefined2 *)(iVar6 + 0x38) = uVar9;
            *(int *)(iVar6 + 0x3c) = iVar7 + *(int *)(iVar6 + 0x3a);
            *(undefined2 *)(iVar6 + 0x3e) = uVar9;
            *(undefined2 *)((int)*(undefined4 *)0x73e + iVar5 * 0xc) =
                 *(undefined2 *)((int)*(undefined4 *)0x746 + 8);
            *(int *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e + 4) =
                 *(int *)((int)*(undefined4 *)0x746 + 8) + *(int *)(iVar6 + 0x3a);
            *(undefined2 *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e + 2) =
                 *(undefined2 *)(iVar6 + 0x3a);
            *(undefined2 *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e + 6) =
                 *(undefined2 *)(iVar6 + 0x40);
            piVar2 = (int *)((int)*(undefined4 *)0x746 + 8);
            *piVar2 = *piVar2 + *(int *)(iVar6 + 0x3a) + *(int *)(iVar6 + 0x40);
            piVar2 = (int *)((int)*(undefined4 *)0x746 + 6);
            *piVar2 = *piVar2 - (*(int *)(iVar6 + 0x3a) + *(int *)(iVar6 + 0x40));
            *param_3 = 0;
            pbVar1 = (byte *)(iVar5 * 0xc + (int)*(undefined4 *)0x73e + 10);
            *pbVar1 = *pbVar1 | 0x10;
            piVar2 = (int *)((int)*(undefined4 *)0x746 + 4);
            *piVar2 = *piVar2 + 1;
            *(undefined2 *)*(undefined4 *)0x746 = 0;
            func_0x0009470e(0xfbbe,uVar4);
          }
        }
      }
    }
    else {
      *(undefined2 *)0x690 = 0x165;
      *param_3 = 0x40;
      *(undefined2 *)*(undefined4 *)0x746 = 0;
      func_0x0009470e(0x9471,uVar4);
    }
  }
  return;
}



undefined2 __stdcall16far FUN_0009_4d9c(byte param_1)

{
  int *piVar1;
  byte *pbVar2;
  byte bVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  int iVar6;
  undefined2 unaff_DS;
  
  if ((param_1 < 0x61) || (0x7a < param_1)) {
    uVar4 = 0;
  }
  else {
    uVar4 = 0x8f80;
    uVar5 = func_0x0009470e();
    iVar6 = param_1 - 0x61;
    bVar3 = *(byte *)(iVar6 * 0xc + (int)*(undefined4 *)0x73e + 10);
    if ((bVar3 & 0x18) == 0) {
      func_0x0009470e(38000,uVar5);
      uVar4 = 8;
    }
    else if ((bVar3 & 8) == 0) {
      uVar4 = func_0x000950c2(38000,iVar6,uVar4);
      func_0x0009470e(0x9471,0x41);
      func_0x000e5d2f(38000);
      func_0x0009470e(0xe5d1,0x8f80);
      pbVar2 = (byte *)(iVar6 * 0xc + (int)*(undefined4 *)0x73e + 10);
      *pbVar2 = *pbVar2 & 0xef;
      piVar1 = (int *)((int)*(undefined4 *)0x746 + 4);
      *piVar1 = *piVar1 + -1;
      func_0x0009470e(38000,0x41);
      func_0x000e5d28(38000);
      func_0x0009470e(0xe5d1,uVar5);
    }
    else {
      uVar4 = func_0x000951aa(38000,iVar6);
      func_0x0009470e(0x9471,0x41);
      func_0x000e5d2f(38000);
      func_0x0009470e(0xe5d1,0x8f80);
      piVar1 = (int *)((int)*(undefined4 *)0x746 + 2);
      *piVar1 = *piVar1 + -1;
      piVar1 = (int *)((int)*(undefined4 *)0x73e + iVar6 * 0xc + 8);
      *piVar1 = *piVar1 + -1;
      if (*(int *)((int)*(undefined4 *)0x73e + iVar6 * 0xc + 8) == 0) {
        pbVar2 = (byte *)((int)*(undefined4 *)0x73e + iVar6 * 0xc + 10);
        *pbVar2 = *pbVar2 & 0xf7;
      }
      func_0x0009470e(38000,0x41);
      func_0x000e5d28(38000);
      func_0x0009470e(0xe5d1,uVar5);
    }
  }
  return uVar4;
}



undefined2 __stdcall16far FUN_0009_4ef5(int param_1)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  undefined2 uVar4;
  undefined2 local_10;
  
  uVar4 = 0x8f80;
  uVar3 = 38000;
  uVar1 = func_0x0009470e();
  param_1 = param_1 + -0x61;
  if ((param_1 < 0) || (0x19 < param_1)) {
    local_10 = 0x10;
  }
  else {
    uVar3 = 0x9471;
    iVar2 = func_0x00096126(38000,uVar4);
    if (iVar2 == 0) {
      if (((*(byte *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 10) & 1) == 0) &&
         ((*(byte *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 10) & 4) == 0)) {
        func_0x00095d81(0x9471,param_1);
        local_10 = 0;
      }
      else if ((*(byte *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 10) & 4) == 0) {
        if ((*(byte *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 10) & 1) == 0) {
          local_10 = 0x20;
        }
        else {
          local_10 = 2;
        }
      }
      else {
        local_10 = 0x40;
      }
      uVar3 = 0x9471;
      *(undefined2 *)*(undefined4 *)0x746 = 0;
    }
    else {
      local_10 = 1;
    }
  }
  func_0x0009470e(uVar3,uVar1);
  return local_10;
}



undefined2 __stdcall16far FUN_0009_4fda(int param_1)

{
  byte *pbVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  undefined2 local_10;
  
  uVar5 = 0x8f80;
  uVar4 = 38000;
  uVar2 = func_0x0009470e();
  param_1 = param_1 + -0x61;
  if ((param_1 < 0) || (0x19 < param_1)) {
    local_10 = 0x10;
  }
  else {
    uVar4 = 0x9471;
    iVar3 = func_0x00096126(38000,uVar5);
    if (iVar3 == 0) {
      if ((*(byte *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 10) & 0x40) == 0) {
        pbVar1 = (byte *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 10);
        *pbVar1 = *pbVar1 ^ 4;
        local_10 = 0;
      }
      else {
        local_10 = 0x400;
      }
    }
    else {
      local_10 = 1;
    }
    *(undefined2 *)*(undefined4 *)0x746 = 0;
  }
  func_0x0009470e(uVar4,uVar2);
  return local_10;
}



undefined1 __stdcall16far FUN_0009_5067(byte param_1)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  uVar3 = 0x8f80;
  uVar2 = func_0x0009470e();
  if ((param_1 < 0x61) || (0x7a < param_1)) {
    uVar1 = 0;
  }
  else {
    uVar1 = *(undefined1 *)((param_1 - 0x61) * 0xc + (int)*(undefined4 *)0x73e + 10);
  }
  func_0x0009470e(38000,uVar2,uVar3);
  return uVar1;
}



/* build_gpib_message_checksum (confidence: Mechanism confirmed; exact message format not fully
   traced)
   
   Evidence: Indexes a 78-byte record table at `[0x742]` and a 12-byte record table at `[0x73E]` by
   a caller-given index, computes checksums (via `checksum_bytes`) over message regions derived from
   those records' fields - part of GPIB message building/verification
   
   (physical 0x0950C2, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __stdcall16far build_gpib_message_checksum(int param_1)

{
  byte *pbVar1;
  undefined1 uVar2;
  char cVar3;
  char cVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  undefined2 unaff_DS;
  
  uVar7 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10);
  iVar6 = (int)*(undefined4 *)0x742;
  iVar5 = *(int *)(param_1 * 0x4e + (int)*(undefined4 *)0x742 + 0x3a) +
          *(int *)(param_1 * 0x4e + iVar6 + 0x40);
  uVar8 = (undefined2)((ulong)*(undefined4 *)0x74a >> 0x10);
  uVar2 = func_0x0009605a();
  cVar3 = func_0x0009605a(0x9471,(int)*(undefined4 *)0x742 + param_1 * 0x4e,
                          (int)((ulong)*(undefined4 *)0x742 >> 0x10),0x4c,uVar8,iVar5,uVar7,iVar6);
  cVar4 = func_0x0009605a(0x9471,(int)*(undefined4 *)0x73e + param_1 * 0xc,
                          (int)((ulong)*(undefined4 *)0x73e >> 0x10),8);
  *(undefined1 *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 0xb) = uVar2;
  *(char *)(param_1 * 0x4e + (int)*(undefined4 *)0x742 + 0x4c) = cVar3 + cVar4;
  pbVar1 = (byte *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 10);
  *pbVar1 = *pbVar1 & 0xdf;
  return;
}



/* validate_gpib_record_consistency (confidence: Mechanism confirmed)
   
   Evidence: `(index)` - cross-checks a GPIB record against the same 3 linked tables
   `build_gpib_message_checksum`/`init_gpib_record` use (`[0x73E]` stride `0xC`, `[0x742]` stride
   `0x4E`, `[0x74A]` a base far pointer): far-pointer equality checks, range checks against
   `[0,0x6800]`, and several cross-field comparisons; each failure ORs a distinct bit into an
   accumulator and records a diagnostic code (`0x387`, `0x38F`, `0x39B`, ... `0x3F6`) into
   `[0x690]`; returns the accumulated failure bitmask - a GPIB parameter-record consistency
   validator, sibling to `SUB_95B69`/`SUB_96DBE` which use the same 3 tables
   
   (physical 0x0951AA, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

undefined4 __stdcall16far validate_gpib_record_consistency(word index)

{
  byte bVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  char cVar5;
  char cVar6;
  char cVar7;
  int iVar8;
  uint uVar9;
  int iVar10;
  undefined2 uVar11;
  undefined2 uVar12;
  undefined2 unaff_DS;
  uint local_1c;
  
  local_1c = 0;
  iVar8 = *(int *)(index * 0xc + (int)*(undefined4 *)0x73e);
  iVar10 = *(int *)(index * 0xc + (int)*(undefined4 *)0x73e + 4);
  uVar3 = *(undefined4 *)(index * 0x4e + (int)*(undefined4 *)0x742 + 0x36);
  uVar4 = *(undefined4 *)(index * 0x4e + (int)*(undefined4 *)0x742 + 0x3c);
  if (((((int)*(undefined4 *)0x74a + iVar8 != (int)uVar3) ||
       ((int)((ulong)*(undefined4 *)0x74a >> 0x10) != (int)((ulong)uVar3 >> 0x10))) ||
      ((int)*(undefined4 *)0x74a + iVar10 != (int)uVar4)) ||
     ((int)((ulong)*(undefined4 *)0x74a >> 0x10) != (int)((ulong)uVar4 >> 0x10))) {
    *(undefined2 *)0x690 = 0x387;
    local_1c = 1;
  }
  if (((iVar8 < 0) || (0x6800 < iVar8)) || ((iVar10 < 0 || (0x6800 < iVar10)))) {
    *(undefined2 *)0x690 = 0x38f;
    local_1c = 1;
  }
  iVar10 = *(int *)(index * 0xc + (int)*(undefined4 *)0x73e + 2);
  iVar2 = *(int *)(index * 0xc + (int)*(undefined4 *)0x73e + 6);
  uVar9 = iVar10 + iVar2;
  if ((iVar10 != *(int *)((int)*(undefined4 *)0x742 + index * 0x4e + 0x3a)) ||
     (iVar2 != *(int *)((int)*(undefined4 *)0x742 + index * 0x4e + 0x40))) {
    *(undefined2 *)0x690 = 0x39b;
    local_1c = local_1c | 2;
  }
  if ((((0x2000 < (int)uVar9) || ((int)uVar9 < 0x400)) || ((uVar9 & 0x3ff) != 0)) ||
     (0x6800 < (int)(iVar8 + uVar9))) {
    *(undefined2 *)0x690 = 0x3ab;
    local_1c = local_1c | 2;
  }
  bVar1 = *(byte *)(index * 0xc + (int)*(undefined4 *)0x73e + 10);
  uVar9 = bVar1 & 1;
  if (((bVar1 & 1) != 0) && ((bVar1 & 0xfa) != 0)) {
    local_1c = local_1c | 0x80;
  }
  if (((bVar1 & 2) != 0) && ((bVar1 & 0x80) != 0)) {
    local_1c = local_1c | 0x80;
  }
  if (((bVar1 & 0x18) != 0) ||
     (uVar9 = (uint)((ulong)((long)(int)index * 0xc) >> 0x10),
     *(int *)((int)((long)(int)index * 0xc) + (int)*(undefined4 *)0x73e + 8) != 0)) {
    local_1c = local_1c | 0x200;
  }
  if ((local_1c == 0) || (local_1c == 0x200)) {
    uVar11 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10);
    iVar10 = (int)*(undefined4 *)0x742;
    iVar8 = *(int *)(index * 0x4e + (int)*(undefined4 *)0x742 + 0x3a) +
            *(int *)(index * 0x4e + iVar10 + 0x40);
    uVar12 = (undefined2)((ulong)*(undefined4 *)0x74a >> 0x10);
    cVar5 = func_0x0009605a();
    cVar6 = func_0x0009605a(0x9471,(int)*(undefined4 *)0x742 + index * 0x4e,
                            (int)((ulong)*(undefined4 *)0x742 >> 0x10),0x4c,uVar12,iVar8,uVar11,
                            iVar10);
    cVar7 = func_0x0009605a(0x9471,(int)*(undefined4 *)0x73e + index * 0xc,
                            (int)((ulong)*(undefined4 *)0x73e >> 0x10),8);
    if (cVar5 != *(char *)(index * 0xc + (int)*(undefined4 *)0x73e + 0xb)) {
      *(undefined2 *)0x690 = 0x3ef;
      local_1c = local_1c | 4;
    }
    cVar5 = *(char *)((int)((long)(int)index * 0x4e) + (int)*(undefined4 *)0x742 + 0x4c);
    uVar9 = CONCAT11((char)((ulong)((long)(int)index * 0x4e) >> 0x18),cVar5);
    if ((char)(cVar6 + cVar7) != cVar5) {
      local_1c = local_1c | 0x80;
      *(undefined2 *)0x690 = 0x3f6;
    }
  }
  return CONCAT22(uVar9,local_1c);
}



undefined4 FUN_0009_526b(void)

{
  byte bVar1;
  long lVar2;
  undefined1 uVar3;
  char cVar4;
  int iVar5;
  uint uVar6;
  int unaff_BP;
  int iVar7;
  undefined2 uVar8;
  undefined2 uVar9;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined2 uVar10;
  
  *(undefined2 *)0x690 = 0x38f;
  *(uint *)(unaff_BP + -0x1a) = *(uint *)(unaff_BP + -0x1a) | 1;
  iVar5 = *(int *)(unaff_BP + 6) * 0xc;
  *(undefined2 *)(unaff_BP + -0x14) = *(undefined2 *)(iVar5 + (int)*(undefined4 *)0x73e + 2);
  iVar5 = *(int *)(iVar5 + (int)*(undefined4 *)0x73e + 6);
  *(int *)(unaff_BP + -0x16) = iVar5;
  *(int *)(unaff_BP + -0x18) = *(int *)(unaff_BP + -0x14) + iVar5;
  iVar5 = *(int *)(unaff_BP + 6) * 0x4e;
  if ((*(int *)(unaff_BP + -0x14) != *(int *)((int)*(undefined4 *)0x742 + iVar5 + 0x3a)) ||
     (*(int *)(unaff_BP + -0x16) != *(int *)((int)*(undefined4 *)0x742 + iVar5 + 0x40))) {
    *(undefined2 *)0x690 = 0x39b;
    *(uint *)(unaff_BP + -0x1a) = *(uint *)(unaff_BP + -0x1a) | 2;
  }
  if ((((0x2000 < *(int *)(unaff_BP + -0x18)) || (*(int *)(unaff_BP + -0x18) < 0x400)) ||
      ((*(uint *)(unaff_BP + -0x18) & 0x3ff) != 0)) ||
     (0x6800 < *(int *)(unaff_BP + -0x1e) + *(int *)(unaff_BP + -0x18))) {
    *(undefined2 *)0x690 = 0x3ab;
    *(uint *)(unaff_BP + -0x1a) = *(uint *)(unaff_BP + -0x1a) | 2;
  }
  bVar1 = *(byte *)(*(int *)(unaff_BP + 6) * 0xc + (int)*(undefined4 *)0x73e + 10);
  *(uint *)(unaff_BP + -0x2a) = (uint)bVar1;
  uVar6 = bVar1 & 1;
  if (((bVar1 & 1) != 0) && ((*(uint *)(unaff_BP + -0x2a) & 0xfffa) != 0)) {
    *(uint *)(unaff_BP + -0x1a) = *(uint *)(unaff_BP + -0x1a) | 0x80;
  }
  if (((*(uint *)(unaff_BP + -0x2a) & 2) != 0) && ((*(uint *)(unaff_BP + -0x2a) & 0x80) != 0)) {
    *(uint *)(unaff_BP + -0x1a) = *(uint *)(unaff_BP + -0x1a) | 0x80;
  }
  if (((*(uint *)(unaff_BP + -0x2a) & 0x18) != 0) ||
     (lVar2 = (long)*(int *)(unaff_BP + 6) * 0xc, uVar6 = (uint)((ulong)lVar2 >> 0x10),
     *(int *)((int)lVar2 + (int)*(undefined4 *)0x73e + 8) != 0)) {
    *(uint *)(unaff_BP + -0x1a) = *(uint *)(unaff_BP + -0x1a) | 0x200;
  }
  if ((*(int *)(unaff_BP + -0x1a) == 0) || (*(int *)(unaff_BP + -0x1a) == 0x200)) {
    iVar5 = *(int *)(unaff_BP + 6) * 0x4e;
    uVar8 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10);
    iVar7 = (int)*(undefined4 *)0x742;
    *(int *)(unaff_BP + -8) =
         *(int *)(iVar5 + (int)*(undefined4 *)0x742 + 0x3a) + *(int *)(iVar5 + iVar7 + 0x40);
    *(undefined2 *)(unaff_BP + -0x1c) =
         *(undefined2 *)(*(int *)(unaff_BP + 6) * 0xc + (int)*(undefined4 *)0x73e);
    uVar10 = *(undefined2 *)(unaff_BP + -8);
    uVar9 = (undefined2)((ulong)*(undefined4 *)0x74a >> 0x10);
    uVar3 = func_0x0009605a();
    *(undefined1 *)(unaff_BP + -9) = uVar3;
    uVar3 = func_0x0009605a(0x9471,(int)*(undefined4 *)0x742 + *(int *)(unaff_BP + 6) * 0x4e,
                            (int)((ulong)*(undefined4 *)0x742 >> 0x10),0x4c,uVar9,uVar10,uVar8,iVar7
                           );
    *(undefined1 *)(unaff_BP + -10) = uVar3;
    cVar4 = func_0x0009605a(0x9471,(int)*(undefined4 *)0x73e + *(int *)(unaff_BP + 6) * 0xc,
                            (int)((ulong)*(undefined4 *)0x73e >> 0x10),8);
    *(char *)(unaff_BP + -10) = *(char *)(unaff_BP + -10) + cVar4;
    if (*(char *)(unaff_BP + -9) !=
        *(char *)(*(int *)(unaff_BP + 6) * 0xc + (int)*(undefined4 *)0x73e + 0xb)) {
      *(undefined2 *)0x690 = 0x3ef;
      *(uint *)(unaff_BP + -0x1a) = *(uint *)(unaff_BP + -0x1a) | 4;
    }
    lVar2 = (long)*(int *)(unaff_BP + 6) * 0x4e;
    cVar4 = *(char *)((int)lVar2 + (int)*(undefined4 *)0x742 + 0x4c);
    uVar6 = CONCAT11((char)((ulong)lVar2 >> 0x18),cVar4);
    if (*(char *)(unaff_BP + -10) != cVar4) {
      *(uint *)(unaff_BP + -0x1a) = *(uint *)(unaff_BP + -0x1a) | 0x80;
      *(undefined2 *)0x690 = 0x3f6;
    }
  }
  return CONCAT22(uVar6,*(undefined2 *)(unaff_BP + -0x1a));
}



/* scan_and_repair_gpib_records (confidence: Mechanism confirmed)
   
   Evidence: A large (184-byte local frame) loop over the GPIB record table `[0x73E]` (`index*0xC`
   stride, matches its confirmed 12-byte record size): for each entry, calls
   `validate_gpib_record_consistency`; on failure, increments an error counter and accumulates the
   failure bitmask into `[0x746]+0xA`/`+0xC`, then either calls `init_gpib_record` to reset a minor
   failure or takes a more elaborate recovery path for more severe ones - a boot/reinit-time GPIB
   record-table integrity scrub
   
   (physical 0x095476, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

undefined2 __cdecl16far scan_and_repair_gpib_records(void)

{
  int *piVar1;
  uint *puVar2;
  byte *pbVar3;
  byte bVar4;
  undefined2 uVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  undefined2 uVar10;
  undefined2 uVar11;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined2 uVar12;
  int local_ba;
  int local_ae;
  int local_ac;
  int local_a4;
  int local_a2;
  byte local_a0 [158];
  
  uVar12 = 0x8f80;
  uVar11 = 38000;
  uVar5 = func_0x0009470e();
  *(undefined2 *)((int)*(undefined4 *)0x746 + 0xc) = 0;
  *(undefined2 *)((int)*(undefined4 *)0x746 + 10) = 0;
  if (*(int *)*(undefined4 *)0x746 != 0) {
    piVar1 = (int *)((int)*(undefined4 *)0x746 + 0xc);
    *piVar1 = *piVar1 + 1;
    puVar2 = (uint *)((int)*(undefined4 *)0x746 + 10);
    *puVar2 = *puVar2 | 0x100;
    *(undefined2 *)*(undefined4 *)0x746 = 0;
  }
  if ((*(int *)((int)*(undefined4 *)0x746 + 2) != 0) ||
     (*(int *)((int)*(undefined4 *)0x746 + 4) != 0)) {
    piVar1 = (int *)((int)*(undefined4 *)0x746 + 0xc);
    *piVar1 = *piVar1 + 1;
    puVar2 = (uint *)((int)*(undefined4 *)0x746 + 10);
    *puVar2 = *puVar2 | 0x200;
    *(undefined2 *)((int)*(undefined4 *)0x746 + 4) = 0;
    *(undefined2 *)((int)*(undefined4 *)0x746 + 2) = 0;
  }
  for (local_ac = 0; local_ac < 0x1a; local_ac = local_ac + 1) {
    local_a0[local_ac * 6] = 0;
    iVar6 = local_ac * 0xc;
    if ((*(byte *)((int)*(undefined4 *)0x73e + iVar6 + 10) & 1) == 0) {
      uVar7 = func_0x000951aa(uVar11,local_ac);
      uVar11 = 0x9471;
      if (uVar7 != 0) {
        *(undefined2 *)0x690 = 0x459;
        piVar1 = (int *)((int)*(undefined4 *)0x746 + 0xc);
        *piVar1 = *piVar1 + 1;
        puVar2 = (uint *)((int)*(undefined4 *)0x746 + 10);
        *puVar2 = *puVar2 | uVar7;
        if ((uVar7 & 0xfdfb) == 0) {
          pbVar3 = (byte *)(local_ac * 0xc + (int)*(undefined4 *)0x73e + 10);
          *pbVar3 = *pbVar3 | 0x20;
          pbVar3 = (byte *)(local_ac * 0xc + (int)*(undefined4 *)0x73e + 10);
          *pbVar3 = *pbVar3 & 0xe7;
          *(undefined2 *)(local_ac * 0xc + (int)*(undefined4 *)0x73e + 8) = 0;
        }
        else {
          *(undefined2 *)0x690 = 0x462;
          func_0x000961e9(0x9471,local_ac);
          uVar11 = 0x9471;
        }
      }
    }
    else if ((((((*(int *)((int)*(undefined4 *)0x73e + iVar6) != 0) ||
                (*(int *)((int)*(undefined4 *)0x73e + iVar6 + 4) != 0)) ||
               (*(int *)((int)*(undefined4 *)0x73e + iVar6 + 2) != 0)) ||
              ((*(int *)((int)*(undefined4 *)0x73e + iVar6 + 4) != 0 ||
               (*(int *)((int)*(undefined4 *)0x73e + iVar6 + 8) != 0)))) ||
             ((iVar6 = local_ac * 0x4e, uVar10 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10),
              iVar8 = (int)*(undefined4 *)0x742,
              *(int *)(iVar8 + iVar6 + 0x36) != 0 || *(int *)(iVar8 + iVar6 + 0x38) != 0 ||
              ((*(int *)((int)*(undefined4 *)0x742 + iVar6 + 0x3a) != 0 ||
               (uVar10 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10),
               iVar8 = (int)*(undefined4 *)0x742,
               *(int *)(iVar8 + iVar6 + 0x3c) != 0 || *(int *)(iVar8 + iVar6 + 0x3e) != 0)))))) ||
            (*(int *)((int)*(undefined4 *)0x742 + iVar6 + 0x40) != 0)) {
      *(undefined2 *)0x690 = 0x444;
      *(undefined2 *)(local_ac * 0xc + (int)*(undefined4 *)0x73e) = 0;
      *(undefined2 *)(local_ac * 0xc + (int)*(undefined4 *)0x73e + 4) = 0;
      *(undefined2 *)(local_ac * 0xc + (int)*(undefined4 *)0x73e + 2) = 0;
      *(undefined2 *)(local_ac * 0xc + (int)*(undefined4 *)0x73e + 4) = 0;
      *(undefined2 *)(local_ac * 0xc + (int)*(undefined4 *)0x73e + 8) = 0;
      func_0x00096087(uVar11,(int)*(undefined4 *)0x742 + local_ac * 0x4e,
                      (int)((ulong)*(undefined4 *)0x742 >> 0x10),0x4e,0);
      piVar1 = (int *)((int)*(undefined4 *)0x746 + 0xc);
      *piVar1 = *piVar1 + 1;
      puVar2 = (uint *)((int)*(undefined4 *)0x746 + 10);
      *puVar2 = *puVar2 | 0x80;
      *(undefined1 *)(local_ac * 0xc + (int)*(undefined4 *)0x73e + 10) = 1;
      uVar11 = 0x9471;
    }
  }
  local_ba = 0;
  for (local_ac = 0; local_ac < 0x1a; local_ac = local_ac + 1) {
    if ((*(byte *)(local_ac * 0xc + (int)*(undefined4 *)0x73e + 10) & 2) != 0) {
      (&local_a4)[local_ba * 3] = *(int *)(local_ac * 0xc + (int)*(undefined4 *)0x73e);
      *(int *)(local_a0 + local_ba * 6 + -2) = local_ac;
      local_a0[local_ba * 6] = 0;
      local_ba = local_ba + 1;
    }
  }
  if (local_ba < 2) {
    if (local_ba == 1) {
      iVar6 = local_a4 + *(int *)(local_a2 * 0xc + (int)*(undefined4 *)0x73e + 2) +
              *(int *)(local_a2 * 0xc + (int)*(undefined4 *)0x73e + 6);
      if (iVar6 == *(int *)((int)*(undefined4 *)0x746 + 8)) {
        *(undefined2 *)0x690 = 0x506;
      }
      else {
        *(undefined2 *)0x690 = 0x50b;
        local_a0[0] = local_a0[0] | 1;
        puVar2 = (uint *)((int)*(undefined4 *)0x746 + 10);
        *puVar2 = *puVar2 | 0x20;
        piVar1 = (int *)((int)*(undefined4 *)0x746 + 0xc);
        *piVar1 = *piVar1 + 1;
        *(int *)((int)*(undefined4 *)0x746 + 8) = iVar6;
      }
    }
    else {
      *(undefined2 *)0x690 = 0x519;
    }
  }
  else {
    for (local_ac = 0; local_ac < local_ba; local_ac = local_ac + 1) {
      for (local_ae = 0; local_ae < (local_ba - local_ac) + -1; local_ae = local_ae + 1) {
        iVar6 = (&local_a4)[local_ae * 3];
        if ((&local_a4)[(local_ae + 1) * 3] < iVar6) {
          iVar8 = local_ae * 6;
          uVar10 = *(undefined2 *)(local_a0 + iVar8 + -2);
          bVar4 = local_a0[iVar8];
          iVar9 = local_ae + 1;
          (&local_a4)[local_ae * 3] = (&local_a4)[iVar9 * 3];
          *(undefined2 *)(local_a0 + iVar8 + -2) = *(undefined2 *)(local_a0 + iVar9 * 6 + -2);
          local_a0[iVar8] = local_a0[iVar9 * 6];
          (&local_a4)[iVar9 * 3] = iVar6;
          *(undefined2 *)(local_a0 + (local_ae + 1) * 6 + -2) = uVar10;
          local_a0[(local_ae + 1) * 6] = bVar4;
        }
      }
    }
    if (local_a4 != 0) {
      puVar2 = (uint *)((int)*(undefined4 *)0x746 + 10);
      *puVar2 = *puVar2 | 0x10;
      piVar1 = (int *)((int)*(undefined4 *)0x746 + 0xc);
      *piVar1 = *piVar1 + 1;
    }
    for (local_ac = 0; iVar6 = local_ba + -1, local_ac < iVar6; local_ac = local_ac + 1) {
      iVar6 = (&local_a4)[local_ac * 3] +
              *(int *)((int)*(undefined4 *)0x73e + *(int *)(local_a0 + local_ac * 6 + -2) * 0xc + 2)
              + *(int *)((int)*(undefined4 *)0x73e + *(int *)(local_a0 + local_ac * 6 + -2) * 0xc +
                        6);
      if (iVar6 == (&local_a4)[(local_ac + 1) * 3]) {
        *(undefined2 *)0x690 = 0x4bc;
      }
      else if ((&local_a4)[(local_ac + 1) * 3] < iVar6) {
        *(undefined2 *)0x690 = 0x4c5;
        puVar2 = (uint *)((int)*(undefined4 *)0x746 + 10);
        *puVar2 = *puVar2 | 8;
        piVar1 = (int *)((int)*(undefined4 *)0x746 + 0xc);
        *piVar1 = *piVar1 + 1;
        local_a0[local_ac * 6] = local_a0[local_ac * 6] | 1;
        local_a0[(local_ac + 1) * 6] = local_a0[(local_ac + 1) * 6] | 1;
      }
      else {
        *(undefined2 *)0x690 = 0x4d3;
        puVar2 = (uint *)((int)*(undefined4 *)0x746 + 10);
        *puVar2 = *puVar2 | 0x10;
        piVar1 = (int *)((int)*(undefined4 *)0x746 + 0xc);
        *piVar1 = *piVar1 + 1;
      }
    }
    iVar8 = (&local_a4)[iVar6 * 3] +
            *(int *)(*(int *)(local_a0 + iVar6 * 6 + -2) * 0xc + (int)*(undefined4 *)0x73e + 2) +
            *(int *)(*(int *)(local_a0 + iVar6 * 6 + -2) * 0xc + (int)*(undefined4 *)0x73e + 6);
    if (iVar8 == *(int *)((int)*(undefined4 *)0x746 + 8)) {
      *(undefined2 *)0x690 = 0x4e7;
    }
    else {
      *(undefined2 *)0x690 = 0x4ee;
      local_a0[iVar6 * 6] = local_a0[iVar6 * 6] | 1;
      puVar2 = (uint *)((int)*(undefined4 *)0x746 + 10);
      *puVar2 = *puVar2 | 0x20;
      piVar1 = (int *)((int)*(undefined4 *)0x746 + 0xc);
      *piVar1 = *piVar1 + 1;
      *(int *)((int)*(undefined4 *)0x746 + 8) = iVar8;
    }
  }
  if (*(int *)((int)*(undefined4 *)0x746 + 8) + *(int *)((int)*(undefined4 *)0x746 + 6) == 0x6800) {
    *(undefined2 *)0x690 = 0x523;
  }
  else {
    *(undefined2 *)0x690 = 0x528;
    puVar2 = (uint *)((int)*(undefined4 *)0x746 + 10);
    *puVar2 = *puVar2 | 0x40;
    piVar1 = (int *)((int)*(undefined4 *)0x746 + 0xc);
    *piVar1 = *piVar1 + 1;
    *(int *)((int)*(undefined4 *)0x746 + 6) = 0x6800 - *(int *)((int)*(undefined4 *)0x746 + 8);
  }
  if (*(int *)((int)*(undefined4 *)0x746 + 0xc) < 7) {
    if (*(int *)((int)*(undefined4 *)0x746 + 10) == 0) {
      func_0x0009470e(uVar11,uVar5,uVar12);
      *(undefined2 *)0x690 = 0x54f;
      uVar11 = 0;
    }
    else {
      iVar6 = func_0x00095b69(uVar11,&local_a4);
      if (iVar6 == 0) {
        *(undefined2 *)0x690 = 0x547;
        func_0x0009470e(0x9471,uVar5);
        uVar11 = 1;
      }
      else {
        *(undefined2 *)0x690 = 0x540;
        func_0x00095f69(0x9471);
        func_0x0009470e(0x9471,uVar5);
        uVar11 = 0xffff;
      }
    }
  }
  else {
    *(undefined2 *)0x690 = 0x533;
    func_0x00095f69(uVar11);
    func_0x0009470e(0x9471,uVar5);
    uVar11 = 0xffff;
  }
  return uVar11;
}



/* validate_gpib_device_records (confidence: Mechanism confirmed at a high level; internal logic not
   exhaustively traced)
   
   Evidence: `(count, far ptr device_array)` - called during GPIB device-table setup (sibling checks
   in the same caller reset via `init_comm_device_table` on failure); large function (~1000 lines)
   that searches the caller-given device array for a matching/available entry, cross-referencing the
   `[0x73E]` GPIB record table (same one
   `validate_gpib_record_consistency`/`build_gpib_message_checksum` use, bound by the same `0x6800`
   range) and writing diagnostic codes to `[0x690]`; returns nonzero on failure - full internal
   logic not exhaustively traced given its size
   
   (physical 0x095B69, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

undefined2 __stdcall16far validate_gpib_device_records(word device_off,word device_seg,word count)

{
  int iVar1;
  undefined4 uVar2;
  undefined1 *puVar3;
  undefined1 *puVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  undefined2 uVar8;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 local_22;
  undefined4 local_20;
  undefined4 local_1c;
  uint local_12;
  int local_c;
  uint local_a;
  
  local_12 = 0;
  local_22 = 0;
  local_c = 0;
  do {
    if ((int)count <= local_c) {
LAB_0009_5d47:
      if (local_12 < 0x6801) {
        *(undefined2 *)0x690 = 0x5d1;
        *(uint *)((int)*(undefined4 *)0x746 + 8) = local_12;
        *(int *)((int)*(undefined4 *)0x746 + 6) = 0x6800 - local_12;
      }
      else {
        local_22 = 1;
      }
      return local_22;
    }
    iVar5 = local_c * 6;
    iVar1 = *(int *)(iVar5 + device_off + 2);
    if ((local_12 < *(uint *)(iVar5 + device_off)) && (*(char *)(iVar5 + device_off + 4) == '\0')) {
      *(undefined2 *)0x690 = 0x585;
      iVar6 = iVar1 * 0xc;
      iVar5 = *(int *)(iVar6 + (int)*(undefined4 *)0x73e + 2);
      uVar7 = iVar5 + *(int *)(iVar6 + (int)*(undefined4 *)0x73e + 6);
      local_1c = (undefined1 *)
                 CONCAT22((int)((ulong)*(undefined4 *)0x74a >> 0x10),
                          (undefined1 *)
                          ((int)*(undefined4 *)0x74a + *(int *)(iVar6 + (int)*(undefined4 *)0x73e)))
      ;
      local_20 = (undefined1 *)
                 CONCAT22((int)((ulong)*(undefined4 *)0x74a >> 0x10),
                          (undefined1 *)((int)*(undefined4 *)0x74a + local_12));
      if (0x6800 < local_12 + uVar7) {
        local_22 = 1;
        goto LAB_0009_5d47;
      }
      for (local_a = 0; puVar4 = local_1c, puVar3 = local_20, local_a < uVar7; local_a = local_a + 1
          ) {
        local_20 = (undefined1 *)CONCAT22(local_20._2_2_,(undefined1 *)local_20 + 1);
        local_1c = (undefined1 *)CONCAT22(local_1c._2_2_,(undefined1 *)local_1c + 1);
        *puVar3 = *puVar4;
      }
      *(uint *)(iVar1 * 0xc + (int)*(undefined4 *)0x73e) = local_12;
      *(int *)((int)*(undefined4 *)0x73e + iVar1 * 0xc + 4) = local_12 + iVar5;
      uVar2 = *(undefined4 *)0x74a;
      uVar8 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10);
      iVar6 = (int)*(undefined4 *)0x742;
      *(int *)(iVar6 + iVar1 * 0x4e + 0x36) = (int)uVar2 + local_12;
      *(undefined2 *)(iVar6 + iVar1 * 0x4e + 0x38) = (int)((ulong)uVar2 >> 0x10);
      uVar2 = *(undefined4 *)0x74a;
      uVar8 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10);
      iVar6 = (int)*(undefined4 *)0x742;
      *(int *)(iVar6 + iVar1 * 0x4e + 0x3c) = (int)uVar2 + local_12 + iVar5;
      *(undefined2 *)(iVar6 + iVar1 * 0x4e + 0x3e) = (int)((ulong)uVar2 >> 0x10);
      func_0x000950c2(unaff_CS,iVar1,local_12);
      local_12 = local_12 + uVar7;
      unaff_CS = 0x9471;
    }
    else if (*(char *)(local_c * 6 + device_off + 4) == '\0') {
      local_12 = local_12 +
                 *(int *)(iVar1 * 0xc + (int)*(undefined4 *)0x73e + 2) +
                 *(int *)(iVar1 * 0xc + (int)*(undefined4 *)0x73e + 6);
      if (0x6800 < local_12) {
        local_22 = 1;
        goto LAB_0009_5d47;
      }
    }
    else {
      *(undefined2 *)0x690 = 0x5aa;
      func_0x000961e9(unaff_CS,iVar1);
      unaff_CS = 0x9471;
      func_0x000950c2(0x9471,iVar1);
    }
    local_c = local_c + 1;
  } while( true );
}



void __stdcall16far FUN_0009_5d81(int param_1)

{
  int *piVar1;
  undefined4 uVar2;
  undefined1 *puVar3;
  undefined1 *puVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined2 uVar10;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  int local_1e;
  int local_16;
  int local_14;
  int local_12;
  undefined4 local_10;
  undefined4 local_c;
  
  *(undefined1 *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 10) = 1;
  iVar5 = param_1 * 0xc;
  iVar9 = *(int *)(iVar5 + (int)*(undefined4 *)0x73e + 2) +
          *(int *)(iVar5 + (int)*(undefined4 *)0x73e + 6);
  local_14 = *(int *)(iVar5 + (int)*(undefined4 *)0x73e);
  while (local_12 = local_14 + iVar9, local_14 + iVar9 < *(int *)((int)*(undefined4 *)0x746 + 8)) {
    local_16 = 0;
    while ((local_16 < 0x1a &&
           ((*(int *)((int)*(undefined4 *)0x73e + local_16 * 0xc) != local_12 ||
            ((*(byte *)(local_16 * 0xc + (int)*(undefined4 *)0x73e + 10) & 2) == 0))))) {
      local_16 = local_16 + 1;
    }
    local_10 = (undefined1 *)
               CONCAT22((int)((ulong)*(undefined4 *)0x74a >> 0x10),
                        (undefined1 *)((int)*(undefined4 *)0x74a + local_12));
    local_c = (undefined1 *)
              CONCAT22((int)((ulong)*(undefined4 *)0x74a >> 0x10),
                       (undefined1 *)((int)*(undefined4 *)0x74a + local_14));
    iVar5 = *(int *)(local_16 * 0xc + (int)*(undefined4 *)0x73e + 2);
    iVar6 = iVar5 + *(int *)(local_16 * 0xc + (int)*(undefined4 *)0x73e + 6);
    for (local_1e = 0; puVar4 = local_c, puVar3 = local_10, local_1e < iVar6;
        local_1e = local_1e + 1) {
      local_c = (undefined1 *)CONCAT22(local_c._2_2_,(undefined1 *)local_c + 1);
      local_10 = (undefined1 *)CONCAT22(local_10._2_2_,(undefined1 *)local_10 + 1);
      *puVar4 = *puVar3;
    }
    *(int *)(local_16 * 0xc + (int)*(undefined4 *)0x73e) = local_14;
    *(int *)(local_16 * 0xc + (int)*(undefined4 *)0x73e + 4) = local_14 + iVar5;
    uVar2 = *(undefined4 *)0x74a;
    uVar10 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10);
    iVar7 = (int)*(undefined4 *)0x742;
    *(int *)(iVar7 + local_16 * 0x4e + 0x36) = (int)uVar2 + local_14;
    *(undefined2 *)(iVar7 + local_16 * 0x4e + 0x38) = (int)((ulong)uVar2 >> 0x10);
    uVar2 = *(undefined4 *)0x74a;
    iVar7 = (int)uVar2;
    uVar10 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10);
    iVar8 = (int)*(undefined4 *)0x742;
    *(int *)(local_16 * 0x4e + iVar8 + 0x3c) = iVar7 + local_14 + iVar5;
    *(undefined2 *)(local_16 * 0x4e + iVar8 + 0x3e) = (int)((ulong)uVar2 >> 0x10);
    func_0x000950c2(unaff_CS,local_16,iVar7,local_14);
    unaff_CS = 0x9471;
    local_14 = local_14 + iVar6;
  }
  *(int *)((int)*(undefined4 *)0x746 + 8) = local_14;
  piVar1 = (int *)((int)*(undefined4 *)0x746 + 6);
  *piVar1 = *piVar1 + iVar9;
  func_0x000961e9(unaff_CS,param_1);
  return;
}



/* init_comm_device_table (confidence: Mechanism confirmed; exact per-record field meanings not
   confirmed)
   
   Evidence: Zeroes bytes 0-9 and sets byte `0xA=1` for each of 26 12-byte records at far pointer
   `[0x73E]`, then `memset_far`s a large block at `[0x742]` and clears fields at `[0x746]`
   
   (physical 0x095F69, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far init_comm_device_table(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  undefined2 local_a;
  
  uVar2 = 0x8f80;
  uVar1 = func_0x0009470e();
  for (local_a = 0; local_a < 0x1a; local_a = local_a + 1) {
    *(undefined2 *)(local_a * 0xc + (int)*(undefined4 *)0x73e) = 0;
    *(undefined2 *)(local_a * 0xc + (int)*(undefined4 *)0x73e + 2) = 0;
    *(undefined2 *)(local_a * 0xc + (int)*(undefined4 *)0x73e + 4) = 0;
    *(undefined2 *)(local_a * 0xc + (int)*(undefined4 *)0x73e + 6) = 0;
    *(undefined2 *)(local_a * 0xc + (int)*(undefined4 *)0x73e + 8) = 0;
    *(undefined1 *)(local_a * 0xc + (int)*(undefined4 *)0x73e + 10) = 1;
  }
  func_0x00096087(38000,(int)*(undefined4 *)0x742,(int)((ulong)*(undefined4 *)0x742 >> 0x10),0x7ec,0
                  ,uVar2);
  *(undefined2 *)*(undefined4 *)0x746 = 0;
  *(undefined2 *)((int)*(undefined4 *)0x746 + 2) = 0;
  *(undefined2 *)((int)*(undefined4 *)0x746 + 4) = 0;
  *(undefined2 *)((int)*(undefined4 *)0x746 + 6) = 0x6800;
  *(undefined2 *)((int)*(undefined4 *)0x746 + 8) = 0;
  func_0x0009470e(0x9471,uVar1);
  return;
}



/* checksum_bytes (confidence: Confirmed)
   
   Evidence: `(far ptr, count)` - sums `count` bytes into a byte accumulator (wrapping), returns it
   - a plain byte checksum
   
   (physical 0x09605A, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

char __stdcall16far checksum_bytes(word ptr_off,word ptr_seg,word count)

{
  char *pcVar1;
  undefined1 local_9;
  
  local_9 = '\0';
  while (pcVar1 = _ptr_off, 0 < (int)count) {
    _ptr_off = (char *)CONCAT22(ptr_seg,(char *)(ptr_off + 1));
    local_9 = local_9 + *pcVar1;
    count = count - 1;
  }
  return local_9;
}



/* memset_far (confidence: Confirmed)
   
   Evidence: `(far ptr, fill_byte, count)` - writes `fill_byte` to `count` consecutive bytes
   
   (physical 0x096087, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __stdcall16far memset_far(word dest_off,word dest_seg,word count,byte fill_byte)

{
  byte *pbVar1;
  
  while (pbVar1 = _dest_off, count = count - 1, 0 < (int)count) {
    _dest_off = (byte *)CONCAT22(dest_seg,(byte *)(dest_off + 1));
    *pbVar1 = fill_byte;
  }
  return;
}



void __stdcall16far FUN_0009_60ab(char *param_1)

{
  char *pcVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  int local_a;
  
  uVar5 = 0x8f80;
  uVar2 = func_0x0009470e();
  for (local_a = 0; pcVar1 = param_1, local_a < 0x1a; local_a = local_a + 1) {
    uVar4 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10);
    iVar3 = (int)*(undefined4 *)0x742 + local_a * 0x4e;
    param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
    *pcVar1 = (char)(*(int *)(iVar3 + 0x3a) + *(int *)(iVar3 + 0x40) >> 10) + '0';
  }
  func_0x0009470e(38000,uVar2,uVar5);
  return;
}



undefined2 __cdecl16far FUN_0009_6126(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 local_a;
  
  uVar4 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x000e5d2f(38000,uVar4);
  uVar3 = 0x8f80;
  uVar2 = func_0x0009470e(0xe5d1,0x8f80);
  if (((*(int *)((int)*(undefined4 *)0x746 + 2) == 0) &&
      (*(int *)((int)*(undefined4 *)0x746 + 4) == 0)) && (*(int *)*(undefined4 *)0x746 == 0)) {
    *(undefined2 *)0x690 = 0x6d9;
    local_a = 0;
    *(undefined2 *)*(undefined4 *)0x746 = 1;
  }
  else {
    *(undefined2 *)0x690 = 0x6e4;
    local_a = 1;
  }
  func_0x0009470e(38000,0x41,uVar3,uVar4,uVar2,uVar1);
  func_0x000e5d28(38000);
  func_0x0009470e(0xe5d1,0x8f80);
  return local_a;
}



void __stdcall16far FUN_0009_61af(undefined4 param_1)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  uVar2 = 0x8f80;
  uVar1 = func_0x0009470e();
  func_0x000fbc09(38000,(int)*(undefined4 *)0x746,(int)((ulong)*(undefined4 *)0x746 >> 0x10),
                  (int)param_1,(int)((ulong)param_1 >> 0x10),0xe,uVar2);
  func_0x0009470e(0xfbbe,uVar1);
  return;
}



/* init_gpib_record (confidence: Confirmed)
   
   Evidence: `(record_index)` - zeroes 5 word fields and sets a status byte to 1 within one 12-byte
   entry of the `[0x73E]` record table (the same table `build_gpib_message_checksum` reads a length
   field from)
   
   (physical 0x0961E9, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __stdcall16far init_gpib_record(word record_index)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)(record_index * 0xc + (int)*(undefined4 *)0x73e) = 0;
  *(undefined2 *)(record_index * 0xc + (int)*(undefined4 *)0x73e + 4) = 0;
  *(undefined2 *)(record_index * 0xc + (int)*(undefined4 *)0x73e + 2) = 0;
  *(undefined2 *)(record_index * 0xc + (int)*(undefined4 *)0x73e + 6) = 0;
  *(undefined2 *)(record_index * 0xc + (int)*(undefined4 *)0x73e + 8) = 0;
  *(undefined1 *)(record_index * 0xc + (int)*(undefined4 *)0x73e + 10) = 1;
  func_0x00096087();
  return;
}



/* comm_rom_boot_init (confidence: Confirmed)
   
   Evidence: Called exactly once, cross-ROM, from the main ROM's boot sequence (`0xE6D5A`), gated by
   `[0x1BF9]!=0` (the comm-option-installed result from `check_comm_option_installed`) - bootstraps
   the comm ROM via `init_far_pointer_table`, `init_comm_device_type_and_defaults`,
   `poll_dip_switch_change`
   
   (physical 0x09628C, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far comm_rom_boot_init(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x8f80;
  uVar1 = func_0x0009470e();
  func_0x00080133(38000,uVar2,uVar1);
  func_0x0009470e(0x8013,0x8f80);
  func_0x00085e66(38000);
  func_0x000962c2(0x85e6);
  func_0x0009470e(0x9628,uVar1);
  return;
}



/* poll_dip_switch_change (confidence: Mechanism confirmed; exact purpose of the change-detection
   not confirmed)
   
   Evidence: Toggles a strobe flag (`es:[0x6E2+3]`) bracketing two reads of the GPIB-config DIP
   switch byte (`[0x6DA]`), XORs them to detect changed bits, branches on bits `0x40`/`0x80`
   
   (physical 0x0962C2, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far poll_dip_switch_change(void)

{
  byte *pbVar1;
  byte bVar2;
  byte bVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  
  *(undefined1 *)((int)*(undefined4 *)0x6e2 + 3) = 0;
  bVar3 = *(byte *)*(undefined4 *)0x6da;
  *(undefined1 *)((int)*(undefined4 *)0x6e2 + 3) = 1;
  bVar3 = bVar3 ^ *(byte *)*(undefined4 *)0x6da;
  if (((bVar3 & 0x40) == 0) || ((bVar3 & 0x80) != 0)) {
    if (((bVar3 & 0x40) != 0) || ((bVar3 & 0x80) == 0)) {
      *(undefined1 *)((int)*(undefined4 *)0x732 + 0x95) = 0;
      *(undefined1 *)((int)*(undefined4 *)0x732 + 0x96) = 0;
      return;
    }
    *(undefined1 *)0x629 = 0xff;
    *(undefined1 *)((int)*(undefined4 *)0x732 + 0x96) = 0xff;
  }
  else {
    *(undefined1 *)0x629 = 0;
    *(undefined1 *)((int)*(undefined4 *)0x732 + 0x96) = 0;
  }
  *(undefined1 *)0x45a = 0;
  *(undefined1 *)0x3 = 0;
  *(undefined1 *)0x452 = 0;
  *(undefined1 *)0x453 = 0;
  *(undefined *)&DAT_0009_0500 = 0x80;
  *(undefined1 *)0xc = 0;
  if (*(char *)0x629 == '\0') {
    uVar4 = 0xffff;
    func_0x0008009b();
    *(undefined1 *)0x59c = 0;
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 2) = 0;
    *(undefined1 *)0x59a = 0;
    *(undefined1 *)0x59f = 0;
    *(undefined1 *)0x59b = 0x40;
    *(undefined1 *)0x45e = 0xff;
    uVar5 = 0x9628;
    func_0x000966e7(0x8006,uVar4);
    if (*(char *)0x4ec == '\x1f') {
      *(undefined1 *)*(undefined4 *)0x6e2 = 0;
      *(undefined1 *)0x597 = 0xff;
      *(undefined1 *)((int)*(undefined4 *)0x6e2 + 1) = 0;
      *(undefined1 *)0x596 = 0xff;
      *(undefined1 *)0x455 = 0xff;
    }
    else {
      *(undefined1 *)0x45d = 0;
      *(undefined1 *)0x45c = 0;
      *(byte *)0x59f = *(byte *)((uint)*(byte *)0x4ec + (int)*(undefined4 *)0x716) | 2;
      *(byte *)0x59a = *(byte *)((uint)*(byte *)0x4ed + (int)*(undefined4 *)0x71a) & 0x3f;
      *(undefined1 *)0x59c = 0x67;
      uVar5 = 0x96f5;
      func_0x00097b01(0x9628);
      *(undefined1 *)0x57f = 0x80;
      *(undefined *)&DAT_0009_0500 = 0x80;
      *(undefined1 *)0x580 = 0x80;
    }
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = *(undefined1 *)0x59f;
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 1) = *(undefined1 *)0x59a;
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 2) = *(undefined1 *)0x59c;
    uVar4 = 0x8006;
    func_0x0008009b(uVar5,0);
    *(undefined1 *)0x5a3 = 0;
  }
  else {
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 0x80;
    uVar4 = 0x9628;
    func_0x00096781();
    if ((*(byte *)0x4f0 & 0x1f) != 0x1f) {
      if ((*(byte *)0x4f1 & 0x40) == 0) {
        bVar3 = 0;
      }
      else {
        bVar3 = 0x40;
      }
      if ((*(byte *)0x4f1 & 0x80) == 0) {
        bVar2 = 0;
      }
      else {
        bVar2 = 0x20;
      }
      *(byte *)((int)*(undefined4 *)0x6d6 + 4) = *(byte *)0x4f0 & 0x1f | bVar2 | bVar3;
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 7;
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 0x84;
      *(undefined1 *)0x587 = 0;
      *(undefined1 *)0x580 = 0x80;
      *(undefined1 *)0x6 = 0x80;
      func_0x0008526b(0x9628);
      if ((*(byte *)0x4f1 & 0x80) == 0) {
        *(undefined1 *)0x6 = 1;
        func_0x0008526b(0x8511);
      }
      else {
        *(undefined1 *)0x61c = 0;
      }
      func_0x00097b01(0x8511);
      if ((*(byte *)0x4f1 & 0x20) != 0) {
        *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 0x83;
      }
      *(undefined1 *)*(undefined4 *)0x6d6 = 0x37;
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 1) = 0x8d;
      func_0x00080060(0x96f5,0);
      *(undefined1 *)0x5a3 = 0;
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 0;
      if ((*(byte *)0x4f1 & 0x80) != 0) {
        *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 0x89;
      }
      if ((*(byte *)0x4f1 & 0x40) != 0) {
        *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 0x8a;
        *(undefined1 *)0x5a3 = 0xff;
      }
      if ((*(byte *)((int)*(undefined4 *)0x6d6 + 2) & 2) == 0 &&
          (*(byte *)((int)*(undefined4 *)0x6d6 + 2) & 4) == 0) {
        pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
        *pbVar1 = *pbVar1 & 0xfd;
      }
      else {
        pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
        *pbVar1 = *pbVar1 | 10;
      }
      uVar4 = 0x82c9;
      func_0x000839d1(0x8006);
    }
  }
  func_0x00096800(uVar4);
  return;
}



/* init_comm_channel_state_a (confidence: Confirmed)
   
   Evidence: Resets default far pointers and counters/flags, sets the channel status byte
   `es:[0x6D6+3]` to 2 or 3; first half of a channel reinit pair
   
   (physical 0x096597, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far init_comm_channel_state_a(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x43c = 0xf;
  *(undefined2 *)0x43e = unaff_DS;
  *(undefined2 *)0x438 = 0xf;
  *(undefined2 *)0x43a = unaff_DS;
  *(undefined2 *)0x434 = 0xf;
  *(undefined2 *)0x436 = unaff_DS;
  *(undefined2 *)0x468 = 0x46c;
  *(undefined2 *)0x46a = unaff_DS;
  *(undefined2 *)0x464 = 0x46c;
  *(undefined2 *)0x466 = unaff_DS;
  *(undefined2 *)&LAB_0009_03b4 = 0;
  *(undefined2 *)0x446 = 0;
  *(undefined2 *)0x440 = 0;
  *(undefined2 *)0x442 = 0;
  *(undefined1 *)0x1 = 0;
  *(undefined1 *)0x0 = 0;
  *(undefined1 *)0x454 = 0;
  *(undefined1 *)0x3 = 0;
  *(undefined1 *)0x451 = 0;
  *(undefined1 *)0x2 = 0;
  if (*(char *)0x629 != '\0') {
    func_0x00096dbe();
    if ((*(byte *)0x4f1 & 0x20) == 0) {
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 3;
    }
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 2;
  }
  return;
}



/* init_comm_tx_queue_and_ready_flags (confidence: Confirmed)
   
   Evidence: Resets the tx ring buffer pointers (`[0x448]`/`[0x44C]`, base `0xAF` - same as
   `service_comm_tx_queue`) and sets the channel status byte or clears tx-ready depending on
   `[0x629]`; second half of the pair with `init_comm_channel_state_a`
   
   (physical 0x096634, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far init_comm_tx_queue_and_ready_flags(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x454 = 0;
  *(undefined1 *)&DAT_0009_03c9 = 0;
  *(undefined1 *)0x68f = 0;
  *(undefined1 *)0x455 = 0xff;
  *(undefined2 *)0x448 = 0xaf;
  *(undefined2 *)0x44a = unaff_DS;
  *(undefined2 *)0x44c = 0xaf;
  *(undefined2 *)0x44e = unaff_DS;
  *(undefined1 *)0x457 = 0;
  *(undefined1 *)0x456 = 0;
  *(undefined1 *)0x458 = 0;
  if (*(char *)0x629 == '\0') {
    *(undefined1 *)((int)*(undefined4 *)0x6e2 + 1) = 0;
    *(undefined1 *)0x596 = 0xff;
  }
  else {
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 5;
  }
  return;
}



/* reset_gpib_addr_scratch (confidence: Mechanism confirmed; exact scratch-buffer semantics not
   confirmed)
   
   Evidence: Clears the 10-byte scratch buffer at `[0x586]` used elsewhere for building a GPIB
   device-address list, conditionally calls `SUB_97905` when in GPIB mode and not device 1, sets
   `[0x581]=0x80`
   
   (physical 0x096696, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far reset_gpib_addr_scratch(void)

{
  int unaff_DS;
  
  *(undefined2 *)0x582 = 0x586;
  *(int *)0x584 = unaff_DS;
  while (((int)*(undefined4 *)0x582 != 0x590 ||
         ((int)((ulong)*(undefined4 *)0x582 >> 0x10) != unaff_DS))) {
    *(undefined1 *)*(undefined4 *)0x582 = 0;
    *(int *)0x582 = *(int *)0x582 + 1;
  }
  if ((*(char *)0x629 != '\0') && (*(char *)0x580 != '\x01')) {
    func_0x00097905();
  }
  *(undefined1 *)0x581 = 0x80;
  return;
}



/* read_dip_switches_serial_config (confidence: Confirmed as a real hardware switch reader - very
   likely the firmware side of the comm option board's physical "PARAMETERS" DIP switch bank (see
   `HARDWARE.md`); exact bit-to-setting mapping not fully confirmed)
   
   Evidence: Reads two hardware switch bytes via far pointers `[0x6DE]`/`[0x6DA]` (inverted,
   active-low) and decodes RS-232 settings: a baud-rate-like code into `[0x4EC]`, parity mode into
   `[0x4ED]`, a CR/LF option into `[0x4EF]`, and another config byte into `[0x461]`
   
   (physical 0x0966E7, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far read_dip_switches_serial_config(void)

{
  byte bVar1;
  byte bVar2;
  undefined2 unaff_DS;
  
  bVar1 = ~*(byte *)*(undefined4 *)0x6de;
  if ((bVar1 & 0xf) == 0xf) {
    bVar2 = 0x1f;
  }
  else {
    bVar2 = bVar1 & 0xf;
  }
  *(byte *)0x4ec = bVar2;
  if ((bVar1 & 0x10) == 0) {
    *(undefined1 *)0x4ed = 0;
  }
  else {
    *(char *)0x4ed = (char)((int)(bVar1 & 0x60) >> 5) + '\x01';
  }
  *(bool *)0x4ef = (*(byte *)*(undefined4 *)0x6da & 8) == 0;
  *(byte *)0x461 = (byte)((int)~(uint)*(byte *)*(undefined4 *)0x6da >> 4) & 3;
  return;
}



/* read_dip_switches_gpib_config (confidence: Confirmed as a real hardware switch reader; exact
   bit-to-setting mapping not fully confirmed)
   
   Evidence: Reads the same two switch bytes as `read_dip_switches_serial_config` but decodes GPIB
   fields instead: a 5-bit primary address (0-30) into `[0x4F0]`, mode flags into
   `[0x4F1]`/`[0x461]`
   
   (physical 0x096781, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far read_dip_switches_gpib_config(void)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  undefined2 unaff_DS;
  
  bVar1 = ~*(byte *)*(undefined4 *)0x6de;
  *(byte *)0x4f0 = bVar1 & 0x1f;
  if ((*(byte *)*(undefined4 *)0x6da & 8) == 0) {
    bVar2 = 0x40;
  }
  else {
    bVar2 = 0;
  }
  if ((bVar1 & 0x40) == 0) {
    bVar3 = 0;
  }
  else {
    bVar3 = 0x80;
  }
  if ((bVar1 & 0x20) == 0) {
    bVar1 = 0;
  }
  else {
    bVar1 = 0x20;
  }
  *(byte *)0x4f1 = bVar1 | bVar3 | bVar2;
  *(byte *)0x461 = (byte)((int)~(uint)*(byte *)*(undefined4 *)0x6da >> 4) & 3;
  return;
}



/* compute_parity_mode_code (confidence: Parity dispatch confirmed; the
   `scale_and_plot_point_default` calls are an open puzzle)
   
   Evidence: Dispatches on DIP-switch-derived `[0x461]` to produce a standard parity code stored
   into the config array at `es:[0x73A+0x20]`; also brackets the dispatch with 2 calls to
   `scale_and_plot_point_default` in a DS-switch-like argument/save/restore shape - see
   `docs/comm-rom/rs232-flow-control-and-open-puzzle.md` "Open puzzle"
   
   (physical 0x096800, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far compute_parity_mode_code(void)

{
  char cVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined1 local_b;
  
  uVar3 = 0x8f80;
  uVar2 = func_0x000f0078();
  cVar1 = *(char *)0x461;
  if (cVar1 == '\0') {
    local_b = 1;
  }
  else if (cVar1 == '\x01') {
    local_b = 4;
  }
  else if (cVar1 == '\x02') {
    local_b = 3;
    if ((*(char *)0x629 == '\0') && (*(char *)0x4ed != '\0')) {
      local_b = 2;
    }
  }
  else {
    local_b = 0;
  }
  *(undefined1 *)((int)*(undefined4 *)0x73a + 0x20) = local_b;
  func_0x000f0078(0xf007,uVar2,uVar3);
  return;
}



/* WARNING: Unknown calling convention -- yet parameter storage is locked */
/* spawn_task_with_tag (confidence: Confirmed)
   
   Evidence: `(tag)` - stores tag into the current task's per-task scratch byte at
   `[task_index+0x744]` (task_index from `[0x1ACD]`), then calls `create_task()`. **Corrected
   2026-09-15**: does not launch anything independent - traced its post-call continuation directly,
   and it's just "restore `DS`, return normally" straight back to its own caller
   (`serial_tx_buffer_put`). Real effect: tag the current task's own scratch byte, then insert one
   deliberate scheduling yield (presumably to let a higher-priority tick service something) before
   continuing the same call chain. Note: `[x+0x744]` is the same per-item table
   `update_plot_retry_counters` iterates - likely a general per-task scratch byte, not specifically
   a "plot retry counter"
   
   (physical 0x096872, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void spawn_task_with_tag(byte tag)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined1 in_stack_00000004;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  *(undefined1 *)(*(int *)0x1acd + 0x744) = in_stack_00000004;
  func_0x000e6224(38000,uVar2,uVar1);
  func_0x0009470e(0xe60b,uVar1);
  return;
}



void __cdecl16far FUN_0009_68a2(void)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  if (*(char *)0x451 == '\0') {
    uVar1 = *(undefined4 *)0x438;
    *(undefined2 *)0x43c = (int)uVar1;
    *(undefined2 *)0x43e = (int)((ulong)uVar1 >> 0x10);
  }
  else if ((((int)*(undefined4 *)0x438 != (int)*(undefined4 *)0x43c) ||
           ((int)((ulong)*(undefined4 *)0x438 >> 0x10) != (int)((ulong)*(undefined4 *)0x43c >> 0x10)
           )) || ((*(char *)0x3 == '\0' && (*(char *)0x1 == '\0')))) {
    *(undefined1 *)0x451 = 0;
    uVar1 = *(undefined4 *)0x438;
    *(undefined2 *)0x43c = (int)uVar1;
    *(undefined2 *)0x43e = (int)((ulong)uVar1 >> 0x10);
    if ((*(char *)0x629 != '\0') && (*(int *)&LAB_0009_03b4 == 0 && *(int *)0x446 == 0)) {
      if ((*(byte *)0x4f1 & 0x20) == 0) {
        *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 3;
      }
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 2;
    }
  }
  if ((*(char *)0x629 == '\0') && ((*(byte *)0x45c & 1) != 0)) {
    if ((*(uint *)0x434 < *(uint *)0x43c) || (*(char *)0x451 != '\0')) {
      iVar2 = (*(int *)0x434 - *(int *)0x43c) + 0xa0;
    }
    else {
      iVar2 = *(int *)0x434 - *(int *)0x43c;
    }
    if (iVar2 < 0x28) {
      *(undefined1 *)0x5a3 = 0xff;
      uVar3 = 1;
      func_0x00097c28();
      func_0x00097b94(0x96f5,0,uVar3);
    }
  }
  return;
}



void __cdecl16far FUN_0009_699b(void)

{
  undefined1 *puVar1;
  undefined2 unaff_CS;
  int unaff_DS;
  
  if ((*(char *)0x2 != '\0') && (*(char *)0x3 == '\0')) {
    return;
  }
  *(undefined1 *)0x5a3 = 0xff;
  *(undefined1 *)0x2 = 0;
  while (*(char *)0x3 == '\0') {
    if (*(char *)0x451 == '\0') {
      func_0x00096e5f(unaff_CS);
      func_0x00097b94(0x9687,0);
      unaff_CS = 0x9687;
      func_0x00096872(0x96f5,10);
      *(undefined1 *)0x5a3 = 0xff;
    }
    else {
      *(undefined1 *)0x623 = 0x19;
      *(undefined1 *)0x6 = 0x19;
      func_0x0008526b(unaff_CS);
      unaff_CS = 0x9687;
      func_0x000968a2(0x8511);
    }
  }
  if (*(char *)0x3 != '\0') {
    puVar1 = (undefined1 *)*(undefined4 *)0x438;
    *(int *)0x438 = *(int *)0x438 + 1;
    *(undefined1 *)0x4 = *puVar1;
    if (((int)*(undefined4 *)0x438 == 0xaf) &&
       ((int)((ulong)*(undefined4 *)0x438 >> 0x10) == unaff_DS)) {
      *(undefined2 *)0x438 = 0xf;
      *(int *)0x43a = unaff_DS;
    }
    if (((int)*(undefined4 *)0x438 != (int)*(undefined4 *)0x434) ||
       ((int)((ulong)*(undefined4 *)0x438 >> 0x10) != (int)((ulong)*(undefined4 *)0x434 >> 0x10))) {
      if (((int)*(undefined4 *)0x438 != (int)*(undefined4 *)0x440) ||
         ((int)((ulong)*(undefined4 *)0x438 >> 0x10) != (int)((ulong)*(undefined4 *)0x440 >> 0x10)))
      goto LAB_0009_6a64;
    }
    *(undefined1 *)0x3 = 0;
  }
LAB_0009_6a64:
  if (((int)*(undefined4 *)0x438 == (int)*(undefined4 *)0x440) &&
     ((int)((ulong)*(undefined4 *)0x438 >> 0x10) == (int)((ulong)*(undefined4 *)0x440 >> 0x10))) {
    *(undefined1 *)0x2 = 0xff;
  }
  func_0x00097b94(unaff_CS,0);
  return;
}



void __stdcall16far FUN_0009_6a89(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x629 == '\0') {
    func_0x0008009b();
  }
  else {
    func_0x00080060();
  }
  return;
}



void __cdecl16far FUN_0009_6aaf(void)

{
  undefined4 uVar1;
  undefined1 uVar2;
  undefined2 unaff_DS;
  
  func_0x000968a2();
  *(undefined1 *)0x5a3 = 0xff;
  uVar1 = *(undefined4 *)&LAB_0009_03b4;
  *(undefined2 *)0x440 = (int)uVar1;
  *(undefined2 *)0x442 = (int)((ulong)uVar1 >> 0x10);
  *(undefined1 *)0x452 = *(undefined1 *)0x453;
  *(undefined2 *)&LAB_0009_03b4 = 0;
  *(undefined2 *)0x446 = 0;
  *(undefined1 *)0x453 = 0;
  if (*(char *)0x629 == '\0') {
    if (((int)*(undefined4 *)0x434 == (int)*(undefined4 *)0x43c) &&
       ((int)((ulong)*(undefined4 *)0x434 >> 0x10) == (int)((ulong)*(undefined4 *)0x43c >> 0x10))) {
      uVar2 = 0;
    }
    else {
      uVar2 = 1;
    }
  }
  else {
    uVar2 = *(undefined1 *)0x1;
  }
  *(undefined1 *)0x0 = uVar2;
  *(undefined1 *)0x3 = uVar2;
  func_0x00096dbe(0x9687);
  *(undefined1 *)0x1 = 0;
  *(undefined1 *)0x454 = 0;
  if (((*(char *)0x629 != '\0') && (*(char *)0x451 == '\0')) &&
     (*(int *)0x440 != 0 || *(int *)0x442 != 0)) {
    if ((*(byte *)0x4f1 & 0x20) == 0) {
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 3;
    }
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 2;
  }
  func_0x00097b94(0x9687,0);
  return;
}



/* serial_tx_buffer_put (confidence: Confirmed mechanism (ring buffer + wraparound); "transmit
   buffer" identity inferred from context, not confirmed against a schematic)
   
   Evidence: Writes a byte into a ring buffer at `[0x44C]`, wrapping at a fixed boundary (`0x433`
   bytes); the byte's source depends on mode flags `[0x459]`/`[0x629]` - likely the GPIB/RS-232
   transmit buffer
   
   (physical 0x096B68, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far serial_tx_buffer_put(void)

{
  undefined1 uVar1;
  uint uVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  undefined2 uVar4;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x5a3 = 0xff;
  if (*(char *)0xc == '\0') {
    while ((*(char *)0x455 == '\0' && (*(char *)0x454 == '\0'))) {
      if (((int)*(undefined4 *)0x44c != (int)*(undefined4 *)0x448) ||
         ((int)((ulong)*(undefined4 *)0x44c >> 0x10) != (int)((ulong)*(undefined4 *)0x448 >> 0x10)))
      break;
      func_0x00096e5f(unaff_CS);
      func_0x00097b94(0x9687,0);
      unaff_CS = 0x9687;
      func_0x00096872(0x96f5,10);
      *(undefined1 *)0x5a3 = 0xff;
    }
    if (*(char *)0x454 == '\0') {
      uVar4 = unaff_CS;
      if ((*(char *)&DAT_0009_03c9 == '\0') && (*(char *)0x629 != '\0')) {
        *(undefined *)&DAT_0009_03c9 = 0xff;
      }
      else {
        if (*(char *)0x629 == '\0') {
          uVar1 = *(undefined1 *)0x5;
        }
        else {
          uVar1 = *(undefined1 *)0x450;
        }
        *(undefined1 *)*(undefined4 *)0x44c = uVar1;
        uVar3 = (undefined2)((ulong)*(undefined4 *)0x44c >> 0x10);
        uVar2 = (int)*(undefined4 *)0x44c + 1;
        if (0x432 < uVar2) {
          uVar2 = 0xaf;
          uVar3 = unaff_DS;
        }
        *(uint *)0x44c = uVar2;
        *(undefined2 *)0x44e = uVar3;
        *(undefined1 *)0x455 = 0;
        *(undefined *)&DAT_0009_03c9 = 0xff;
        if (*(char *)0x45a != '\0') {
          if (*(char *)0x629 == '\0') {
            uVar4 = 0x96f5;
            func_0x00097431(unaff_CS);
          }
          else {
            *(undefined1 *)0x68f = 0xff;
          }
        }
      }
      *(undefined1 *)0x450 = *(undefined1 *)0x5;
      unaff_CS = uVar4;
    }
  }
  func_0x00097b94(unaff_CS,0);
  return;
}



/* send_response_terminator (confidence: Confirmed)
   
   Evidence: If `[0x459]` set, sends CR+LF (directly for GPIB, or via `send_serial_newline` for
   RS-232), marks completion flags, calls `set_comm_critical_flag(0)`
   
   (physical 0x096C5F, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far send_response_terminator(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (*(char *)&DAT_0009_03c9 != '\0') {
    if ((*(char *)0x629 == '\0') || ((*(byte *)0x4f1 & 0x20) == 0)) {
      if (*(char *)0x629 == '\0') {
        unaff_CS = 0x9687;
        func_0x00096cb5();
      }
    }
    else {
      *(undefined1 *)0x5 = 0xd;
      func_0x00096b68();
      *(undefined1 *)0x5 = 10;
      unaff_CS = 0x9687;
      func_0x00096b68(0x9687);
    }
    *(undefined1 *)0x5a3 = 0xff;
    *(undefined1 *)0x456 = 0xff;
    func_0x00097b94(unaff_CS,0);
  }
  return;
}



/* send_serial_newline (confidence: Confirmed)
   
   Evidence: Sends CR (`0xD`) via `serial_tx_buffer_put`, and also LF (`0xA`) afterward if `[0x629]`
   is clear and `[0x4EF]` (a CR+LF line-ending option) is set; otherwise CR only
   
   (physical 0x096CB5, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far send_serial_newline(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if ((*(char *)0x629 == '\0') && (*(char *)0x4ef != '\0')) {
    *(undefined1 *)0x5 = 0xd;
    unaff_CS = 0x9687;
    func_0x00096b68();
    *(undefined1 *)0x5 = 10;
  }
  else {
    *(undefined1 *)0x5 = 0xd;
  }
  func_0x00096b68(unaff_CS);
  return;
}



void __cdecl16far FUN_0009_6ce9(void)

{
  undefined4 uVar1;
  undefined2 unaff_DS;
  
  if ((((int)*(undefined4 *)0x438 != (int)*(undefined4 *)0x43c) ||
      ((int)((ulong)*(undefined4 *)0x438 >> 0x10) != (int)((ulong)*(undefined4 *)0x43c >> 0x10))) ||
     (*(char *)0x451 != '\0')) {
    uVar1 = *(undefined4 *)0x43c;
    *(undefined2 *)0x438 = (int)uVar1;
    *(undefined2 *)0x43a = (int)((ulong)uVar1 >> 0x10);
    *(undefined1 *)0x3 = 0xff;
    *(undefined1 *)0x2 = 0;
  }
  return;
}



void __cdecl16far FUN_0009_6d24(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x5a3 = 0xff;
  if (((*(char *)0x629 != '\0') && ((*(byte *)((int)*(undefined4 *)0x6d6 + 2) & 0x80) != 0)) ||
     ((*(char *)0x629 == '\0' && ((*(byte *)0x627 & 0x80) != 0)))) {
    if (*(char *)0x629 == '\0') {
      if ((*(byte *)0x627 & 0x40) == 0) {
        *(byte *)0x627 = *(byte *)0x627 & 0x7f;
      }
    }
    else {
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 7;
    }
    if (((*(char *)0x629 != '\0') && ((*(byte *)((int)*(undefined4 *)0x6d6 + 2) & 0x80) == 0)) ||
       ((*(char *)0x629 == '\0' && ((*(byte *)0x627 & 0x80) == 0)))) {
      *(undefined1 *)0x452 = 0;
      *(undefined1 *)0x453 = 0;
      *(undefined1 *)0x7 = 0xff;
      *(undefined1 *)0xe = 0xff;
    }
  }
  func_0x00097b94();
  return;
}



/* sync_rs232_control_bit (confidence: Confirmed)
   
   Evidence: When the RS-232 strap `[0x629]` is set, folds bit `0x10` from `[0]` into `[0x57F]`
   (keeping other bits, clearing bit `0x40`) and writes the result to the UART-like register pair at
   far ptr `[0x6D6]+5` - the same register family `parse_next_gpib_command_byte` writes `[+3]` of
   with tag `0x98`; called from `init_comm_channel_state_a`
   
   (physical 0x096DBE, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far sync_rs232_control_bit(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x629 != '\0') {
    *(byte *)((int)*(undefined4 *)0x6d6 + 5) = *(byte *)0x57f & 0xbf | *(byte *)0x0 & 0x10;
  }
  return;
}



void __cdecl16far FUN_0009_6de5(void)

{
  undefined1 uVar1;
  undefined4 uVar2;
  undefined2 unaff_DS;
  
  func_0x000968a2();
  uVar1 = *(undefined1 *)0x5a3;
  *(undefined1 *)0x5a3 = 0xff;
  *(undefined1 *)0x3 = *(undefined1 *)0x1;
  *(undefined1 *)0x0 = *(undefined1 *)0x1;
  uVar2 = *(undefined4 *)&LAB_0009_03b4;
  *(undefined2 *)0x440 = (int)uVar2;
  *(undefined2 *)0x442 = (int)((ulong)uVar2 >> 0x10);
  *(undefined1 *)0x452 = *(undefined1 *)0x453;
  *(undefined2 *)&LAB_0009_03b4 = 0;
  *(undefined2 *)0x446 = 0;
  *(undefined1 *)0x1 = 0;
  *(undefined1 *)0x2 = 0;
  if (((*(char *)0x629 != '\0') && (*(char *)0x451 == '\0')) &&
     (*(int *)0x440 != 0 || *(int *)0x442 != 0)) {
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 2;
  }
  func_0x00097b94(0x9687,uVar1);
  return;
}



/* finish_comm_response (confidence: Mechanism confirmed; exact protocol role of the `0xFF` marker
   not confirmed)
   
   Evidence: Checks a channel-ready bit; if set, sends a `0xFF` marker byte then calls
   `send_response_terminator`; if data was left pending, sets busy flags and calls
   `set_comm_critical_flag(0)`
   
   (physical 0x096E5F, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far finish_comm_response(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if ((*(byte *)((int)*(undefined4 *)0x6d6 + 2) & 2) != 0) {
    if (((*(char *)&DAT_0009_03c9 == '\0') && (*(char *)0x0 == '\0')) &&
       ((*(byte *)0x4f1 & 0x40) == 0)) {
      *(undefined1 *)0x5 = 0xff;
      func_0x00096b68();
      unaff_CS = 0x9687;
      func_0x00096c5f(0x9687);
    }
    if ((*(char *)0x45a != '\0') && ((*(char *)0x455 == '\0' || (*(char *)0x456 != '\0')))) {
      *(undefined1 *)0x5a3 = 0xff;
      *(undefined1 *)0x68f = 0xff;
      func_0x00097b94(unaff_CS,0);
    }
  }
  return;
}



/* release_comm_hold_critical (confidence: Confirmed)
   
   Evidence: RS-232-only wrapper: engages `set_comm_critical_flag(0xFFFF)`, calls
   `release_comm_hold(2)`, restores the critical flag
   
   (physical 0x096EC6, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far release_comm_hold_critical(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  uVar3 = 0x8f80;
  uVar2 = func_0x0009470e();
  if (*(char *)0x629 == '\0') {
    uVar1 = func_0x00097b94(38000,0xffff,uVar3);
    func_0x00097c28(0x96f5,2);
    func_0x00097b94(0x96f5,uVar1);
    func_0x0009470e(0x96f5,uVar2);
  }
  return;
}



/* engage_comm_hold_critical (confidence: Confirmed)
   
   Evidence: Mirror of `release_comm_hold_critical`, calling `engage_comm_hold(2)` instead
   
   (physical 0x096F0D, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far engage_comm_hold_critical(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  uVar3 = 0x8f80;
  uVar2 = func_0x0009470e();
  if (*(char *)0x629 == '\0') {
    uVar1 = func_0x00097b94(38000,0xffff,uVar3);
    func_0x00097be6(0x96f5,2);
    func_0x00097b94(0x96f5,uVar1);
    func_0x0009470e(0x96f5,uVar2);
  }
  return;
}



int __cdecl16far FUN_0009_6f54(void)

{
  byte *pbVar1;
  byte bVar2;
  undefined1 uVar3;
  undefined2 uVar4;
  byte bVar5;
  uint uVar6;
  undefined2 uVar7;
  undefined2 unaff_DS;
  undefined2 uVar8;
  char local_f;
  
  uVar8 = 0x8f80;
  uVar7 = 38000;
  uVar4 = func_0x0009470e();
  local_f = '\0';
  if (*(char *)0x629 == '\0') {
    bVar2 = *(byte *)*(undefined4 *)0x6da;
    bVar5 = bVar2 & 0x80;
    if (*(byte *)0x45e != bVar5) {
      *(byte *)0x45e = bVar5;
      if (bVar5 == 0) {
        pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
        *pbVar1 = *pbVar1 | 10;
      }
      else {
        pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
        *pbVar1 = *pbVar1 & 0xfd;
      }
      uVar7 = 0x82c9;
      func_0x000839d1(38000,uVar8);
    }
    if ((bVar2 & 2) != 0) {
      uVar3 = *(undefined1 *)((int)*(undefined4 *)0x6d6 + 1);
      *(undefined1 *)0x59b = uVar3;
      *(undefined1 *)((int)*(undefined4 *)0x464 + 1) = uVar3;
      uVar3 = *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3);
      *(undefined1 *)0x59d = uVar3;
      *(undefined1 *)((int)*(undefined4 *)0x464 + 2) = uVar3;
      uVar3 = *(undefined1 *)*(undefined4 *)0x6d6;
      *(undefined1 *)0x59e = uVar3;
      *(undefined1 *)*(undefined4 *)0x464 = uVar3;
      *(byte *)((int)*(undefined4 *)0x464 + 3) = bVar2;
      uVar8 = (undefined2)((ulong)*(undefined4 *)0x464 >> 0x10);
      uVar6 = (int)*(undefined4 *)0x464 + 4;
      if (0x4eb < uVar6) {
        uVar6 = 0x46c;
        uVar8 = unaff_DS;
      }
      *(uint *)0x464 = uVar6;
      *(undefined2 *)0x466 = uVar8;
      local_f = -1;
    }
    if (*(char *)0x596 == '\0') {
      if ((bVar2 & 4) != 0) {
        func_0x000800fc(uVar7,0xffff);
        local_f = -1;
        uVar7 = 0x8006;
      }
    }
    else if (((*(char *)0x45a == '\0') && (*(char *)0x5a3 == '\0')) &&
            ((*(byte *)0x1a96 & 0xf) == 0)) {
      local_f = -1;
    }
    if (local_f != '\0') {
      *(undefined1 *)0x5a1 = 0xff;
    }
  }
  else if (*(char *)0x5a1 == '\0') {
    func_0x00080060(38000,0xffff);
    bVar2 = *(byte *)*(undefined4 *)0x6d6;
    if ((bVar2 & 0xc0) == 0) {
      if ((*(char *)0x68f == '\0') || (*(char *)0x45a == '\0')) {
        func_0x00080060(0x8006,0);
      }
      else {
        local_f = -1;
        *(undefined1 *)0x598 = 0;
      }
    }
    else {
      local_f = -1;
      *(byte *)0x598 = bVar2;
      if ((bVar2 & 0x40) != 0) {
        *(undefined1 *)0x599 = *(undefined1 *)((int)*(undefined4 *)0x6d6 + 1);
      }
    }
    uVar7 = 0x8006;
  }
  func_0x0009470e(uVar7,uVar4);
  return (int)local_f;
}



void __cdecl16far FUN_0009_70f9(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  uVar2 = 0x8f80;
  uVar1 = func_0x0009470e();
  if (*(char *)0x629 == '\0') {
    func_0x0009712b(38000,uVar2,uVar1);
  }
  else {
    func_0x000975ac(38000);
  }
  func_0x0009470e(0x96f5,uVar1);
  return;
}



void __cdecl16far FUN_0009_712b(void)

{
  byte bVar1;
  undefined2 uVar2;
  uint uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  undefined2 uVar6;
  byte local_f;
  
  uVar6 = 0x8f80;
  uVar2 = func_0x0009470e();
  uVar4 = 38000;
  while (uVar5 = uVar4, *(char *)0x5a3 != '\0') {
    if (((((int)*(undefined4 *)0x464 == (int)*(undefined4 *)0x468) &&
         ((int)((ulong)*(undefined4 *)0x464 >> 0x10) == (int)((ulong)*(undefined4 *)0x468 >> 0x10)))
        && ((*(char *)0x596 == '\0' || (*(char *)0x45a != '\0')))) && (*(char *)0x456 == '\0'))
    goto LAB_0009_7242;
    func_0x000838c0(uVar4,uVar6);
    uVar4 = 0x82c9;
  }
  while( true ) {
    if (((int)*(undefined4 *)0x464 == (int)*(undefined4 *)0x468) &&
       ((int)((ulong)*(undefined4 *)0x464 >> 0x10) == (int)((ulong)*(undefined4 *)0x468 >> 0x10)))
    break;
    bVar1 = *(byte *)((int)*(undefined4 *)0x468 + 1);
    local_f = bVar1 & 0x9f;
    if (local_f != 0) {
      uVar4 = uVar5;
      if ((bVar1 & 0xf) != 0) {
        uVar4 = 0x96f5;
        local_f = func_0x0009754c(uVar5,local_f);
      }
      uVar6 = uVar4;
      if ((local_f & 0x80) != 0) {
        uVar6 = 0x96f5;
        func_0x00097253(uVar4);
      }
      uVar5 = uVar6;
      if ((local_f & 0x10) != 0) {
        uVar5 = 0x96f5;
        func_0x00097583(uVar6);
      }
    }
    if (((int)*(undefined4 *)0x464 != (int)*(undefined4 *)0x468) ||
       ((int)((ulong)*(undefined4 *)0x464 >> 0x10) != (int)((ulong)*(undefined4 *)0x468 >> 0x10))) {
      uVar4 = (undefined2)((ulong)*(undefined4 *)0x468 >> 0x10);
      uVar3 = (int)*(undefined4 *)0x468 + 4;
      if (0x4eb < uVar3) {
        uVar3 = 0x46c;
        uVar4 = unaff_DS;
      }
      *(uint *)0x468 = uVar3;
      *(undefined2 *)0x46a = uVar4;
    }
  }
  if (((*(char *)0x596 != '\0') && (*(char *)0x45a == '\0')) ||
     ((uVar4 = uVar5, *(char *)0x455 != '\0' && (*(char *)0x456 != '\0')))) {
    uVar4 = 0x96f5;
    func_0x00097431(uVar5);
  }
LAB_0009_7242:
  *(undefined1 *)0x5a1 = 0;
  func_0x0009470e(uVar4,uVar2);
  return;
}



void __cdecl16far FUN_0009_7253(void)

{
  int iVar1;
  undefined2 unaff_CS;
  int unaff_DS;
  undefined2 uVar2;
  int local_10;
  int local_e;
  byte local_9;
  
  if (*(byte *)0x4ec < 0x28) {
    uVar2 = 0x41;
    func_0x0009470e();
    *(undefined1 *)0x7aa = 0x40;
    unaff_CS = 38000;
    func_0x0009470e(38000,0x8f80,uVar2);
  }
  if ((*(byte *)((int)*(undefined4 *)0x468 + 2) & 2) == 0) {
    return;
  }
  if (((*(byte *)((int)*(undefined4 *)0x468 + 3) ^ 0xff) & 0x80) == 0) {
    return;
  }
  local_9 = *(byte *)*(undefined4 *)0x468;
  if (*(char *)0x4ed != '\0') {
    if (*(char *)0x4ed == '\x04') {
      if ((local_9 & 0x80) != 0) {
        return;
      }
    }
    else if ((*(char *)0x4ed == '\x03') && ((local_9 & 0x80) == 0)) {
      return;
    }
    local_9 = local_9 & 0x7f;
  }
  *(byte *)*(undefined4 *)0x434 = local_9;
  if (*(char *)0x45b != '\0') {
    if (local_9 == 4) {
      func_0x00097b01(unaff_CS);
      return;
    }
    if (local_9 == 0x11) {
      *(undefined1 *)0x45d = 0;
      if (*(char *)0x45a == '\0') {
        return;
      }
      func_0x00097431(unaff_CS);
      return;
    }
    if (local_9 == 0x13) {
      *(undefined1 *)0x45d = 0xff;
      return;
    }
  }
  if (*(char *)0x0 == '\0') {
    *(undefined1 *)0x0 = 0xff;
    *(byte *)0x627 = *(byte *)0x627 & 0xdf;
    func_0x00096634(unaff_CS);
    *(byte *)0x452 = *(byte *)0x627 & 0x80;
    unaff_CS = 0x9628;
  }
  else if (((int)*(undefined4 *)0x440 == (int)*(undefined4 *)0x434) &&
          ((int)((ulong)*(undefined4 *)0x440 >> 0x10) == (int)((ulong)*(undefined4 *)0x434 >> 0x10))
          ) {
    *(undefined1 *)0x1 = 0xff;
    *(byte *)0x453 = *(byte *)0x627 & 0x80;
    *(undefined1 *)0x454 = 0xff;
  }
  iVar1 = (int)((ulong)*(undefined4 *)0x434 >> 0x10);
  local_e = (int)*(undefined4 *)0x434 + 1;
  if ((local_e == 0xaf) && (iVar1 == unaff_DS)) {
    local_e = 0xf;
    iVar1 = unaff_DS;
  }
  if (((local_9 == 0xd) && (*(char *)0x4ef == '\0')) ||
     ((local_9 == 10 && (*(char *)0x4ef != '\0')))) {
    if (*(int *)0x440 == 0 && *(int *)0x442 == 0) {
      *(int *)0x440 = local_e;
      *(int *)0x442 = iVar1;
    }
    else {
      *(int *)&LAB_0009_03b4 = local_e;
      *(int *)0x446 = iVar1;
    }
  }
  *(byte *)&LAB_0009_03d2 = local_9;
  *(int *)0x434 = local_e;
  *(int *)0x436 = iVar1;
  local_10 = *(int *)0x434 - *(int *)0x43c;
  if ((local_10 < 0) || (*(char *)0x451 != '\0')) {
    local_10 = local_10 + 0xa0;
  }
  if (0x82 < local_10) {
    func_0x00097be6(unaff_CS,1);
  }
  if (((int)*(undefined4 *)0x434 == (int)*(undefined4 *)0x43c) &&
     ((int)((ulong)*(undefined4 *)0x434 >> 0x10) == (int)((ulong)*(undefined4 *)0x43c >> 0x10))) {
    *(undefined1 *)0x451 = 0xff;
    *(undefined1 *)0x68c = 0x19;
  }
  *(undefined1 *)0x3 = 0xff;
  return;
}



/* service_comm_tx_queue (confidence: Confirmed)
   
   Evidence: The consumer side of the tx ring buffer `serial_tx_buffer_put` produces into
   (`[0x448]`/`[0x44A]` read ptr, base `0xAF`, size `0x384`, wrapping); pulls one byte, forwards a
   pending XON/XOFF byte or the next queued outgoing byte via `enqueue_comm_char`, sets `[0x455]`
   once caught up to the write pointer `[0x44C]`
   
   (physical 0x097431, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far service_comm_tx_queue(void)

{
  undefined1 uVar1;
  uint uVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x45a = 0xff;
  if ((*(char *)0x45d == '\0') && ((*(byte *)((int)*(undefined4 *)0x6d6 + 3) & 2) != 0)) {
    if (*(char *)0x460 == '\0') {
      if (*(char *)0x455 == '\0') {
        func_0x000974e1();
        uVar3 = (undefined2)((ulong)*(undefined4 *)0x448 >> 0x10);
        uVar2 = (int)*(undefined4 *)0x448 + 1;
        if (0x432 < uVar2) {
          uVar2 = 0xaf;
          uVar3 = unaff_DS;
        }
        *(uint *)0x448 = uVar2;
        *(undefined2 *)0x44a = uVar3;
        if (((int)*(undefined4 *)0x448 == (int)*(undefined4 *)0x44c) &&
           ((int)((ulong)*(undefined4 *)0x448 >> 0x10) == (int)((ulong)*(undefined4 *)0x44c >> 0x10)
           )) {
          *(undefined1 *)0x455 = 0xff;
        }
      }
      else {
        if (*(char *)0x456 != '\0') {
          *(undefined1 *)0x456 = 0;
        }
        *(undefined1 *)0x45a = 0xff;
      }
    }
    else {
      uVar1 = func_0x0009751a();
      func_0x000974e1(0x96f5,uVar1);
    }
  }
  return;
}



/* enqueue_comm_char (confidence: Confirmed)
   
   Evidence: Applies the parity mode in `[0x4ED]` to the argument byte (0=unchanged, else strip bit
   7, and force bit 7 set if mode==3 - space/mark parity), stores it into the tx path at `[0x6D6]`,
   clears `[0x45A]`, calls `SUB_800FC(0)`
   
   (physical 0x0974E1, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __stdcall16far enqueue_comm_char(byte param_1)

{
  undefined2 unaff_DS;
  
  if ((*(char *)0x4ed != '\0') && (param_1 = param_1 & 0x7f, *(char *)0x4ed == '\x03')) {
    param_1 = param_1 | 0x80;
  }
  *(byte *)*(undefined4 *)0x6d6 = param_1;
  *(undefined1 *)0x45a = 0;
  func_0x000800fc();
  return;
}



/* get_xon_xoff_byte (confidence: Confirmed - matches standard RS-232 software flow control codes)
   
   Evidence: Checks `[0x460]` flow-control-request bits: bit 2 -> clears it, returns `AL=0x13`
   (XOFF); bit 1 -> clears it, returns `AL=0x11` (XON); else `AL=0`
   
   (physical 0x09751A, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

undefined1 __cdecl16far get_xon_xoff_byte(void)

{
  undefined1 uVar1;
  undefined2 unaff_DS;
  
  if ((*(byte *)0x460 & 2) == 0) {
    if ((*(byte *)0x460 & 1) == 0) {
      uVar1 = 0;
    }
    else {
      *(byte *)0x460 = *(byte *)0x460 & 0xfe;
      uVar1 = 0x11;
    }
  }
  else {
    *(byte *)0x460 = *(byte *)0x460 & 0xfd;
    uVar1 = 0x13;
  }
  return uVar1;
}



byte __stdcall16far FUN_0009_754c(byte param_1)

{
  undefined2 unaff_DS;
  
  if ((param_1 & 2) == 0) {
    if ((param_1 & 4) != 0) {
      *(undefined1 *)0x68c = 0x19;
    }
  }
  else {
    *(undefined1 *)0x68c = 0x2a;
  }
  return param_1 & 0x70;
}



void __cdecl16far FUN_0009_7583(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x45a != '\0') {
    func_0x00097431();
  }
  return;
}



void __cdecl16far FUN_0009_75ac(void)

{
  byte bVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  
  uVar5 = 0x8f80;
  uVar2 = func_0x0009470e();
  uVar3 = 38000;
  while ((*(char *)0x5a3 != '\0' && (*(char *)0x5a1 != '\0'))) {
    func_0x000838c0(uVar3,uVar5);
    uVar3 = 0x82c9;
  }
  if (*(char *)0x5a1 != '\0') {
    while ((*(byte *)0x598 & 0xc0) != 0) {
      if ((*(byte *)0x598 & 0x80) != 0) {
        uVar5 = uVar3;
        if ((*(byte *)0x598 & 0x20) != 0) {
          uVar5 = 0x96f5;
          func_0x000976c0(uVar3);
        }
        uVar3 = uVar5;
        if ((*(byte *)0x598 & 0x10) != 0) {
          uVar3 = 0x96f5;
          func_0x0009785b(uVar5);
        }
        uVar5 = uVar3;
        if ((*(byte *)0x598 & 4) != 0) {
          uVar5 = 0x96f5;
          func_0x00097905(uVar3);
        }
        uVar4 = uVar5;
        if ((*(byte *)0x598 & 2) != 0) {
          uVar4 = 0x96f5;
          func_0x00097a16(uVar5);
        }
        uVar3 = uVar4;
        if ((*(byte *)0x598 & 1) != 0) {
          uVar3 = 0x96f5;
          func_0x00097a9c(uVar4);
        }
      }
      if ((*(byte *)0x598 & 0x40) != 0) {
        uVar5 = uVar3;
        if ((*(byte *)0x599 & 4) != 0) {
          uVar5 = 0x96f5;
          func_0x00097a57(uVar3);
        }
        uVar4 = uVar5;
        if ((*(byte *)0x599 & 8) != 0) {
          uVar4 = 0x96f5;
          func_0x00097b01(uVar5);
        }
        uVar3 = uVar4;
        if ((*(byte *)0x599 & 1) != 0) {
          uVar3 = 0x96f5;
          func_0x00097a9c(uVar4);
        }
        *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 1;
      }
      bVar1 = *(byte *)*(undefined4 *)0x6d6;
      *(byte *)0x598 = bVar1;
      if ((bVar1 & 0x40) != 0) {
        *(undefined1 *)0x599 = *(undefined1 *)((int)*(undefined4 *)0x6d6 + 1);
      }
    }
    uVar5 = uVar3;
    if ((*(char *)0x68f != '\0') && (*(char *)0x45a != '\0')) {
      uVar5 = 0x96f5;
      func_0x0009785b(uVar3);
    }
    uVar3 = 0x8006;
    func_0x00080060(uVar5,0);
  }
  func_0x0009470e(uVar3,uVar2);
  return;
}



void __cdecl16far FUN_0009_76c0(void)

{
  undefined1 uVar1;
  char *pcVar2;
  undefined4 uVar3;
  undefined2 unaff_CS;
  int unaff_DS;
  
  if (*(char *)0x0 == '\0') {
    *(undefined1 *)0x0 = 0xff;
    func_0x00096dbe();
    unaff_CS = 0x9628;
    func_0x00096634(0x9687);
    *(byte *)0x452 = *(byte *)((int)*(undefined4 *)0x6d6 + 2) & 0x80;
  }
  else if (((int)*(undefined4 *)0x440 == (int)*(undefined4 *)0x434) &&
          ((int)((ulong)*(undefined4 *)0x440 >> 0x10) == (int)((ulong)*(undefined4 *)0x434 >> 0x10))
          ) {
    *(undefined1 *)0x1 = 0xff;
    *(byte *)0x453 = *(byte *)((int)*(undefined4 *)0x6d6 + 2) & 0x80;
    unaff_CS = 0x9628;
    func_0x00096634();
    *(undefined1 *)0x454 = 0xff;
  }
  pcVar2 = (char *)*(undefined4 *)0x434;
  *(int *)0x434 = *(int *)0x434 + 1;
  if (((int)*(undefined4 *)0x434 == 0xaf) &&
     ((int)((ulong)*(undefined4 *)0x434 >> 0x10) == unaff_DS)) {
    *(undefined2 *)0x434 = 0xf;
    *(int *)0x436 = unaff_DS;
  }
  if (((int)*(undefined4 *)0x434 == (int)*(undefined4 *)0x43c) &&
     ((int)((ulong)*(undefined4 *)0x434 >> 0x10) == (int)((ulong)*(undefined4 *)0x43c >> 0x10))) {
    *(undefined1 *)0x451 = 0xff;
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 0x83;
    if (*(char *)0x455 == '\0') {
      if (((int)*(undefined4 *)0x44c == (int)*(undefined4 *)0x448) &&
         ((int)((ulong)*(undefined4 *)0x44c >> 0x10) == (int)((ulong)*(undefined4 *)0x448 >> 0x10)))
      {
        func_0x00096634(unaff_CS);
        *(undefined1 *)0x454 = 0xff;
        uVar1 = *(undefined1 *)0x6;
        *(undefined1 *)0x6 = 0x14;
        func_0x0008526b(0x9628);
        *(undefined1 *)0x6 = uVar1;
      }
    }
  }
  *pcVar2 = *(char *)((int)*(undefined4 *)0x6d6 + 7);
  if (*(int *)0x440 == 0 && *(int *)0x442 == 0) {
    *(undefined1 *)0x3 = 0xff;
  }
  if (((*(byte *)0x598 & 8) != 0) || (((*(byte *)0x4f1 & 0x20) != 0 && (*pcVar2 == '\n')))) {
    if (*(int *)0x440 == 0 && *(int *)0x442 == 0) {
      uVar3 = *(undefined4 *)0x434;
      *(undefined2 *)0x440 = (int)uVar3;
      *(undefined2 *)0x442 = (int)((ulong)uVar3 >> 0x10);
    }
    else {
      uVar3 = *(undefined4 *)0x434;
      *(undefined2 *)&LAB_0009_03b4 = (int)uVar3;
      *(undefined2 *)0x446 = (int)((ulong)uVar3 >> 0x10);
    }
  }
  if ((*(byte *)0x4f1 & 0x20) == 0) {
    if ((((int)*(undefined4 *)0x434 == (int)*(undefined4 *)0x440) &&
        ((int)((ulong)*(undefined4 *)0x434 >> 0x10) == (int)((ulong)*(undefined4 *)0x440 >> 0x10)))
       && (*(char *)0x451 == '\0')) {
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 2;
    }
  }
  else if ((*(int *)&LAB_0009_03b4 == 0 && *(int *)0x446 == 0) && (*(char *)0x451 == '\0')) {
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 2;
  }
  return;
}



void __cdecl16far FUN_0009_785b(void)

{
  int unaff_DS;
  
  *(undefined1 *)0x45a = 0;
  *(undefined1 *)0x68f = 0;
  if (*(char *)0x455 == '\0') {
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 7) = *(undefined1 *)*(undefined4 *)0x448;
    if (((int)*(undefined4 *)0x448 == 0x432) &&
       ((int)((ulong)*(undefined4 *)0x448 >> 0x10) == unaff_DS)) {
      *(undefined2 *)0x448 = 0xaf;
      *(int *)0x44a = unaff_DS;
    }
    else {
      *(int *)0x448 = *(int *)0x448 + 1;
    }
    if (((int)*(undefined4 *)0x448 == (int)*(undefined4 *)0x44c) &&
       ((int)((ulong)*(undefined4 *)0x448 >> 0x10) == (int)((ulong)*(undefined4 *)0x44c >> 0x10))) {
      *(undefined1 *)0x455 = 0xff;
    }
  }
  else if (*(char *)0x456 == '\0') {
    if (*(char *)0x457 == '\0') {
      *(undefined1 *)0x45a = 0xff;
    }
    else {
      *(undefined1 *)0x458 = 0xff;
    }
  }
  else {
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 8;
    *(undefined1 *)0x457 = 0xff;
    *(undefined1 *)0x456 = 0;
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 7) = *(undefined1 *)0x450;
  }
  return;
}



/* parse_next_gpib_command_byte (confidence: Mechanism confirmed)
   
   Evidence: The GPIB command-byte-stream cursor: on first entry (`[0x61C]==0`) resets the read
   pointer `[0x582]`/`[0x584]` to the start of the command buffer `[0x586]`, using the confirmed
   device-address table `[0x712]` to clear a per-device flag first; else advances past the current
   byte, scanning to the next non-NUL byte into `[0x580]`/`[0x581]`; for a non-address byte (bit
   `0x80` clear) looks up its data byte via `[0x712][byte*4+1]` into `[0x57F]`, and when the
   RS-232/GPIB strap `[0x629]` is set, folds a bit from `[0]` into it and writes it plus tag `0x98`
   to a UART-like register pair via far ptr `[0x6D6]` - called unconditionally as the first step of
   `process_gpib_command_byte`
   
   (physical 0x097905, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far parse_next_gpib_command_byte(void)

{
  byte *pbVar1;
  undefined1 uVar2;
  undefined2 unaff_DS;
  
  if (*(char *)0x61c == '\0') {
    *(undefined2 *)0x582 = 0x590;
    *(undefined2 *)0x584 = unaff_DS;
  }
  else {
    if ((*(byte *)0x580 & 0x80) == 0) {
      *(undefined1 *)(*(byte *)((uint)*(byte *)0x580 * 4 + (int)*(undefined4 *)0x712) + 0x586) = 0;
      *(undefined1 *)0x581 = *(undefined1 *)0x580;
    }
    *(undefined2 *)0x582 = 0x586;
    *(undefined2 *)0x584 = unaff_DS;
    while (*(char *)*(undefined4 *)0x582 == '\0') {
      *(int *)0x582 = *(int *)0x582 + 1;
    }
  }
  *(undefined1 *)0x580 = *(undefined1 *)*(undefined4 *)0x582;
  if ((*(byte *)0x580 & 0x80) == 0) {
    *(undefined1 *)0x57f =
         *(undefined1 *)((uint)*(byte *)*(undefined4 *)0x582 * 4 + (int)*(undefined4 *)0x712 + 1);
    if (*(char *)0x629 != '\0') {
      *(byte *)((int)*(undefined4 *)0x6d6 + 5) = *(byte *)0x57f & 0xbf | *(byte *)0x0 & 0x10;
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 0x98;
    }
    pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
    *pbVar1 = *pbVar1 | 0xc;
  }
  else {
    if ((*(byte *)0x580 & 0x80) == 0) {
      uVar2 = *(undefined1 *)0x580;
    }
    else {
      uVar2 = 0;
    }
    *(undefined1 *)0x57f = uVar2;
    if (*(char *)0x629 != '\0') {
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 0x18;
      *(byte *)((int)*(undefined4 *)0x6d6 + 5) = *(byte *)0x57f | *(byte *)0x0 & 0x10;
    }
    pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
    *pbVar1 = *pbVar1 & 0xfb;
  }
  func_0x000839d1();
  return;
}



void __cdecl16far FUN_0009_7a16(void)

{
  undefined2 unaff_DS;
  
  if ((*(byte *)((int)*(undefined4 *)0x6d6 + 2) & 0xc0) == 0xc0) {
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 7;
  }
  if ((*(byte *)((int)*(undefined4 *)0x6d6 + 2) & 0x80) != 0) {
    *(undefined1 *)0x7 = 0;
  }
  *(undefined1 *)0xe = 0xff;
  return;
}



void __cdecl16far FUN_0009_7a57(void)

{
  undefined2 unaff_DS;
  
  if ((*(byte *)((int)*(undefined4 *)0x6d6 + 2) & 2) == 0) {
    if ((*(byte *)((int)*(undefined4 *)0x6d6 + 2) & 4) != 0) {
      *(undefined2 *)0xa = 1;
    }
  }
  else {
    if (*(char *)0x458 != '\0') {
      func_0x00096634();
    }
    *(undefined2 *)0xa = 2;
  }
  return;
}



void __cdecl16far FUN_0009_7a9c(void)

{
  byte *pbVar1;
  undefined2 unaff_DS;
  
  if ((*(byte *)0x4f1 & 0x80) != 0) {
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 0x89;
  }
  if ((*(byte *)0x4f1 & 0x40) != 0) {
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 0x8a;
  }
  if ((*(byte *)((int)*(undefined4 *)0x6d6 + 2) & 2) == 0 &&
      (*(byte *)((int)*(undefined4 *)0x6d6 + 2) & 4) == 0) {
    pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
    *pbVar1 = *pbVar1 & 0xfd;
  }
  else {
    pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
    *pbVar1 = *pbVar1 | 10;
  }
  func_0x000839d1();
  return;
}



/* reinit_comm_channel (confidence: Confirmed)
   
   Evidence: The comm ROM's own top-level channel reinit sequence (analogous to
   `reinit_system_state`): conditionally calls `reinit_system_state` itself cross-ROM, then
   unconditionally runs `reset_comm_parser_state`, `init_comm_channel_state_a`,
   `init_comm_tx_queue_and_ready_flags`, `SUB_96696`, and resets several more flags
   
   (physical 0x097B01, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __cdecl16far reinit_comm_channel(void)

{
  int iVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  if (*(char *)((int)*(undefined4 *)0x732 + 0x93) != '\0') {
    uVar2 = 0x41;
    func_0x0009470e();
    func_0x000e6dc4(38000,uVar2);
    unaff_CS = 38000;
    func_0x0009470e(0xe6a8,0x8f80);
  }
  iVar1 = func_0x00094488(unaff_CS,0x20);
  if (iVar1 != 0) {
    func_0x000944a2(0x941f,0x1d,0);
  }
  func_0x00085238(0x941f);
  func_0x00096597(0x8511);
  func_0x00096634(0x9628);
  func_0x00096696(0x9628);
  *(undefined1 *)0xc = 0xff;
  *(undefined1 *)0x454 = 0xff;
  func_0x0009470e(0x9628,0x41);
  *(undefined1 *)0x7b8 = 0;
  func_0x0009470e(38000,0x8f80);
  if (*(char *)0x5a4 == '\x16') {
    func_0x000850a2(38000);
  }
  else {
    *(undefined1 *)0x5a4 = 0;
  }
  return;
}



/* set_comm_critical_flag (confidence: Confirmed mechanism; the specific main-ROM callback's purpose
   not traced)
   
   Evidence: Sets a critical-section-style flag `[0x5A3]` to the given value; on the "leaving" edge
   (new value `0`, old value nonzero, `[0x5A1]` set), swaps `DS` to the main ROM's low-RAM segment
   (`0x41`, via `set_ds_return_old`) and calls a main-ROM routine before swapping back - a cross-ROM
   notify-on-unlock pattern
   
   (physical 0x097B94, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

char __stdcall16far set_comm_critical_flag(int param_1)

{
  char cVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  cVar1 = *(char *)0x5a3;
  if (((param_1 == 0) && (cVar1 != '\0')) && (*(char *)0x5a1 != '\0')) {
    *(undefined1 *)0x5a3 = 0;
    uVar3 = 0x41;
    uVar2 = func_0x0009470e();
    func_0x000e6a8e(38000,5,uVar3,uVar2);
    func_0x0009470e(0xe6a8,uVar2);
  }
  *(undefined1 *)0x5a3 = (undefined1)param_1;
  return cVar1;
}



/* engage_comm_hold (confidence: Confirmed)
   
   Evidence: `(reason)` - ORs reason into hold bitmask `[0x45C]`; if this is the first hold,
   requests XOFF via `[0x460]` and services the tx queue if data pending
   
   (physical 0x097BE6, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __stdcall16far engage_comm_hold(byte reason)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x45c == '\0') {
    *(byte *)0x59c = *(byte *)0x59c & 0xfe;
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 2) = *(undefined1 *)0x59c;
    if ((*(char *)0x45b != '\0') && (*(byte *)0x460 = *(byte *)0x460 | 2, *(char *)0x45a != '\0')) {
      func_0x00097431();
    }
  }
  *(byte *)0x45c = *(byte *)0x45c | reason;
  return;
}



/* release_comm_hold (confidence: Confirmed)
   
   Evidence: `(reason)` - clears reason from hold bitmask `[0x45C]`; if this releases the last hold,
   requests XON via `[0x460]` and services the tx queue if pending - counterpart to
   `engage_comm_hold`
   
   (physical 0x097C28, reached via the comm ROM's 0x90000-0x97FFF alias of 0x88000-0x8FFFF) */

void __stdcall16far release_comm_hold(word reason)

{
  undefined2 unaff_DS;
  
  *(byte *)0x45c = *(byte *)0x45c & ~(byte)reason;
  if (*(char *)0x45c == '\0') {
    *(byte *)0x59c = *(byte *)0x59c | 1;
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 2) = *(undefined1 *)0x59c;
    if ((*(char *)0x45b != '\0') && (*(byte *)0x460 = *(byte *)0x460 | 1, *(char *)0x45a != '\0')) {
      func_0x00097431();
    }
  }
  return;
}



uint __stdcall16far FUN_0009_7c6e(uint param_1)

{
  undefined2 unaff_DS;
  
  return *(uint *)0x1e6 & param_1;
}



void __stdcall16far FUN_0009_7c80(uint param_1)

{
  undefined2 unaff_DS;
  
  *(uint *)0x1e6 = *(uint *)0x1e6 & ~param_1;
  return;
}



void __stdcall16far FUN_0009_7c95(uint param_1)

{
  undefined2 unaff_DS;
  
  *(uint *)0x1e6 = *(uint *)0x1e6 | param_1;
  return;
}



void __stdcall16far FUN_0009_7ca8(byte *param_1,undefined4 param_2,byte param_3)

{
  uint *puVar1;
  byte bVar2;
  undefined4 uVar3;
  int iVar4;
  byte bVar5;
  byte *pbVar6;
  undefined2 unaff_CS;
  undefined2 uVar7;
  undefined2 unaff_DS;
  bool bVar8;
  undefined2 uVar9;
  bool local_b;
  
  bVar2 = *param_1;
  bVar8 = (bVar2 & 0x20) == 0;
  local_b = !bVar8;
  if (param_3 == 4) {
    uVar9 = 2;
    func_0x000f8f24();
    unaff_CS = 0xf8e6;
    func_0x000f8f24(0xf8e6,3,uVar9);
    param_3 = 1;
  }
  else if ((param_3 != 1) && ((*(byte *)0x19c & 1) != 0)) {
    unaff_CS = 0xf8e6;
    func_0x000f8f24();
  }
  bVar5 = param_3 * '\x02' + bVar8;
  pbVar6 = (byte *)param_1;
  uVar9 = (undefined2)((ulong)param_1 >> 0x10);
  if ((bVar2 & 0x40) == 0) {
    func_0x000f8ec8(unaff_CS,bVar5,0);
    uVar7 = 0x97c6;
    func_0x00097c95(0xf8e6,1);
    while (iVar4 = func_0x00097c6e(uVar7,1), iVar4 != 0) {
      uVar7 = 0xe693;
      func_0x000e693c(0x97c6);
    }
    func_0x000f930d(0x97c6,pbVar6,uVar9,param_3,bVar8);
  }
  else {
    func_0x000f8f24(unaff_CS,param_3);
    uVar7 = 0x97c6;
    func_0x00097c95(0xf8e6,1);
    while (iVar4 = func_0x00097c6e(uVar7,1), iVar4 != 0) {
      uVar7 = 0xe693;
      func_0x000e693c(0x97c6);
    }
    func_0x000f9166(0x97c6,param_3,pbVar6,uVar9);
    func_0x000fbb24(0xf916,param_3,bVar2 & 2,bVar2 & 0x20,
                    *(undefined2 *)((int)*(undefined4 *)0x1c94 + (uint)param_3 * 0x10),
                    *(undefined2 *)(pbVar6 + 0xe),*(undefined2 *)(pbVar6 + 0xe));
    func_0x000f956b(0xfb9f,param_3,bVar2 & 1);
    func_0x000f930d(0xf916,pbVar6,uVar9,param_3,bVar8);
    if ((bVar2 & 2) != 0) {
      func_0x000f930d(0xf916,pbVar6,uVar9,param_3,local_b);
    }
  }
  uVar3 = *(undefined4 *)0x1c80;
  uVar9 = (undefined2)((ulong)param_2 >> 0x10);
  *(int *)((int)param_2 + 0x24) = (int)uVar3 + *(int *)((uint)bVar5 * 4 + 0x3c);
  *(undefined2 *)((int)param_2 + 0x26) = (int)((ulong)uVar3 >> 0x10);
  func_0x000f8e66(0xf916,param_3);
  puVar1 = (uint *)((uint)param_3 * 2 + 0x55a);
  *puVar1 = *puVar1 | 0xb5f;
  return;
}


