
/* WARNING: Unknown calling convention -- yet parameter storage is locked */
/* set_comm_flow_hold (confidence: Mechanism confirmed; exact protocol role not confirmed)
   
   Evidence: `(engage)` - guarded on/off toggle of `[0x5A1]`/`[0x5A2]`, writing `0x93` (engage) or
   `0x13`/XOFF (release) into the comm status structure at `es:[0x6D6+3]` */

void set_comm_flow_hold(word engage)

{
  undefined1 uVar1;
  undefined1 uVar2;
  undefined2 unaff_DS;
  char in_stack_00000004;
  
  if (in_stack_00000004 == '\0') {
    if (*(char *)0x5a1 == '\0') {
      return;
    }
    uVar2 = 0x13;
    uVar1 = 0;
  }
  else {
    if (*(char *)0x5a1 != '\0') {
      return;
    }
    uVar1 = 0xff;
    uVar2 = 0x93;
  }
  *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = uVar2;
  *(undefined1 *)0x5a2 = uVar1;
  *(undefined1 *)0x5a1 = uVar1;
  return;
}



/* set_comm_queue_busy (confidence: Confirmed. **Physical significance found 2026-09-13**: output
   `0D` (physical `0x406F8`) is the UART's `DR` (byte-received) interrupt enable/mask bit -
   "rx-ready" literally means "RS-232 receive interrupt unmasked." **Confirmed this function's
   disengage path IS called during normal comm-channel init** (from the sequence around `0x8E584`,
   part of `reinit_comm_channel`'s init chain) - so the RX interrupt masking mechanism is correctly
   implemented and exercised, weakening (not confirming) the "interrupt never unmasked" theory from
   `TODO.md`. See `docs/comm-rom/rs232-early-investigation.md`'s "Traced the interrupt mask latch's
   real outputs" for the full writeup and what's still unresolved)
   
   Evidence: `(engage)` - nestable, interrupt-safe critical-section marker for the comm rx/tx queue:
   `engage!=0` pushes a level onto `[0x5A1]` and clears both ready flags (one word write to
   `es:[di]`, zeroing both `0D`/`1D` outputs at once); `engage==0` pops a level and, once fully
   unnested, restores rx-ready unconditionally (`es:[di]=1`, output `0D`) and tx-ready only if data
   is pending */

void __stdcall16far set_comm_queue_busy(word engage)

{
  byte bVar1;
  undefined2 *puVar2;
  undefined2 unaff_DS;
  
  puVar2 = (undefined2 *)*(undefined4 *)0x6e2;
  if (engage == 0) {
    bVar1 = *(byte *)0x5a1;
    *(byte *)0x5a1 = bVar1 >> 1;
    if (bVar1 >> 1 == 0) {
      *(undefined1 *)0x597 = 0;
      *(undefined1 *)puVar2 = 1;
      if (*(char *)0x45a == '\0') {
        *(undefined1 *)0x596 = 0;
        *(undefined1 *)((int)(undefined2 *)puVar2 + 1) = 1;
      }
    }
  }
  else {
    *(undefined1 *)0x68e = 0xff;
    *puVar2 = 0;
    *(undefined1 *)0x597 = 0xff;
    *(undefined1 *)0x596 = 0xff;
    *(byte *)0x5a1 = *(char *)0x5a1 << 1 | 1;
  }
  return;
}



/* update_comm_tx_ready_flag (confidence: Confirmed. **Physical significance found 2026-09-13**:
   `[0x6E2]` is the far pointer to the service manual's **Option Interrupt Mask Latch**
   (`0x406F8`-`0x406FB`, `U1236`) - `es:[di+1]` is output `1D` (physical `0x406F9`). This "tx-ready
   flag" IS the UART's `TBRE` interrupt enable/mask bit, not just a software flag - see
   `MEMORY_MAP.md`'s "Option 12 (RS-232) hardware confirmed" section)
   
   Evidence: `(clear)` - single-shot toggle of the tx-ready byte pair (`[0x596]`/`es:[di+1]` via far
   ptr `[0x6E2]`); `clear==0` sets ready only if `[0x45A]` indicates data is pending; called by
   `enqueue_comm_char(0)` */

void __stdcall16far update_comm_tx_ready_flag(word clear)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  uVar2 = (undefined2)((ulong)*(undefined4 *)0x6e2 >> 0x10);
  iVar1 = (int)*(undefined4 *)0x6e2;
  if (clear == 0) {
    if (*(char *)0x45a == '\0') {
      *(undefined1 *)0x596 = 0;
      *(undefined1 *)(iVar1 + 1) = 1;
    }
  }
  else {
    *(undefined1 *)(iVar1 + 1) = 0;
    *(undefined1 *)0x596 = 0xff;
  }
  return;
}



/* init_far_pointer_table (confidence: Confirmed)
   
   Evidence: Applies a compiled-in table (embedded right after the code) of `(dest_offset, far_ptr)`
   entries, terminated by `-1`, writing each far pointer into a RAM location - bulk-initializes
   RAM-resident far-pointer variables; called from both ROMs */

void init_far_pointer_table(void)

{
  undefined2 *puVar1;
  undefined2 uVar2;
  undefined2 *puVar3;
  
  uVar2 = uRam00080152;
  for (puVar3 = (undefined2 *)0x24; puVar1 = (undefined2 *)*puVar3, puVar1 != (undefined2 *)0xffff;
      puVar3 = puVar3 + 3) {
    *puVar1 = puVar3[1];
    puVar1[1] = puVar3[2];
  }
  return;
}



void __cdecl16far FUN_0008_025a(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  *(undefined2 *)0x548 = 0xffff;
  *(undefined1 *)0x1bf6 = 0;
  func_0x0009470e(38000,uVar1,uVar2,uVar1);
  return;
}



void __cdecl16far FUN_0008_0281(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2);
  return;
}



void __cdecl16far FUN_0008_02cd(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x557 = *(undefined1 *)0x5fe;
  return;
}



void __cdecl16far FUN_0008_02dd(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x558 = *(undefined1 *)0x5fe;
  if (*"\x19=\x01" == 'F') {
    *(undefined1 *)0x558 = 0x42;
  }
  else if (*"\x19=\x01" == 'Y') {
    *(undefined1 *)0x558 = 0x55;
  }
  return;
}



void __cdecl16far FUN_0008_0308(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x559 = *(undefined1 *)0x5fe;
  if (*"=\x01" == 'Y') {
    *(undefined1 *)0x559 = 0x55;
  }
  return;
}



void __cdecl16far FUN_0008_0324(void)

{
  int iVar1;
  undefined2 unaff_DS;
  
  iVar1 = func_0x000836aa();
  if (*(char *)0x623 == '\0') {
    if ((((iVar1 == 0x100) || (iVar1 == 0x200)) || (iVar1 == 0x400)) ||
       ((iVar1 == 0x800 || (iVar1 == 0x1000)))) {
      *(int *)0x55c = iVar1;
    }
    else {
      *(undefined1 *)0x623 = 0x15;
    }
  }
  return;
}



void __cdecl16far FUN_0008_0371(void)

{
  int iVar1;
  undefined2 unaff_DS;
  
  iVar1 = func_0x000836aa();
  if (*(char *)0x623 == '\0') {
    if ((iVar1 < -0x1000) || (0x1000 < iVar1)) {
      *(undefined1 *)0x623 = 0x15;
    }
    else {
      *(int *)0x55e = iVar1;
    }
  }
  return;
}



void __cdecl16far FUN_0008_039f(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  if (*(char *)0x57e == '\0') {
    uVar1 = *(undefined2 *)0x57a;
    *(undefined2 *)0x566 = *(undefined2 *)0x578;
    *(undefined2 *)0x568 = uVar1;
    *(undefined2 *)0x572 = *(undefined2 *)0x57c;
  }
  else {
    *(undefined2 *)0x566 = 0;
    *(undefined2 *)0x568 = 0;
    *(undefined2 *)0x572 = 0;
    *(undefined1 *)0x623 = 0x15;
  }
  return;
}



void __cdecl16far FUN_0008_03de(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  if (*(char *)0x57e == '\0') {
    uVar1 = *(undefined2 *)0x57a;
    *(undefined2 *)0x56a = *(undefined2 *)0x578;
    *(undefined2 *)0x56c = uVar1;
    *(undefined2 *)0x574 = *(undefined2 *)0x57c;
  }
  else {
    *(undefined2 *)0x56a = 0;
    *(undefined2 *)0x56c = 0;
    *(undefined2 *)0x574 = 0;
    *(undefined1 *)0x623 = 0x15;
  }
  return;
}



void __cdecl16far FUN_0008_041d(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  if (*(char *)0x57e == '\0') {
    uVar1 = *(undefined2 *)0x57a;
    *(undefined2 *)0x56e = *(undefined2 *)0x578;
    *(undefined2 *)0x570 = uVar1;
    *(undefined2 *)(code *)FUN_0008_04f6 = *(undefined2 *)0x57c;
  }
  else {
    *(undefined2 *)0x56e = 0;
    *(undefined2 *)0x570 = 0;
    *(undefined2 *)(code *)FUN_0008_04f6 = 0;
    *(undefined1 *)0x623 = 0x15;
  }
  return;
}



void __cdecl16far FUN_0008_045c(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x625 == '\0') {
    if (-1 < *(int *)0x600) {
      *(undefined2 *)0x562 = *(undefined2 *)0x600;
      return;
    }
  }
  else if (*(char *)0x625 == '\x01') {
    *(undefined2 *)0x562 = *(undefined2 *)0x578;
    return;
  }
  *(undefined1 *)0x623 = 0x15;
  return;
}



void __cdecl16far FUN_0008_0492(void)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined1 local_a;
  
  iVar1 = func_0x000836aa();
  if (*(char *)0x623 == '\0') {
    if ((iVar1 == 8) || (iVar1 == 0x10)) {
      local_a = (undefined1)iVar1;
      *(undefined1 *)0x55a = local_a;
    }
    else {
      *(undefined1 *)0x623 = 0x15;
    }
  }
  return;
}



void __cdecl16far FUN_0008_04c4(void)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined1 local_a;
  
  iVar1 = func_0x000836aa();
  if (*(char *)0x623 == '\0') {
    if ((iVar1 == 1) || (iVar1 == 2)) {
      local_a = (undefined1)iVar1;
      *(undefined1 *)0x55b = local_a;
    }
    else {
      *(undefined1 *)0x623 = 0x15;
    }
  }
  return;
}



void __cdecl16far FUN_0008_04f6(void)

{
  byte bVar1;
  int iVar2;
  undefined2 uVar3;
  char *pcVar4;
  undefined2 unaff_DS;
  
  iVar2 = func_0x00080a8c();
  if (iVar2 == 0) {
    if (*(char *)0x4f4 == '\0') {
      func_0x00082d01(0x802c,0x26c,(char *)s_Delete_failed_0008_906a + 7);
    }
    else {
      func_0x00082d01(0x802c,0x271,(char *)s_Delete_failed_0008_906a + 7);
      if (*(byte *)0x4f4 < 5) {
        iVar2 = 0x30;
      }
      else {
        iVar2 = 0x3c;
      }
      func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                      (uint)*(byte *)0x4f4 + iVar2);
    }
    func_0x00082d01((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x276,
                    (char *)s_Delete_failed_0008_906a + 7);
    if ((*(byte *)0x500 & 0x20) == 0) {
      func_0x00082d9b((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                      (int)*(undefined4 *)0x6f2 + 0x25a,(int)((ulong)*(undefined4 *)0x6f2 >> 0x10));
    }
    else if ((*(byte *)0x503 & 0xb) == 0) {
      func_0x00082d9b((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                      (int)*(undefined4 *)0x6f2 + 0x275,(int)((ulong)*(undefined4 *)0x6f2 >> 0x10));
    }
    else {
      if (*(char *)((int)*(undefined4 *)0x73a + 0x34) == '\0') {
        uVar3 = (undefined2)((ulong)*(undefined4 *)0x6f2 >> 0x10);
        iVar2 = (int)*(undefined4 *)0x6f2 + 0x60;
      }
      else {
        uVar3 = (undefined2)((ulong)*(undefined4 *)0x6f2 >> 0x10);
        iVar2 = (int)*(undefined4 *)0x6f2 + 100;
      }
      func_0x00082d9b((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,iVar2,uVar3);
    }
    func_0x00082d01((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x279,
                    (char *)s_Delete_failed_0008_906a + 7);
    if ((*(byte *)0x500 & 0x20) == 0) {
      func_0x00080b60((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                      (int)*(undefined4 *)0x54e,(int)((ulong)*(undefined4 *)0x54e >> 0x10));
      func_0x00080b60(0x802c,(int)*(undefined4 *)0x552,(int)((ulong)*(undefined4 *)0x552 >> 0x10));
    }
    else {
      func_0x00080b60((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                      (int)*(undefined4 *)0x552,(int)((ulong)*(undefined4 *)0x552 >> 0x10));
    }
    if (*(char *)0x502 == '\x19') {
      pcVar4 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
      func_0x00082d01(0x802c,0x27c,(char *)s_Delete_failed_0008_906a + 7);
    }
    else {
      if ((*(byte *)0x500 & 0x40) == 0) {
        uVar3 = 0;
      }
      else {
        uVar3 = 0x10;
      }
      pcVar4 = (char *)0x802c;
      func_0x00081470(0x802c,*(undefined1 *)0x502,uVar3);
    }
    func_0x00082d01(pcVar4,0x280,(char *)s_Delete_failed_0008_906a + 7);
    bVar1 = *(byte *)0x501 & 0xf;
    if (bVar1 == 0) {
      func_0x00082d01((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x283,
                      (char *)s_Delete_failed_0008_906a + 7);
    }
    else if (bVar1 == 1) {
      func_0x00082d01((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x28a,
                      (char *)s_Delete_failed_0008_906a + 7);
    }
    else if (bVar1 == 2) {
      func_0x00082d01((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x292,
                      (char *)s_Delete_failed_0008_906a + 7);
    }
    else {
      func_0x00082d01((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x29a,
                      (char *)s_Delete_failed_0008_906a + 7);
    }
    if ((*(byte *)0x501 & 0xf0) != 0) {
      func_0x00082d01((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x2a2,
                      (char *)s_Delete_failed_0008_906a + 7);
    }
    func_0x00082d01((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x2ac,
                    (char *)s_Delete_failed_0008_906a + 7);
    func_0x0008142f((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,*(undefined2 *)0x4fc);
    func_0x00083241(0x802c,0x22);
  }
  return;
}



void __cdecl16far FUN_0008_06ca(void)

{
  int iVar1;
  undefined2 unaff_DS;
  
  iVar1 = func_0x00080a8c();
  if (iVar1 == 0) {
    if (*(char *)((int)*(undefined4 *)0x73a + 0x34) == '\0') {
      iVar1 = *(int *)0x53a;
    }
    else {
      iVar1 = *(int *)0x540;
    }
    if ((*(byte *)0x556 & 0x10) != 0) {
      iVar1 = iVar1 >> 1;
    }
    func_0x00081404(0x802c,iVar1);
  }
  return;
}



void __cdecl16far FUN_0008_0718(void)

{
  int iVar1;
  int unaff_SI;
  undefined2 unaff_DS;
  
  iVar1 = func_0x00080a8c();
  if (iVar1 == 0) {
    unaff_SI = *(int *)0x504;
  }
  if ((*(byte *)0x556 & 0x10) != 0) {
    unaff_SI = unaff_SI >> 1;
  }
  func_0x00081404(0x802c,unaff_SI);
  return;
}



void __cdecl16far FUN_0008_074b(void)

{
  func_0x00080a8c();
  return;
}



void __cdecl16far FUN_0008_0797(void)

{
  int iVar1;
  undefined1 uVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  int iStack_a;
  
  iVar1 = func_0x00080a8c();
  if (iVar1 == 0) {
    if ((((*(byte *)0x500 & 0x20) == 0) || (*(char *)0x606 != '^')) ||
       (*(char *)((int)*(undefined4 *)0x732 + 0x1f) != '\x1e')) {
      if (*(char *)0x606 == '^') {
        iStack_a = (int)((ulong)*(undefined4 *)0x54e >> 0x10);
        iVar1 = (int)*(undefined4 *)0x54e;
      }
      else {
        iStack_a = (int)((ulong)*(undefined4 *)0x552 >> 0x10);
        iVar1 = (int)*(undefined4 *)0x552;
      }
      uVar3 = 0xd;
      if ((*(byte *)0x500 & 0x10) == 0) {
        uVar3 = 0xf;
      }
      if (iVar1 == 0 && iStack_a == 0) {
        func_0x00082d01(0x802c,699,(char *)s_Delete_failed_0008_906a + 7);
      }
      else {
        if ((*(byte *)(iVar1 + 6) & 1) == 0) {
          uVar2 = 9;
        }
        else {
          uVar2 = *(undefined1 *)(iVar1 + 4);
        }
        func_0x00081470(0x802c,uVar2,uVar3);
      }
    }
    else {
      func_0x00082d01(0x802c,0x2b4,(char *)s_Delete_failed_0008_906a + 7);
    }
  }
  return;
}



void __cdecl16far FUN_0008_0851(void)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined2 local_a;
  
  iVar1 = func_0x00080a8c();
  if (iVar1 == 0) {
    if ((*(char *)((int)*(undefined4 *)0x552 + 7) == '\0') || (*(char *)0x606 != 'd')) {
      func_0x0008142f(0x802c,0);
    }
    else {
      local_a = *(uint *)((int)*(undefined4 *)0x552 + 8);
      if ((*(byte *)0x500 & 0x10) == 0) {
        local_a = local_a >> 2;
      }
      else {
        local_a = local_a << 6;
      }
      func_0x0008142f(0x802c,local_a);
    }
  }
  return;
}



void __cdecl16far FUN_0008_08b3(void)

{
  func_0x00080a8c();
  return;
}



void __cdecl16far FUN_0008_08fc(void)

{
  int iVar1;
  uint uVar2;
  undefined2 unaff_DS;
  
  iVar1 = func_0x00080a8c();
  if (iVar1 == 0) {
    uVar2 = 0xc;
    if ((*(byte *)0x500 & 4) == 0) {
      if (((*(byte *)0x556 & 0xf) != 2) && ((*(byte *)0x500 & 0x20) != 0)) {
        uVar2 = 0xe;
      }
    }
    else if ((*(byte *)0x556 & 0xf) == 2) {
      uVar2 = 0x2c;
    }
    if (((*(byte *)0x500 & 0x40) != 0) && (*(char *)0x4f4 != '\0')) {
      uVar2 = uVar2 | 0x10;
    }
    func_0x00081470(0x802c,*(undefined1 *)0x502,uVar2);
  }
  return;
}



void __cdecl16far FUN_0008_097f(void)

{
  func_0x00080a8c();
  return;
}



void __cdecl16far FUN_0008_09e5(void)

{
  int iVar1;
  
  iVar1 = func_0x00080a8c();
  if (iVar1 == 0) {
    func_0x00082d01(0x802c,0x2c2,(char *)s_Delete_failed_0008_906a + 7);
  }
  return;
}



void __cdecl16far FUN_0008_0a0a(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  iVar1 = func_0x00080a8c();
  if (iVar1 == 0) {
    if ((*(byte *)0x500 & 0x10) == 0) {
      uVar2 = 1;
    }
    else {
      uVar2 = 2;
    }
    func_0x00081404(0x802c,uVar2);
  }
  return;
}



void __cdecl16far FUN_0008_0a3a(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  iVar1 = func_0x00080a8c();
  if (iVar1 == 0) {
    if ((*(byte *)0x500 & 0x10) == 0) {
      uVar2 = 8;
    }
    else {
      uVar2 = 0x10;
    }
    func_0x00081404(0x802c,uVar2);
  }
  return;
}



void __cdecl16far FUN_0008_0a6a(void)

{
  func_0x00080a8c();
  return;
}



undefined1 __cdecl16far FUN_0008_0a8c(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  if ((*(int *)0x5fc < 2) && (iVar1 = func_0x00081344(), iVar1 != 0)) {
    return *(undefined1 *)0x623;
  }
  if ((*(byte *)0x500 & 0x20) == 0) {
    *(undefined2 *)0x54e = 0x512;
    *(undefined2 *)0x550 = unaff_DS;
    *(undefined2 *)0x552 = 0x51c;
    *(undefined2 *)0x554 = unaff_DS;
  }
  else {
    *(undefined2 *)0x54e = 0;
    *(undefined2 *)0x550 = 0;
    if (*(char *)((int)*(undefined4 *)0x73a + 0x34) == '\0') {
      uVar2 = 0x512;
    }
    else {
      uVar2 = 0x51c;
    }
    *(undefined2 *)0x552 = uVar2;
    *(undefined2 *)0x554 = unaff_DS;
  }
  if ((*(byte *)0x500 & 0x10) == 0) {
    if ((*(byte *)0x500 & 0x20) == 0) {
      *(undefined1 *)0x556 = 0x11;
    }
    else if (((*(byte *)0x501 & 0xf0) == 0) &&
            (((*(byte *)0x501 & 0xf) == 1 || ((*(byte *)0x501 & 0xf) == 2)))) {
      *(undefined1 *)0x556 = 0x12;
    }
    else {
      *(undefined1 *)0x556 = 0;
    }
  }
  else {
    *(undefined1 *)0x556 = 0x10;
  }
  return *(undefined1 *)0x623;
}



void __stdcall16far FUN_0008_0b60(undefined4 param_1)

{
  byte bVar1;
  undefined2 uVar2;
  char *unaff_CS;
  undefined2 unaff_DS;
  
  if (*(char *)((int)*(undefined4 *)0x732 + 0x1f) != '\x14') {
    uVar2 = (undefined2)((ulong)param_1 >> 0x10);
    bVar1 = *(byte *)((int)param_1 + 6);
    if ((bVar1 & 1) == 0) {
      unaff_CS = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
      func_0x00083241();
    }
    func_0x00081470(unaff_CS,*(undefined1 *)((int)param_1 + 4),1);
    func_0x00082d01(0x802c,0x2c5,(char *)s_Delete_failed_0008_906a + 7);
    if (((*(byte *)0x503 & 0xb) == 0) && (((*(byte *)0x518 ^ *(byte *)0x522) & 0xf) != 0)) {
      func_0x00082d9b((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                      (int)*(undefined4 *)0x6f2 + 700,(int)((ulong)*(undefined4 *)0x6f2 >> 0x10));
    }
    else if ((bVar1 & 4) == 0) {
      if ((bVar1 & 8) == 0) {
        if ((bVar1 & 2) == 0) {
          func_0x00082d9b((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                          (int)*(undefined4 *)0x6f2 + 700,(int)((ulong)*(undefined4 *)0x6f2 >> 0x10)
                         );
        }
        else {
          func_0x00082d9b((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                          (int)*(undefined4 *)0x6f2 + 0x299,
                          (int)((ulong)*(undefined4 *)0x6f2 >> 0x10));
        }
      }
      else {
        func_0x00082d9b((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                        (int)*(undefined4 *)0x6f2 + 0x272,(int)((ulong)*(undefined4 *)0x6f2 >> 0x10)
                       );
      }
    }
    else {
      func_0x00082d9b((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                      (int)*(undefined4 *)0x6f2 + 0x296,(int)((ulong)*(undefined4 *)0x6f2 >> 0x10));
    }
    func_0x00082d01((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x2c8,
                    (char *)s_Delete_failed_0008_906a + 7);
  }
  return;
}



void __cdecl16far FUN_0008_0c55(void)

{
  byte *pbVar1;
  char cVar2;
  char cVar3;
  char cVar4;
  int iVar5;
  byte bVar6;
  byte bVar7;
  int *piVar8;
  undefined2 uVar9;
  char *unaff_CS;
  char *pcVar10;
  undefined2 unaff_DS;
  bool bVar11;
  undefined1 local_2e;
  uint local_2c;
  int *local_22;
  code *local_18;
  code *pcStack_16;
  int local_14;
  undefined2 uStack_12;
  int local_10;
  undefined2 uStack_e;
  int local_c;
  int local_a;
  
  local_18 = (code *)0x9;
  pcStack_16 = (code *)((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
  if (*(char *)((int)*(undefined4 *)0x73a + 0x2f) == '\x01') {
    unaff_CS = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
    iVar5 = func_0x000838e3();
    if (iVar5 == 0) {
      return;
    }
    local_18 = (code *)0x5b1;
  }
  pcStack_16 = (code *)((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
  iVar5 = func_0x00081344(unaff_CS);
  if (iVar5 == 0) {
    bVar7 = *(byte *)((int)*(undefined4 *)0x732 + 0x12);
    pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x12);
    *pbVar1 = *pbVar1 | 2;
    if ((*(byte *)((int)*(undefined4 *)0x732 + 0xf) & 2) == 0) {
      bVar6 = 3;
    }
    else {
      bVar6 = 1;
    }
    *(byte *)0x603 = *(byte *)0x603 | bVar6;
    pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
    *pbVar1 = *pbVar1 | 9;
    func_0x000839d1(0x802c);
    *(int *)0x4fc = *(int *)0x4fc + 1;
    local_a = 0;
    bVar11 = *(char *)((int)*(undefined4 *)0x73a + 0x34) != '\0';
    if (bVar11) {
      local_c = *(int *)0x540;
      uStack_12 = (undefined2)((ulong)*(undefined4 *)0x53c >> 0x10);
      local_14 = (int)*(undefined4 *)0x53c;
    }
    else {
      local_c = *(int *)0x53a;
      uStack_12 = (undefined2)((ulong)*(undefined4 *)0x536 >> 0x10);
      local_14 = (int)*(undefined4 *)0x536;
    }
    func_0x0009470e((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x41);
    piVar8 = (int *)((uint)bVar11 * 4 + 0x3c);
    _local_22 = (int *)CONCAT22(unaff_DS,piVar8);
    uVar9 = (undefined2)((ulong)*(undefined4 *)0x1c80 >> 0x10);
    iVar5 = (int)*(undefined4 *)0x1c80 + *_local_22;
    local_2c = func_0x000f97d3(38000,bVar11,0,piVar8,unaff_DS);
    pcVar10 = (char *)0x9470;
    func_0x0009470e(0xf916,(char *)s_TRIggerd_0008_8ef8 + 8);
    local_2e = 0;
    if (((*(byte *)0x501 & 0xf0) == 0) &&
       (((*(byte *)0x501 & 3) != 0 || ((*(byte *)0x500 & 0x20) == 0)))) {
      local_2e = 1;
      local_2c = local_2c & 0xfffe;
    }
    if ((*(char *)0x4f4 == '\0') &&
       (((*(char *)0x1f == '\x03' || (*(char *)0x1f == '\a')) ||
        ((*(char *)0x1f == '\x05' && ((*(uint *)0x22 & 4) == 0)))))) {
      local_14 = iVar5 + local_2c;
      _local_10 = CONCAT22(uVar9,local_14);
      if (*(int *)0x2a < (int)(local_2c + local_c + -1)) {
        local_a = (*(int *)0x2a - local_2c) + 1;
        local_14 = iVar5;
      }
      local_c = local_c - local_a;
      uStack_12 = uVar9;
    }
    if (*(char *)((int)*(undefined4 *)0x73a + 0x2f) == '\0') {
      if (local_a != 0) {
        func_0x00080f36(38000,(int)_local_10,(int)((ulong)_local_10 >> 0x10),local_a,local_2e);
        pcVar10 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
        func_0x00083241(0x802c,0x2c);
      }
      func_0x00080f36(pcVar10,local_14,uStack_12,local_c,local_2e);
    }
    else {
      cVar2 = func_0x0008106f(38000,local_18,pcStack_16,local_a + local_c);
      cVar3 = func_0x00080ff9(0x802c,local_18,pcStack_16,(int)_local_10,
                              (int)((ulong)_local_10 >> 0x10),local_a,local_2e);
      cVar4 = func_0x00080ff9(0x802c,local_18,pcStack_16,local_14,uStack_12,local_c,local_2e);
      (*local_18)(0x802c,~(cVar2 + cVar3 + cVar4) + 1);
    }
    *(byte *)((int)*(undefined4 *)0x732 + 0x12) =
         *(byte *)((int)*(undefined4 *)0x732 + 0x12) & 0xfd | bVar7 & 2;
    if ((*(byte *)((int)*(undefined4 *)0x732 + 0xf) & 2) == 0) {
      bVar7 = 3;
    }
    else {
      bVar7 = 1;
    }
    *(byte *)0x603 = *(byte *)0x603 | bVar7;
    pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
    *pbVar1 = *pbVar1 & 0xfe;
    func_0x000839d1(0x802c);
  }
  return;
}



void __stdcall16far FUN_0008_0f36(undefined2 *param_1,int param_2,byte param_3)

{
  undefined2 *puVar1;
  undefined2 uVar2;
  char *unaff_CS;
  char *pcVar3;
  undefined2 unaff_DS;
  undefined2 uVar4;
  int local_a;
  
  puVar1 = (undefined2 *)param_1;
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  if ((*(byte *)0x501 & 3) == 3) {
    uVar4 = *param_1;
    func_0x0008142f();
    for (local_a = 1; local_a < param_2 >> 1; local_a = local_a + 1) {
      func_0x00083241(0x802c,0x2c,uVar4);
      func_0x0008142f((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,puVar1[local_a]);
    }
  }
  else {
    for (local_a = 0; local_a < param_2; local_a = local_a + 2) {
      pcVar3 = unaff_CS;
      if (local_a != 0) {
        pcVar3 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
        func_0x00083241(unaff_CS,0x2c);
      }
      func_0x0008142f(pcVar3,*(undefined1 *)((int)puVar1 + local_a + (uint)param_3));
      if (param_2 + -1 <= local_a) {
        return;
      }
      func_0x00083241(0x802c,0x2c);
      unaff_CS = (char *)0x802c;
      func_0x0008142f((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                      *(undefined1 *)((int)puVar1 + local_a + (uint)(byte)(1 - param_3)));
    }
  }
  return;
}



char __stdcall16far
FUN_0008_0ff9(code *param_1,undefined2 param_2,undefined4 param_3,int param_4,byte param_5)

{
  char cVar1;
  char cVar2;
  byte bVar3;
  undefined2 unaff_DS;
  int local_c;
  char local_9;
  
  local_9 = '\0';
  bVar3 = 1 - param_5;
  local_c = 0;
  while( true ) {
    if (param_4 <= local_c) {
      return local_9;
    }
    cVar1 = *(char *)((int)param_3 + local_c + (uint)param_5);
    (*param_1)();
    if (param_4 + -1 <= local_c) break;
    cVar2 = *(char *)((int)param_3 + local_c + (uint)bVar3);
    (*param_1)();
    local_9 = local_9 + cVar1 + cVar2;
    local_c = local_c + 2;
  }
  return local_9 + cVar1;
}



char __stdcall16far FUN_0008_106f(code *param_1,int param_2)

{
  int iVar1;
  undefined2 unaff_DS;
  
  if (((code *)param_1 == (code *)0x9) &&
     ((char *)((ulong)param_1 >> 0x10) == (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18)) {
    func_0x00082d01();
  }
  else {
    func_0x00083241();
  }
  (*(code *)param_1)((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,param_2 + 1U >> 8);
  iVar1 = param_2 + 1;
  (*(code *)param_1)((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,param_2 + 1U & 0xff);
  return (char)((uint)iVar1 >> 8) + (char)(param_2 + 1);
}



void __cdecl16far FUN_0008_10f6(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x4f4 = 0;
  *(byte *)0x603 = *(byte *)0x603 | 1;
  return;
}



void __cdecl16far FUN_0008_110a(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x1e') {
    if (*(int *)0x600 == 0) {
      *(undefined1 *)0x623 = 4;
    }
    else {
      *(undefined1 *)0x4f4 = *(undefined1 *)0x600;
      *(byte *)0x603 = *(byte *)0x603 | 1;
    }
  }
  else if ((*(int *)0x600 == 1) || (*(int *)0x600 == 4)) {
    *(undefined1 *)0x4f4 = 4;
    *(byte *)0x603 = *(byte *)0x603 | 1;
  }
  else {
    *(undefined1 *)0x623 = 4;
  }
  return;
}



void __cdecl16far FUN_0008_115c(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x1e') {
    if (*(int *)0x600 == 0) {
      *(undefined1 *)0x623 = 4;
    }
    else {
      *(undefined1 *)0x4f5 = *(undefined1 *)0x600;
    }
  }
  else if ((*(int *)0x600 == 1) || (*(int *)0x600 == 4)) {
    *(undefined1 *)0x4f5 = 4;
  }
  else {
    *(undefined1 *)0x623 = 4;
  }
  return;
}



void __cdecl16far FUN_0008_11a4(void)

{
  undefined2 unaff_DS;
  
  *(bool *)((int)*(undefined4 *)0x73a + 0x34) = *"<" != '\x0e';
  *(byte *)0x603 = *(byte *)0x603 | 1;
  return;
}



void __cdecl16far FUN_0008_11cc(void)

{
  undefined2 unaff_DS;
  
  if (*"<" == '\x04') {
    *(undefined1 *)((int)*(undefined4 *)0x73a + 0x2f) = 0;
  }
  else if (*"<" == '\t') {
    *(undefined1 *)((int)*(undefined4 *)0x73a + 0x2f) = 1;
  }
  else {
    *(undefined1 *)((int)*(undefined4 *)0x73a + 0x2f) = 2;
  }
  *(byte *)0x603 = *(byte *)0x603 | 1;
  return;
}



void __cdecl16far FUN_0008_120a(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  if (*(char *)0x4f4 == '\0') {
    uVar2 = (undefined2)((ulong)*(undefined4 *)0x6f2 >> 0x10);
    iVar1 = (int)*(undefined4 *)0x6f2 + 0xe;
  }
  else {
    uVar2 = (undefined2)((ulong)*(undefined4 *)0x6f2 >> 0x10);
    iVar1 = (int)*(undefined4 *)0x6f2 + 0x17b;
  }
  func_0x00082d9b();
  if ((*(char *)0x4f4 == '\0') || (4 < *(byte *)0x4f4)) {
    if (4 < *(byte *)0x4f4) {
      func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,*(byte *)0x4f4 + 0x3c,
                      iVar1,uVar2);
    }
  }
  else {
    func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,*(byte *)0x4f4 + 0x30,
                    iVar1,uVar2);
  }
  return;
}



void __cdecl16far FUN_0008_1270(void)

{
  char cVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  int iVar3;
  
  uVar2 = (undefined2)((ulong)*(undefined4 *)0x6f2 >> 0x10);
  iVar3 = (int)*(undefined4 *)0x6f2 + 0x17b;
  func_0x00082d9b();
  if ((*(char *)0x4f5 == '\0') || (4 < *(byte *)0x4f5)) {
    if (*(byte *)0x4f5 < 5) {
      cVar1 = '1';
      *(undefined1 *)0x4f5 = 1;
    }
    else {
      cVar1 = *(char *)0x4f5 + '<';
    }
  }
  else {
    cVar1 = *(char *)0x4f5 + '0';
  }
  func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,cVar1,iVar3,uVar2);
  return;
}



void __cdecl16far FUN_0008_12c9(void)

{
  return;
}



void __cdecl16far FUN_0008_1304(void)

{
  return;
}



undefined1 __cdecl16far FUN_0008_1344(void)

{
  char cVar1;
  undefined1 uVar2;
  int iVar3;
  char cVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  
  if (*(byte *)0x4f4 < 5) {
    cVar1 = *(char *)0x4f4;
    uVar5 = 0x41;
    func_0x0009470e();
    cVar4 = cVar1;
    if (cVar1 == '\x04') {
      cVar4 = '\x01';
    }
    iVar3 = func_0x00086b4b(38000,cVar4,0x500,unaff_DS,uVar5);
    func_0x0009470e(0x85eb,(char *)s_TRIggerd_0008_8ef8 + 8);
    if (iVar3 == 0) {
      if (*(char *)((int)*(undefined4 *)0x73a + 0x34) == '\0') {
        if ((*(byte *)0x526 & 2) == 0) {
          *(undefined1 *)0x623 = 0x15;
        }
      }
      else if ((*(byte *)0x527 & 2) == 0) {
        *(undefined1 *)0x623 = 0x15;
      }
      if ((*(byte *)0x500 & 1) == 0) {
        if (cVar1 == '\x04') {
          *(undefined1 *)0x623 = 0x22;
        }
      }
      else if (cVar1 == '\x01') {
        *(undefined1 *)0x623 = 0x22;
      }
      uVar2 = *(undefined1 *)0x623;
    }
    else {
      *(undefined1 *)0x623 = 0x22;
      uVar2 = 0x22;
    }
  }
  else {
    *(undefined1 *)0x623 = 0x15;
    uVar2 = 0x15;
  }
  return uVar2;
}



/* print_signed_decimal_serial (confidence: Confirmed)
   
   Evidence: `(n)` - prints `-` and negates `n` if negative, then calls
   `print_unsigned_decimal_serial` */

void __stdcall16far print_signed_decimal_serial(word n)

{
  char *unaff_CS;
  
  if ((int)n < 0) {
    unaff_CS = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
    func_0x00083241();
    n = -n;
  }
  func_0x0008142f(unaff_CS,n);
  return;
}



/* print_unsigned_decimal_serial (confidence: Confirmed)
   
   Evidence: `(n)` - converts to decimal via repeated `div 10` into a stack buffer, then prints the
   string via `SUB_82D01` */

void __stdcall16far print_unsigned_decimal_serial(word n)

{
  int iVar1;
  undefined2 unaff_SS;
  
  iVar1 = 5;
  do {
    (&stack0xfff1)[iVar1] = (char)((ulong)n % 10) + '0';
    n = n / 10;
    iVar1 = iVar1 + -1;
  } while (n != 0);
  func_0x00082d01();
  return;
}



void __stdcall16far FUN_0008_1470(int param_1,uint param_2)

{
  byte *pbVar1;
  byte *pbVar2;
  byte *pbVar3;
  int iVar4;
  char in_CL;
  byte bVar5;
  char cVar6;
  uint uVar7;
  undefined2 uVar8;
  undefined2 unaff_DS;
  uint uVar9;
  char local_10;
  char local_e;
  char local_d;
  undefined4 local_c;
  
  pbVar1 = (byte *)*(undefined4 *)0x70e;
  local_c._0_2_ = (byte *)pbVar1;
  local_c._2_2_ = (undefined2)((ulong)pbVar1 >> 0x10);
  local_c = pbVar1;
  if ((param_2 & 1) == 0) {
    if ((param_2 & 4) == 0) {
      if ((param_2 & 0x10) == 0) {
        param_1 = param_1 + 8;
      }
      else {
        local_c = (byte *)CONCAT22(local_c._2_2_,(byte *)local_c + 0xc);
        param_1 = param_1 + 9;
      }
    }
    else if ((param_2 & 2) == 0) {
      if ((param_2 & 0x20) == 0) {
        if ((param_2 & 0x10) == 0) {
          local_c = (byte *)CONCAT22(local_c._2_2_,(byte *)local_c + 6);
          param_1 = param_1 + 3;
          in_CL = '\x02';
        }
        else {
          local_c = (byte *)CONCAT22(local_c._2_2_,(byte *)local_c + 0x23);
          param_1 = param_1 + 4;
          in_CL = '\b';
        }
      }
      else {
        if ((param_2 & 0x10) == 0) {
          local_c._0_2_ = (byte *)local_c + 0xc;
          param_1 = param_1 + 3;
          in_CL = '\x04';
        }
        else {
          local_c._0_2_ = (byte *)local_c + 0x2a;
          param_1 = param_1 + 5;
          in_CL = '\x10';
        }
      }
    }
    else if ((param_2 & 0x10) == 0) {
      param_1 = param_1 + 2;
      in_CL = '\x01';
    }
    else {
      local_c = (byte *)CONCAT22(local_c._2_2_,(byte *)local_c + 0xc);
      param_1 = param_1 + 3;
      in_CL = '\x04';
    }
  }
  else if ((param_2 & 4) == 0) {
    param_1 = param_1 + 0x15;
  }
  else if ((param_2 & 2) == 0) {
    local_c = (byte *)CONCAT22(local_c._2_2_,(byte *)local_c + 0x12);
    param_1 = param_1 + 9;
  }
  else {
    local_c = (byte *)CONCAT22(local_c._2_2_,(byte *)local_c + 0xc);
    param_1 = param_1 + 0x10;
  }
  if (local_d == '\x19') {
    iVar4 = (int)in_CL;
    func_0x0008142f();
    func_0x00082d01(0x802c,0x2ce,(char *)s_Delete_failed_0008_906a + 7,iVar4);
  }
  else {
    cVar6 = (char)((param_1 / 3) % 3);
    local_e = (char)(param_1 % 3);
    while( true ) {
      pbVar1 = local_c;
      pbVar2 = (byte *)local_c;
      if (local_e == '\0') break;
      local_c = (byte *)CONCAT22(local_c._2_2_,(byte *)local_c + *local_c + 1);
      local_e = local_e + -1;
    }
    local_c = (byte *)CONCAT22(local_c._2_2_,(byte *)local_c + 1);
    pbVar3 = local_c;
    bVar5 = *pbVar1;
    uVar7 = (uint)bVar5;
    if (cVar6 == '\0') {
      uVar9 = 0x30;
      func_0x00083241();
    }
    else if (cVar6 == '\x01') {
      local_c = (byte *)CONCAT22(local_c._2_2_,pbVar2 + 2);
      uVar9 = (uint)*pbVar3;
      func_0x00083241();
      uVar7 = (uint)(byte)(bVar5 - 1);
    }
    else {
      local_c = (byte *)CONCAT22(local_c._2_2_,pbVar2 + 2);
      pbVar1 = local_c;
      uVar9 = (uint)*pbVar3;
      func_0x00083241();
      bVar5 = bVar5 - 1;
      uVar7 = (uint)bVar5;
      if (bVar5 == 0) {
        func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x30,uVar9);
      }
      else {
        local_c = (byte *)CONCAT22(local_c._2_2_,pbVar2 + 3);
        func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,*pbVar1,uVar9);
        uVar7 = (uint)(byte)(bVar5 - 1);
      }
    }
    func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x2e,uVar9);
    if ((char)uVar7 == '\0') {
      func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x30);
    }
    else {
      while (pbVar1 = local_c, cVar6 = (char)uVar7, '\0' < cVar6) {
        local_c = (byte *)CONCAT22(local_c._2_2_,(byte *)local_c + 1);
        func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,*pbVar1);
        uVar7 = (uint)(byte)(cVar6 - 1);
      }
    }
    local_10 = (char)(param_1 / 9);
    if ((param_2 & 8) == 0) {
      if (local_10 == '\0') {
        local_e = 'n';
      }
      else if (local_10 == '\x01') {
        local_e = 'u';
      }
      else if (local_10 == '\x02') {
        local_e = 'm';
      }
      else if (local_10 == '\x04') {
        local_e = 'k';
      }
      if (local_10 != '\x03') {
        func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,local_e);
      }
      if ((param_2 & 1) == 0) {
        uVar8 = 0x53;
      }
      else {
        uVar8 = 0x56;
      }
      func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,uVar8);
    }
    else {
      func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x45);
      if (local_10 < '\x03') {
        uVar8 = 0x2d;
      }
      else {
        uVar8 = 0x2b;
      }
      func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,uVar8);
      local_10 = local_10 + -3;
      if (local_10 < '\0') {
        local_10 = -local_10;
      }
      func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,local_10 * 3 + 0x30);
    }
  }
  return;
}



void __stdcall16far FUN_0008_1731(char *param_1,uint param_2)

{
  char cVar1;
  int iVar2;
  undefined2 uVar3;
  uint uVar4;
  char *unaff_CS;
  char *pcVar5;
  char *pcVar6;
  undefined2 unaff_DS;
  char local_b;
  char local_a;
  char cStack_9;
  
  *(undefined2 *)0x646 = 0;
  if (*(char *)0x4 == '%') {
    *(undefined1 *)0x4fe = 1;
  }
  else if (*(char *)0x4 == '#') {
    unaff_CS = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
    func_0x00082fd7();
    if (*(char *)0x4 == 'H') {
      *(undefined1 *)0x4fe = 2;
    }
  }
  else {
    if ((*(byte *)0x4 < 0x30) || (0x39 < *(byte *)0x4)) {
      *(uint *)0x646 = *(uint *)0x646 | 1;
      goto LAB_0008_18e6;
    }
    *(undefined2 *)0x646 = 0x100;
    *(undefined1 *)0x4fe = 0;
    unaff_CS = (char *)0x9687;
    func_0x00096ce9();
  }
  pcVar5 = unaff_CS;
  if (*(char *)0x4fe != '\0') {
    pcVar5 = (char *)0x802c;
    iVar2 = func_0x00081991(unaff_CS);
    local_a = (char)iVar2;
    cStack_9 = (char)((uint)iVar2 >> 8);
    local_b = local_a + cStack_9;
    uVar4 = iVar2 - 1;
    if (uVar4 != param_2) {
      if (uVar4 < param_2) {
        param_2 = uVar4;
      }
      *(uint *)0x646 = *(uint *)0x646 | 0x20;
    }
  }
  if (*"" == '\x02') {
    for (uVar4 = 0; (int)uVar4 < (int)param_2; uVar4 = uVar4 + 2) {
      pcVar6 = (char *)0x802c;
      uVar3 = func_0x00081991(pcVar5);
      pcVar5 = pcVar6;
      if ((*(char *)0x623 != '\0') || ((*(uint *)0x646 & 0x24f) != 0)) break;
      local_a = (char)uVar3;
      cStack_9 = (char)((uint)uVar3 >> 8);
      local_b = local_b + local_a + cStack_9;
      if (*"u\x19=\x01" == 'b') {
        *(undefined2 *)param_1 = uVar3;
      }
      else if ((uVar4 & 2) == 0) {
        *(undefined2 *)((char *)param_1 + 2) = uVar3;
      }
      else {
        *(undefined2 *)((char *)param_1 + -2) = uVar3;
      }
      param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 2);
    }
  }
  else {
    for (uVar4 = 0; (int)uVar4 < (int)param_2; uVar4 = uVar4 + 1) {
      pcVar6 = (char *)0x802c;
      cVar1 = func_0x00081909(pcVar5);
      pcVar5 = pcVar6;
      if ((*(char *)0x623 != '\0') || ((*(uint *)0x646 & 0x24f) != 0)) break;
      local_b = local_b + cVar1;
      if (*"u\x19=\x01" == 'b') {
        *param_1 = cVar1;
      }
      else if ((uVar4 & 1) == 0) {
        ((char *)param_1)[1] = cVar1;
      }
      else {
        ((char *)param_1)[-1] = cVar1;
      }
      param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
    }
  }
  unaff_CS = pcVar5;
  if ((*(char *)0x623 == '\0') && ((*(uint *)0x646 & 0x14f) == 0)) {
    unaff_CS = (char *)0x802c;
    cVar1 = func_0x00081909(pcVar5);
    if ((char)(local_b + cVar1) != '\0') {
      *(uint *)0x646 = *(uint *)0x646 | 0x40;
    }
    if (*(char *)0x4fe != *(char *)((int)*(undefined4 *)0x73a + 0x2f)) {
      *(uint *)0x646 = *(uint *)0x646 | 0x10;
    }
  }
LAB_0008_18e6:
  pcVar5 = unaff_CS;
  if (*(char *)0x2 == '\0') {
    pcVar5 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
    func_0x00082fd7(unaff_CS);
  }
  if ((*(uint *)0x646 & 0x7f) != 0) {
    func_0x00081a5b(pcVar5);
  }
  return;
}



uint __cdecl16far FUN_0008_1909(void)

{
  uint unaff_SI;
  undefined2 unaff_DS;
  
  if (*(char *)0x4fe == '\x01') {
    func_0x0009699b();
    if ((*(char *)0x2 != '\0') && ((*(byte *)0x4f1 & 0x20) != 0)) {
      func_0x00096de5(0x9687);
    }
    unaff_SI = (uint)*(byte *)0x4;
    func_0x000968a2(0x9687);
  }
  else if (*(char *)0x4fe == '\x02') {
    func_0x00082fd7();
    if (*(char *)0x623 == '\0') {
      unaff_SI = func_0x00082f3a((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
    }
    func_0x000968a2((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
  }
  else {
    func_0x000819f0();
    if ((*(uint *)0x646 & 0x24f) == 0) {
      unaff_SI = func_0x000836f4(0x802c);
      if (0xff < unaff_SI) {
        *(uint *)0x646 = *(uint *)0x646 | 2;
      }
    }
    else {
      unaff_SI = 0;
    }
  }
  return unaff_SI;
}



undefined2 __cdecl16far FUN_0008_1991(void)

{
  undefined1 uVar1;
  undefined1 uVar2;
  undefined2 unaff_DS;
  undefined2 local_a;
  
  if (*(char *)0x4fe == '\0') {
    func_0x000819f0();
    if ((*(uint *)0x646 & 0x24f) == 0) {
      local_a = func_0x000836f4(0x802c);
    }
    else {
      local_a = 0;
    }
  }
  else {
    uVar1 = func_0x00081909();
    if ((*(char *)0x623 == '\0') && ((*(uint *)0x646 & 0x4f) == 0)) {
      uVar2 = func_0x00081909(0x802c);
      local_a = CONCAT11(uVar1,uVar2);
    }
    else {
      local_a = 0;
    }
  }
  return local_a;
}



void __cdecl16far FUN_0008_19f0(void)

{
  char *unaff_CS;
  undefined2 unaff_DS;
  bool bVar1;
  char local_9;
  
  local_9 = '\0';
  while( true ) {
    if ((*(char *)0x2 != '\0') || (*(char *)0x4 == ';')) {
      *(uint *)0x646 = *(uint *)0x646 | 0x200;
      return;
    }
    if ((0x2f < *(byte *)0x4) && (*(byte *)0x4 < 0x3a)) break;
    if (*(char *)0x4 == ',') {
      bVar1 = local_9 != '\0';
      local_9 = local_9 + '\x01';
      if (bVar1) {
        *(uint *)0x646 = *(uint *)0x646 | 4;
        return;
      }
    }
    else if (*(char *)0x4 != ' ') {
      *(uint *)0x646 = *(uint *)0x646 | 8;
      return;
    }
    func_0x000968a2(unaff_CS);
    unaff_CS = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
    func_0x00082fd7(0x9687);
  }
  return;
}



void __cdecl16far FUN_0008_1a5b(void)

{
  undefined2 unaff_DS;
  
  if ((*(uint *)0x646 & 0x4f) != 0) {
    if ((*(uint *)0x646 & 1) == 0) {
      if ((*(uint *)0x646 & 2) == 0) {
        if ((*(uint *)0x646 & 4) == 0) {
          if ((*(uint *)0x646 & 8) == 0) {
            if ((*(uint *)0x646 & 0x40) != 0) {
              *(undefined1 *)0x623 = 9;
            }
          }
          else {
            *(undefined1 *)0x623 = 5;
          }
        }
        else {
          *(undefined1 *)0x623 = 7;
        }
      }
      else {
        *(undefined1 *)0x623 = 0x15;
      }
    }
    else {
      *(undefined1 *)0x623 = 0xf;
    }
  }
  if ((*(uint *)0x646 & 0x30) != 0) {
    func_0x00085301();
  }
  return;
}



undefined2 __stdcall16far FUN_0008_1d22(undefined4 param_1)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  int iVar4;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  iVar4 = iVar2;
  uVar1 = uVar3;
  func_0x00081dd7();
  if (*(int *)0x68a == 0) {
    if ((*(uint *)(iVar2 + 10) & 0xff00) != 0) {
      *(undefined1 *)0x623 = 4;
      return 1;
    }
    if ((*(uint *)(iVar2 + 10) & 0xff) != 0) {
      *(undefined1 *)0x623 = 0x1a;
      return 1;
    }
  }
  else {
    func_0x000821f7(0x81ae,*(undefined2 *)0x68a,iVar4,uVar1);
  }
  func_0x00081f68(0x81ae,iVar2,uVar3);
  func_0x00082257(0x81ae,iVar2,uVar3);
  if ((*(uint *)(iVar2 + 10) & 0xff00) == 0) {
    if ((*(uint *)(iVar2 + 10) & 0xff) == 0) {
      func_0x00082043(0x81ae,iVar2,uVar3);
      uVar1 = 0;
    }
    else {
      *(undefined1 *)0x623 = 0x1a;
      uVar1 = 1;
    }
  }
  else {
    *(undefined1 *)0x623 = 4;
    uVar1 = 1;
  }
  return uVar1;
}



void __stdcall16far FUN_0008_1dd7(undefined4 param_1)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 local_a;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  *(undefined2 *)(iVar1 + 10) = 0;
  if (((*"u\x19=\x01" != 'a') && (*"u\x19=\x01" != '\x1f')) && (*"u\x19=\x01" != 'b')) {
    *(uint *)(iVar1 + 10) = *(uint *)(iVar1 + 10) | 1;
  }
  if ((*(char *)((int)*(undefined4 *)0x73a + 0x34) != '\0') &&
     (*(char *)((int)*(undefined4 *)0x73a + 0x34) != '\x01')) {
    *(uint *)(iVar1 + 10) = *(uint *)(iVar1 + 10) | 0x200;
  }
  if ((*"\x01" != '\b') && (*"\x01" != '\x10')) {
    *(uint *)(iVar1 + 10) = *(uint *)(iVar1 + 10) | 0x20;
  }
  if ((*"\x01" == '\b') && (*"" == '\x02')) {
    *(uint *)(iVar1 + 10) = *(uint *)(iVar1 + 10) | 0x20;
  }
  if ((*"\x01" == '\x10') && (*"" == '\x01')) {
    *(uint *)(iVar1 + 10) = *(uint *)(iVar1 + 10) | 0x20;
  }
  if ((*"" != '\x01') && (*"" != '\x02')) {
    *(uint *)(iVar1 + 10) = *(uint *)(iVar1 + 10) | 0x10;
  }
  if (((*"u\x19=\x01" == 'a') || (*"u\x19=\x01" == '\x1f')) && (*"" == '\x02')) {
    *(uint *)(iVar1 + 10) = *(uint *)(iVar1 + 10) | 0x10;
  }
  if (((*(char *)((int)*(undefined4 *)0x732 + 0x1f) != '\x1e') && (*(int *)0x55c < 0x401)) &&
     (*(int *)0x55c = *(int *)0x55c << 2, *(int *)0x55e < 0)) {
    *(undefined2 *)0x55e = 0;
  }
  if (*"u\x19=\x01" == 'b') {
    local_a = *(int *)0x55c;
  }
  else {
    local_a = *(int *)0x55c << 1;
  }
  if (*"u\x19=\x01" == 'a') {
    if ((local_a != 0x400) && (local_a != 0x1000)) {
      *(uint *)(iVar1 + 10) = *(uint *)(iVar1 + 10) | 0x80;
    }
  }
  else if (((local_a != 0x200) && (local_a != 0x400)) && ((local_a != 0x800 && (local_a != 0x1000)))
          ) {
    *(uint *)(iVar1 + 10) = *(uint *)(iVar1 + 10) | 0x80;
  }
  if ((4 < *(byte *)0x4f5) || (*(char *)0x4f5 == '\0')) {
    *(uint *)(iVar1 + 10) = *(uint *)(iVar1 + 10) | 0x100;
  }
  if (*(char *)0x4f5 == '\x04') {
    if (local_a < 0x800) {
      *(uint *)(iVar1 + 10) = *(uint *)(iVar1 + 10) | 0x100;
    }
  }
  else if (0x400 < local_a) {
    *(uint *)(iVar1 + 10) = *(uint *)(iVar1 + 10) | 0x100;
  }
  return;
}



void __stdcall16far FUN_0008_1f68(byte *param_1)

{
  byte *pbVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  int local_a;
  
  *param_1 = 0;
  if (*"u\x19=\x01" == 'b') {
    local_a = *(int *)0x55c;
  }
  else {
    local_a = *(int *)0x55c << 1;
  }
  if ((local_a == 0x200) || (local_a == 0x800)) {
    *param_1 = *param_1 | 2;
  }
  if ((local_a == 0x800) || (local_a == 0x1000)) {
    *param_1 = *param_1 | 1;
  }
  if (*"" == '\x02') {
    *param_1 = *param_1 | 4;
  }
  if (*"u\x19=\x01" == '\x1f') {
    *param_1 = *param_1 | 0x10;
  }
  if (*"u\x19=\x01" == 'a') {
    *param_1 = *param_1 | 0x20;
  }
  else {
    *param_1 = *param_1 | 8;
    if (*(char *)((int)*(undefined4 *)0x73a + 0x34) == '\0') {
      *param_1 = *param_1 | 0x20;
    }
  }
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  pbVar1 = (byte *)param_1;
  *(uint *)(pbVar1 + 0xe) = (uint)*(byte *)0x55b * local_a;
  *(undefined2 *)(pbVar1 + 0x10) = *(undefined2 *)0x562;
  *(undefined2 *)(pbVar1 + 0x12) = *(undefined2 *)0x560;
  *(undefined2 *)(pbVar1 + 0x14) = *(undefined2 *)0x55e;
  func_0x000f9710();
  return;
}



void __stdcall16far FUN_0008_2043(byte *param_1)

{
  int iVar1;
  byte *pbVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  undefined2 uVar4;
  byte local_11;
  undefined1 *local_c;
  
  _local_c = (undefined1 *)CONCAT22(unaff_DS,(undefined1 *)0x500);
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  pbVar2 = (byte *)param_1;
  pbVar2[1] = 0;
  if ((*param_1 & 2) == 0) {
    *param_1 = *param_1 | 0x40;
  }
  else {
    if (*(char *)0x4f5 == '\x04') {
      local_11 = 1;
    }
    else {
      local_11 = *(byte *)0x4f5;
    }
    iVar1 = (local_11 & 0x7f) * 2;
    if ((*(char *)(iVar1 + (int)*(undefined4 *)0x732 + 0x7e) == '\0') &&
       (*(char *)(iVar1 + (int)*(undefined4 *)0x732 + 0x7f) == '\0')) {
      *param_1 = *param_1 | 0x40;
    }
    else {
      uVar4 = unaff_DS;
      func_0x00086b4b();
      func_0x00082121(0x85eb,pbVar2,uVar3,*_local_c,uVar4);
      if (*(byte *)0x502 != pbVar2[4]) {
        pbVar2[1] = pbVar2[1] | 8;
      }
      if (*(int *)0x504 != *(int *)(pbVar2 + 0xc)) {
        pbVar2[1] = pbVar2[1] | 0x20;
      }
      if (pbVar2[1] != 0) {
        *param_1 = *param_1 | 0x40;
      }
    }
  }
  return;
}



void __stdcall16far FUN_0008_2121(byte *param_1,byte param_2)

{
  byte bVar1;
  byte *pbVar2;
  undefined2 uVar3;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  pbVar2 = (byte *)param_1;
  bVar1 = *param_1;
  if (((bVar1 & 1) != 0) && ((param_2 & 1) == 0)) {
    pbVar2[1] = pbVar2[1] | 4;
  }
  if (((bVar1 & 1) == 0) && ((param_2 & 1) != 0)) {
    pbVar2[1] = pbVar2[1] | 4;
  }
  if (((bVar1 & 8) == 0) && ((param_2 & 0x20) != 0)) {
    pbVar2[1] = pbVar2[1] | 0x10;
  }
  if (((bVar1 & 8) != 0) && ((param_2 & 0x20) == 0)) {
    pbVar2[1] = pbVar2[1] | 0x10;
  }
  if ((param_2 & 4) == 0) {
    pbVar2[1] = pbVar2[1] | 1;
  }
  if (((bVar1 & 4) != 0) && ((param_2 & 0x10) == 0)) {
    pbVar2[1] = pbVar2[1] | 2;
  }
  if (((bVar1 & 4) == 0) && ((param_2 & 0x10) != 0)) {
    pbVar2[1] = pbVar2[1] | 2;
  }
  return;
}



/* init_comm_default_params (confidence: Mechanism confirmed; exact parameter meanings not
   confirmed)
   
   Evidence: `(mode)` - no-op unless `mode==1`; sets `[0x4F5]` (device-type select) based on a
   status byte, then initializes a batch of comm parameter defaults */

void __stdcall16far init_comm_default_params(word mode)

{
  undefined1 uVar1;
  undefined2 unaff_DS;
  
  if (mode == 1) {
    if (*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x1e') {
      uVar1 = 1;
    }
    else {
      uVar1 = 4;
    }
    *(undefined1 *)0x4f5 = uVar1;
    *(undefined1 *)0x557 = 0x62;
    *(undefined2 *)0x55c = 0x400;
    *(undefined1 *)0x55b = 1;
    *(undefined1 *)0x55a = 8;
    *(undefined2 *)0x560 = 0;
    *(undefined2 *)0x562 = 0;
    *(undefined2 *)0x55e = 0;
    *(undefined1 *)0x559 = 0x55;
  }
  return;
}



void __stdcall16far FUN_0008_2257(byte *param_1)

{
  undefined2 uVar1;
  undefined1 uVar2;
  byte bVar3;
  uint uVar4;
  byte *pbVar5;
  undefined2 uVar6;
  undefined2 unaff_DS;
  byte local_d;
  byte local_c;
  int local_a;
  
  bVar3 = *param_1;
  local_d = 0;
  uVar6 = *(undefined2 *)0x56c;
  *(undefined2 *)0x578 = *(undefined2 *)0x56a;
  *(undefined2 *)0x57a = uVar6;
  *(undefined2 *)0x57c = *(undefined2 *)0x574;
  *(undefined1 *)0x57e = 0;
  if (((bVar3 & 0x12) == 0) && ((bVar3 & 8) != 0)) {
    local_c = 3;
  }
  else if (((bVar3 & 2) == 0) || (((bVar3 & 0x10) == 0 && ((bVar3 & 8) != 0)))) {
    local_c = 4;
  }
  else {
    local_c = 7;
  }
  uVar4 = (uint)local_c;
  local_a = func_0x0008373a();
  if ((local_a == -1) && ((bVar3 & 1) == 0)) {
    local_d = 1;
    uVar6 = *(undefined2 *)0x56c;
    *(undefined2 *)0x578 = *(undefined2 *)0x56a;
    *(undefined2 *)0x57a = uVar6;
    *(undefined2 *)0x57c = *(undefined2 *)0x574;
    *(undefined1 *)0x57e = 0;
    if (local_c == 7) {
      local_c = '\b';
    }
    else {
      local_c = local_c + 2;
    }
    local_a = func_0x0008373a((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,local_c,uVar4);
  }
  pbVar5 = (byte *)param_1;
  uVar6 = (undefined2)((ulong)param_1 >> 0x10);
  if (local_a == -1) {
    *(uint *)(pbVar5 + 10) = *(uint *)(pbVar5 + 10) | 2;
  }
  pbVar5[5] = local_d;
  pbVar5[4] = (byte)local_a;
  if ((bVar3 & 4) == 0) {
    uVar2 = 1;
  }
  else {
    uVar2 = 2;
  }
  if ((bVar3 & 8) == 0) {
    uVar1 = *(undefined2 *)0x568;
    *(undefined2 *)0x578 = *(undefined2 *)0x566;
    *(undefined2 *)0x57a = uVar1;
    *(undefined2 *)0x57c = *(undefined2 *)0x572;
    *(undefined1 *)0x57e = 0;
    bVar3 = func_0x0008373a((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,uVar2,uVar4);
    pbVar5[7] = bVar3;
    if (bVar3 == 0xffff) {
      *(uint *)(pbVar5 + 10) = *(uint *)(pbVar5 + 10) | 8;
    }
  }
  uVar1 = *(undefined2 *)0x570;
  *(undefined2 *)0x578 = *(undefined2 *)0x56e;
  *(undefined2 *)0x57a = uVar1;
  *(undefined2 *)0x57c = *(undefined2 *)(code *)FUN_0008_04f6;
  *(undefined1 *)0x57e = 0;
  bVar3 = func_0x0008373a((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,uVar2,uVar4);
  pbVar5[6] = bVar3;
  if (bVar3 == 0xffff) {
    *(uint *)(pbVar5 + 10) = *(uint *)(pbVar5 + 10) | 4;
  }
  pbVar5[8] = 4;
  if (*"=\x01" == 'U') {
    pbVar5[8] = pbVar5[8] | 1;
  }
  return;
}



void __cdecl16far FUN_0008_23f5(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2);
  return;
}



void __cdecl16far FUN_0008_2468(void)

{
  byte bVar1;
  char cVar2;
  char cVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  byte local_b;
  
  uVar5 = 0x41;
  uVar4 = func_0x0009470e();
  local_b = *(byte *)0x1b64;
  bVar1 = *(byte *)0x1b72;
  cVar2 = *(char *)0x1b78;
  cVar3 = *(char *)0x1b70;
  func_0x0009470e(38000,uVar4,uVar5);
  if (local_b == 0x16) {
    func_0x00082d01(38000,0x24e,(char *)s_Delete_failed_0008_906a + 7);
    func_0x00085301((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x36);
  }
  else if (cVar2 == '\0') {
    func_0x00081470(38000,local_b,8);
    if ((bVar1 & 0x10) != 0) {
      func_0x00085301(0x802c,0x37);
    }
  }
  else {
    if (((bVar1 & 0x40) != 0) && (0x12 < local_b)) {
      local_b = local_b + 3;
    }
    if (((bVar1 & 0x10) == 0) || (cVar3 != '\x04')) {
      func_0x00081470(38000,local_b,8);
    }
    else {
      func_0x00081470(38000,local_b,0x18);
    }
  }
  return;
}



void __cdecl16far FUN_0008_253d(void)

{
  char cVar1;
  undefined1 uVar2;
  byte bVar3;
  char cVar4;
  char cVar5;
  undefined2 uVar6;
  undefined2 unaff_DS;
  undefined2 uVar7;
  
  uVar7 = 0x41;
  uVar6 = func_0x0009470e();
  cVar1 = *(char *)0x1b64;
  uVar2 = *(undefined1 *)0x1b67;
  bVar3 = *(byte *)0x1b72;
  cVar4 = *(char *)0x1b78;
  cVar5 = *(char *)0x1b70;
  func_0x0009470e(38000,uVar6,uVar7);
  if (cVar1 == '\x16') {
    func_0x00082d01(38000,0x255,(char *)s_Delete_failed_0008_906a + 7);
    func_0x00085301((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x36);
  }
  else if (cVar4 == '\0') {
    func_0x00081470(38000,uVar2,8);
    if ((bVar3 & 0x10) != 0) {
      func_0x00085301(0x802c,0x37);
    }
  }
  else if (((bVar3 & 0x10) == 0) || (cVar5 != '\x04')) {
    func_0x00081470(38000,uVar2,8);
  }
  else {
    func_0x00081470(38000,uVar2,0x18);
  }
  return;
}



void __cdecl16far FUN_0008_2603(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2);
  return;
}



void __cdecl16far FUN_0008_264f(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2);
  return;
}



void __cdecl16far FUN_0008_26a0(void)

{
  byte bVar1;
  char cVar2;
  byte bVar3;
  int iVar4;
  char *pcVar5;
  undefined2 unaff_DS;
  undefined2 uVar6;
  undefined1 local_12 [16];
  
  uVar6 = 0x41;
  func_0x0009470e();
  bVar1 = *(byte *)0x1b72;
  iVar4 = *(int *)0x570;
  cVar2 = *(char *)0x1b78;
  bVar3 = *(byte *)0x1b75;
  pcVar5 = (char *)0x9470;
  func_0x0009470e(38000,(char *)s_TRIggerd_0008_8ef8 + 8,uVar6);
  if ((((bVar1 & 2) == 0) || ((iVar4 != 0 && (cVar2 != '\0')))) ||
     ((cVar2 == '\0' && ((bVar3 & 8) != 0)))) {
    *(undefined1 *)0x623 = 0x1b;
  }
  else {
    while (*(char *)0x6bb != '\0') {
      func_0x000838c0(pcVar5);
      pcVar5 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
    }
    func_0x000eac86(pcVar5,0x6b1);
    func_0x000827db(0xea34,local_12);
    func_0x00082d01(0x823f,local_12);
  }
  return;
}



void __cdecl16far FUN_0008_273d(void)

{
  char cVar1;
  byte bVar2;
  byte bVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  undefined2 uVar6;
  
  uVar6 = 0x41;
  uVar5 = func_0x0009470e();
  cVar1 = *(char *)0x1b78;
  bVar2 = *(byte *)0x1b72;
  iVar4 = *(int *)0x570;
  bVar3 = *(byte *)0x1b75;
  func_0x0009470e(38000,uVar5,uVar6);
  if ((((bVar2 & 2) == 0) || ((iVar4 != 0 && (cVar1 != '\0')))) ||
     ((cVar1 == '\0' && ((bVar3 & 8) != 0)))) {
    *(undefined1 *)0x623 = 0x1b;
  }
  return;
}



void __stdcall16far FUN_0008_27db(char *param_1)

{
  char *pcVar1;
  char *pcVar2;
  char *pcVar3;
  char cVar4;
  undefined2 unaff_SS;
  int local_1a;
  char local_18 [8];
  char local_10 [14];
  
  func_0x000eac86();
  func_0x000eac86(0xea34,0x264,(char *)s_Delete_failed_0008_906a + 7,local_18);
  do {
    pcVar2 = param_1;
    pcVar1 = (char *)param_1;
    param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
  } while (*pcVar2 != '\0');
  param_1 = (char *)CONCAT22(param_1._2_2_,pcVar1 + -2);
  for (local_1a = 0; (*param_1 != local_10[local_1a] && (local_1a < 6)); local_1a = local_1a + 1) {
  }
  if (5 < local_1a) {
    local_1a = 3;
    param_1 = (char *)CONCAT22(param_1._2_2_,pcVar1 + -1);
  }
  pcVar2 = param_1;
  pcVar1 = (char *)param_1;
  param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
  pcVar3 = param_1;
  *pcVar2 = 'E';
  if (local_1a < 3) {
    cVar4 = '-';
  }
  else {
    cVar4 = '+';
  }
  param_1 = (char *)CONCAT22(param_1._2_2_,pcVar1 + 2);
  pcVar2 = param_1;
  *pcVar3 = cVar4;
  param_1 = (char *)CONCAT22(param_1._2_2_,pcVar1 + 3);
  *pcVar2 = local_18[local_1a];
  *param_1 = '\0';
  return;
}



void __cdecl16far FUN_0008_2887(void)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  
  uVar5 = 0x41;
  uVar4 = func_0x0009470e();
  bVar1 = *(byte *)0x1b6a;
  bVar2 = *(byte *)0x1b69;
  bVar3 = *(byte *)0x1b68;
  func_0x0009470e(38000,uVar4,uVar5);
  if (bVar2 < 0x80) {
    uVar5 = 0x802c;
    func_0x00081470(38000,(uint)bVar1 + (uint)bVar2,9);
    if ((bVar3 & 1) == 0) {
      uVar5 = 0x8511;
      func_0x00085301(0x802c,0x37);
    }
  }
  else {
    func_0x00081470(38000,bVar1,9);
    if ((bVar3 & 1) == 0) {
      uVar5 = 0x8511;
      func_0x00085301(0x802c,0x37);
    }
    else {
      uVar5 = 0x8511;
      func_0x00085301(0x802c,0x35);
    }
  }
  func_0x0009470e(uVar5,uVar4);
  return;
}



void __cdecl16far FUN_0008_292b(void)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  
  uVar5 = 0x41;
  uVar4 = func_0x0009470e();
  bVar1 = *(byte *)0x1b6e;
  bVar2 = *(byte *)0x1b6d;
  bVar3 = *(byte *)0x1b6b;
  func_0x0009470e(38000,uVar4,uVar5);
  if (bVar2 < 0x80) {
    func_0x00081470(38000,(uint)bVar1 + (uint)bVar2,9);
    if ((bVar3 & 1) == 0) {
      func_0x00085301(0x802c,0x37);
    }
  }
  else {
    func_0x00081470(38000,bVar1,9);
    if ((bVar3 & 1) == 0) {
      func_0x00085301(0x802c,0x37);
    }
    else {
      func_0x00085301(0x802c,0x35);
    }
  }
  return;
}



void __cdecl16far FUN_0008_29c7(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2);
  return;
}



void __cdecl16far FUN_0008_2a3a(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2);
  return;
}



void __cdecl16far FUN_0008_2aad(void)

{
  undefined2 uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 local_e;
  undefined2 local_c;
  
  uVar3 = 0x41;
  uVar1 = func_0x0009470e();
  uVar2 = (uint)*(byte *)0x1b69;
  func_0x0009470e(38000,uVar1,uVar3);
  if (uVar2 < 0x80) {
    local_c = 1;
    local_e = uVar2 / 3;
    while (0 < (int)local_e) {
      local_c = local_c * 10;
      local_e = local_e - 1;
    }
    func_0x00081404(38000,local_c);
  }
  else {
    func_0x00081404(38000,uVar2 - 0x100);
  }
  return;
}



void __cdecl16far FUN_0008_2b21(void)

{
  undefined2 uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 local_e;
  undefined2 local_c;
  
  uVar3 = 0x41;
  uVar1 = func_0x0009470e();
  uVar2 = (uint)*(byte *)0x1b6d;
  func_0x0009470e(38000,uVar1,uVar3);
  if (uVar2 < 0x80) {
    local_c = 1;
    local_e = uVar2 / 3;
    while (0 < (int)local_e) {
      local_c = local_c * 10;
      local_e = local_e - 1;
    }
    func_0x00081404(38000,local_c);
  }
  else {
    func_0x00081404(38000,uVar2 - 0x100);
  }
  return;
}



void __cdecl16far FUN_0008_2b95(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2);
  return;
}



void __cdecl16far FUN_0008_2be1(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2);
  return;
}



void __stdcall16far FUN_0008_2cd7(byte param_1)

{
  undefined2 unaff_DS;
  
  *(char *)0x5 = (param_1 & 0xf) + 0x30;
  if (0x39 < *(byte *)0x5) {
    *(char *)0x5 = *(char *)0x5 + '\a';
  }
  func_0x00096b68();
  return;
}



/* print_string_serial (confidence: Confirmed)
   
   Evidence: `(far_str_ptr)` - loops sending each byte via `serial_tx_buffer_put` until a null
   terminator */

void __stdcall16far print_string_serial(word str_off,word str_seg)

{
  char *pcVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  while (pcVar1 = _str_off, *_str_off != '\0') {
    _str_off = (char *)CONCAT22(str_seg,(char *)(str_off + 1));
    *(char *)0x5 = *pcVar1;
    func_0x00096b68(unaff_CS);
    unaff_CS = 0x9687;
  }
  return;
}



/* print_padded_label (confidence: Confirmed)
   
   Evidence: Prints a run of `si` characters (from a table at `[0x6EE]` or a far pointer `[0x60A]`,
   per `[0x4F2]`), uppercasing lowercase letters, then a trailing space */

void __cdecl16far print_padded_label(void)

{
  undefined1 *puVar1;
  uint uVar2;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (*(char *)0x4f2 == '\0') {
    uVar2 = (uint)*(byte *)((uint)*(byte *)0x608 + (int)*(undefined4 *)0x6ee);
  }
  else {
    uVar2 = *(byte *)*(undefined4 *)0x60a & 0xf;
  }
  *(int *)0x60a = *(int *)0x60a + 1;
  while (uVar2 != 0) {
    puVar1 = (undefined1 *)*(undefined4 *)0x60a;
    *(int *)0x60a = *(int *)0x60a + 1;
    *(undefined1 *)0x5 = *puVar1;
    if (0x60 < *(byte *)0x5) {
      *(byte *)0x5 = *(byte *)0x5 & 0x5f;
    }
    func_0x00096b68(unaff_CS);
    uVar2 = uVar2 - 1;
    unaff_CS = 0x9687;
  }
  *(undefined1 *)0x5 = 0x20;
  func_0x00096b68(unaff_CS);
  return;
}



void __stdcall16far FUN_0008_2d9b(byte *param_1)

{
  byte *pbVar1;
  uint uVar2;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  pbVar1 = param_1;
  param_1 = (byte *)CONCAT22(param_1._2_2_,(byte *)param_1 + 1);
  uVar2 = *pbVar1 & 0xf;
  do {
    pbVar1 = param_1;
    if ((int)uVar2 < 1) {
      return;
    }
    param_1 = (byte *)CONCAT22(param_1._2_2_,(byte *)param_1 + 1);
    *(byte *)0x5 = *pbVar1;
    if ((0x60 < *(byte *)0x5) || (*(char *)0x5 == '/')) {
      if (*(char *)0x4f2 == '\0') {
        return;
      }
      if ((0x60 < *(byte *)0x5) && (*(byte *)0x5 < 0x7b)) {
        *(byte *)0x5 = *(byte *)0x5 & 0xdf;
      }
    }
    func_0x00096b68(unaff_CS);
    uVar2 = uVar2 - 1;
    unaff_CS = 0x9687;
  } while( true );
}



void __cdecl16far FUN_0008_2dff(void)

{
  byte bVar1;
  undefined1 *puVar2;
  uint uVar3;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  bVar1 = *(byte *)*(undefined4 *)0x60a;
  *(int *)0x60a = *(int *)0x60a + 1;
  uVar3 = bVar1 & 0xf;
  while (uVar3 != 0) {
    puVar2 = (undefined1 *)*(undefined4 *)0x60a;
    *(int *)0x60a = *(int *)0x60a + 1;
    *(undefined1 *)0x5 = *puVar2;
    func_0x00096b68(unaff_CS);
    uVar3 = uVar3 - 1;
    unaff_CS = 0x9687;
  }
  return;
}



void __cdecl16far FUN_0008_2e3d(void)

{
  undefined2 unaff_DS;
  
  if (*"\t" == '\0') {
    func_0x00082d01();
  }
  return;
}



void __stdcall16far FUN_0008_2e5b(void)

{
  int iVar1;
  char cVar2;
  int iVar3;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined1 uVar4;
  
  iVar1 = 10;
  do {
    iVar3 = iVar1;
    cVar2 = func_0x000e7866(unaff_CS,10,0);
    (&stack0xffeb)[iVar3] = cVar2 + '0';
    uVar4 = 0;
    func_0x000e783d(0xe772,10,0,0,0);
    unaff_CS = 0xe772;
    func_0x000e7753(0xe772);
    iVar1 = iVar3 + -1;
  } while ((bool)uVar4);
  func_0x00082d01(0xe772,&stack0xffeb + iVar3);
  return;
}



/* print_param_list_response (confidence: Confirmed)
   
   Evidence: Loops calling `get_next_param_value` and printing each result via
   `print_signed_decimal_serial`, comma-separated, terminated by `;` - IEEE-488/GPIB query-response
   value-list formatting */

void __cdecl16far print_param_list_response(void)

{
  undefined4 uVar1;
  undefined1 uVar2;
  undefined2 unaff_DS;
  
  uVar1 = *(undefined4 *)0x6e6;
  *(int *)0x60a = (int)uVar1 + 0xfe;
  *(undefined2 *)0x60c = (int)((ulong)uVar1 >> 0x10);
  *(undefined1 *)0x608 = 0;
  func_0x00082d2b();
  while( true ) {
    uVar2 = func_0x00082f14((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
    func_0x00081404((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,uVar2);
    if (*(char *)0x580 == *(char *)0x590) break;
    func_0x00083241(0x802c,0x2c);
  }
  func_0x00083241(0x802c,0x3b);
  return;
}



/* get_next_param_value (confidence: Mechanism confirmed; exact parameter domain not confirmed)
   
   Evidence: Saves the previous `[0x580]` into `[0x581]`, fetches a value via `SUB_97905([0x57F])`,
   returns it masked to a byte */

undefined1 __cdecl16far get_next_param_value(void)

{
  undefined1 uVar1;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x581 = *(undefined1 *)0x580;
  uVar1 = *(undefined1 *)0x57f;
  func_0x00097905();
  return uVar1;
}



int __cdecl16far FUN_0008_2f3a(void)

{
  int iVar1;
  byte bVar2;
  undefined2 unaff_DS;
  
  func_0x00082f97();
  if (*(char *)0x623 == '\0') {
    bVar2 = *(byte *)0x4;
    if (*(char *)0x2 == '\0') {
      func_0x00082fd7((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
      func_0x00082f97((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
      if (*(char *)0x623 == '\0') {
        iVar1 = (uint)bVar2 * 0x10 + (uint)*(byte *)0x4;
      }
      else {
        iVar1 = 0;
      }
    }
    else {
      *(undefined1 *)0x623 = 4;
      iVar1 = 0;
    }
  }
  else {
    iVar1 = 0;
  }
  return iVar1;
}



void __cdecl16far FUN_0008_2f97(void)

{
  undefined2 unaff_DS;
  
  if ((*(byte *)0x4 < 0x30) || (0x39 < *(byte *)0x4)) {
    if ((*(byte *)0x4 < 0x41) || (0x46 < *(byte *)0x4)) {
      *(undefined1 *)0x623 = 0xc;
      *(undefined1 *)0x616 = 0xff;
    }
    else {
      *(char *)0x4 = *(char *)0x4 + -0x37;
    }
  }
  else {
    *(char *)0x4 = *(char *)0x4 + -0x30;
  }
  return;
}



void __cdecl16far FUN_0008_2fd7(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x616 = 0;
  func_0x0009699b();
  if (*"\t" != '\0') {
    *(byte *)0x4 = *(byte *)0x4 & 0x7f;
  }
  if ((*(byte *)0x4 < 0x61) || (0x7a < *(byte *)0x4)) {
    if (*(char *)0x4 == '\x7f') {
      *(undefined1 *)0x616 = 0xff;
    }
  }
  else {
    *(byte *)0x4 = *(byte *)0x4 & 0x5f;
  }
  if ((*(byte *)0x4 < 0x40) &&
     ((((*(byte *)0x4 < 0x21 || (*(char *)0x4 == ',')) || (*(char *)0x4 == ';')) ||
      ((*(char *)0x4 == ':' || (*(char *)0x4 == '?')))))) {
    *(undefined1 *)0x616 = 0xff;
  }
  return;
}



void __cdecl16far FUN_0008_3048(void)

{
  undefined1 *puVar1;
  byte *pbVar2;
  byte bVar3;
  char cVar4;
  char *unaff_CS;
  char *pcVar5;
  char *pcVar6;
  undefined2 unaff_DS;
  int local_a;
  
  do {
    if (*(char *)*(undefined4 *)0x60e == '\0') {
LAB_0008_31e7:
      *(undefined1 *)0x607 = 0;
      return;
    }
    puVar1 = (undefined1 *)*(undefined4 *)0x60e;
    *(int *)0x60e = *(int *)0x60e + 1;
    *(undefined1 *)0x607 = *puVar1;
    bVar3 = *(byte *)0x607 & 0xf;
    local_a = 0;
    while( true ) {
      pcVar5 = unaff_CS;
      if ((bVar3 == 0) || (*(char *)0x2 != '\0')) goto LAB_0008_30ac;
      pcVar5 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
      func_0x00082fd7(unaff_CS);
      if (*(char *)0x616 != '\0') goto LAB_0008_30ac;
      bVar3 = bVar3 - 1;
      pbVar2 = (byte *)*(undefined4 *)0x60e;
      *(int *)0x60e = *(int *)0x60e + 1;
      if (((*(byte *)0x4 ^ *pbVar2) & 0x5f) != 0) break;
      local_a = -1;
      unaff_CS = pcVar5;
    }
    local_a = 0;
LAB_0008_30ac:
    pcVar6 = pcVar5;
    if (local_a != 0) {
      if ((*(char *)0x2 != '\0') && (*(char *)0x616 == '\0')) {
        if (*(char *)0x608 == '=') {
          *(undefined1 *)0x609 = 0;
          *(undefined1 *)0x607 = 0;
        }
        func_0x000968a2(pcVar5);
        return;
      }
      if (bVar3 == 0) {
        pcVar6 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
        func_0x00082fd7(pcVar5);
        if (*(char *)0x608 == '=') {
          if (((0x30 < *(byte *)0x4) && (*(byte *)0x4 < 0x35)) ||
             ((0x40 < *(byte *)0x4 && (*(byte *)0x4 < 0x5b)))) {
            if (*(byte *)0x4 < 0x35) {
              cVar4 = '0';
            }
            else {
              cVar4 = '<';
            }
            *(char *)0x609 = *(char *)0x4 - cVar4;
            func_0x000968a2((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
            if (*(char *)0x2 != '\0') {
              return;
            }
            func_0x00082fd7(0x9687);
            if ((*(char *)0x616 != '\0') || (*(char *)0x2 != '\0')) {
              func_0x000968a2((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
              return;
            }
            goto LAB_0008_31e7;
          }
          if ((*(char *)0x616 != '\0') || (*(char *)0x2 != '\0')) {
            *(undefined1 *)0x609 = 0;
            func_0x000968a2((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
            *(undefined1 *)0x607 = 0;
            return;
          }
        }
        else {
          if ((*(char *)0x616 != '\0') || (*(char *)0x2 != '\0')) {
            func_0x000968a2((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
            return;
          }
          *(undefined1 *)0x607 = 0;
          pcVar6 = (char *)0x9687;
          func_0x00096ce9((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
        }
      }
      else if (((*(byte *)*(undefined4 *)0x60e & 0x60) == 0x60) ||
              (*(char *)*(undefined4 *)0x60e == '/')) {
        func_0x000968a2(pcVar5);
        return;
      }
    }
    *(char *)0x608 = *(char *)0x608 + '\x01';
    *(int *)0x60e = *(int *)0x60e + (uint)bVar3;
    unaff_CS = (char *)0x9687;
    func_0x00096ce9(pcVar6,(uint)bVar3);
  } while( true );
}



void __stdcall16far FUN_0008_31f1(byte *param_1,byte param_2)

{
  byte *pbVar1;
  int local_a;
  
  local_a = 0;
  while( true ) {
    if ((int)(uint)param_2 <= local_a) {
      return;
    }
    pbVar1 = (byte *)param_1 + (*param_1 & 0xf) + 1;
    param_1 = (byte *)CONCAT22(param_1._2_2_,pbVar1);
    if ((*param_1 == 0) && (param_1 = (byte *)CONCAT22(param_1._2_2_,pbVar1 + 1), *param_1 == 0))
    break;
    local_a = local_a + 1;
  }
  return;
}



/* putchar_serial (confidence: Confirmed)
   
   Evidence: Thin wrapper: stores the byte argument at local `[5]`, calls `serial_tx_buffer_put` */

void __stdcall16far putchar_serial(byte char)

{
  undefined2 unaff_DS;
  
  *(byte *)0x5 = char;
  func_0x00096b68();
  return;
}



/* putchar_serial_seg (confidence: Confirmed)
   
   Evidence: Same as `putchar_serial` but swaps `DS` to a fixed segment (`0x8F80`) first via
   `set_ds_return_old` */

void __stdcall16far putchar_serial_seg(undefined1 param_1)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  char *pcVar2;
  
  pcVar2 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar1 = func_0x0009470e();
  *(undefined1 *)0x5 = param_1;
  func_0x00096b68(38000,pcVar2,uVar1);
  func_0x0009470e(0x9687,uVar1);
  return;
}



void __cdecl16far FUN_0008_3285(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x623 = 4;
  return;
}



void __cdecl16far FUN_0008_3298(void)

{
  char cVar1;
  char *pcVar2;
  undefined2 unaff_DS;
  bool bVar3;
  bool bVar4;
  undefined4 uVar5;
  long lVar6;
  int local_e;
  int local_c;
  uint local_a;
  
  func_0x00096ce9();
  func_0x00082fd7(0x9687);
  local_e = 0;
  local_a = 0;
  *(undefined2 *)0x57c = 0;
  *(undefined2 *)0x578 = 0;
  *(undefined2 *)0x57a = 0;
  *(undefined1 *)0x57e = 0;
  local_c = 1;
  while ((pcVar2 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18, local_a < 0xb &&
         (*(char *)0x616 == '\0'))) {
    if ((*(byte *)0x4 < 0x30) || (0x39 < *(byte *)0x4)) {
      cVar1 = *(char *)0x4;
      if ((cVar1 == '+') || (cVar1 == '-')) {
        if (local_a == 0) {
          if (*(char *)0x4 == '-') {
            *(undefined1 *)0x57e = 1;
          }
          local_a = 1;
        }
        else if ((local_a == 2) || (local_a == 3)) {
LAB_0008_342b:
          local_a = 99;
        }
        else if (local_a == 5) {
          if (*(char *)0x4 == '-') {
            local_c = -1;
          }
          local_a = 6;
        }
        else {
          if (local_a == 7) goto LAB_0008_342b;
          local_a = 0x15;
        }
      }
      else if (cVar1 == '.') {
        if ((local_a == 0) || (local_a == 1)) {
          local_a = 4;
        }
        else if (local_a == 2) {
          local_a = 3;
        }
        else {
          local_a = 99;
        }
      }
      else if ((cVar1 == 'E') || (cVar1 == 'e')) {
        if ((local_a == 2) || (local_a == 3)) {
          local_a = 5;
        }
        else {
          local_a = 99;
        }
      }
      else if (((local_a == 2) || (local_a == 3)) || (local_a == 7)) {
        local_a = 99;
      }
      else {
        local_a = 0x15;
      }
    }
    else if ((local_a == 0) || (local_a == 1)) {
      uVar5 = func_0x000e777d((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,10,0);
      pcVar2 = (char *)0xe772;
      lVar6 = func_0x000e773d(0xe772,uVar5);
      *(undefined2 *)0x578 = (int)(lVar6 + -0x30);
      *(undefined2 *)0x57a = (int)((ulong)(lVar6 + -0x30) >> 0x10);
      local_a = 2;
    }
    else {
      if (local_a == 2) {
        bVar4 = true;
      }
      else {
        if (local_a != 3) {
          if (local_a != 4) {
            if (((local_a == 5) || (local_a == 6)) || (local_a == 7)) {
              if (*(int *)0x57c < 10) {
                *(int *)0x57c = *(int *)0x57c * 10 + (uint)*(byte *)0x4 + -0x30;
                local_a = 7;
              }
              else {
                local_a = 0x15;
              }
            }
            else {
              local_a = 0x15;
            }
            goto LAB_0008_34fc;
          }
          local_a = 3;
        }
        local_e = local_e + -1;
        bVar4 = local_e == 0;
      }
      bVar3 = false;
      func_0x000e7753((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x3d70,10);
      if ((!bVar3 && !bVar4) ||
         ((func_0x000e7753(0xe772,0x3d70,10), bVar4 && (*(byte *)0x4 < 0x36)))) {
        uVar5 = func_0x000e777d(0xe772,10,0);
        lVar6 = func_0x000e773d(0xe772,uVar5);
        *(undefined2 *)0x578 = (int)(lVar6 + -0x30);
        *(undefined2 *)0x57a = (int)((ulong)(lVar6 + -0x30) >> 0x10);
      }
      else {
        local_a = 0x15;
      }
      pcVar2 = (char *)0xe772;
    }
LAB_0008_34fc:
    if (*(char *)0x2 != '\0') break;
    func_0x00082fd7(pcVar2);
  }
  if (((local_a == 99) || (local_a == 2)) || ((local_a == 3 || (local_a == 7)))) {
    if (local_c == -1) {
      *(int *)0x57c = -*(int *)0x57c;
    }
    *(int *)0x57c = *(int *)0x57c + local_e;
    func_0x00083570(pcVar2,0);
  }
  else {
    *(undefined2 *)0x57c = 0;
    *(undefined2 *)0x578 = 0;
    *(undefined2 *)0x57a = 0;
    *(undefined1 *)0x57e = 0;
    *(undefined1 *)0x623 = 4;
  }
  return;
}



void __stdcall16far FUN_0008_3570(int param_1)

{
  int *piVar1;
  undefined2 unaff_DS;
  bool bVar2;
  undefined1 uVar3;
  undefined4 uVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  
  bVar2 = true;
  uVar6 = 0;
  uVar5 = 0;
  func_0x000e7753();
  if (bVar2) {
    *(undefined1 *)0x57e = 0;
    *(undefined2 *)0x57c = 0;
  }
  else {
    while( true ) {
      uVar3 = 1;
      func_0x000e783d(0xe772,10,0,10,0,*(undefined2 *)0x578,*(undefined2 *)0x57a,uVar5,uVar6);
      func_0x000e777d(0xe772);
      func_0x000e7753(0xe772);
      if (!(bool)uVar3) break;
      uVar6 = 0;
      uVar5 = 10;
      uVar4 = func_0x000e783d(0xe772);
      *(undefined2 *)0x578 = (int)uVar4;
      *(undefined2 *)0x57a = (int)((ulong)uVar4 >> 0x10);
      *(int *)0x57c = *(int *)0x57c + 1;
    }
    uVar3 = 0;
    if (param_1 == 0) {
      bVar2 = true;
      while( true ) {
        func_0x000e7753(0xe772,0x3d70,10);
        if (((bool)uVar3 || bVar2) || (*(int *)0x57c < 1)) break;
        uVar3 = 0;
        uVar4 = func_0x000e777d(0xe772,10,0);
        *(undefined2 *)0x578 = (int)uVar4;
        *(undefined2 *)0x57a = (int)((ulong)uVar4 >> 0x10);
        piVar1 = (int *)0x57c;
        *piVar1 = *piVar1 + -1;
        bVar2 = *piVar1 == 0;
      }
    }
  }
  return;
}



int __cdecl16far FUN_0008_3625(void)

{
  int iVar1;
  undefined2 unaff_DS;
  bool bVar2;
  bool bVar3;
  
  *(undefined1 *)0x625 = 0;
  func_0x00083298();
  iVar1 = *(int *)0x578;
  if (*(char *)0x623 == '\0') {
    if (*(int *)0x57c == 0) {
      bVar2 = false;
      bVar3 = true;
      func_0x000e7753((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x8000,0);
      if (bVar2 || bVar3) {
        bVar2 = false;
        func_0x000e7753(0xe772,0xffff,0);
        if ((bVar2) || (*(char *)0x57e != '\0')) {
          *(undefined1 *)0x625 = 2;
        }
        else {
          *(undefined1 *)0x625 = 1;
        }
      }
      else if (*(char *)0x57e != '\0') {
        iVar1 = -iVar1;
      }
    }
    else {
      *(undefined1 *)0x625 = 2;
    }
  }
  else {
    iVar1 = 0;
  }
  return iVar1;
}



undefined2 __cdecl16far FUN_0008_36aa(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  if (*(char *)0x625 == '\0') {
    uVar1 = *(undefined2 *)0x600;
  }
  else {
    *(undefined1 *)0x623 = 0x15;
    uVar1 = 0;
  }
  return uVar1;
}



undefined2 __cdecl16far FUN_0008_36c8(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  if ((*(char *)0x625 == '\x01') || ((*(char *)0x625 == '\0' && (*(char *)0x57e == '\0')))) {
    uVar1 = *(undefined2 *)0x578;
  }
  else {
    *(undefined1 *)0x623 = 0x15;
    uVar1 = 0;
  }
  return uVar1;
}



undefined2 __cdecl16far FUN_0008_36f4(void)

{
  undefined2 unaff_DS;
  bool bVar1;
  
  func_0x00083298();
  if (((*(char *)0x623 == '\0') && (*(int *)0x57c == 0)) && (*(char *)0x57e == '\0')) {
    bVar1 = false;
    func_0x000e7753((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0xffff,0);
    if (!bVar1) {
      return *(undefined2 *)0x578;
    }
  }
  *(undefined1 *)0x623 = 0x15;
  return 0;
}



int __stdcall16far FUN_0008_373a(int param_1)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  bool bVar4;
  undefined2 uVar5;
  
  uVar5 = 1;
  func_0x00083570();
  uVar3 = (undefined2)((ulong)*(undefined4 *)0x72e >> 0x10);
  iVar1 = (int)*(undefined4 *)0x72e + param_1 * 10;
  bVar4 = true;
  func_0x000e7753((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,*(undefined2 *)0x578,
                  *(undefined2 *)0x57a,uVar5);
  if (bVar4) {
    iVar2 = 0;
  }
  else {
    bVar4 = true;
    func_0x000e7753(0xe772,*(undefined2 *)0x578,*(undefined2 *)0x57a);
    if (bVar4) {
      if ((*(byte *)(iVar1 + 9) & 0x80) == 0) {
        iVar2 = 1;
      }
      else {
        iVar2 = -2;
      }
    }
    else {
      func_0x000e7753(0xe772,*(undefined2 *)0x578,*(undefined2 *)0x57a);
      if (!bVar4) {
        return -1;
      }
      iVar2 = 2;
    }
  }
  iVar2 = iVar2 + *(int *)0x57c * 3 + (int)*(char *)(iVar1 + 8);
  if ((iVar2 < 0) || ((int)(*(byte *)(iVar1 + 9) & 0x1f) < iVar2)) {
    iVar2 = -1;
  }
  return iVar2;
}



int __cdecl16far FUN_0008_380b(void)

{
  int iVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  bool bVar2;
  undefined4 uVar3;
  
  while( true ) {
    do {
      while( true ) {
        if (*(int *)0x57c == -2) {
          bVar2 = false;
          func_0x000e7753(unaff_CS,0x2800,0);
          if (bVar2) {
            iVar1 = 20000;
          }
          else {
            iVar1 = *(int *)0x578 / 4;
            if (*(char *)0x57e != '\0') {
              iVar1 = -iVar1;
            }
          }
          return iVar1;
        }
        if (-3 < *(int *)0x57c) break;
        uVar3 = func_0x000e783d(unaff_CS,10,0);
        *(undefined2 *)0x578 = (int)uVar3;
        *(undefined2 *)0x57a = (int)((ulong)uVar3 >> 0x10);
        *(int *)0x57c = *(int *)0x57c + 1;
        unaff_CS = 0xe772;
      }
    } while (*(int *)0x57c < -1);
    bVar2 = false;
    func_0x000e7753(unaff_CS,0x400,0);
    if (bVar2) break;
    unaff_CS = 0xe772;
    uVar3 = func_0x000e777d(0xe772,10,0);
    *(undefined2 *)0x578 = (int)uVar3;
    *(undefined2 *)0x57a = (int)((ulong)uVar3 >> 0x10);
    *(int *)0x57c = *(int *)0x57c + -1;
  }
  return 20000;
}



void __cdecl16far FUN_0008_38c0(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x000e693c(38000,uVar2,uVar1);
  func_0x0009470e(0xe693,uVar1);
  return;
}



undefined2 __cdecl16far FUN_0008_38e3(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  char *pcVar2;
  undefined2 local_c;
  
  pcVar2 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar1 = func_0x0009470e();
  if ((*"\t" == '\0') && ((*"" != '\0' || (*(char *)0x45b != '\0')))) {
    *(undefined1 *)0x623 = 0x1b;
    local_c = 0;
  }
  else {
    local_c = 0xffff;
  }
  func_0x0009470e(38000,uVar1,pcVar2);
  return local_c;
}



undefined2 __cdecl16far FUN_0008_392b(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  char *pcVar2;
  undefined2 local_c;
  
  local_c = 0;
  pcVar2 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar1 = func_0x0009470e();
  if ((((*(char *)((int)*(undefined4 *)0x732 + 0x95) != '\0') &&
       (*(char *)((int)*(undefined4 *)0x732 + 0x96) != '\0')) && ((*(byte *)0x4f1 & 0xc0) == 0)) &&
     (*(char *)0x4f0 != '\x1f')) {
    local_c = 0xffff;
  }
  func_0x0009470e(38000,uVar1,pcVar2);
  return local_c;
}



undefined2 __cdecl16far FUN_0008_397e(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  char *pcVar2;
  undefined2 local_c;
  
  local_c = 0;
  pcVar2 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar1 = func_0x0009470e();
  if ((((*(char *)((int)*(undefined4 *)0x732 + 0x95) != '\0') &&
       (*(char *)((int)*(undefined4 *)0x732 + 0x96) != '\0')) && ((*(byte *)0x4f1 & 0x80) != 0)) &&
     (*(char *)0x4f0 != '\x1f')) {
    local_c = 0xffff;
  }
  func_0x0009470e(38000,uVar1,pcVar2);
  return local_c;
}



/* comm_call_main_rom (confidence: Loop structure, `process_gpib_command_byte`/`create_task` calls,
   and the `[0x72A]` dispatch table confirmed by direct trace; the `[0x72A]` table's own
   contents/targets not yet decoded)
   
   Evidence: **Corrected/substantially deepened 2026-09-15** - the earlier description ("swaps `DS`,
   calls a main-ROM handler, swaps back") undersold this badly; it's a real **state-machine loop**,
   not a simple wrapper. Swaps `DS` to the main ROM's segment (`0x41`), calls
   `process_gpib_command_byte` at 2 points, does an **indexed call through a computed pointer table
   at `[0x72A]`** (`les di,[0x72A]`, indexed by `[0x5A4]*4` - a genuine state-dispatch jump table,
   not yet independently traced), checks whether the dispatched state changed, and loops back to the
   top if not done. Uses `create_task` purely as a **cooperative yield point**: forks a continuation
   of itself right before looping, letting the scheduler run other tasks, then resumes its own state
   machine from the top once rescheduled - see `docs/interrupts/task-scheduler.md`'s 2026-09-15
   follow-up for the full trace and what this reveals about `create_task`'s real common usage
   pattern */

void __cdecl16far comm_call_main_rom(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x000ed7df(38000,uVar2,uVar1);
  func_0x0009470e(0xead0,uVar1);
  return;
}



void __cdecl16far FUN_0008_3b1a(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x5a4 = 3;
  *(undefined1 *)0x623 = 0;
  *(undefined1 *)0x625 = 0;
  return;
}



void __cdecl16far FUN_0008_3b33(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x6 = *(undefined1 *)0x623;
  func_0x0008526b();
  *(undefined1 *)0x5a4 = 2;
  *(undefined1 *)0x623 = 0;
  return;
}



void __cdecl16far FUN_0008_3b52(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  while ((*(char *)0x3 != '\0' || (*(char *)0x2 != '\0'))) {
    if (*(char *)0x2 != '\0') {
      *(undefined1 *)0x5a4 = 0x14;
      return;
    }
    func_0x000968a2(unaff_CS);
    unaff_CS = 0x9687;
    func_0x0009699b(0x9687);
  }
  return;
}



void __cdecl16far FUN_0008_3b86(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 uVar1;
  
  if ((*"\t" != '\0') && ((*(byte *)0x4f1 & 0x40) != 0)) {
    uVar1 = 0x41;
    func_0x000f0078();
    *(undefined1 *)0x7b8 = 0xff;
    unaff_CS = 0xf007;
    func_0x000f0078(0xf007,(char *)s_TRIggerd_0008_8ef8 + 8,uVar1);
    *(undefined1 *)0x5a3 = 0;
  }
  do {
    if (*(char *)0x3 == '\0') goto LAB_0008_3c27;
    func_0x000968a2(unaff_CS);
    unaff_CS = 0x9687;
    func_0x0009699b(0x9687);
    if ((((*(char *)0x4 != '\r') && (*(char *)0x4 != '\n')) && (*(char *)0x4 != ' ')) &&
       (*(char *)0x4 != ';')) {
      if (*(char *)0x2 == '\0') {
        *(undefined2 *)0x5fc = 0;
        *(undefined1 *)0x5a4 = 4;
        *(undefined2 *)0x63e = 0;
        func_0x00096ce9(0x9687);
      }
      else {
        *(undefined1 *)0x623 = 2;
        *(undefined1 *)0x5a4 = 1;
      }
      goto LAB_0008_3c27;
    }
  } while (*(char *)0x2 == '\0');
  *(undefined1 *)0x5a4 = 0x14;
LAB_0008_3c27:
  *(undefined1 *)0x609 = 0xff;
  *(undefined1 *)0x5fe = 0xff;
  return;
}



void __cdecl16far FUN_0008_3c35(void)

{
  char *unaff_CS;
  undefined2 unaff_DS;
  
  while( true ) {
    if (*(char *)0x3 == '\0') {
      return;
    }
    func_0x00082fd7(unaff_CS);
    *(int *)0x63e = *(int *)0x63e + 1;
    if (0xe < *(int *)0x63e) {
      *(undefined1 *)0x623 = 2;
    }
    if (*(char *)0x623 != '\0') break;
    if ((*(char *)0x2 != '\0') ||
       (unaff_CS = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18, *(char *)0x616 != '\0')) {
      func_0x00085115((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
      if (*(char *)0x623 == '\0') {
        if (*(char *)0x4 == '?') {
          *(undefined1 *)0x5a4 = 5;
        }
        else if ((*(char *)0x2 == '\0') && (*(char *)0x4 != ';')) {
          if ((((*(char *)0x4 == ' ') || (*(char *)0x4 == ',')) || (*(char *)0x4 == '\r')) ||
             (*(char *)0x4 == '\n')) {
            *(undefined1 *)0x605 = 0x10;
            *(undefined1 *)0x5a4 = 6;
            if (*"\x05" != '\0') {
              *(undefined1 *)0x7 = 0;
            }
          }
          else {
            *(undefined1 *)0x623 = 3;
            *(undefined1 *)0x5a4 = 1;
          }
        }
        else {
          *(undefined1 *)0x605 = 0x10;
          *(undefined1 *)0x5a4 = 0x10;
        }
        *(undefined1 *)0x5ff = 0;
        *(undefined1 *)0x5f7 = 0;
        *(undefined1 *)0x609 = 0xfb;
        *(undefined1 *)0x5fe = 0xfb;
        *(undefined1 *)0x606 = 0xfb;
        *(undefined2 *)0x600 = 0;
        *(undefined2 *)0x5f8 = 0;
        *(undefined2 *)0x5f4 = 0;
      }
      else {
        *(undefined1 *)0x5a4 = 1;
      }
      return;
    }
  }
  *(undefined1 *)0x5a4 = 1;
  return;
}



void __cdecl16far FUN_0008_3d2c(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x2 == '\0') {
    func_0x0009699b();
  }
  *(undefined1 *)0x605 = 0x20;
  *(undefined1 *)0x5a4 = 6;
  return;
}



void __cdecl16far FUN_0008_3d4c(void)

{
  undefined2 unaff_DS;
  
  if ((*(char *)0x2 == '\0') && (*(char *)0x4 != ';')) {
    if (((*(char *)0x4 == ' ') || (*(char *)0x4 == ',')) ||
       ((*"\t" == '\0' && ((*(char *)0x4 == '\r' || (*(char *)0x4 == '\n')))))) {
      if (*(char *)0x3 != '\0') {
        func_0x000968a2();
        func_0x00082fd7(0x9687);
        *(undefined1 *)0x5a4 = 7;
      }
    }
    else if ((*(char *)0x4 == '\r') || (*(char *)0x4 == '\n')) {
      if (*(char *)0x3 != '\0') {
        func_0x000968a2();
        func_0x00082fd7(0x9687);
      }
    }
    else {
      *(undefined1 *)0x623 = 3;
      *(undefined1 *)0x5a4 = 1;
    }
  }
  else {
    *(undefined1 *)0x5a4 = 0x10;
  }
  return;
}



void __cdecl16far FUN_0008_3dd4(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x4 == ';') {
    *(undefined1 *)0x5a4 = 0x10;
  }
  else if (((*(char *)0x4 == '\r') || (*(char *)0x4 == '\n')) || (*(char *)0x4 == ' ')) {
    if (*(char *)0x3 == '\0') {
      if (*(char *)0x2 != '\0') {
        *(undefined1 *)0x5a4 = 0x10;
      }
    }
    else {
      func_0x000968a2();
      func_0x00082fd7(0x9687);
    }
  }
  else if ((*(char *)0x604 == '\a') && ((*(byte *)0x605 & 0x10) != 0)) {
    *(undefined1 *)0x5a4 = 0x10;
  }
  else {
    *(undefined1 *)0x5a4 = 8;
  }
  return;
}



void __cdecl16far FUN_0008_3e42(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x2 == '\0') {
    if (*(char *)0x3 != '\0') {
      func_0x000968a2();
      func_0x00082fd7(0x9687);
      *(undefined1 *)0x5a4 = 7;
    }
  }
  else {
    *(undefined1 *)0x623 = 7;
    *(undefined1 *)0x5a4 = 1;
  }
  return;
}



void __cdecl16far FUN_0008_3e79(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x625 = 0;
  *(undefined2 *)0x640 = 0;
  if (*(char *)0x4 == '%') {
    *(undefined1 *)0x5a4 = 10;
  }
  else if (*(char *)0x4 == '#') {
    *(undefined1 *)0x5a4 = 0xc;
  }
  else if (*(char *)0x4 == '\"') {
    *(undefined2 *)0x5f0 = 0x5a9;
    *(undefined2 *)0x5f2 = unaff_DS;
    *(undefined1 *)0x5a4 = 9;
  }
  else if ((*(byte *)0x4 < 0x41) || (0x5a < *(byte *)0x4)) {
    if (*(char *)0x616 == '\0') {
      func_0x00096ce9();
      *(undefined1 *)0x5a4 = 0xe;
    }
    else {
      *(undefined1 *)0x5f7 = 6;
      *(undefined1 *)0x623 = 0xf;
      *(undefined1 *)0x5a4 = 1;
    }
  }
  else if (*(char *)0x2 == '\0') {
    *(undefined1 *)0x5a4 = 0xf;
  }
  else {
    *(undefined1 *)0x5f7 = 6;
    *(undefined1 *)0x623 = 0xf;
    *(undefined1 *)0x5a4 = 1;
  }
  *(undefined1 *)0x5fa = 0;
  *(undefined2 *)0x5f4 = 0;
  return;
}



/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Instruction at (ram,0x00004132) overlaps instruction at (ram,0x00004131)
    */
/* WARNING: Stack frame is not setup normally: Input value of stackpointer is not used */
/* WARNING: Removing unreachable block (ram,0x0000410d) */
/* WARNING: Removing unreachable block (ram,0x00004107) */
/* WARNING: Removing unreachable block (ram,0x000040f9) */

uint __cdecl16far FUN_0008_3f22(void)

{
  char *pcVar1;
  uint *puVar2;
  undefined1 uVar3;
  int iVar4;
  char in_AL;
  char cVar5;
  byte bVar6;
  uint uVar7;
  undefined2 in_DX;
  undefined2 extraout_DX;
  undefined2 extraout_DX_00;
  byte bVar8;
  int in_BX;
  int iVar9;
  undefined2 *puVar10;
  undefined2 *puVar11;
  int iVar12;
  undefined1 *puVar13;
  undefined1 *puVar14;
  undefined1 *puVar15;
  ulong in_ESP;
  undefined2 uVar18;
  undefined2 *puVar16;
  ulong uVar17;
  undefined2 unaff_BP;
  undefined1 *unaff_SI;
  undefined1 *unaff_DI;
  undefined1 *puVar19;
  undefined2 unaff_ES;
  char *unaff_CS;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined4 uVar20;
  
  iVar9 = (int)in_ESP;
  uVar18 = (undefined2)(in_ESP >> 0x10);
  *(undefined2 *)(iVar9 + -2) = unaff_BP;
  puVar16 = (undefined2 *)CONCAT22(uVar18,(undefined2 *)(iVar9 + -0xc));
  if (*(char *)0x2 == '\0') {
    if (*(char *)0x3 == '\0') {
LAB_0008_4085:
      if (*(char *)0x623 != '\0') {
        *(undefined1 *)0x5a4 = 1;
      }
      *unaff_DI = *unaff_SI;
      uVar7 = in(0x5d);
      return uVar7;
    }
    *(undefined2 *)(iVar9 + -0xe) = unaff_CS;
    unaff_CS = (char *)0x9687;
    puVar16 = (undefined2 *)CONCAT22(uVar18,(undefined2 *)(iVar9 + -0x10));
    *(undefined2 *)(iVar9 + -0x10) = 0x3fbc;
    func_0x0009699b();
    *(undefined2 *)(iVar9 + -10) = 0xffff;
    uVar7 = (uint)*(byte *)0x5fa;
    in_DX = extraout_DX;
    if (uVar7 == 4) {
      if (*(char *)0x4 == '\"') {
        *(undefined1 *)0x5fa = 3;
      }
      else {
        *(undefined2 *)(iVar9 + -10) = 0;
        *(undefined1 *)0x5fa = 6;
      }
    }
    else if (uVar7 == 7) {
      if (*(char *)0x4 == '\"') {
        *(undefined1 *)0x4 = 0x5e;
        *(undefined1 *)0x5fa = 4;
      }
      else if ((*(byte *)0x4 < 0x40) || (0x5f < *(byte *)0x4)) {
        *(undefined1 *)(iVar9 + -0xb) = *(undefined1 *)0x4;
        puVar10 = (undefined2 *)puVar16;
        puVar10[-1] = 0x9687;
        unaff_CS = (char *)0x839f;
        puVar16 = (undefined2 *)CONCAT22((int)((ulong)puVar16 >> 0x10),puVar10 + -2);
        puVar10[-2] = 0x401d;
        uVar20 = func_0x000850e4();
        in_DX = (undefined2)((ulong)uVar20 >> 0x10);
        uVar7 = (uint)uVar20;
        in_AL = (char)uVar20;
        if (*(char *)0x623 != '\0') goto code_r0x0000409e;
        in_DX = CONCAT11((char)((ulong)uVar20 >> 0x18),*(undefined1 *)(iVar9 + -0xb));
        *(undefined1 *)0x4 = *(undefined1 *)(iVar9 + -0xb);
        *(undefined1 *)0x5fa = 3;
      }
      else {
        *(char *)0x4 = *(char *)0x4 + -0x40;
        *(undefined1 *)0x5fa = 3;
      }
    }
    else if (*(char *)0x4 == '\"') {
      *(undefined2 *)(iVar9 + -10) = 0;
      *(undefined1 *)0x5fa = 4;
    }
    else if (*(char *)0x4 == '^') {
      *(undefined2 *)(iVar9 + -10) = 0;
      *(undefined1 *)0x5fa = 7;
    }
    uVar18 = (undefined2)((ulong)puVar16 >> 0x10);
    if (*(int *)(iVar9 + -10) == 0) {
      pcVar1 = unaff_DI + iVar9 + 0x6d;
      *pcVar1 = *pcVar1 + 'p';
      if (-1 < *pcVar1) goto LAB_0008_4085;
      puVar16 = (undefined2 *)CONCAT22(uVar18,(undefined2 *)(*(int *)(in_BX + 0x68) * 0x2074));
    }
    else {
      puVar11 = (undefined2 *)puVar16;
      puVar11[-1] = unaff_CS;
      unaff_CS = (char *)0x839f;
      puVar16 = (undefined2 *)CONCAT22(uVar18,puVar11 + -2);
      puVar11[-2] = 0x407e;
      cVar5 = func_0x000850e4();
      uVar7 = CONCAT11(cVar5 >> 7,cVar5 + -0x15 + (*(byte *)0xa32b < 0x29) + unaff_SI[in_BX]);
      *(uint *)(unaff_SI + in_BX + 0x6f43) = *(uint *)(unaff_SI + in_BX + 0x6f43) & uVar7;
      puVar16 = puVar16 + -1;
      *puVar16 = 0x2074;
      in_DX = extraout_DX_00;
    }
    in_AL = (char)uVar7;
    unaff_DI[iVar9 + 0x27] = unaff_DI[iVar9 + 0x27] - in_AL;
    unaff_SI[0x65] = unaff_SI[0x65] & (byte)in_DX;
    unaff_SI = (undefined1 *)(*(int *)(unaff_SI + 0x72) * 0x6f);
  }
  else {
    *(undefined1 *)0x623 = 4;
  }
code_r0x0000409e:
  out(*unaff_SI,in_DX);
  puVar19 = (undefined1 *)(*(int *)(unaff_SI + 1 + in_BX + 0x2c) * 0x4920);
  out(unaff_SI[1],in_DX);
  puVar2 = (uint *)0x202c;
  iVar4 = (iVar9 - 2U & 3) - (*puVar2 & 3);
  *puVar2 = *puVar2 + (uint)(0 < iVar4) * iVar4;
  *(uint *)(puVar19 + in_BX) = *(uint *)(puVar19 + in_BX) ^ (uint)puVar19;
  bVar6 = in_AL - 0x40;
  uVar7 = (uint)bVar6;
  bVar8 = (byte)((uint)in_DX >> 8);
  if (-1 < (char)bVar6) {
    puVar19[in_BX + 0x6c] = puVar19[in_BX + 0x6c] & bVar6;
    uVar3 = in(in_DX);
    *puVar19 = uVar3;
    unaff_SI[iVar9 + 0x69] = unaff_SI[iVar9 + 0x69] & bVar8;
    puVar16[-1] = 0x606;
    ((undefined2 *)(puVar16 + -1))[-1] = unaff_ES;
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  }
  *(int *)(unaff_SI + iVar9 + 0x347) = *(int *)(unaff_SI + iVar9 + 0x347) + in_BX;
  uVar17 = in_ESP & 0xffff;
  unaff_SI[in_BX + 0x240] = unaff_SI[in_BX + 0x240];
  unaff_SI[0x13] = unaff_SI[0x13] + bVar8;
  do {
    if (*"\x03\x05$\v<" != '\x06') {
      *(undefined1 *)0x623 = 4;
      *(undefined1 *)0x5a4 = 1;
    }
    do {
      iVar12 = (int)uVar17;
      if (*"\x03\x05$\v<" == '\0') {
        if (*(char *)0x4 != 'H') {
          if (*(char *)0x4 == 'B') {
            *(undefined1 *)0x5fa = 1;
            *(undefined2 *)(iVar12 + -2) = unaff_CS;
            *(undefined2 *)(iVar12 + -4) = 0x417b;
            uVar7 = func_0x000968a2();
            *(undefined1 *)0x5a4 = 0xb;
            return uVar7;
          }
          *(undefined1 *)0x5a4 = 0xe;
          return uVar7;
        }
        *(undefined1 *)0x5fa = 1;
        *(undefined2 *)(iVar12 + -2) = unaff_CS;
        puVar13 = (undefined1 *)(iVar12 + -4);
        *(undefined2 *)(iVar12 + -4) = 0x4167;
        uVar7 = func_0x000968a2();
        if ((((*(char *)0x604 == '\x01') || (*(char *)0x604 == '\"')) || (*(char *)0x604 == '\x03'))
           || (*(char *)0x604 == '\x13')) {
          *(undefined2 *)0x612 = 0;
          *(undefined1 *)0x5a4 = 0xd;
          return uVar7;
        }
        *(undefined2 *)(puVar13 + -2) = 0x9687;
        puVar15 = puVar13 + -4;
        *(undefined2 *)(puVar13 + -4) = 0x41bd;
        uVar7 = func_0x000968a2();
      }
      else {
        if (*"\x03\x05$\v<" == '\x06') {
          *(undefined2 *)(iVar12 + -2) = unaff_CS;
          *(undefined2 *)(iVar12 + -4) = 0x41cc;
          uVar7 = func_0x000968a2();
          *(undefined1 *)0x5a4 = 0x10;
          return uVar7;
        }
        if (*(char *)0x3 == '\0') {
          *(undefined2 *)(iVar12 + -2) = unaff_CS;
          *(undefined2 *)(iVar12 + -4) = 0x42a5;
          uVar7 = func_0x00096ce9();
          return uVar7;
        }
        *(undefined2 *)(iVar12 + -2) = unaff_CS;
        puVar14 = (undefined1 *)(iVar12 + -4);
        *(undefined2 *)(iVar12 + -4) = 0x41e0;
        uVar7 = func_0x00082f3a();
        *(undefined1 *)0x4 = (char)uVar7;
        if (*(char *)0x623 != '\0') {
          *(undefined1 *)0x5a4 = 1;
          return uVar7;
        }
        cVar5 = *"\x03\x05$\v<";
        if (cVar5 == '\x01') {
          *(undefined1 *)0x5f6 = *(undefined1 *)0x4;
          uVar7 = *(byte *)0x4 - 1 & 0xff;
          *(uint *)0x5f4 = uVar7;
          *(undefined1 *)0x5fa = 3;
          if ((0x60 < *(int *)0x5f4) || (*(int *)0x5f4 < 0)) {
            *(undefined1 *)0x623 = 0xb;
            *(undefined1 *)0x5a4 = 1;
            return uVar7;
          }
        }
        else if (cVar5 == '\x03') {
          *(char *)0x5f6 = *(char *)0x5f6 + *(char *)0x4;
          *(int *)0x5f4 = *(int *)0x5f4 + -1;
          if (*(int *)0x5f4 == 0) {
            *(undefined1 *)0x5fa = 5;
          }
        }
        else if (cVar5 == '\x05') {
          *(undefined1 *)0x5fa = 6;
          bVar6 = *(byte *)0x4;
          *(char *)0x5f6 = *(char *)0x5f6 + bVar6;
          if (*"B" == '\0') {
            *(undefined1 *)0x5f7 = 2;
          }
          else {
            *(undefined1 *)0x5ff = 2;
          }
          if (*(char *)0x5f6 != '\0') {
            *(undefined1 *)0x623 = 9;
            *(undefined1 *)0x5a4 = 1;
            return (uint)bVar6;
          }
        }
        *(char **)(puVar14 + -2) = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
        puVar15 = puVar14 + -4;
        *(undefined2 *)(puVar14 + -4) = 0x429d;
        uVar7 = func_0x000968a2();
      }
      if ((*(char *)0x3 == '\0') && (*(char *)0x2 == '\0')) {
        return uVar7;
      }
      *(undefined2 *)(puVar15 + -2) = 0x9687;
      unaff_CS = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
      uVar17 = ZEXT24(puVar15 + -4);
      *(undefined2 *)(puVar15 + -4) = 0x4137;
      uVar7 = func_0x00082fd7();
    } while (*(char *)0x2 == '\0');
  } while( true );
}



void __cdecl16far FUN_0008_4095(void)

{
  return;
}



void __cdecl16far FUN_0008_409f(void)

{
  return;
}



void FUN_0008_40a9(void)

{
  char cVar1;
  undefined1 uVar2;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  while( true ) {
    while( true ) {
      if ((*(char *)0x3 == '\0') && (*(char *)0x2 == '\0')) {
        return;
      }
      func_0x00082fd7(unaff_CS);
      if ((*(char *)0x2 != '\0') && (*"\x03\x05$\v<" != '\x06')) {
        *(undefined1 *)0x623 = 4;
        *(undefined1 *)0x5a4 = 1;
      }
      if (*"\x03\x05$\v<" != '\0') break;
      if (*(char *)0x4 != 'H') {
        if (*(char *)0x4 == 'B') {
          *(undefined1 *)0x5fa = 1;
          func_0x000968a2((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
          *(undefined1 *)0x5a4 = 0xb;
          return;
        }
        *(undefined1 *)0x5a4 = 0xe;
        return;
      }
      *(undefined1 *)0x5fa = 1;
      func_0x000968a2((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
      if ((((*(char *)0x604 == '\x01') || (*(char *)0x604 == '\"')) || (*(char *)0x604 == '\x03'))
         || (*(char *)0x604 == '\x13')) {
        *(undefined2 *)0x612 = 0;
        *(undefined1 *)0x5a4 = 0xd;
        return;
      }
      unaff_CS = 0x9687;
      func_0x000968a2(0x9687);
    }
    if (*"\x03\x05$\v<" == '\x06') {
      func_0x000968a2((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
      *(undefined1 *)0x5a4 = 0x10;
      return;
    }
    if (*(char *)0x3 == '\0') break;
    uVar2 = func_0x00082f3a((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
    *(undefined1 *)0x4 = uVar2;
    if (*(char *)0x623 != '\0') {
      *(undefined1 *)0x5a4 = 1;
      return;
    }
    cVar1 = *"\x03\x05$\v<";
    if (cVar1 == '\x01') {
      *(undefined1 *)0x5f6 = *(undefined1 *)0x4;
      *(uint *)0x5f4 = *(byte *)0x4 - 1 & 0xff;
      *(undefined1 *)0x5fa = 3;
      if ((0x60 < *(int *)0x5f4) || (*(int *)0x5f4 < 0)) {
        *(undefined1 *)0x623 = 0xb;
        *(undefined1 *)0x5a4 = 1;
        return;
      }
    }
    else if (cVar1 == '\x03') {
      *(char *)0x5f6 = *(char *)0x5f6 + *(char *)0x4;
      *(int *)0x5f4 = *(int *)0x5f4 + -1;
      if (*(int *)0x5f4 == 0) {
        *(undefined1 *)0x5fa = 5;
      }
    }
    else if (cVar1 == '\x05') {
      *(undefined1 *)0x5fa = 6;
      *(char *)0x5f6 = *(char *)0x5f6 + *(char *)0x4;
      if (*"B" == '\0') {
        *(undefined1 *)0x5f7 = 2;
      }
      else {
        *(undefined1 *)0x5ff = 2;
      }
      if (*(char *)0x5f6 != '\0') {
        *(undefined1 *)0x623 = 9;
        *(undefined1 *)0x5a4 = 1;
        return;
      }
    }
    unaff_CS = 0x9687;
    func_0x000968a2((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
  }
  func_0x00096ce9((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
  return;
}



void __cdecl16far FUN_0008_4240(void)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  while( true ) {
    if ((*(char *)0x3 == '\0') && (*(char *)0x2 == '\0')) {
      return;
    }
    func_0x00082fd7(unaff_CS);
    if ((*(char *)0x2 != '\0') || (*(char *)0x616 != '\0')) break;
    if (*(char *)0x3 == '\0') {
      if (*(char *)0x2 != '\0') {
        *(undefined1 *)0x623 = 4;
        *(undefined1 *)0x5a4 = 1;
        return;
      }
      func_0x00096ce9((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
      return;
    }
    iVar1 = *(int *)0x612 << 8;
    iVar2 = func_0x00082f3a((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
    *(int *)0x612 = iVar1 + iVar2;
    if (*(char *)0x623 != '\0') {
      *(undefined1 *)0x5a4 = 1;
      return;
    }
    unaff_CS = 0x9687;
    func_0x000968a2((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
  }
  *(undefined1 *)0x5fa = 6;
  if (*"B" == '\0') {
    *(undefined1 *)0x5f7 = 2;
  }
  else {
    *(undefined1 *)0x5ff = 2;
  }
  func_0x000968a2((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
  *(undefined1 *)0x5a4 = 0x10;
  return;
}



void __cdecl16far FUN_0008_42f4(void)

{
  bool bVar1;
  undefined2 uVar2;
  char *unaff_CS;
  char *pcVar3;
  undefined2 unaff_DS;
  
  while( true ) {
    if (*(char *)0x3 == '\0') {
      return;
    }
    pcVar3 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
    func_0x00082fd7(unaff_CS);
    if (*(char *)0x4 == ':') break;
    if ((*(char *)0x616 == '\0') || (*(char *)0x4 == ':')) {
      bVar1 = false;
    }
    else {
      bVar1 = true;
    }
    if (!bVar1) {
      pcVar3 = (char *)0x839f;
      func_0x000850c2((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
    }
    if ((*(char *)0x2 != '\0') || (bVar1)) {
      if (0 < *(int *)0x5f4) {
        uVar2 = func_0x00083625(pcVar3);
        if (*(char *)0x623 != '\0') {
          return;
        }
        if (*"B" == '\0') {
          *(undefined1 *)0x5f7 = 5;
          *(undefined1 *)0x606 = 0xfb;
          if (*(char *)0x625 != '\0') {
            *(undefined1 *)0x623 = 0x15;
            return;
          }
          *(undefined2 *)0x5f8 = uVar2;
        }
        else {
          *(undefined1 *)0x5ff = 5;
          *(undefined1 *)0x5fe = 0xfb;
          *(undefined2 *)0x600 = uVar2;
        }
      }
      *(undefined1 *)0x5a4 = 0x10;
      return;
    }
    unaff_CS = pcVar3;
    if (*(char *)0x623 != '\0') {
      *(undefined1 *)0x5a4 = 1;
      return;
    }
  }
  uVar2 = func_0x00083625((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
  if (*(char *)0x623 != '\0') {
    return;
  }
  *(undefined1 *)0x5f7 = 5;
  *(undefined1 *)0x606 = 0xfb;
  if (*(char *)0x625 != '\0') {
    *(undefined1 *)0x623 = 0x15;
    return;
  }
  *(undefined2 *)0x5f8 = uVar2;
  func_0x000968a2((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
  *(undefined1 *)0x5a4 = 0x17;
  return;
}



void __cdecl16far FUN_0008_43f0(void)

{
  undefined2 unaff_CS;
  char *pcVar1;
  undefined2 unaff_DS;
  
  while( true ) {
    if (*(char *)0x3 == '\0') {
      return;
    }
    pcVar1 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
    func_0x00082fd7(unaff_CS);
    if ((*(char *)0x2 != '\0') || (*(char *)0x616 != '\0')) {
      pcVar1 = (char *)0x8511;
      func_0x0008519a((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
      if (*(char *)0x623 != '\0') {
        *(undefined1 *)0x5a4 = 1;
        return;
      }
      if (*"B" == '\0') {
        *(undefined1 *)0x5f7 = 4;
        *(undefined1 *)0x606 = *(undefined1 *)0x608;
        if (*(char *)0x606 == '=') {
          *(uint *)0x5f8 = (uint)*(byte *)0x609;
        }
      }
      else {
        *(undefined1 *)0x5ff = 4;
        *(undefined1 *)0x5fe = *(undefined1 *)0x608;
        if (*"<" == '=') {
          *(uint *)0x600 = (uint)*(byte *)0x609;
        }
      }
    }
    if (*(char *)0x623 != '\0') {
      *(undefined1 *)0x5a4 = 1;
      return;
    }
    if (*(char *)0x4 == ':') {
      func_0x000968a2(pcVar1);
      *(undefined1 *)0x5a4 = 0x17;
      return;
    }
    if ((*(char *)0x2 != '\0') || ((*(char *)0x616 != '\0' && (*(char *)0x4 != ':')))) break;
    unaff_CS = 0x839f;
    func_0x000850c2(pcVar1);
    *(int *)0x640 = *(int *)0x640 + 1;
    if (0x10 < *(int *)0x640) {
      *(undefined1 *)0x623 = 4;
    }
    if (*(char *)0x623 != '\0') {
      *(undefined1 *)0x5a4 = 1;
      return;
    }
  }
  *(undefined1 *)0x5a4 = 0x10;
  return;
}



void __cdecl16far FUN_0008_44d0(void)

{
  char cVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  char *pcVar5;
  undefined2 uVar6;
  undefined2 unaff_DS;
  char *pcVar7;
  undefined2 uVar8;
  byte local_11;
  int local_10;
  undefined4 local_e;
  
  *(undefined1 *)0x5a4 = 0x11;
  *(undefined1 *)0x626 = 0;
  if (*(char *)((int)*(undefined4 *)0x732 + 0x93) != '\0') {
    if ((*(byte *)0x605 & 0x10) == 0) {
      return;
    }
    if (*(char *)0x604 != '\x17') {
      return;
    }
    if (*"B" != '\x04') {
      return;
    }
    if (*(char *)0x606 != '\0') {
      return;
    }
    if (*"" != '\0') {
      return;
    }
  }
  uVar4 = (uint)*(byte *)((uint)*(byte *)0x604 * 2 + (int)*(undefined4 *)0x702);
  uVar6 = (undefined2)((ulong)*(undefined4 *)0x6fe >> 0x10);
  pcVar5 = (char *)((int)*(undefined4 *)0x6fe +
                   (uint)*(byte *)((int)*(undefined4 *)0x702 + (uint)*(byte *)0x604 * 2 + 1) * 0xc);
  local_e = (char *)CONCAT22(uVar6,pcVar5);
  local_11 = 0;
  if ((*(byte *)0x605 & 0x10) == 0) {
    if (((int)*(undefined4 *)(pcVar5 + 8) == 0x5f5) &&
       ((char *)((ulong)*(undefined4 *)(pcVar5 + 8) >> 0x10) ==
        (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18)) {
      *(undefined1 *)0x623 = 2;
      goto LAB_0008_4769;
    }
    if ((*(uint *)(pcVar5 + 2) & 0x1f00) == 0) {
      if (*"B" != '\0') goto LAB_0008_4769;
      local_11 = func_0x000849ec();
      cVar1 = *(char *)0x623;
      goto joined_r0x000047d8;
    }
    pcVar7 = pcVar5;
    uVar8 = uVar6;
    iVar3 = func_0x00084be6();
    if (iVar3 == 0) goto LAB_0008_4769;
    if (*"B" == '\0') {
      *(undefined1 *)0x624 = 0;
      for (local_10 = 0; local_10 < (int)uVar4; local_10 = local_10 + 1) {
        uVar6 = (undefined2)((ulong)local_e >> 0x10);
        bVar2 = func_0x000849ec(0x839f,(char *)local_e,uVar6,0xe00f,pcVar7,uVar8);
        local_11 = local_11 | bVar2;
        if (*(char *)0x623 != '\0') goto LAB_0008_4769;
        local_e = (char *)CONCAT22(uVar6,(char *)local_e + 0xc);
      }
      if ((local_11 == 0) && (*(char *)0x624 != '\0')) {
        *(undefined1 *)0x623 = *(undefined1 *)0x624;
        goto LAB_0008_4769;
      }
    }
    else {
      if (*"" != '\0') goto LAB_0008_4769;
      local_11 = func_0x0008477f(0x839f,pcVar5,uVar6,uVar4);
    }
  }
  else {
    if (((int)*(undefined4 *)(pcVar5 + 4) == 0x5f5) &&
       ((char *)((ulong)*(undefined4 *)(pcVar5 + 4) >> 0x10) ==
        (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18)) {
      *(undefined1 *)0x623 = 2;
      goto LAB_0008_4769;
    }
    pcVar7 = pcVar5;
    uVar8 = uVar6;
    iVar3 = func_0x00084be6();
    if (iVar3 == 0) goto LAB_0008_4769;
    if (*local_e == -5) {
      if (*"B" != '\0') goto LAB_0008_4769;
      local_11 = func_0x00084835(0x839f,pcVar5,uVar6,0xc00f,pcVar7,uVar8);
    }
    else if ((*(uint *)(pcVar5 + 2) & 0x1f00) == 0) {
      if (*"B" == '\0') {
        local_11 = func_0x00084835(0x839f,pcVar5,uVar6,0xc02f,pcVar7,uVar8);
      }
      else {
        if (*"" != '\0') goto LAB_0008_4769;
        local_11 = func_0x0008477f(0x839f,pcVar5,uVar6,uVar4);
      }
    }
    else if (*"B" == '\0') {
      for (local_10 = 0; local_10 < (int)uVar4; local_10 = local_10 + 1) {
        uVar6 = (undefined2)((ulong)local_e >> 0x10);
        bVar2 = func_0x00084835(0x839f,(char *)local_e,uVar6,0xc02f,pcVar7,uVar8);
        local_11 = local_11 | bVar2;
        if (*(char *)0x623 != '\0') goto LAB_0008_4769;
        local_e = (char *)CONCAT22(uVar6,(char *)local_e + 0xc);
      }
    }
    else {
      local_11 = func_0x0008477f(0x839f,pcVar5,uVar6,uVar4);
      if (*(char *)0x623 != '\0') goto LAB_0008_4769;
    }
    if (local_11 == 0) goto LAB_0008_4769;
    cVar1 = *(char *)0x623;
joined_r0x000047d8:
    if (cVar1 != '\0') goto LAB_0008_4769;
  }
  if (local_11 != 0) {
    return;
  }
LAB_0008_4769:
  if (*(char *)0x623 == '\0') {
    *(undefined1 *)0x623 = 4;
  }
  *(undefined1 *)0x5a4 = 1;
  return;
}



undefined2 __stdcall16far FUN_0008_477f(char *param_1,int param_2)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  char *pcVar3;
  undefined2 uVar4;
  undefined2 local_c;
  int local_a;
  
  local_c = 0;
  local_a = 0;
  if (*"B" == '\x04') {
    while ((local_a < param_2 && (*(char *)0x606 != *param_1))) {
      local_a = local_a + 1;
      param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 0xc);
    }
  }
  if (local_a < param_2) {
    uVar2 = (undefined2)((ulong)param_1 >> 0x10);
    if ((*(byte *)0x605 & 0x10) == 0) {
      local_c = func_0x000849ec();
    }
    else {
      pcVar3 = (char *)param_1;
      uVar4 = uVar2;
      iVar1 = func_0x00084ca7();
      if (iVar1 == 0) {
        if (*"" != '\0') {
          *(undefined1 *)0x623 = 4;
        }
      }
      else {
        local_c = func_0x00084835(0x839f,(char *)param_1,uVar2,0xc00f,pcVar3,uVar4);
      }
    }
  }
  else {
    *(undefined1 *)0x623 = 4;
  }
  if (*(char *)0x623 != '\0') {
    *(undefined1 *)0x5a4 = 1;
  }
  return local_c;
}



undefined2 __stdcall16far FUN_0008_4835(undefined4 param_1,uint param_2)

{
  code *pcVar1;
  int iVar2;
  byte bVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 unaff_DS;
  uint uVar7;
  byte local_f;
  char local_e;
  undefined4 local_c;
  
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  iVar4 = (int)param_1;
  uVar7 = param_2;
  iVar2 = func_0x00084d68();
  if (iVar2 == 0) {
    return 0;
  }
  local_c = (code *)*(undefined4 *)(iVar4 + 4);
  uVar6 = (undefined2)((ulong)local_c >> 0x10);
  pcVar1 = (code *)local_c;
  if (((*(uint *)(iVar4 + 2) & 0x1f00) == 0) || (0x17ff < (*(uint *)(iVar4 + 2) & 0x1f00))) {
    (*pcVar1)(0x839f,uVar7);
  }
  else {
    if ((((byte)pcVar1[1] & 0x40) == 0) && (*(char *)0x625 != '\0')) {
      *(undefined1 *)0x623 = 0x15;
      return 0;
    }
    bVar3 = (byte)((uint)*(undefined2 *)(iVar4 + 2) >> 8) & 0x1f;
    local_e = '\0';
    if (*"" == '\0') {
      if (((param_2 & 0x20) != 0) &&
         ((((*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x14' && (((byte)pcVar1[1] & 1) == 0))
           || ((*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x15' &&
               (((byte)pcVar1[1] & 8) == 0)))) ||
          ((*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x1e' && (((byte)pcVar1[1] & 2) == 0))))
         )) {
        return 0;
      }
      iVar2 = func_0x00084d68(0x839f,1,pcVar1[1],0xc02f);
      if (iVar2 != 0) {
        (**(code **)((code *)local_c + 2))(0x839f);
        local_e = -1;
      }
    }
    else {
      local_f = 0;
      if (*"" == '\x04') {
        while ((local_f < bVar3 && (*(code *)0x5fe != *local_c))) {
          local_f = local_f + 1;
          local_c = (code *)CONCAT22(local_c._2_2_,(code *)local_c + 6);
        }
      }
      if (bVar3 <= local_f) {
        *(undefined1 *)0x623 = 4;
        return 0;
      }
      iVar2 = func_0x00084d68(0x839f,1,((code *)local_c)[1],0xc00f);
      if (iVar2 != 0) {
        (**(code **)((code *)local_c + 2))(0x839f);
        local_e = -1;
      }
    }
    if (local_e == '\0') {
      if (*(char *)0x623 == '\0') {
        *(undefined1 *)0x623 = 4;
      }
      return 0;
    }
  }
  return 0xffff;
}



undefined2 __stdcall16far FUN_0008_49ec(char *param_1,undefined2 param_2)

{
  char cVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 in_BX;
  char *pcVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  
  pcVar4 = (char *)param_1;
  if ((*(char *)0x604 == '#') || (*(char *)0x604 == '*')) {
    (**(code **)(pcVar4 + 8))();
    *(undefined1 *)0x602 = 0xff;
    uVar2 = 0xffff;
  }
  else {
    uVar2 = (undefined2)((ulong)param_1 >> 0x10);
    iVar3 = func_0x00084d68();
    if (iVar3 == 0) {
      uVar2 = 0;
    }
    else {
      if (((*"B" == '\0') && ((*(uint *)(pcVar4 + 2) & 0x1f00) != 0)) &&
         (cVar1 = *param_1, *(char *)0x606 = cVar1, cVar1 == '=')) {
        if (*(char *)0x604 == '\x1d') {
          *(undefined2 *)0x5f8 = 5;
        }
        else {
          if (*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x1e') {
            uVar5 = 1;
          }
          else {
            uVar5 = 4;
          }
          *(undefined2 *)0x5f8 = uVar5;
        }
      }
      *(int *)0x5fc = *(int *)0x5fc + 1;
      if (*(int *)0x5fc < 2) {
        uVar5 = (undefined2)((ulong)*(undefined4 *)0x6e6 >> 0x10);
        func_0x000831f1(0x839f,(int)*(undefined4 *)0x6e6,uVar5,*(undefined1 *)0x604);
        *(undefined2 *)0x60a = in_BX;
        *(undefined2 *)0x60c = uVar5;
        *(undefined1 *)0x608 = *(undefined1 *)0x604;
        func_0x00082d2b((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
      }
      else {
        func_0x00083241(0x839f,0x2c,param_2);
      }
      if (((*(uint *)(pcVar4 + 2) & 0x1f00) != 0) && (*(char *)0x604 != '\x15')) {
        uVar5 = (undefined2)((ulong)*(undefined4 *)0x6f2 >> 0x10);
        func_0x000831f1((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                        (int)*(undefined4 *)0x6f2,uVar5,*(undefined1 *)0x606);
        func_0x00082d9b((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,in_BX,uVar5);
        if (*(char *)0x606 == '=') {
          if ((*(int *)0x5f8 < 1) || (4 < *(int *)0x5f8)) {
            if ((4 < *(int *)0x5f8) && (*(int *)0x5f8 < 0x1f)) {
              func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                              *(int *)0x5f8 + 0x3cU & 0xff);
            }
          }
          else {
            func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                            *(int *)0x5f8 + 0x30U & 0xff);
          }
        }
        func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x3a);
      }
      (**(code **)(pcVar4 + 8))((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
      *(undefined2 *)0x60a = in_BX;
      *(undefined2 *)0x60c = uVar2;
      if ((*(int *)0x60a != 0 || *(int *)0x60c != 0) && (*(char *)0x623 == '\0')) {
        func_0x00082d9b((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                        (int)*(undefined4 *)0x60a,(int)((ulong)*(undefined4 *)0x60a >> 0x10));
        if (((int)*(undefined4 *)0x60a == (int)*(undefined4 *)0x6f2 + 0x17b) &&
           ((int)((ulong)*(undefined4 *)0x60a >> 0x10) == (int)((ulong)*(undefined4 *)0x6f2 >> 0x10)
           )) {
          if ((*(uint *)(pcVar4 + 2) & 0x1f00) != 0) {
            *(undefined2 *)0x5f8 = *(undefined2 *)0x600;
          }
          if ((*(int *)0x5f8 < 1) || (4 < *(int *)0x5f8)) {
            if ((4 < *(int *)0x5f8) && (*(int *)0x5f8 < 0x1f)) {
              func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                              *(int *)0x5f8 + 0x3cU & 0xff);
            }
          }
          else {
            func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                            *(int *)0x5f8 + 0x30U & 0xff);
          }
        }
      }
      *(undefined1 *)0x602 = 0xff;
      uVar2 = 0xffff;
    }
  }
  return uVar2;
}



undefined2 __stdcall16far FUN_0008_4be6(byte *param_1)

{
  byte bVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  bVar1 = *param_1;
  if ((((((*(byte *)0x605 & 0x10) == 0) || ((*(uint *)((byte *)param_1 + 2) & 0x20) == 0)) &&
       (((*(byte *)0x605 & 0x20) == 0 || ((*(uint *)((byte *)param_1 + 2) & 0x2000) == 0)))) &&
      (bVar1 != 0xfb)) || (*"B" != '\0')) {
    if ((((bVar1 < 0xc9) || (bVar1 == 0xfd)) && (*"B" == '\x04')) ||
       ((bVar1 == 0xfc && (*"B" == '\x05')))) {
      uVar2 = 0xffff;
    }
    else if (((bVar1 == 0xfe) && (*"B" == '\x03')) || ((bVar1 == 0xff && (*"B" == '\x02')))) {
      uVar2 = 0xffff;
    }
    else {
      *(byte *)0x626 = ~bVar1 & 0xf;
      if (*"B" == '\0') {
        *(undefined1 *)0x623 = 7;
      }
      uVar2 = 0;
    }
  }
  else {
    uVar2 = 0xffff;
  }
  return uVar2;
}



undefined2 __stdcall16far FUN_0008_4ca7(undefined4 param_1)

{
  uint uVar1;
  undefined2 uVar2;
  uint uVar3;
  undefined2 unaff_DS;
  
  uVar1 = *(uint *)((int)param_1 + 2);
  uVar3 = uVar1 & 0x1b00;
  if (((uVar1 & 0x1f00) == 0) && (*"" == '\0')) {
    uVar2 = 0xffff;
  }
  else if ((((uVar1 & 0x1f00) == 0) || (0x17ff < (uVar1 & 0x1f00))) ||
          ((*"" != '\x04' && (*"" != '\0')))) {
    if (((((uVar1 & 0x1c00) == 0x1c00) && (*"" == '\0')) ||
        ((((uVar3 == 0x1800 && (*"" == '\x02')) || ((uVar3 == 0x1900 && (*"" == '\x03')))) ||
         ((uVar3 == 0x1a00 && (*"" == '\x04')))))) || ((uVar3 == 0x1b00 && (*"" == '\x05')))) {
      uVar2 = 0xffff;
    }
    else {
      *(byte *)0x626 = (byte)uVar1 & 0x1f | 0x80;
      uVar2 = 0;
    }
  }
  else {
    uVar2 = 0xffff;
  }
  return uVar2;
}



undefined2 __stdcall16far FUN_0008_4d68(int param_1,uint param_2,uint param_3)

{
  undefined2 uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  
  if (param_1 == 1) {
    param_3 = param_3 & 0xff;
  }
  uVar2 = ~param_2 & param_3;
  if ((uVar2 & 0x2020) == 0) {
    if (*(char *)((int)*(undefined4 *)0x732 + 0x96) == '\0') {
      if ((uVar2 & 0x4000) != 0) {
        *(undefined1 *)0x626 = 0x23;
        *(undefined1 *)0x623 = 0x1d;
        return 0;
      }
    }
    else if ((uVar2 & 0x8000) != 0) {
      *(undefined1 *)0x626 = 0x22;
      *(undefined1 *)0x623 = 0x1c;
      return 0;
    }
    if (((*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x14') && ((uVar2 & 1) != 0)) ||
       ((*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x15' && ((uVar2 & 8) != 0)))) {
      if (*"B" != '\0') {
        *(undefined1 *)0x626 = 0x24;
        *(undefined1 *)0x623 = 0x1e;
      }
      *(undefined1 *)0x624 = 0x1e;
      uVar1 = 0;
    }
    else if ((*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x1e') && ((uVar2 & 2) != 0)) {
      if (*"B" != '\0') {
        *(undefined1 *)0x626 = 0x25;
        *(undefined1 *)0x623 = 0x1f;
      }
      *(undefined1 *)0x624 = 0x1f;
      uVar1 = 0;
    }
    else if (((*(char *)0x605 == '\x10') &&
             (((*"\x05" == '\0' && ((param_2 & param_3 & 4) != 0)) && (*"\t" != '\0')))) &&
            ((*(byte *)0x4f1 & 0x80) == 0)) {
      *(undefined1 *)0x626 = 0x28;
      *(undefined1 *)0x623 = 0x12;
      uVar1 = 0;
    }
    else {
      uVar1 = 0xffff;
    }
  }
  else {
    *(undefined1 *)0x626 = 0x21;
    uVar1 = 0;
  }
  return uVar1;
}



void __cdecl16far FUN_0008_4e99(void)

{
  char *unaff_CS;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x5ff = 0;
  *(undefined1 *)0x5f7 = 0;
  *(undefined1 *)0x609 = 0;
  *(undefined1 *)0x5fe = 0;
  *(undefined1 *)0x606 = 0;
  *(undefined2 *)0x600 = 0;
  *(undefined2 *)0x5f8 = 0;
  if (((*(char *)0x4 == ' ') || (*(char *)0x4 == '\n')) || (*(char *)0x4 == '\r')) {
    if (*(char *)0x2 == '\0') {
      if (*(char *)0x3 != '\0') {
        func_0x000968a2();
        unaff_CS = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
        func_0x00082fd7(0x9687);
      }
    }
    else {
      *(undefined1 *)0x5a4 = 0x14;
    }
  }
  if (*(char *)0x4 == ',') {
    if (*(char *)0x2 == '\0') {
      if (*(char *)0x3 != '\0') {
        func_0x000968a2(unaff_CS);
        func_0x00082fd7(0x9687);
        *(undefined1 *)0x5a4 = 7;
      }
    }
    else {
      *(undefined1 *)0x5a4 = 0x14;
    }
  }
  else {
    *(undefined1 *)0x5a4 = 0x13;
  }
  return;
}



void __cdecl16far FUN_0008_4f31(void)

{
  return;
}



void __cdecl16far FUN_0008_4f3b(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x2 == '\0') {
    if (*(char *)0x4 == ';') {
      if ((*(byte *)0x605 & 0x20) != 0) {
        func_0x00083241();
      }
      *(undefined1 *)0x5a4 = 3;
    }
    else if (((*(char *)0x4 == ' ') || (*(char *)0x4 == '\r')) || (*(char *)0x4 == '\n')) {
      if (*(char *)0x3 != '\0') {
        func_0x000968a2();
        func_0x0009699b(0x9687);
      }
    }
    else {
      *(undefined1 *)0x623 = 8;
      *(undefined1 *)0x5a4 = 1;
      func_0x0009699b();
    }
  }
  else {
    if ((*(byte *)0x605 & 0x20) != 0) {
      func_0x00083241();
    }
    *(undefined1 *)0x5a4 = 0x14;
  }
  return;
}



void __cdecl16far FUN_0008_4fbf(void)

{
  char *unaff_CS;
  char *pcVar1;
  undefined2 unaff_DS;
  
  if (*"\t" == '\0') {
    pcVar1 = unaff_CS;
    if (*(char *)0x602 == '\0') {
      func_0x00082e3d();
      pcVar1 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
      func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x3b);
    }
    unaff_CS = (char *)0x9687;
    func_0x00096c5f(pcVar1);
  }
  pcVar1 = unaff_CS;
  if ((*(byte *)0x603 & 1) != 0) {
    pcVar1 = (char *)0xf5f5;
    func_0x000f635e(unaff_CS);
  }
  if ((*(byte *)0x603 & 2) != 0) {
    func_0x0009470e(pcVar1,0x41);
    func_0x000e6c85(38000);
    func_0x0009470e(0xe6a8,(char *)s_TRIggerd_0008_8ef8 + 8);
  }
  *(undefined1 *)0x603 = 0;
  *(undefined1 *)0x5a4 = 0x15;
  return;
}



void __cdecl16far FUN_0008_5022(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (*"\t" == '\0') {
    func_0x00096aaf();
    *(undefined1 *)0x642 = 0;
    *(undefined1 *)0x602 = 0;
    *(undefined1 *)0x5a4 = 3;
  }
  else {
    do {
      if (*(char *)0x3 == '\0') {
        if (*(char *)0x2 == '\0') {
          return;
        }
        func_0x00096c5f(unaff_CS);
        func_0x00096aaf(0x9687);
        *(undefined1 *)0x5a4 = 3;
        *(undefined1 *)0x602 = 0;
        return;
      }
      func_0x000968a2(unaff_CS);
      unaff_CS = 0x9687;
      func_0x0009699b(0x9687);
    } while (((*(char *)0x4 == '\r') || (*(char *)0x4 == '\n')) || (*(char *)0x4 == ' '));
    *(undefined1 *)0x623 = 8;
    *(undefined1 *)0x5a4 = 1;
  }
  return;
}



void __cdecl16far FUN_0008_5098(void)

{
  return;
}



void __cdecl16far FUN_0008_50a2(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x5f4 = 0;
  return;
}



void __cdecl16far FUN_0008_50b2(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x5f4 = 0;
  return;
}



void __cdecl16far FUN_0008_50c2(void)

{
  undefined2 unaff_DS;
  
  if (*(int *)0x5f4 < 100) {
    *(int *)0x5f4 = *(int *)0x5f4 + 1;
  }
  else {
    *(undefined1 *)0x623 = 0xb;
    *(undefined1 *)0x5a4 = 1;
  }
  return;
}



void __cdecl16far FUN_0008_50e4(void)

{
  undefined1 *puVar1;
  undefined2 unaff_DS;
  
  if (*(int *)0x5f4 < 0x47) {
    puVar1 = (undefined1 *)*(undefined4 *)0x5f0;
    *(int *)0x5f0 = *(int *)0x5f0 + 1;
    *puVar1 = *(undefined1 *)0x4;
    *(int *)0x5f4 = *(int *)0x5f4 + 1;
  }
  else {
    *(undefined1 *)0x623 = 0xb;
    *(undefined1 *)0x5a4 = 1;
  }
  return;
}



void __cdecl16far FUN_0008_519a(void)

{
  undefined4 uVar1;
  undefined2 unaff_DS;
  
  func_0x00096ce9();
  func_0x00082fd7(0x9687);
  if ((*(byte *)0x4 < 0x41) || (0x5a < *(byte *)0x4)) {
    *(undefined1 *)0x623 = 4;
  }
  else {
    uVar1 = *(undefined4 *)((*(byte *)0x4 - 0x41) * 6 + (int)*(undefined4 *)0x6f6 + 2);
    *(undefined2 *)0x60e = (int)uVar1;
    *(undefined2 *)0x610 = (int)((ulong)uVar1 >> 0x10);
    *(undefined1 *)0x608 = *(undefined1 *)((*(byte *)0x4 - 0x41) * 6 + (int)*(undefined4 *)0x6f6);
    func_0x00096ce9((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
    func_0x00083048(0x9687);
    if (*(char *)0x607 == '\0') {
      *(undefined1 *)0x623 = 4;
    }
  }
  return;
}



void __cdecl16far FUN_0008_5213(void)

{
  return;
}



void __cdecl16far FUN_0008_521d(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0xc != '\0') {
    *(undefined1 *)0xc = 0;
    *(undefined1 *)0x454 = 0;
  }
  return;
}



/* reset_comm_parser_state (confidence: Mechanism confirmed; exact per-variable meanings not
   confirmed)
   
   Evidence: Clears a cluster of comm ROM parser/command state variables; called as part of the comm
   channel reinitialization sequence in `SUB_97B01` */

void __cdecl16far reset_comm_parser_state(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x62a = 0;
  *(undefined1 *)0x63b = 0;
  *(undefined1 *)0x63a = 0;
  *(undefined2 *)0x632 = 0;
  *(undefined1 *)0x6 = 0;
  *(undefined1 *)0x61b = 0;
  *(undefined1 *)0x5a4 = 0;
  *(undefined1 *)0x3 = 0;
  return;
}



/* process_gpib_command_byte (confidence: Mechanism confirmed; exact GPIB protocol semantics not
   confirmed. **Traced further 2026-09-13** (looking into what commands the RS-232 side expects, see
   `docs/comm-rom/rs232-early-investigation.md`'s "Traced the comm ROM's byte-dispatch/parser core"
   section): `[0x712]` is a far pointer to a **4-byte-per-entry dispatch table**, indexed by `[6]`
   clamped to a max of `0x42` (67 possible entries) - a per-input-byte-class lookup, the clearest
   evidence yet of a real character/byte classification table feeding the command parser. Explicitly
   checks `[0x629]` (RS-232-vs-other mode flag) before deciding whether to call
   `finish_comm_response` - confirms the RS-232 path is a genuine, actively-maintained branch
   through this code, not dead/GPIB-only logic)
   
   Evidence: Handles a received GPIB byte `[6]` under the critical-section lock: bit `0x80` set ->
   marks `[0x590]=0x80`; else clamps and records it into a per-row scratch slot via the `[0x712]`
   address-to-index table; always calls `SUB_97905`, and conditionally triggers a query response via
   `print_param_list_response` */

void __cdecl16far process_gpib_command_byte(void)

{
  undefined1 uVar1;
  char *pcVar2;
  undefined2 unaff_DS;
  
  uVar1 = *(undefined1 *)0x5a3;
  *(undefined1 *)0x5a3 = 0xff;
  if ((*(byte *)0x6 & 0x80) == 0) {
    if (0x42 < *(byte *)0x6) {
      *(undefined1 *)0x6 = 0x26;
    }
    *(int *)0x582 = *(byte *)((uint)*(byte *)0x6 * 4 + (int)*(undefined4 *)0x712) + 0x586;
    *(undefined2 *)0x584 = unaff_DS;
    if (*(char *)*(undefined4 *)0x582 == '\0') {
      *(undefined1 *)*(undefined4 *)0x582 = *(undefined1 *)0x6;
    }
  }
  else {
    *(undefined1 *)0x590 = 0x80;
  }
  pcVar2 = (char *)0x96f5;
  func_0x00097905();
  if (((*"\t" == '\0') && (*"" != '\0')) && (*(char *)0x57f != '\0')) {
    *(undefined1 *)0x6 = 0;
    pcVar2 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
    func_0x00082eb9(0x96f5);
  }
  func_0x00097b94(pcVar2,uVar1);
  return;
}



void __stdcall16far FUN_0008_5301(undefined1 param_1)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  char *pcVar2;
  
  pcVar2 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar1 = func_0x0009470e();
  *(undefined1 *)0x6 = param_1;
  func_0x0008526b(38000,pcVar2,uVar1);
  func_0x0009470e(0x8511,uVar1);
  return;
}



void __cdecl16far FUN_0008_532d(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  
  uVar1 = 0;
  func_0x000944a2();
  *(undefined1 *)((int)*(undefined4 *)0x732 + 0x94) = 0;
  func_0x0009470e(0x941f,0x41,uVar1);
  func_0x000e6dc4(38000);
  func_0x0009470e(0xe6a8,(char *)s_TRIggerd_0008_8ef8 + 8);
  return;
}



void __cdecl16far FUN_0008_5367(void)

{
  undefined2 unaff_DS;
  
  func_0x000944a2();
  *(undefined1 *)((int)*(undefined4 *)0x732 + 0x94) = 0;
  return;
}



void __cdecl16far FUN_0008_538a(void)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  uVar2 = 0x20;
  iVar1 = func_0x00094488();
  if (iVar1 == 0) {
    func_0x000944a2(0x941f,0x1d,1,uVar2);
  }
  else {
    *(undefined1 *)0x623 = 0x1b;
  }
  return;
}



void __cdecl16far FUN_0008_53ba(void)

{
  undefined2 uVar1;
  
  uVar1 = 0;
  func_0x000944a2();
  func_0x000944a2(0x941f,0x20,2,uVar1);
  return;
}



void __cdecl16far FUN_0008_53e3(void)

{
  undefined2 uVar1;
  
  uVar1 = 0;
  func_0x000944a2();
  func_0x000944a2(0x941f,0x20,3,uVar1);
  return;
}



void __cdecl16far FUN_0008_540c(void)

{
  undefined2 uVar1;
  
  uVar1 = 0;
  func_0x000944a2();
  func_0x000944a2(0x941f,0x20,1,uVar1);
  return;
}



void __cdecl16far FUN_0008_5435(void)

{
  undefined2 uVar1;
  
  uVar1 = 0;
  func_0x000944a2();
  func_0x000944a2(0x941f,0x20,4,uVar1);
  return;
}



void __cdecl16far FUN_0008_545e(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0008_5477(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0008_5490(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0008_54aa(void)

{
  int iVar1;
  char *unaff_CS;
  undefined2 unaff_DS;
  
  if (*"" == '\0') {
    iVar1 = 1;
  }
  else {
    unaff_CS = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
    iVar1 = func_0x000836aa();
  }
  if (*(char *)0x623 == '\0') {
    if (iVar1 < 1) {
      *(undefined1 *)0x623 = 0x15;
    }
    else {
      iVar1 = (iVar1 + -1) * 0x40;
      if (iVar1 < 0x260) {
        func_0x0009470e(unaff_CS,0x41);
        *(int *)0x3e0 = iVar1;
        func_0x0009470e(38000,(char *)s_TRIggerd_0008_8ef8 + 8);
        *(byte *)0x603 = *(byte *)0x603 | 1;
      }
      else {
        *(undefined1 *)0x623 = 0x15;
      }
    }
  }
  return;
}



void __cdecl16far FUN_0008_5517(void)

{
  byte *pbVar1;
  int iVar2;
  char *pcVar3;
  undefined2 unaff_DS;
  
  if (*(char *)((int)*(undefined4 *)0x732 + 0x93) == '\0') {
    pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
    *pbVar1 = *pbVar1 | 9;
    func_0x000839d1();
    func_0x0009470e((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x41);
    *(undefined1 *)0x706 = 0;
    func_0x000e92b0(38000);
    func_0x0009470e(0xe925,(char *)s_TRIggerd_0008_8ef8 + 8);
    *(undefined1 *)0x63c = 0xff;
    *(undefined1 *)((int)*(undefined4 *)0x732 + 0x93) = 1;
    pcVar3 = (char *)0x941f;
    iVar2 = func_0x00094488(38000,0x20);
    if (iVar2 != 0) {
      while (*(char *)((int)*(undefined4 *)0x732 + 0x93) != '\0') {
        func_0x000838c0(pcVar3);
        pcVar3 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
      }
    }
    pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
    *pbVar1 = *pbVar1 & 0xfe;
    func_0x000839d1(pcVar3);
  }
  else {
    *(undefined1 *)0x623 = 0x1b;
  }
  return;
}



void __cdecl16far FUN_0008_55a3(void)

{
  func_0x00094488();
  return;
}



void __cdecl16far FUN_0008_5625(void)

{
  func_0x00094488();
  return;
}



void __cdecl16far FUN_0008_5650(void)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  func_0x0009470e();
  iVar1 = *(int *)0x3e0 >> 6;
  func_0x0009470e(38000,(char *)s_TRIggerd_0008_8ef8 + 8,uVar2,iVar1);
  func_0x00081404(38000,iVar1 + 1);
  return;
}



void __cdecl16far FUN_0008_568d(void)

{
  func_0x00094488();
  return;
}



void __cdecl16far FUN_0008_56df(void)

{
  undefined2 uVar1;
  
  uVar1 = func_0x00085d91();
  func_0x00081404(0x85d8,uVar1);
  return;
}



void __cdecl16far FUN_0008_56f8(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x61c = 0xff;
  *(undefined1 *)0x6 = *(undefined1 *)0x590;
  func_0x0008526b();
  return;
}



void __cdecl16far FUN_0008_5712(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x61c = 0;
  *(undefined1 *)0x6 = *(undefined1 *)0x590;
  func_0x0008526b();
  return;
}



void __cdecl16far FUN_0008_572c(void)

{
  return;
}



void __cdecl16far FUN_0008_5750(void)

{
  undefined2 unaff_DS;
  
  if (*"}4" == '\0') {
    *(undefined1 *)0x61b = 0;
  }
  *(undefined1 *)0x61a = 0xff;
  return;
}



void __cdecl16far FUN_0008_576b(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x61a = 0;
  *(undefined1 *)0x61b = 0;
  return;
}



void __cdecl16far FUN_0008_577f(void)

{
  return;
}



void __cdecl16far FUN_0008_57a3(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x4f2 = 0xff;
  return;
}



void __cdecl16far FUN_0008_57b2(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x4f2 = 0;
  return;
}



void __cdecl16far FUN_0008_57c1(void)

{
  return;
}



void __cdecl16far FUN_0008_57e5(void)

{
  undefined4 uVar1;
  char *pcVar2;
  undefined2 unaff_DS;
  char local_d;
  undefined4 local_c;
  
  uVar1 = *(undefined4 *)0x6e6;
  *(undefined2 *)0x60a = (int)uVar1;
  *(undefined2 *)0x60c = (int)((ulong)uVar1 >> 0x10);
  *(undefined1 *)0x608 = 0;
  func_0x00082dff();
  do {
    if (*(char *)*(undefined4 *)0x60a == '\0') {
      return;
    }
    *(char *)0x608 = *(char *)0x608 + '\x01';
    local_d = -1;
    if ((((*(char *)0x608 == '\x01') || (*(char *)0x608 == '\x03')) || (*(char *)0x608 == '\"')) ||
       ((*(char *)0x608 == ',' || (*(char *)0x608 == '\x13')))) {
      local_d = '\0';
    }
    else if (*(char *)((int)*(undefined4 *)0x732 + 0x1f) != '\x1e') {
      local_c = (char *)*(undefined4 *)0x71e;
      if (*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x15') {
        local_c = (char *)CONCAT22((int)((ulong)local_c >> 0x10),(char *)local_c + 8);
      }
      do {
        pcVar2 = local_c;
        if (*local_c == -1) goto LAB_0008_587c;
        local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
      } while (*(char *)0x608 != *pcVar2);
      local_d = '\0';
    }
LAB_0008_587c:
    if (local_d == '\0') {
      *(int *)0x60a = *(int *)0x60a + (*(byte *)*(undefined4 *)0x60a & 0xf) + 1;
    }
    else {
      *(undefined1 *)0x5 = 0x2c;
      func_0x00096b68((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18);
      func_0x00082dff(0x9687);
    }
    if (*(char *)*(undefined4 *)0x60a == '\0') {
      *(int *)0x60a = *(int *)0x60a + 1;
    }
  } while( true );
}



void __cdecl16far FUN_0008_58c9(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  char *pcVar2;
  
  pcVar2 = (char *)s_Delete_failed_0008_906a + 7;
  uVar1 = 0x234;
  func_0x00082d01();
  func_0x00081404((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,
                  *(undefined1 *)((int)*(undefined4 *)0x732 + 0x1f),uVar1,pcVar2);
  func_0x00082d01(0x802c,0x23b,(char *)s_Delete_failed_0008_906a + 7);
  func_0x00082c99((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,DAT_e000_0004);
  return;
}



void __cdecl16far FUN_0008_591c(void)

{
  undefined2 unaff_DS;
  undefined2 local_c;
  
  _local_c = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)0x620);
  *_local_c = *(undefined2 *)0x612;
  return;
}



void __cdecl16far FUN_0008_593b(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x612 = *(undefined2 *)0x620;
  func_0x00085985();
  return;
}



void __cdecl16far FUN_0008_5954(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x61e = *(undefined2 *)0x612;
  return;
}



void __cdecl16far FUN_0008_596c(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x612 = *(undefined2 *)0x61e;
  func_0x00085985();
  return;
}



void __cdecl16far FUN_0008_5985(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  uVar2 = (undefined2)((ulong)*(undefined4 *)0x722 >> 0x10);
  uVar1 = (undefined2)*(undefined4 *)0x722;
  func_0x00082d01();
  func_0x00082c99((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,*(int *)0x612 >> 8,uVar1,
                  uVar2);
  func_0x00082c99((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,*(undefined2 *)0x612);
  return;
}



void __cdecl16far FUN_0008_59bc(void)

{
  undefined2 uVar1;
  undefined1 *puVar2;
  undefined2 unaff_DS;
  
  uVar1 = *(undefined2 *)0x612;
  puVar2 = (undefined1 *)*(undefined4 *)0x61e;
  *(int *)0x61e = *(int *)0x61e + 1;
  *puVar2 = (char)uVar1;
  return;
}



void __cdecl16far FUN_0008_59d9(void)

{
  byte *pbVar1;
  int unaff_SI;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  
  if (*"B" == '\0') {
    unaff_SI = 1;
  }
  else if ((*"B" == '\x05') && (unaff_SI = *(int *)0x5f8, unaff_SI < 1)) {
    *(undefined1 *)0x623 = 0x15;
    return;
  }
  uVar3 = (undefined2)((ulong)*(undefined4 *)0x722 >> 0x10);
  uVar2 = (undefined2)*(undefined4 *)0x722;
  func_0x00082d01();
  while (unaff_SI = unaff_SI + -1, -1 < unaff_SI) {
    pbVar1 = (byte *)*(undefined4 *)0x61e;
    *(int *)0x61e = *(int *)0x61e + 1;
    *(uint *)0x612 = (uint)*pbVar1;
    func_0x00082c99((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,*(undefined2 *)0x612,uVar2
                    ,uVar3);
  }
  return;
}



void __cdecl16far FUN_0008_5a44(void)

{
  undefined2 unaff_DS;
  
  (*(code *)*(undefined2 *)0x61e)();
  return;
}



void __cdecl16far FUN_0008_5a52(void)

{
  undefined2 uVar1;
  
  uVar1 = func_0x00082f14();
  func_0x00081404((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,uVar1);
  return;
}



void __cdecl16far FUN_0008_5a6b(void)

{
  uint uVar1;
  char *pcVar2;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x4f3 = 0xff;
  uVar1 = (uint)*(byte *)0x5ff;
  func_0x000911e4();
  pcVar2 = (char *)s_Lock_Unlock_failed_0008_9096 + 8;
  while (*(char *)0x4f3 != '\0') {
    func_0x000838c0(pcVar2,uVar1);
    pcVar2 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
  }
  return;
}



void __cdecl16far FUN_0008_5a9b(void)

{
  undefined2 unaff_DS;
  
  *(byte *)0x627 = *(byte *)0x627 | 0x80;
  return;
}



void __cdecl16far FUN_0008_5aaa(void)

{
  undefined2 unaff_DS;
  
  *(byte *)0x627 = *(byte *)0x627 & 0x7f;
  return;
}



void __cdecl16far FUN_0008_5ab9(void)

{
  return;
}



void __cdecl16far FUN_0008_5adf(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x45b = 0xff;
  *(byte *)0x603 = *(byte *)0x603 | 1;
  return;
}



void __cdecl16far FUN_0008_5af3(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x45b = 0;
  *(byte *)0x603 = *(byte *)0x603 | 1;
  return;
}



void __cdecl16far FUN_0008_5b07(void)

{
  return;
}



void __cdecl16far FUN_0008_5b2b(void)

{
  undefined2 unaff_DS;
  
  if ((*(int *)0x5f8 == 1) || (*(int *)0x5f8 == 2)) {
    *(int *)0x5f8 = *(int *)0x5f8 + 1;
    *(byte *)0x59a = *(byte *)0x59a & 0xfe | (byte)*(undefined2 *)0x5f8 & 1;
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 1) = *(undefined1 *)0x59a;
    *(byte *)0x603 = *(byte *)0x603 | 1;
  }
  else {
    *(undefined1 *)0x623 = 0x15;
  }
  return;
}



void __cdecl16far FUN_0008_5b75(void)

{
  func_0x00083241();
  return;
}



void __cdecl16far FUN_0008_5b9b(void)

{
  char cVar1;
  char *unaff_CS;
  undefined2 unaff_DS;
  int local_e;
  char *local_c;
  
  local_e = -1;
  local_c = (char *)*(undefined4 *)0x726;
  while ((*(char *)0x623 == '\0' && (*local_c != -1))) {
    if (((*local_c != '\x19') && (*local_c != '\x06')) ||
       (*(char *)((int)*(undefined4 *)0x732 + 0x1f) != '\x14')) {
      if (*local_c == -2) {
        cVar1 = *"\t";
      }
      else {
        *(char *)0x604 = *local_c;
        if (local_e == 0) {
          func_0x00083241(unaff_CS,0x3b);
          unaff_CS = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
        }
        else {
          local_e = 0;
        }
        func_0x00085c9d(unaff_CS,0x80);
        cVar1 = *(char *)0x623;
        unaff_CS = (char *)0x856c;
      }
      if (cVar1 != '\0') break;
    }
    local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
  }
  *(undefined1 *)0x604 = 0x23;
  return;
}



void __cdecl16far FUN_0008_5c31(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  
  uVar1 = 0x41;
  func_0x0009470e();
  func_0x000e6ec0(38000,uVar1);
  func_0x0009470e(0xe6a8,(char *)s_TRIggerd_0008_8ef8 + 8);
  *(undefined1 *)0x4f4 = 0;
  func_0x000821f7(38000,1);
  *(byte *)0x603 = *(byte *)0x603 | 3;
  return;
}



void __cdecl16far FUN_0008_5c65(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  
  *(undefined1 *)0x604 = 0x2b;
  uVar1 = 0x2000;
  func_0x00085c9d();
  func_0x00083241(0x856c,0x3b,uVar1);
  *(undefined1 *)0x604 = 7;
  func_0x00085c9d((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x2000);
  *(undefined1 *)0x604 = 0x2a;
  return;
}



/* compute_response_format_flags (confidence: Mechanism traced via direct capstone decode; exact
   downstream use of the flags word (the `[0x6fe]` table lookup) not confirmed. **Open question**:
   whether `[0x732+0x1F]`'s value `0x1E` at *boot* time (checked by
   `init_comm_device_type_and_defaults`, before any command has been parsed) is a genuine
   device-type code that coincidentally matches `REFStat`'s command ID, or whether this field is
   truly dual-purpose (device type at boot, current-command ID once the parser is running) - not
   resolved)
   
   Evidence: `(word flags, passed by value)` - reads `es:[0x732+0x1F]` (same field
   `init_comm_device_type_and_defaults` checks at boot) and ORs a bit into the caller's flags word
   for exactly 3 values: `0x14`->bit0, `0x15`->bit3, `0x1E`->bit1. Cross-referencing
   `disasm/comm_keyword_tables.json`'s alphabetical header-table order, these are the numeric
   command IDs for `LONg`, `MESsage`, and `REFStat` - all three commands whose *response* involves
   either a verbosity toggle or literal quoted text, suggesting this marks "needs quote/string-aware
   response formatting." Also ORs in `0x8000` (GPIB) or `0x4000` (RS-232) based on `[0x629]`. Called
   3x from a small caller that sets `[0x604]` to a phase value (`0x2b`/`7`/`0x2a` seen) then walks a
   12-byte-record table at far ptr `[0x6fe]`, matching each entry's required-flags word against the
   computed value - looks like part of a response-formatting-behavior lookup, not the command's
   actual state-change handler. See `docs/comm-rom/rs232-live-session-2026-09-14.md`'s "Tried
   tracing `MESsage`" section for the full trace and how this was found (searching for `cmp ax,
   0x15` against MESsage's known dispatch ID) */

void __stdcall16far compute_response_format_flags(word flags)

{
  char cVar1;
  byte bVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  undefined2 unaff_CS;
  undefined2 uVar6;
  undefined2 unaff_DS;
  byte local_9;
  
  cVar1 = *(char *)((int)*(undefined4 *)0x732 + 0x1f);
  if (cVar1 == '\x14') {
    flags = flags | 1;
  }
  else if (cVar1 == '\x15') {
    flags = flags | 8;
  }
  else if (cVar1 == '\x1e') {
    flags = flags | 2;
  }
  if (*"\t" == '\0') {
    uVar5 = 0x4000;
  }
  else {
    uVar5 = 0x8000;
  }
  *(undefined2 *)0x5fc = 0;
  cVar1 = *(char *)((uint)*(byte *)0x604 * 2 + (int)*(undefined4 *)0x702 + 1);
  bVar2 = *(byte *)((uint)*(byte *)0x604 * 2 + (int)*(undefined4 *)0x702);
  local_9 = 0;
  while( true ) {
    if (bVar2 <= local_9) {
      return;
    }
    iVar4 = (uint)(byte)(cVar1 + local_9) * 0xc;
    uVar3 = *(uint *)(iVar4 + (int)*(undefined4 *)0x6fe + 2);
    uVar6 = unaff_CS;
    if ((uVar3 & (flags | uVar5)) == (flags | uVar5)) {
      *(undefined1 *)0x606 = *(undefined1 *)(iVar4 + (int)*(undefined4 *)0x6fe);
      uVar6 = 0x839f;
      func_0x000849ec(unaff_CS,iVar4 + (int)*(undefined4 *)0x6fe,
                      (int)((ulong)*(undefined4 *)0x6fe >> 0x10),0xc00f,uVar3);
    }
    if (*(char *)0x623 != '\0') break;
    local_9 = local_9 + 1;
    unaff_CS = uVar6;
  }
  return;
}



undefined2 __cdecl16far FUN_0008_5d91(void)

{
  undefined2 uVar1;
  int iVar2;
  int unaff_DS;
  char *local_c;
  
  if ((*"" == '\0') || ((*(byte *)0x581 & 0x80) != 0)) {
    _local_c = (char *)CONCAT22(unaff_DS,(char *)0x587);
    while( true ) {
      iVar2 = (int)((ulong)_local_c >> 0x10);
      if (*_local_c != '\0') break;
      _local_c = (char *)CONCAT22(iVar2,(char *)_local_c + 1);
    }
    *(char *)0x581 = *_local_c;
    if (((char *)_local_c != (char *)0x590) || (iVar2 != unaff_DS)) {
      *_local_c = '\0';
    }
  }
  uVar1 = 0;
  if ((*(byte *)0x581 & 0x80) == 0) {
    uVar1 = *(undefined2 *)((uint)*(byte *)0x581 * 4 + (int)*(undefined4 *)0x712 + 2);
  }
  *(undefined1 *)0x581 = 0x80;
  return uVar1;
}



/* init_comm_device_type_and_defaults (confidence: Confirmed)
   
   Evidence: Detects device type (`es:[0x732+0x1F]==0x1E` -> `[0x4F5]=1` else `4`), sets buffer-size
   defaults `[0x4FC]`/`[0x4F8]`/`[0x4FA]`, calls `init_comm_default_params(1)` */

void init_comm_device_type_and_defaults(void)

{
  undefined1 uVar1;
  undefined2 unaff_DS;
  
  if (*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x1e') {
    uVar1 = 1;
  }
  else {
    uVar1 = 4;
  }
  *(undefined1 *)0x4f5 = uVar1;
  *(undefined2 *)0x4fc = 1;
  *(undefined2 *)0x4f8 = 2;
  *(undefined2 *)0x4fa = 1;
  func_0x000821f7();
  *(undefined1 *)0x628 = 0xff;
  *(undefined1 *)0x61c = 1;
  *(undefined1 *)0x639 = 0;
  *(undefined1 *)0x636 = 0;
  *(undefined1 *)0x4f2 = 0xff;
  return;
}



void __stdcall16far FUN_0008_5ebe(int param_1)

{
  undefined4 uVar1;
  char cVar2;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  if (param_1 == 1) {
    func_0x00086647();
    *(undefined1 *)0x4cd = 0;
  }
  else if (param_1 == 3) {
    *(undefined2 *)0x3da = 0;
    if (((*(byte *)0x4e8 & 0x80) != 0) && ((*(byte *)0x4e7 & 0x80) != 0)) {
      if (*(char *)0x4cd == '\0') {
        *(undefined1 *)0x4cd = 1;
      }
      else if (*(char *)0x4cd == '\x01') {
        *(undefined1 *)0x4cd = 2;
      }
      else if (*(char *)0x4cd == '\x02') {
        *(undefined1 *)0x4cd = 0;
        unaff_CS = 0x85eb;
        func_0x000860b1();
      }
      *(undefined1 *)0x46a = 1;
      *(undefined2 *)0x4c6 = 0;
    }
    if (*(int *)0x52a != 0) {
      *(int *)0x4c6 = *(int *)0x4c6 + *(int *)0x52a;
      if ((*(byte *)0x4cd < 2) || ((*(int *)0x4c6 < 0x33 && (-0x33 < *(int *)0x4c6)))) {
        if (*(int *)0x4c6 < 0xfb) {
          if ((*(int *)0x4c6 < -0xfa) &&
             (cVar2 = func_0x0008718d(unaff_CS,*(undefined1 *)0x4cd,0xffff,1), cVar2 != '\0')) {
            *(undefined2 *)0x4c6 = 0;
            *(undefined1 *)0x46a = 1;
            *(char *)(*(byte *)0x4cd + 0x4ca) = cVar2;
          }
        }
        else {
          cVar2 = func_0x0008718d(unaff_CS,*(undefined1 *)0x4cd,1,1);
          if (cVar2 != '\0') {
            *(undefined2 *)0x4c6 = 0;
            *(undefined1 *)0x46a = 1;
            *(char *)(*(byte *)0x4cd + 0x4ca) = cVar2;
          }
        }
      }
      else {
        *(undefined1 *)0x4cd = 0;
      }
    }
  }
  else if (param_1 == 4) {
    uVar3 = 2;
    func_0x000866af();
    uVar1 = *(undefined4 *)0x4d2;
    *(int *)(code *)FUN_0008_03de = (int)uVar1;
    *(undefined2 *)0x460 = (int)((ulong)uVar1 >> 0x10);
    uVar1 = *(undefined4 *)0x4d6;
    *(undefined2 *)0x472 = (int)uVar1;
    *(undefined2 *)0x474 = (int)((ulong)uVar1 >> 0x10);
    if (*(char *)0x46a != '\0') {
      if (*(char *)0x4cd == '\0') {
        func_0x000872a4(0x85eb,0xe,(char *)s_Delete_failed_0008_906a + 7,uVar3);
      }
      else if (*(char *)0x4cd == '\x01') {
        func_0x000872a4(0x85eb,0x1c,(char *)s_Delete_failed_0008_906a + 7,uVar3);
      }
      else {
        func_0x000872a4(0x85eb,0x28,(char *)s_Delete_failed_0008_906a + 7,uVar3);
      }
      *(undefined1 *)0x46a = 0;
    }
    if (*"" != '\0') {
      if (*(int *)0x3da == 0) {
        func_0x000f3ea3(0x85eb,0,0x7d,0x3c,(char *)s_Delete_failed_0008_906a + 7,0x5c,
                        (char *)s_Delete_failed_0008_906a + 7);
      }
      else {
        func_0x00087367(0x85eb);
      }
      *(undefined1 *)0x468 = 0;
    }
  }
  return;
}



void __stdcall16far FUN_0008_60b1(int param_1)

{
  byte bVar1;
  byte bVar2;
  undefined4 uVar3;
  int iVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  
  if (param_1 == 1) {
    func_0x00086647();
  }
  else if (param_1 == 3) {
    *(undefined2 *)0x3da = 0;
    if (((*(byte *)0x4e8 & 0x80) != 0) && ((*(byte *)0x4e7 & 0x80) != 0)) {
      bVar1 = *(byte *)0x4ca;
      bVar2 = *(byte *)0x4cb;
      if ((bVar1 < 4) || (bVar2 < 4)) {
        iVar4 = func_0x000f0c2a();
        if (iVar4 == 0) {
          func_0x00087609(0xf0c2,bVar1,bVar2,0x3da);
          func_0x000f0c81(0x85eb,2);
        }
        else {
          *(undefined2 *)0x3da = 2;
        }
      }
      else {
        func_0x00087609();
      }
      *(undefined1 *)0x468 = 1;
      *(undefined1 *)0x46a = 1;
    }
  }
  else if (param_1 == 4) {
    uVar5 = 2;
    func_0x000866af();
    if (*(int *)0x4d2 == 0 && *(int *)0x4d4 == 0) {
      uVar3 = *(undefined4 *)(code *)FUN_0008_03de;
      *(int *)0x4d2 = (int)uVar3 + 6;
      *(undefined2 *)0x4d4 = (int)((ulong)uVar3 >> 0x10);
    }
    else {
      uVar3 = *(undefined4 *)0x4d2;
      *(int *)(code *)FUN_0008_03de = (int)uVar3;
      *(undefined2 *)0x460 = (int)((ulong)uVar3 >> 0x10);
    }
    if (*(char *)0x46a != '\0') {
      func_0x000872a4(0x85eb,0x5d,(char *)s_Delete_failed_0008_906a + 7,uVar5);
      *(undefined1 *)0x46a = 0;
    }
    if (*"" != '\0') {
      if (*(int *)0x3da == 0) {
        func_0x000f3ea3(0x85eb,0,0x7d,0x62,(char *)s_Delete_failed_0008_906a + 7,0x81,
                        (char *)s_Delete_failed_0008_906a + 7);
      }
      else {
        func_0x00087367(0x85eb);
      }
      *(undefined1 *)0x468 = 0;
    }
  }
  return;
}



void __stdcall16far FUN_0008_6205(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  
  if (param_1 == 1) {
    func_0x00086647();
    if (*(byte *)0x4cb < 0x61) {
      *(undefined1 *)0x4cb = 0x61;
    }
    else if (0x7a < *(byte *)0x4cb) {
      *(undefined1 *)0x4cb = 0x7a;
    }
  }
  else if (param_1 == 3) {
    if (*(int *)0x52a == 0) {
      if (((*(byte *)0x4e8 & 0x80) != 0) && ((*(byte *)0x4e7 & 0x80) != 0)) {
        uVar3 = func_0x00094ef5();
        if (uVar3 == 0) {
          *(undefined2 *)0x3da = 0;
        }
        else if ((uVar3 & 0x40) == 0) {
          if ((uVar3 & 2) == 0) {
            if ((uVar3 & 1) == 0) {
              *(undefined2 *)0x3da = 5;
            }
            else {
              *(undefined2 *)0x3da = 9;
            }
          }
          else {
            *(undefined2 *)0x3da = 0xc;
          }
        }
        else {
          *(undefined2 *)0x3da = 0xb;
        }
        *(undefined1 *)0x468 = 1;
        *(undefined1 *)0x46a = 1;
        *(undefined2 *)0x4c6 = 0;
      }
    }
    else {
      *(int *)0x4c6 = *(int *)0x4c6 + *(int *)0x52a;
      if (*(int *)0x4c6 < 0xfb) {
        if (*(int *)0x4c6 < -0xfa) {
          *(undefined2 *)0x4c6 = 0;
          *(undefined2 *)0x3da = 0;
          *(undefined1 *)0x46a = 1;
          iVar2 = func_0x0008718d();
          if (0 < iVar2) {
            *(undefined1 *)0x4cb = (char)iVar2;
          }
        }
      }
      else {
        *(undefined2 *)0x4c6 = 0;
        *(undefined2 *)0x3da = 0;
        *(undefined1 *)0x46a = 1;
        iVar2 = func_0x0008718d();
        if (0 < iVar2) {
          *(undefined1 *)0x4cb = (char)iVar2;
        }
      }
    }
  }
  else if (param_1 == 4) {
    uVar5 = 1;
    uVar4 = 0x85eb;
    func_0x000866af();
    uVar1 = *(undefined4 *)0x4d2;
    *(int *)(code *)FUN_0008_03de = (int)uVar1;
    *(undefined2 *)0x460 = (int)((ulong)uVar1 >> 0x10);
    uVar1 = *(undefined4 *)0x4d6;
    *(undefined2 *)0x472 = (int)uVar1;
    *(undefined2 *)0x474 = (int)((ulong)uVar1 >> 0x10);
    if (*"" != '\0') {
      if (*(int *)0x3da == 0) {
        uVar4 = 0xf313;
        func_0x000f3ea3(0x85eb,0,0x7d,0x82,(char *)s_Delete_failed_0008_906a + 7,0xa2,
                        (char *)s_Delete_failed_0008_906a + 7);
      }
      else {
        uVar4 = 0x85eb;
        func_0x00087367(0x85eb,uVar5);
      }
      *(undefined1 *)0x468 = 0;
    }
    if (*(char *)0x46a != '\0') {
      func_0x000872a4(uVar4,0xa3,(char *)s_Delete_failed_0008_906a + 7);
      *(undefined1 *)0x46a = 0;
    }
  }
  return;
}



void __stdcall16far FUN_0008_63ea(int param_1)

{
  byte bVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  undefined2 uVar6;
  
  if (param_1 == 1) {
    func_0x00086647();
    *(undefined2 *)0x3dc = 0;
    if (*(byte *)0x4cb < 0x61) {
      *(undefined1 *)0x4cb = 0x61;
    }
    else if (0x7a < *(byte *)0x4cb) {
      *(undefined1 *)0x4cb = 0x7a;
    }
  }
  else if (param_1 == 3) {
    bVar1 = *(byte *)0x4cb;
    if (*(int *)0x52a == 0) {
      if (((*(byte *)0x4e8 & 0x80) != 0) && ((*(byte *)0x4e7 & 0x80) != 0)) {
        uVar4 = (uint)bVar1;
        iVar3 = func_0x00094fda();
        if (iVar3 == 0) {
          *(undefined2 *)0x3da = 0;
        }
        else if (iVar3 == 0x400) {
          *(undefined2 *)0x3da = 8;
        }
        else {
          *(undefined2 *)0x3da = 7;
        }
        uVar4 = func_0x00095067(0x9471,(uint)bVar1,uVar4,iVar3);
        if ((uVar4 & 4) == 0) {
          *(undefined2 *)0x3dc = 2;
        }
        else {
          *(undefined2 *)0x3dc = 1;
        }
        *(undefined1 *)0x468 = 1;
        *(undefined1 *)0x46a = 1;
        *(undefined2 *)0x4c6 = 0;
      }
    }
    else {
      *(int *)0x4c6 = *(int *)0x4c6 + *(int *)0x52a;
      if (*(int *)0x4c6 < 0xfb) {
        if (*(int *)0x4c6 < -0xfa) {
          *(undefined2 *)0x4c6 = 0;
          *(undefined2 *)0x3da = 0;
          *(undefined1 *)0x46a = 1;
          iVar3 = func_0x0008718d();
          if (0 < iVar3) {
            *(undefined1 *)0x4cb = (char)iVar3;
          }
          *(undefined2 *)0x3dc = 0;
        }
      }
      else {
        *(undefined2 *)0x4c6 = 0;
        *(undefined2 *)0x3da = 0;
        *(undefined1 *)0x46a = 1;
        iVar3 = func_0x0008718d();
        if (0 < iVar3) {
          *(undefined1 *)0x4cb = (char)iVar3;
        }
        *(undefined2 *)0x3dc = 0;
      }
    }
  }
  else if (param_1 == 4) {
    uVar6 = 1;
    uVar5 = 0x85eb;
    func_0x000866af();
    uVar2 = *(undefined4 *)0x4d2;
    *(int *)(code *)FUN_0008_03de = (int)uVar2;
    *(undefined2 *)0x460 = (int)((ulong)uVar2 >> 0x10);
    uVar2 = *(undefined4 *)0x4d6;
    *(undefined2 *)0x472 = (int)uVar2;
    *(undefined2 *)0x474 = (int)((ulong)uVar2 >> 0x10);
    if (*"" != '\0') {
      if (*(int *)0x3da == 0) {
        uVar5 = 0xf313;
        func_0x000f3ea3(0x85eb,0,0x7d,0xaa,(char *)s_Delete_failed_0008_906a + 7,0xca,
                        (char *)s_Delete_failed_0008_906a + 7);
      }
      else {
        uVar5 = 0x85eb;
        func_0x00087367(0x85eb,uVar6);
      }
      *(undefined1 *)0x468 = 0;
    }
    if (*(char *)0x46a != '\0') {
      if (*(int *)0x3dc == 1) {
        func_0x000872a4(uVar5,0xd9,(char *)s_Delete_failed_0008_906a + 7);
      }
      else if (*(int *)0x3dc == 2) {
        func_0x000872a4(uVar5,0xe8,(char *)s_Delete_failed_0008_906a + 7);
      }
      else {
        func_0x000872a4(uVar5,0xcb,(char *)s_Delete_failed_0008_906a + 7);
      }
      *(undefined1 *)0x46a = 0;
    }
  }
  return;
}



byte __stdcall16far FUN_0008_65ff(byte param_1,byte param_2)

{
  undefined2 unaff_DS;
  
  if (2 < *(byte *)0x4cd) {
    *(undefined1 *)0x4cd = 0;
  }
  if (((param_1 == 0) || (3 < param_1)) && ((param_1 < 0x61 || (0x7a < param_1)))) {
    param_1 = param_2;
  }
  return param_1;
}



void __cdecl16far FUN_0008_6647(void)

{
  undefined1 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  *(undefined2 *)0x4c6 = 0;
  uVar2 = 1;
  uVar1 = func_0x000865ff();
  *(undefined1 *)0x4ca = uVar1;
  uVar1 = func_0x000865ff(0x85eb,*(undefined1 *)0x4cb,0x61,uVar2);
  *(undefined1 *)0x4cb = uVar1;
  *(undefined2 *)0x4ce = 0;
  *(undefined2 *)0x4d0 = 0;
  *(undefined2 *)0x4d2 = 0;
  *(undefined2 *)0x4d4 = 0;
  *(undefined1 *)0x46a = 1;
  *(undefined1 *)0x468 = 1;
  *(undefined2 *)0x3da = 0;
  return;
}



void __stdcall16far FUN_0008_66af(int param_1)

{
  undefined4 uVar1;
  uint uVar2;
  int iVar3;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined2 uVar4;
  int local_52;
  undefined1 local_50 [6];
  int local_4a;
  undefined1 local_42 [10];
  undefined1 local_38 [26];
  int aiStack_1e [4];
  undefined2 local_16;
  int local_14;
  byte local_12;
  byte local_11;
  undefined4 local_10;
  byte local_b;
  int local_a;
  
  if (*(int *)0x4ce == 0 && *(int *)0x4d0 == 0) {
    uVar1 = *(undefined4 *)(code *)FUN_0008_03de;
    *(undefined2 *)0x4ce = (int)uVar1;
    *(undefined2 *)0x4d0 = (int)((ulong)uVar1 >> 0x10);
    uVar1 = *(undefined4 *)0x472;
    *(undefined2 *)0x4da = (int)uVar1;
    *(int *)(code *)FUN_0008_045c = (int)((ulong)uVar1 >> 0x10);
  }
  else {
    uVar1 = *(undefined4 *)0x4ce;
    *(int *)(code *)FUN_0008_03de = (int)uVar1;
    *(undefined2 *)0x460 = (int)((ulong)uVar1 >> 0x10);
    uVar1 = *(undefined4 *)0x4da;
    *(undefined2 *)0x472 = (int)uVar1;
    *(undefined2 *)0x474 = (int)((ulong)uVar1 >> 0x10);
  }
  if (*"" != '\0') {
    local_10 = *(undefined4 *)(code *)FUN_0008_03de;
    *(undefined2 *)0x470 = 0;
    local_b = '1';
    uVar4 = 0;
    func_0x000f6510();
    for (local_a = 1; local_a < 4; local_a = local_a + 1) {
      *(int *)0x46e = local_a * 9 + 8;
      func_0x000f6510(0xf651,local_b,uVar4);
      local_b = local_b + '\x01';
    }
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)(code *)FUN_0008_03de + -1) = 10;
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)(code *)FUN_0008_03de + -2) = 10;
    uVar4 = 0xf408;
    func_0x000f44c8(0xf651,0x14,0x2a3,(int)local_10,(int)((ulong)local_10 >> 0x10));
    *(int *)0x472 = *(int *)0x472 + 9;
    local_10._2_2_ = (undefined2)((ulong)*(undefined4 *)(code *)FUN_0008_03de >> 0x10);
    local_10._0_2_ = (undefined2)*(undefined4 *)(code *)FUN_0008_03de;
    *(undefined2 *)0x470 = 0;
    local_b = 0x41;
    for (local_a = 0; local_a < 0x1a; local_a = local_a + 1) {
      *(int *)0x46e = local_a * 9 + 8;
      func_0x000f6510(uVar4,0);
      uVar2 = func_0x00095067(0xf651,local_b | 0x20);
      if ((uVar2 & 0x20) == 0) {
        func_0x000f6510(0x9471,local_b);
      }
      else {
        func_0x000f660c(0x9471,0x3f);
      }
      uVar4 = 0xf651;
      local_b = local_b + 1;
    }
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)(code *)FUN_0008_03de + -1) = 10;
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)(code *)FUN_0008_03de + -2) = 10;
    func_0x000f44c8(uVar4,0x14,0x177,(undefined2)local_10,local_10._2_2_);
    *(int *)0x472 = *(int *)0x472 + 9;
    *(undefined2 *)0x470 = 0;
    local_10 = *(undefined4 *)(code *)FUN_0008_03de;
    func_0x000f6510(0xf408,0);
    for (local_52 = 1; local_52 < 4; local_52 = local_52 + 1) {
      iVar3 = local_52 * 2;
      aiStack_1e[local_52] = 0;
      if ((((undefined *)&DAT_0008_1b62)[iVar3] & 2) != 0) {
        aiStack_1e[local_52] = aiStack_1e[local_52] + *(int *)(local_52 * 8 + 0x3e);
      }
      if ((((undefined *)&DAT_0008_1b63)[iVar3] & 2) != 0) {
        aiStack_1e[local_52] = aiStack_1e[local_52] + *(int *)((iVar3 + 1) * 4 + 0x3e);
      }
      aiStack_1e[local_52] = (aiStack_1e[local_52] >> 10) + 0x30;
      *(int *)0x46e = local_52 * 9 + 8;
      func_0x000f6510(0xf651,(char)aiStack_1e[local_52]);
    }
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)(code *)FUN_0008_03de + -1) = 10;
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)(code *)FUN_0008_03de + -2) = 10;
    func_0x000f44c8(0xf651,0x14,0x271,(int)local_10,(int)((ulong)local_10 >> 0x10));
    *(int *)0x472 = *(int *)0x472 + 9;
    func_0x000960ab(0xf408,local_38);
    local_10 = *(undefined4 *)(code *)FUN_0008_03de;
    func_0x000f6510(0x9471,0);
    for (local_a = 0; local_a < 0x1a; local_a = local_a + 1) {
      *(int *)0x46e = local_a * 9 + 8;
      uVar2 = func_0x00095067(0xf651,local_a + 0x61);
      if ((uVar2 & 4) == 0) {
        func_0x000f6510(0x9471,local_38[local_a]);
      }
      else {
        func_0x000f660c(0x9471,local_38[local_a]);
      }
    }
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)(code *)FUN_0008_03de + -1) = 10;
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)(code *)FUN_0008_03de + -2) = 10;
    func_0x000f44c8(0xf651,0x14,0x145,(int)local_10,(int)((ulong)local_10 >> 0x10));
    *(int *)0x472 = *(int *)0x472 + 9;
    func_0x000961af(0xf408,local_50);
    func_0x000f5d89(0x9471,local_4a >> 10,local_42);
    unaff_CS = 0xf313;
    func_0x000f3ea3(0xf59b,0x200,0x1db,0xf9,(char *)s_Delete_failed_0008_906a + 7,local_42);
    uVar1 = *(undefined4 *)(code *)FUN_0008_03de;
    *(undefined2 *)0x4d2 = (int)uVar1;
    *(undefined2 *)0x4d4 = (int)((ulong)uVar1 >> 0x10);
    uVar1 = *(undefined4 *)0x472;
    *(undefined2 *)0x4d6 = (int)uVar1;
    *(undefined2 *)0x4d8 = (int)((ulong)uVar1 >> 0x10);
  }
  if (*(char *)0x46a != '\0') {
    local_11 = *(byte *)0x4ca;
    local_12 = *(byte *)0x4cb;
    if (param_1 == 2) {
      if ((local_11 == 0) || (3 < local_11)) {
        iVar3 = local_11 - 0x60;
        local_16 = 0x131;
      }
      else {
        iVar3 = local_11 + 1;
        local_16 = 0x25d;
      }
      local_14 = iVar3 * 0x24 + 8;
      func_0x000f63bc(unaff_CS,(int)*(undefined4 *)0x1d08,
                      (int)((ulong)*(undefined4 *)0x1d08 >> 0x10),local_14,local_16,
                      (int)*(undefined4 *)0x1cc4 + 0x11ee,
                      (int)((ulong)*(undefined4 *)0x1cc4 >> 0x10),(int)*(undefined4 *)0x1ddc + 0x291
                      ,(int)((ulong)*(undefined4 *)0x1ddc >> 0x10),*(char *)0x4cd == '\0');
      unaff_CS = 0xf638;
    }
    else {
      *(undefined1 *)((int)*(undefined4 *)0x1ddc + 0x291) = 0;
    }
    if ((local_12 == 0) || (3 < local_12)) {
      iVar3 = local_12 - 0x60;
      local_16 = 0x131;
    }
    else {
      iVar3 = local_12 + 1;
      local_16 = 0x25d;
    }
    local_14 = iVar3 * 0x24 + 8;
    if ((*(char *)0x4cd == '\x01') || (param_1 != 2)) {
      uVar4 = 1;
    }
    else {
      uVar4 = 0;
    }
    func_0x000f63bc(unaff_CS,(int)*(undefined4 *)0x1d04,(int)((ulong)*(undefined4 *)0x1d04 >> 0x10),
                    local_14,local_16,(int)*(undefined4 *)0x1cc4 + 0x12f2,
                    (int)((ulong)*(undefined4 *)0x1cc4 >> 0x10),(int)*(undefined4 *)0x1ddc + 0x29a,
                    (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),uVar4);
  }
  return;
}



undefined2 __stdcall16far FUN_0008_6b4b(byte param_1,undefined4 param_2)

{
  undefined4 uVar1;
  byte *pbVar2;
  undefined2 uVar3;
  byte *pbVar4;
  byte *pbVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined2 uVar10;
  undefined2 uVar11;
  undefined2 uVar12;
  undefined2 unaff_DS;
  undefined2 uVar13;
  undefined4 local_18;
  int local_14;
  int local_12;
  undefined2 local_10;
  int local_e;
  
  uVar13 = 0x41;
  uVar12 = 38000;
  uVar3 = func_0x0009470e();
  uVar6 = (uint)param_1;
  iVar7 = uVar6 * 2;
  iVar8 = iVar7 + 1;
  if (((((undefined *)&DAT_0008_1b62)[iVar7] & 2) == 0) &&
     ((((undefined *)&DAT_0008_1b63)[iVar7] & 2) == 0)) {
    local_10 = 1;
  }
  else {
    uVar10 = (undefined2)((ulong)param_2 >> 0x10);
    iVar9 = (int)param_2;
    func_0x000fbc09(38000,(uint)param_1 * 0x10 + 0x18c);
    *(undefined1 *)(iVar9 + 0x10) = *(undefined1 *)(iVar7 + 0x1dc);
    *(undefined1 *)(iVar9 + 0x11) = *(undefined1 *)(iVar7 + 0x1dd);
    func_0x000fbc09(0xfbbe,uVar6 * 0x14 + 0x1c14);
    uVar12 = 0xfbbe;
    func_0x000fbc09(0xfbbe,iVar8 * 10 + 0x1c14);
    *(undefined1 *)(iVar9 + 0x26) = ((undefined *)&DAT_0008_1b62)[iVar7];
    *(undefined1 *)(iVar9 + 0x27) = ((undefined *)&DAT_0008_1b63)[iVar7];
    *(undefined2 *)(iVar9 + 0x28) = *(undefined2 *)(uVar6 * 0x1c + 0x576);
    *(undefined2 *)(iVar9 + 0x2a) = *(undefined2 *)(iVar8 * 0xe + 0x576);
    *(undefined2 *)(iVar9 + 0x2c) = *(undefined2 *)(uVar6 * 0x1c + 0x578);
    *(undefined2 *)(iVar9 + 0x2e) = *(undefined2 *)(iVar8 * 0xe + 0x578);
    *(undefined1 *)(iVar9 + 0x30) = *(undefined1 *)(uVar6 * 0x1c + 0x580);
    *(undefined1 *)(iVar9 + 0x31) = *(undefined1 *)(iVar8 * 0xe + 0x580);
    local_e = iVar8;
    if ((((undefined *)&DAT_0008_1b62)[iVar7] & 2) != 0) {
      local_e = iVar7;
    }
    uVar11 = (undefined2)((ulong)*(undefined4 *)0x1c84 >> 0x10);
    pbVar4 = (byte *)((int)*(undefined4 *)0x1c84 + *(int *)(local_e * 4 + 0x3c));
    local_18 = (byte *)CONCAT22(uVar11,pbVar4);
    pbVar5 = pbVar4 + *(int *)(local_e * 4 + 0x3e);
    local_12 = 0;
    local_14 = 0;
    if ((*(byte *)((uint)param_1 * 0x10 + 0x18c) & 0x30) == 0x20) {
      while (pbVar2 = local_18, (byte *)local_18 <= pbVar5) {
        local_18 = (byte *)CONCAT22(local_18._2_2_,(byte *)local_18 + 1);
        if ((*pbVar2 & 3) == 3) break;
        local_12 = local_12 + 1;
      }
      do {
        pbVar2 = local_18;
        if (pbVar5 < (byte *)local_18) goto LAB_0008_6da9;
        local_18 = (byte *)CONCAT22(local_18._2_2_,(byte *)local_18 + 1);
        if ((*pbVar2 & 3) == 1) goto LAB_0008_6da9;
        local_14 = local_14 + 1;
      } while( true );
    }
    local_18 = (byte *)CONCAT22(uVar11,pbVar4 + 1);
    while (((byte *)local_18 <= pbVar5 && ((*local_18 & 3) != 3))) {
      local_12 = local_12 + 2;
      local_18 = (byte *)CONCAT22(local_18._2_2_,(byte *)local_18 + 2);
    }
    while (((byte *)local_18 <= pbVar5 && ((*local_18 & 3) != 1))) {
      local_14 = local_14 + 2;
      local_18 = (byte *)CONCAT22(local_18._2_2_,(byte *)local_18 + 2);
    }
LAB_0008_6da9:
    if (local_12 < *(int *)(local_e * 4 + 0x3e)) {
      *(int *)(iVar9 + 0x32) = local_12;
      *(int *)(iVar9 + 0x34) = local_14;
    }
    else {
      *(undefined2 *)(iVar9 + 0x32) = 0;
      *(undefined2 *)(iVar9 + 0x34) = 0;
    }
    *(undefined1 *)(iVar9 + 0x42) = *(undefined1 *)(uVar6 * 0x28 + 0xb0);
    *(undefined1 *)(iVar9 + 0x43) = *(undefined1 *)(uVar6 * 0x28 + 0xb4);
    *(undefined1 *)(iVar9 + 0x44) = *(undefined1 *)(uVar6 * 0x28 + 0xb3);
    *(undefined1 *)(iVar9 + 0x45) = *(undefined1 *)(uVar6 * 0x28 + 0xb2);
    *(undefined1 *)(iVar9 + 0x46) = *(undefined1 *)(uVar6 * 0x28 + 0xb1);
    *(undefined1 *)(iVar9 + 0x47) = *(undefined1 *)(iVar8 * 0x14 + 0xb0);
    *(undefined1 *)(iVar9 + 0x4b) = *(undefined1 *)(iVar8 * 0x14 + 0xb1);
    *(undefined1 *)(iVar9 + 0x4a) = *(undefined1 *)(iVar8 * 0x14 + 0xb2);
    *(undefined1 *)(iVar9 + 0x49) = *(undefined1 *)(iVar8 * 0x14 + 0xb3);
    *(undefined1 *)(iVar9 + 0x48) = *(undefined1 *)(iVar8 * 0x14 + 0xb4);
    if ((((undefined *)&DAT_0008_1b62)[iVar7] & 2) == 0) {
      *(undefined2 *)(iVar9 + 0x36) = 0;
      *(undefined2 *)(iVar9 + 0x38) = 0;
      *(undefined2 *)(iVar9 + 0x3a) = 0;
    }
    else {
      uVar1 = *(undefined4 *)0x1c80;
      *(int *)(iVar9 + 0x36) = (int)uVar1 + *(int *)(uVar6 * 8 + 0x3c);
      *(undefined2 *)(iVar9 + 0x38) = (int)((ulong)uVar1 >> 0x10);
      *(undefined2 *)(iVar9 + 0x3a) = *(undefined2 *)(uVar6 * 8 + 0x3e);
    }
    if ((((undefined *)&DAT_0008_1b63)[iVar7] & 2) == 0) {
      *(undefined2 *)(iVar9 + 0x3c) = 0;
      *(undefined2 *)(iVar9 + 0x3e) = 0;
      *(undefined2 *)(iVar9 + 0x40) = 0;
    }
    else {
      uVar1 = *(undefined4 *)0x1c80;
      *(int *)(iVar9 + 0x3c) = (int)uVar1 + *(int *)(iVar8 * 4 + 0x3c);
      *(undefined2 *)(iVar9 + 0x3e) = (int)((ulong)uVar1 >> 0x10);
      *(undefined2 *)(iVar9 + 0x40) = *(undefined2 *)(iVar8 * 4 + 0x3e);
    }
    local_10 = 0;
  }
  func_0x0009470e(uVar12,uVar3,uVar13);
  return local_10;
}



undefined2 __stdcall16far FUN_0008_6f6f(byte param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  
  uVar1 = (uint)param_1;
  iVar2 = uVar1 * 2;
  iVar3 = iVar2 + 1;
  uVar5 = (undefined2)((ulong)param_2 >> 0x10);
  iVar4 = (int)param_2;
  func_0x000fbc09();
  *(undefined1 *)(iVar2 + 0x1dc) = *(undefined1 *)(iVar4 + 0x10);
  *(undefined1 *)(iVar2 + 0x1dd) = *(undefined1 *)(iVar4 + 0x11);
  func_0x000fbc09(0xfbbe,iVar4 + 0x12,uVar5,uVar1 * 0x14 + 0x1c14);
  func_0x000fbc09(0xfbbe,iVar4 + 0x1c,uVar5,iVar3 * 10 + 0x1c14);
  ((undefined *)&DAT_0008_1b62)[iVar2] = *(byte *)(iVar4 + 0x26) & 2;
  ((undefined *)&DAT_0008_1b63)[iVar2] = *(byte *)(iVar4 + 0x27) & 2;
  *(undefined2 *)(uVar1 * 0x1c + 0x574) = *(undefined2 *)(iVar4 + 0x28);
  *(undefined2 *)(iVar3 * 0xe + 0x576) = *(undefined2 *)(iVar4 + 0x2a);
  *(undefined2 *)(uVar1 * 0x1c + 0x578) = *(undefined2 *)(iVar4 + 0x2c);
  *(undefined2 *)(iVar3 * 0xe + 0x578) = *(undefined2 *)(iVar4 + 0x2e);
  *(undefined1 *)(uVar1 * 0x1c + 0x580) = *(undefined1 *)(iVar4 + 0x30);
  *(undefined1 *)(iVar3 * 0xe + 0x580) = *(undefined1 *)(iVar4 + 0x31);
  *(undefined1 *)(param_1 + 0x564) = *(undefined1 *)(iVar4 + 0x30);
  *(undefined2 *)(uVar1 * 8 + 0x3e) = *(undefined2 *)(iVar4 + 0x3a);
  *(undefined2 *)(iVar3 * 4 + 0x3e) = *(undefined2 *)(iVar4 + 0x40);
  iVar2 = uVar1 * 0x28;
  *(undefined1 *)(iVar2 + 0xb0) = *(undefined1 *)(iVar4 + 0x42);
  *(undefined1 *)(iVar2 + 0xb4) = *(undefined1 *)(iVar4 + 0x43);
  *(undefined1 *)(iVar2 + 0xb3) = *(undefined1 *)(iVar4 + 0x44);
  *(undefined1 *)(iVar2 + 0xb2) = *(undefined1 *)(iVar4 + 0x45);
  *(undefined1 *)(iVar2 + 0xb1) = *(undefined1 *)(iVar4 + 0x46);
  iVar3 = iVar3 * 0x14;
  *(undefined1 *)(iVar3 + 0xb0) = *(undefined1 *)(iVar4 + 0x47);
  *(undefined1 *)(iVar3 + 0xb1) = *(undefined1 *)(iVar4 + 0x4b);
  *(undefined1 *)(iVar3 + 0xb2) = *(undefined1 *)(iVar4 + 0x4a);
  *(undefined1 *)(iVar3 + 0xb3) = *(undefined1 *)(iVar4 + 0x49);
  *(undefined1 *)(iVar3 + 0xb4) = *(undefined1 *)(iVar4 + 0x48);
  return 0;
}



uint __stdcall16far FUN_0008_718d(int param_1,int param_2,int param_3)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  undefined2 unaff_CS;
  undefined2 uVar5;
  undefined2 unaff_DS;
  int iVar6;
  uint local_a;
  
  uVar3 = (uint)*(byte *)(param_1 + 0x4ca);
  uVar4 = (uint)(param_1 == 0);
  iVar6 = 0;
  local_a = uVar3;
  do {
    bVar1 = false;
    iVar6 = iVar6 + 1;
    local_a = local_a + param_2;
    if (param_3 == 0) {
      if ((int)local_a < 0x61) {
        local_a = 0x7a;
      }
      else if (0x7a < (int)local_a) {
        local_a = 0x61;
      }
    }
    else if (((int)local_a < 0x61) && (4 < (int)local_a)) {
      local_a = 3;
    }
    else if (local_a == 0) {
      local_a = 0x7a;
    }
    else if (local_a == 4) {
      local_a = 0x61;
    }
    else if (0x7a < (int)local_a) {
      local_a = 1;
    }
    uVar5 = unaff_CS;
    if (param_1 == 0) {
      if ((int)local_a < 4) {
        if (((((undefined *)&DAT_0008_1b62)[local_a * 2] & 2) == 0) &&
           ((((undefined *)&DAT_0008_1b63)[local_a * 2] & 2) == 0)) {
          bVar1 = true;
        }
      }
      else {
        uVar5 = 0x9471;
        uVar2 = func_0x00095067(unaff_CS,local_a,iVar6,uVar4);
        if ((uVar2 & 1) != 0) {
          bVar1 = true;
        }
      }
    }
  } while (((bVar1) && (local_a != uVar3)) && (unaff_CS = uVar5, iVar6 < 0x1e));
  if (local_a == uVar3) {
    local_a = 0;
  }
  return local_a;
}



void __stdcall16far FUN_0008_72a4(char *param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  
  uVar1 = *(undefined4 *)(code *)FUN_0008_03de;
  uVar2 = *(undefined4 *)0x472;
  *(undefined2 *)0x46e = 0;
  *(undefined2 *)0x470 = 0;
  uVar3 = *(undefined4 *)0x1cc4;
  *(int *)(code *)FUN_0008_03de = (int)uVar3 + 0xe74;
  *(undefined2 *)0x460 = (int)((ulong)uVar3 >> 0x10);
  uVar5 = 0;
  iVar4 = func_0x000f6510();
  *(int *)0x46e = *(int *)0x46e + iVar4;
  while (*param_1 != '\0') {
    iVar4 = func_0x000f660c(0xf651,*param_1,uVar5);
    *(int *)0x46e = *(int *)0x46e + iVar4;
    param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
  }
  *(undefined1 *)((int)*(undefined4 *)(code *)FUN_0008_03de + *(int *)0x1c02 + -1) = 10;
  *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)(code *)FUN_0008_03de + -2) = 10;
  uVar3 = *(undefined4 *)0x1ddc;
  *(int *)0x472 = (int)uVar3 + 0x288;
  *(undefined2 *)0x474 = (int)((ulong)uVar3 >> 0x10);
  func_0x000f44c8(0xf651,0x200,0x20d,(int)*(undefined4 *)0x1cc4 + 0xe74,
                  (int)((ulong)*(undefined4 *)0x1cc4 >> 0x10));
  *(int *)(code *)FUN_0008_03de = (int)uVar1;
  *(undefined2 *)0x460 = (int)((ulong)uVar1 >> 0x10);
  *(undefined2 *)0x472 = (int)uVar2;
  *(undefined2 *)0x474 = (int)((ulong)uVar2 >> 0x10);
  return;
}



void __cdecl16far FUN_0008_7367(void)

{
  int iVar1;
  undefined2 unaff_DS;
  
  iVar1 = *(int *)0x3da;
  if (iVar1 == 1) {
    func_0x000f3ea3();
  }
  else if (iVar1 == 2) {
    func_0x000f3ea3();
  }
  else if (iVar1 == 3) {
    func_0x000f3ea3();
  }
  else if (iVar1 == 4) {
    func_0x000f3ea3();
  }
  else if (iVar1 == 5) {
    func_0x000f3ea3();
  }
  else if (iVar1 == 6) {
    func_0x000f3ea3();
  }
  else if (iVar1 == 7) {
    func_0x000f3ea3();
  }
  else if (iVar1 == 8) {
    func_0x000f3ea3();
  }
  else if (iVar1 == 9) {
    func_0x000f3ea3();
  }
  else if (iVar1 == 10) {
    func_0x000f3ea3();
  }
  else if (iVar1 == 0xb) {
    func_0x000f3ea3();
  }
  else if (iVar1 == 0xc) {
    func_0x000f3ea3();
  }
  else {
    func_0x000f3ea3();
  }
  return;
}



void __stdcall16far FUN_0008_7594(uint param_1)

{
  undefined2 unaff_DS;
  
  if (param_1 == 0) {
    *(undefined2 *)0x3da = 0;
  }
  else if ((param_1 & 0x40) == 0) {
    if ((param_1 & 1) == 0) {
      if ((param_1 & 4) == 0) {
        if ((param_1 & 2) == 0) {
          *(undefined2 *)0x3da = 3;
        }
        else {
          *(undefined2 *)0x3da = 0xc;
        }
      }
      else {
        *(undefined2 *)0x3da = 10;
      }
    }
    else {
      *(undefined2 *)0x3da = 9;
    }
  }
  else {
    *(undefined2 *)0x3da = 0xb;
  }
  return;
}



void __stdcall16far FUN_0008_7609(byte param_1,byte param_2,undefined2 *param_3)

{
  uint *puVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  int iVar4;
  undefined1 *puVar5;
  undefined1 *in_BX;
  undefined1 *puVar6;
  int iVar7;
  undefined1 *puVar8;
  int unaff_ES;
  undefined2 uVar9;
  undefined2 uVar10;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined2 uVar11;
  undefined2 local_8e;
  undefined2 local_8c;
  int local_8a;
  int local_88;
  int local_86;
  int local_84;
  undefined1 *local_82;
  undefined2 local_80;
  undefined1 *local_7e;
  undefined2 uStack_7c;
  undefined4 local_7a;
  undefined4 local_76;
  undefined4 local_72;
  undefined1 *local_6e;
  int local_6a;
  int local_68;
  undefined2 local_66;
  int local_64;
  undefined1 *local_62;
  int iStack_60;
  undefined1 *local_5e;
  undefined1 local_5a [78];
  int local_c;
  int local_a;
  
  uVar11 = 0x41;
  local_8c = func_0x0009470e();
  local_8a = 0;
  uVar10 = 38000;
  do {
    uVar9 = uVar10;
    if (0 < local_8a) {
LAB_0008_7d4c:
      uVar10 = uVar9;
      if (((0x60 < param_1) && (param_1 < 0x7b)) && (local_86 == 1)) {
        uVar10 = 0x9471;
        func_0x00094d9c(uVar9,param_1);
      }
      uVar11 = uVar10;
      if (((0x60 < param_2) && (param_1 < 0x7b)) && (local_88 == 1)) {
        uVar11 = 0x9471;
        func_0x00094d9c(uVar10,param_2);
      }
      func_0x0009470e(uVar11,local_8c);
      return;
    }
    if (param_1 == param_2) {
      *param_3 = 4;
      goto LAB_0008_7d4c;
    }
    local_88 = 0;
    local_86 = 0;
    local_a = (uint)param_1 * 2;
    local_c = local_a + 1;
    if ((param_1 == 0) || (3 < param_1)) {
      uVar9 = 0x9471;
      func_0x0009471c(uVar10,param_1,&local_64);
      _local_5e = (undefined1 *)CONCAT22(unaff_ES,in_BX);
      if (unaff_ES == 0 && in_BX == (undefined1 *)0x0) {
        *param_3 = 1;
        goto LAB_0008_7d4c;
      }
      local_86 = 1;
      uVar10 = uVar9;
    }
    else {
      in_BX = local_5a;
      _local_5e = (undefined1 *)CONCAT22(unaff_SS,in_BX);
      local_64 = func_0x00086b4b(uVar10,param_1,in_BX,unaff_SS,uVar11);
      uVar10 = 0x85eb;
      if (local_64 != 0) {
        *param_3 = 1;
        uVar9 = 0x85eb;
        goto LAB_0008_7d4c;
      }
    }
    local_a = (uint)param_2 * 2;
    local_c = local_a + 1;
    puVar5 = (undefined1 *)_local_5e;
    iVar4 = (int)((ulong)_local_5e >> 0x10);
    if ((param_2 == 0) || (3 < param_2)) {
      if ((param_1 < 0x61) || (0x7a < param_1)) {
        uVar9 = 0x9471;
        func_0x0009485b(uVar10,param_2,puVar5,iVar4,&local_66);
        local_7a = (undefined1 *)CONCAT22(iVar4,in_BX);
        if (iVar4 == 0 && in_BX == (undefined1 *)0x0) {
          unaff_ES = (int)((ulong)param_3 >> 0x10);
          *param_3 = 3;
          uVar9 = 0x85eb;
          func_0x00087594(0x9471,local_66);
          *(undefined1 *)0x468 = 1;
        }
        else {
          local_88 = 1;
          unaff_ES = (int)((ulong)_local_5e >> 0x10);
          puVar5 = (undefined1 *)_local_5e;
          local_6e = *(undefined1 **)(puVar5 + 0x36);
          local_72 = (undefined1 *)CONCAT22(iVar4,in_BX);
          for (local_84 = 0; puVar3 = local_6e, puVar2 = local_72,
              local_84 < *(int *)(puVar5 + 0x3a); local_84 = local_84 + 1) {
            local_72 = (undefined1 *)CONCAT22(local_72._2_2_,(undefined1 *)local_72 + 1);
            local_6e = (undefined1 *)CONCAT22(local_6e._2_2_,(undefined1 *)local_6e + 1);
            in_BX = (undefined1 *)CONCAT11((char)((uint)in_BX >> 8),*puVar3);
            *puVar2 = *puVar3;
          }
          local_6e = *(undefined1 **)(puVar5 + 0x3c);
          for (local_84 = 0; puVar3 = local_6e, puVar2 = local_72,
              local_84 < *(int *)(puVar5 + 0x40); local_84 = local_84 + 1) {
            local_72 = (undefined1 *)CONCAT22(local_72._2_2_,(undefined1 *)local_72 + 1);
            local_6e = (undefined1 *)CONCAT22(local_6e._2_2_,(undefined1 *)local_6e + 1);
            in_BX = (undefined1 *)CONCAT11((char)((uint)in_BX >> 8),*puVar3);
            *puVar2 = *puVar3;
          }
        }
      }
      else {
        func_0x00094d9c(uVar10,param_1);
        iVar4 = (int)((ulong)_local_5e >> 0x10);
        func_0x0009485b(0x9471,param_2,(undefined1 *)_local_5e,iVar4,&local_66);
        local_7a = (undefined1 *)CONCAT22(iVar4,in_BX);
        if (iVar4 == 0 && in_BX == (undefined1 *)0x0) {
          *param_3 = 3;
          uVar9 = 0x85eb;
          func_0x00087594(0x9471,local_66);
          goto LAB_0008_7d4c;
        }
        local_88 = 1;
        uVar9 = 0x9471;
        func_0x0009471c(0x9471,param_1,&local_64);
        _local_62 = (undefined1 *)CONCAT22(iVar4,in_BX);
        if (iVar4 == 0 && in_BX == (undefined1 *)0x0) {
          local_86 = 0;
        }
        unaff_ES = (int)((ulong)_local_5e >> 0x10);
        puVar5 = (undefined1 *)_local_5e;
        if (*(int *)(puVar5 + 0x3a) + *(int *)(puVar5 + 0x40) !=
            *(int *)(in_BX + 0x3a) + *(int *)(in_BX + 0x40)) {
          *param_3 = 2;
          goto LAB_0008_7d4c;
        }
        local_6e = *(undefined1 **)(puVar5 + 0x36);
        local_72 = local_7a;
        for (local_84 = 0; puVar3 = local_6e, puVar2 = local_72, local_84 < *(int *)(puVar5 + 0x3a);
            local_84 = local_84 + 1) {
          local_72 = (undefined1 *)CONCAT22(local_72._2_2_,(undefined1 *)local_72 + 1);
          local_6e = (undefined1 *)CONCAT22(local_6e._2_2_,(undefined1 *)local_6e + 1);
          in_BX = (undefined1 *)CONCAT11((char)((uint)in_BX >> 8),*puVar3);
          *puVar2 = *puVar3;
        }
        local_6e = *(undefined1 **)(puVar5 + 0x3c);
        for (local_84 = 0; puVar3 = local_6e, puVar2 = local_72, local_84 < *(int *)(puVar5 + 0x40);
            local_84 = local_84 + 1) {
          local_72 = (undefined1 *)CONCAT22(local_72._2_2_,(undefined1 *)local_72 + 1);
          local_6e = (undefined1 *)CONCAT22(local_6e._2_2_,(undefined1 *)local_6e + 1);
          in_BX = (undefined1 *)CONCAT11((char)((uint)in_BX >> 8),*puVar3);
          *puVar2 = *puVar3;
        }
      }
    }
    else {
      local_6a = *(int *)(puVar5 + 0x3a) + *(int *)(puVar5 + 0x40);
      if ((0x800 < local_6a) && (1 < param_2)) {
        *param_3 = 6;
        uVar9 = uVar10;
        goto LAB_0008_7d4c;
      }
      if ((param_2 == 2) || (param_2 == 3)) {
        local_68 = 0;
        if ((*(byte *)0x1be4 & 2) != 0) {
          local_68 = *(int *)0x46;
        }
        if ((*(byte *)0x1be5 & 2) != 0) {
          local_68 = local_68 + *(int *)0x4a;
        }
        if (0xffe < local_68) {
          func_0x000f8f24(uVar10,1);
          uVar10 = 0xf8e6;
        }
      }
      else if ((param_2 == 1) && (0xffe < local_6a)) {
        func_0x000f8f24(uVar10,2);
        uVar10 = 0xf8e6;
        func_0x000f8f24(0xf8e6,3);
      }
      func_0x000f0414(uVar10,param_2);
      uVar10 = 0x97c6;
      func_0x00097c95(0xf008,1);
      while (iVar4 = func_0x00097c6e(uVar10,1), iVar4 != 0) {
        uVar10 = 0xe693;
        func_0x000e693c(0x97c6);
      }
      uVar10 = (undefined2)((ulong)_local_5e >> 0x10);
      local_6e = *(undefined1 **)((undefined1 *)_local_5e + 0x36);
      local_72._2_2_ = (undefined2)((ulong)*(undefined4 *)0x1c80 >> 0x10);
      local_72._0_2_ =
           (undefined1 *)
           ((int)*(undefined4 *)0x1c80 + *(int *)(local_a * 8 + (int)*(undefined4 *)0x1c94));
      *(undefined2 *)(local_a * 4 + 0x3c) =
           *(undefined2 *)(local_a * 8 + (int)*(undefined4 *)0x1c94);
      local_6a = *(int *)((undefined1 *)_local_5e + 0x3a);
      *(int *)(local_a * 4 + 0x3e) = local_6a;
      func_0x000fbc09(0x97c6,(int)local_6e,(int)((ulong)local_6e >> 0x10),(undefined1 *)local_72,
                      local_72._2_2_,local_6a);
      uVar10 = (undefined2)((ulong)_local_5e >> 0x10);
      puVar6 = (undefined1 *)_local_5e;
      puVar5 = (undefined1 *)
               ((int)(undefined1 *)local_72 + *(int *)(puVar6 + 0x32) + *(int *)0x1c02);
      _local_7e = (undefined1 *)CONCAT22(local_72._2_2_,puVar5);
      local_82 = puVar5 + *(int *)(puVar6 + 0x34);
      local_80 = local_72._2_2_;
      if (*(int *)(puVar6 + 0x34) < 1) {
        for (local_76 = (undefined1 *)
                        CONCAT22(local_72._2_2_,
                                 (undefined1 *)((int)(undefined1 *)local_72 + *(int *)0x1c02));
            (undefined1 *)local_76 <
            (undefined1 *)((int)(undefined1 *)local_72 + *(int *)0x1c02 + local_6a);
            local_76 = (undefined1 *)CONCAT22(local_76._2_2_,(undefined1 *)local_76 + 1)) {
          *local_76 = 9;
        }
      }
      else {
        local_76 = (undefined1 *)
                   CONCAT22(local_72._2_2_,
                            (undefined1 *)((int)(undefined1 *)local_72 + *(int *)0x1c02));
        while (puVar6 = (undefined1 *)local_76,
              puVar6 < (undefined1 *)((int)(undefined1 *)local_72 + *(int *)0x1c02 + local_6a)) {
          if ((puVar6 < puVar5) || (local_82 < puVar6)) {
            *local_76 = 9;
          }
          else {
            *local_76 = 0xb;
          }
          local_76 = (undefined1 *)CONCAT22(local_76._2_2_,puVar6 + 1);
        }
      }
      local_76 = (undefined1 *)
                 CONCAT22(local_72._2_2_,
                          (undefined1 *)((int)(undefined1 *)local_72 + *(int *)0x1c02));
      puVar2 = local_76;
      local_76 = (undefined1 *)
                 CONCAT22(local_72._2_2_,
                          (undefined1 *)((int)(undefined1 *)local_72 + *(int *)0x1c02) + 1);
      *puVar2 = 0xb;
      *local_76 = 0xb;
      local_8e = func_0x000fbc2f(0xfbbe,(undefined1 *)local_72,local_72._2_2_);
      iVar7 = local_a * 0x14 + 0xb5;
      func_0x000fbc4d(0xfbc2,&local_8e);
      iVar4 = local_a * 0x14;
      *(undefined1 *)(iVar4 + 0xb7) = *(undefined1 *)(iVar4 + 0xb5);
      *(undefined1 *)(iVar4 + 0xb8) = *(undefined1 *)(iVar4 + 0xb6);
      uVar10 = (undefined2)((ulong)_local_5e >> 0x10);
      puVar5 = (undefined1 *)_local_5e;
      local_6e = *(undefined1 **)(puVar5 + 0x3c);
      uVar9 = (undefined2)((ulong)*(undefined4 *)0x1c80 >> 0x10);
      puVar6 = (undefined1 *)
               ((int)*(undefined4 *)0x1c80 +
               *(int *)((int)*(undefined4 *)0x1c94 + local_c * 8) + *(int *)(puVar5 + 0x3a));
      local_72 = (undefined1 *)CONCAT22(uVar9,puVar6);
      *(int *)(local_c * 4 + 0x3c) =
           *(int *)(local_c * 8 + (int)*(undefined4 *)0x1c94) + *(int *)(puVar5 + 0x3a);
      local_6a = *(int *)(puVar5 + 0x40);
      *(int *)(local_c * 4 + 0x3e) = local_6a;
      func_0x000fbc09(0xfbc2,(undefined1 *)local_6e,(int)((ulong)local_6e >> 0x10),puVar6,uVar9,
                      local_6a,uVar10,iVar7);
      local_80 = (undefined2)((ulong)local_72 >> 0x10);
      puVar8 = (undefined1 *)local_72;
      uVar10 = (undefined2)((ulong)_local_5e >> 0x10);
      puVar5 = (undefined1 *)_local_5e;
      puVar6 = puVar8 + *(int *)(puVar5 + 0x32) + *(int *)0x1c02;
      _local_7e = (undefined1 *)CONCAT22(local_80,puVar6);
      local_82 = puVar6 + *(int *)(puVar5 + 0x34);
      if (*(int *)(puVar5 + 0x34) < 1) {
        for (local_76 = (undefined1 *)CONCAT22(local_80,puVar8 + *(int *)0x1c02);
            (undefined1 *)local_76 < puVar8 + *(int *)0x1c02 + local_6a;
            local_76 = (undefined1 *)CONCAT22(local_76._2_2_,(undefined1 *)local_76 + 1)) {
          *local_76 = 9;
        }
      }
      else {
        local_76 = (undefined1 *)CONCAT22(local_80,puVar8 + *(int *)0x1c02);
        while (puVar5 = (undefined1 *)local_76, puVar5 < puVar8 + *(int *)0x1c02 + local_6a) {
          if ((puVar5 < puVar6) || (local_82 < puVar5)) {
            *local_76 = 9;
          }
          else {
            *local_76 = 0xb;
          }
          local_76 = (undefined1 *)CONCAT22(local_76._2_2_,puVar5 + 1);
        }
      }
      local_76 = (undefined1 *)CONCAT22(local_80,puVar8 + *(int *)0x1c02);
      puVar2 = local_76;
      local_76 = (undefined1 *)CONCAT22(local_80,puVar8 + *(int *)0x1c02 + 1);
      *puVar2 = 0xb;
      *local_76 = 0xb;
      local_8e = func_0x000fbc2f(0xfbbe,puVar8,local_80);
      iVar7 = local_c * 0x14 + 0xb5;
      func_0x000fbc4d(0xfbc2,&local_8e);
      iVar4 = local_c * 0x14;
      *(undefined1 *)(iVar4 + 0xb7) = *(undefined1 *)(iVar4 + 0xb5);
      *(undefined1 *)(iVar4 + 0xb8) = *(undefined1 *)(iVar4 + 0xb6);
      local_66 = func_0x00086f6f(0xfbc2,param_2,(undefined1 *)_local_5e,
                                 (int)((ulong)_local_5e >> 0x10),iVar7);
      iVar4 = local_a * 0x14;
      *(undefined2 *)(iVar4 + 0xa8) = 0;
      *(undefined2 *)(iVar4 + 0xaa) = 1;
      unaff_ES = (int)((ulong)_local_5e >> 0x10);
      if (*(int *)((undefined1 *)_local_5e + 0x3a) != 0) {
        *(undefined1 *)(iVar4 + 0xa6) = 6;
      }
      iVar4 = local_c * 0x14;
      *(undefined2 *)(iVar4 + 0xa8) = 0;
      *(undefined2 *)(iVar4 + 0xaa) = 1;
      if (*(int *)((undefined1 *)_local_5e + 0x40) != 0) {
        *(undefined1 *)(iVar4 + 0xa6) = 6;
      }
      puVar1 = (uint *)((uint)param_2 * 2 + 0x55a);
      *puVar1 = *puVar1 | 0xb5f;
      in_BX = (undefined1 *)(uint)param_2;
      func_0x000f8e66(0x85eb,in_BX);
      uVar9 = 0xeda2;
      func_0x000ee13b(0xf8e6,param_2);
    }
    local_8a = local_8a + 1;
    uVar10 = uVar9;
  } while( true );
}



void __stdcall16far FUN_0008_7d9c(int param_1)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  if (param_1 == 1) {
    *(undefined1 *)0x468 = 1;
    *(undefined2 *)0x4c6 = 0;
    uVar2 = 0;
    uVar1 = func_0x00093bac();
    func_0x00087e3e(0x92cf,uVar1,uVar2,uVar1);
  }
  else if (param_1 == 3) {
    if (*(int *)0x52a != 0) {
      *(int *)0x4c6 = *(int *)0x4c6 + *(int *)0x52a;
    }
    if (*(int *)0x4c6 < 0xfb) {
      if (*(int *)0x4c6 < -0xfa) {
        uVar2 = 0xffff;
        uVar1 = func_0x00093bac();
        func_0x00087e3e(0x92cf,uVar1,uVar2,uVar1);
        *(undefined2 *)0x4c6 = 0;
        *(undefined1 *)0x46a = 1;
      }
    }
    else {
      uVar2 = 1;
      uVar1 = func_0x00093bac();
      func_0x00087e3e(0x92cf,uVar1,uVar2,uVar1);
      *(undefined2 *)0x4c6 = 0;
      *(undefined1 *)0x46a = 1;
    }
  }
  return;
}



void __stdcall16far FUN_0008_7e3e(int param_1)

{
  undefined2 unaff_DS;
  
  if (param_1 == 0) {
    func_0x000f5e1a();
    *(undefined1 *)0x479 = 0;
  }
  else if (param_1 < 5) {
    func_0x000f5e1a();
    *(char *)0x479 = (char)param_1 + '0';
    *(undefined1 *)0x47a = 0;
  }
  else {
    func_0x000f5e1a();
    *(char *)0x479 = (char)param_1 + '=';
    *(undefined1 *)0x47a = 0;
  }
  return;
}



void __stdcall16far FUN_0008_7ec3(int param_1)

{
  int iVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (param_1 == 1) {
    *(undefined1 *)0x468 = 1;
    *(undefined2 *)0x4c6 = 0;
  }
  else if (param_1 != 3) {
    return;
  }
  if (*(int *)0x52a != 0) {
    *(int *)0x4c6 = *(int *)0x4c6 + *(int *)0x52a;
  }
  if ((0xfa < *(int *)0x4c6) || (*(int *)0x4c6 < -0xfa)) {
    unaff_CS = 0x92cf;
    func_0x00093c07();
    *(undefined2 *)0x4c6 = 0;
    *(undefined1 *)0x46a = 1;
  }
  iVar1 = func_0x00093c07(unaff_CS,0);
  if (iVar1 == 1) {
    *(undefined1 *)0x476 = 0x31;
  }
  else {
    *(undefined1 *)0x476 = 0x32;
  }
  *(undefined1 *)0x477 = 0;
  return;
}



void __stdcall16far FUN_0008_7f44(int param_1)

{
  int iVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (param_1 == 1) {
    *(undefined1 *)0x468 = 1;
    *(undefined2 *)0x4c6 = 0;
  }
  else if (param_1 != 3) {
    return;
  }
  if (*(int *)0x52a != 0) {
    *(int *)0x4c6 = *(int *)0x4c6 + *(int *)0x52a;
  }
  if ((0xfa < *(int *)0x4c6) || (*(int *)0x4c6 < -0xfa)) {
    unaff_CS = 0x92cf;
    func_0x00093c6e();
    *(undefined2 *)0x4c6 = 0;
    *(undefined1 *)0x46a = 1;
  }
  iVar1 = func_0x00093c6e(unaff_CS,0);
  if (iVar1 == 1) {
    func_0x000f5e1a(0x92cf,0x476);
  }
  else {
    func_0x000f5e1a(0x92cf,0x476);
  }
  *(undefined1 *)0x479 = 0;
  return;
}



void __stdcall16far FUN_0008_92cd(undefined4 param_1,undefined2 param_2)

{
  byte *pbVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 uVar4;
  char *pcVar5;
  char *pcVar6;
  undefined2 unaff_DS;
  
  pcVar6 = (char *)s_TRIggerd_0008_8ef8 + 8;
  pcVar5 = (char *)0x9470;
  uVar2 = func_0x0009470e();
  *(int *)0x6d2 = *(int *)0x6d2 + -1;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  if ((*(byte *)(*(int *)0x6d2 * 9 + iVar3) & 1) != 0) {
    pbVar1 = (byte *)(*(int *)0x6d2 * 9 + iVar3);
    *pbVar1 = *pbVar1 & 0xfe;
    pcVar5 = (char *)s_Lock_Unlock_failed_0008_9096 + 8;
    func_0x000914c2(38000,*(undefined2 *)0x6d2,iVar3,uVar4,param_2,pcVar6);
  }
  pcVar6 = pcVar5;
  if (*(char *)*(undefined4 *)0x6ce != '\0') {
    pcVar6 = (char *)s_Lock_Unlock_failed_0008_9096 + 8;
    func_0x0009134c(pcVar5,(int)*(undefined4 *)0x6ce,(int)((ulong)*(undefined4 *)0x6ce >> 0x10),
                    *(undefined2 *)0x6d2);
  }
  *(undefined1 *)0x4f3 = 0;
  func_0x0009470e(pcVar6,uVar2);
  return;
}



void __stdcall16far FUN_0008_934c(byte *param_1,int param_2)

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
  func_0x0009470e(0xe951,(char *)s_TRIggerd_0008_8ef8 + 8);
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



void __stdcall16far FUN_0008_94c2(int param_1,undefined4 param_2,int param_3)

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
  func_0x000fbc09((char *)s_Lock_Unlock_failed_0008_9096 + 8,iVar2,0,iVar3,0,iVar1,uVar4);
  func_0x000fbc09(0xfbbe,iVar2 + param_3,0,iVar3 + param_3,0,iVar1);
  return;
}



void __stdcall16far FUN_0008_95ee(uint param_1,int param_2,undefined4 param_3)

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



void __cdecl16far FUN_0008_96d6(void)

{
  ulong uVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  char *pcVar6;
  undefined1 *local_14;
  undefined4 local_10;
  int local_a;
  
  pcVar6 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar4 = func_0x0009470e();
  _local_14 = (undefined1 *)CONCAT22(unaff_DS,(undefined1 *)0x6bc);
  uVar5 = 38000;
  func_0x0009470e(38000,0x41,pcVar6);
  local_10 = (undefined1 *)
             CONCAT22((int)((ulong)*(undefined4 *)0x1ddc >> 0x10),
                      (undefined1 *)((int)*(undefined4 *)0x1ddc + 0xa2));
  for (local_a = 0x12; puVar3 = local_10, puVar2 = _local_14, local_a < 0x24; local_a = local_a + 1)
  {
    local_10 = (undefined1 *)CONCAT22(local_10._2_2_,(undefined1 *)local_10 + 9);
    *puVar3 = 0;
    uVar1 = (ulong)_local_14 >> 0x10;
    _local_14 = (undefined1 *)CONCAT22((int)uVar1,local_14 + 1);
    *puVar2 = 0xff;
  }
  if (*"" == '\x11') {
    *(undefined1 *)0x468 = 1;
  }
  else if (*"" == '!') {
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
  func_0x0009470e(0xe951,(char *)s_TRIggerd_0008_8ef8 + 8);
  *(undefined1 *)0x4f3 = 0;
  func_0x0009470e(38000,uVar4);
  return;
}



void __cdecl16far FUN_0008_97ba(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2);
  return;
}



void __cdecl16far FUN_0008_9806(void)

{
  return;
}



void __cdecl16far FUN_0008_9830(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  
  uVar1 = 0x41;
  func_0x0009470e();
  *(undefined1 *)0x7b4 = 0x10;
  func_0x0009470e(38000,(char *)s_TRIggerd_0008_8ef8 + 8,uVar1);
  *(byte *)0x603 = *(byte *)0x603 | 1;
  return;
}



void __cdecl16far FUN_0008_9856(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  
  uVar1 = 0x41;
  func_0x0009470e();
  *(undefined1 *)0x7b4 = 0;
  func_0x0009470e(38000,(char *)s_TRIggerd_0008_8ef8 + 8,uVar1);
  *(byte *)0x603 = *(byte *)0x603 | 1;
  return;
}



void __cdecl16far FUN_0008_987c(void)

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
  
  cVar1 = *"<";
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
  func_0x0009470e(uVar4,(char *)s_TRIggerd_0008_8ef8 + 8);
  if (iVar2 == iVar3) {
    func_0x00085301(38000,0x36);
  }
  else {
    *(byte *)0x603 = *(byte *)0x603 | 1;
  }
  return;
}



void __cdecl16far FUN_0008_9948(void)

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
  func_0x0009470e(38000,(char *)s_TRIggerd_0008_8ef8 + 8,uVar3,uVar4,uVar2);
  func_0x00083241(38000,0x22);
  func_0x00082d01((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,uVar4,uVar2);
  func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x22);
  return;
}



void __cdecl16far FUN_0008_999e(void)

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



void __cdecl16far FUN_0008_9a47(void)

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



void __cdecl16far FUN_0008_9afa(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2);
  return;
}



void __cdecl16far FUN_0008_9b62(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2);
  return;
}



void __cdecl16far FUN_0008_9bae(void)

{
  char *pcVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined1 local_14 [10];
  undefined2 local_a;
  
  uVar3 = 0x41;
  local_a = func_0x0009470e();
  if (*(char *)0x1b78 == '\0') {
    uVar2 = 38000;
    func_0x0009470e(38000,local_a,uVar3);
    *(undefined1 *)0x623 = 0x1b;
    local_14[0] = 0;
  }
  else {
    func_0x0009470e(38000,local_a,uVar3);
    pcVar1 = (char *)0x9470;
    while (*(char *)0x6bb != '\0') {
      func_0x000838c0(pcVar1);
      pcVar1 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
    }
    func_0x000eac86(pcVar1,0x693);
    uVar2 = 0x823f;
    func_0x000827db(0xea34,local_14);
  }
  func_0x00082d01(uVar2,local_14);
  return;
}



void __cdecl16far FUN_0008_9c1e(void)

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



void __cdecl16far FUN_0008_9c9b(void)

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
  
  local_11 = *"";
  uVar8 = 0x41;
  uVar7 = 38000;
  uVar4 = func_0x0009470e();
  iVar1 = *(int *)0x570;
  iVar5 = iVar1 * 2;
  iVar6 = *(int *)0x570 * 0x10;
  cVar2 = ((undefined *)&DAT_0008_1b98)[iVar1 * 0x14];
  cVar3 = ((undefined *)&DAT_0008_1b98)[(iVar5 + 1) * 10];
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
  *"" = local_11;
  return;
}



void __cdecl16far FUN_0008_9ec3(void)

{
  int iVar1;
  char cVar2;
  char cVar3;
  char cVar4;
  undefined2 uVar5;
  int iVar6;
  int iVar7;
  char *pcVar8;
  undefined2 uVar9;
  undefined2 unaff_DS;
  undefined1 *puVar10;
  undefined2 uVar11;
  byte local_25;
  undefined1 local_24 [10];
  int local_1a;
  
  local_25 = *(byte *)0x692;
  uVar11 = 0x41;
  uVar5 = func_0x0009470e();
  iVar6 = *(int *)0x570 * 0x10;
  iVar1 = *(int *)0x570;
  iVar7 = iVar1 * 2 + 1;
  cVar2 = ((undefined *)&DAT_0008_1b98)[iVar1 * 0x14];
  cVar3 = ((undefined *)&DAT_0008_1b98)[iVar7 * 10];
  func_0x0009470e(38000,(char *)s_TRIggerd_0008_8ef8 + 8,uVar11);
  pcVar8 = (char *)0x9470;
  while (*(char *)0x6bb != '\0') {
    func_0x000838c0(pcVar8);
    pcVar8 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
  }
  if (((*(byte *)(iVar6 + 399) | 0xf4) == 0xf4) || ((*(byte *)(iVar6 + 399) & 0xb) == 1)) {
    puVar10 = local_24;
    uVar11 = 0xea34;
    func_0x000eac86(pcVar8,0x69d);
    if ((local_25 != 0) && ((*(byte *)(iVar6 + 399) | 0xf4) != 0xf4)) {
      local_25 = 0;
      uVar11 = 0x8511;
      func_0x00085301(0xea34,0x36,puVar10);
    }
  }
  else if ((*(byte *)(iVar6 + 399) & 0xb) == 2) {
    puVar10 = local_24;
    uVar11 = 0xea34;
    func_0x000eac86(pcVar8,0x6a7);
    if (local_25 == 0) {
      local_25 = 1;
      uVar11 = 0x8511;
      func_0x00085301(0xea34,0x36,puVar10);
    }
  }
  else if (local_25 == 0) {
    puVar10 = local_24;
    uVar11 = 0xea34;
    func_0x000eac86(pcVar8,0x69d);
  }
  else {
    puVar10 = local_24;
    uVar11 = 0xea34;
    func_0x000eac86(pcVar8,0x6a7);
  }
  uVar9 = 38000;
  func_0x0009470e(uVar11,0x41,puVar10);
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
      uVar9 = 0x8511;
      func_0x00085301(38000,0x34);
    }
    func_0x000827db(uVar9,local_24);
    uVar9 = 0x823f;
  }
  func_0x0009470e(uVar9,uVar5);
  if (cVar4 != '\0') {
    *(char *)0x623 = cVar4;
  }
  *(byte *)0x692 = local_25;
  func_0x00082d01(38000,local_24);
  return;
}



void __cdecl16far FUN_0008_a0db(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x692 = 0;
  return;
}



void __cdecl16far FUN_0008_a0ea(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x692 = 1;
  return;
}



void __cdecl16far FUN_0008_a0f9(void)

{
  return;
}



void __cdecl16far FUN_0008_a12a(void)

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



void __cdecl16far FUN_0008_a1b9(void)

{
  uint *puVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  int *local_14;
  int local_10;
  undefined1 local_c;
  
  local_10 = func_0x000836aa();
  if (*(char *)0x623 == '\0') {
    uVar2 = func_0x0009470e((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x41);
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



void __cdecl16far FUN_0008_a28a(void)

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



void __cdecl16far FUN_0008_a2d2(void)

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



void __cdecl16far FUN_0008_a31a(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x0009470e(38000,uVar1,uVar2,*(undefined1 *)0x1b78);
  return;
}



void __cdecl16far FUN_0008_a366(void)

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
    func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,iVar1 + 0x30);
  }
  return;
}



void __cdecl16far FUN_0008_a3d3(void)

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



void __cdecl16far FUN_0008_a426(void)

{
  uint *puVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  int local_10;
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
     (((((undefined *)&DAT_0008_1b62)[local_10 * 2] | ((undefined *)&DAT_0008_1b63)[local_10 * 2]) &
      1) == 0)) {
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



void __stdcall16far FUN_0008_a4d2(int param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  char *pcVar3;
  int iVar4;
  int iVar5;
  undefined2 local_c;
  
  pcVar3 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar1 = func_0x0009470e();
  if (param_1 == 0) {
    _local_c = CONCAT22(unaff_DS,0x693);
  }
  else if (param_1 == 1) {
    _local_c = CONCAT22(unaff_DS,0x69d);
  }
  else if (param_1 == 2) {
    _local_c = CONCAT22(unaff_DS,0x6a7);
  }
  else if (param_1 == 3) {
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
  func_0x000eac86(38000,0x65e,unaff_DS,(int)_local_c,uVar2,pcVar3,iVar5,uVar1);
  *(undefined1 *)(iVar5 + (int)_local_c) = 0xff;
  *(undefined1 *)0x6bb = 0;
  func_0x0009470e(0xea34,uVar1);
  return;
}



void __stdcall16far FUN_0008_a581(uint param_1)

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



void __cdecl16far FUN_0008_a609(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x600 = *(undefined2 *)0x4fa;
  return;
}



void __cdecl16far FUN_0008_a621(void)

{
  undefined2 unaff_DS;
  
  if (*"<" == '\x0e') {
    *(uint *)0x4f8 = *(uint *)0x4f8 & 0xfffe;
  }
  else if (*"<" == '\x0f') {
    *(uint *)0x4f8 = *(uint *)0x4f8 | 1;
  }
  else {
    *(undefined1 *)0x623 = 0x15;
  }
  return;
}



void __cdecl16far FUN_0008_a650(void)

{
  return;
}



int __cdecl16far FUN_0008_a676(void)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_DS;
  byte local_5d;
  
  uVar1 = *(undefined2 *)0x4f8;
  iVar2 = func_0x00086b4b();
  if (iVar2 == 0) {
    if ((local_5d & 8) == 0) {
      iVar2 = func_0x0008373a(0x85eb,0);
      if ((iVar2 < 0) || (*(char *)0x57e != '\0')) {
        *(undefined1 *)0x623 = 0x15;
      }
      else {
        func_0x0009470e((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x41);
        iVar3 = func_0x000f3495(38000,uVar1,iVar2);
        iVar2 = func_0x0009470e(0xf313,(char *)s_TRIggerd_0008_8ef8 + 8);
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



void __cdecl16far FUN_0008_a719(void)

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



void __cdecl16far FUN_0008_a77e(void)

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



void __cdecl16far FUN_0008_a7e3(void)

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



void __cdecl16far FUN_0008_a832(void)

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



void __cdecl16far FUN_0008_a881(void)

{
  int iVar1;
  undefined2 unaff_DS;
  
  iVar1 = func_0x00086b4b();
  if (iVar1 != 0) {
    *(undefined1 *)0x623 = 0x1b;
  }
  return;
}



void __cdecl16far FUN_0008_a8d3(void)

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
    iVar2 = func_0x00086b4b((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,iVar2 >> 1,
                            local_5c);
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
        func_0x0009470e(0xf313,(char *)s_TRIggerd_0008_8ef8 + 8);
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



void __cdecl16far FUN_0008_aa2f(void)

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



void __cdecl16far FUN_0008_ac17(void)

{
  int iVar1;
  undefined2 unaff_DS;
  
  iVar1 = func_0x00086b4b();
  if (iVar1 != 0) {
    *(undefined1 *)0x623 = 0x1b;
  }
  return;
}



void __cdecl16far FUN_0008_acf2(void)

{
  func_0x00092d17();
  return;
}



void __cdecl16far FUN_0008_ad05(void)

{
  func_0x00092d17();
  return;
}



void __stdcall16far FUN_0008_ad17(int param_1)

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
    if (*"" == '\0') {
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
          func_0x0009470e(0xf008,(char *)s_TRIggerd_0008_8ef8 + 8);
        }
        else {
          uVar2 = 0x41;
          func_0x0009470e();
          func_0x000f8e66(38000,iVar1,uVar2);
          func_0x0009470e(0xf8e6,(char *)s_TRIggerd_0008_8ef8 + 8);
        }
        unaff_CS = 38000;
      }
      goto LAB_0008_aeab;
    }
    if (iVar1 != 4) {
      *(undefined1 *)0x623 = 0x15;
      goto LAB_0008_aeab;
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
      func_0x0009470e(0xf008,(char *)s_TRIggerd_0008_8ef8 + 8);
    }
    else {
      func_0x0009470e(0x85eb,0x41);
      func_0x000f8e66(38000,1);
      unaff_CS = 38000;
      func_0x0009470e(0xf8e6,(char *)s_TRIggerd_0008_8ef8 + 8);
    }
  }
  else {
    *(undefined1 *)0x623 = 0x22;
  }
LAB_0008_aeab:
  *(uint *)*(undefined4 *)0x736 = *(uint *)*(undefined4 *)0x736 | 0x200;
  func_0x0009470e(unaff_CS,0x41);
  *(uint *)0x55a = *(uint *)0x55a | 0x200;
  func_0x0009470e(38000,(char *)s_TRIggerd_0008_8ef8 + 8);
  return;
}



void __cdecl16far FUN_0008_aed2(void)

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
      if (*"" == '\0') {
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



void __stdcall16far FUN_0008_af8a(int param_1,int param_2)

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
  *(uint *)0x55a = *(uint *)0x55a | 0x200;
  func_0x0009470e(38000,(char *)s_TRIggerd_0008_8ef8 + 8);
  return;
}



void __cdecl16far FUN_0008_b051(void)

{
  int iVar1;
  undefined1 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  if (*(char *)((int)*(undefined4 *)0x732 + 0x1f) == '\x1e') {
    if (*"B" == '\0') {
      *(undefined2 *)0x5f8 = 1;
    }
  }
  else {
    if (*"B" == '\0') {
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



void __cdecl16far FUN_0008_b1e8(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x4f6 = 0;
  *(byte *)0x603 = *(byte *)0x603 | 1;
  return;
}



void __cdecl16far FUN_0008_b1fc(void)

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



void __cdecl16far FUN_0008_b227(void)

{
  undefined2 unaff_DS;
  
  *(uint *)0x5f8 = (uint)*(byte *)0x4f6;
  return;
}



void __cdecl16far FUN_0008_b252(void)

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



void __cdecl16far FUN_0008_b2bc(void)

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



void __cdecl16far FUN_0008_b339(void)

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



void __cdecl16far FUN_0008_b3a3(void)

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



void __cdecl16far FUN_0008_b440(void)

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
  func_0x00082d01((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,local_28);
  func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x22);
  return;
}



void __cdecl16far FUN_0008_b568(void)

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
  func_0x00082d01((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,local_28);
  func_0x00083241((char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18,0x22);
  return;
}



void __cdecl16far FUN_0008_b600(void)

{
  func_0x00081404();
  return;
}



void __cdecl16far FUN_0008_b622(void)

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
    if (((*"B" != '\x04') || (0x1e < *(int *)0x5f8)) || (*(int *)0x5f8 < 1)) {
      if (*"B" == '\0') {
        *(undefined1 *)0x623 = 7;
        return;
      }
      *(undefined1 *)0x623 = 0x15;
      return;
    }
  }
  else {
    if (((*"B" != '\0') && (*(int *)0x5f8 != 4)) && ((*(int *)0x5f8 != 1 && (*(int *)0x5f8 != 0))))
    {
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
        func_0x0009470e(38000,(char *)s_TRIggerd_0008_8ef8 + 8);
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



void __stdcall16far FUN_0008_b925(int param_1,int param_2)

{
  byte bVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  uVar3 = 0x41;
  func_0x0009470e();
  bVar1 = *(byte *)0x18c;
  func_0x0009470e(38000,(char *)s_TRIggerd_0008_8ef8 + 8,uVar3);
  if ((((param_1 == 4) && ((bVar1 & 1) != 0)) || (param_1 == 1)) || (param_1 == 0)) {
    if ((param_1 == 1) && (param_2 == 1)) {
      func_0x0009470e(38000,0x41);
      func_0x000f706e(38000,0,1,1);
      func_0x0009470e(0xf706,(char *)s_TRIggerd_0008_8ef8 + 8);
      *(uint *)*(undefined4 *)0x736 = *(uint *)*(undefined4 *)0x736 | 0x200;
    }
    else if ((param_1 == 4) && (param_2 == 1)) {
      func_0x0009470e(38000,0x41);
      func_0x000f0414(38000,2);
      func_0x000f0414(0xf008,3);
      func_0x000f8f24(0xf008,2);
      func_0x000f8f24(0xf8e6,3);
      uVar3 = 0x97c6;
      func_0x00097c95(0xf8e6,1);
      while( true ) {
        iVar2 = func_0x00097c6e(uVar3,1);
        if (iVar2 == 0) break;
        uVar3 = 0xe693;
        func_0x000e693c(0x97c6);
      }
      func_0x000f706e(0x97c6,0,1,4);
      func_0x0009470e(0xf706,(char *)s_TRIggerd_0008_8ef8 + 8);
      *(uint *)*(undefined4 *)0x736 = *(uint *)*(undefined4 *)0x736 | 0x200;
    }
    else {
      if (((*(byte *)((int)*(undefined4 *)0x732 + 0x80) & 2) != 0) ||
         ((*(byte *)((int)*(undefined4 *)0x732 + 0x81) & 2) != 0)) {
        func_0x0009470e(38000,0x41);
        bVar1 = *(byte *)0x19c;
        func_0x0009470e(38000,(char *)s_TRIggerd_0008_8ef8 + 8);
        if ((bVar1 & 1) != 0) {
          func_0x0009470e(38000,0x41);
          func_0x000f0414(38000,1);
          func_0x000f8f24(0xf008,1);
          uVar3 = 0x97c6;
          func_0x00097c95(0xf8e6,1);
          while( true ) {
            iVar2 = func_0x00097c6e(uVar3,1);
            if (iVar2 == 0) break;
            uVar3 = 0xe693;
            func_0x000e693c(0x97c6);
          }
          func_0x0009470e(0x97c6,(char *)s_TRIggerd_0008_8ef8 + 8);
        }
      }
      func_0x0009470e(38000,0x41);
      func_0x000f706e(38000,0,param_2,1);
      func_0x0009470e(0xf706,(char *)s_TRIggerd_0008_8ef8 + 8);
      *(uint *)*(undefined4 *)0x736 = *(uint *)*(undefined4 *)0x736 | 0x200;
    }
  }
  else {
    *(undefined1 *)0x623 = 0x1b;
  }
  return;
}



char __stdcall16far FUN_0008_bb1c(int param_1)

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



int __stdcall16far FUN_0008_bb7e(byte param_1)

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



uint __stdcall16far FUN_0008_bbac(int param_1)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  char *pcVar2;
  uint local_e;
  
  pcVar2 = (char *)s_TRIggerd_0008_8ef8 + 8;
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
  func_0x0009470e(38000,uVar1,pcVar2);
  return local_e;
}



undefined2 __stdcall16far FUN_0008_bc07(int param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  char *pcVar3;
  int local_e;
  
  pcVar3 = (char *)s_TRIggerd_0008_8ef8 + 8;
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
    func_0x00085b2b(38000,pcVar3);
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



bool __stdcall16far FUN_0008_bc6e(int param_1)

{
  char cVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  char *pcVar3;
  
  pcVar3 = (char *)s_TRIggerd_0008_8ef8 + 8;
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
  func_0x0009470e(38000,uVar2,pcVar3);
  return cVar1 != '\0';
}



void __cdecl16far FUN_0008_bcc2(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0008_bcdb(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0008_bcf5(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0008_bd0e(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0008_bd28(void)

{
  int iVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  bool bVar2;
  undefined1 uVar3;
  undefined4 uVar4;
  int local_c;
  
  if (*"" == '\0') {
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



void __cdecl16far FUN_0008_be0c(void)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined2 local_c;
  undefined2 local_a;
  
  *(byte *)0x603 = *(byte *)0x603 | 3;
  if (*"" == '\0') {
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



void __cdecl16far FUN_0008_be9c(void)

{
  return;
}



void __cdecl16far FUN_0008_bec6(void)

{
  func_0x00094488();
  return;
}



void __cdecl16far FUN_0008_bef1(void)

{
  func_0x00094488();
  return;
}



void __cdecl16far FUN_0008_bf1c(void)

{
  return;
}



void __cdecl16far FUN_0008_bf46(void)

{
  return;
}



void __cdecl16far FUN_0008_bf75(void)

{
  byte *pbVar1;
  undefined2 unaff_DS;
  
  pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x12);
  *pbVar1 = *pbVar1 | 2;
  *(byte *)0x603 = *(byte *)0x603 | 1;
  return;
}



void __cdecl16far FUN_0008_bf8d(void)

{
  byte *pbVar1;
  undefined2 unaff_DS;
  
  pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x12);
  *pbVar1 = *pbVar1 & 0xfd;
  *(byte *)0x603 = *(byte *)0x603 | 3;
  return;
}



void __cdecl16far FUN_0008_bfa5(void)

{
  return;
}



/* WARNING: Unable to track spacebase fully for stack */

void __cdecl16far FUN_0008_bfcf(void)

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
  char *unaff_CS;
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
    *(char **)(puVar5 + -4) = unaff_CS;
    unaff_CS = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
    puVar9 = (undefined2 *)(puVar5 + -6);
    puVar5 = puVar5 + -6;
    *puVar9 = 0xc0ce;
    func_0x00083241();
    bVar3 = 0 < (int)unaff_SI;
    unaff_SI = unaff_SI + -1;
  } while (bVar3);
  return;
}



void __cdecl16far FUN_0008_c05f(void)

{
  func_0x00082e5b();
  return;
}



void __cdecl16far FUN_0008_c082(void)

{
  func_0x00081404();
  return;
}



void __cdecl16far FUN_0008_c0aa(void)

{
  func_0x00081404();
  return;
}



void __cdecl16far FUN_0008_c14c(void)

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



void __cdecl16far FUN_0008_c1fa(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0008_c22f(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0008_c264(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0008_c2a6(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0008_c2e8(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0008_c301(void)

{
  func_0x000944a2();
  return;
}



void __cdecl16far FUN_0008_c31b(void)

{
  func_0x000946c2();
  return;
}



void __cdecl16far FUN_0008_c356(void)

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



void __cdecl16far FUN_0008_c3b8(void)

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



void __cdecl16far FUN_0008_c411(void)

{
  func_0x000944d5();
  return;
}



void __cdecl16far FUN_0008_c424(void)

{
  func_0x000944d5();
  return;
}



void __cdecl16far FUN_0008_c437(void)

{
  func_0x000944d5();
  return;
}



void __cdecl16far FUN_0008_c44a(void)

{
  func_0x000944d5();
  return;
}



void __cdecl16far FUN_0008_c45d(void)

{
  func_0x000944d5();
  return;
}



void __cdecl16far FUN_0008_c470(void)

{
  undefined2 uVar1;
  
  uVar1 = func_0x00094559();
  func_0x000944d5(0x941f,uVar1,uVar1);
  return;
}



undefined1 __stdcall16far FUN_0008_c488(int param_1)

{
  undefined2 unaff_DS;
  
  return *(undefined1 *)(param_1 + (int)*(undefined4 *)0x73a);
}



void __stdcall16far FUN_0008_c4a2(int param_1,undefined1 param_2)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)(param_1 + (int)*(undefined4 *)0x73a) = param_2;
  *(undefined2 *)*(undefined4 *)0x736 = 0xffff;
  *(byte *)0x603 = *(byte *)0x603 | 1;
  if (*(char *)0x604 == '\0') {
    *(byte *)0x603 = *(byte *)0x603 | 2;
  }
  return;
}



void __stdcall16far FUN_0008_c4d5(void)

{
  func_0x00094488();
  return;
}



undefined2 __cdecl16far FUN_0008_c559(void)

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



byte __stdcall16far FUN_0008_c5cf(int param_1,int param_2)

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



void __stdcall16far FUN_0008_c6c2(int param_1)

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



undefined2 __stdcall16far FUN_0008_c70e(void)

{
  undefined2 unaff_DS;
  
  return unaff_DS;
}



void __stdcall16far FUN_0008_c71c(byte param_1,undefined2 *param_2)

{
  byte *pbVar1;
  int *piVar2;
  byte bVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  int iVar6;
  undefined2 unaff_DS;
  char *pcVar7;
  
  pcVar7 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar4 = func_0x0009470e();
  iVar6 = param_1 - 0x61;
  bVar3 = *(byte *)(iVar6 * 0xc + (int)*(undefined4 *)0x73e + 10);
  uVar5 = func_0x000951aa(38000,iVar6,pcVar7);
  if ((bVar3 & 2) == 0) {
    *(undefined2 *)0x690 = 0x117;
    *param_2 = 2;
    func_0x0009470e(0x9471,uVar4);
  }
  else {
    func_0x0009470e(0x9471,0x41);
    func_0x000e5d2f(38000);
    func_0x0009470e(0xe5d1,(char *)s_TRIggerd_0008_8ef8 + 8);
    if ((((bVar3 & 0x10) != 0) || (*(int *)((int)*(undefined4 *)0x746 + 4) != 0)) ||
       (*(int *)*(undefined4 *)0x746 != 0)) {
      *(undefined2 *)0x690 = 0xf4;
    }
    *(undefined2 *)0x690 = 0xfa;
    pbVar1 = (byte *)(iVar6 * 0xc + (int)*(undefined4 *)0x73e + 10);
    *pbVar1 = *pbVar1 | 8;
    piVar2 = (int *)(iVar6 * 0xc + (int)*(undefined4 *)0x73e + 8);
    *piVar2 = *piVar2 + 1;
    piVar2 = (int *)((int)*(undefined4 *)0x746 + 2);
    *piVar2 = *piVar2 + 1;
    func_0x0009470e(38000,0x41);
    func_0x000e5d28(38000);
    func_0x0009470e(0xe5d1,(char *)s_TRIggerd_0008_8ef8 + 8);
    *param_2 = uVar5;
    func_0x0009470e(38000,uVar4);
  }
  return;
}



void __stdcall16far FUN_0008_c85b(byte param_1,undefined4 param_2,int *param_3)

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
    uVar9 = (undefined2)((ulong)*(undefined4 *)(code *)FUN_0008_06ca >> 0x10);
    iVar7 = (int)*(undefined4 *)(code *)FUN_0008_06ca + *(int *)((int)*(undefined4 *)0x746 + 8);
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
            uVar9 = (undefined2)((ulong)*(undefined4 *)(code *)FUN_0008_06ca >> 0x10);
            iVar7 = (int)*(undefined4 *)(code *)FUN_0008_06ca +
                    *(int *)((int)*(undefined4 *)0x746 + 8);
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



undefined2 __stdcall16far FUN_0008_cd9c(byte param_1)

{
  int *piVar1;
  byte *pbVar2;
  byte bVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  int iVar6;
  undefined2 unaff_DS;
  char *pcVar7;
  
  if ((param_1 < 0x61) || (0x7a < param_1)) {
    uVar4 = 0;
  }
  else {
    pcVar7 = (char *)s_TRIggerd_0008_8ef8 + 8;
    uVar5 = func_0x0009470e();
    iVar6 = param_1 - 0x61;
    bVar3 = *(byte *)(iVar6 * 0xc + (int)*(undefined4 *)0x73e + 10);
    if ((bVar3 & 0x18) == 0) {
      func_0x0009470e(38000,uVar5);
      uVar4 = 8;
    }
    else if ((bVar3 & 8) == 0) {
      uVar4 = func_0x000950c2(38000,iVar6,pcVar7);
      func_0x0009470e(0x9471,0x41);
      func_0x000e5d2f(38000);
      func_0x0009470e(0xe5d1,(char *)s_TRIggerd_0008_8ef8 + 8);
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
      func_0x0009470e(0xe5d1,(char *)s_TRIggerd_0008_8ef8 + 8);
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



undefined2 __stdcall16far FUN_0008_cef5(int param_1)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  char *pcVar4;
  undefined2 local_10;
  
  pcVar4 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar3 = 38000;
  uVar1 = func_0x0009470e();
  param_1 = param_1 + -0x61;
  if ((param_1 < 0) || (0x19 < param_1)) {
    local_10 = 0x10;
  }
  else {
    uVar3 = 0x9471;
    iVar2 = func_0x00096126(38000,pcVar4);
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



undefined2 __stdcall16far FUN_0008_cfda(int param_1)

{
  byte *pbVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  char *pcVar5;
  undefined2 local_10;
  
  pcVar5 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar4 = 38000;
  uVar2 = func_0x0009470e();
  param_1 = param_1 + -0x61;
  if ((param_1 < 0) || (0x19 < param_1)) {
    local_10 = 0x10;
  }
  else {
    uVar4 = 0x9471;
    iVar3 = func_0x00096126(38000,pcVar5);
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



undefined1 __stdcall16far FUN_0008_d067(byte param_1)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  char *pcVar3;
  
  pcVar3 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar2 = func_0x0009470e();
  if ((param_1 < 0x61) || (0x7a < param_1)) {
    uVar1 = 0;
  }
  else {
    uVar1 = *(undefined1 *)((param_1 - 0x61) * 0xc + (int)*(undefined4 *)0x73e + 10);
  }
  func_0x0009470e(38000,uVar2,pcVar3);
  return uVar1;
}



void __stdcall16far FUN_0008_d0c2(int param_1)

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
  uVar8 = (undefined2)((ulong)*(undefined4 *)(code *)FUN_0008_06ca >> 0x10);
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



undefined4 __stdcall16far FUN_0008_d1aa(int param_1)

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
  iVar8 = *(int *)(param_1 * 0xc + (int)*(undefined4 *)0x73e);
  iVar10 = *(int *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 4);
  uVar3 = *(undefined4 *)(param_1 * 0x4e + (int)*(undefined4 *)0x742 + 0x36);
  uVar4 = *(undefined4 *)(param_1 * 0x4e + (int)*(undefined4 *)0x742 + 0x3c);
  if (((((int)*(undefined4 *)(code *)FUN_0008_06ca + iVar8 != (int)uVar3) ||
       ((int)((ulong)*(undefined4 *)(code *)FUN_0008_06ca >> 0x10) != (int)((ulong)uVar3 >> 0x10)))
      || ((int)*(undefined4 *)(code *)FUN_0008_06ca + iVar10 != (int)uVar4)) ||
     ((int)((ulong)*(undefined4 *)(code *)FUN_0008_06ca >> 0x10) != (int)((ulong)uVar4 >> 0x10))) {
    *(undefined2 *)0x690 = 0x387;
    local_1c = 1;
  }
  if (((iVar8 < 0) || (0x6800 < iVar8)) || ((iVar10 < 0 || (0x6800 < iVar10)))) {
    *(undefined2 *)0x690 = 0x38f;
    local_1c = 1;
  }
  iVar10 = *(int *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 2);
  iVar2 = *(int *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 6);
  uVar9 = iVar10 + iVar2;
  if ((iVar10 != *(int *)((int)*(undefined4 *)0x742 + param_1 * 0x4e + 0x3a)) ||
     (iVar2 != *(int *)((int)*(undefined4 *)0x742 + param_1 * 0x4e + 0x40))) {
    *(undefined2 *)0x690 = 0x39b;
    local_1c = local_1c | 2;
  }
  if ((((0x2000 < (int)uVar9) || ((int)uVar9 < 0x400)) || ((uVar9 & 0x3ff) != 0)) ||
     (0x6800 < (int)(iVar8 + uVar9))) {
    *(undefined2 *)0x690 = 0x3ab;
    local_1c = local_1c | 2;
  }
  bVar1 = *(byte *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 10);
  uVar9 = bVar1 & 1;
  if (((bVar1 & 1) != 0) && ((bVar1 & 0xfa) != 0)) {
    local_1c = local_1c | 0x80;
  }
  if (((bVar1 & 2) != 0) && ((bVar1 & 0x80) != 0)) {
    local_1c = local_1c | 0x80;
  }
  if (((bVar1 & 0x18) != 0) ||
     (uVar9 = (uint)((ulong)((long)param_1 * 0xc) >> 0x10),
     *(int *)((int)((long)param_1 * 0xc) + (int)*(undefined4 *)0x73e + 8) != 0)) {
    local_1c = local_1c | 0x200;
  }
  if ((local_1c == 0) || (local_1c == 0x200)) {
    uVar11 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10);
    iVar10 = (int)*(undefined4 *)0x742;
    iVar8 = *(int *)(param_1 * 0x4e + (int)*(undefined4 *)0x742 + 0x3a) +
            *(int *)(param_1 * 0x4e + iVar10 + 0x40);
    uVar12 = (undefined2)((ulong)*(undefined4 *)(code *)FUN_0008_06ca >> 0x10);
    cVar5 = func_0x0009605a();
    cVar6 = func_0x0009605a(0x9471,(int)*(undefined4 *)0x742 + param_1 * 0x4e,
                            (int)((ulong)*(undefined4 *)0x742 >> 0x10),0x4c,uVar12,iVar8,uVar11,
                            iVar10);
    cVar7 = func_0x0009605a(0x9471,(int)*(undefined4 *)0x73e + param_1 * 0xc,
                            (int)((ulong)*(undefined4 *)0x73e >> 0x10),8);
    if (cVar5 != *(char *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 0xb)) {
      *(undefined2 *)0x690 = 0x3ef;
      local_1c = local_1c | 4;
    }
    cVar5 = *(char *)((int)((long)param_1 * 0x4e) + (int)*(undefined4 *)0x742 + 0x4c);
    uVar9 = CONCAT11((char)((ulong)((long)param_1 * 0x4e) >> 0x18),cVar5);
    if ((char)(cVar6 + cVar7) != cVar5) {
      local_1c = local_1c | 0x80;
      *(undefined2 *)0x690 = 0x3f6;
    }
  }
  return CONCAT22(uVar9,local_1c);
}



undefined2 __cdecl16far FUN_0008_d476(void)

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
  char *pcVar12;
  int local_ba;
  int local_ae;
  int local_ac;
  int local_a4;
  int local_a2;
  byte local_a0 [158];
  
  pcVar12 = (char *)s_TRIggerd_0008_8ef8 + 8;
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
      func_0x0009470e(uVar11,uVar5,pcVar12);
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



undefined2 __stdcall16far FUN_0008_db69(undefined4 param_1,int param_2)

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
    if (param_2 <= local_c) {
LAB_0008_dd47:
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
    uVar8 = (undefined2)((ulong)param_1 >> 0x10);
    iVar6 = (int)param_1;
    iVar1 = *(int *)(iVar5 + iVar6 + 2);
    if ((local_12 < *(uint *)(iVar5 + iVar6)) && (*(char *)(iVar5 + iVar6 + 4) == '\0')) {
      *(undefined2 *)0x690 = 0x585;
      iVar6 = iVar1 * 0xc;
      iVar5 = *(int *)(iVar6 + (int)*(undefined4 *)0x73e + 2);
      uVar7 = iVar5 + *(int *)(iVar6 + (int)*(undefined4 *)0x73e + 6);
      local_1c = (undefined1 *)
                 CONCAT22((int)((ulong)*(undefined4 *)(code *)FUN_0008_06ca >> 0x10),
                          (undefined1 *)
                          ((int)*(undefined4 *)(code *)FUN_0008_06ca +
                          *(int *)(iVar6 + (int)*(undefined4 *)0x73e)));
      local_20 = (undefined1 *)
                 CONCAT22((int)((ulong)*(undefined4 *)(code *)FUN_0008_06ca >> 0x10),
                          (undefined1 *)((int)*(undefined4 *)(code *)FUN_0008_06ca + local_12));
      if (0x6800 < local_12 + uVar7) {
        local_22 = 1;
        goto LAB_0008_dd47;
      }
      for (local_a = 0; puVar4 = local_1c, puVar3 = local_20, local_a < uVar7; local_a = local_a + 1
          ) {
        local_20 = (undefined1 *)CONCAT22(local_20._2_2_,(undefined1 *)local_20 + 1);
        local_1c = (undefined1 *)CONCAT22(local_1c._2_2_,(undefined1 *)local_1c + 1);
        *puVar3 = *puVar4;
      }
      *(uint *)(iVar1 * 0xc + (int)*(undefined4 *)0x73e) = local_12;
      *(int *)((int)*(undefined4 *)0x73e + iVar1 * 0xc + 4) = local_12 + iVar5;
      uVar2 = *(undefined4 *)(code *)FUN_0008_06ca;
      uVar8 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10);
      iVar6 = (int)*(undefined4 *)0x742;
      *(int *)(iVar6 + iVar1 * 0x4e + 0x36) = (int)uVar2 + local_12;
      *(undefined2 *)(iVar6 + iVar1 * 0x4e + 0x38) = (int)((ulong)uVar2 >> 0x10);
      uVar2 = *(undefined4 *)(code *)FUN_0008_06ca;
      uVar8 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10);
      iVar6 = (int)*(undefined4 *)0x742;
      *(int *)(iVar6 + iVar1 * 0x4e + 0x3c) = (int)uVar2 + local_12 + iVar5;
      *(undefined2 *)(iVar6 + iVar1 * 0x4e + 0x3e) = (int)((ulong)uVar2 >> 0x10);
      func_0x000950c2(unaff_CS,iVar1,local_12);
      local_12 = local_12 + uVar7;
      unaff_CS = 0x9471;
    }
    else if (*(char *)(local_c * 6 + iVar6 + 4) == '\0') {
      local_12 = local_12 +
                 *(int *)(iVar1 * 0xc + (int)*(undefined4 *)0x73e + 2) +
                 *(int *)(iVar1 * 0xc + (int)*(undefined4 *)0x73e + 6);
      if (0x6800 < local_12) {
        local_22 = 1;
        goto LAB_0008_dd47;
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



void __stdcall16far FUN_0008_dd81(int param_1)

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
               CONCAT22((int)((ulong)*(undefined4 *)(code *)FUN_0008_06ca >> 0x10),
                        (undefined1 *)((int)*(undefined4 *)(code *)FUN_0008_06ca + local_12));
    local_c = (undefined1 *)
              CONCAT22((int)((ulong)*(undefined4 *)(code *)FUN_0008_06ca >> 0x10),
                       (undefined1 *)((int)*(undefined4 *)(code *)FUN_0008_06ca + local_14));
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
    uVar2 = *(undefined4 *)(code *)FUN_0008_06ca;
    uVar10 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10);
    iVar7 = (int)*(undefined4 *)0x742;
    *(int *)(iVar7 + local_16 * 0x4e + 0x36) = (int)uVar2 + local_14;
    *(undefined2 *)(iVar7 + local_16 * 0x4e + 0x38) = (int)((ulong)uVar2 >> 0x10);
    uVar2 = *(undefined4 *)(code *)FUN_0008_06ca;
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



void __cdecl16far FUN_0008_df69(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  char *pcVar2;
  int local_a;
  
  pcVar2 = (char *)s_TRIggerd_0008_8ef8 + 8;
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
                  ,pcVar2);
  *(undefined2 *)*(undefined4 *)0x746 = 0;
  *(undefined2 *)((int)*(undefined4 *)0x746 + 2) = 0;
  *(undefined2 *)((int)*(undefined4 *)0x746 + 4) = 0;
  *(undefined2 *)((int)*(undefined4 *)0x746 + 6) = 0x6800;
  *(undefined2 *)((int)*(undefined4 *)0x746 + 8) = 0;
  func_0x0009470e(0x9471,uVar1);
  return;
}



char __stdcall16far FUN_0008_e05a(char *param_1,int param_2)

{
  char *pcVar1;
  char local_9;
  
  local_9 = '\0';
  while (pcVar1 = param_1, 0 < param_2) {
    param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
    local_9 = local_9 + *pcVar1;
    param_2 = param_2 + -1;
  }
  return local_9;
}



void __stdcall16far FUN_0008_e087(undefined1 *param_1,int param_2,undefined1 param_3)

{
  undefined1 *puVar1;
  
  while (puVar1 = param_1, param_2 = param_2 + -1, 0 < param_2) {
    param_1 = (undefined1 *)CONCAT22(param_1._2_2_,(undefined1 *)param_1 + 1);
    *puVar1 = param_3;
  }
  return;
}



void __stdcall16far FUN_0008_e0ab(char *param_1)

{
  char *pcVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  char *pcVar5;
  int local_a;
  
  pcVar5 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar2 = func_0x0009470e();
  for (local_a = 0; pcVar1 = param_1, local_a < 0x1a; local_a = local_a + 1) {
    uVar4 = (undefined2)((ulong)*(undefined4 *)0x742 >> 0x10);
    iVar3 = (int)*(undefined4 *)0x742 + local_a * 0x4e;
    param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
    *pcVar1 = (char)(*(int *)(iVar3 + 0x3a) + *(int *)(iVar3 + 0x40) >> 10) + '0';
  }
  func_0x0009470e(38000,uVar2,pcVar5);
  return;
}



undefined2 __cdecl16far FUN_0008_e126(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  char *pcVar3;
  undefined2 uVar4;
  undefined2 local_a;
  
  uVar4 = 0x41;
  uVar1 = func_0x0009470e();
  func_0x000e5d2f(38000,uVar4);
  pcVar3 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar2 = func_0x0009470e(0xe5d1,(char *)s_TRIggerd_0008_8ef8 + 8);
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
  func_0x0009470e(38000,0x41,pcVar3,uVar4,uVar2,uVar1);
  func_0x000e5d28(38000);
  func_0x0009470e(0xe5d1,(char *)s_TRIggerd_0008_8ef8 + 8);
  return local_a;
}



void __stdcall16far FUN_0008_e1af(undefined4 param_1)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  char *pcVar2;
  
  pcVar2 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar1 = func_0x0009470e();
  func_0x000fbc09(38000,(int)*(undefined4 *)0x746,(int)((ulong)*(undefined4 *)0x746 >> 0x10),
                  (int)param_1,(int)((ulong)param_1 >> 0x10),0xe,pcVar2);
  func_0x0009470e(0xfbbe,uVar1);
  return;
}



void __stdcall16far FUN_0008_e1e9(int param_1)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)(param_1 * 0xc + (int)*(undefined4 *)0x73e) = 0;
  *(undefined2 *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 4) = 0;
  *(undefined2 *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 2) = 0;
  *(undefined2 *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 6) = 0;
  *(undefined2 *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 8) = 0;
  *(undefined1 *)(param_1 * 0xc + (int)*(undefined4 *)0x73e + 10) = 1;
  func_0x00096087();
  return;
}



void __cdecl16far FUN_0008_e28c(void)

{
  undefined2 uVar1;
  char *pcVar2;
  
  pcVar2 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar1 = func_0x0009470e();
  func_0x00080133(38000,pcVar2,uVar1);
  func_0x0009470e(0x8013,(char *)s_TRIggerd_0008_8ef8 + 8);
  func_0x00085e66(38000);
  func_0x000962c2(0x85e6);
  func_0x0009470e(0x9628,uVar1);
  return;
}



void __cdecl16far FUN_0008_e2c2(void)

{
  byte *pbVar1;
  byte bVar2;
  byte bVar3;
  char *pcVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  undefined2 uVar6;
  
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
  *(undefined1 *)0x590 = 0x80;
  *(undefined1 *)0xc = 0;
  if (*"\t" == '\0') {
    uVar6 = 0xffff;
    func_0x0008009b();
    *(undefined1 *)0x59c = 0;
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 2) = 0;
    *(undefined1 *)0x59a = 0;
    *(undefined1 *)0x59f = 0;
    *(undefined1 *)0x59b = 0x40;
    *(code *)FUN_0008_03de = (code)0xff;
    uVar5 = 0x9628;
    func_0x000966e7(0x8006,uVar6);
    if (*"\x06" == '\x1f') {
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
      *(undefined1 *)0x590 = 0x80;
      *(undefined1 *)0x580 = 0x80;
    }
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = *(undefined1 *)0x59f;
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 1) = *(undefined1 *)0x59a;
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 2) = *(undefined1 *)0x59c;
    pcVar4 = "";
    func_0x0008009b(uVar5,0);
    *(undefined1 *)0x5a3 = 0;
  }
  else {
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 0x80;
    pcVar4 = (char *)0x9628;
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
      pcVar4 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
      func_0x000839d1(0x8006);
    }
  }
  func_0x00096800(pcVar4);
  return;
}



void __cdecl16far FUN_0008_e597(void)

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
  *(undefined2 *)0x444 = 0;
  *(undefined2 *)0x446 = 0;
  *(undefined2 *)0x440 = 0;
  *(undefined2 *)0x442 = 0;
  *(undefined1 *)0x1 = 0;
  *(undefined1 *)0x0 = 0;
  *(undefined1 *)0x454 = 0;
  *(undefined1 *)0x3 = 0;
  *(undefined1 *)0x451 = 0;
  *(undefined1 *)0x2 = 0;
  if (*"\t" != '\0') {
    func_0x00096dbe();
    if ((*(byte *)0x4f1 & 0x20) == 0) {
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 3;
    }
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 2;
  }
  return;
}



void __cdecl16far FUN_0008_e634(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x454 = 0;
  *(undefined1 *)0x459 = 0;
  *(undefined1 *)0x68f = 0;
  *(undefined1 *)0x455 = 0xff;
  *(undefined2 *)0x448 = 0xaf;
  *(undefined2 *)0x44a = unaff_DS;
  *(undefined2 *)0x44c = 0xaf;
  *(undefined2 *)0x44e = unaff_DS;
  *(undefined1 *)0x457 = 0;
  *(undefined1 *)0x456 = 0;
  *(undefined1 *)0x458 = 0;
  if (*"\t" == '\0') {
    *(undefined1 *)((int)*(undefined4 *)0x6e2 + 1) = 0;
    *(undefined1 *)0x596 = 0xff;
  }
  else {
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 5;
  }
  return;
}



void __cdecl16far FUN_0008_e696(void)

{
  int unaff_DS;
  
  *(undefined2 *)0x582 = 0x586;
  *(int *)0x584 = unaff_DS;
  while (((int)*(undefined4 *)0x582 != 0x590 ||
         ((int)((ulong)*(undefined4 *)0x582 >> 0x10) != unaff_DS))) {
    *(undefined1 *)*(undefined4 *)0x582 = 0;
    *(int *)0x582 = *(int *)0x582 + 1;
  }
  if ((*"\t" != '\0') && (*(char *)0x580 != '\x01')) {
    func_0x00097905();
  }
  *(undefined1 *)0x581 = 0x80;
  return;
}



void __cdecl16far FUN_0008_e6e7(void)

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
    *"" = (char)((int)(bVar1 & 0x60) >> 5) + '\x01';
  }
  *(bool *)0x4ef = (*(byte *)*(undefined4 *)0x6da & 8) == 0;
  *(byte *)0x461 = (byte)((int)~(uint)*(byte *)*(undefined4 *)0x6da >> 4) & 3;
  return;
}



void __cdecl16far FUN_0008_e781(void)

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



void __cdecl16far FUN_0008_e800(void)

{
  char cVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  char *pcVar3;
  undefined1 local_b;
  
  pcVar3 = (char *)s_TRIggerd_0008_8ef8 + 8;
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
    if ((*"\t" == '\0') && (*"" != '\0')) {
      local_b = 2;
    }
  }
  else {
    local_b = 0;
  }
  *(undefined1 *)((int)*(undefined4 *)0x73a + 0x20) = local_b;
  func_0x000f0078(0xf007,uVar2,pcVar3);
  return;
}



void __cdecl16far FUN_0008_e8a2(void)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  if (*"r\x05" == '\0') {
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
    if ((*"\t" != '\0') && (*(int *)0x444 == 0 && *(int *)0x446 == 0)) {
      if ((*(byte *)0x4f1 & 0x20) == 0) {
        *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 3;
      }
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 2;
    }
  }
  if ((*"\t" == '\0') && ((*(byte *)0x45c & 1) != 0)) {
    if ((*(uint *)0x434 < *(uint *)0x43c) || (*"r\x05" != '\0')) {
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



void __cdecl16far FUN_0008_e99b(void)

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
    if (*"r\x05" == '\0') {
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
      goto LAB_0008_ea64;
    }
    *(undefined1 *)0x3 = 0;
  }
LAB_0008_ea64:
  if (((int)*(undefined4 *)0x438 == (int)*(undefined4 *)0x440) &&
     ((int)((ulong)*(undefined4 *)0x438 >> 0x10) == (int)((ulong)*(undefined4 *)0x440 >> 0x10))) {
    *(undefined1 *)0x2 = 0xff;
  }
  func_0x00097b94(unaff_CS,0);
  return;
}



void __stdcall16far FUN_0008_ea89(void)

{
  undefined2 unaff_DS;
  
  if (*"\t" == '\0') {
    func_0x0008009b();
  }
  else {
    func_0x00080060();
  }
  return;
}



void __cdecl16far FUN_0008_eaaf(void)

{
  undefined4 uVar1;
  undefined1 uVar2;
  undefined2 unaff_DS;
  
  func_0x000968a2();
  *(undefined1 *)0x5a3 = 0xff;
  uVar1 = *(undefined4 *)0x444;
  *(undefined2 *)0x440 = (int)uVar1;
  *(undefined2 *)0x442 = (int)((ulong)uVar1 >> 0x10);
  *(undefined1 *)0x452 = *(undefined1 *)0x453;
  *(undefined2 *)0x444 = 0;
  *(undefined2 *)0x446 = 0;
  *(undefined1 *)0x453 = 0;
  if (*"\t" == '\0') {
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
  if (((*"\t" != '\0') && (*"r\x05" == '\0')) && (*(int *)0x440 != 0 || *(int *)0x442 != 0)) {
    if ((*(byte *)0x4f1 & 0x20) == 0) {
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 3;
    }
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 2;
  }
  func_0x00097b94(0x9687,0);
  return;
}



void __cdecl16far FUN_0008_eb68(void)

{
  undefined1 uVar1;
  uint uVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  undefined2 uVar4;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x5a3 = 0xff;
  if (*(char *)0xc == '\0') {
    while ((*(char *)0x455 == '\0' && (*"" == '\0'))) {
      if (((int)*(undefined4 *)0x44c != (int)*(undefined4 *)0x448) ||
         ((int)((ulong)*(undefined4 *)0x44c >> 0x10) != (int)((ulong)*(undefined4 *)0x448 >> 0x10)))
      break;
      func_0x00096e5f(unaff_CS);
      func_0x00097b94(0x9687,0);
      unaff_CS = 0x9687;
      func_0x00096872(0x96f5,10);
      *(undefined1 *)0x5a3 = 0xff;
    }
    if (*"" == '\0') {
      uVar4 = unaff_CS;
      if ((*(char *)0x459 == '\0') && (*"\t" != '\0')) {
        *(undefined1 *)0x459 = 0xff;
      }
      else {
        if (*"\t" == '\0') {
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
        *(undefined1 *)0x459 = 0xff;
        if (*(char *)0x45a != '\0') {
          if (*"\t" == '\0') {
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



void __cdecl16far FUN_0008_ec5f(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (*(char *)0x459 != '\0') {
    if ((*"\t" == '\0') || ((*(byte *)0x4f1 & 0x20) == 0)) {
      if (*"\t" == '\0') {
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



void __cdecl16far FUN_0008_ecb5(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if ((*"\t" == '\0') && (*(char *)0x4ef != '\0')) {
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



void __cdecl16far FUN_0008_ece9(void)

{
  undefined4 uVar1;
  undefined2 unaff_DS;
  
  if ((((int)*(undefined4 *)0x438 != (int)*(undefined4 *)0x43c) ||
      ((int)((ulong)*(undefined4 *)0x438 >> 0x10) != (int)((ulong)*(undefined4 *)0x43c >> 0x10))) ||
     (*"r\x05" != '\0')) {
    uVar1 = *(undefined4 *)0x43c;
    *(undefined2 *)0x438 = (int)uVar1;
    *(undefined2 *)0x43a = (int)((ulong)uVar1 >> 0x10);
    *(undefined1 *)0x3 = 0xff;
    *(undefined1 *)0x2 = 0;
  }
  return;
}



void __cdecl16far FUN_0008_ed24(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x5a3 = 0xff;
  if (((*"\t" != '\0') && ((*(byte *)((int)*(undefined4 *)0x6d6 + 2) & 0x80) != 0)) ||
     ((*"\t" == '\0' && ((*(byte *)0x627 & 0x80) != 0)))) {
    if (*"\t" == '\0') {
      if ((*(byte *)0x627 & 0x40) == 0) {
        *(byte *)0x627 = *(byte *)0x627 & 0x7f;
      }
    }
    else {
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 7;
    }
    if (((*"\t" != '\0') && ((*(byte *)((int)*(undefined4 *)0x6d6 + 2) & 0x80) == 0)) ||
       ((*"\t" == '\0' && ((*(byte *)0x627 & 0x80) == 0)))) {
      *(undefined1 *)0x452 = 0;
      *(undefined1 *)0x453 = 0;
      *(undefined1 *)0x7 = 0xff;
      *(undefined1 *)0xe = 0xff;
    }
  }
  func_0x00097b94();
  return;
}



void __cdecl16far FUN_0008_edbe(void)

{
  undefined2 unaff_DS;
  
  if (*"\t" != '\0') {
    *(byte *)((int)*(undefined4 *)0x6d6 + 5) = *(byte *)0x57f & 0xbf | *(byte *)0x0 & 0x10;
  }
  return;
}



void __cdecl16far FUN_0008_ede5(void)

{
  undefined1 uVar1;
  undefined4 uVar2;
  undefined2 unaff_DS;
  
  func_0x000968a2();
  uVar1 = *(undefined1 *)0x5a3;
  *(undefined1 *)0x5a3 = 0xff;
  *(undefined1 *)0x3 = *(undefined1 *)0x1;
  *(undefined1 *)0x0 = *(undefined1 *)0x1;
  uVar2 = *(undefined4 *)0x444;
  *(undefined2 *)0x440 = (int)uVar2;
  *(undefined2 *)0x442 = (int)((ulong)uVar2 >> 0x10);
  *(undefined1 *)0x452 = *(undefined1 *)0x453;
  *(undefined2 *)0x444 = 0;
  *(undefined2 *)0x446 = 0;
  *(undefined1 *)0x1 = 0;
  *(undefined1 *)0x2 = 0;
  if (((*"\t" != '\0') && (*"r\x05" == '\0')) && (*(int *)0x440 != 0 || *(int *)0x442 != 0)) {
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 2;
  }
  func_0x00097b94(0x9687,uVar1);
  return;
}



void __cdecl16far FUN_0008_ee5f(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if ((*(byte *)((int)*(undefined4 *)0x6d6 + 2) & 2) != 0) {
    if (((*(char *)0x459 == '\0') && (*(char *)0x0 == '\0')) && ((*(byte *)0x4f1 & 0x40) == 0)) {
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



void __cdecl16far FUN_0008_eec6(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  char *pcVar3;
  
  pcVar3 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar2 = func_0x0009470e();
  if (*"\t" == '\0') {
    uVar1 = func_0x00097b94(38000,0xffff,pcVar3);
    func_0x00097c28(0x96f5,2);
    func_0x00097b94(0x96f5,uVar1);
    func_0x0009470e(0x96f5,uVar2);
  }
  return;
}



void __cdecl16far FUN_0008_ef0d(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  char *pcVar3;
  
  pcVar3 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar2 = func_0x0009470e();
  if (*"\t" == '\0') {
    uVar1 = func_0x00097b94(38000,0xffff,pcVar3);
    func_0x00097be6(0x96f5,2);
    func_0x00097b94(0x96f5,uVar1);
    func_0x0009470e(0x96f5,uVar2);
  }
  return;
}



int __cdecl16far FUN_0008_ef54(void)

{
  byte *pbVar1;
  byte bVar2;
  undefined1 uVar3;
  undefined2 uVar4;
  code cVar5;
  uint uVar6;
  undefined2 uVar7;
  char *pcVar8;
  undefined2 unaff_DS;
  char *pcVar9;
  char local_f;
  
  pcVar9 = (char *)s_TRIggerd_0008_8ef8 + 8;
  pcVar8 = (char *)0x9470;
  uVar4 = func_0x0009470e();
  local_f = '\0';
  if (*"\t" == '\0') {
    bVar2 = *(byte *)*(undefined4 *)0x6da;
    cVar5 = (code)(bVar2 & 0x80);
    if (*(code *)FUN_0008_03de != cVar5) {
      *(code *)FUN_0008_03de = cVar5;
      if (cVar5 == (code)0x0) {
        pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
        *pbVar1 = *pbVar1 | 10;
      }
      else {
        pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
        *pbVar1 = *pbVar1 & 0xfd;
      }
      pcVar8 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
      func_0x000839d1(38000,pcVar9);
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
      uVar7 = (undefined2)((ulong)*(undefined4 *)0x464 >> 0x10);
      uVar6 = (int)*(undefined4 *)0x464 + 4;
      if (0x4eb < uVar6) {
        uVar6 = 0x46c;
        uVar7 = unaff_DS;
      }
      *(uint *)0x464 = uVar6;
      *(undefined2 *)0x466 = uVar7;
      local_f = -1;
    }
    if (*(char *)0x596 == '\0') {
      if ((bVar2 & 4) != 0) {
        func_0x000800fc(pcVar8,0xffff);
        local_f = -1;
        pcVar8 = "";
      }
    }
    else if (((*(char *)0x45a == '\0') && (*"," == '\0')) && ((*(byte *)0x1a96 & 0xf) == 0)) {
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
      if ((*"$@<" == '\0') || (*(char *)0x45a == '\0')) {
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
    pcVar8 = "";
  }
  func_0x0009470e(pcVar8,uVar4);
  return (int)local_f;
}



void __cdecl16far FUN_0008_f0f9(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  char *pcVar2;
  
  pcVar2 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar1 = func_0x0009470e();
  if (*"\t" == '\0') {
    func_0x0009712b(38000,pcVar2,uVar1);
  }
  else {
    func_0x000975ac(38000);
  }
  func_0x0009470e(0x96f5,uVar1);
  return;
}



void __cdecl16far FUN_0008_f12b(void)

{
  byte bVar1;
  undefined2 uVar2;
  uint uVar3;
  undefined2 uVar4;
  char *pcVar5;
  char *pcVar6;
  char *pcVar7;
  undefined2 unaff_DS;
  byte local_f;
  
  pcVar6 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar2 = func_0x0009470e();
  pcVar5 = (char *)0x9470;
  while (pcVar7 = pcVar5, *"," != '\0') {
    if (((((int)*(undefined4 *)0x464 == (int)*(undefined4 *)0x468) &&
         ((int)((ulong)*(undefined4 *)0x464 >> 0x10) == (int)((ulong)*(undefined4 *)0x468 >> 0x10)))
        && ((*(char *)0x596 == '\0' || (*(char *)0x45a != '\0')))) && (*(char *)0x456 == '\0'))
    goto LAB_0008_f242;
    func_0x000838c0(pcVar5,pcVar6);
    pcVar5 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
  }
  while( true ) {
    if (((int)*(undefined4 *)0x464 == (int)*(undefined4 *)0x468) &&
       ((int)((ulong)*(undefined4 *)0x464 >> 0x10) == (int)((ulong)*(undefined4 *)0x468 >> 0x10)))
    break;
    bVar1 = *(byte *)((int)*(undefined4 *)0x468 + 1);
    local_f = bVar1 & 0x9f;
    if (local_f != 0) {
      pcVar5 = pcVar7;
      if ((bVar1 & 0xf) != 0) {
        pcVar5 = (char *)0x96f5;
        local_f = func_0x0009754c(pcVar7,local_f);
      }
      pcVar6 = pcVar5;
      if ((local_f & 0x80) != 0) {
        pcVar6 = (char *)0x96f5;
        func_0x00097253(pcVar5);
      }
      pcVar7 = pcVar6;
      if ((local_f & 0x10) != 0) {
        pcVar7 = (char *)0x96f5;
        func_0x00097583(pcVar6);
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
     ((pcVar5 = pcVar7, *(char *)0x455 != '\0' && (*(char *)0x456 != '\0')))) {
    pcVar5 = (char *)0x96f5;
    func_0x00097431(pcVar7);
  }
LAB_0008_f242:
  *(undefined1 *)0x5a1 = 0;
  func_0x0009470e(pcVar5,uVar2);
  return;
}



void __cdecl16far FUN_0008_f253(void)

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
    func_0x0009470e(38000,(char *)s_TRIggerd_0008_8ef8 + 8,uVar2);
  }
  if ((*(byte *)((int)*(undefined4 *)0x468 + 2) & 2) == 0) {
    return;
  }
  if (((*(byte *)((int)*(undefined4 *)0x468 + 3) ^ 0xff) & 0x80) == 0) {
    return;
  }
  local_9 = *(byte *)*(undefined4 *)0x468;
  if (*"" != '\0') {
    if (*"" == '\x04') {
      if ((local_9 & 0x80) != 0) {
        return;
      }
    }
    else if ((*"" == '\x03') && ((local_9 & 0x80) == 0)) {
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
      *(int *)0x444 = local_e;
      *(int *)0x446 = iVar1;
    }
  }
  *(byte *)0x462 = local_9;
  *(int *)0x434 = local_e;
  *(int *)0x436 = iVar1;
  local_10 = *(int *)0x434 - *(int *)0x43c;
  if ((local_10 < 0) || (*"r\x05" != '\0')) {
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



void __cdecl16far FUN_0008_f431(void)

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



void __stdcall16far FUN_0008_f4e1(byte param_1)

{
  undefined2 unaff_DS;
  
  if ((*"" != '\0') && (param_1 = param_1 & 0x7f, *"" == '\x03')) {
    param_1 = param_1 | 0x80;
  }
  *(byte *)*(undefined4 *)0x6d6 = param_1;
  *(undefined1 *)0x45a = 0;
  func_0x000800fc();
  return;
}



undefined1 __cdecl16far FUN_0008_f51a(void)

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



byte __stdcall16far FUN_0008_f54c(byte param_1)

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



void __cdecl16far FUN_0008_f583(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x45a != '\0') {
    func_0x00097431();
  }
  return;
}



void __cdecl16far FUN_0008_f5ac(void)

{
  byte bVar1;
  undefined2 uVar2;
  char *pcVar3;
  char *pcVar4;
  char *pcVar5;
  undefined2 unaff_DS;
  
  pcVar3 = (char *)s_TRIggerd_0008_8ef8 + 8;
  uVar2 = func_0x0009470e();
  pcVar4 = (char *)0x9470;
  while ((*"," != '\0' && (*(char *)0x5a1 != '\0'))) {
    func_0x000838c0(pcVar4,pcVar3);
    pcVar4 = (char *)s_8TEK_2230_SYS_0_0_COMM_0_0_0008_8231 + 0x18;
  }
  if (*(char *)0x5a1 != '\0') {
    while ((*(byte *)0x598 & 0xc0) != 0) {
      if ((*(byte *)0x598 & 0x80) != 0) {
        pcVar3 = pcVar4;
        if ((*(byte *)0x598 & 0x20) != 0) {
          pcVar3 = (char *)0x96f5;
          func_0x000976c0(pcVar4);
        }
        pcVar4 = pcVar3;
        if ((*(byte *)0x598 & 0x10) != 0) {
          pcVar4 = (char *)0x96f5;
          func_0x0009785b(pcVar3);
        }
        pcVar3 = pcVar4;
        if ((*(byte *)0x598 & 4) != 0) {
          pcVar3 = (char *)0x96f5;
          func_0x00097905(pcVar4);
        }
        pcVar5 = pcVar3;
        if ((*(byte *)0x598 & 2) != 0) {
          pcVar5 = (char *)0x96f5;
          func_0x00097a16(pcVar3);
        }
        pcVar4 = pcVar5;
        if ((*(byte *)0x598 & 1) != 0) {
          pcVar4 = (char *)0x96f5;
          func_0x00097a9c(pcVar5);
        }
      }
      if ((*(byte *)0x598 & 0x40) != 0) {
        pcVar3 = pcVar4;
        if ((*(byte *)0x599 & 4) != 0) {
          pcVar3 = (char *)0x96f5;
          func_0x00097a57(pcVar4);
        }
        pcVar5 = pcVar3;
        if ((*(byte *)0x599 & 8) != 0) {
          pcVar5 = (char *)0x96f5;
          func_0x00097b01(pcVar3);
        }
        pcVar4 = pcVar5;
        if ((*(byte *)0x599 & 1) != 0) {
          pcVar4 = (char *)0x96f5;
          func_0x00097a9c(pcVar5);
        }
        *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 1;
      }
      bVar1 = *(byte *)*(undefined4 *)0x6d6;
      *(byte *)0x598 = bVar1;
      if ((bVar1 & 0x40) != 0) {
        *(undefined1 *)0x599 = *(undefined1 *)((int)*(undefined4 *)0x6d6 + 1);
      }
    }
    pcVar3 = pcVar4;
    if ((*"$@<" != '\0') && (*(char *)0x45a != '\0')) {
      pcVar3 = (char *)0x96f5;
      func_0x0009785b(pcVar4);
    }
    pcVar4 = "";
    func_0x00080060(pcVar3,0);
  }
  func_0x0009470e(pcVar4,uVar2);
  return;
}



void __cdecl16far FUN_0008_f6c0(void)

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
      *(undefined2 *)0x444 = (int)uVar3;
      *(undefined2 *)0x446 = (int)((ulong)uVar3 >> 0x10);
    }
  }
  if ((*(byte *)0x4f1 & 0x20) == 0) {
    if ((((int)*(undefined4 *)0x434 == (int)*(undefined4 *)0x440) &&
        ((int)((ulong)*(undefined4 *)0x434 >> 0x10) == (int)((ulong)*(undefined4 *)0x440 >> 0x10)))
       && (*"r\x05" == '\0')) {
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 2;
    }
  }
  else if ((*(int *)0x444 == 0 && *(int *)0x446 == 0) && (*"r\x05" == '\0')) {
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 2;
  }
  return;
}



void __cdecl16far FUN_0008_f85b(void)

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



void __cdecl16far FUN_0008_f905(void)

{
  byte *pbVar1;
  undefined1 uVar2;
  undefined2 unaff_DS;
  
  if (*"" == '\0') {
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
    if (*"\t" != '\0') {
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
    if (*"\t" != '\0') {
      *(undefined1 *)((int)*(undefined4 *)0x6d6 + 3) = 0x18;
      *(byte *)((int)*(undefined4 *)0x6d6 + 5) = *(byte *)0x57f | *(byte *)0x0 & 0x10;
    }
    pbVar1 = (byte *)((int)*(undefined4 *)0x732 + 0x97);
    *pbVar1 = *pbVar1 & 0xfb;
  }
  func_0x000839d1();
  return;
}



void __cdecl16far FUN_0008_fa16(void)

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



void __cdecl16far FUN_0008_fa57(void)

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



void __cdecl16far FUN_0008_fa9c(void)

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



void __cdecl16far FUN_0008_fb01(void)

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
    func_0x0009470e(0xe6a8,(char *)s_TRIggerd_0008_8ef8 + 8);
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
  func_0x0009470e(38000,(char *)s_TRIggerd_0008_8ef8 + 8);
  if (*"" == '\x16') {
    func_0x000850a2(38000);
  }
  else {
    *(undefined1 *)0x5a4 = 0;
  }
  return;
}



char __stdcall16far FUN_0008_fb94(int param_1)

{
  char cVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  cVar1 = *",";
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



void __stdcall16far FUN_0008_fbe6(byte param_1)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x45c == '\0') {
    *(byte *)0x59c = *(byte *)0x59c & 0xfe;
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 2) = *(undefined1 *)0x59c;
    if ((*(char *)0x45b != '\0') && (*(byte *)0x460 = *(byte *)0x460 | 2, *(char *)0x45a != '\0')) {
      func_0x00097431();
    }
  }
  *(byte *)0x45c = *(byte *)0x45c | param_1;
  return;
}



void __stdcall16far FUN_0008_fc28(byte param_1)

{
  undefined2 unaff_DS;
  
  *(byte *)0x45c = *(byte *)0x45c & ~param_1;
  if (*(char *)0x45c == '\0') {
    *(byte *)0x59c = *(byte *)0x59c | 1;
    *(undefined1 *)((int)*(undefined4 *)0x6d6 + 2) = *(undefined1 *)0x59c;
    if ((*(char *)0x45b != '\0') && (*(byte *)0x460 = *(byte *)0x460 | 1, *(char *)0x45a != '\0')) {
      func_0x00097431();
    }
  }
  return;
}



uint __stdcall16far FUN_0008_fc6e(uint param_1)

{
  undefined2 unaff_DS;
  
  return *(uint *)0x1e6 & param_1;
}



void __stdcall16far FUN_0008_fc80(uint param_1)

{
  undefined2 unaff_DS;
  
  *(uint *)0x1e6 = *(uint *)0x1e6 & ~param_1;
  return;
}



void __stdcall16far FUN_0008_fc95(uint param_1)

{
  undefined2 unaff_DS;
  
  *(uint *)0x1e6 = *(uint *)0x1e6 | param_1;
  return;
}



void __stdcall16far FUN_0008_fca8(byte *param_1,undefined4 param_2,byte param_3)

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


