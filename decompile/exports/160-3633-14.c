
/* plot_point_with_flag_update (confidence: Mechanism confirmed; exact purpose of the
   `scale_and_plot_point_default` bracketing not confirmed)
   
   Evidence: Calls `scale_and_plot_point_default(0x8F80)`, conditionally ORs an arg into `[0x61B]`,
   then calls `scale_and_plot_point_default` again with the first result - see
   `docs/comm-rom/rs232-flow-control-and-open-puzzle.md`'s "Open puzzle" for the shared bracketing
   shape also seen in `compute_parity_mode_code` */

void __stdcall16far plot_point_with_flag_update(byte param_1)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  uVar2 = 0x8f80;
  uVar1 = func_0x000f0078();
  if (*(char *)0x61a != '\0') {
    *(byte *)0x61b = *(byte *)0x61b | param_1;
  }
  func_0x000f0078(0xf007,uVar1,uVar2);
  return;
}



void __cdecl16far FUN_000e_007d(void)

{
  return;
}



/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* boot_init (confidence: Confirmed (proven boot path))
   
   Evidence: Early boot: `cli`, clear a RAM buffer, set up the first stack (`SS:SP=4000:3FFA`),
   branch on a config/option byte at `cs:[bx+0x1AF]` */

void boot_init(void)

{
  byte *pbVar1;
  undefined1 *puVar2;
  byte bVar3;
  uint in_CX;
  byte bVar4;
  byte bVar5;
  char cVar6;
  int iVar7;
  uint uVar8;
  byte *pbVar9;
  byte *pbVar10;
  undefined2 uVar11;
  
  DAT_4000_07ce = 0;
  bVar4 = 0;
  bVar5 = 0;
  do {
    if (*(char *)(bVar4 + 0x1af) == -1) {
      uRam00001ef4 = (uint)bVar5;
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    }
    iVar7 = (uint)bVar4 * 4;
    pbVar9 = (byte *)(byte *)*(undefined4 *)(iVar7 + 0x155);
    uVar11 = (undefined2)((ulong)*(undefined4 *)(iVar7 + 0x17d) >> 0x10);
    pbVar10 = (byte *)((int)*(undefined4 *)(iVar7 + 0x17d) + 1);
    bVar3 = *(byte *)(CONCAT11((char)((uint)iVar7 >> 8),bVar4) + 0x1af);
    if (1 < bVar3) {
LAB_000e_0116:
      bVar3 = 0xfd;
LAB_000e_011b:
      pbVar1 = pbVar9;
      pbVar9 = pbVar9 + 1;
      *pbVar1 = bVar3;
      if (pbVar10 != pbVar9) break;
      iVar7 = 0x56d;
      do {
        iVar7 = iVar7 + -1;
      } while (iVar7 != 0);
      pbVar1 = (byte *)*(undefined4 *)((uint)bVar4 * 4 + 0x155);
      uVar11 = (undefined2)((ulong)pbVar1 >> 0x10);
      pbVar9 = (byte *)pbVar1;
      if (*(char *)(bVar4 + 0x1af) == '\x02') {
        uVar8 = 0xf0f;
      }
      else if (*(char *)(bVar4 + 0x1af) == '\x03') {
        uVar8 = 0xf0f0;
      }
      else {
        uVar8 = 0xffff;
      }
      do {
        cVar6 = -3;
        do {
          in_CX = CONCAT11(cVar6,*pbVar9) & uVar8;
          if ((char)(in_CX >> 8) != (char)in_CX) {
            while( true ) {
              pbVar1 = pbVar9;
              pbVar9 = pbVar9 + 1;
              *pbVar1 = 0;
              if (pbVar10 == pbVar9) break;
              bVar3 = (char)uVar8 - 1;
              uVar8 = (uint)bVar3;
              if (bVar3 == 0) {
                in_CX = _DAT_4000_3ffa;
              }
            }
            goto LAB_000e_01d4;
          }
          pbVar1 = pbVar9;
          pbVar9 = pbVar9 + 1;
          *pbVar1 = 0;
          if (pbVar10 == pbVar9) goto LAB_000e_01cb;
          cVar6 = cVar6 + -1;
        } while (cVar6 != '\0');
      } while( true );
    }
    if (bVar3 != 1) {
      in_CX = 0;
      pbVar9 = pbVar9 + 2;
    }
LAB_000e_0195:
    cVar6 = -0x80;
    while( true ) {
      pbVar1 = pbVar9;
      pbVar9 = pbVar9 + 1;
      in_CX = in_CX * 2 + (uint)*pbVar1 + (uint)((int)in_CX < 0);
      if (pbVar9 == pbVar10) break;
      cVar6 = cVar6 + -1;
      if (cVar6 == '\0') goto LAB_000e_0195;
    }
    if (bVar3 != 1) goto LAB_000e_01cf;
    puVar2 = (undefined1 *)*(undefined4 *)((bVar4 - 1) * 4 + 0x155);
    if (in_CX == CONCAT11(*puVar2,((undefined1 *)puVar2)[1])) {
LAB_000e_01cb:
      bVar5 = bVar5 >> 1;
LAB_000e_01cf:
      bVar4 = bVar4 + 1;
    }
    else {
LAB_000e_01d4:
      bVar4 = bVar4 + 1;
      DAT_4000_07ce = bVar4 | bVar4 * '\x10';
      bVar5 = (byte)((bVar5 | 0x100) >> 1);
      iVar7 = 0x4000;
      do {
        iVar7 = iVar7 + 1;
      } while (iVar7 != 0);
      bVar4 = bVar4 & 0xf;
    }
  } while( true );
  bVar3 = bVar3 - 1;
  if (bVar3 == 0) goto LAB_000e_0116;
  goto LAB_000e_011b;
}



void __stdcall16far FUN_000e_027e(int param_1)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  if (param_1 == 1) {
    *(undefined1 *)0x1afd = 0;
  }
  else if (*(char *)0x1afd == '\0') {
    *(undefined1 *)0x1afd = 1;
  }
  if (param_1 == 3) {
    if (10 < *(int *)0x52a) {
      *(int *)0x1b60 = *(int *)0x1b60 + 1;
    }
    if (*(int *)0x52a < -10) {
      *(int *)0x1b60 = *(int *)0x1b60 + -1;
    }
    if (*(int *)0x1b60 < 2) {
      iVar1 = 1;
    }
    else {
      iVar1 = *(int *)0x1b60;
    }
    if (iVar1 < 0x40) {
      if (*(int *)0x1b60 < 2) {
        uVar2 = 1;
      }
      else {
        uVar2 = *(undefined2 *)0x1b60;
      }
    }
    else {
      uVar2 = 0x40;
    }
    *(undefined2 *)0x1b60 = uVar2;
    func_0x000e3395();
    func_0x000e323f(0xe31d,0x476);
    *(undefined2 *)0x1b62 = (int)(0x40 / (long)*(int *)0x1b60);
  }
  return;
}



void __stdcall16far FUN_000e_0324(int param_1)

{
  int iVar1;
  undefined2 in_BX;
  undefined2 unaff_ES;
  undefined2 unaff_CS;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined1 local_34 [42];
  undefined2 local_a;
  
  if (param_1 == 1) {
    *(undefined1 *)0x1afd = 0;
  }
  else if ((param_1 != 2) && (param_1 == 4)) {
    if (*(char *)0x1afd == '\0') {
      *(undefined1 *)0x1afd = 1;
      unaff_ES = (undefined2)((ulong)*(undefined4 *)0x1c80 >> 0x10);
      unaff_CS = 0xe026;
      func_0x000e041d();
      *(undefined2 *)0x1b0c = *(undefined2 *)0x1b60;
    }
    func_0x000fe74a(unaff_CS);
    local_a = unaff_ES;
    func_0x000e3169(0xfe74,0x1000,0,0x1000,(int)*(undefined4 *)0x1c80,
                    (int)((ulong)*(undefined4 *)0x1c80 >> 0x10),in_BX,unaff_ES,*(undefined2 *)0x1b62
                   );
    iVar1 = *(int *)0x1b0c;
    *(int *)0x1b0c = *(int *)0x1b0c + -1;
    if (iVar1 == 0) {
      *(undefined2 *)0x1b0c = *(undefined2 *)0x1b60;
      local_34[0] = 0;
      uVar2 = 0xe026;
      iVar1 = func_0x000e0599(0xe316,(int)*(undefined4 *)0x1c80,
                              (int)((ulong)*(undefined4 *)0x1c80 >> 0x10));
      if (iVar1 != 0) {
        uVar2 = 0xe31d;
        func_0x000e31dc(0xe026,local_34);
      }
      *(undefined2 *)0x1b18 = 1;
      func_0x000e3599(uVar2,(int)*(undefined4 *)0x1ddc + 0x291,
                      (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
      func_0x000e07b4(0xe352,0x810,0xff7b,0x811,0xff7b,local_34);
      func_0x000e041d(0xe06b,(int)*(undefined4 *)0x1c80,(int)((ulong)*(undefined4 *)0x1c80 >> 0x10))
      ;
    }
  }
  return;
}



void __stdcall16far FUN_000e_041d(undefined4 param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 local_a;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  uVar1 = (undefined2)param_1;
  func_0x000e35c2();
  func_0x000e3a3b(0xe352,0,0,uVar1,uVar2);
  for (local_a = 1; local_a < 0x100; local_a = local_a + 1) {
    func_0x000e3a3b(0xe352,0,9);
  }
  func_0x000e371e(0xe352,(int)*(undefined4 *)0x1ddc + 0x20a,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
  return;
}



int __stdcall16far FUN_000e_048b(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  int iVar5;
  int iVar6;
  int local_10;
  undefined4 local_e;
  int local_a;
  
  local_a = 0;
  iVar6 = 0;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  local_e = (uint *)CONCAT22(uVar4,(uint *)((int)param_1 + 200));
  for (local_10 = 0; local_10 < 0x20; local_10 = local_10 + 1) {
    iVar6 = iVar6 + *local_e / *(uint *)0x1b62;
    local_e = (uint *)CONCAT22(local_e._2_2_,(uint *)local_e + 1);
  }
  iVar1 = *(int *)0x1b62 << 5;
  iVar2 = func_0x000e05ef();
  local_e = (uint *)CONCAT22(uVar4,(int *)((int)param_1 + 2));
  for (local_10 = 1; local_10 < 0xff; local_10 = local_10 + 1) {
    iVar5 = iVar2;
    iVar3 = func_0x000e05ef(0xe05c,*local_e << 2,0x6400,iVar2,iVar1,iVar2,iVar6);
    iVar3 = 100 - iVar3 / iVar2;
    if (iVar3 < 1) {
      iVar3 = -iVar3;
    }
    if (iVar3 < local_a) {
      iVar3 = local_a;
    }
    local_e = (uint *)CONCAT22(local_e._2_2_,(uint *)local_e + 1);
    iVar2 = iVar5;
    local_a = iVar3;
  }
  return local_a;
}



void __stdcall16far FUN_000e_0564(undefined4 param_1,byte *param_2)

{
  int *piVar1;
  int iVar2;
  byte *pbVar3;
  int iVar4;
  undefined2 unaff_DS;
  
  iVar2 = *(int *)0x1b62;
  for (iVar4 = 0; pbVar3 = param_2, iVar4 < 0x1000; iVar4 = iVar4 + 1) {
    param_2 = (byte *)CONCAT22(param_2._2_2_,(byte *)param_2 + 1);
    piVar1 = (int *)((int)param_1 + (uint)*pbVar3 * 2);
    *piVar1 = *piVar1 + iVar2;
  }
  return;
}



undefined2 __stdcall16far FUN_000e_0599(undefined4 param_1)

{
  int local_a;
  
  local_a = 0;
  while( true ) {
    if (0xff < local_a) {
      return 0;
    }
    if (*(int *)((int)param_1 + local_a * 2) == 0) break;
    local_a = local_a + 1;
  }
  return 1;
}



int __stdcall16far FUN_000e_05ef(int param_1,int param_2)

{
  int iVar1;
  
  iVar1 = (int)((ulong)((long)param_1 * (long)param_2) >> 0x10);
  if (((uint)((long)param_1 * (long)param_2) | 0x80) == 0) {
    iVar1 = iVar1 + 1;
  }
  return iVar1;
}



/* verify_prc_readback_pattern (confidence: Mechanism confirmed)
   
   Evidence: A 2-phase state machine (`[0x1B5F]`) comparing readback bits from `[0x322]` (comm probe
   `0x40377E`) then `[0x1D20]` against expected pattern state; returns `0x20` on verify - the
   verification half of `PRC_READBACK`, paired with
   `init_selftest_register_group`/`step_progress_pattern_*` */

undefined1 __cdecl16far verify_prc_readback_pattern(void)

{
  undefined1 uVar1;
  byte bVar2;
  undefined2 unaff_DS;
  undefined1 local_c;
  
  local_c = 0;
  if (*(char *)0x1b5f == '\x01') {
    if (((*(byte *)0x258 & 0x80) != 0) && ((*(uint *)*(undefined4 *)0x322 & 0x8000) == 0)) {
      *(uint *)0x1b1e = *(uint *)*(undefined4 *)0x322;
      *(byte *)0x257 = *(byte *)0x257 & 0x1f;
      uVar1 = *(undefined1 *)0x257;
      *(undefined1 *)0x258 = uVar1;
      *(undefined1 *)*(undefined4 *)0x336 = uVar1;
      local_c = 0x20;
      *(undefined1 *)0x1b5f = 0;
    }
  }
  else if (((*(char *)0x1b5f == '\x02') &&
           (bVar2 = *(byte *)((int)*(undefined4 *)0x1d20 + 5), (*(uint *)0x1b1c & 1) == 0)) &&
          ((bVar2 & 4) != 0)) {
    *(uint *)0x1b1e = (uint)bVar2;
    *(uint *)0x1b1c = *(uint *)0x1b1c | 1;
    *(undefined1 *)*(undefined4 *)0x1d20 = *(undefined1 *)0x1b1c;
    local_c = 0x20;
    *(undefined1 *)0x1b5f = 0;
  }
  return local_c;
}



/* update_menu_position (confidence: Confirmed mechanism; exact op-code meanings and which physical
   control feeds `[0x4E7]`/`[0x4E8]` not confirmed)
   
   Evidence: `(min, max, op_nibble)` - a bounded index/cursor tracker at `[0x1B50]` with clamping
   and wraparound between `min`/`max`. The op nibble selects init/cancel/increment variants; the
   increment path reads two front-panel button-state bytes (`[0x4E7]`/`[0x4E8]`, bit 7) and
   accelerates if both are set simultaneously. A menu/parameter-selection cursor - see
   `VARIABLES.md` */

uint __stdcall16far update_menu_position(word min,word max,word op_nibble)

{
  uint uVar1;
  undefined2 unaff_DS;
  
  uVar1 = op_nibble & 0xf;
  if (uVar1 == 1) {
    *(char *)0x1b50 = (char)min + -1;
    *(undefined1 *)0x1b51 = 1;
  }
  else if (uVar1 != 2) {
    if ((uVar1 == 3) || (uVar1 == 4)) {
      if ((op_nibble & 0xf0) == 0) {
        if (*(char *)0x1b51 == '\0') {
          *(undefined2 *)0x1b18 = 1;
        }
        else {
          *(char *)0x1b50 = *(char *)0x1b50 + '\x01';
        }
        if (((*(byte *)0x4e7 & 0x80) != 0) && ((*(byte *)0x4e8 & 0x80) != 0)) {
          *(undefined1 *)0x1b51 = 0;
          *(char *)0x1b50 = *(char *)0x1b50 + '\x01';
        }
      }
      else {
        *(char *)0x1b50 = *(char *)0x1b50 + '\x01';
      }
    }
    if ((int)max < (int)(uint)*(byte *)0x1b50) {
      *(char *)0x1b50 = (char)min;
      *(undefined2 *)0x1b18 = 1;
    }
    if ((int)(uint)*(byte *)0x1b50 < (int)min) {
      *(undefined1 *)0x1b50 = (undefined1)max;
    }
    return (uint)*(byte *)0x1b50;
  }
  return 0xfffe;
}



void __stdcall16far FUN_000e_0770(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x476 = 0;
  return;
}



void __stdcall16far FUN_000e_0781(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x476 = 0;
  return;
}



void __stdcall16far FUN_000e_0792(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x476 = 0;
  return;
}



void __stdcall16far FUN_000e_07a3(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x476 = 0;
  return;
}



/* print_selftest_report_line (confidence: Confirmed mechanism; the exact report-line layout not
   fully mapped)
   
   Evidence: The outer routine that wraps `print_selftest_banner`; computes a row Y coordinate from
   `[0x1B10]*0x32` and prints one self-test report line per call (via `SUB_E35C2`,
   `plot_readout_point`, `draw_readout_char`, and the other print primitives) */

void __stdcall16far
print_selftest_report_line(char *param_1,undefined4 param_2,undefined4 param_3,undefined1 param_4)

{
  undefined4 uVar1;
  undefined2 uVar2;
  undefined2 unaff_CS;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 unaff_DS;
  int local_e;
  int local_c;
  undefined2 uStack_a;
  undefined2 uVar3;
  undefined2 uVar4;
  
  if (*(char *)0x1b7a == '\x01') {
    uVar1 = *(undefined4 *)0x1ddc;
    *(int *)0x1b52 = (int)uVar1 + 0x2ac;
    *(undefined2 *)0x1b54 = (int)((ulong)uVar1 >> 0x10);
    uStack_a = (undefined2)((ulong)*(undefined4 *)0x1c80 >> 0x10);
    local_c = (int)*(undefined4 *)0x1c80 + 0x800;
    local_e = 0x7f;
  }
  else {
    local_c = 0;
    uStack_a = 0;
    if (*(int *)0x1b10 == 0) {
      func_0x000e416f();
      unaff_CS = 0xe06b;
      func_0x000e094b(0xe3b1);
      uVar1 = *(undefined4 *)0x1ddc;
      *(int *)0x1b52 = (int)uVar1 + 0x21c;
      *(undefined2 *)0x1b54 = (int)((ulong)uVar1 >> 0x10);
    }
    else if (0xf < *(int *)0x1b10) {
      return;
    }
    local_e = *(int *)0x1b10 * -0x32 + 0x339;
    if ((*(int *)0x1b18 == 1) || (*(char *)0x1b48 == '\x03')) {
      *(int *)0x1b10 = *(int *)0x1b10 + 1;
    }
  }
  uVar5 = unaff_CS;
  if (*(int *)0x1b18 == 1) {
    uVar5 = 0xe06b;
    func_0x000e0af5(unaff_CS,0x7ca,0xff7b);
  }
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  uVar3 = (undefined2)((ulong)param_2 >> 0x10);
  uVar4 = (undefined2)((ulong)param_3 >> 0x10);
  if (((*(int *)0x1b18 == 1) || (*(char *)0x1b48 == '\x03')) && (*"" != '\x14')) {
    *(undefined2 *)0x1b18 = 0;
    func_0x000e35c2(uVar5,(int)*(undefined4 *)0x1b52,(int)((ulong)*(undefined4 *)0x1b52 >> 0x10),0,
                    0x10,local_e,0x10,local_c,uStack_a);
    func_0x000e3930(0xe352,0,0,0);
    func_0x000e3854(0xe352,0x20);
    func_0x000e3821(0xe352,(char *)param_1,uVar2);
    if (*param_1 != '\0') {
      func_0x000e3821(0xe352,0x7cd,0xff7b);
    }
    func_0x000e3821(0xe352,(int)param_2,uVar3);
    func_0x000e3821(0xe352,(int)param_3,uVar4);
    uVar1 = *(undefined4 *)0x1b52;
    *(int *)0x1b52 = *(int *)0x1b52 + 9;
    uVar5 = 0xe352;
    func_0x000e374e(0xe352,(int)uVar1,(int)((ulong)uVar1 >> 0x10));
  }
  uVar6 = uVar5;
  if (*(char *)0x1b7a != '\x01') {
    uVar6 = 0xe06b;
    func_0x000e097b(uVar5,param_4);
  }
  func_0x000e0af5(uVar6,(char *)param_1,uVar2);
  if (*param_1 != '\0') {
    func_0x000e0af5(0xe06b,0x7d1,0xff7b);
  }
  func_0x000e0af5(0xe06b,(int)param_2,uVar3);
  func_0x000e0af5(0xe06b,(int)param_3,uVar4);
  func_0x000e0af5(0xe06b,0x7d5,0xff7b);
  return;
}



/* init_selftest_report_record (confidence: Confirmed)
   
   Evidence: Writes a fixed 3-byte header record (`3, 2, 0`) into `[0x1B56]`/`[0x1B58]`; called
   once, right after `print_selftest_banner`, from `print_selftest_report_line`'s first-call setup -
   seeds the initial readout display-list entry for the self-test report */

void __cdecl16far init_selftest_report_record(void)

{
  undefined4 uVar1;
  undefined2 unaff_DS;
  
  uVar1 = *(undefined4 *)0x1c80;
  *(undefined2 *)0x1b56 = (int)uVar1;
  *(undefined2 *)0x1b58 = (int)((ulong)uVar1 >> 0x10);
  *(undefined1 *)*(undefined4 *)0x1b56 = 3;
  *(undefined1 *)((int)*(undefined4 *)0x1b56 + 1) = 2;
  *(undefined1 *)((int)*(undefined4 *)0x1b56 + 2) = 0;
  return;
}



/* append_selftest_report_char (confidence: Confirmed)
   
   Evidence: `(char)` - appends to the self-test report record at `[0x1B56]`, nibble-packing into
   the previous byte when both share a nonzero high nibble, else appending normally and
   null-terminating */

void __stdcall16far append_selftest_report_char(byte char)

{
  byte *pbVar1;
  undefined2 unaff_DS;
  
  if (((*(byte *)*(undefined4 *)0x1b56 < 4) || ((char & 0xf0) == 0)) ||
     ((*(byte *)((uint)*(byte *)*(undefined4 *)0x1b56 + (int)*(undefined4 *)0x1b56 + -1) & 0xf0) !=
      (char & 0xf0))) {
    *(byte *)((uint)*(byte *)*(undefined4 *)0x1b56 + (int)*(undefined4 *)0x1b56) = char;
    *(char *)*(undefined4 *)0x1b56 = *(char *)*(undefined4 *)0x1b56 + '\x01';
    *(undefined1 *)((uint)*(byte *)*(undefined4 *)0x1b56 + (int)*(undefined4 *)0x1b56) = 0;
  }
  else {
    pbVar1 = (byte *)((uint)*(byte *)*(undefined4 *)0x1b56 + (int)*(undefined4 *)0x1b56 + -1);
    *pbVar1 = *pbVar1 | char;
  }
  return;
}



/* scroll_selftest_report_display (confidence: Confirmed)
   
   Evidence: Called periodically; decrements a countdown at report-record offset 2, and on expiry
   either blanks a channel display register (`write_measurement_channel_reg(0xFF)`) or writes the
   next character of the report string to it - scrolls the self-test report text across a small
   channel/status display */

void __cdecl16far scroll_selftest_report_display(void)

{
  char *pcVar1;
  char cVar2;
  byte bVar3;
  byte bVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 unaff_DS;
  
  uVar6 = (undefined2)((ulong)*(undefined4 *)0x1b56 >> 0x10);
  iVar5 = (int)*(undefined4 *)0x1b56;
  cVar2 = *(char *)(iVar5 + 2);
  pcVar1 = (char *)(iVar5 + 2);
  *pcVar1 = *pcVar1 + -1;
  if (cVar2 == '\0') {
    *(undefined1 *)((int)*(undefined4 *)0x1b56 + 2) = 0x32;
    if (*(char *)((int)*(undefined4 *)0x1b56 + 1) == '\x02') {
      pcVar1 = (char *)((int)*(undefined4 *)0x1b56 + 1);
      *pcVar1 = *pcVar1 + '\x01';
      func_0x000e0aaa();
    }
    else {
      func_0x000e0aaa();
      uVar6 = (undefined2)((ulong)*(undefined4 *)0x1b56 >> 0x10);
      iVar5 = (int)*(undefined4 *)0x1b56;
      bVar3 = *(byte *)*(undefined4 *)0x1b56;
      bVar4 = *(byte *)(iVar5 + 1);
      pcVar1 = (char *)(iVar5 + 1);
      *pcVar1 = *pcVar1 + '\x01';
      if (bVar3 < bVar4) {
        *(undefined1 *)((int)*(undefined4 *)0x1b56 + 1) = 2;
      }
    }
  }
  return;
}



/* write_measurement_channel_reg (confidence: Confirmed mechanism)
   
   Evidence: Writes the byte argument into the shared hardware register `[0x33A]` - a simple setter
   for the same `configure_measurement_hw` register cluster */

void __stdcall16far write_measurement_channel_reg(undefined1 param_1)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)*(undefined4 *)0x33a = param_1;
  return;
}



void __stdcall16far FUN_000e_0ac0(int param_1)

{
  undefined2 unaff_CS;
  
  for (; 0 < param_1; param_1 = param_1 + -1) {
    func_0x000e0add(unaff_CS);
    unaff_CS = 0xe06b;
  }
  return;
}



/* wait_readout_tick (confidence: Confirmed mechanism; what increments `[0x752]` not yet found)
   
   Evidence: Busy-waits for `[0x752]` (a counter, presumably interrupt-incremented) to change value
   - throttles `print_string_far`'s per-character loop to the readout hardware's real pace */

void __cdecl16far wait_readout_tick(void)

{
  undefined2 unaff_DS;
  
  do {
  } while (*(int *)0x752 == *(int *)0x752);
  return;
}



/* print_string_far (confidence: Confirmed (including the newly-found suppress gate))
   
   Evidence: `(str_off, str_seg)` - loops a far-pointer nul-terminated string byte-by-byte, calling
   `print_char` per byte (each preceded by `wait_readout_tick`, throttling to the readout hardware's
   real pace). Every observed call site passes a far pointer into segment `0xFF7B` (landing in
   `160-3532`), i.e. a fixed string table. **Found 2026-09-18**: checks `[0x1B48]==0` on entry and
   returns *immediately* (`retf 4`) if so - printing literally nothing, not even a partial string -
   the same flag `print_boot_rom_id_banner` also checks itself before ever calling this.
   `[0x1B48]==0` occurs exactly when the momentary front-panel button SELECT C1/C2 is held alone (no
   `MEM1`/`2`/`3`/`MENU ADV`) at the moment `[0x758]` gets sampled early in boot (see
   `JUMP_MAP.md`'s boot-sequence diagram) - this is the confirmed, disassembly-level mechanism
   behind a live investigation into why holding that button suppresses all self-test diagnostic text
   in the emulator, though it directly contradicts the service manual's own description of that
   button ("invoking extended DIAGNOSTICS," *more* output including an RS-232 ASCII error dump, not
   none) - see `MEMORY_MAP.md`'s "Puzzle" section for the still-open question of whether this
   function's target (`0x40000+0x6F0`) is genuinely the real UART or just a CRT-adjacent mirror,
   since the manual's promised extended output may go out through some other, not-yet-found channel
   entirely */

void __stdcall16far print_string_far(word str_off,word str_seg)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (*(char *)0x1b48 != '\0') {
    while (*_str_off != '\0') {
      func_0x000e0add(unaff_CS);
      unaff_CS = 0xe06b;
      func_0x000e0b2a(0xe06b,*_str_off);
      _str_off = (char *)CONCAT22(str_seg,(char *)(str_off + 1));
    }
  }
  return;
}



/* print_char (confidence: Confirmed)
   
   Evidence: `(char)` - thin wrapper: unpacks one byte argument, calls `write_readout_port_byte` */

void __stdcall16far print_char(byte char)

{
  func_0x000e0b50();
  return;
}



/* reset_readout_port (confidence: Confirmed)
   
   Evidence: Thin wrapper: calls `init_readout_port_config` */

void __cdecl16far reset_readout_port(void)

{
  func_0x000e0b6c();
  return;
}



/* write_readout_port_byte (confidence: Confirmed mechanism; hardware identity (readout/CRT
   character generator) is the best-fit inference, not yet confirmed against a schematic)
   
   Evidence: Writes the passed byte to the **fixed** physical address `0x40000+0x6F0` (never
   incremented) - see `docs/display/readout-memory.md` "The readout/CRT display memory" for why this
   is very likely a hardware port (character generator with an internal cursor), not plain RAM */

void __stdcall16far write_readout_port_byte(undefined1 param_1)

{
  DAT_4000_06f0 = param_1;
  return;
}



/* init_readout_port_config (confidence: Confirmed)
   
   Evidence: Writes 3 fixed config bytes to the readout port structure at `0x40000+0x6F0+1/+2/+3`
   (`0x29`/`0x23`/`6`) */

void __cdecl16far init_readout_port_config(void)

{
  DAT_4000_06f1 = 0x29;
  DAT_4000_06f2 = 0x23;
  DAT_4000_06f3 = 6;
  return;
}



/* print_scratch_buffer_range (confidence: Confirmed)
   
   Evidence: Opens a print region, then loops appending bytes read from the `[0x31E]`-based scratch
   buffer to the readout via `append_readout_char`, closes the record - prints a range of the
   acquisition/diagnostic scratch buffer to the readout display */

void __stdcall16far
print_scratch_buffer_range(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  int local_a;
  
  if (*(char *)0x1b7a == '\x01') {
    uVar2 = (undefined2)((ulong)param_4 >> 0x10);
    uVar1 = (undefined2)param_4;
    func_0x000e35c2();
    func_0x000e39f0(0xe352,*(undefined1 *)((int)*(undefined4 *)0x31e + param_1),0,uVar1,uVar2);
    for (local_a = 1; (local_a < param_2 && (local_a < 0x400)); local_a = local_a + 1) {
      func_0x000e39f0(0xe352,*(undefined1 *)((int)*(undefined4 *)0x31e + param_1 + local_a),9);
    }
    func_0x000e3736(0xe352,(int)param_3,(int)((ulong)param_3 >> 0x10));
  }
  return;
}



/* format_selftest_result_string (confidence: Confirmed - resolves the "actual message printing
   happening elsewhere" open question from earlier sessions)
   
   Evidence: `(status_bits)` - builds a `PASSED`/`FAILED`/`UNTESTED`/`Not installed` text string
   into buffer `[0x476]` based on which bits are set (`0x20`=not installed, `0x02`=failed,
   `0x01`=passed, else untested); returns `status_bits` unchanged. **This is the missing link** for
   how self-test results become the printed pass/fail text */

word __stdcall16far format_selftest_result_string(word status_bits)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  uVar1 = 0xe06b;
  func_0x000e0e56();
  if (*(int *)0x1b5a == 0 && *(int *)0x1b5c == 0) {
    *(undefined1 *)0x476 = 0;
  }
  else {
    func_0x000e323f(0xe06b,0x476);
    uVar1 = 0xe31d;
    func_0x000e31dc(0xe31d,0x476);
  }
  if ((status_bits & 0x20) == 0) {
    func_0x000e31dc(uVar1,0x476);
  }
  else {
    func_0x000e31dc(uVar1,0x476);
  }
  return status_bits;
}



/* save_restore_measurement_settings (confidence: Mechanism confirmed; exact settings-block contents
   not confirmed)
   
   Evidence: `(mode)` - `1` copies `[0x256]`-`[0x264]` into a backup at `[0x1B20]`-`[0x1B32]`; `2`
   copies it back - save/restore around self-test runs */

void __stdcall16far save_restore_measurement_settings(word mode)

{
  undefined2 unaff_DS;
  undefined1 in_stack_00000006;
  undefined1 in_stack_00000008;
  undefined1 in_stack_0000000a;
  
  if (mode == 1) {
    *(uint *)0x1b20 = (uint)*(byte *)0x256;
    *(uint *)0x1b22 = (uint)*(byte *)0x257;
    *(uint *)0x1b24 = (uint)*(byte *)0x258;
    *(uint *)0x1b26 = (uint)*(byte *)0x259;
    *(uint *)0x1b28 = (uint)*(byte *)0x25a;
    *(undefined2 *)0x1b2a = *(undefined2 *)0x25c;
    *(undefined2 *)0x1b2c = *(undefined2 *)0x25e;
    *(undefined2 *)0x1b2e = *(undefined2 *)0x260;
    *(undefined2 *)0x1b30 = *(undefined2 *)0x262;
    *(undefined2 *)0x1b32 = *(undefined2 *)0x264;
  }
  if (mode == 2) {
    *(undefined1 *)0x256 = *(undefined1 *)0x1b20;
    *(undefined1 *)0x257 = (char)*(undefined2 *)0x1b22;
    *(undefined1 *)0x258 = (char)*(undefined2 *)0x1b24;
    *(undefined1 *)0x259 = *(undefined1 *)0x1b26;
    *(undefined1 *)0x25a = *(undefined1 *)0x1b28;
    *(undefined2 *)0x25c = *(undefined2 *)0x1b2a;
    *(undefined2 *)0x25e = *(undefined2 *)0x1b2c;
    *(undefined2 *)0x260 = *(undefined2 *)0x1b2e;
    *(undefined2 *)0x262 = *(undefined2 *)0x1b30;
    *(undefined2 *)0x264 = *(undefined2 *)0x1b32;
  }
  if ((mode == 3) || (mode == 1)) {
    *(undefined1 *)0x256 = in_stack_0000000a;
    *(undefined1 *)0x258 = in_stack_00000006;
    *(undefined1 *)0x257 = in_stack_00000006;
    *(undefined1 *)0x259 = in_stack_00000008;
  }
  return;
}



/* configure_measurement_hw (confidence: Mechanism confirmed; peripheral identity not confirmed)
   
   Evidence: Writes 5 caller-given parameters into a shared "hardware register" cluster (`[0x326]`,
   `[0x336]`, `[0x33A]`, `[0x32E]`, plus reverse-indexed lookups into the `[0x31E]`-based scratch
   buffer). See `docs/self-test/hardware-probes.md` "Possible ADC/measurement self-test hardware" */

void __stdcall16far
configure_measurement_hw
          (undefined2 param_1,int param_2,undefined1 param_3,byte param_4,undefined1 param_5)

{
  undefined2 unaff_DS;
  
  func_0x000e0add();
  *(undefined1 *)0x258 = 0;
  *(undefined1 *)0x257 = 0;
  *(undefined1 *)*(undefined4 *)0x336 = 0;
  *(int *)*(undefined4 *)0x32e = 0xff0 - param_2;
  *(undefined1 *)0x259 = param_5;
  *(undefined1 *)*(undefined4 *)0x33a = param_5;
  *(undefined1 *)0x256 = param_3;
  *(undefined1 *)*(undefined4 *)0x326 = param_3;
  *(byte *)0x258 = param_4;
  *(byte *)0x257 = param_4;
  *(byte *)*(undefined4 *)0x336 = param_4;
  param_4 = param_4 | 0x10;
  *(byte *)0x258 = param_4;
  *(byte *)0x257 = param_4;
  *(byte *)*(undefined4 *)0x336 = param_4;
  return;
}



/* clear_selftest_status_flags (confidence: Confirmed mechanism; overall purpose (a per-cycle
   self-test reset) inferred from context)
   
   Evidence: Zeroes 4 fixed status bytes (`[0x256]`-`[0x259]`) and the byte pointed to by each of 3
   far pointers `SUB_E4443` sets up (`[0x326]`, `[0x336]`, `[0x33A]`) */

void __cdecl16far clear_selftest_status_flags(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x259 = 0;
  *(undefined1 *)*(undefined4 *)0x33a = 0;
  *(undefined1 *)0x258 = 0;
  *(undefined1 *)0x257 = 0;
  *(undefined1 *)*(undefined4 *)0x336 = 0;
  *(undefined1 *)0x256 = 0;
  *(undefined1 *)*(undefined4 *)0x326 = 0;
  return;
}



uint __stdcall16far FUN_000e_0e8c(int param_1)

{
  uint uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  *(undefined2 *)0x1b18 = 1;
  uVar5 = 0;
  uVar4 = 0x80;
  if ((param_1 == 2) || (param_1 == 2)) {
    *(undefined1 *)0x476 = 0;
  }
  uVar3 = 0;
  func_0x000e0d06();
  *(undefined2 *)0x1b5a = 0;
  *(undefined2 *)0x1b5c = 0;
  if (param_1 == 3) {
    uVar1 = func_0x000e28fe(0xe06b,uVar3,uVar4,uVar5);
    uVar2 = func_0x000e0c3d(57999,uVar1,uVar4,uVar3,uVar4,uVar1);
    uVar2 = uVar2 | uVar1;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}



uint __stdcall16far FUN_000e_0ef8(int param_1)

{
  uint uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  *(undefined2 *)0x1b18 = 1;
  uVar5 = 0;
  uVar4 = 0x80;
  if ((param_1 == 2) || (param_1 == 2)) {
    *(undefined1 *)0x476 = 0;
  }
  uVar3 = 0;
  func_0x000e0d06();
  *(undefined2 *)0x1b5a = 0;
  *(undefined2 *)0x1b5c = 0;
  if (param_1 == 3) {
    uVar1 = func_0x000e26d6(0xe06b,uVar3,uVar4,uVar5);
    uVar2 = func_0x000e0c3d(0xe26d,uVar1,uVar4,uVar3,uVar4,uVar1);
    uVar2 = uVar2 | uVar1;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}



uint __stdcall16far FUN_000e_0f64(int param_1)

{
  uint uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  *(undefined2 *)0x1b18 = 1;
  uVar5 = 0;
  uVar4 = 0x80;
  if ((param_1 == 2) || (param_1 == 2)) {
    *(undefined1 *)0x476 = 0;
  }
  uVar3 = 0;
  func_0x000e0d06();
  *(undefined2 *)0x1b5a = 0;
  *(undefined2 *)0x1b5c = 0;
  if (param_1 == 3) {
    uVar1 = func_0x000e286c(0xe06b,uVar3,uVar4,uVar5);
    uVar2 = func_0x000e0c3d(0xe26d,uVar1,uVar4,uVar3,uVar4,uVar1);
    uVar2 = uVar2 | uVar1;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}



/* selftest_measure_and_report (confidence: Mechanism confirmed; peripheral not confirmed)
   
   Evidence: Enable/run/disable measurement pattern */

void __cdecl16far selftest_measure_and_report(void)

{
  undefined2 uVar1;
  
  uVar1 = 1;
  func_0x000e0ff5();
  func_0x000e0ff5(0xe0e8,3,uVar1);
  func_0x000e0ff5(0xe0e8,2);
  return;
}



/* selftest_measure_mode (confidence: Confirmed)
   
   Evidence: `idx==1`: enable a measurement mode (`[0x1B5E]=1`); `idx==2`: disable (complementary
   reset); else (incl. `idx==3`): run the actual measurement (calls `SUB_E296E` + `SUB_E0C3D`) and
   return its result code in `ax` */

void __stdcall16far selftest_measure_mode(int param_1)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  
  if (param_1 == 1) {
    *(undefined1 *)0x530 = 1;
    *(undefined1 *)0x1b5f = 0;
    *(undefined1 *)0x1b5e = 1;
  }
  else if (param_1 == 2) {
    *(undefined1 *)0x530 = 0;
    *(undefined1 *)0x1b5e = 0;
  }
  else {
    *(undefined2 *)0x1b18 = 1;
    uVar3 = 0;
    uVar2 = 0x80;
    if ((param_1 == 2) || (param_1 == 2)) {
      *(undefined1 *)0x476 = 0;
    }
    uVar1 = 0;
    func_0x000e0d06();
    *(undefined2 *)0x1b5a = 0;
    *(undefined2 *)0x1b5c = 0;
    if (param_1 == 3) {
      uVar3 = func_0x000e296e(0xe06b,uVar1,uVar2,uVar3);
      func_0x000e0c3d(0xe296,uVar3,uVar2,uVar1,uVar2,uVar3);
    }
  }
  return;
}



void __stdcall16far FUN_000e_108c(int param_1)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  
  if (param_1 == 1) {
    *(undefined1 *)0x1b5f = 0;
    *(undefined1 *)0x1b5e = 1;
  }
  if (param_1 == 2) {
    *(undefined1 *)0x1b5e = 0;
  }
  else {
    *(undefined2 *)0x1b18 = 1;
    uVar3 = 0;
    uVar2 = 0x80;
    if ((param_1 == 2) || (param_1 == 2)) {
      *(undefined1 *)0x476 = 0;
    }
    uVar1 = 0;
    func_0x000e0d06();
    *(undefined2 *)0x1b5a = 0;
    *(undefined2 *)0x1b5c = 0;
    if (param_1 == 3) {
      uVar3 = func_0x000e2cfb(0xe06b,uVar1,uVar2,uVar3);
      func_0x000e0c3d(0xe2ce,uVar3,uVar2,uVar1,uVar2,uVar3);
    }
  }
  return;
}



/* verify_pattern_with_report (confidence: Mechanism confirmed; exact subsystem (likely
   acquisition-memory related) not confirmed)
   
   Evidence: Compares a byte from the `[0x31E]`-based (physical `0x48000`) scratch buffer against an
   expected value in a loop; on mismatch builds a detailed error message (offset + a captured value)
   rather than a plain pass/fail */

uint __stdcall16far
verify_pattern_with_report
          (undefined4 param_1,uint param_2,int param_3,int param_4,int param_5,uint param_6,
          int param_7,int param_8)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined1 local_38 [40];
  uint local_10;
  uint local_e;
  int local_c;
  uint local_a;
  
  local_a = 0;
  local_10 = param_6;
  for (local_c = param_4; local_c < param_5; local_c = local_c + 1) {
    uVar2 = (undefined2)((ulong)*(undefined4 *)0x31e >> 0x10);
    local_e = (uint)*(byte *)((int)*(undefined4 *)0x31e + param_3 + local_c);
    if (local_e == local_10) {
      local_a = local_a | 1;
    }
    else {
      uVar5 = 0x28;
      uVar4 = 0xff7b;
      func_0x000e323f(unaff_CS,local_38);
      func_0x000e33b8(0xe31d,param_3 + local_c & 0xfff,4,0x28,uVar4,uVar5);
      uVar4 = uVar2;
      func_0x000e31dc(0xe31d,local_38);
      uVar3 = 0x28;
      uVar5 = 0xff7b;
      func_0x000e31dc(0xe31d,local_38);
      func_0x000e33b8(0xe31d,local_e,2,0x28,uVar5,uVar3,uVar4);
      func_0x000e31dc(0xe31d,local_38);
      uVar5 = 0x28;
      uVar4 = 0xff7b;
      func_0x000e31dc(0xe31d,local_38);
      func_0x000e33b8(0xe31d,local_10,2,param_2 | 1,uVar4,uVar5,uVar2);
      unaff_CS = 0xe06b;
      func_0x000e07b4(0xe31d,(int)param_1,(int)((ulong)param_1 >> 0x10),local_38);
      local_a = local_a | 2;
      if (6 < local_c) break;
    }
    iVar1 = param_8;
    if ((local_c - param_4 & 1U) != 0) {
      iVar1 = param_7;
    }
    local_10 = local_10 + iVar1 & 0xff;
  }
  if ((local_a & 2) != 0) {
    func_0x000e0ba3(unaff_CS,param_3,param_5,(int)*(undefined4 *)0x1ddc + 0x2b5,
                    (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),(int)*(undefined4 *)0x1c80 + 0xb20,
                    (int)((ulong)*(undefined4 *)0x1c80 >> 0x10));
  }
  return local_a;
}



/* verify_adc_control_toggle (confidence: Mechanism confirmed)
   
   Evidence: Toggles a control byte `0x10`/`0`, writing `[0x259]` to the measurement channel
   register each time and comparing readback bit `0x1000` of `[0x332]` before/after into `[0x1B4E]`
   - a hardware sanity check called right before `run_adc_selftest` */

void __cdecl16far verify_adc_control_toggle(void)

{
  uint uVar1;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x256 = 0x10;
  *(undefined1 *)*(undefined4 *)0x326 = 0x10;
  *(undefined1 *)*(undefined4 *)0x33a = *(undefined1 *)0x259;
  uVar1 = *(uint *)*(undefined4 *)0x332;
  *(undefined1 *)0x256 = 0;
  *(undefined1 *)*(undefined4 *)0x326 = 0;
  *(undefined1 *)*(undefined4 *)0x33a = *(undefined1 *)0x259;
  *(bool *)0x1b4e = (uVar1 & 0x1000) != (*(uint *)*(undefined4 *)0x332 & 0x1000);
  return;
}



/* run_adc_selftest (confidence: Mechanism confirmed; which physical ADC not confirmed)
   
   Evidence: Clears a lookup-table range, calls `configure_measurement_hw`, polls a busy bit
   (`0x8000`) at status register `[0x322]` with a timeout, reads a 12-bit result (mask `0xFFF`),
   compares to a reference, prints pass/fail via `print_selftest_report_line` - shape matches a
   12-bit-ADC-based measurement self-test */

uint __stdcall16far
run_adc_selftest(undefined4 param_1,int param_2,int param_3,undefined2 param_4,undefined2 param_5,
                undefined2 param_6,int param_7,int param_8,uint param_9)

{
  undefined1 uVar1;
  uint *puVar2;
  undefined1 *puVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 unaff_DS;
  undefined2 uVar7;
  undefined2 uVar8;
  uint local_44;
  undefined4 local_42;
  undefined1 local_3e [40];
  uint local_16;
  uint local_14;
  undefined2 local_12;
  uint local_10;
  uint local_e;
  uint local_c;
  int local_a;
  
  local_a = 0;
  local_44 = 1;
  func_0x000e0e56();
  local_10 = param_3 + param_2;
  local_42 = (undefined1 *)
             CONCAT22((int)((ulong)*(undefined4 *)0x31e >> 0x10),
                      (undefined1 *)(((int)*(undefined4 *)0x31e + 0x1000) - param_2));
  for (local_e = 0; puVar3 = local_42, (int)local_e < (int)local_10; local_e = local_e + 1) {
    local_42 = (undefined1 *)CONCAT22(local_42._2_2_,(undefined1 *)local_42 + 1);
    *puVar3 = 0;
  }
  func_0x000e0dcc(0xe06b,param_2,param_3,param_4,param_5,param_6);
  for (; (local_c = *(uint *)*(undefined4 *)0x322 & 0x8000, local_c != 0 && (local_a < param_7));
      local_a = local_a + 1) {
  }
  local_14 = *(uint *)*(undefined4 *)0x322 & 0xfff;
  *(byte *)0x258 = *(byte *)0x258 & 0xfd;
  uVar1 = *(undefined1 *)0x258;
  *(undefined1 *)0x257 = uVar1;
  *(undefined1 *)*(undefined4 *)0x336 = uVar1;
  *(undefined1 *)0x259 = 0;
  *(undefined1 *)*(undefined4 *)0x33a = 0;
  puVar2 = (uint *)*(undefined4 *)0x332;
  local_16 = *puVar2 & 0xfff;
  func_0x000e0e56(0xe06b);
  uVar4 = (undefined2)param_1;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  uVar6 = (int)((ulong)puVar2 >> 0x10);
  if ((local_c & 0x8000) != 0) {
    func_0x000e07b4(0xe06b,uVar4,uVar5,0x79c,0xff7b,0x7b1,0xff7b,param_9 | 8);
    local_44 = 3;
    uVar6 = uVar5;
  }
  if ((((*(char *)0x1b4e != '\0') || (*(char *)0x1b7a == '\x01')) && (*"" == '\x1e')) &&
     (local_12 = 0xff1, local_16 != 0xff1)) {
    uVar8 = 0x28;
    uVar7 = 0xff7b;
    func_0x000e323f(0xe06b,local_3e);
    func_0x000e33b8(0xe31d,local_16,3,0x28,uVar7,uVar8);
    func_0x000e31dc(0xe31d,local_3e);
    uVar8 = 0x28;
    uVar7 = 0xff7b;
    func_0x000e31dc(0xe31d,local_3e);
    func_0x000e33b8(0xe31d,local_12,3,param_9 | 4,uVar7,uVar8,uVar6);
    func_0x000e07b4(0xe31d,uVar4,uVar5,local_3e);
    local_44 = local_44 | 2;
    uVar6 = uVar5;
  }
  local_10 = local_14;
  local_e = param_3 + param_8;
  if (local_14 != local_e) {
    uVar8 = 0x28;
    uVar7 = 0xff7b;
    func_0x000e323f(0xe06b,local_3e);
    func_0x000e33b8(0xe31d,local_10,3,0x28,uVar7,uVar8);
    func_0x000e31dc(0xe31d,local_3e);
    uVar8 = 0x28;
    uVar7 = 0xff7b;
    func_0x000e31dc(0xe31d,local_3e);
    func_0x000e33b8(0xe31d,local_e,3,param_9 | 2,uVar7,uVar8,uVar6);
    func_0x000e07b4(0xe31d,uVar4,uVar5,local_3e);
    local_44 = local_44 | 2;
  }
  return local_44;
}



uint __stdcall16far FUN_000e_1544(int param_1)

{
  uint uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  *(undefined2 *)0x1b18 = 1;
  uVar5 = 0;
  uVar4 = 0x80;
  if ((param_1 == 2) || (param_1 == 2)) {
    *(undefined1 *)0x476 = 0;
  }
  uVar3 = 0;
  func_0x000e0d06();
  *(undefined2 *)0x1b5a = 0;
  *(undefined2 *)0x1b5c = 0;
  if (param_1 == 3) {
    uVar1 = func_0x000e1816(0xe06b,0x76a,0xff7b,(int)*(undefined4 *)0x1dd4,
                            (int)((ulong)*(undefined4 *)0x1dd4 >> 0x10),2,0xe000,0x7fff,0xe000,0,
                            0xf000,0x7fff,0xf000,uVar3,uVar4,uVar5);
    uVar2 = func_0x000e0c3d(0xe154,uVar1,uVar4);
    uVar2 = uVar2 | uVar1;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}



uint __stdcall16far FUN_000e_15e1(int param_1)

{
  uint uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  *(undefined2 *)0x1b18 = 1;
  uVar5 = 0;
  uVar4 = 0x80;
  if ((param_1 == 2) || (param_1 == 2)) {
    *(undefined1 *)0x476 = 0;
  }
  uVar3 = 0;
  func_0x000e0d06();
  *(undefined2 *)0x1b5a = 0;
  *(undefined2 *)0x1b5c = 0;
  if (param_1 == 3) {
    uVar1 = func_0x000e1816(0xe06b,0x76e,0xff7b,(int)*(undefined4 *)0x1dd8,
                            (int)((ulong)*(undefined4 *)0x1dd8 >> 0x10),0x8002,0xe000,0xffff,0xe000,
                            0x8000,0xf000,0xffff,0xf000,uVar3,uVar4,uVar5);
    uVar2 = func_0x000e0c3d(0xe154,uVar1,uVar4);
    uVar2 = uVar2 | uVar1;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}



uint __stdcall16far FUN_000e_167e(int param_1)

{
  uint uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  *(undefined2 *)0x1b18 = 1;
  uVar5 = 0;
  uVar4 = 0x80;
  if ((param_1 == 2) || (param_1 == 2)) {
    *(undefined1 *)0x476 = 0;
  }
  uVar3 = 0;
  func_0x000e0d06();
  *(undefined2 *)0x1b5a = 0;
  *(undefined2 *)0x1b5c = 0;
  if (param_1 == 3) {
    uVar1 = func_0x000e16ea(0xe06b,uVar3,uVar4,uVar5);
    uVar2 = func_0x000e0c3d(0xe154,uVar1,uVar4,uVar3,uVar4,uVar1);
    uVar2 = uVar2 | uVar1;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}



/* selftest_rom_checksum (confidence: Confirmed via string reference)
   
   Evidence: Main ROM checksum (`ROMS`/`MISMATCH`) */

undefined2 __cdecl16far selftest_rom_checksum(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined1 local_32 [40];
  undefined2 local_a;
  
  local_a = 1;
  if (*(char *)((int)*(undefined4 *)0x1dd4 + 4) == *(char *)((int)*(undefined4 *)0x1dd8 + 4)) {
    unaff_CS = 0xe3b1;
    iVar1 = func_0x000e4571();
    if (iVar1 != 0) {
      return local_a;
    }
    if (*(char *)((int)*(undefined4 *)0x1dd4 + 4) == DAT_8000_0004) {
      return local_a;
    }
  }
  uVar5 = 0x28;
  uVar4 = 0xff7b;
  func_0x000e323f(unaff_CS,local_32);
  uVar2 = (undefined2)((ulong)*(undefined4 *)0x1dd4 >> 0x10);
  func_0x000e34cb(0xe31d,*(undefined1 *)((int)*(undefined4 *)0x1dd4 + 4),0x28,uVar4,uVar5);
  func_0x000e31dc(0xe31d,local_32);
  uVar3 = 0x28;
  uVar5 = 0xff7b;
  func_0x000e31dc(0xe31d,local_32);
  uVar4 = (undefined2)((ulong)*(undefined4 *)0x1dd8 >> 0x10);
  func_0x000e34cb(0xe31d,*(undefined1 *)((int)*(undefined4 *)0x1dd8 + 4),0x28,uVar5,uVar3,uVar2);
  func_0x000e31dc(0xe31d,local_32);
  uVar5 = 0x28;
  uVar2 = 0xff7b;
  func_0x000e31dc(0xe31d,local_32);
  func_0x000e34cb(0xe31d,DAT_8000_0004,0x28,uVar2,uVar5,uVar4);
  func_0x000e31dc(0xe31d,local_32);
  func_0x000e07b4(0xe31d,0x778,0xff7b,0x77d,0xff7b,local_32);
  return 2;
}



/* verify_rom_checksum_and_report (confidence: Confirmed)
   
   Evidence: `(name_ptr, range1, range2)` - the real generic ROM-checksum-and-report engine:
   computes `compute_range_checksum` over 2 ranges (chained), compares against a stored expected
   value, and on mismatch builds a "`<name> <computed> <expected>`" diagnostic message. Confirmed
   caller: `selftest_comm_rom`, checksumming the comm ROM's two 32KB halves against the `"COMM_ROM"`
   label */

undefined2 __stdcall16far
verify_rom_checksum_and_report
          (undefined4 param_1,byte *param_2,undefined2 param_3,undefined2 param_4,undefined4 param_5
          ,undefined4 param_6,undefined4 param_7)

{
  byte *pbVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined1 local_34 [40];
  uint local_c;
  undefined2 local_a;
  
  local_a = 1;
  uVar2 = (undefined2)((ulong)param_5 >> 0x10);
  local_c = func_0x000e45a6();
  local_c = func_0x000e45a6(0xe45a,local_c,(int)param_6,(int)((ulong)param_6 >> 0x10),(int)param_7,
                            (int)((ulong)param_7 >> 0x10),uVar2);
  uVar2 = (undefined2)((ulong)param_2 >> 0x10);
  pbVar1 = (byte *)param_2;
  if (local_c != (uint)*param_2 * 0x100 + (uint)pbVar1[1]) {
    uVar7 = 10;
    uVar3 = (undefined2)((ulong)*(undefined4 *)(pbVar1 + 6) >> 0x10);
    uVar4 = uVar3;
    func_0x000e323f(0xe45a,local_34);
    uVar6 = 0x28;
    uVar5 = 0xff7b;
    func_0x000e31dc(0xe31d,local_34);
    func_0x000e34cb(0xe31d,local_c >> 8,0x28,uVar5,uVar6,uVar4,uVar7);
    uVar4 = uVar3;
    func_0x000e31dc(0xe31d,local_34);
    func_0x000e34cb(0xe31d,local_c,0x28,uVar4);
    func_0x000e31dc(0xe31d,local_34);
    uVar5 = 0x28;
    uVar4 = 0xff7b;
    func_0x000e31dc(0xe31d,local_34);
    func_0x000e34cb(0xe31d,*param_2,0x28,uVar4,uVar5,uVar3);
    uVar4 = uVar2;
    func_0x000e31dc(0xe31d,local_34);
    func_0x000e34cb(0xe31d,pbVar1[1],0x28,uVar4);
    func_0x000e31dc(0xe31d,local_34);
    func_0x000e07b4(0xe31d,(int)param_1,(int)((ulong)param_1 >> 0x10),local_34);
    local_a = 2;
  }
  return local_a;
}



uint __stdcall16far FUN_000e_1a0b(int param_1)

{
  uint uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  *(undefined2 *)0x1b18 = 1;
  uVar5 = 0;
  uVar4 = 0x80;
  if ((param_1 == 2) || (param_1 == 2)) {
    *(undefined1 *)0x476 = 0;
  }
  uVar3 = 0;
  func_0x000e0d06();
  *(undefined2 *)0x1b5a = 0;
  *(undefined2 *)0x1b5c = 0;
  if (param_1 == 3) {
    uVar1 = func_0x000e1b89(0xe06b,0x740,0xff7b,0,(int)*(undefined4 *)0x1c80 + *(int *)0x1c02,
                            (int)((ulong)*(undefined4 *)0x1c80 >> 0x10),
                            (int)*(undefined4 *)0x1c80 + *(int *)0x1c02 + 99,
                            (int)((ulong)*(undefined4 *)0x1c80 >> 0x10),1,0xf,uVar3,uVar4,uVar5);
    uVar2 = func_0x000e0c3d(0xe197,uVar1,uVar4);
    uVar2 = uVar2 | uVar1;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}



uint __stdcall16far FUN_000e_1aaa(int param_1)

{
  uint uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  *(undefined2 *)0x1b18 = 1;
  uVar5 = 0;
  uVar4 = 0x80;
  if ((param_1 == 2) || (param_1 == 2)) {
    *(undefined1 *)0x476 = 0;
  }
  uVar3 = 0;
  func_0x000e0d06();
  *(undefined2 *)0x1b5a = 0;
  *(undefined2 *)0x1b5c = 0;
  if (param_1 == 3) {
    uVar1 = func_0x000e1b16(0xe06b,uVar3,uVar4,uVar5);
    uVar2 = func_0x000e0c3d(0xe197,uVar1,uVar4,uVar3,uVar4,uVar1);
    uVar2 = uVar2 | uVar1;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}



/* selftest_acq_ram (confidence: Confirmed via string reference)
   
   Evidence: Acquisition RAM, even/odd banks (`ACQ_RAM even`/`ACQ_RAM odd`) */

uint __cdecl16far selftest_acq_ram(void)

{
  uint uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  
  uVar4 = 0xff;
  uVar3 = 2;
  uVar1 = func_0x000e1b89();
  uVar2 = func_0x000e1b89(0xe197,0x755,0xff7b,0xb,(int)*(undefined4 *)0x31e + 1,
                          (int)((ulong)*(undefined4 *)0x31e >> 0x10),
                          (int)*(undefined4 *)0x31e + 0xfff,
                          (int)((ulong)*(undefined4 *)0x31e >> 0x10),2,0xff,uVar3,uVar4);
  return uVar1 | uVar2;
}



/* ram_pattern_test (confidence: Confirmed from code alone (classic march-pattern RAM test shape))
   
   Evidence: `(start_off, start_seg, end, step, mask)` **(corrected 2026-09-16 - `end` is a plain
   word bound within `start`'s own segment, not a second far pointer as previously listed)** -
   generic RAM test engine: writes an alternating `0xAA`/`0x55` pattern across the range, then reads
   back and compares (masked). The shared implementation likely behind the
   `SYS_RAM`/`NIB_RAM`/`ACQ_RAM`/`COMM_RAM` self-tests */

int __stdcall16far ram_pattern_test(word start_off,word start_seg,word end,word step,word mask)

{
  int iVar1;
  byte bVar2;
  uint unaff_SI;
  undefined2 uVar3;
  byte *in_stack_0000000e;
  int in_stack_00000012;
  uint in_stack_00000014;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined1 local_38 [40];
  int local_10;
  uint local_e;
  undefined4 local_c;
  
  local_10 = 1;
  iVar1 = 0xaa;
  do {
    if (iVar1 != 0xaa) {
LAB_000e_1c14:
      if (local_10 != 1) {
        uVar5 = 0x28;
        uVar4 = 0xff7b;
        func_0x000e323f();
        uVar3 = (undefined2)((ulong)local_c >> 0x10);
        uVar4 = func_0x000e6d2f(0xe31d,(byte *)local_c,uVar3,0x28,uVar4,uVar5);
        func_0x000e3372(0xe6a8,uVar4);
        uVar4 = uVar3;
        func_0x000e31dc(0xe31d,local_38);
        func_0x000e3372(0xe31d,local_e,0x28,uVar4);
        func_0x000e31dc(0xe31d,local_38);
        uVar5 = 0x28;
        uVar4 = 0xff7b;
        func_0x000e31dc(0xe31d,local_38);
        func_0x000e3372(0xe31d,unaff_SI & in_stack_00000014,end,uVar4,uVar5,uVar3);
        func_0x000e07b4(0xe31d,start_off,start_seg,local_38);
      }
      return local_10;
    }
    bVar2 = 0xaa;
    local_c = (byte *)CONCAT22(mask,step);
    while ((byte *)local_c <= in_stack_0000000e) {
      *local_c = bVar2;
      bVar2 = ~bVar2;
      local_c = (byte *)CONCAT22(local_c._2_2_,(byte *)local_c + in_stack_00000012);
    }
    unaff_SI = 0xaa;
    local_c = (byte *)CONCAT22(mask,step);
    while ((byte *)local_c <= in_stack_0000000e) {
      local_e = (uint)*local_c;
      if ((local_e & in_stack_00000014) != (unaff_SI & in_stack_00000014)) {
        local_10 = 2;
        goto LAB_000e_1c14;
      }
      unaff_SI = ~unaff_SI;
      local_c = (byte *)CONCAT22(local_c._2_2_,(byte *)local_c + in_stack_00000012);
    }
    iVar1 = 0x55;
  } while( true );
}



uint __stdcall16far FUN_000e_1cbc(int param_1)

{
  uint uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  *(undefined2 *)0x1b18 = 1;
  uVar5 = 0;
  uVar4 = 0x80;
  if ((param_1 == 2) || (param_1 == 2)) {
    *(undefined1 *)0x476 = 0;
  }
  uVar3 = 0;
  func_0x000e0d06();
  *(undefined2 *)0x1b5a = 0;
  *(undefined2 *)0x1b5c = 0;
  if (param_1 == 3) {
    uVar1 = func_0x000e1d28(0xe06b,uVar3,uVar4,uVar5);
    uVar2 = func_0x000e0c3d(0xe1cb,uVar1,uVar4,uVar3,uVar4,uVar1);
    uVar2 = uVar2 | uVar1;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}



/* selftest_comm_loopback_a (confidence: Confirmed via string reference)
   
   Evidence: Comm-board loopback, phase A (via `SUB_E20B0`, `COMM_LB`) */

undefined2 __cdecl16far selftest_comm_loopback_a(void)

{
  int iVar1;
  undefined2 uVar2;
  
  iVar1 = func_0x000e4571();
  if (iVar1 == 0) {
    uVar2 = func_0x000e20b0(0xe3b1);
  }
  else {
    uVar2 = 0x20;
  }
  return uVar2;
}



uint __stdcall16far FUN_000e_1d47(int param_1)

{
  uint uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  *(undefined2 *)0x1b18 = 1;
  uVar5 = 0;
  uVar4 = 0x80;
  if ((param_1 == 2) || (param_1 == 2)) {
    *(undefined1 *)0x476 = 0;
  }
  uVar3 = 0;
  func_0x000e0d06();
  *(undefined2 *)0x1b5a = 0;
  *(undefined2 *)0x1b5c = 0;
  if (param_1 == 3) {
    uVar1 = func_0x000e1db3(0xe06b,uVar3,uVar4,uVar5);
    uVar2 = func_0x000e0c3d(0xe1cb,uVar1,uVar4,uVar3,uVar4,uVar1);
    uVar2 = uVar2 | uVar1;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}



/* selftest_comm_loopback_b (confidence: Confirmed via string reference)
   
   Evidence: Comm-board loopback, phase B (via `SUB_E1FBC`, `COMM_LB`/`FGET NOT SET`/`FGET NOT
   CLEAR`) */

undefined2 __cdecl16far selftest_comm_loopback_b(void)

{
  int iVar1;
  undefined2 uVar2;
  
  iVar1 = func_0x000e4571();
  if (iVar1 == 0) {
    uVar2 = func_0x000e1fbc(0xe3b1);
  }
  else {
    uVar2 = 0x20;
  }
  return uVar2;
}



uint __stdcall16far FUN_000e_1dd2(int param_1)

{
  uint uVar1;
  uint uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  *(undefined2 *)0x1b18 = 1;
  uVar5 = 0;
  uVar4 = 0x80;
  if ((param_1 == 2) || (param_1 == 2)) {
    *(undefined1 *)0x476 = 0;
  }
  uVar3 = 0;
  func_0x000e0d06();
  *(undefined2 *)0x1b5a = 0;
  *(undefined2 *)0x1b5c = 0;
  if (param_1 == 3) {
    uVar1 = func_0x000e1e3e(0xe06b,uVar3,uVar4,uVar5);
    uVar2 = func_0x000e0c3d(0xe1cb,uVar1,uVar4,uVar3,uVar4,uVar1);
    uVar2 = uVar2 | uVar1;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}



/* selftest_comm_rom (confidence: Confirmed via string reference)
   
   Evidence: Comm ROM checksum, both its real address and `0x90000` alias (`COMM_ROM`) */

undefined2 __cdecl16far selftest_comm_rom(void)

{
  int iVar1;
  undefined2 uVar2;
  
  iVar1 = func_0x000e4571();
  if (iVar1 == 0) {
    uVar2 = func_0x000e1816(0xe3b1,0x6ae,0xff7b,0,0x8000,2,0x8000,0x7fff,0x8000,0,0x9000,0x7fff,
                            0x9000);
  }
  else {
    uVar2 = 0x20;
  }
  return uVar2;
}



/* selftest_comm_ram (confidence: Confirmed via string reference)
   
   Evidence: Comm-board RAM (`COMM_RAM`/`CMOS NOT SUPPORTED`) */

undefined2 __cdecl16far selftest_comm_ram(void)

{
  undefined1 *puVar1;
  int iVar2;
  undefined2 uVar3;
  undefined1 *local_e;
  
  iVar2 = func_0x000e4571();
  if (iVar2 == 0) {
    uVar3 = func_0x000e1b89(0xe3b1,0x6b7,0xff7b,0,0,0x8f80,0x7ff,0x8f80,1,0xff);
    local_e = (undefined1 *)0x8f800000;
    while (puVar1 = local_e, (undefined1 *)local_e < (undefined1 *)0x7ff) {
      local_e = (undefined1 *)CONCAT22(local_e._2_2_,(undefined1 *)local_e + 1);
      *puVar1 = 0;
    }
    func_0x00080133(0xe197);
    func_0x000f0078(0x8013,0x41);
  }
  else {
    uVar3 = 0x20;
  }
  return uVar3;
}



/* selftest_cmos (confidence: Confirmed via string reference)
   
   Evidence: CMOS/NVRAM, with recovery (`CMOS`/`reformated`/`recovered`) */

undefined2 __cdecl16far selftest_cmos(void)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 unaff_DS;
  
  if ((*(byte *)0x1bf9 & 2) == 0) {
    uVar1 = 0x20;
  }
  else if (*"" == '\x1e') {
    iVar2 = func_0x00095476();
    func_0x000f0078(0x9471,0x41);
    if (iVar2 == 0) {
      uVar1 = 1;
    }
    else {
      if (iVar2 < 0) {
        uVar1 = 0x6da;
      }
      else {
        uVar1 = 0x6e5;
      }
      func_0x000e07b4(0xf007,0x6d5,0xff7b,uVar1,0xff7b,0x6ef,0xff7b,0);
      uVar1 = 2;
    }
  }
  else {
    func_0x000e07b4();
    uVar1 = 2;
  }
  return uVar1;
}



/* selftest_comm_fget_flag (confidence: Confirmed (full instruction trace))
   
   Evidence: The real body of `selftest_comm_loopback_b`: writes a command byte to physical
   `0x406F3` (the readout-window comm command register, `0x40000+0x6F0+3`), reads back status bits
   at `0x4067C`/`0x406BC`, reports "COMM_LB / FGET NOT SET" or "FGET NOT CLEAR" via
   `print_selftest_report_line` if the flag doesn't behave as expected. **Fully traced 2026-09-13**:
   this real check only runs if `[0x1BFA]!=0` (set by `selftest_comm_readback` only when its own
   readback was exactly `0xD0`); otherwise falls through to checking `[0x1B7A]` - returns `1`
   (PASSED, no real check) if that's `0`, or `0x80` (renders as `UNTESTED`, not a recognized
   pass/fail/not-installed bit) if `[0x1B7A]` is nonzero. This fully explains live-tested
   `COMM_LOOPBACK`/`UNTESTED` results without needing a `[0x1B83]` comm-detection failure - see
   `docs/comm-rom/rs232-early-investigation.md`'s "Correction" writeup */

undefined2 __cdecl16far selftest_comm_fget_flag(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 local_a;
  
  local_a = 1;
  if (*(char *)0x1bfa == '\0') {
    if (*(char *)0x1b7a != '\0') {
      local_a = 0x80;
    }
  }
  else {
    DAT_4000_06f3 = 0x86;
    if (((DAT_4000_067c & 4) != 4) || ((DAT_4000_06bc & 0x80) != 0x80)) {
      unaff_CS = 0xe06b;
      func_0x000e07b4();
      local_a = 2;
    }
    DAT_4000_06f3 = 6;
    if (((DAT_4000_067c & 4) != 0) || ((DAT_4000_06bc & 0x80) != 0)) {
      func_0x000e07b4(unaff_CS,0x706,0xff7b,0x70e,0xff7b,0x71d,0xff7b,0);
      local_a = 2;
    }
  }
  return local_a;
}



/* selftest_comm_readback (confidence: Confirmed (full instruction trace + emulator verification))
   
   Evidence: References `COMM_RB`/`rb(1)=`/`rb(0)=` - called as the 2nd phase by
   `selftest_comm_loopback_a`. Reads/writes physical `0x40000+0x67C`/`0x6F8` (the readout memory
   window, not the comm ROM's own `0x80000` address). **Fully traced 2026-09-13**: resets
   `[0x1BFA]=0`, reads a status byte from `0x4067C`, and treats `0x40`/`0x60`/`0xC0`/`0xD0`/`0xE0`
   as passing values - but only sets `[0x1BFA]=1` when the value is exactly `0xD0`; anything else
   fails with a report line and clears `[0x1BF9]`. **Exact mechanism decoded 2026-09-17**: it writes
   `0` then `1` to the Interrupt Mask Latch's diagnostic output `3D` (`0x406FB`), reading `0x4067C`
   once after each write, and combines the two as `(read1&0xC0)>>2 | (read2&0xC0)` - the "status
   byte" isn't one raw readback, it's this derived combination, requiring bit `0x40` fixed high and
   bit `0x80` to go `0`→`1` between the two reads (a real hardware loopback of output `3D` - see
   `MEMORY_MAP.md`'s Interrupt Mask Latch section) to ever reach exactly `0xD0`. Modeled by
   `io_stubs.DiagCommLatchLoopback`, confirmed to make this test pass. **Conflicts with the Options
   manual's Table 7-36** (`docs/options.md`), which names bit `0x40` (not `0x80`) as `DIAG`/`3D` and
   bit `0x80` as the unrelated `/DCD2` - tested bit `0x40` directly and it does NOT reach `0xD0`
   (produces `0x40`, a genuine fail), so the disassembly-required bit `0x80` is kept pending a real
   schematic trace - see `MEMORY_MAP.md`'s Interrupt Mask Latch section and
   `io_stubs.DiagCommLatchLoopback`'s docstring for the full conflict writeup. This `[0x1BFA]` side
   effect is what `selftest_comm_fget_flag` checks next - see
   `VARIABLES.md`/`docs/comm-rom/rs232-early-investigation.md`'s "Correction: COMM_LOOPBACK's
   UNTESTED result" */

undefined2 __cdecl16far selftest_comm_readback(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 local_34;
  undefined1 local_32 [41];
  byte local_9;
  
  local_34 = 1;
  *(undefined1 *)0x1bfa = 0;
  DAT_4000_06fb = 1;
  local_9 = (byte)((int)(DAT_4000_067c & 0xc0) >> 2) | DAT_4000_067c & 0xc0;
  if (((local_9 != 0x40) && (local_9 != 0x60)) && (local_9 != 0xc0)) {
    if (local_9 == 0xd0) {
      *(undefined1 *)0x1bfa = 1;
    }
    else if (local_9 != 0xe0) {
      uVar3 = 0x28;
      uVar2 = 0xff7b;
      func_0x000e323f();
      func_0x000e327f(0xe31d,(int)(local_9 & 0xc) >> 2,2,2,0,0,0x28,uVar2,uVar3);
      uVar1 = 0x4000;
      func_0x000e31dc(0xe31d,local_32);
      uVar3 = 0x28;
      uVar2 = 0xff7b;
      func_0x000e31dc(0xe31d,local_32);
      func_0x000e327f(0xe31d,local_9 & 3,2,2,0,0,0x28,uVar2,uVar3,uVar1);
      func_0x000e31dc(0xe31d,local_32);
      func_0x000e07b4(0xe31d,0x72f,0xff7b,local_32);
      local_34 = 2;
      *(undefined1 *)0x1bf9 = 0;
    }
  }
  return local_34;
}



undefined2 __stdcall16far FUN_000e_2202(int param_1)

{
  undefined2 uVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  if (param_1 == 1) {
    *(undefined2 *)0x1b18 = 1;
    unaff_CS = 0xe06b;
    func_0x000e06b6();
  }
  if ((param_1 == 2) || (param_1 == 2)) {
    *(undefined1 *)0x476 = 0;
  }
  uVar2 = 0;
  func_0x000e0d06(unaff_CS,param_1,0,0,0,0);
  *(undefined2 *)0x1b5a = 0;
  *(undefined2 *)0x1b5c = 0;
  if (param_1 == 3) {
    uVar1 = func_0x000e22af(0xe06b,3);
    uVar2 = func_0x000e0c3d(0xe220,uVar1,param_1,uVar2,uVar1);
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}



/* selftest_front_panel_switch_a (confidence: Mechanism confirmed; which physical control not
   confirmed)
   
   Evidence: Front-panel control test, scans `update_menu_position` over range 0-8 */

void __cdecl16far selftest_front_panel_switch_a(void)

{
  uint uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  
  uVar3 = 1;
  func_0x000e06b6();
  uVar2 = 0xe06b;
  do {
    uVar1 = func_0x000e22af(uVar2,3,uVar3);
    uVar2 = 0xe220;
  } while ((uVar1 & 0x80) == 0);
  return;
}



/* run_adc_selftest_range (confidence: Confirmed)
   
   Evidence: Loops `update_menu_position(0, 8, ...)` to scan up to 8 positions, calling
   `run_indexed_adc_selftest` for each until the `0xFFFE` sentinel, OR-folding results */

uint __stdcall16far run_adc_selftest_range(undefined2 param_1)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 local_a;
  
  local_a = 0;
  iVar2 = func_0x000e06b6();
  if (iVar2 != -2) {
    local_a = func_0x000e230b(0xe06b,iVar2,param_1);
    uVar1 = *(undefined4 *)(iVar2 * 10 + (int)*(undefined4 *)0x1dcc + 2);
    *(undefined2 *)0x1b5a = (int)uVar1;
    *(undefined2 *)0x1b5c = (int)((ulong)uVar1 >> 0x10);
    if (7 < iVar2) {
      local_a = local_a | 0x80;
    }
  }
  return local_a;
}



/* run_indexed_adc_selftest (confidence: Confirmed)
   
   Evidence: `(index)` - builds a descriptive test name from a base string plus a per-index name
   from a 10-byte-per-record device table at `[0x1DCC]`, then calls `run_adc_selftest` with
   per-record min/max thresholds - ties to the `A_TO_D_TESTS` menu entry found in `HARDWARE.md` */

uint __stdcall16far run_indexed_adc_selftest(word index)

{
  uint uVar1;
  undefined2 unaff_DS;
  undefined1 local_28 [20];
  uint local_14;
  undefined2 local_12;
  int local_10;
  int local_e;
  uint local_c;
  undefined2 local_a;
  
  func_0x000e323f();
  func_0x000e31dc(0xe31d,local_28);
  local_12 = 2;
  local_10 = *(byte *)(index * 10 + (int)*(undefined4 *)0x1dcc + 8) - 2;
  local_e = 0xffe;
  local_c = func_0x000e12f4(0xe31d,local_28);
  local_a = 2;
  local_14 = (uint)*(byte *)((int)*(undefined4 *)0x31e + local_e + 2);
  uVar1 = func_0x000e1116(0xe0e8,local_28);
  return local_c | uVar1;
}



/* selftest_tb_divider (confidence: Confirmed)
   
   Evidence: Conditional on `[0x1B83]==0x1E`; scans `update_menu_position` over range 0-0x18,
   calling `step_tb_divider_test` per position - matches the real on-screen name `TB_DIVIDER`
   confirmed in `HARDWARE.md` */

uint __cdecl16far selftest_tb_divider(void)

{
  uint uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  
  uVar3 = 1;
  func_0x000e06b6();
  uVar2 = 0xe06b;
  do {
    uVar1 = func_0x000e255e(uVar2,3,uVar3);
    uVar2 = 0xe248;
  } while ((uVar1 & 0x80) == 0);
  return uVar1;
}



/* step_tb_divider_test (confidence: Confirmed)
   
   Evidence: Per-position worker for `selftest_tb_divider`: converts the scanned position into a
   shifted `0xFFF` threshold mask and calls `verify_timebase_prc` */

uint __stdcall16far step_tb_divider_test(undefined2 param_1)

{
  int iVar1;
  uint uVar2;
  undefined2 unaff_DS;
  uint local_a;
  
  iVar1 = func_0x000e06b6();
  if (iVar1 != -2) {
    if (iVar1 < 0xc) {
      uVar2 = 0xfff >> (0xcU - (char)iVar1 & 0x1f);
    }
    else {
      uVar2 = 0xfff << ((char)iVar1 - 0xcU & 0x1f);
    }
    local_a = func_0x000e260a(0xe06b,uVar2 & 0xfff,param_1);
    func_0x000e327f(0xe248,uVar2 & 0xfff,0x10,3,0,0,10);
    func_0x000e323f(0xe31d,0x476);
    *(undefined2 *)0x1b5a = 0x476;
    *(undefined2 *)0x1b5c = unaff_DS;
    if (0x17 < iVar1) {
      local_a = local_a | 0x80;
    }
  }
  return local_a;
}



/* verify_timebase_prc (confidence: Mechanism confirmed; "PRC"'s exact meaning and peripheral not
   confirmed)
   
   Evidence: Configures the shared measurement hardware register cluster, reads back a 12-bit value
   from `[0x332]`, compares against an expected value, and reports `PRC ... read-back <hex>` on
   mismatch. References sweep-speed divider names (`hs/2`, `ps/2`...) suggesting "PRC" is a
   timebase/reference-clock counter */

undefined2 __stdcall16far verify_timebase_prc(uint param_1)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 local_34;
  undefined1 local_32 [40];
  uint local_a;
  
  local_34 = 1;
  func_0x000e0e56();
  *(uint *)*(undefined4 *)0x32e = param_1;
  *(byte *)0x258 = *(byte *)0x258 & 0xfd;
  uVar1 = *(undefined1 *)0x258;
  *(undefined1 *)0x257 = uVar1;
  *(undefined1 *)*(undefined4 *)0x336 = uVar1;
  *(undefined1 *)0x259 = 0;
  *(undefined1 *)*(undefined4 *)0x33a = 0;
  uVar2 = (undefined2)((ulong)*(undefined4 *)0x332 >> 0x10);
  local_a = *(uint *)*(undefined4 *)0x332 & 0xfff;
  if (local_a != param_1) {
    uVar4 = 0x28;
    uVar3 = 0xff7b;
    func_0x000e323f(0xe06b,local_32);
    func_0x000e3372(0xe31d,local_a,0x28,uVar3,uVar4);
    func_0x000e31dc(0xe31d,local_32);
    uVar4 = 0x28;
    uVar3 = 0xff7b;
    func_0x000e31dc(0xe31d,local_32);
    func_0x000e3372(0xe31d,param_1,0xc,uVar3,uVar4,uVar2);
    func_0x000e07b4(0xe31d,0x66e,0xff7b,local_32);
    local_34 = 3;
  }
  return local_34;
}



/* selftest_mm_acq (confidence: Confirmed via string reference)
   
   Evidence: Min-max acquisition mode (`MM_ACQ`) */

uint __cdecl16far selftest_mm_acq(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined1 local_3c [40];
  undefined2 local_14;
  undefined2 local_12;
  int local_10;
  int local_e;
  uint local_c;
  int local_a;
  
  *(undefined2 *)0x1b18 = 1;
  local_14 = 2;
  local_12 = 0x1e;
  local_10 = 0xffe;
  uVar6 = 0;
  uVar5 = 100;
  uVar2 = 0xe0e8;
  local_c = func_0x000e12f4();
  for (local_a = 2; local_a < 0x20; local_a = local_a + 2) {
    uVar1 = (undefined2)((ulong)*(undefined4 *)0x31e >> 0x10);
    local_e = (uint)*(byte *)((int)*(undefined4 *)0x31e + local_10 + local_a + 1) -
              (uint)*(byte *)((int)*(undefined4 *)0x31e + local_10 + local_a);
    if ((local_e != 0xff) && (local_e != 199)) {
      uVar4 = 0x28;
      uVar3 = 0xff7b;
      func_0x000e323f(uVar2,local_3c);
      func_0x000e33b8(0xe31d,local_10 + local_a & 0xfff,4,0x28,uVar3,uVar4,uVar5,uVar6);
      uVar2 = uVar1;
      func_0x000e31dc(0xe31d,local_3c);
      uVar4 = 0x28;
      uVar3 = 0xff7b;
      func_0x000e31dc(0xe31d,local_3c);
      func_0x000e33b8(0xe31d,local_e,2,0x28,uVar3,uVar4,uVar2);
      func_0x000e31dc(0xe31d,local_3c);
      uVar3 = 0x28;
      uVar2 = 0xff7b;
      func_0x000e31dc(0xe31d,local_3c);
      func_0x000e33b8(0xe31d,199,2,0xc1,uVar2,uVar3,uVar1);
      uVar2 = 0xe06b;
      func_0x000e07b4(0xe31d,0x63b,0xff7b,local_3c);
      local_c = local_c | 2;
      if (6 < local_a) break;
    }
  }
  if ((local_c & 2) != 0) {
    func_0x000e0ba3(uVar2,local_10,0x20,(int)*(undefined4 *)0x1ddc + 0x2b5,
                    (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),(int)*(undefined4 *)0x1c80 + 0xb20,
                    (int)((ulong)*(undefined4 *)0x1c80 >> 0x10));
  }
  return local_c;
}



/* selftest_xy_acq (confidence: Confirmed via string reference)
   
   Evidence: X-Y acquisition mode (`XY_ACQ`) */

uint __cdecl16far selftest_xy_acq(void)

{
  undefined1 uVar1;
  uint uVar2;
  uint uVar3;
  undefined2 unaff_DS;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  int iVar8;
  
  uVar6 = 2;
  uVar7 = 0xe;
  iVar8 = 0xffe;
  uVar5 = 4;
  uVar4 = 100;
  uVar2 = func_0x000e12f4();
  uVar1 = *(undefined1 *)((int)*(undefined4 *)0x31e + iVar8 + 2);
  uVar3 = func_0x000e1116(0xe0e8,0x649,0xff7b,0xd0,iVar8,2,0x10,uVar1,3,1,uVar4,uVar5,uVar6,uVar7,
                          iVar8,uVar1);
  return uVar2 | uVar3;
}



/* selftest_hs_acq (confidence: Confirmed via string reference)
   
   Evidence: High-speed acquisition mode (`HS_ACQ`) */

uint __cdecl16far selftest_hs_acq(void)

{
  uint uVar1;
  uint uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  uVar5 = 0xa2;
  uVar4 = 9;
  uVar3 = 0xff;
  uVar1 = func_0x000e12f4();
  uVar2 = func_0x000e1116(0xe0e8,0x61f,0xff7b,0x20,0xf9e,2,0x104,5,1,1,uVar3,uVar4,uVar5);
  return uVar1 | uVar2;
}



/* selftest_front_panel_adc (confidence: Confirmed via string reference - identifies the peripheral
   behind `configure_measurement_hw`/`run_adc_selftest`/`selftest_measure_and_report`)
   
   Evidence: References `FP_a2d`/`gnd =`/`TIME-OUT` - tests the front-panel A/D converter. Called
   from `selftest_measure_mode`'s "run" case (`idx==3`), whose result feeds
   `format_selftest_result_string` */

uint __cdecl16far selftest_front_panel_adc(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined1 local_3a [40];
  uint local_12;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  
  local_12 = 1;
  if (*(char *)0x1b7a != '\x01') {
    *(undefined1 *)0x75b = 0;
  }
  uVar3 = 0x22;
  uVar1 = 0xe296;
  local_e = func_0x000e2ab0();
  if (*"" != '\x14') {
    local_a = func_0x000e2ab0(0xe296,0xe0,uVar3);
    uVar1 = 0xe296;
    local_c = func_0x000e2ab0(0xe296,0x40);
    local_10 = local_a + local_c;
    if ((0x700 < local_10) || (local_10 < 0x100)) {
      uVar3 = 0x28;
      uVar1 = 0xff7b;
      func_0x000e323f(0xe296,local_3a);
      func_0x000e3372(0xe31d,local_a,0x28,uVar1,uVar3);
      func_0x000e31dc(0xe31d,local_3a);
      uVar3 = 0x28;
      uVar1 = 0xff7b;
      func_0x000e31dc(0xe31d,local_3a);
      func_0x000e3372(0xe31d,local_c,0xf2,uVar1,uVar3);
      uVar1 = 0xe06b;
      func_0x000e07b4(0xe31d,0x5ef,0xff7b,local_3a);
      local_12 = local_12 | 2;
    }
  }
  if ((5 < local_e) || (local_e < 0)) {
    uVar2 = 0x28;
    uVar3 = 0xff7b;
    func_0x000e323f(uVar1,local_3a);
    func_0x000e3372(0xe31d,local_e,0x28,uVar3,uVar2);
    func_0x000e31dc(0xe31d,local_3a);
    func_0x000e07b4(0xe31d,0x5fc,0xff7b,local_3a);
    local_12 = local_12 | 2;
  }
  return local_12;
}



/* selftest_init_channel_hw (confidence: Mechanism confirmed; hardware identity (which peripheral
   `[0x1D20]` addresses) not confirmed)
   
   Evidence: Calls `clear_selftest_status_flags` and `read_display_chip_int_reset`, then writes a
   short command sequence (`0x1D`, `9`, `0x1D`) combined with the caller's channel-select bits to a
   fixed far-pointer hardware register at `[0x1D20]` - looks like a front-end/ADC initialization
   sequence */

int __stdcall16far selftest_init_channel_hw(byte param_1)

{
  byte bVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  int local_a;
  
  local_a = 0x800;
  param_1 = param_1 & 0xe2;
  uVar3 = 0xe06b;
  func_0x000e0e56();
  if (*(char *)0x1b7a != '\x01') {
    uVar3 = 0xe3b1;
    func_0x000e4429(0xe06b);
  }
  *(undefined2 *)0x1b1c = 0xff;
  *(undefined1 *)*(undefined4 *)0x1d20 = 0xff;
  *(undefined2 *)0x1af0 = 1;
  *(undefined2 *)0x1af2 = 0;
  *(undefined2 *)0x1aee = 0;
  *(undefined2 *)0x1b1e = 0;
  *(uint *)0x1b1c = param_1 | 0x1d;
  *(char *)*(undefined4 *)0x1d20 = (char)(param_1 | 0x1d);
  *(uint *)0x1b1c = param_1 | 9;
  *(char *)*(undefined4 *)0x1d20 = (char)(param_1 | 9);
  *(uint *)0x1b1c = param_1 | 0x1d;
  *(char *)*(undefined4 *)0x1d20 = (char)(param_1 | 0x1d);
  if (*(char *)0x1b7a == '\x01') {
    uVar4 = 0xe06b;
    func_0x000e0add(uVar3);
    *(undefined1 *)0x1b5f = 2;
    *(uint *)0x1b1c = param_1 | 0x1c;
    *(char *)*(undefined4 *)0x1d20 = (char)(param_1 | 0x1c);
    do {
      local_a = local_a + -1;
      if (((local_a < 1) || ((*(byte *)((int)*(undefined4 *)0x1d20 + 5) & 4) != 0)) ||
         ((*(uint *)0x1b1e & 4) != 0)) break;
    } while ((*(uint *)0x1b1c & 1) == 0);
  }
  else {
    func_0x000e5d2d(uVar3);
    *(uint *)0x1b1c = param_1 | 0x1c;
    *(char *)*(undefined4 *)0x1d20 = (char)(param_1 | 0x1c);
    do {
      local_a = local_a + -1;
      if ((local_a < 1) || ((*(uint *)0x1aee & 1) != 0)) break;
    } while ((*(byte *)((int)*(undefined4 *)0x1d20 + 5) & 4) == 0);
    uVar4 = 0xe5d1;
    func_0x000e5d2f(0xe5d1);
    if (((*(byte *)((int)*(undefined4 *)0x1d20 + 5) & 4) != 0) && (*(int *)0x1aee == 0)) {
      *(undefined1 *)0x75b = 1;
    }
  }
  if (local_a < 1) {
    func_0x000e07b4(uVar4,0x607,0xff7b,0x60e,0xff7b,0x617,0xff7b,0xf8);
    *(undefined2 *)0x1b1c = 0xff;
    *(undefined1 *)*(undefined4 *)0x1d20 = 0xff;
    iVar2 = -1;
  }
  else {
    *(uint *)0x1b1c = param_1 | 0x1d;
    *(char *)*(undefined4 *)0x1d20 = (char)(param_1 | 0x1d);
    *(uint *)0x1b1c = param_1 | 5;
    *(char *)*(undefined4 *)0x1d20 = (char)(param_1 | 5);
    bVar1 = *(byte *)((int)*(undefined4 *)0x1d20 + 4);
    *(uint *)0x1b1c = param_1 | 0x1d;
    *(char *)*(undefined4 *)0x1d20 = (char)(param_1 | 0x1d);
    *(uint *)0x1b1c = param_1 | 5;
    *(char *)*(undefined4 *)0x1d20 = (char)(param_1 | 5);
    iVar2 = (uint)bVar1 * 4 + ((int)(uint)*(byte *)((int)*(undefined4 *)0x1d20 + 4) >> 6);
    *(undefined2 *)0x1b1c = 0xff;
    *(undefined1 *)*(undefined4 *)0x1d20 = 0xff;
  }
  return iVar2;
}



/* reset_progress_pattern_e (confidence: Confirmed)
   
   Evidence: Clears `[0x1B1C]` and `es:[0x1D20]` to `0xFF`; called once at the start of
   `selftest_display_irq_idle` */

void __cdecl16far reset_progress_pattern_e(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x1b1c = 0xff;
  *(undefined1 *)*(undefined4 *)0x1d20 = 0xff;
  return;
}



/* selftest_cursor_delta_time (confidence: Confirmed via string reference)
   
   Evidence: Cursor delta-time measurement (`CDT`/`PRE-DETRIG`/`TIME-OUT`, via
   `measure_cursor_delta_time`) */

void __cdecl16far selftest_cursor_delta_time(void)

{
  func_0x000e2cfb();
  return;
}



/* measure_cursor_delta_time (confidence: Confirmed)
   
   Evidence: Implementation for `selftest_cursor_delta_time` - calls `wait_stable_measurement` twice
    */

uint __cdecl16far measure_cursor_delta_time(void)

{
  int iVar1;
  int iVar2;
  undefined2 in_BX;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 local_10;
  
  local_10 = 1;
  uVar5 = 0x9a;
  iVar1 = func_0x000e2dc9();
  uVar4 = 0xe2ce;
  iVar2 = func_0x000e2dc9(0xe2ce,0x8e,0x8a,uVar5);
  iVar3 = iVar1 - iVar2;
  if ((iVar2 == -1) || (iVar1 == -1)) {
    local_10 = 3;
  }
  if ((iVar2 < 0x55) || (0x73 < iVar2)) {
    func_0x000e3395(0xe2ce,iVar2,0xe2);
    uVar4 = 0xe06b;
    func_0x000e07b4(0xe31d,0x594,0xff7b,0x598,0xff7b,in_BX);
    local_10 = local_10 | 2;
  }
  if ((iVar3 < 200) || (0xd2 < iVar3)) {
    func_0x000e3395(uVar4,iVar3,0xe1);
    func_0x000e07b4(0xe31d,0x5a8,0xff7b,0x5ac,0xff7b,in_BX);
    local_10 = local_10 | 2;
  }
  return local_10;
}



undefined2 __stdcall16far FUN_000e_2dbb(void)

{
  return 0;
}



/* wait_stable_measurement (confidence: Mechanism confirmed; peripheral identity not confirmed)
   
   Evidence: Waits for a byte at `[0x32A]` to stabilize across consecutive reads (timeout via a
   countdown), then checks status bits `0x2000`/`0x4000` at `[0x322]` and reports an error if the
   expected bit isn't set */

uint __stdcall16far wait_stable_measurement(undefined1 param_1,undefined2 param_2)

{
  undefined1 uVar1;
  undefined1 uVar2;
  uint *puVar3;
  undefined1 *puVar4;
  uint *puVar5;
  undefined2 unaff_DS;
  undefined2 local_e;
  undefined2 local_c;
  undefined1 local_a;
  undefined1 local_9;
  
  local_e = 0x200;
  local_9 = '\0';
  local_a = -1;
  *(undefined1 *)0x257 = 0;
  *(undefined1 *)0x258 = 0;
  *(undefined1 *)*(undefined4 *)0x336 = 0;
  func_0x000e0add();
  *(undefined1 *)0x1b5f = 1;
  *(char *)*(undefined4 *)0x326 = (char)param_2;
  *(undefined1 *)0x257 = 0;
  *(undefined1 *)0x258 = 0;
  *(undefined1 *)*(undefined4 *)0x336 = 0;
  *(undefined2 *)*(undefined4 *)0x32e = 1;
  uVar2 = *(undefined1 *)((int)*(undefined4 *)0x31e + 0xffe);
  *(undefined1 *)0x257 = param_1;
  *(undefined1 *)*(undefined4 *)0x336 = param_1;
  *(byte *)0x257 = *(byte *)0x257 | 0x10;
  uVar1 = *(undefined1 *)0x257;
  *(undefined1 *)0x258 = uVar1;
  *(undefined1 *)*(undefined4 *)0x336 = uVar1;
  for (; ((local_9 != local_a &&
          (((*(char *)0x1b7a != '\x01' && ((*(uint *)*(undefined4 *)0x322 & 0x8000) != 0)) ||
           ((*(char *)0x1b7a == '\x01' && ((*(uint *)0x1b1e & 0x8000) != 0)))))) && (local_e != 0));
      local_e = local_e + -1) {
    local_9 = *(char *)*(undefined4 *)0x32a;
    local_a = *(char *)*(undefined4 *)0x32a;
  }
  local_c = (uint)*(byte *)*(undefined4 *)0x32a;
  puVar5 = (uint *)(uint *)*(undefined4 *)0x322;
  if ((*(uint *)*(undefined4 *)0x322 & 0x4000) != 0) {
    local_c = local_c | 0x100;
  }
  func_0x000e0add(0xe06b,uVar2);
  puVar3 = (uint *)*(undefined4 *)0x322;
  if ((*puVar3 & 0x2000) == 0) {
    func_0x000e3372(0xe06b,param_2,0xe4);
    func_0x000e07b4(0xe31d,0x5be,0xff7b,0x5c2,0xff7b,puVar5,(int)((ulong)puVar3 >> 0x10));
    local_c = 0xffff;
  }
  *(undefined1 *)0x257 = 0;
  *(undefined1 *)0x258 = 0;
  puVar4 = (undefined1 *)*(undefined4 *)0x336;
  *puVar4 = 0;
  if (local_e < 1) {
    func_0x000e3372(0xe06b,param_2,0xe8);
    func_0x000e07b4(0xe31d,0x5ce,0xff7b,0x5d2,0xff7b,puVar5,(int)((ulong)puVar4 >> 0x10));
    local_c = 0xffff;
  }
  return local_c;
}



/* selftest_front_panel_switch_b (confidence: Mechanism confirmed; which physical control not
   confirmed)
   
   Evidence: Front-panel control test, scans `update_menu_position` over range 0-0x15 */

uint __cdecl16far selftest_front_panel_switch_b(void)

{
  uint uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  
  uVar3 = 1;
  func_0x000e06b6();
  uVar2 = 0xe06b;
  do {
    uVar1 = func_0x000e2ffc(uVar2,3,uVar3);
    uVar2 = 0xe2f4;
  } while ((uVar1 & 0x80) == 0);
  return uVar1;
}



/* step_front_panel_switch_b_test (confidence: Confirmed)
   
   Evidence: Per-position worker for `selftest_front_panel_switch_b` (0-0x15): computes a shifted
   `0xFFE` threshold mask and calls `verify_adc_calibration` - same ADC-readback shape as switch_a
    */

uint __stdcall16far step_front_panel_switch_b_test(undefined2 param_1)

{
  int iVar1;
  uint uVar2;
  undefined2 unaff_DS;
  uint local_a;
  
  *(undefined1 *)0x256 = 0;
  *(undefined1 *)*(undefined4 *)0x326 = 0;
  iVar1 = func_0x000e06b6();
  if (iVar1 != -2) {
    if (iVar1 < 0xb) {
      uVar2 = 0xffe >> (0xbU - (char)iVar1 & 0x1f);
    }
    else {
      uVar2 = 0xffe << ((char)iVar1 - 0xbU & 0x1f);
    }
    local_a = func_0x000e30b5(0xe06b,uVar2 & 0xffe,param_1);
    func_0x000e327f(0xe2f4,uVar2 & 0xffe,0x10,3,0,0,10);
    func_0x000e323f(0xe31d,0x476);
    *(undefined2 *)0x1b5a = 0x476;
    *(undefined2 *)0x1b5c = unaff_DS;
    if (0x14 < iVar1) {
      local_a = local_a | 0x80;
    }
  }
  return local_a;
}



/* verify_adc_calibration (confidence: Confirmed)
   
   Evidence: Compares the 12-bit value field of status register `[0x322]` against a caller-given
   expected value; on mismatch, builds an `ACQ_AB ... uncaled : min=<hex>` error message */

undefined2 __stdcall16far verify_adc_calibration(uint param_1)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 local_36;
  undefined1 local_34 [40];
  uint local_c;
  uint local_a;
  
  local_36 = 1;
  func_0x000e0e56();
  local_a = (uint)*(byte *)(param_1 + (int)*(undefined4 *)0x31e);
  uVar1 = (undefined2)((ulong)*(undefined4 *)0x322 >> 0x10);
  local_c = *(uint *)*(undefined4 *)0x322 & 0xffe;
  if (local_c != param_1) {
    uVar3 = 0x28;
    uVar2 = 0xff7b;
    func_0x000e323f(0xe06b,local_34);
    func_0x000e3372(0xe31d,local_c,0x28,uVar2,uVar3);
    func_0x000e31dc(0xe31d,local_34);
    uVar3 = 0x28;
    uVar2 = 0xff7b;
    func_0x000e31dc(0xe31d,local_34);
    func_0x000e3372(0xe31d,param_1,9,uVar2,uVar3,uVar1);
    func_0x000e07b4(0xe31d,0x58c,0xff7b,local_34);
    local_36 = 3;
  }
  return local_36;
}



/* strncat_far (confidence: Confirmed)
   
   Evidence: `(dest_off, dest_seg, src_off, src_seg, max_len)` - walks `dest` to its existing NUL
   terminator (decrementing `max_len` as it goes), then copies bytes from `src` until NUL or
   `max_len` is exhausted, re-terminates. A bounded string-append, i.e. `strncat` */

void __stdcall16far strncat_far(word dest_off,word dest_seg,word src_off,word src_seg,word max_len)

{
  word wVar1;
  bool bVar2;
  undefined4 local_c;
  
  local_c = (char *)CONCAT22(dest_seg,dest_off);
  while ((*local_c != '\0' &&
         (wVar1 = max_len - 1, bVar2 = 0 < (int)max_len, max_len = wVar1, bVar2))) {
    local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
  }
  while ((*_src_off != '\0' && (0 < (int)max_len))) {
    *local_c = *_src_off;
    local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
    _src_off = (char *)CONCAT22(src_seg,(char *)(src_off + 1));
    max_len = max_len - 1;
  }
  *local_c = '\0';
  return;
}



/* strncpy_far (confidence: Confirmed)
   
   Evidence: `(dest far ptr, src far ptr, max_len)` - same shape as `strncat_far` but copies from
   `dest`'s *start* rather than finding its existing end first - a bounded string copy, i.e.
   `strncpy` */

void __stdcall16far strncpy_far(word dest_off,word dest_seg,word src_off,word src_seg,word max_len)

{
  char *pcVar1;
  char *pcVar2;
  
  while ((pcVar2 = _src_off, pcVar1 = _dest_off, *_src_off != '\0' && (0 < (int)max_len))) {
    _dest_off = (char *)CONCAT22(dest_seg,(char *)(dest_off + 1));
    _src_off = (char *)CONCAT22(src_seg,(char *)(src_off + 1));
    *pcVar1 = *pcVar2;
    max_len = max_len - 1;
  }
  *_dest_off = '\0';
  return;
}



/* format_number (confidence: Confirmed)
   
   Evidence: `(value, radix, width, overflow_flag, extra)` - the core integer-to-ASCII formatter:
   divides by `radix` repeatedly, builds digits backwards into the fixed scratch buffer `[0x1B34]`
   (hex digits `>9` get `+0x37` for `A`-`F`), handles a leading `-`/space sign column and an `*`
   overflow marker, returns a far pointer to the resulting string. The likely engine behind this
   firmware's printf-style `%d`/`%c` formatting (see `STRINGS.md`) */

void __stdcall16far format_number(word value,word radix,word width,byte overflow_flag,word extra)

{
  char cVar1;
  undefined1 uVar2;
  undefined2 unaff_DS;
  int local_c;
  word local_a;
  
  local_c = 0;
  if (((value & 0x8000) != 0) && (overflow_flag == 1)) {
    value = value + 0x8000;
    local_c = -1;
  }
  *(undefined1 *)(width + 0x1b35) = 0;
  local_a = width;
  if (value == 0) {
    *(undefined1 *)(width + 0x1b34) = 0x30;
    extra = extra - 1;
    local_a = width - 1;
  }
  while ((value != 0 && (-1 < (int)local_a))) {
    if ((int)(value % radix) < 10) {
      cVar1 = '0';
    }
    else {
      cVar1 = '7';
    }
    *(char *)(local_a + 0x1b34) = (char)(value % radix) + cVar1;
    value = value / radix;
    extra = extra - 1;
    local_a = local_a - 1;
  }
  for (; (-1 < (int)local_a && (0 < (int)extra)); extra = extra - 1) {
    *(undefined1 *)(local_a + 0x1b34) = 0x30;
    local_a = local_a - 1;
  }
  while (-1 < (int)local_a) {
    if (local_c == -1) {
      uVar2 = 0x2d;
    }
    else {
      uVar2 = 0x20;
    }
    *(undefined1 *)(local_a + 0x1b34) = uVar2;
    local_c = local_c + 1;
    local_a = local_a - 1;
  }
  if (value != 0) {
    *(undefined1 *)0x1b34 = 0x2a;
  }
  return;
}



/* format_hex_word (confidence: Confirmed)
   
   Evidence: `(value)` - `format_number` wrapper: fixed `radix=16, width=5` */

void __stdcall16far format_hex_word(word value)

{
  func_0x000e327f();
  return;
}



/* format_decimal_word (confidence: Confirmed)
   
   Evidence: `(value)` - `format_number` wrapper: fixed `radix=10, width=6` */

void __stdcall16far format_decimal_word(word value)

{
  func_0x000e327f();
  return;
}



/* format_word_radix (confidence: Confirmed)
   
   Evidence: `(value, radix)` - `format_number` wrapper: caller-supplied `radix`, fixed `width=5` */

void __stdcall16far format_word_radix(word value,word radix)

{
  func_0x000e327f();
  return;
}



void __stdcall16far FUN_000e_33dd(int param_1,int param_2,int param_3)

{
  bool bVar1;
  int iVar2;
  undefined1 uVar3;
  undefined2 unaff_DS;
  int local_c;
  int local_a;
  
  local_c = 0;
  bVar1 = false;
  param_2 = param_2 / 10;
  if (param_1 < 0) {
    param_1 = -param_1;
    local_c = -1;
  }
  *(undefined1 *)(param_3 + 0x1b35) = 0;
  iVar2 = param_3;
  if (param_1 == 0) {
    *(undefined1 *)(param_3 + 0x1b34) = 0x30;
    iVar2 = param_3 + -1;
  }
  while (((local_a = iVar2, 0 < param_1 && (-1 < local_a)) || (0 < param_2))) {
    *(char *)(local_a + 0x1b34) = (char)(param_1 % 10) + '0';
    param_1 = param_1 / 10;
    param_2 = param_2 / 10;
    iVar2 = local_a + -1;
    if ((param_2 == 0) && (!bVar1)) {
      *(undefined1 *)(local_a + 0x1b33) = 0x2e;
      bVar1 = true;
      iVar2 = local_a + -2;
    }
  }
  while (-1 < local_a) {
    if (local_c == -1) {
      uVar3 = 0x2d;
    }
    else {
      uVar3 = 0x20;
    }
    *(undefined1 *)(local_a + 0x1b34) = uVar3;
    local_c = local_c + 1;
    local_a = local_a + -1;
  }
  if (param_1 != 0) {
    *(undefined1 *)0x1b34 = 0x2a;
  }
  return;
}



/* format_byte_hex (confidence: Confirmed)
   
   Evidence: `(value)` - byte -> 2 hex ASCII digits + NUL, written to a *separate* fixed scratch
   buffer at `[0x1B4A]` (not `format_number`'s `[0x1B34]`) - a simpler, dedicated byte-to-hex
   formatter */

void __stdcall16far format_byte_hex(byte value)

{
  undefined2 unaff_DS;
  
  *(char *)0x1b4a = (char)((int)(value & 0xf0) >> 4) + '0';
  if (0x39 < *(byte *)0x1b4a) {
    *(char *)0x1b4a = *(char *)0x1b4a + '\a';
  }
  *(char *)0x1b4b = (value & 0xf) + 0x30;
  if (0x39 < *(byte *)0x1b4b) {
    *(char *)0x1b4b = *(char *)0x1b4b + '\a';
  }
  *(undefined1 *)0x1b4c = 0;
  return;
}



/* build_print_record (confidence: Confirmed)
   
   Evidence: Orchestrates `init_print_record` -> `set_position_record` -> `pack_row_col_bits` into
   one combined record-building call - the general entry point behind `init_print_region` */

void __stdcall16far
build_print_record(undefined4 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
                  undefined2 param_5,undefined2 param_6,undefined4 param_7)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  
  uVar2 = (undefined2)((ulong)param_7 >> 0x10);
  uVar1 = (undefined2)param_7;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  func_0x000e35ef();
  func_0x000e36a5(0xe352,(int)param_1,uVar3,param_2,param_4,uVar1,uVar2);
  func_0x000e3662(0xe352,(int)param_1,uVar3,param_3,param_5);
  return;
}



/* init_print_region (confidence: Confirmed)
   
   Evidence: `build_print_record` wrapper fixing the cell-size arguments to `0x10 x 0x10` (16x16)
   with no position override - the common "start a normal report line" case, used by
   `print_selftest_banner` */

void __stdcall16far init_print_region(void)

{
  func_0x000e3520();
  return;
}



/* init_print_region_home (confidence: Confirmed)
   
   Evidence: `build_print_record` wrapper: fixed `0x10x0x10` cell size, position override `=
   [0x45E]` (a saved copy of the readout buffer's own base pointer `[0x1CC4]`) - starts a print
   record at the readout's "home" position */

void __stdcall16far init_print_region_home(void)

{
  func_0x000e3520();
  return;
}



/* build_print_region (confidence: Confirmed)
   
   Evidence: `build_print_record` wrapper passing all 4 position/size arguments straight through -
   the general form behind `init_print_region`'s fixed-size convenience wrapper; used by
   `print_selftest_report_line` for its computed per-row position */

void __stdcall16far build_print_region(void)

{
  func_0x000e3520();
  return;
}



/* init_print_record (confidence: Confirmed)
   
   Evidence: Sets a caller-supplied print record's attribute byte and position: if given a `(0,0)`
   far-ptr override, defaults to the current global cursor `[0x1AF4]`; otherwise moves the global
   cursor to the given position first */

void __stdcall16far init_print_record(undefined1 *param_1,undefined1 param_2,undefined4 param_3)

{
  int iVar1;
  undefined1 *puVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  undefined1 local_c;
  undefined1 uStack_b;
  
  *param_1 = param_2;
  if ((int)param_3 == 0 && param_3._2_2_ == 0) {
    param_3 = *(undefined4 *)0x1af4;
  }
  else {
    *(int *)0x1af4 = (int)param_3;
    *(int *)0x1af6 = param_3._2_2_;
  }
  iVar1 = (int)param_3 + (int)((ulong)param_3 >> 0x10) * 0x10;
  local_c = (undefined1)iVar1;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  puVar2 = (undefined1 *)param_1;
  puVar2[5] = local_c;
  uStack_b = (undefined1)((uint)iVar1 >> 8);
  puVar2[6] = uStack_b;
  puVar2[7] = local_c;
  puVar2[8] = uStack_b;
  return;
}



/* pack_row_col_bits (confidence: Confirmed mechanism; exact bit-field semantics not fully mapped)
   
   Evidence: Bit-packs two 5-bit fields into a print record's bytes 1 and 3, preserving each byte's
   high 3 bits - a compact coordinate/attribute packing helper */

void __stdcall16far pack_row_col_bits(undefined4 param_1,byte param_2,byte param_3)

{
  int iVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  *(byte *)(iVar1 + 3) = param_2 & 0x1f | *(byte *)(iVar1 + 3) & 0xe0;
  *(byte *)(iVar1 + 1) = param_3 & 0x1f | *(byte *)(iVar1 + 3) & 0xe0;
  return;
}



/* set_position_record (confidence: Confirmed mechanism; the `>>3` strongly suggests
   pixel-to-character-cell conversion (8-pixel cells))
   
   Evidence: Encodes two coordinates (each `>>3`, i.e. divided by 8 - character-cell granularity)
   into a print record's bytes 1-4 */

void __stdcall16far set_position_record(undefined4 param_1,uint param_2,uint param_3)

{
  int iVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  *(undefined1 *)(iVar1 + 4) = (char)((int)param_2 >> 3);
  *(byte *)(iVar1 + 3) = *(byte *)(iVar1 + 3) & 0x1f | (byte)((param_2 & 7) << 5);
  *(undefined1 *)(iVar1 + 2) = (char)((int)param_3 >> 3);
  *(byte *)(iVar1 + 1) = *(byte *)(iVar1 + 1) & 0x1f | (byte)((param_3 & 7) << 5);
  return;
}



void __stdcall16far FUN_000e_371e(undefined1 *param_1)

{
  func_0x000e3766();
  *param_1 = 0x3d;
  return;
}



/* close_print_record_b (confidence: Confirmed mechanism; the two codes' meaning is a guess)
   
   Evidence: Same shape as `close_print_record` but tags with `0x39` instead of `0x11` - a different
   record "kind"/completion code. Exact meaning of `0x11` vs `0x39` not confirmed */

void __stdcall16far close_print_record_b(undefined1 *param_1)

{
  func_0x000e3766();
  *param_1 = 0x39;
  return;
}



/* close_print_record (confidence: Confirmed)
   
   Evidence: `(record_off, record_seg)` - calls `mark_readout_delimiter`, then tags the caller's
   record (byte 0) with completion code `0x11`. **Correction**: earlier notes/pseudocode wrongly
   assumed this printed a string via its far-pointer argument (same shape as `print_string_far`) -
   it doesn't; it only tags the record. Fixed in `PSEUDOCODE.md` and
   `docs/display/vector-display-and-stroke-font.md` */

void __stdcall16far close_print_record(word record_off,word record_seg)

{
  func_0x000e3766();
  *_record_off = 0x11;
  return;
}



/* mark_readout_delimiter (confidence: Confirmed mechanism; the marker's exact semantic role in
   whatever consumes this buffer isn't confirmed)
   
   Evidence: Writes attribute code `2` into the *next 2* readout-buffer cells' attribute plane and
   advances the cursor by 2, without writing anything to the primary plane. `2` is the same code
   `plot_readout_point`'s circular-buffer wraparound handler uses as its own marker - likely a
   reserved "delimiter" attribute throughout this buffer format */

void __cdecl16far mark_readout_delimiter(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x1af4 + 1) = 2;
  *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x1af4 + 2) = 2;
  *(int *)0x1af4 = *(int *)0x1af4 + 1;
  *(int *)0x1af4 = *(int *)0x1af4 + 1;
  return;
}



void __stdcall16far FUN_000e_3792(void)

{
  return;
}



void __stdcall16far FUN_000e_379e(undefined1 param_1)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x1afc = param_1;
  return;
}



/* plot_readout_point_scaled (confidence: Confirmed)
   
   Evidence: Divides both coordinate arguments by a local count (hardcoded to `1` in this build - a
   no-op divide, vestige of generic code once supporting a count > 1) and calls
   `plot_readout_point_relative` */

void __stdcall16far plot_readout_point_scaled(byte param_1,byte param_2)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 local_a;
  
  for (local_a = 0; local_a < 1; local_a = local_a + 1) {
    func_0x000e3900(unaff_CS,param_1 / 1,param_2 / 1,*(byte *)0x1afc | 9);
    unaff_CS = 0xe352;
  }
  return;
}



void __stdcall16far FUN_000e_37fe(void)

{
  func_0x000e3900();
  return;
}



/* print_readout_string (confidence: Confirmed)
   
   Evidence: `(str_off, str_seg)` - loops a far-pointer nul-terminated string, calling
   `draw_readout_char` per byte - the vector-display-list sibling of `print_string_far` (which
   instead writes straight to the `0x40000+0x6F0` hardware port) */

void __stdcall16far print_readout_string(word str_off,word str_seg)

{
  undefined2 unaff_CS;
  undefined2 local_a;
  
  local_a = 0;
  while( true ) {
    if (*(char *)(local_a + str_off) == '\0') break;
    func_0x000e3854(unaff_CS,*(char *)(local_a + str_off));
    unaff_CS = 0xe352;
    local_a = local_a + 1;
  }
  return;
}



/* draw_readout_char (confidence: Confirmed mechanism and exact arithmetic; the downstream stage
   that converts these internal display-list coordinates into real CRT/plotter output is still
   unfound)
   
   Evidence: Looks a character up (`char & 0x7F`, `*4`) in a stroke-font table at `[0x1DB0]`, then
   walks each stroke byte of the resulting far pointer - extracting a pen-up/down bit (`0x80`), a
   coarse component (bits 4-6), and a fine component (bits 0-3) - calling `plot_readout_point`
   **(corrected 2026-09-15 - not `plot_readout_point_relative`, verified against the actual `lcall`
   target)** once per stroke, with `Y = baseline([0x1AF8], captured once at function entry) +
   coarse` (raw add, not scaled) and `X = fine` (raw, unscaled, no per-character offset). The
   character rasterizer for the vector-stroke readout system; see
   `docs/display/vector-display-and-stroke-font.md` "The readout vector display list" and its
   "Follow-up, 2026-09-15" section */

void __stdcall16far draw_readout_char(byte param_1)

{
  byte bVar1;
  byte *pbVar2;
  uint uVar3;
  undefined1 uVar4;
  undefined1 uVar5;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  byte *local_e;
  
  if (*"" != '\x14') {
    local_e = (byte *)*(undefined4 *)((param_1 & 0x7f) * 4 + (int)*(undefined4 *)0x1db0);
    bVar1 = *(byte *)0x1af8;
    if ((param_1 & 0x80) == 0) {
      uVar4 = 1;
    }
    else {
      uVar4 = 3;
    }
    while( true ) {
      pbVar2 = local_e;
      local_e = (byte *)CONCAT22(local_e._2_2_,(byte *)local_e + 1);
      uVar3 = (uint)*pbVar2;
      if (uVar3 == 0) break;
      uVar5 = uVar4;
      if ((*pbVar2 & 0x80) == 0) {
        uVar5 = 0;
      }
      func_0x000e3930(unaff_CS,((int)(uVar3 & 0x70) >> 4) + (uint)bVar1,uVar3 & 0xf,uVar5);
      unaff_CS = 0xe352;
    }
  }
  return;
}



/* plot_readout_point_relative (confidence: Confirmed)
   
   Evidence: `(dx, dy, attr)` - adds the offsets to the current base position
   (`[0x1AF8]`/`[0x1AFA]`) and calls `plot_readout_point` */

void __stdcall16far plot_readout_point_relative(byte dx,byte dy,byte attr)

{
  func_0x000e3930();
  return;
}



/* plot_readout_point (confidence: Confirmed)
   
   Evidence: `(x, y, attr)` - THE fundamental primitive: updates the current position
   (`[0x1AF8]`/`[0x1AFA]`), appends a `(y, x)` coordinate pair plus a duplicated attribute byte to
   the readout vector display-list buffer at `[0x1CC4]` (tracked via `[0x1AF4]`/`[0x1AF6]`, second
   plane offset in `[0x1C02]`), and handles circular-buffer wraparound (marker attribute `2`,
   matching `mark_readout_delimiter`) */

void __stdcall16far plot_readout_point(byte x,byte y,byte attr)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  
  *(uint *)0x1af8 = (uint)x;
  *(uint *)0x1afa = (uint)y;
  *(byte *)*(undefined4 *)0x1af4 = y;
  uVar1 = *(undefined4 *)0x1af4;
  *(int *)0x1af4 = *(int *)0x1af4 + 1;
  *(byte *)(*(int *)0x1c02 + (int)uVar1) = attr;
  *(byte *)*(undefined4 *)0x1af4 = x;
  uVar1 = *(undefined4 *)0x1af4;
  *(int *)0x1af4 = *(int *)0x1af4 + 1;
  *(byte *)(*(int *)0x1c02 + (int)uVar1) = attr;
  if ((*(char *)0x1b7a == '\x01') &&
     ((int)*(undefined4 *)0x1cc4 + 0x1400U < (uint)*(undefined4 *)0x1af4)) {
    uVar3 = (undefined2)((ulong)*(undefined4 *)0x1cc4 >> 0x10);
    iVar2 = (int)*(undefined4 *)0x1cc4;
    *(int *)0x1af4 = iVar2 + 0x13ff;
    *(undefined2 *)0x1af6 = uVar3;
    *(undefined1 *)(iVar2 + *(int *)0x1c02 + 0x13ff) = 2;
    uVar1 = *(undefined4 *)0x1af4;
    *(int *)0x1af4 = *(int *)0x1af4 + 1;
    *(undefined1 *)(*(int *)0x1c02 + (int)uVar1) = 2;
  }
  return;
}



undefined2 __cdecl16far FUN_000e_39c8(void)

{
  undefined2 unaff_DS;
  
  return *(undefined2 *)0x1af8;
}



undefined2 __cdecl16far FUN_000e_39d5(void)

{
  undefined2 unaff_DS;
  
  return *(undefined2 *)0x1afa;
}



void __cdecl16far FUN_000e_39e2(void)

{
  return;
}



/* append_readout_char (confidence: Confirmed mechanism; which plane is "primary" vs
   "attribute/shadow" not yet confirmed)
   
   Evidence: Appends a byte at the current write pointer of a running line buffer (`[0x1AF4]`), then
   writes a second byte at the same offset `+0x8000` - a dual-plane buffer in the same `0x4000`
   segment. See `docs/display/readout-memory.md` "The readout/CRT display memory" */

void __stdcall16far append_readout_char(undefined1 param_1,undefined1 param_2)

{
  undefined4 uVar1;
  undefined2 unaff_DS;
  
  *(undefined1 *)*(undefined4 *)0x1af4 = param_1;
  uVar1 = *(undefined4 *)0x1af4;
  *(int *)0x1af4 = *(int *)0x1af4 + 1;
  *(undefined1 *)((int)uVar1 + *(int *)0x1c02) = param_2;
  return;
}



void __stdcall16far FUN_000e_3a1a(undefined1 *param_1,undefined1 param_2,undefined1 param_3)

{
  undefined2 unaff_DS;
  
  *param_1 = param_2;
  ((undefined1 *)param_1)[*(int *)0x1c02] = param_3;
  return;
}



void __stdcall16far FUN_000e_3a3b(void)

{
  undefined2 unaff_DS;
  
  func_0x000e3a63();
  *(int *)0x1af4 = *(int *)0x1af4 + 1;
  *(int *)0x1af4 = *(int *)0x1af4 + 1;
  return;
}



void __stdcall16far FUN_000e_3a63(int *param_1,int param_2,undefined1 param_3)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  uVar1 = (undefined2)((ulong)param_1 >> 0x10);
  *param_1 = param_2 << 4;
  *(undefined1 *)((int)(int *)param_1 + *(int *)0x1c02) = param_3;
  *(undefined1 *)((int)(int *)param_1 + *(int *)0x1c02 + 1) = param_3;
  return;
}



/* run_selftest_sequence (confidence: Confirmed)
   
   Evidence: **The top-level self-test orchestrator.** Writes a marker byte at physical `0x00000`,
   initializes the readout display-list buffer, runs setup (`SUB_E4443`/`SUB_E75C0`/`SUB_E128D`),
   picks a test-mode byte `[0x1B48]` from `[0x758]`, calls `init_selftest_report_screen`, then calls
   `print_selftest_report_line` once immediately followed by `self_test_dispatcher` - ties together
   `print_selftest_banner`, `self_test_dispatcher`, and the readout buffer findings from across
   multiple sessions into one place */

void __cdecl16far run_selftest_sequence(void)

{
  byte *pbVar1;
  byte bVar2;
  bool bVar3;
  undefined4 uVar4;
  undefined2 *puVar5;
  bool bVar6;
  uint uVar7;
  int iVar8;
  undefined2 uVar9;
  undefined2 unaff_DS;
  bool bVar10;
  undefined2 uVar11;
  int local_e;
  int local_c;
  int local_a;
  
  local_a = 0;
  local_c = 0x200;
  local_e = 1;
  bVar6 = false;
  bVar3 = false;
  uRam00000000 = 0xcb;
  uVar4 = *(undefined4 *)0x1cc4;
  *(undefined2 *)0x1af4 = (int)uVar4;
  *(undefined2 *)0x1af6 = (int)((ulong)uVar4 >> 0x10);
  *(undefined2 *)0x1c02 = 0x8000;
  uVar11 = 1;
  func_0x000e4443();
  func_0x000e75c0(0xe3b1,uVar11);
  uVar11 = 0xe0e8;
  func_0x000e128d(0xe75c);
  if (((*(byte *)0x758 & 99) == 0) && ((*(byte *)0x758 & 0x80) != 0)) {
    *(undefined1 *)0x1b48 = 0;
  }
  else {
    *(undefined1 *)0x1b48 = 3;
  }
  if ((*(byte *)0x758 & 99) != 0) {
    *(undefined2 *)0x1ae6 = 0x3fff;
    *(undefined1 *)0x1b48 = 1;
    bVar6 = true;
    uVar9 = uVar11;
LAB_000e_3bae:
    if (local_e == 1) {
      func_0x000e4680(uVar9,1);
      uVar9 = 0xe3b1;
      func_0x000e43a0(0xe45d);
      local_e = 3;
    }
    bVar10 = 0x1ff < local_c;
    uVar11 = uVar9;
    local_c = local_c + 1;
    if (bVar10) {
      local_c = 0;
      if ((bVar3) && ((*(byte *)0x758 & 99) != 0)) goto LAB_000e_3c26;
      if ((*(byte *)0x758 & 99) == 0) {
        bVar3 = true;
      }
      uVar11 = 0xe3b1;
      func_0x000e440a(uVar9);
    }
    bVar10 = local_a == *(int *)0x1ae6;
    uVar9 = uVar11;
    local_a = local_a + 1;
    if (bVar10) {
      local_c = local_c + 1;
      local_a = 0;
      uVar9 = 0xe45d;
      func_0x000e4680(uVar11,local_e);
    }
    goto LAB_000e_3bae;
  }
LAB_000e_3c2b:
  uVar9 = 0xe45d;
  func_0x000e4680(uVar11,2);
  if (bVar6) {
    bVar3 = false;
    uVar11 = 0xe45d;
    func_0x000e5b34(0xe45d,1);
    while( true ) {
      for (local_c = 9; 0 < local_c; local_c = local_c + -1) {
        func_0x000e0add(uVar11);
        uVar11 = 0xe06b;
      }
      if ((bVar3) && ((*(byte *)0x758 & 99) != 0)) break;
      if ((*(byte *)0x758 & 99) == 0) {
        bVar3 = true;
      }
      func_0x000e5b34(uVar11,3);
      func_0x000e5b34(0xe45d,4);
      uVar11 = 0xe3b1;
      func_0x000e440a(0xe45d);
    }
    func_0x000e5b34(uVar11,2);
    func_0x000e4429(0xe45d);
    bVar3 = false;
    pbVar1 = (byte *)((int)*(undefined4 *)0x1ddc + 0x213);
    *pbVar1 = *pbVar1 | 0x80;
    uVar11 = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
    iVar8 = (int)*(undefined4 *)0x1ddc + 0x20a;
    *(int *)0x1bfe = iVar8;
    *(undefined2 *)0x1c00 = uVar11;
    uVar11 = func_0x000e6d2f(0xe3b1,iVar8,uVar11);
    *(undefined2 *)*(undefined4 *)0x1de0 = uVar11;
    uVar11 = 0xe45d;
    func_0x000e591d(0xe6a8,1);
    while( true ) {
      for (local_c = 9; 0 < local_c; local_c = local_c + -1) {
        func_0x000e0add(uVar11);
        uVar11 = 0xe06b;
      }
      if (((bVar3) && ((*(byte *)0x758 & 0x23) != 0)) ||
         ((bVar3 && (((*(byte *)0x758 & 0x40) != 0 && (*"" == '\x1e')))))) break;
      if ((*(byte *)0x758 & 99) == 0) {
        bVar3 = true;
      }
      func_0x000e591d(uVar11,3);
      func_0x000e591d(0xe45d,4);
      uVar11 = 0xe3b1;
      func_0x000e440a(0xe45d);
    }
    func_0x000e591d(uVar11,2);
    uVar9 = 0xe3b1;
    func_0x000e4429(0xe45d);
    if (*"" == '\x15') {
      uVar9 = 0xefb6;
      func_0x000efba5(0xe3b1);
    }
  }
  *(undefined2 *)0x1b10 = 0;
  pbVar1 = (byte *)((int)*(undefined4 *)0x1ddc + 0x291);
  *pbVar1 = *pbVar1 | 0x80;
  uVar11 = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
  iVar8 = (int)*(undefined4 *)0x1ddc + 0x20a;
  *(int *)0x1bfe = iVar8;
  *(undefined2 *)0x1c00 = uVar11;
  uVar11 = func_0x000e6d2f(uVar9,iVar8,uVar11);
  puVar5 = (undefined2 *)*(undefined4 *)0x1de0;
  *puVar5 = uVar11;
  uVar11 = 0xe3b1;
  func_0x000e40ce(0xe6a8);
  if (*(int *)0x1ae4 != 0) {
    *(undefined2 *)0x1b18 = 1;
    func_0x000e33b8(0xe3b1,*(undefined2 *)0x1ae4,4,0xf);
    uVar11 = 0xe06b;
    func_0x000e07b4(0xe31d,0x486,0xff7b,0x489,0xff7b,iVar8,(int)((ulong)puVar5 >> 0x10));
  }
  uVar7 = func_0x000e4244(uVar11);
  if ((uVar7 & 2) == 0) {
    uVar7 = 0;
  }
  else {
    uVar7 = 0x4000;
  }
  *(uint *)0x1ae4 = *(uint *)0x1ae4 | uVar7;
  func_0x000e0af5(0xe3b1,0x497,0xff7b);
  pbVar1 = (byte *)((int)*(undefined4 *)0x1ddc + 0x291);
  *pbVar1 = *pbVar1 | 0x80;
  uVar11 = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
  iVar8 = (int)*(undefined4 *)0x1ddc + 0x20a;
  *(int *)0x1bfe = iVar8;
  *(undefined2 *)0x1c00 = uVar11;
  uVar9 = 0xe6a8;
  uVar11 = func_0x000e6d2f(0xe06b,iVar8,uVar11);
  *(undefined2 *)*(undefined4 *)0x1de0 = uVar11;
  if (*(int *)0x1ae4 != 0) {
    local_c = 0x200;
    bVar2 = *(byte *)0x758;
    if (*"" == '\x14') {
      uVar9 = 0xe3b1;
      func_0x000e3e97(0xe6a8);
    }
    while( true ) {
      do {
        bVar3 = local_c < 0x200;
        local_c = local_c + 1;
      } while (bVar3);
      local_c = 0;
      if ((*(byte *)0x758 & 99) != (bVar2 & 99)) break;
      func_0x000e440a(uVar9);
      uVar9 = 0xe06b;
      func_0x000e0a23(0xe3b1);
    }
  }
  func_0x000e4429(uVar9);
  return;
LAB_000e_3c26:
  uVar11 = 0xe3b1;
  func_0x000e43f2(uVar9);
  goto LAB_000e_3c2b;
}



/* draw_display_test_pattern (confidence: Mechanism confirmed)
   
   Evidence: Called from `run_selftest_sequence`'s wrap-up phase (right after "Power up tests
   complete."), gated on `[0x1B83]==0x14`: draws two 250-point diagonal lines via
   `plot_readout_point_scaled` - the readout test pattern for the "MI / line stuck high"
   interrupt-line check */

void __cdecl16far draw_display_test_pattern(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 local_a;
  
  uVar2 = 0;
  uVar1 = 0;
  func_0x000e3567();
  func_0x000e3930(0xe352,0,0,0,uVar1,uVar2);
  for (local_a = 0; local_a < 0xfa; local_a = local_a + 1) {
    func_0x000e37b1(0xe352,local_a - *(int *)0x1af8,local_a - *(int *)0x1afa);
  }
  for (local_a = 0; local_a < 0xfa; local_a = local_a + 1) {
    func_0x000e37b1(0xe352,local_a - *(int *)0x1af8,(0xfa - local_a) - *(int *)0x1afa);
  }
  func_0x000e374e(0xe352,(int)*(undefined4 *)0x1ddc + 0x20a,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
  return;
}



/* selftest_display_irq_idle (confidence: Mechanism confirmed; confirmed this function itself passes
   and is not the TIMEOUT source)
   
   Evidence: Readout/CRT display controller interrupt line, idle-state check (`MI`/`line stuck
   high`/`Display controller`): zeroes `[0x1AEE]`, calls `enable_interrupts`/`disable_interrupts`
   (`0xE5D2D`/`0xE5D2F`, literal `sti`/`cli`) back-to-back with no code between them, then would
   fail with a "line stuck asserted while idle" message if `[0x1AEE]` were nonzero afterward.
   **Corrected 2026-09-18**: a fresh instruction-level trace proved this function actually PASSES in
   the emulator - `[0x1AEE]`/`[0x1AF2]` are both `0` immediately before and after the `sti`/`cli`
   window, so its own `je` branch to the pass path is taken, and it falls straight through into
   `selftest_display_irq_active` next. The previously-documented "`MI : Display controller :
   TIMEOUT`" failure was wrongly attributed to *this* function (see `emulator/docs/design.md`'s
   superseded 2026-09-16 section) - it's actually produced by `selftest_display_irq_active` below */

undefined2 __cdecl16far selftest_display_irq_idle(void)

{
  undefined2 unaff_DS;
  undefined2 local_a;
  
  local_a = 1;
  func_0x000e4429();
  func_0x000e0e56(0xe3b1);
  func_0x000e2cd3(0xe06b);
  *(undefined2 *)0x1af0 = 1;
  *(undefined2 *)0x1af2 = 0;
  *(undefined2 *)0x1aee = 0;
  func_0x000e5d2d(0xe296);
  func_0x000e5d2f(0xe5d1);
  if (*(int *)0x1aee != 0) {
    func_0x000e07b4(0xe5d1,0x4bd,0xff7b,0x4c0,0xff7b,0x4d0,0xff7b,0x12);
    local_a = 3;
  }
  return local_a;
}



/* selftest_display_irq_active (confidence: Fully traced and confirmed 2026-09-18; fixed in the
   emulator via `io_stubs.DisplayChipIrqStub` (see `emulator/docs/design.md`))
   
   Evidence: Readout/CRT display controller interrupt line, active check after drawing a test shape.
   Sets a trigger bit (`or byte es:[di+0x201], 0x80` on the far-pointer struct at `[0x1DDC]`), draws
   and commits a small test vector (coordinates derived from `[0x1AFA]`/`[0x1AF8]`), zeroes
   `[0x1AF0]`/`[0x1AF2]`/`[0x1AEE]`, enables interrupts, reads the Display Chip Interrupt Reset
   register (physical `0x41000`, Table 3-1) via a helper at `0xE3B1:0x8FA`, then busy-polls
   `[0x1AEE]` for up to 100 iterations (interrupts left enabled throughout) - prints **"`MI :
   Display controller : TIMEOUT`"** (confirmed by decoding the ROM string table at
   `0xFF7B0+0x4d1/0x4d4/0x4e9`) and fails if `[0x1AEE]` never goes nonzero. `[0x1AEE]` only becomes
   nonzero via `INT2_HANDLER_EARLY` (`0xE5D67`) ORing `[0x1AF2]` into it on an NMI tick - and
   nothing in this emulator ever set `[0x1AF2]`, so the loop always exhausted, which is the *real*
   source of the TIMEOUT message (not `selftest_display_irq_idle`, corrected above). Regardless of
   that outcome, it then calls a reset routine (`0xE3B1:0x919`), re-zeroes `[0x1AEE]`, does a second
   bare `sti`/`cli` window, and prints a second, different message, **"`MI : Display controller :
   unable to reset mi[splay controller]`"** (string table offsets `0x4f2/0x4f5/0x50a`) if `[0x1AEE]`
   is nonzero *immediately* after that (not yet observed in practice - the reset call doesn't
   re-read `0x41000`, so `[0x1AEE]` stays at the `0` this function itself just set) */

uint __cdecl16far selftest_display_irq_active(void)

{
  byte *pbVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  uint local_c;
  int local_a;
  
  local_a = 100;
  local_c = 1;
  pbVar1 = (byte *)((int)*(undefined4 *)0x1ddc + 0x201);
  *pbVar1 = *pbVar1 | 0x80;
  uVar4 = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
  iVar3 = (int)*(undefined4 *)0x1ddc + 0x1f8;
  *(int *)0x1bfe = iVar3;
  *(undefined2 *)0x1c00 = uVar4;
  uVar2 = func_0x000e6d2f();
  *(undefined2 *)*(undefined4 *)0x1de0 = uVar2;
  func_0x000e3567(0xe6a8,(int)*(undefined4 *)0x1ddc + 0x1f8,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0,0,iVar3,uVar4);
  func_0x000e3930(0xe352,0,0,0);
  func_0x000e37b1(0xe352,100 - *(int *)0x1af8,100 - *(int *)0x1afa);
  func_0x000e374e(0xe352,(int)*(undefined4 *)0x1ddc + 0x1f8,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
  *(undefined2 *)0x1af0 = 1;
  *(undefined2 *)0x1af2 = 0;
  *(undefined2 *)0x1aee = 0;
  func_0x000e5d2d(0xe352);
  uVar2 = 0xe3b1;
  func_0x000e440a(0xe5d1);
  do {
    local_a = local_a + -1;
    if (local_a < 1) break;
  } while (*(int *)0x1aee == 0);
  if (local_a < 1) {
    uVar2 = 0xe06b;
    func_0x000e07b4(0xe3b1,0x4d1,0xff7b,0x4d4,0xff7b,0x4e9,0xff7b,0x18);
    local_c = 3;
  }
  func_0x000e4429(uVar2);
  *(undefined2 *)0x1aee = 0;
  func_0x000e5d2d(0xe3b1);
  func_0x000e5d2f(0xe5d1);
  if (*(int *)0x1aee != 0) {
    func_0x000e07b4(0xe5d1,0x4f2,0xff7b,0x4f5,0xff7b,0x50a,0xff7b,0x14);
    local_c = local_c | 2;
  }
  *(undefined1 *)((int)*(undefined4 *)0x1ddc + 0x1f8) = 0x13;
  return local_c;
}



/* print_boot_rom_id_banner (confidence: Confirmed)
   
   Evidence: Gated on `[0x1B48]!=0`; prints `2230/2220 boot : 160-` plus ROM ID/revision bytes from
   `[0x1DD4]+2/+3/+4` (via `format_byte_hex`) and "POWER UP FAILURES"-area text, all via
   `print_string_far` (to the CRT readout) */

void __cdecl16far print_boot_rom_id_banner(void)

{
  undefined2 in_BX;
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  if (*(char *)0x1b48 != '\0') {
    func_0x000e0b41();
    func_0x000e0af5(0xe06b,0x51e,0xff7b);
    func_0x000e0af5(0xe06b,0x520,0xff7b);
    func_0x000e0af5(0xe06b,0x523,0xff7b);
    uVar1 = (undefined2)((ulong)*(undefined4 *)0x1dd4 >> 0x10);
    func_0x000e34cb(0xe06b,*(undefined1 *)((int)*(undefined4 *)0x1dd4 + 2));
    func_0x000e0af5(0xe31d,in_BX,uVar1);
    uVar1 = (undefined2)((ulong)*(undefined4 *)0x1dd4 >> 0x10);
    func_0x000e34cb(0xe06b,*(undefined1 *)((int)*(undefined4 *)0x1dd4 + 3));
    func_0x000e0af5(0xe31d,in_BX,uVar1);
    func_0x000e0af5(0xe06b,0x539,0xff7b);
    uVar1 = (undefined2)((ulong)*(undefined4 *)0x1dd4 >> 0x10);
    func_0x000e34cb(0xe06b,*(undefined1 *)((int)*(undefined4 *)0x1dd4 + 4));
    func_0x000e0af5(0xe31d,in_BX,uVar1);
    func_0x000e0af5(0xe06b,0x53b,0xff7b);
  }
  return;
}



/* print_selftest_banner (confidence: Confirmed)
   
   Evidence: Prints the self-test "start"/"complete" banner lines (via
   `init_print_region`/`plot_readout_point`/`draw_readout_char`/`print_banner_line`/`close_print_record`)
   and sets `[0x1B10]=3`. **Contains no test calls at all** - the OR-fold test-dispatch pattern
   previously attributed to this address actually belongs to `0xE4244` (next row). Found by tracing
   what calls `SUB_E094B` and reading this routine end-to-end for the first time. See
   `docs/self-test/dispatcher-and-siblings.md` "self_test_dispatcher was misnamed" */

void __cdecl16far print_selftest_banner(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0x339;
  uVar1 = 0xfa;
  func_0x000e3567();
  func_0x000e3930(0xe352,0,0,0,uVar1,uVar2);
  func_0x000e3854(0xe352,0x20);
  func_0x000e4217(0xe352,0x53e,0xff7b);
  func_0x000e374e(0xe3b1,(int)*(undefined4 *)0x1ddc + 0x20a,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
  func_0x000e3567(0xe352,(int)*(undefined4 *)0x1ddc + 0x213,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x7d,0x307);
  func_0x000e3930(0xe352,0,0,0);
  func_0x000e3854(0xe352,0x20);
  func_0x000e4217(0xe352,0x550,0xff7b);
  func_0x000e374e(0xe3b1,(int)*(undefined4 *)0x1ddc + 0x213,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
  *(undefined2 *)0x1b10 = 3;
  return;
}



/* print_banner_line (confidence: Confirmed)
   
   Evidence: `(far-ptr string)` - calls `print_readout_string` on the given string, then
   `print_string_far` twice more on two *fixed* strings from the `0xFF7B` string table. Used by
   `print_selftest_banner` for both its "before"/"after" lines */

void __stdcall16far print_banner_line(word string_off,word string_seg)

{
  word wVar1;
  word wVar2;
  
  wVar1 = string_off;
  wVar2 = string_seg;
  func_0x000e3821();
  func_0x000e0af5(0xe352,string_off,string_seg,wVar1,wVar2);
  func_0x000e0af5(0xe06b,0x56c,0xff7b);
  return;
}



/* self_test_dispatcher (confidence: Confirmed)
   
   Evidence: The real dispatcher: ~14 calls to per-subsystem test routines in sequence, each folding
   a return code into an accumulator at `[bp-0xA]`, returned in `ax`. One test (`SUB_E252A`) is
   conditionally skipped based on `[0x1B83]==0x1E`. Called from `0xE3DEE`, unconditionally (no
   `[0x1B10]` gate on this one - that gate belongs to `print_selftest_banner`'s caller instead). See
   `docs/self-test/dispatcher-and-siblings.md` "Found: the self-test dispatcher" for the full call
   list */

uint __cdecl16far self_test_dispatcher(void)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  undefined2 unaff_CS;
  undefined2 uVar14;
  undefined2 unaff_DS;
  undefined2 local_c;
  undefined2 local_a;
  
  local_a = 100;
  local_c = 0;
  if (*(char *)0x1b7a != '\x01') {
    *(undefined2 *)0x1b18 = 1;
    local_c = func_0x000e3f2c();
    *(undefined2 *)0x1b18 = 1;
    unaff_CS = 0xe3b1;
    uVar2 = func_0x000e3f99(0xe3b1);
    local_c = local_c | uVar2;
  }
  *(undefined2 *)0x1b18 = 1;
  uVar2 = func_0x000e2fc8(unaff_CS);
  *(undefined2 *)0x1b18 = 1;
  uVar14 = 0xe197;
  uVar3 = func_0x000e1b16(0xe2f4);
  local_c = local_c | uVar2 | uVar3;
  if (*"" == '\x1e') {
    *(undefined2 *)0x1b18 = 1;
    uVar14 = 0xe248;
    uVar2 = func_0x000e252a(0xe197);
    local_c = local_c | uVar2;
  }
  if (*(char *)0x1b7a != '\x01') {
    func_0x000e0add(uVar14);
    func_0x000e0dcc(0xe06b,4,4,0x89,0xe,0);
    do {
      if ((*(uint *)*(undefined4 *)0x322 & 0x8000) == 0) break;
      bVar1 = 0 < local_a;
      local_a = local_a + -1;
    } while (bVar1);
    uVar14 = 0xe06b;
    func_0x000e0e56(0xe06b);
  }
  *(undefined2 *)0x1b18 = 1;
  uVar2 = func_0x000e28fe(uVar14);
  *(undefined2 *)0x1b18 = 1;
  uVar3 = func_0x000e227e(57999);
  *(undefined2 *)0x1b18 = 1;
  uVar4 = func_0x000e26d6(0xe220);
  *(undefined2 *)0x1b18 = 1;
  uVar5 = func_0x000e286c(0xe26d);
  *(undefined2 *)0x1b18 = 1;
  uVar6 = func_0x000e2cec(0xe26d);
  *(undefined2 *)0x1b18 = 1;
  uVar7 = func_0x000e0fd0(0xe2ce);
  func_0x000e44f1(0xe0e8);
  *(undefined2 *)0x1b18 = 1;
  uVar8 = func_0x000e16ea(0xe3b1);
  *(undefined2 *)0x1b18 = 1;
  uVar9 = func_0x000e1e3e(0xe154);
  *(undefined2 *)0x1b18 = 1;
  uVar10 = func_0x000e1d28(0xe1cb);
  *(undefined2 *)0x1b18 = 1;
  uVar11 = func_0x000e1db3(0xe1cb);
  *(undefined2 *)0x1b18 = 1;
  uVar12 = func_0x000e1e90(0xe1cb);
  *(undefined2 *)0x1b18 = 1;
  uVar13 = func_0x000e1f18(0xe1cb);
  return local_c | uVar2 | uVar3 | uVar4 | uVar5 | uVar6 | uVar7 | uVar8 | uVar9 | uVar10 | uVar11 |
         uVar12 | uVar13;
}



/* init_selftest_report_screen (confidence: Confirmed)
   
   Evidence: Saves the readout buffer's base pointer (`[0x1CC4]`) to `[0x45E]`/`[0x460]` (used by
   `init_print_region_home`'s default position), calls `report_screen_mode` with idx 1 then 4
   (reset, then init title/frame), sets a flag bit, and stores the report area's linear address via
   `seg_off_to_linear`. Called from `run_selftest_sequence` */

void __cdecl16far init_selftest_report_screen(void)

{
  byte *pbVar1;
  undefined4 uVar2;
  int iVar3;
  undefined2 unaff_DS;
  undefined2 uVar4;
  
  uVar2 = *(undefined4 *)0x1cc4;
  *(undefined2 *)0x45e = (int)uVar2;
  *(undefined2 *)0x460 = (int)((ulong)uVar2 >> 0x10);
  uVar4 = 1;
  func_0x000e553b();
  func_0x000e553b(0xe45d,4,uVar4);
  pbVar1 = (byte *)((int)*(undefined4 *)0x1ddc + 0x25b);
  *pbVar1 = *pbVar1 | 0x80;
  uVar4 = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
  iVar3 = (int)*(undefined4 *)0x1ddc + 0x20a;
  *(int *)0x1bfe = iVar3;
  *(undefined2 *)0x1c00 = uVar4;
  uVar4 = func_0x000e6d2f(0xe45d,iVar3,uVar4);
  *(undefined2 *)*(undefined4 *)0x1de0 = uVar4;
  return;
}



/* report_and_read_display_chip_int_reset (confidence: Confirmed)
   
   Evidence: Sets `report_screen_mode(2)` then calls `read_display_chip_int_reset`; called when
   `run_selftest_sequence`'s readback loop is aborted */

void __cdecl16far report_and_read_display_chip_int_reset(void)

{
  undefined2 uVar1;
  
  uVar1 = 2;
  func_0x000e553b();
  func_0x000e4429(0xe45d,uVar1);
  return;
}



/* read_display_chip_frame_trigger (confidence: Mechanism and hardware identity now confirmed)
   
   Evidence: Reads one fixed byte from physical `0x42000` - service manual Table 3-1 labels this
   "Display chip next frame" (FRAME). Was `read_display_chip_frame_trigger` - see
   `read_display_chip_int_reset` */

void __cdecl16far read_display_chip_frame_trigger(void)

{
  func_0x000e4429();
  return;
}



/* read_display_chip_int_reset (confidence: Mechanism and hardware identity now confirmed against
   the service manual)
   
   Evidence: Reads one fixed byte from physical `0x41000` - called alternately with
   `read_display_chip_frame_trigger` in a self-test loop that takes repeated readings, spaced by
   `wait_readout_tick` delays. **Was named `read_display_chip_int_reset`; the real service manual
   (provided 2026-09-13) confirms `0x41000` is the readout/CRT "Display chip interrupt reset"
   register (Table 3-1), not a per-channel front-end status port** */

void __cdecl16far read_display_chip_int_reset(void)

{
  return;
}



/* init_selftest_register_group (confidence: Confirmed)
   
   Evidence: `(group)` - initializes far pointers for the self-test register table: group `1` mixes
   the readout second plane (`0x48000`) with the confirmed comm-option probe pair
   (`0x40377E`/`0x4007DE`); else, the readout-window register family
   (`0x406F0`/`0x4067C`/`0x406F8`/`0x406BC`) - directly ties `PRC_READBACK` to real hardware */

void __stdcall16far init_selftest_register_group(word group)

{
  undefined2 unaff_DS;
  
  if (group == 1) {
    *(undefined2 *)0x31e = 0x8000;
    *(undefined2 *)0x320 = 0x4000;
    *(undefined2 *)0x326 = 0x37be;
    *(undefined2 *)0x328 = 0x4000;
    *(undefined2 *)0x336 = 0x7de;
    *(undefined2 *)0x338 = 0x4000;
    *(undefined2 *)0x33a = 0x7ee;
    *(undefined2 *)0x33c = 0x4000;
    *(undefined2 *)0x332 = 0x37de;
    *(undefined2 *)0x334 = 0x4000;
    *(undefined2 *)0x322 = 0x377e;
    *(undefined2 *)0x324 = 0x4000;
    *(undefined2 *)0x32e = 0x37ee;
    *(undefined2 *)0x330 = 0x4000;
    *(undefined2 *)0x32a = 0x37f7;
    *(undefined2 *)0x32c = 0x4000;
  }
  *(undefined2 *)0x1ad0 = 0x6f0;
  *(undefined2 *)0x1ad2 = 0x4000;
  *(undefined2 *)0x1ad8 = 0x6f8;
  *(undefined2 *)0x1ada = 0x4000;
  *(undefined2 *)0x1ad4 = 0x67c;
  *(undefined2 *)0x1ad6 = 0x4000;
  *(undefined2 *)0x1adc = 0x6bc;
  *(undefined2 *)0x1ade = 0x4000;
  return;
}



/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* check_comm_option_installed (confidence: Confirmed)
   
   Evidence: Comm/GPIB option detect - not OR-folded (informational) */

void __cdecl16far check_comm_option_installed(void)

{
  uint uVar1;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x1bf9 = 0xff;
  uVar1 = _DAT_8000_0004;
  *(undefined1 *)0x1bf9 = 0;
  if ((((int)uVar1 >> 8 & 0xffU) + (uVar1 & 0xff) == 0xff) &&
     (*(undefined1 *)0x1bf9 = 1, *"" == '\x1e')) {
    *(byte *)0x1bf9 = *(byte *)0x1bf9 | 2;
  }
  return;
}



/* check_comm_installed_gate (confidence: Confirmed)
   
   Evidence: Checks `[0x1BF9]&1`; gates the 3 comm-board tests above, copying a "not installed"
   message and returning skip if the option isn't present */

bool __cdecl16far check_comm_installed_gate(void)

{
  undefined2 unaff_DS;
  bool bVar1;
  
  bVar1 = (*(byte *)0x1bf9 & 1) == 0;
  if (bVar1) {
    func_0x000e323f();
  }
  return bVar1;
}



/* compute_range_checksum (confidence: Confirmed)
   
   Evidence: `(seed, start_far_ptr, end_far_ptr)` - classic shift-and-add-with-carry running
   checksum over an inclusive byte range */

word __stdcall16far
compute_range_checksum(word seed,word start_off,word start_seg,word end_off,word end_seg)

{
  for (; seed = seed * 2 + (uint)*(byte *)start_off + (uint)((int)seed < 0), start_off != end_off;
      start_off = start_off + 1) {
  }
  return seed;
}



void __stdcall16far FUN_000e_45d1(void)

{
  return;
}



void __stdcall16far FUN_000e_45dd(int param_1)

{
  int iVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  if (param_1 == 1) {
    *(undefined2 *)0x1b0a = 0;
    *(undefined2 *)0x1b08 = 1;
    uVar2 = 2;
    func_0x000e4443();
    *(undefined2 *)0x1ae6 = 0xf;
    *(undefined1 *)0x530 = 1;
    func_0x000e0d06(0xe3b1,1,0,0,0,uVar2);
    unaff_CS = 0xe31d;
    func_0x000e323f(0xe06b,0x476);
  }
  if (param_1 == 2) {
    *(undefined1 *)0x530 = 0;
    func_0x000e0d06(unaff_CS,2,0,0,0);
  }
  else {
    iVar1 = *(int *)0x1b0a;
    *(int *)0x1b0a = *(int *)0x1b0a + 1;
    if (iVar1 == 0) {
      func_0x000e4680(unaff_CS,*(undefined2 *)0x1b08);
      *(undefined2 *)0x1b08 = 3;
    }
    if (*(int *)0x1b0a == *(int *)0x1ae6) {
      *(undefined2 *)0x1b0a = 0;
    }
  }
  return;
}



/* refresh_report_display (confidence: Mechanism confirmed; exact per-cycle purpose not confirmed)
   
   Evidence: `(mode)` - calls 5-6 sub-functions in sequence with the same mode argument; one
   maintains a rotating bit/offset cursor in `[0x1B0C]` and clears bytes at that offset in the
   `[0x31E]`-based scratch buffer. Called from `print_selftest_report_line`'s context */

void __stdcall16far refresh_report_display(word mode)

{
  undefined2 unaff_DS;
  word wVar1;
  
  wVar1 = mode;
  func_0x000e46c3();
  func_0x000e470b(0xe45d,mode,wVar1);
  func_0x000e4762(0xe45d,mode);
  func_0x000e47c5(0xe45d,mode);
  func_0x000e480e(0xe45d,mode);
  if (*(char *)0x1b7a != '\x01') {
    func_0x000e4858(0xe45d,mode);
  }
  return;
}



void __stdcall16far step_progress_pattern_a(int param_1)

{
  undefined2 unaff_DS;
  
  if (param_1 == 2) {
    *(undefined1 *)*(undefined4 *)0x31e = 0;
  }
  else {
    *(int *)0x1b0c = *(int *)0x1b0c << 1;
    if ((param_1 == 1) || ((*(uint *)0x1b0c & 0xfff) == 0)) {
      *(undefined2 *)0x1b0c = 1;
    }
    *(undefined1 *)(*(int *)0x1b0c + (int)*(undefined4 *)0x31e) = 0;
  }
  return;
}



void __stdcall16far step_progress_pattern_b(int param_1)

{
  undefined2 unaff_DS;
  
  if (param_1 == 2) {
    *(undefined1 *)*(undefined4 *)0x326 = 0;
  }
  else {
    *(int *)0x1b0e = *(int *)0x1b0e / 2;
    if (*(int *)0x1b0e == 0) {
      *(undefined2 *)0x1b0e = 0x88;
    }
    if ((param_1 == 1) || ((*(uint *)0x1b0e & 0xf0) == 0)) {
      *(undefined2 *)0x1b0e = 0;
    }
    *(undefined1 *)*(undefined4 *)0x326 = *(undefined1 *)0x1b0e;
  }
  return;
}



void __stdcall16far step_progress_pattern_c(int param_1)

{
  undefined1 uVar1;
  undefined2 unaff_DS;
  
  if (param_1 == 2) {
    *(undefined1 *)0x258 = 0;
    *(undefined1 *)0x257 = 0;
    *(undefined1 *)*(undefined4 *)0x336 = 0;
  }
  else {
    *(int *)0x1b10 = *(int *)0x1b10 << 1;
    if (*(int *)0x1b10 == 0) {
      *(undefined2 *)0x1b10 = 0x11;
    }
    if ((param_1 == 1) || ((*(uint *)0x1b10 & 0xf) == 0)) {
      *(undefined2 *)0x1b10 = 0;
    }
    uVar1 = *(undefined1 *)0x1b10;
    *(undefined1 *)0x258 = uVar1;
    *(undefined1 *)0x257 = uVar1;
    *(undefined1 *)*(undefined4 *)0x336 = uVar1;
  }
  return;
}



void __stdcall16far step_progress_pattern_d(int param_1)

{
  undefined2 unaff_DS;
  
  if (param_1 == 2) {
    *(undefined1 *)*(undefined4 *)0x33a = 0;
  }
  else {
    *(uint *)0x1b12 = *(int *)0x1b12 << 1 | 0x11;
    if ((param_1 == 1) || ((*(uint *)0x1b12 & 0x1f0) == 0x1f0)) {
      *(undefined2 *)0x1b12 = 0;
    }
    *(undefined1 *)*(undefined4 *)0x33a = *(undefined1 *)0x1b12;
  }
  return;
}



void __stdcall16far step_progress_pattern_e(int param_1)

{
  undefined2 unaff_DS;
  
  if (param_1 == 2) {
    *(undefined1 *)*(undefined4 *)0x1d20 = 0;
  }
  else {
    if ((param_1 == 1) || ((*(uint *)0x1b14 & 0xff) == 0)) {
      *(undefined2 *)0x1b14 = 0x1ff;
    }
    *(int *)0x1b14 = *(int *)0x1b14 / 2;
    *(undefined1 *)*(undefined4 *)0x1d20 = *(undefined1 *)0x1b14;
  }
  return;
}



/* step_readout_window_pattern (confidence: Mechanism confirmed)
   
   Evidence: Sibling of `step_progress_pattern_*`, targeting the readout-window register range
   `0x40000+0x6F8..0x6FF` (8 bytes) with a cycling counter `[0x1B16]` */

void __stdcall16far step_readout_window_pattern(int param_1)

{
  undefined2 unaff_DS;
  
  if ((param_1 == 1) || (*(int *)0x1b16 == 0xf)) {
    *(undefined2 *)0x1b16 = 0;
  }
  *(undefined1 *)((*(uint *)0x1b16 & 7) + 0x6f8) = (char)(*(int *)0x1b16 >> 3);
  *(int *)0x1b16 = *(int *)0x1b16 + 1;
  return;
}



void __stdcall16far FUN_000e_48a3(int param_1)

{
  undefined2 unaff_CS;
  
  if (param_1 == 2) {
    unaff_CS = 0xe45d;
    func_0x000e4ea1();
  }
  if (param_1 == 4) {
    func_0x000e48c5(unaff_CS);
  }
  return;
}



void __cdecl16far FUN_000e_48c5(void)

{
  undefined2 unaff_DS;
  undefined1 local_30 [46];
  
  func_0x000e3599();
  func_0x000e323f(0xe352,local_30);
  func_0x000e4ed8(0xe31d,*(undefined1 *)0x4f1,0x1b0c);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4ed8(0xe31d,*(undefined1 *)0x4f5,0x1b0e);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4ed8(0xe31d,*(undefined1 *)0x4e9,0x1b10);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4ed8(0xe31d,*(undefined1 *)0x4ed,0x1b12);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f6d(0xe31d,(int)*(undefined4 *)0x1ddc + 0x213,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x2a3,local_30);
  func_0x000e323f(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)0x4fe,0x1b14);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)0x506,0x1b16);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)0x51e,0x1b18);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f6d(0xe31d,(int)*(undefined4 *)0x1ddc + 0x21c,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x23f,local_30);
  func_0x000e323f(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)0x50e,0x1b1a);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)((int)*(undefined4 *)0x1d1c + 0x20),0x1b1c);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)((int)*(undefined4 *)0x1d1c + 0x26),0x1b1e);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)((int)*(undefined4 *)0x1d1c + 0x3e),0x1b20);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f6d(0xe31d,(int)*(undefined4 *)0x1ddc + 0x225,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x20d,local_30);
  func_0x000e323f(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)0x516,0x1b22);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)((int)*(undefined4 *)0x1d1c + 0x2c),0x1b24);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)((int)*(undefined4 *)0x1d1c + 0x32),0x1b26);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)((int)*(undefined4 *)0x1d1c + 0x38),0x1b28);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f6d(0xe31d,(int)*(undefined4 *)0x1ddc + 0x22e,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x1db,local_30);
  func_0x000e323f(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)((int)*(undefined4 *)0x1d1c + 0x44),0x1b2a);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)((int)*(undefined4 *)0x1d1c + 0x4a),0x1b2c);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f6d(0xe31d,(int)*(undefined4 *)0x1ddc + 0x237,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x1a9,local_30);
  func_0x000e323f(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)((int)*(undefined4 *)0x1d1c + 0x50),0x1b2e);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)((int)*(undefined4 *)0x1d1c + 0x56),0x1b30);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f6d(0xe31d,(int)*(undefined4 *)0x1ddc + 0x240,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x177,local_30);
  func_0x000e323f(0xe45d,local_30);
  func_0x000e4f03(0xe31d,0x10,4,3,*(undefined2 *)((int)*(undefined4 *)0x1d1c + 0x5c),0x1b32);
  func_0x000e31dc(0xe45d,local_30);
  func_0x000e4f6d(0xe31d,(int)*(undefined4 *)0x1ddc + 0x249,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x145,local_30);
  return;
}



void __cdecl16far FUN_000e_4ea1(void)

{
  undefined4 uVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 local_e;
  undefined2 local_c;
  
  uVar1 = *(undefined4 *)0x1ddc;
  local_c = (int)uVar1 + 0x20a;
  for (local_e = 0; local_e < 0x11; local_e = local_e + 1) {
    func_0x000e3599(unaff_CS,local_c,(int)((ulong)uVar1 >> 0x10));
    unaff_CS = 0xe352;
    local_c = local_c + 9;
  }
  return;
}



void __stdcall16far FUN_000e_4ed8(void)

{
  func_0x000e4f03();
  return;
}



void __stdcall16far FUN_000e_4f03(void)

{
  ulong uVar1;
  byte *pbVar2;
  byte *in_BX;
  undefined2 unaff_ES;
  int in_stack_0000000a;
  int *in_stack_0000000c;
  int in_stack_00000010;
  byte *local_10;
  
  func_0x000e327f();
  _local_10 = (byte *)CONCAT22(unaff_ES,in_BX);
  if ((*in_stack_0000000c + in_stack_00000010 < in_stack_0000000a) ||
     (in_stack_0000000a < *in_stack_0000000c - in_stack_00000010)) {
    while (pbVar2 = _local_10, *_local_10 != 0) {
      uVar1 = (ulong)_local_10 >> 0x10;
      _local_10 = (byte *)CONCAT22((int)uVar1,local_10 + 1);
      *pbVar2 = *pbVar2 | 0x80;
    }
    *in_stack_0000000c = in_stack_0000000a;
  }
  return;
}



void __stdcall16far FUN_000e_4f6d(undefined4 param_1,undefined2 param_2,undefined4 param_3)

{
  undefined2 uVar1;
  
  uVar1 = 1;
  func_0x000e3567();
  func_0x000e3930(0xe352,0,0,0,uVar1,param_2);
  func_0x000e3821(0xe352,(int)param_3,(int)((ulong)param_3 >> 0x10));
  func_0x000e374e(0xe352,(int)param_1,(int)((ulong)param_1 >> 0x10));
  return;
}



void __stdcall16far FUN_000e_4fae(undefined4 param_1,undefined2 param_2,undefined4 param_3)

{
  undefined2 uVar1;
  
  uVar1 = 0x262;
  func_0x000e3567();
  func_0x000e3930(0xe352,0,0,0,uVar1,param_2);
  func_0x000e3821(0xe352,(int)param_3,(int)((ulong)param_3 >> 0x10));
  func_0x000e374e(0xe352,(int)param_1,(int)((ulong)param_1 >> 0x10));
  return;
}



void __stdcall16far FUN_000e_4fef(int param_1)

{
  undefined2 unaff_CS;
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  if (param_1 == 1) {
    *(undefined1 *)0x1afd = 0;
    *(undefined1 *)0x530 = 1;
    unaff_CS = 0xe06b;
    func_0x000e0d06();
  }
  uVar1 = unaff_CS;
  if (param_1 == 2) {
    *(undefined1 *)0x530 = 0;
    uVar1 = 0xe06b;
    func_0x000e0d06(unaff_CS,2,0,0,0);
  }
  uVar2 = uVar1;
  if ((param_1 == 4) && (*(char *)0x1afd == '\0')) {
    *(undefined1 *)0x1afd = 1;
    uVar2 = 0xe45d;
    func_0x000e5058(uVar1);
  }
  if (param_1 == 4) {
    func_0x000e5150(uVar2);
  }
  return;
}



void __cdecl16far FUN_000e_5058(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  uVar2 = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
  iVar1 = (int)*(undefined4 *)0x1ddc + 0x20a;
  func_0x000e3599();
  func_0x000e4f6d(0xe352,(int)*(undefined4 *)0x1ddc + 0x20a,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x2d5,0x3f5,0xff7b,iVar1,uVar2);
  func_0x000e4f6d(0xe45d,(int)*(undefined4 *)0x1ddc + 0x213,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x2a3,0x40b,0xff7b);
  func_0x000e4f6d(0xe45d,(int)*(undefined4 *)0x1ddc + 0x21c,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x271,0x420,0xff7b);
  func_0x000e4f6d(0xe45d,(int)*(undefined4 *)0x1ddc + 0x225,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x20d,0x437,0xff7b);
  func_0x000e4f6d(0xe45d,(int)*(undefined4 *)0x1ddc + 0x22e,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x1db,0x449,0xff7b);
  func_0x000e4f6d(0xe45d,(int)*(undefined4 *)0x1ddc + 0x237,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x177,0x45b,0xff7b);
  uVar2 = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
  iVar1 = (int)*(undefined4 *)0x1ddc;
  func_0x000e4f6d(0xe45d,iVar1 + 0x240,uVar2,0x145,0x46c,0xff7b);
  func_0x000e39e2(0xe45d);
  *(int *)0x1ae0 = iVar1;
  *(undefined2 *)0x1ae2 = uVar2;
  return;
}



/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_000e_5150(void)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  
  uVar4 = (undefined2)((ulong)*(undefined4 *)0x1ae0 >> 0x10);
  uVar3 = (undefined2)*(undefined4 *)0x1ae0;
  func_0x000e35c2();
  uVar5 = 0;
  uVar1 = 0x377e;
  func_0x000e4f03(0xe352,0x10,5,4,_DAT_4000_377e,0x1b0c);
  func_0x000e4fae(0xe45d,(int)*(undefined4 *)0x1ddc + 0x249,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x2d5,uVar1,0x4000,uVar5,uVar3,uVar4);
  uVar3 = 0;
  uVar1 = 0x37f7;
  func_0x000e4ed8(0xe45d,DAT_4000_37f7,0x1b0e);
  func_0x000e4fae(0xe45d,(int)*(undefined4 *)0x1ddc + 0x252,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x2a3,uVar1,0x4000,uVar3);
  uVar3 = 0;
  uVar1 = 0x37de;
  func_0x000e4f03(0xe45d,0x10,5,4,DAT_4000_37de,0x1b10);
  func_0x000e4fae(0xe45d,(int)*(undefined4 *)0x1ddc + 0x25b,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x271,uVar1,0x4000,uVar3);
  uVar3 = 0;
  uVar1 = (undefined2)((ulong)*(undefined4 *)0x1d20 >> 0x10);
  iVar2 = (int)*(undefined4 *)0x1d20;
  func_0x000e4f03(0xe45d,2,9,8,*(undefined1 *)(iVar2 + 5),0x1b12);
  func_0x000e4fae(0xe45d,(int)*(undefined4 *)0x1ddc + 0x264,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x20d,iVar2,uVar1,uVar3);
  uVar3 = 0;
  uVar1 = (undefined2)((ulong)*(undefined4 *)0x1d20 >> 0x10);
  iVar2 = (int)*(undefined4 *)0x1d20;
  func_0x000e4f03(0xe45d,2,9,8,*(undefined1 *)(iVar2 + 4),0x1b14);
  func_0x000e4fae(0xe45d,(int)*(undefined4 *)0x1ddc + 0x26d,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x1db,iVar2,uVar1,uVar3);
  uVar3 = 0;
  uVar1 = 9;
  func_0x000e4f03(0xe45d,2,9,8,DAT_4000_067c,0x1b16);
  func_0x000e4fae(0xe45d,(int)*(undefined4 *)0x1ddc + 0x276,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x177,uVar1,0x4000,uVar3);
  uVar3 = 0;
  uVar1 = 0x6bc;
  func_0x000e4f03(0xe45d,2,9,8,DAT_4000_06bc,0x1b18);
  func_0x000e4fae(0xe45d,(int)*(undefined4 *)0x1ddc + 0x27f,
                  (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x145,uVar1,0x4000,uVar3);
  return;
}



void __stdcall16far FUN_000e_534d(int param_1)

{
  undefined2 unaff_DS;
  undefined2 local_12;
  int local_10;
  undefined2 local_e;
  int local_c;
  undefined2 uStack_a;
  
  local_c = 0;
  uStack_a = 0x8000;
  if (param_1 == 1) {
    *(undefined1 *)0x1afd = 0;
  }
  if ((param_1 == 4) && (*(char *)0x1afd == '\0')) {
    *(undefined1 *)0x1afd = 1;
    func_0x000e3599();
    local_12 = 0x2a3;
    local_e = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
    local_10 = (int)*(undefined4 *)0x1ddc + 0x20a;
    func_0x000e540a(0xe352,(int)*(undefined4 *)0x1dd4,(int)((ulong)*(undefined4 *)0x1dd4 >> 0x10),
                    &local_12);
    func_0x000e540a(0xe45d,(int)*(undefined4 *)0x1dd8,(int)((ulong)*(undefined4 *)0x1dd8 >> 0x10),
                    &local_12);
    if ((uint)*(byte *)(local_c + 4) + (uint)*(byte *)(local_c + 5) == 0xff) {
      func_0x000e540a(0xe45d,local_c,uStack_a,&local_12);
    }
  }
  return;
}



void __stdcall16far FUN_000e_540a(undefined4 param_1,int *param_2,int *param_3)

{
  int *piVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined1 local_30 [46];
  
  func_0x000e323f();
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  func_0x000e31dc(0xe31d,local_30);
  func_0x000e4f6d(0xe31d,(int)*(undefined4 *)param_3,(int)((ulong)*(undefined4 *)param_3 >> 0x10),
                  *param_2,local_30);
  *param_3 = *param_3 + 9;
  piVar1 = (int *)param_2;
  *param_2 = *param_2 + -0x32;
  uVar4 = (undefined2)((ulong)*(undefined4 *)param_3 >> 0x10);
  func_0x000e35c2(0xe45d,(int)*(undefined4 *)param_3,uVar4,1,0x10,*param_2,0x10,0,0);
  func_0x000e3930(0xe352,0,0,0);
  func_0x000e3821(0xe352,0x480,0xff7b);
  func_0x000e3372(0xe352,0x160);
  func_0x000e3821(0xe31d,piVar1,uVar4);
  func_0x000e3821(0xe352,0x482,0xff7b);
  func_0x000e34cb(0xe352,*(undefined1 *)(iVar2 + 2));
  func_0x000e3821(0xe31d,piVar1,uVar3);
  func_0x000e34cb(0xe352,*(undefined1 *)(iVar2 + 3));
  func_0x000e3821(0xe31d,piVar1,uVar3);
  func_0x000e3821(0xe352,0x484,0xff7b);
  func_0x000e34cb(0xe352,*(undefined1 *)(iVar2 + 4));
  func_0x000e3821(0xe31d,piVar1,uVar3);
  func_0x000e374e(0xe352,(int)*(undefined4 *)param_3,(int)((ulong)*(undefined4 *)param_3 >> 0x10));
  *param_3 = *param_3 + 9;
  *param_2 = *param_2 + -100;
  return;
}



/* report_screen_mode (confidence: Confirmed mechanism)
   
   Evidence: idx-based dispatcher for the self-test report screen: `idx==1` resets; `idx==2`
   initializes 8 print regions in a loop (the report table's 8 rows); `idx==4` sets up the
   title/frame region */

void __stdcall16far report_screen_mode(int param_1)

{
  undefined2 uVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  int iVar2;
  int local_a;
  
  if (param_1 == 1) {
    *(undefined1 *)0x1afd = 0;
  }
  if (param_1 == 2) {
    for (local_a = 0; local_a < 8; local_a = local_a + 1) {
      func_0x000e3599(unaff_CS,(int)*(undefined4 *)0x1ddc + local_a * 9 + 0x20a,
                      (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
      unaff_CS = 0xe352;
    }
  }
  else if ((param_1 == 4) && (*(char *)0x1afd == '\0')) {
    *(undefined1 *)0x1afd = 1;
    uVar1 = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
    iVar2 = (int)*(undefined4 *)0x1ddc + 0x20a;
    func_0x000e5676();
    *(undefined1 *)((int)*(undefined4 *)0x1ddc + 0x225) = 0;
    func_0x000e3567(0xe45d,(int)*(undefined4 *)0x1ddc + 0x22e,
                    (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x3b6,100,iVar2,uVar1);
    func_0x000e3567(0xe352,(int)*(undefined4 *)0x1ddc + 0x237,
                    (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),10,800);
    func_0x000e3567(0xe352,(int)*(undefined4 *)0x1ddc + 0x240,
                    (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0x3b6,800);
    func_0x000e3567(0xe352,(int)*(undefined4 *)0x1ddc + 0x249,
                    (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),10,100);
    func_0x000e3930(0xe352,0,7,0);
    func_0x000e3854(0xe352,4);
    func_0x000e374e(0xe352,(int)*(undefined4 *)0x1ddc + 0x22e,
                    (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
    func_0x000e374e(0xe352,(int)*(undefined4 *)0x1ddc + 0x237,
                    (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
    func_0x000e374e(0xe352,(int)*(undefined4 *)0x1ddc + 0x240,
                    (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
    func_0x000e374e(0xe352,(int)*(undefined4 *)0x1ddc + 0x249,
                    (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
  }
  return;
}



/* draw_selftest_report_frame (confidence: Confirmed)
   
   Evidence: `(far ptr region)` - draws 2 nested box outlines via `draw_box_outline`
   (`y=0x19,h=0xE1` with two different x/w pairs), bracketed by
   `init_print_region_home`/`close_print_record`/`init_print_region` - called exactly once, latched
   by `[0x1AFD]`, from `report_screen_mode`'s `mode==4` case: the self-test report screen's
   frame/border */

void __stdcall16far draw_selftest_report_frame(word region_off,word region_seg)

{
  undefined2 unaff_DS;
  word wVar1;
  word wVar2;
  undefined2 local_c;
  undefined2 local_a;
  
  wVar1 = region_off;
  wVar2 = region_seg;
  func_0x000e3599();
  func_0x000e5839(0xe352,0,0x19,0xfa,0xe1,2,wVar1,wVar2);
  func_0x000e374e(0xe45d,region_off,region_seg);
  func_0x000e3567(0xe352,region_off + 9,region_seg,0,0);
  func_0x000e5839(0xe352,0x19,0x32,0xe1,200,2);
  func_0x000e374e(0xe45d,region_off + 9,region_seg);
  *(undefined1 *)(region_off + 9) = 0x31;
  func_0x000e3567(0xe352,region_off + 0x12,region_seg,0,0);
  local_c = 0;
  func_0x000e3930(0xe352,0x7d,0,0);
  for (; local_c < 0xfb; local_c = local_c + 6) {
    func_0x000e37b1(0xe352,0x7d - *(int *)0x1af8,local_c - *(int *)0x1afa);
  }
  local_a = 0;
  func_0x000e3930(0xe352,0,0x7d,0);
  for (; local_a < 0xfb; local_a = local_a + 6) {
    func_0x000e37b1(0xe352,local_a - *(int *)0x1af8,0x7d - *(int *)0x1afa);
  }
  func_0x000e374e(0xe352,region_off + 0x12,region_seg);
  func_0x000e3567(0xe352,region_off + 0x1b,region_seg,0,0);
  func_0x000e3930(0xe352,0x32,0x4b,0);
  for (local_a = 0; local_a < 0x19; local_a = local_a + 1) {
    func_0x000e37b1(0xe352,0x32 - *(int *)0x1af8,0xaf - *(int *)0x1afa);
    func_0x000e37b1(0xe352,200 - *(int *)0x1af8,0xaf - *(int *)0x1afa);
    func_0x000e37b1(0xe352,200 - *(int *)0x1af8,0x4b - *(int *)0x1afa);
    func_0x000e37b1(0xe352,0x32 - *(int *)0x1af8,0x4b - *(int *)0x1afa);
  }
  func_0x000e374e(0xe352,region_off + 0x1b,region_seg);
  return;
}



/* draw_box_outline (confidence: Confirmed)
   
   Evidence: `(x1, y1, x2, y2, step)` - draws a rectangle's 4 edges via 4 calls to
   `draw_readout_line`, resolved by tracing each call's exact push order against
   `draw_readout_line`'s own confirmed `(x1, y1, x_max, y_max, dx, dy)` signature */

void __stdcall16far draw_box_outline(word x1,word y1,word x2,word y2,word step)

{
  word wVar1;
  
  wVar1 = step;
  func_0x000e58ad();
  func_0x000e58ad(0xe45d,x1,y2,x2,y2,step,0,wVar1);
  func_0x000e58ad(0xe45d,x2,y1,x2,y2,0,step);
  func_0x000e58ad(0xe45d,x1,y1,x2,y1,step,0);
  return;
}



/* draw_readout_line (confidence: Confirmed)
   
   Evidence: `(x1, y1, x_max, y_max, dx, dy)` - steps from `(x1,y1)` toward `(x_max,y_max)` by
   `(dx,dy)`, calling `plot_readout_point_scaled` per step - a line-drawing primitive for the
   readout vector display */

void __stdcall16far draw_readout_line(word x1,word y1,word x_max,word y_max,word dx,word dy)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  undefined2 local_c;
  undefined2 local_a;
  
  uVar1 = 0;
  local_c = y1;
  local_a = x1;
  func_0x000e3930();
  for (; ((int)local_a <= (int)x_max && ((int)local_c <= (int)y_max)); local_c = local_c + dy) {
    func_0x000e37b1(0xe352,local_a - *(int *)0x1af8,local_c - *(int *)0x1afa,uVar1);
    local_a = local_a + dx;
  }
  return;
}



void __stdcall16far FUN_000e_5909(void)

{
  func_0x000e5b34();
  return;
}



/* print_report_frame_mode (confidence: Confirmed mechanism)
   
   Evidence: Same idx-dispatcher shape as `selftest_display_result_mode`: `idx==1` resets; `idx==4`
   draws a bordered frame (two print regions + a 150-point line via `plot_readout_point_scaled`) */

undefined2 __stdcall16far print_report_frame_mode(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined1 *puVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined4 local_12;
  int local_e;
  
  if (*(char *)0x1b7a == '\x01') {
    uVar6 = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
    puVar5 = (undefined1 *)((int)*(undefined4 *)0x1ddc + 0x291);
  }
  else {
    uVar6 = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
    puVar5 = (undefined1 *)((int)*(undefined4 *)0x1ddc + 0x21c);
  }
  local_12 = (undefined1 *)CONCAT22(uVar6,puVar5);
  if (param_1 == 1) {
    *(undefined1 *)0x1afd = 0;
  }
  else {
    if ((param_1 == 4) && (*(char *)0x1afd == '\0')) {
      *(undefined1 *)0x1afd = 1;
      uVar7 = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
      iVar3 = (int)*(undefined4 *)0x1ddc + 0x20a;
      func_0x000e3599();
      func_0x000e3567(0xe352,(int)*(undefined4 *)0x1ddc + 0x213,
                      (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0,0,iVar3,uVar7);
      func_0x000e3567(0xe352,puVar5,uVar6,0,0);
      *local_12 = 0x93;
      local_e = 100;
      func_0x000e3930(0xe352,100,0x80,0);
      for (; local_e < 0x96; local_e = local_e + 1) {
        func_0x000e37b1(0xe352,local_e - *(int *)0x1af8,0x80 - *(int *)0x1afa);
      }
      func_0x000e374e(0xe352,(int)*(undefined4 *)0x1ddc + 0x20a,
                      (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
      unaff_CS = 0xe352;
      func_0x000e374e(0xe352,(int)*(undefined4 *)0x1ddc + 0x213,
                      (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
      *(undefined2 *)0x1b20 = 0x200;
      *(undefined2 *)0x1b22 = 0x200;
    }
    if (param_1 == 4) {
      if (((*(byte *)0x758 & 0x80) == 0) || ((*"" == '\x14' && ((*(byte *)0x758 & 0x40) != 0)))) {
        if (*(char *)0x1b7a == '\x01') {
          uVar6 = *(undefined2 *)0x50e;
        }
        else {
          uVar6 = func_0x000e2ab0(unaff_CS,0x82);
          unaff_CS = 0xe296;
        }
        *(undefined2 *)0x1b20 = uVar6;
        if (*(char *)0x1b7a == '\x01') {
          uVar6 = *(undefined2 *)0x516;
        }
        else {
          uVar6 = func_0x000e2ab0(unaff_CS,0xa2);
          unaff_CS = 0xe296;
        }
        *(undefined2 *)0x1b22 = uVar6;
      }
      if (*(char *)0x1b7a == '\x01') {
        iVar3 = *(int *)0x50e;
      }
      else {
        iVar3 = func_0x000e2ab0(unaff_CS,0x82);
        unaff_CS = 0xe296;
      }
      iVar1 = *(int *)0x1b20;
      if (*(char *)0x1b7a == '\x01') {
        iVar4 = *(int *)0x516;
      }
      else {
        iVar4 = func_0x000e2ab0(unaff_CS,0xa2);
        unaff_CS = 0xe296;
      }
      iVar2 = *(int *)0x1b22;
      func_0x000e36a5(unaff_CS,(int)*(undefined4 *)0x1ddc + 0x20a,
                      (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0,(iVar1 - iVar3) * 4);
      func_0x000e36a5(0xe352,(int)*(undefined4 *)0x1ddc + 0x213,
                      (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),0,(iVar2 - iVar4) * 4);
    }
    else if (param_1 == 2) {
      func_0x000e3599(unaff_CS,(int)*(undefined4 *)0x1ddc + 0x20a,
                      (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
      func_0x000e3599(0xe352,(int)*(undefined4 *)0x1ddc + 0x213,
                      (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
      func_0x000e3599(0xe352,puVar5,uVar6,0,0);
    }
  }
  return 0;
}



/* selftest_display_result_mode (confidence: Mechanism confirmed; relationship to
   `selftest_measure_mode`'s flag not fully resolved)
   
   Evidence: `idx==1`/`2` toggle the *same* `[0x1B5E]` flag `selftest_measure_mode` uses (a shared
   "measurement active" flag?); `idx==3`/`4` fall through to positioning + printing a result via
   `set_position_record`/`SUB_E2DC9` */

void __stdcall16far selftest_display_result_mode(int param_1)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (param_1 == 1) {
    *(undefined1 *)0x1afd = 0;
    *(undefined1 *)0x1b5e = 1;
  }
  else if (param_1 == 2) {
    *(undefined1 *)0x1b5e = 0;
  }
  else {
    if (*(char *)0x1afd == '\0') {
      *(undefined1 *)0x1afd = 1;
      uVar3 = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
      iVar1 = (int)*(undefined4 *)0x1ddc + 0x20a;
      func_0x000e5676();
      *(undefined1 *)((int)*(undefined4 *)0x1ddc + 0x213) = 0;
      *(undefined1 *)((int)*(undefined4 *)0x1ddc + 0x225) = 0;
      func_0x000e3662(0xe45d,(int)*(undefined4 *)0x1ddc + 0x20a,
                      (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),2,8,iVar1,uVar3);
      unaff_CS = 0xe352;
      func_0x000e36a5(0xe352,(int)*(undefined4 *)0x1ddc + 0x20a,
                      (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),400,0x177);
    }
    iVar1 = func_0x000e2dc9(unaff_CS,0x8f,0x9a);
    iVar2 = func_0x000e2dc9(0xe2ce,0x8e,0x8a);
    iVar1 = iVar1 - iVar2;
    func_0x000e36a5(0xe2ce,(int)*(undefined4 *)0x1ddc + 0x21c,
                    (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),((iVar2 + -100) * 200) / 0x1e,
                    ((iVar1 + -0xcd) * 200) / 10);
    if ((((iVar1 < 0xc9) || (0xd1 < iVar1)) || (iVar2 < 0x56)) || (0x72 < iVar2)) {
      *(undefined1 *)((int)*(undefined4 *)0x1ddc + 0x225) = 0;
    }
    else {
      *(undefined1 *)((int)*(undefined4 *)0x1ddc + 0x225) = 0x11;
    }
  }
  return;
}



/* select_next_ready_task (confidence: Confirmed)
   
   Evidence: The task scheduler's task-selection logic: walks the 12-entry per-task ready-state
   table at `[task_idx+0x1A91]` (confirms it's a real ready/priority table, not plot-specific)
   looking for a ready task, sets `[0x1ACD]` (current task index) and `[0x1A8F]` */

void __cdecl16far select_next_ready_task(void)

{
  uint uVar1;
  uint uVar2;
  byte *pbVar3;
  undefined2 unaff_DS;
  
  if (*(char *)0x1b76 == '\0') {
    if (*(char *)0x760 == '\0') {
      *(undefined2 *)0x79a = 1;
    }
    else {
      *(undefined2 *)0x79a = 0;
    }
  }
  else {
    *(undefined2 *)0x79a = 3;
  }
  pbVar3 = (byte *)(*(int *)0x79a * 0xc + 0x17);
  *(undefined2 *)0x798 = 0xb;
  while( true ) {
    if (*(int *)0x798 < 0) {
      *(undefined2 *)0x1acd = 0;
      return;
    }
    uVar2 = (uint)*pbVar3;
    uVar1 = CONCAT11(((undefined *)&DAT_000e_19b1)[uVar2],((undefined *)&DAT_000e_19b1)[uVar2]) &
            0xb0ff;
    if ((char)(uVar1 >> 8) != '\0') {
      ((undefined *)&DAT_000e_19b1)[uVar2] = ((undefined *)&DAT_000e_19b1)[uVar2] & 0x4f;
      uVar1 = (uint)(byte)((undefined *)&DAT_000e_19b1)[uVar2];
    }
    if ((uVar1 & 0xf) != 0) break;
    *(int *)0x798 = *(int *)0x798 + -1;
    pbVar3 = pbVar3 + -1;
  }
  *(uint *)0x1acd = uVar2;
  if (uVar2 == 8) {
    *(undefined2 *)0x1a8f = 0;
    return;
  }
  *(undefined2 *)0x1a8f = 0x200;
  return;
}



/* sync_and_enable_interrupts (confidence: Confirmed)
   
   Evidence: Calls `sync_status_byte_to_hw`, then falls through into `enable_interrupts` - a "leave
   critical section" combo */

void sync_and_enable_interrupts(void)

{
  func_0x000fdc73();
  return;
}



/* enable_interrupts (confidence: Confirmed)
   
   Evidence: `sti; retf` - one instruction */

void __cdecl16far enable_interrupts(void)

{
  return;
}



/* disable_interrupts (confidence: Confirmed)
   
   Evidence: `cli; retf` - one instruction */

void __cdecl16far disable_interrupts(void)

{
  return;
}



/* delay_read_128w (confidence: Mechanism confirmed; "why" is an inference)
   
   Evidence: `rep lodsw` x128 words from `0000:0000`, result unused - called right before
   `install_late_interrupt_vectors`; likely a deliberate timing delay (hardware settling) rather
   than a memory test, given sibling routines use different word counts */

void __cdecl16far delay_read_128w(void)

{
  int iVar1;
  
  for (iVar1 = 0x80; iVar1 != 0; iVar1 = iVar1 + -1) {
  }
  return;
}



/* scan_low_ram_chunk0 (confidence: Confirmed)
   
   Evidence: `rep lodsw` x48 words from `0000:0x00` (bytes `0x00-0x5F`) - part of a 3-way rotation
   covering `0000:0x00-0xFF` (a quarter of the IVT) across 3 scheduler ticks - a background
   low-memory watchdog scan */

void __cdecl16far scan_low_ram_chunk0(void)

{
  int iVar1;
  
  for (iVar1 = 0x30; iVar1 != 0; iVar1 = iVar1 + -1) {
  }
  return;
}



/* scan_low_ram_chunk1 (confidence: Confirmed)
   
   Evidence: Same shape, bytes `0x60-0xBF` - see `scan_low_ram_chunk0` */

void __cdecl16far scan_low_ram_chunk1(void)

{
  int iVar1;
  
  for (iVar1 = 0x30; iVar1 != 0; iVar1 = iVar1 + -1) {
  }
  return;
}



/* scan_low_ram_chunk2 (confidence: Confirmed)
   
   Evidence: Same shape, bytes `0xC0-0xFF` (32 words) - see `scan_low_ram_chunk0` */

void __cdecl16far scan_low_ram_chunk2(void)

{
  int iVar1;
  
  for (iVar1 = 0x20; iVar1 != 0; iVar1 = iVar1 + -1) {
  }
  return;
}



/* install_late_interrupt_vectors (confidence: Confirmed)
   
   Evidence: Installs `INT255_HANDLER_LATE` (`0x3FC`) and `INT2_HANDLER_LATE` (`0x008`), both into
   segment `0xE60B` - matches the "late" IVT writes documented in
   `docs/interrupts/ivt-and-int255.md` "Interrupt vector table entries" */

void __cdecl16far install_late_interrupt_vectors(void)

{
  func_0x000e5d31();
  uRam000003fc = 5;
  uRam000003fe = 0xe60b;
  uRam00000008 = 0x3a;
  uRam0000000a = 0xe60b;
  return;
}



/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* init_far_pointer_table_sysrom (confidence: Mechanism confirmed)
   
   Evidence: Main-ROM counterpart of the comm ROM's confirmed `init_far_pointer_table`: copies a
   `(dest_offset, value_dword)` table (terminated by `dest==-1`) compiled in right after the code at
   segment `0xE5D1` (the same segment `INT2_HANDLER_EARLY` lives in) to `es:[dest]` - bulk
   RAM-resident-variable init */

void __cdecl16far init_far_pointer_table_sysrom(void)

{
  undefined2 *puVar1;
  undefined2 uVar2;
  undefined2 *puVar3;
  
  uVar2 = _DAT_e000_5ecd;
  for (puVar3 = (undefined2 *)0x1bf; puVar1 = (undefined2 *)*puVar3, puVar1 != (undefined2 *)0xffff;
      puVar3 = puVar3 + 3) {
    *puVar1 = puVar3[1];
    puVar1[1] = puVar3[2];
  }
  return;
}



/* WARNING: Unable to track spacebase fully for stack */
/* switch_to_next_task (confidence: Confirmed mechanism; how many tasks exist and what each does not
   confirmed)
   
   Evidence: Loads `SP`/`SS` from a per-task context table at `[0x1A9D + idx*4]` (`idx = [0x1ACD]`)
   and resumes it via the standard register-pop + `iret` epilogue - the "switch in" half of a small
   preemptive task-switcher driven by `INT2_HANDLER_LATE`. See `docs/interrupts/task-scheduler.md`
   "A small task scheduler" */

undefined4 switch_to_next_task(void)

{
  int iVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 uVar4;
  code *pcVar5;
  undefined1 *puVar6;
  undefined2 *puVar7;
  undefined2 unaff_DS;
  undefined4 uVar8;
  
  func_0x000e5c65();
  if ((((undefined *)&DAT_000e_19b1)[*(int *)0x1acd] & 0x40) != 0) {
    iVar1 = *(int *)((undefined *)&DAT_000e_19bd + *(int *)0x1acd * 4);
    uVar2 = *(undefined2 *)((undefined1 *)&LAB_000e_19bf + *(int *)0x1acd * 4);
    *(uint *)(iVar1 + 0x16) = *(uint *)(iVar1 + 0x16) | *(uint *)0x1a8f;
    *(undefined2 *)(iVar1 + -2) = 0xe5c6;
    puVar6 = (undefined1 *)(iVar1 + -4);
    *(undefined2 *)(iVar1 + -4) = 0x627d;
    func_0x000fdc73();
    return CONCAT22(*(undefined2 *)(puVar6 + 10),*(undefined2 *)(puVar6 + 0x10));
  }
  uVar2 = *(undefined2 *)(*(int *)0x1acd * 4 + 0x40);
  iVar3 = *(int *)(*(int *)0x1acd * 4 + 0x3e);
  iVar1 = *(int *)0x1acd;
  *(undefined2 *)(iVar3 + -2) = 0xe628;
  *(int *)(iVar3 + -4) = iVar1 * 4 + 0xe;
  *(undefined2 *)((undefined *)&DAT_000e_ff1a + iVar3) = 0xe5c6;
  puVar7 = (undefined2 *)(iVar3 + -8);
  *(undefined2 *)(iVar3 + -8) = 0x62b9;
  func_0x000fdc73();
  uVar4 = ((undefined2 *)*puVar7)[1];
  pcVar5 = (code *)*(undefined2 *)*puVar7;
  puVar7[1] = 0xfdb3;
  *puVar7 = 0x62bf;
  (*pcVar5)();
  uVar8 = FUN_000e_6274();
  return uVar8;
}



/* WARNING: Control flow encountered bad instruction data */
/* create_task_b (confidence: Mechanism confirmed; why it's duplicated rather than shared not
   confirmed)
   
   Evidence: Near-identical duplicate of `create_task`'s body (same
   save-context/register-table/ready-flag pattern) - called specifically from `mark_task_ready`.
   Exact reason for the duplication rather than sharing `create_task` not confirmed */

void create_task_b(void)

{
  int iVar1;
  undefined2 unaff_SS;
  
  LOCK();
  UNLOCK();
  LOCK();
  UNLOCK();
  iVar1 = iRam00001edd * 4;
  *(undefined1 **)((undefined *)&DAT_000e_19bd + iVar1) = &stack0xffec;
  *(undefined2 *)((undefined1 *)&LAB_000e_19bf + iVar1) = unaff_SS;
  ((undefined *)&DAT_000e_19b1)[iRam00001edd] = ((undefined *)&DAT_000e_19b1)[iRam00001edd] | 0x40;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



/* WARNING: Control flow encountered bad instruction data */
/* create_task (confidence: Fork-style mechanism and the "operates on the current task's own slot,
   never allocates a new one" correction both confirmed by direct trace; genuinely new task
   identities (if any exist beyond a fixed roster) must be established some other way, not yet found
   - the real task count is bounded by the already-documented 12-entry ready-state table, not by the
   36 call sites (`docs/interrupts/task-scheduler.md` has the full call-site list and worked
   examples))
   
   Evidence: Saves the current context's `SP`/`SS` into the per-task table `[0x1A9D+idx*4]` and sets
   a ready flag, then jumps into `switch_to_next_task`. **Critical correction**: `idx` comes from
   `[0x1ACD]` - the index of the task that is *currently running* - read once and never modified
   anywhere in this function. There is no allocation of a new/free task slot anywhere in its body.
   So this is really a **self-yield primitive** ("save a new resume point for the currently-running
   task, then let something else run"), not a spawn primitive, despite the inherited name. Takes
   **no explicit caller-pushed arguments**: it's a real `fork()`-style trampoline - rotates its own
   3 saved words at `[bp+0x12]`/`[bp+0x14]`/`[bp+0x16]` (originally `FLAGS`/return-`IP`/return-`CS`,
   pushed automatically by the ordinary far `call` that invoked it) and saves *those* as the resume
   point, meaning **the code immediately following each call site becomes that task's own
   continuation**, run later when the scheduler resumes this same slot - not executed by the calling
   function itself. Verified on 3 independent call sites, including one (`comm_call_main_rom`) whose
   post-call code loops back to the top of the same state machine, making the "yield and resume the
   same code" shape unambiguous. Called from all three ROMs (main + comm) */

void create_task(undefined2 param_1)

{
  byte *pbVar1;
  int iVar2;
  byte *pbVar3;
  undefined2 unaff_SS;
  undefined2 in_stack_00000000;
  
  LOCK();
  UNLOCK();
  LOCK();
  UNLOCK();
  iVar2 = iRam00001edd * 4;
  *(undefined1 **)((undefined *)&DAT_000e_19bd + iVar2) = &stack0xffec;
  *(undefined2 *)((undefined1 *)&LAB_000e_19bf + iVar2) = unaff_SS;
  uRam00000bd0 = param_1;
  pbVar3 = (undefined *)&DAT_000e_19b1 + iRam00001edd;
  pbVar1 = pbVar3;
  uRam00000bd2 = in_stack_00000000;
  *pbVar1 = *pbVar1 | 0x40;
  pbVar1 = pbVar3;
  *pbVar1 = *pbVar1 - 1;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



/* WARNING: Control flow encountered bad instruction data */

void FUN_000e_6274(void)

{
  byte *pbVar1;
  byte *pbVar2;
  
  pbVar2 = (undefined *)&DAT_000e_19b1 + iRam00001edd;
  pbVar1 = pbVar2;
  *pbVar1 = *pbVar1 - 1;
  pbVar1 = pbVar2;
  *pbVar1 = *pbVar1 & 0xf;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



/* run_continuous_selftest_tick (confidence: Mechanism confirmed; **the `[0x740]` hook now traced**
   - see `poll_comm_status_tick` below)
   
   Evidence: Called every tick from `INT255_HANDLER_LATE`: dispatches to
   `verify_prc_readback_pattern` (if `[0x1B5E]`) or `poll_comm_probe_until_match` otherwise into
   `[0x795]`; tracks 3 independent sticky/debounce failure counters (`[0x1A9A]`, `[0x1A99]`, and
   `[0x1A96]` gated by `[0x1BFA]`); on a clean result calls a hook function pointer at `[0x740]` -
   the background continuous self-test/watchdog engine, distinct from the foreground
   `run_selftest_sequence`. **`INT255` confirmed 2026-09-13 to be the real hardware Maskable
   Interrupt (`INTR`) vector**, not software-only (service manual: *"the Maskable Interrupt (INTR)
   is vectored to 03FC"*) - so this really is an interrupt-driven per-tick handler, not just a
   polling-loop callee */

void __cdecl16far run_continuous_selftest_tick(void)

{
  byte bVar1;
  undefined1 uVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  
  if (*(char *)0x1b5e == '\0') {
    uVar4 = 0xfdb3;
    uVar2 = func_0x000fdb8f();
    *(undefined1 *)0x795 = uVar2;
  }
  else {
    uVar4 = 0xe060;
    uVar2 = func_0x000e060a();
    *(undefined1 *)0x795 = uVar2;
  }
  if (((*(byte *)0x795 & 4) != 0) &&
     (*(char *)0x1a9a = *(char *)0x1a9a + '\x01', (*(byte *)0x1a9a & 0xb0) != 0)) {
    *(char *)0x1a9a = (*(byte *)0x1a9a & 0x40) + 1;
  }
  if (((*(byte *)0x795 & 3) != 0) &&
     (*(char *)0x1a99 = *(char *)0x1a99 + '\x01', (*(byte *)0x1a99 & 0xb0) != 0)) {
    *(char *)0x1a99 = (*(byte *)0x1a99 & 0x40) + 1;
  }
  if (*(char *)0x795 == '\0') {
    iVar3 = (*(code *)*(undefined2 *)0x740)(uVar4);
    if (iVar3 == 0) {
      if (*(char *)0x75b == '\0') {
        uVar5 = 0xf158;
        iVar3 = func_0x000f1581(uVar4);
        uVar4 = uVar5;
        if (iVar3 != 0) {
          *(byte *)0x795 = *(byte *)0x795 | 8;
          *(char *)0x1a94 = *(char *)0x1a94 + '\x01';
          if ((*(byte *)0x1a94 & 0xb0) != 0) {
            *(char *)0x1a94 = (*(byte *)0x1a94 & 0x40) + 1;
          }
        }
      }
    }
    else {
      *(byte *)0x795 = *(byte *)0x795 | 0x10;
      if (*(char *)0x1bfa == '\0') {
        bVar1 = *(byte *)0x1a96;
        func_0x0009470e(uVar4,0x8f80);
        if (*(char *)0x5a3 == '\0') {
          if ((bVar1 & 0xf) == 0) {
            uVar4 = 38000;
            func_0x0009470e(38000,0x41);
            (*(code *)*(undefined2 *)0x73c)(38000);
          }
          else {
            uVar4 = 38000;
            func_0x0009470e(38000,0x41);
            *(char *)0x1a96 = *(char *)0x1a96 + '\x01';
            if ((*(byte *)0x1a96 & 0xb0) != 0) {
              *(char *)0x1a96 = (*(byte *)0x1a96 & 0x40) + 1;
            }
          }
        }
        else {
          uVar4 = 38000;
          func_0x0009470e(38000,0x41);
          *(char *)0x1a95 = *(char *)0x1a95 + '\x01';
          if ((*(byte *)0x1a95 & 0xb0) != 0) {
            *(char *)0x1a95 = (*(byte *)0x1a95 & 0x40) + 1;
          }
        }
      }
      else {
        *(char *)0x1a96 = *(char *)0x1a96 + '\x01';
        if ((*(byte *)0x1a96 & 0xb0) != 0) {
          *(char *)0x1a96 = (*(byte *)0x1a96 & 0x40) + 1;
        }
      }
      if ((*(char *)0x7b8 == '\0') &&
         (*(char *)0x1a95 = *(char *)0x1a95 + '\x01', (*(byte *)0x1a95 & 0xb0) != 0)) {
        *(char *)0x1a95 = (*(byte *)0x1a95 & 0x40) + 1;
      }
    }
    if (*(char *)0x795 == '\0') {
      func_0x000fa00c(uVar4);
      *(undefined1 *)0x763 = 0;
      if (((*(char *)0x1bf7 == '\0') && (*(char *)0x1b78 == '\0')) && (*(char *)0x1b7a == '\0')) {
        *(undefined1 *)0x762 = 1;
      }
      else {
        *(char *)0x1a9b = *(char *)0x1a9b + '\x01';
        if ((*(byte *)0x1a9b & 0xb0) != 0) {
          *(char *)0x1a9b = (*(byte *)0x1a9b & 0x40) + 1;
        }
        *(undefined2 *)0x750 = 0;
      }
    }
  }
  return;
}



void __cdecl16far FUN_000e_64c0(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  undefined2 local_c;
  int local_a;
  
  *(undefined1 *)0x766 = 0;
  uVar1 = 0x41;
  func_0x000f0078();
  func_0x000e5e53(0xf007,uVar1);
  func_0x000e5d2f(0xe5d1);
  func_0x000fdb3e(0xe5d1);
  func_0x000f156e(0xfdb3);
  for (local_a = 0; local_a < 0xc; local_a = local_a + 1) {
    ((undefined *)&DAT_000e_19b1)[local_a] = 0;
  }
  func_0x000fbcf3(0xf156);
  func_0x000e9255(0xfbcf);
  func_0x000e5e53(0xe925);
  local_c = 0xffff;
  (*(code *)0x0)(0xe5d1);
  return;
}



/* scheduler_tick_service (confidence: Confirmed)
   
   Evidence: Called unconditionally from both paths inside `INT2_HANDLER_LATE` (every timer tick).
   Reads 2 hardware status bytes from fixed physical `0x403FFA`/`0x403FFB` into `[0x758]`/`[0x759]`,
   XORs `[0x758]` against a previous snapshot for edge detection, and increments `[0x752]` - the
   exact counter `wait_readout_tick` polls. THE per-tick hardware-polling heartbeat behind the task
   scheduler */

void __cdecl16far scheduler_tick_service(void)

{
  char *pcVar1;
  byte bVar2;
  undefined4 uVar3;
  int iVar4;
  undefined2 unaff_CS;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 unaff_DS;
  
  if ((*(char *)0x1b76 == '\0') && (0x12 < *(byte *)0x1b8c)) {
    *(int *)0x7b2 = *(int *)0x7b2 + 1;
    if (2 < *(int *)0x7b2) {
      *(undefined2 *)0x7b2 = 0;
      *(char *)0x1a9c = *(char *)0x1a9c + '\x01';
      if (2 < *(byte *)0x1a9c) {
        *(undefined1 *)0x1a9c = 0;
        unaff_CS = 0xe5d1;
        func_0x000e5d31();
      }
    }
  }
  else {
    iVar4 = *(int *)0x7b2;
    *(int *)0x7b2 = *(int *)0x7b2 + 1;
    if (iVar4 == 0) {
      unaff_CS = 0xe5d1;
      func_0x000e5d3d();
    }
    else if (iVar4 == 1) {
      unaff_CS = 0xe5d1;
      func_0x000e5d49();
    }
    else {
      unaff_CS = 0xe5d1;
      func_0x000e5d58();
      *(undefined2 *)0x7b2 = 0;
    }
  }
  *(undefined1 *)0x758 = DAT_4000_3ffa;
  *(undefined1 *)0x759 = DAT_4000_3ffb;
  *(byte *)0x758 = *(byte *)0x758 ^ *(byte *)0x7b4;
  uVar5 = unaff_CS;
  if (*(char *)0x75b != '\0') {
    uVar5 = 0xf158;
    iVar4 = func_0x000f1581(unaff_CS);
    if (((iVar4 != 0) && ((*(byte *)0x1a94 & 0xf) == 0)) &&
       (*(char *)0x1a94 = *(char *)0x1a94 + '\x01', (*(byte *)0x1a94 & 0xb0) != 0)) {
      *(char *)0x1a94 = (*(byte *)0x1a94 & 0x40) + 1;
    }
  }
  *(int *)0x752 = *(int *)0x752 + 1;
  *(undefined1 *)0x75a = *(undefined1 *)0x1acd;
  *(int *)0x756 = *(int *)0x756 + 1;
  if ((*(int *)0x764 <= *(int *)0x756) && ((*(byte *)0x1a95 & 0xf) == 0)) {
    *(char *)0x1a94 = *(char *)0x1a94 + '\x01';
    if ((*(byte *)0x1a94 & 0xb0) != 0) {
      *(char *)0x1a94 = (*(byte *)0x1a94 & 0x40) + 1;
    }
    *(undefined2 *)0x756 = 0;
  }
  if (*(char *)0x766 != '\0') {
    if (*"" == '\0') {
      if ((*(char *)0x1bf7 != '\0') &&
         (*(char *)0x1a9b = *(char *)0x1a9b + '\x01', (*(byte *)0x1a9b & 0xb0) != 0)) {
        *(char *)0x1a9b = (*(byte *)0x1a9b & 0x40) + 1;
      }
      *(char *)0x1bfc = *(char *)0x1bfc + -1;
      if ((((*(char *)0x1bfc == '\0') && (*(undefined1 *)0x1bfc = 0x80, *(char *)0x7a4 == '\0')) &&
          ((*(byte *)0x1b76 & 0x80) == 0)) && ((*(byte *)0x1a98 & 0xf) == 0)) {
        *(char *)0x1a98 = *(char *)0x1a98 + '\x01';
      }
      if ((*(char *)0x1b7a == '\0') && (*(char *)0x1b78 == '\0')) {
        *(char *)0x7b6 = *(char *)0x7b6 + '\x01';
        if (0xb < *(byte *)0x7b6) {
          *(undefined1 *)0x7b6 = 0;
        }
        if ((*(char *)(*(byte *)0x7b6 + 0x744) != '\0') &&
           (bVar2 = *(byte *)0x7b6, pcVar1 = (char *)(bVar2 + 0x744), *pcVar1 = *pcVar1 + -1,
           *(char *)(bVar2 + 0x744) == '\0')) {
          ((undefined *)&DAT_000e_19b1)[*(byte *)0x7b6] =
               ((undefined *)&DAT_000e_19b1)[*(byte *)0x7b6] + '\x01';
        }
        *(int *)0x754 = *(int *)0x754 + 1;
        uVar6 = uVar5;
        if (*(int *)0x790 < *(int *)0x754) {
          *(char *)0x1a9b = *(char *)0x1a9b + '\x01';
          if (*"" == '\0') {
            *(undefined2 *)0x754 = 0;
          }
          else if (*"" != '\0') {
            func_0x000fa00c(uVar5);
            func_0x000f9fee(0xf9fe);
            *(undefined1 *)0x763 = 1;
            *(undefined1 *)0x762 = 0;
            *(undefined2 *)0x754 = 0;
            uVar6 = 0xf9fe;
          }
        }
        uVar5 = uVar6;
        if (*"" == '\0') {
          uVar5 = 0xed9b;
          iVar4 = func_0x000ed9bc(uVar6);
          if (iVar4 != 0) {
            uVar5 = 0xf9fe;
            func_0x000f9fee(0xed9b);
            *(undefined1 *)0x763 = 1;
            *(undefined1 *)0x762 = 0;
          }
        }
      }
      else {
        *(int *)0x754 = *(int *)0x754 + 1;
        if ((*(int *)0x790 < *(int *)0x754) && (*"" != '\0')) {
          func_0x000fa00c(uVar5);
          uVar5 = 0xf9fe;
          func_0x000f9fee(0xf9fe);
          *(undefined1 *)0x763 = 1;
          *(undefined2 *)0x754 = 0;
          *(undefined1 *)0x762 = 0;
        }
      }
      if (*(int *)0x790 * 2 < *(int *)0x754) {
        func_0x000e6844(uVar5);
      }
    }
    else {
      if ((*"\x1b" != '\0') &&
         (*(char *)0x1a9b = *(char *)0x1a9b + '\x01', (*(byte *)0x1a9b & 0xb0) != 0)) {
        *(char *)0x1a9b = (*(byte *)0x1a9b & 0x40) + 1;
      }
      iVar4 = func_0x000e7f03(uVar5);
      if (iVar4 == 0) {
        *(int *)0x754 = *(int *)0x754 + 1;
        if (*(int *)0x792 < *(int *)0x754) {
          func_0x000e71c9(0xe7f0);
          *(undefined2 *)0x792 = 100;
          *(undefined2 *)0x754 = 0;
          uVar3 = *(undefined4 *)0x1ddc;
          *(int *)0x1bfe = (int)uVar3 + 0x2be;
          *(undefined2 *)0x1c00 = (int)((ulong)uVar3 >> 0x10);
          uVar5 = func_0x000e6d2f(0xe709,(int)*(undefined4 *)0x1bfe,
                                  (int)((ulong)*(undefined4 *)0x1bfe >> 0x10));
          *(undefined2 *)*(undefined4 *)0x1de0 = uVar5;
          func_0x000fa00c(0xe6a8);
          func_0x000f9fee(0xf9fe);
          *(undefined1 *)0x763 = 1;
          *(char *)0x1a97 = *(char *)0x1a97 + '\x01';
          if ((*(byte *)0x1a97 & 0xb0) != 0) {
            *(char *)0x1a97 = (*(byte *)0x1a97 & 0x40) + 1;
          }
        }
      }
      else {
        func_0x000e71c9(0xe7f0);
      }
    }
  }
  return;
}



/* escalate_acq_timeout_reset (confidence: Mechanism confirmed)
   
   Evidence: The severe half of the acquisition-stall watchdog: called only once `[0x754]` exceeds
   2x the timeout `[0x790]`; tracks escalation frequency via `[0x7B0]`, latching `[0x1A9B]=1`
   permanently after >10 escalations, then force-clears both channels via
   `clear_display_chip_int_reset`/`clear_display_chip_frame_trigger` and resets `[0x754]` */

void __cdecl16far escalate_acq_timeout_reset(void)

{
  byte bVar1;
  undefined2 unaff_DS;
  
  bVar1 = *(byte *)0x7b0;
  *(char *)0x7b0 = *(char *)0x7b0 + '\x01';
  if ((10 < bVar1) && (*(char *)0x1a9b == '\0')) {
    *(char *)0x1a9b = *(char *)0x1a9b + '\x01';
    *(undefined1 *)0x7b0 = 0;
  }
  func_0x000fa00c();
  func_0x000f9fee(0xf9fe);
  *(undefined1 *)0x763 = 1;
  *(undefined2 *)0x754 = 0;
  *(undefined1 *)0x762 = 0;
  return;
}



/* compute_acq_channel_scan_counts (confidence: Mechanism confirmed)
   
   Evidence: Derives 2 base scan-count values from the acquisition mode `[0x1B8C]` (`(mode-8)/3` and
   `(mode-8)/4`, or a fixed 4/3 pair outside `[8,0x13]`), scales each per-channel by x1/x2/x3 based
   on `[0x1B6E]` (ch2) and `[0x1B6A]` (ch1) mode selectors, storing results into `[0x7AC]`-`[0x7AF]`
    */

void __cdecl16far compute_acq_channel_scan_counts(void)

{
  undefined2 unaff_DS;
  char local_a;
  char local_9;
  
  if ((*(byte *)0x1b8c < 0x14) && (7 < *(byte *)0x1b8c)) {
    local_a = (char)((int)(*(byte *)0x1b8c - 8) / 3) + '\x01';
    local_9 = (char)((int)(*(byte *)0x1b8c - 8) / 4) + '\x01';
  }
  else if (*(byte *)0x1b8c < 0x14) {
    local_a = '\x04';
    local_9 = '\x03';
  }
  else {
    local_a = '\x04';
    local_9 = '\x03';
  }
  *(char *)0x7ac = local_9;
  *(undefined1 *)0x7ad = *(undefined1 *)0x7ac;
  if (*(char *)0x1b6e == '\x02') {
    *(char *)0x7ad = *(char *)0x7ad << 1;
  }
  if (*(char *)0x1b6e == '\x01') {
    *(char *)0x7ad = *(char *)0x7ad * '\x03';
  }
  if (*(char *)0x1b6a == '\x02') {
    *(char *)0x7ac = *(char *)0x7ac << 1;
  }
  if (*(char *)0x1b6a == '\x01') {
    *(char *)0x7ac = *(char *)0x7ac * '\x03';
  }
  *(char *)0x7af = local_a;
  *(char *)0x7ae = local_a;
  return;
}



/* restart_current_task (confidence: Mechanism confirmed)
   
   Evidence: If `[0x1ACD]` (current task index) is nonzero, sets that task's `[+0x744]` flag and
   calls `create_task` again - a task-restart/respawn trigger */

void __cdecl16far restart_current_task(void)

{
  undefined2 unaff_DS;
  
  if (*(int *)0x1acd != 0) {
    *(undefined1 *)(*(int *)0x1acd + 0x744) = 1;
    func_0x000e6224();
  }
  return;
}



/* mark_task_ready (confidence: Confirmed)
   
   Evidence: `(task_idx)` - clears that task's `[+0x744]` flag, increments its ready-flags byte
   (`[idx+0x1A91]`), then calls `create_task_b` */

void __stdcall16far mark_task_ready(word task_idx)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)(task_idx + 0x744) = 0;
  ((undefined *)&DAT_000e_19b1)[task_idx] = ((undefined *)&DAT_000e_19b1)[task_idx] + '\x01';
  func_0x000e61e3();
  return;
}



void __cdecl16far FUN_000e_6aab(void)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  int local_a;
  
  uVar5 = (undefined2)((ulong)*(undefined4 *)0x1c80 >> 0x10);
  uVar3 = (undefined2)*(undefined4 *)0x1c80;
  uVar1 = func_0x000e6d2f();
  iVar4 = (int)*(undefined4 *)0x1c84;
  iVar2 = func_0x000e6d2f(0xe6a8,iVar4,(int)((ulong)*(undefined4 *)0x1c84 >> 0x10),uVar1,uVar3,uVar5
                         );
  *(int *)0x1c02 = iVar2 - iVar4;
  func_0x000e3b12(0xe6a8);
  *(undefined1 *)0x766 = 0;
  for (local_a = 0; local_a < 0xc; local_a = local_a + 1) {
    *(undefined2 *)(local_a * 2 + 0x768) = 0;
  }
  func_0x000fdb3e(0xe3b1);
  func_0x000fa00c(0xfdb3);
  func_0x000e6d4d(0xf9fe);
  func_0x000f156e(0xe6a8);
  for (local_a = 0; local_a < 0xc; local_a = local_a + 1) {
    ((undefined *)&DAT_000e_19b1)[local_a] = 0;
  }
  if (*(char *)0x780 == '\0') {
    *(undefined1 *)0x780 = 1;
    *(undefined2 *)0x790 = 7;
    *(undefined2 *)0x792 = 100;
    *(undefined1 *)0x794 = 0;
    *(undefined1 *)0x78d = 1;
    *(undefined1 *)0x79c = 1;
    *(undefined2 *)0x764 = 0x18;
    *(code *)update_menu_position = (code)0x3;
    *(undefined1 *)0x760 = 0;
    for (local_a = 0; local_a < 0xc; local_a = local_a + 1) {
      *(undefined1 *)(local_a + 0x781) = 1;
    }
  }
  func_0x000e75c0(0xf156);
  func_0x000fbcf3(0xe75c);
  func_0x000e9255(0xfbcf);
  func_0x000e7238(0xe925);
  *(undefined1 *)0x1b8d = 1;
  *(char *)&DAT_000e_19b1 = *(char *)&DAT_000e_19b1 + '\x01';
  if ((*(byte *)&DAT_000e_19b1 & 0xb0) != 0) {
    *(char *)&DAT_000e_19b1 = (*(byte *)&DAT_000e_19b1 & 0x40) + 1;
  }
  func_0x000e5e53(0xe723);
  func_0x000e6166(0xe5d1);
  return;
}



void FUN_000e_6bb3(void)

{
  undefined2 unaff_CS;
  
  do {
    func_0x000f8624(unaff_CS);
    unaff_CS = 0xe60b;
    func_0x000e6224(0xf862);
  } while( true );
}



void __cdecl16far FUN_000e_6bca(void)

{
  func_0x000ead08();
  return;
}



void __cdecl16far FUN_000e_6bd9(void)

{
  undefined2 unaff_DS;
  
  func_0x000e92bf();
  *(undefined1 *)0x78d = 1;
  return;
}



void __cdecl16far FUN_000e_6bed(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  while( true ) {
    func_0x000f67bf(unaff_CS);
    if (*"\x04" != '\0') break;
    if (*(byte *)0x1b8c < 0x13) {
      func_0x000e6a8e(0xf670,8);
      func_0x000e6224(0xe6a8);
    }
    else {
      func_0x000e6224(0xf670);
    }
    unaff_CS = 0xe60b;
  }
  *(undefined1 *)0x761 = 0;
  *(byte *)0x1a99 = *(byte *)0x1a99 & 0xf;
  return;
}



void FUN_000e_6c30(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  do {
    (*(code *)*(undefined2 *)0x73c)(unaff_CS);
    func_0x000e6224(unaff_CS);
    unaff_CS = 0xe60b;
  } while( true );
}



void __cdecl16far FUN_000e_6c46(void)

{
  undefined2 unaff_DS;
  
  (*(code *)*(undefined2 *)0x738)();
  return;
}



void __cdecl16far FUN_000e_6c54(void)

{
  func_0x000f0ebc();
  return;
}



void __cdecl16far FUN_000e_6c63(void)

{
  func_0x000ff704();
  return;
}



void __cdecl16far FUN_000e_6c72(void)

{
  undefined2 unaff_DS;
  
  *(int *)0x77a = *(int *)0x77a + 1;
  func_0x000fdc27();
  return;
}



/* reset_display_and_notify_comm (confidence: Confirmed)
   
   Evidence: Guarded by `[0x766]`: notifies the comm ROM, clears/sets several
   `[0x1B76]`/`[0x1B8D]`/`[0x760]` flags around a disable/enable-interrupts block, clears
   measurement mode, refreshes the display position cache, and (if `[0x1BF7]` set) calls
   `reinit_system_state` */

void __cdecl16far reset_display_and_notify_comm(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x766 != '\0') {
    func_0x000e6ee0();
    *(byte *)0x1b76 = *(byte *)0x1b76 & 0xbf;
    *(undefined1 *)0x760 = 0;
    func_0x000e5d2f(0xe6a8);
    *(byte *)0x1b8d = *(byte *)0x1b8d | 1;
    func_0x000e6d20(0xe5d1);
    func_0x000e6884(0xe6a8);
    func_0x000e7416(0xe688);
    *(undefined1 *)0x1b8d = 1;
    *(byte *)0x1b76 = *(byte *)0x1b76 | 0x80;
    func_0x000e6ce8(0xe741);
    func_0x000e7322(0xe6a8);
    func_0x000e5d28(0xe723);
    func_0x000e6efd(0xe5d1);
    if (*(char *)0x1bf7 != '\0') {
      func_0x000e6dc4(0xe6a8);
    }
  }
  return;
}



/* finish_acq_reset_and_clear_mode (confidence: Confirmed)
   
   Evidence: Sets `[0x761]=1`, clears measurement mode via `clear_measurement_mode_relay`, then
   calls `SUB_FB8E8(0)` - tail step shared by `reset_display_and_notify_comm` and
   `handle_acq_mode_change` */

void __cdecl16far finish_acq_reset_and_clear_mode(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x761 = 1;
  func_0x000e6d20();
  func_0x000fb8e8(0xe6a8,0);
  return;
}



/* reset_display_if_idle (confidence: Confirmed)
   
   Evidence: Calls `reset_display_and_notify_comm` only when `[0x1B76]` masked with `0xDF` (all bits
   but `0x20`) is already zero */

void __cdecl16far reset_display_if_idle(void)

{
  undefined2 unaff_DS;
  
  if ((*(byte *)0x1b76 & 0xdf) == 0) {
    func_0x000e6c85();
  }
  return;
}



/* clear_measurement_mode_relay (confidence: Confirmed)
   
   Evidence: Thin far-call relay to `clear_measurement_mode_bit`, no logic of its own */

void __cdecl16far clear_measurement_mode_relay(void)

{
  func_0x000fdb3e();
  return;
}



/* seg_off_to_linear (confidence: Confirmed)
   
   Evidence: `(offset, segment) -> offset + segment*16` - a compiled-in runtime helper for exactly
   the seg:off-to-flat-address math this project's own disassembly tooling uses, called 16x */

int __stdcall16far seg_off_to_linear(word offset,word segment)

{
  return offset + segment * 0x10;
}



/* init_comm_dispatch_table (confidence: Confirmed)
   
   Evidence: Called once from the boot sequence right after `run_selftest_sequence`: if the comm
   option is installed (`[0x1BF9]`), calls `comm_rom_boot_init` then points the far-pointer dispatch
   slots `[0x738]`/`[0x73A]`, `[0x73C]`/`[0x73E]`, and `[0x740]`/`[0x742]` (the hook called by
   `run_continuous_selftest_tick`) into the comm ROM (segment `0x96F5`); if not installed, points
   all 3 at main-ROM stub targets in segment `0xE6A8` instead - the comm-option-present-vs-absent
   dispatch switch */

void __cdecl16far init_comm_dispatch_table(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  if (*(char *)0x1bf9 == '\0') {
    *(undefined2 *)0x738 = 2;
    *(undefined2 *)0x73a = 0xe6a8;
    *(undefined2 *)0x73c = 2;
    *(undefined2 *)0x73e = 0xe6a8;
    *(undefined2 *)0x740 = 2;
    *(undefined2 *)0x742 = 0xe6a8;
  }
  else {
    func_0x0009628c();
    *(undefined2 *)0x740 = 4;
    *(undefined2 *)0x742 = 0x96f5;
    *(undefined2 *)0x738 = 5;
    *(undefined2 *)0x73a = 0x839f;
    if (*(char *)0x1bfa == '\0') {
      uVar1 = 0x1db;
    }
    else {
      uVar1 = 0x65c;
    }
    *(undefined2 *)0x73c = uVar1;
    *(undefined2 *)0x73e = 0x96f5;
  }
  return;
}



/* reinit_system_state (confidence: Mechanism confirmed; exact trigger (boot? mode change?) not
   confirmed)
   
   Evidence: Disables interrupts, resets a large group of global flags/state, conditionally notifies
   the comm ROM if the option is installed, calls `divide_scale_default` for both the comm-ROM and
   main-ROM segments, then re-enables interrupts */

void __cdecl16far reinit_system_state(void)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  func_0x000e6ee0();
  func_0x000e5d2f(0xe6a8);
  *(undefined1 *)0x1bf7 = 0;
  *(undefined1 *)0x78e = 0;
  if ((*"" != '\x1e') && (*(char *)0x608 != '\0')) {
    *(undefined1 *)0x3fd = 0;
    *(undefined1 *)0x608 = 0;
  }
  *(undefined2 *)0x548 = 0xffff;
  *(byte *)0x1b76 = *(byte *)0x1b76 & 0xef;
  *(undefined1 *)0x762 = 1;
  *(undefined1 *)0x79c = 1;
  uVar1 = 0xe741;
  func_0x000e7416(0xe5d1);
  if (*(char *)0x1bf9 != '\0') {
    uVar2 = 0xf007;
    func_0x000f0078(0xe741,0x8f80);
    if (*(char *)0x63c != '\0') {
      uVar2 = 0xe004;
      func_0x000e004f(0xf007,1);
    }
    *(undefined1 *)0x63c = 0;
    uVar1 = 0xf007;
    func_0x000f0078(uVar2,0x41);
  }
  func_0x000e6d04(uVar1);
  *(undefined1 *)0x78d = 1;
  func_0x000f635e(0xe6a8);
  *(undefined1 *)0x1a97 = 0;
  func_0x000e5d28(0xf5f5);
  func_0x000e6efd(0xe5d1);
  return;
}



void __cdecl16far FUN_000e_6e60(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  func_0x000e6ee0();
  uVar1 = 0xe5d1;
  func_0x000e5d2f(0xe6a8);
  if ((*(uint *)0x222 & 1) != 0) {
    if ((*(byte *)0x1b76 & 0x40) == 0) {
      *(undefined2 *)0x222 = 0;
      *(byte *)0x1b76 = *(byte *)0x1b76 & 0xbf;
      *(char *)0x1a99 = *(char *)0x1a99 + '\x01';
      if ((*(byte *)0x1a99 & 0xb0) != 0) {
        *(char *)0x1a99 = (*(byte *)0x1a99 & 0x40) + 1;
      }
    }
    else {
      uVar1 = 0xe6a8;
      func_0x000e6c85(0xe5d1);
    }
  }
  func_0x000e5d28(uVar1);
  func_0x000e6efd(0xe5d1);
  return;
}



void __cdecl16far FUN_000e_6ec0(void)

{
  undefined2 unaff_DS;
  
  func_0x000f5e4c();
  *(undefined2 *)0x1bf0 = 0;
  func_0x000ea344(0xf5e4);
  *(undefined2 *)0x7b4 = 0;
  return;
}



/* notify_comm_rom_a (confidence: Mechanism confirmed)
   
   Evidence: If comm option installed and `[0x1BFA]` clear, calls a comm-ROM handler (`SUB_96F0D`) -
   a conditional cross-ROM notification, twin to `notify_comm_rom_b` */

void __cdecl16far notify_comm_rom_a(void)

{
  undefined2 unaff_DS;
  
  if ((*(char *)0x1bf9 != '\0') && (*(char *)0x1bfa == '\0')) {
    func_0x00096f0d();
  }
  return;
}



/* notify_comm_rom_b (confidence: Mechanism confirmed)
   
   Evidence: Same gating as `notify_comm_rom_a` plus an additional check (`[0x1ACD]==0xA`, task
   index 10), calls a different comm-ROM handler (`SUB_96EC6`) */

void __cdecl16far notify_comm_rom_b(void)

{
  undefined2 unaff_DS;
  
  if ((*(char *)0x1bf9 != '\0') && (*(char *)0x1bfa == '\0')) {
    func_0x00096ec6();
    if (*(int *)0x1acd != 10) {
      func_0x000e693c(0x9687);
      func_0x000e693c(0xe693);
    }
  }
  return;
}



void __cdecl16far FUN_000e_6f9e(void)

{
  byte *pbVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 local_a;
  
  *(undefined1 *)0x1a92 = 0;
  *(undefined2 *)0x548 = 0xffff;
  func_0x000ead08();
  *(byte *)0x1b76 = *(byte *)0x1b76 | 0x10;
  uVar2 = 0xe6a8;
  func_0x000e6ce8(0xead0);
  if ((*"" != '\x1e') && (*"" != '\x01')) {
    pbVar1 = (byte *)((int)*(undefined4 *)0x1ddc + 0x13b);
    *pbVar1 = *pbVar1 | 0x80;
  }
  pbVar1 = (byte *)((int)*(undefined4 *)0x1ddc + 0x1e6);
  *pbVar1 = *pbVar1 & 0x7f;
  pbVar1 = (byte *)((int)*(undefined4 *)0x1ddc + 0x1ef);
  *pbVar1 = *pbVar1 | 0x80;
  if ((*"" != '\x14') || (*"" == '\x01')) {
    pbVar1 = (byte *)((int)*(undefined4 *)0x1ddc + 0x1ef);
    *pbVar1 = *pbVar1 | 1;
  }
  *(undefined1 *)0x78d = 0;
  *(undefined1 *)0x78e = 1;
  *(char *)0x1a97 = *(char *)0x1a97 + '\x01';
  if ((*(byte *)0x1a97 & 0xb0) != 0) {
    *(char *)0x1a97 = (*(byte *)0x1a97 & 0x40) + 1;
  }
  *(undefined2 *)0x764 = 0x40;
  while (*"\x1b" == '\0') {
    func_0x000e6224(uVar2);
    uVar2 = 0xe709;
    func_0x000e71c9(0xe60b);
  }
  for (local_a = 0; local_a < 0x38; local_a = local_a + 1) {
    pbVar1 = (byte *)(local_a * 9 + (int)*(undefined4 *)0x1ddc);
    *pbVar1 = *pbVar1 & 0xbf;
  }
  if (*"" != '\x1e') {
    pbVar1 = (byte *)((int)*(undefined4 *)0x1ddc + 0x13b);
    *pbVar1 = *pbVar1 & 0x7f;
  }
  pbVar1 = (byte *)((int)*(undefined4 *)0x1ddc + 0x1e6);
  *pbVar1 = *pbVar1 | 0x80;
  pbVar1 = (byte *)((int)*(undefined4 *)0x1ddc + 0x1ef);
  *pbVar1 = *pbVar1 & 0x7e;
  func_0x000e6dc4(uVar2);
  *(undefined2 *)0x548 = 0xffff;
  return;
}



void __cdecl16far FUN_000e_7098(void)

{
  byte bVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x762 = 0;
  *(undefined1 *)0x7b0 = 0;
  uVar2 = 0xfa60;
  func_0x000fa6c8();
  if ((*(char *)0x1bf7 != '\0') &&
     (((*(char *)0x1bf8 == '\0' && (*(char *)0x400 == '\0')) || ((*(uint *)0x22 & 0x400) != 0)))) {
    uVar2 = 0xe6f9;
    func_0x000e6f9e(0xfa60);
  }
  if ((*(byte *)0x1b76 & 0x80) != 0) {
    uVar3 = 0xe5d1;
    func_0x000e5d2f(uVar2);
    if ((*(byte *)0x1a99 & 0xf0) == 0) {
      if ((*(byte *)0x1a93 & 0xf0) == 0) {
        if (((*(char *)0x1b7a == '\0') && (*(char *)0x1b78 != '\0')) &&
           (((*(byte *)0x1b76 & 0x1f) == 0 &&
            ((*(char *)0x1a99 == '\0' && (*(char *)0x1a93 == '\0')))))) {
          *(byte *)0x1b76 = *(byte *)0x1b76 & 0x7f;
          *(byte *)0x1b8d = *(byte *)0x1b8d | 1;
          *(char *)0x1a99 = *(char *)0x1a99 + '\x01';
          if ((*(byte *)0x1a99 & 0xb0) != 0) {
            *(char *)0x1a99 = (*(byte *)0x1a99 & 0x40) + 1;
          }
          *(undefined1 *)0x761 = 0;
        }
      }
      else {
        *(undefined1 *)0x1a93 = 0x41;
        bVar1 = *(byte *)0x736;
        *(char *)0x736 = *(char *)0x736 + '\x01';
        if (10 < bVar1) {
          uVar3 = 0xe709;
          func_0x000e721a(0xe5d1);
        }
      }
    }
    else {
      *(undefined1 *)0x1a99 = 0x41;
      bVar1 = *(byte *)0x736;
      *(char *)0x736 = *(char *)0x736 + '\x01';
      if (10 < bVar1) {
        uVar3 = 0xe709;
        func_0x000e721a(0xe5d1);
      }
    }
    uVar2 = 0xe5d1;
    func_0x000e5d28(uVar3);
  }
  if (((*(int *)0x55a != 0 ||
        (*(int *)0x55c != 0 || (*(int *)0x55e != 0 || (*(int *)0x560 != 0 || *(int *)0x562 != 0))))
      && (*(char *)0x7a4 == '\0')) && ((*(byte *)0x1a98 & 0xf) == 0)) {
    *(char *)0x1a98 = *(char *)0x1a98 + '\x01';
  }
  *(undefined1 *)0x762 = 1;
  func_0x000e71c9(uVar2);
  func_0x000e695c(0xe709);
  return;
}



/* update_plot_retry_counters (confidence: Mechanism confirmed (matches the identical inline
   single-item version at `[0x1A9B]` = item #10 of the same array); exact meaning of the 12 items
   not confirmed)
   
   Evidence: Iterates a 12-entry per-item table: when a per-item countdown at `[item+0x744]` reaches
   0, increments a per-item attempt counter at `[item+0x1A91]` with bit-masked gating (`0xB0` then
   `0x40`); called from the plot-output task's main loop */

void __cdecl16far update_plot_retry_counters(void)

{
  undefined2 unaff_DS;
  int local_a;
  
  for (local_a = 0; local_a < 0xc; local_a = local_a + 1) {
    if (((*(char *)(local_a + 0x744) != '\0') &&
        (*(char *)(local_a + 0x744) = *(char *)(local_a + 0x744) + -1,
        *(char *)(local_a + 0x744) == '\0')) &&
       (((undefined *)&DAT_000e_19b1)[local_a] = ((undefined *)&DAT_000e_19b1)[local_a] + '\x01',
       (((undefined *)&DAT_000e_19b1)[local_a] & 0xb0) != 0)) {
      ((undefined *)&DAT_000e_19b1)[local_a] = (((undefined *)&DAT_000e_19b1)[local_a] & 0x40) + 1;
    }
  }
  return;
}



void __cdecl16far FUN_000e_721a(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x1a93 = 0;
  *(undefined1 *)0x1a99 = 0;
  *(undefined1 *)0x736 = 0;
  func_0x000fdb3e();
  return;
}



/* init_acq_channel_error_table (confidence: Confirmed)
   
   Evidence: Clears a 12-entry byte array at `[di+0x79D]` plus `[0x732]`/`[0x733]`, and sets the
   "initialized" latch `[0x734]=1` - one-time setup for the acq-channel-error tracking used by
   `ack_acq_channel_error_flag` */

void __cdecl16far init_acq_channel_error_table(void)

{
  undefined2 unaff_DS;
  undefined2 local_a;
  
  for (local_a = 0; local_a < 0xc; local_a = local_a + 1) {
    *(undefined1 *)(local_a + 0x79d) = 0;
  }
  *(undefined1 *)0x734 = 1;
  *(undefined1 *)0x732 = 0;
  *(undefined1 *)0x733 = 0;
  return;
}



void __cdecl16far FUN_000e_726a(void)

{
  undefined2 unaff_DS;
  int local_a;
  
  for (local_a = 0; local_a < 0xc; local_a = local_a + 1) {
    if (*(char *)(local_a + 0x79d) == '\x02') {
      *(undefined1 *)(local_a + 0x79d) = 0;
      ((undefined *)&DAT_000e_19b1)[local_a] = ((undefined *)&DAT_000e_19b1)[local_a] + '\x01';
    }
  }
  return;
}



/* WARNING: Removing unreachable block (ram,0x00007396) */
/* WARNING: Removing unreachable block (ram,0x000073ab) */
/* WARNING: Removing unreachable block (ram,0x000073a0) */
/* WARNING: Removing unreachable block (ram,0x000073b3) */

void __stdcall16far FUN_000e_7298(int param_1,int param_2)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  uVar1 = 0xe5d1;
  func_0x000e5d2f();
  *(undefined1 *)0x733 = (undefined1)param_1;
  *(char *)0x734 = *(char *)0x734 + -1;
  if (param_1 != 0) {
    *(undefined1 *)(param_2 + 0x79d) = 2;
    func_0x000e5d28(0xe5d1);
    uVar1 = 0xe60b;
    func_0x000e6224(0xe5d1);
  }
  func_0x000e5d28(uVar1);
  return;
}



void __stdcall16far FUN_000e_7309(undefined2 param_1,int param_2)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)(param_2 + 0x79d) = 0;
  *(undefined1 *)0x732 = 1;
  return;
}



/* ack_acq_channel_error_flag (confidence: Confirmed)
   
   Evidence: Critical-section check-and-clear of error flag `[0x7A5]` bit 0: lazily sets the same
   "initialized" latch `[0x734]` used by `init_acq_channel_error_table`, and on a set flag
   increments the shared debounced failure counter `[0x1A99]` (the same counter
   `run_continuous_selftest_tick` feeds) before clearing `[0x7A5]` */

void __cdecl16far ack_acq_channel_error_flag(void)

{
  undefined2 unaff_DS;
  
  func_0x000e5d2f();
  if (*(char *)0x734 == '\0') {
    *(char *)0x734 = *(char *)0x734 + '\x01';
  }
  if (((*(byte *)0x7a5 & 1) != 0) && (*(char *)0x7a5 != '\0')) {
    *(char *)0x1a99 = *(char *)0x1a99 + '\x01';
    if ((*(byte *)0x1a99 & 0xb0) != 0) {
      *(char *)0x1a99 = (*(byte *)0x1a99 & 0x40) + 1;
    }
    *(undefined1 *)0x7a5 = 0;
  }
  func_0x000e5d28(0xe5d1);
  return;
}



void __cdecl16far FUN_000e_7371(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x733 = 0;
  *(char *)0x734 = *(char *)0x734 + '\x01';
  if ((*(byte *)0x7a4 & 1) != 0) {
    func_0x000e5d2f();
    if ((*(byte *)0x7a4 & 2) == 0) {
      *(char *)0x1a98 = *(char *)0x1a98 + '\x01';
      if ((*(byte *)0x1a98 & 0xb0) != 0) {
        *(char *)0x1a98 = (*(byte *)0x1a98 & 0x40) + 1;
      }
      *(undefined1 *)0x7a4 = 0;
    }
    else {
      *(undefined1 *)0x733 = 1;
      *(undefined1 *)0x7a4 = 2;
    }
    unaff_CS = 0xe5d1;
    func_0x000e5d28(0xe5d1);
  }
  if ((*(byte *)0x7a5 & 1) != 0) {
    func_0x000e5d2f(unaff_CS);
    if ((*(byte *)0x7a5 & 2) == 0) {
      *(char *)0x1a99 = *(char *)0x1a99 + '\x01';
      if ((*(byte *)0x1a99 & 0xb0) != 0) {
        *(char *)0x1a99 = (*(byte *)0x1a99 & 0x40) + 1;
      }
      *(undefined1 *)0x7a5 = 0;
    }
    else {
      *(undefined1 *)0x733 = 1;
      *(undefined1 *)0x7a5 = 2;
    }
    func_0x000e5d28(0xe5d1);
  }
  return;
}



/* refresh_display_position_cache (confidence: Mechanism confirmed)
   
   Evidence: Skips recomputation if 3 cached config values still match the current ones and a flag
   is clear; otherwise updates the cache and recomputes a display record's linear address via
   `seg_off_to_linear` - a change-detection guard avoiding redundant recomputation */

void __cdecl16far refresh_display_position_cache(void)

{
  byte *pbVar1;
  undefined4 uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  int local_a;
  
  if ((((*(char *)0x1bf7 != '\0') || (*(uint *)0x72c != (uint)*(byte *)0x1b7a)) ||
      (*(uint *)0x730 != (uint)*(byte *)0x1b78)) ||
     ((*(char *)0x79c != '\0' || (*(uint *)0x72e != (uint)*(byte *)0x686)))) {
    *(undefined1 *)0x79c = 0;
    *(uint *)0x72c = (uint)*(byte *)0x1b7a;
    *(uint *)0x730 = (uint)*(byte *)0x1b78;
    *(uint *)0x72e = (uint)*(byte *)0x686;
    if (*(char *)0x1b7a == '\0') {
      if (*"" == '\x11') {
        uVar2 = *(undefined4 *)0x1ddc;
        *(int *)0x1bfe = (int)uVar2 + 0xab;
        *(undefined2 *)0x1c00 = (int)((ulong)uVar2 >> 0x10);
        uVar5 = (undefined2)((ulong)*(undefined4 *)0x1bfe >> 0x10);
        uVar4 = (undefined2)*(undefined4 *)0x1bfe;
        uVar3 = func_0x000e6d2f();
        *(undefined2 *)*(undefined4 *)0x1de0 = uVar3;
      }
      else if ((*(char *)0x1b78 == '\0') && (*"" != '\0')) {
        uVar2 = *(undefined4 *)0x1ddc;
        *(int *)0x1bfe = (int)uVar2 + 0xab;
        *(undefined2 *)0x1c00 = (int)((ulong)uVar2 >> 0x10);
        uVar5 = (undefined2)((ulong)*(undefined4 *)0x1bfe >> 0x10);
        uVar4 = (undefined2)*(undefined4 *)0x1bfe;
        uVar3 = func_0x000e6d2f();
        *(undefined2 *)*(undefined4 *)0x1de0 = uVar3;
      }
      else if (*(char *)0x1b78 == '\0') {
        func_0x000fdb65();
        for (local_a = 0x24; local_a < 0x37; local_a = local_a + 1) {
          pbVar1 = (byte *)(local_a * 9 + (int)*(undefined4 *)0x1ddc);
          *pbVar1 = *pbVar1 & 0xfe;
        }
        pbVar1 = (byte *)((int)*(undefined4 *)0x1ddc + 0x1f8);
        *pbVar1 = *pbVar1 | 1;
        *(undefined2 *)0x548 = 0xffff;
        uVar2 = *(undefined4 *)0x1ddc;
        *(int *)0x1bfe = (int)uVar2 + 0x1f8;
        *(undefined2 *)0x1c00 = (int)((ulong)uVar2 >> 0x10);
        uVar5 = (undefined2)((ulong)*(undefined4 *)0x1bfe >> 0x10);
        uVar4 = (undefined2)*(undefined4 *)0x1bfe;
        uVar3 = func_0x000e6d2f(0xfdb3,uVar4,uVar5);
        *(undefined2 *)*(undefined4 *)0x1de0 = uVar3;
      }
      else {
        for (local_a = 0x24; local_a < 0x38; local_a = local_a + 1) {
          pbVar1 = (byte *)(local_a * 9 + (int)*(undefined4 *)0x1ddc);
          *pbVar1 = *pbVar1 & 0xfe;
        }
        *(undefined2 *)0x548 = 0xffff;
        uVar2 = *(undefined4 *)0x1ddc;
        *(undefined2 *)0x1bfe = (int)uVar2;
        *(undefined2 *)0x1c00 = (int)((ulong)uVar2 >> 0x10);
        uVar5 = (undefined2)((ulong)*(undefined4 *)0x1bfe >> 0x10);
        uVar4 = (undefined2)*(undefined4 *)0x1bfe;
        uVar3 = func_0x000e6d2f();
        *(undefined2 *)*(undefined4 *)0x1de0 = uVar3;
      }
    }
    else {
      uVar2 = *(undefined4 *)0x1ddc;
      *(int *)0x1bfe = (int)uVar2 + 0x201;
      *(undefined2 *)0x1c00 = (int)((ulong)uVar2 >> 0x10);
      uVar5 = (undefined2)((ulong)*(undefined4 *)0x1bfe >> 0x10);
      uVar4 = (undefined2)*(undefined4 *)0x1bfe;
      uVar3 = func_0x000e6d2f();
      *(undefined2 *)*(undefined4 *)0x1de0 = uVar3;
      *(undefined1 *)0x1a92 = 0;
    }
    func_0x000fa00c(0xe6a8,uVar4,uVar5);
    func_0x000f9fee(0xf9fe);
  }
  return;
}


/* detect_comm_option_hw @ 000e:75c0: DECOMPILE FAILED: Exception while decompiling 000e:75c0: ghidra.program.model.address.AddressOutOfBoundsException: Offset must be between 0x0 and 0x10ffef, got 0x400007de instead!
 */


void FUN_000e_766e(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  *(undefined2 *)0x752 = 0;
  *(char *)0x1a94 = *(char *)0x1a94 + '\x01';
  if ((*(byte *)0x1a94 & 0xb0) != 0) {
    *(char *)0x1a94 = (*(byte *)0x1a94 & 0x40) + 1;
  }
  do {
  } while (*(int *)0x752 < 0x200);
  func_0x000e6ec0();
  func_0x000fbcf3(0xe6a8);
  *(char *)0x1a9a = *(char *)0x1a9a + '\x01';
  if ((*(byte *)0x1a9a & 0xb0) != 0) {
    *(char *)0x1a9a = (*(byte *)0x1a9a & 0x40) + 1;
  }
  *(undefined1 *)0x766 = 1;
  func_0x000e6c85(0xfbcf);
  *(char *)0x1a95 = *(char *)0x1a95 + '\x01';
  uVar1 = 0xe6a8;
  if ((*(byte *)0x1a95 & 0xb0) != 0) {
    *(char *)0x1a95 = (*(byte *)0x1a95 & 0x40) + 1;
  }
  do {
    do {
      *(int *)0x768 = *(int *)0x768 + 1;
    } while (*(int *)0x768 != 0);
    if ((((*(char *)0x1b78 == '\0') && (*(code *)selftest_acq_ram == (code)0x0)) && (*"" != '\x11'))
       && ((*(char *)0x1b7a == '\0' && (*(char *)0x1bf7 == '\0')))) {
      *(undefined1 *)0x79c = 1;
      func_0x000e7416(uVar1);
      uVar1 = 0xe741;
    }
  } while( true );
}



undefined4 __stdcall16far FUN_000e_772e(uint param_1,uint param_2)

{
  uint in_AX;
  uint in_DX;
  
  return CONCAT22(in_DX & param_2,in_AX & param_1);
}



undefined4 __stdcall16far FUN_000e_773d(uint param_1,int param_2)

{
  uint in_AX;
  int in_DX;
  
  return CONCAT22(in_DX + param_2 + (uint)CARRY2(in_AX,param_1),in_AX + param_1);
}



/* ashr32 (confidence: Confirmed)
   
   Evidence: 32-bit arithmetic right shift by `CX` bits (`sar`+`rcr` loop) - paired with `mul32` to
   implement fast fixed-point scaling */

void __stdcall16far ashr32(int param_1)

{
  if (0 < param_1) {
    do {
      param_1 = param_1 + -1;
    } while (param_1 != 0);
  }
  return;
}



/* mul32 (confidence: Confirmed)
   
   Evidence: 32-bit x 32-bit -> 32-bit (truncated) multiply, classic 3-partial-product algorithm.
   Called by `scale_and_plot_point` */

undefined4 __stdcall16far mul32(uint param_1,int param_2)

{
  uint in_AX;
  int in_DX;
  
  return CONCAT22((int)((ulong)in_AX * (ulong)param_1 >> 0x10) + param_2 * in_AX + param_1 * in_DX,
                  (int)((ulong)in_AX * (ulong)param_1));
}



/* sdiv32 (confidence: Confirmed mechanism)
   
   Evidence: The real signed 32-bit divide (sign via XOR of the high words, `abs()` both operands,
   unsigned core, reapply sign) - found while correcting `mul32`/`ashr32` above; calls `udiv32` for
   the unsigned core */

int __stdcall16far sdiv32(undefined2 param_1,uint param_2)

{
  int iVar1;
  uint in_DX;
  
  iVar1 = func_0x000e783d();
  if ((int)(in_DX ^ param_2) < 0) {
    iVar1 = -iVar1;
  }
  return iVar1;
}



/* udiv32 (confidence: Confirmed)
   
   Evidence: The unsigned 32-bit/32-bit->32-bit divide `sdiv32` calls after handling signs: builds a
   small `ES:SS` stack-overlay scratch frame and calls `udiv32_core` to do the actual division, then
   reads the quotient back out of it */

undefined4 __stdcall16far udiv32(undefined2 param_1,undefined2 param_2)

{
  udiv32_core();
  return CONCAT22(param_2,param_1);
}



/* udiv32_core (confidence: Confirmed)
   
   Evidence: The restoring-division shift-subtract loop proper (near `call`, `SI:DI` accumulate the
   quotient bit by bit); divide-by-zero short-circuits to a saturated `0xFFFFFFFF` result; called
   only from `udiv32` */

undefined4 __cdecl16near udiv32_core(void)

{
  uint uVar1;
  uint uVar2;
  ulong uVar3;
  undefined2 in_AX;
  uint uVar4;
  uint uVar5;
  undefined2 in_DX;
  uint uVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  int unaff_DI;
  uint uVar10;
  undefined2 unaff_ES;
  bool bVar11;
  bool bVar12;
  
  uVar6 = *(uint *)(unaff_DI + -2);
  uVar5 = *(uint *)(unaff_DI + -4);
  if ((uVar5 != uVar6) || (uVar5 != 0)) {
    uVar7 = *(uint *)((undefined *)&DAT_000e_ff1a + unaff_DI);
    uVar4 = *(uint *)(unaff_DI + -8);
    uVar10 = 0;
    uVar9 = 0;
    iVar8 = 0;
    while( true ) {
      bVar11 = (int)uVar6 < 0;
      uVar6 = uVar6 << 1;
      uVar3 = (ulong)CONCAT12(bVar11,uVar5) << 1;
      uVar5 = (uint)uVar3 | (uint)bVar11;
      if ((uVar3 & 0x10000) != 0) break;
      iVar8 = iVar8 + 1;
    }
    uVar5 = (uint)((uVar3 & 0xffff | 0x10000) >> 1);
    uVar6 = (uint)(CONCAT12(bVar11,uVar6) >> 1);
    do {
      bVar11 = uVar7 < uVar6;
      uVar7 = uVar7 - uVar6;
      uVar1 = (uint)bVar11;
      bVar12 = uVar5 <= uVar4;
      uVar2 = uVar4 - uVar5;
      uVar4 = uVar2 - uVar1;
      bVar11 = bVar12 && uVar1 <= uVar2;
      if (!bVar12 || uVar1 > uVar2) {
        bVar12 = CARRY2(uVar7,uVar6);
        uVar7 = uVar7 + uVar6;
        uVar4 = uVar4 + uVar5 + (uint)bVar12;
      }
      uVar3 = (ulong)CONCAT12(bVar11,uVar9) << 1;
      uVar9 = (uint)uVar3 | (uint)bVar11;
      uVar10 = uVar10 << 1 | (uint)((uVar3 & 0x10000) != 0);
      uVar1 = uVar5 & 1;
      uVar5 = uVar5 >> 1;
      uVar6 = (uint)(CONCAT12(uVar1 != 0,uVar6) >> 1);
      bVar11 = 0 < iVar8;
      iVar8 = iVar8 + -1;
    } while (bVar11);
    *(uint *)((undefined *)&DAT_000e_ff1a + unaff_DI) = uVar9;
    *(uint *)(unaff_DI + -8) = uVar10;
  }
  return CONCAT22(in_DX,in_AX);
}



/* format_string_va (confidence: Confirmed mechanism; individual specifier handlers not all traced)
   
   Evidence: Walks a format string looking for `%` (`0x25`), dispatching per-specifier handlers that
   pull the next vararg off the stack (tracked via a caller-stack-relative far pointer). **The core
   printf-style variadic formatting engine** behind this firmware's `%d`/`%c` format strings (see
   `STRINGS.md`) - dispatches to `format_number`-style helpers per specifier */

void __stdcall16far format_string_va(char *param_1)

{
  char cVar1;
  char *pcVar2;
  char *pcVar3;
  char *pcVar4;
  undefined4 *puVar5;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined4 *local_e;
  undefined2 uVar6;
  
  _local_e = (undefined4 *)CONCAT22(unaff_SS,&stack0x0008);
  while( true ) {
    while( true ) {
      puVar5 = _local_e;
      pcVar3 = param_1;
      pcVar2 = (char *)param_1;
      param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
      pcVar4 = param_1;
      cVar1 = *pcVar3;
      if (cVar1 == '\0') {
        return;
      }
      if (cVar1 == '%') break;
      func_0x000e79d3(unaff_CS,cVar1);
      unaff_CS = 0xe791;
      _local_e = puVar5;
    }
    param_1 = (char *)CONCAT22(param_1._2_2_,pcVar2 + 2);
    cVar1 = *pcVar4;
    if (cVar1 == '\0') break;
    uVar6 = (undefined2)((ulong)_local_e >> 0x10);
    if (cVar1 == 'c') {
      _local_e = (undefined4 *)CONCAT22(uVar6,(undefined4 *)((int)local_e + 2));
      func_0x00083259(unaff_CS,*(undefined2 *)puVar5);
      unaff_CS = 0x82c9;
    }
    else if (cVar1 == 'd') {
      func_0x0009470e(unaff_CS,0x8f80);
      func_0x00081404(38000,*(undefined2 *)puVar5);
      func_0x0009470e(0x802c,0x41);
      _local_e = (undefined4 *)CONCAT22(uVar6,(undefined4 *)((int)local_e + 2));
      unaff_CS = 38000;
    }
    else if (cVar1 == 's') {
      func_0x000e7a0a(unaff_CS,(int)*puVar5,(int)((ulong)*puVar5 >> 0x10));
      _local_e = (undefined4 *)CONCAT22(uVar6,local_e + 1);
      unaff_CS = 0xe791;
    }
    else {
      func_0x000e79d3(unaff_CS,cVar1);
      unaff_CS = 0xe791;
      _local_e = puVar5;
    }
  }
  return;
}



/* putchar_serial_with_newline_handling (confidence: Confirmed)
   
   Evidence: `(char)` - for `0x0A` (LF), switches into the comm ROM's data segment and calls
   `send_serial_newline`; otherwise sends the byte directly via `putchar_serial_seg` */

void __stdcall16far putchar_serial_with_newline_handling(byte char)

{
  undefined2 uVar1;
  
  if (char == 10) {
    uVar1 = 0x8f80;
    func_0x0009470e();
    func_0x00096cb5(38000,uVar1);
    func_0x0009470e(0x9687,0x41);
  }
  else {
    func_0x00083259();
  }
  return;
}



/* print_string_serial_seg (confidence: Confirmed)
   
   Evidence: `(far_str_ptr)` - loops sending each byte via `putchar_serial_seg` until a NUL
   terminator - the `putchar_serial_seg` counterpart of the confirmed `print_string_serial` (which
   uses `serial_tx_buffer_put`) */

void __stdcall16far print_string_serial_seg(word str_off,word str_seg)

{
  char *pcVar1;
  undefined2 unaff_CS;
  
  while (pcVar1 = _str_off, *_str_off != '\0') {
    _str_off = (char *)CONCAT22(str_seg,(char *)(str_off + 1));
    func_0x00083259(unaff_CS,*pcVar1);
    unaff_CS = 0x82c9;
  }
  return;
}



void __stdcall16far
FUN_000e_7a33(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  
  *(undefined1 *)0x6fe = 0;
  uVar1 = param_3;
  func_0x000e7d7d();
  func_0x000e7e0d(0xe7d7,param_1,param_3,uVar1);
  func_0x000e7e0d(0xe7d7,param_1,param_4);
  func_0x000e7e0d(0xe7d7,param_2,param_4);
  func_0x000e7e0d(0xe7d7,param_2,param_3);
  func_0x000e7e0d(0xe7d7,param_1,param_3);
  func_0x000e7d7d(0xe7d7,param_1,param_3);
  return;
}



void __stdcall16far FUN_000e_7c27(void)

{
  undefined2 unaff_DS;
  
  if (*(int *)0x6ca == 0) {
    *(undefined2 *)0x6d0 = 0x1000;
    *(undefined2 *)0x6d2 = 0x40;
  }
  else {
    *(undefined2 *)0x6d0 = *(undefined2 *)0x6ce;
    *(int *)0x6d2 = (*(int *)0x6d4 * 0x80) / *(int *)0x6d6;
  }
  if (*(int *)0x6ca == 4) {
    *(int *)0x6d2 = *(int *)0x6d2 + 1;
  }
  return;
}



/* update_plot_position (confidence: Confirmed mechanism)
   
   Evidence: `(x, y)` - mode-dispatched on `[0x6CA]`: mode 0 computes a position via `SUB_E80E4`;
   mode 1 emits the HPGL command `PU%d,%d;` (pen-up move) via `format_string_va` and stores the
   position; modes 2/3 store directly. Part of the **HPGL plotter output driver** - see
   `MEMORY_MAP.md` I/O port `0x83` */

void __stdcall16far update_plot_position(word x,word y)

{
  int iVar1;
  undefined2 unaff_DS;
  
  iVar1 = *(int *)0x6ca;
  if (iVar1 == 0) {
    func_0x000e80e4();
    *(int *)0x6b2 = x << 2;
    *(int *)0x6b4 = y << 2;
  }
  else if (iVar1 == 1) {
    func_0x000e7911();
  }
  else if ((((iVar1 == 2) || (iVar1 == 3)) || (iVar1 == 4)) || (iVar1 == 8)) {
    *(int *)0x6b2 = x << 2;
    *(int *)0x6b4 = y << 2;
  }
  return;
}



/* plot_line_to (confidence: Confirmed)
   
   Evidence: `(x, y)` - HPGL `PD%d,%d;` (pen-down draw) - the drawing counterpart to
   `update_plot_position`'s pen-up move */

void __stdcall16far plot_line_to(word x,word y)

{
  int iVar1;
  undefined2 unaff_DS;
  
  iVar1 = *(int *)0x6ca;
  if (iVar1 == 0) {
    func_0x000e804f();
    *(int *)0x6b2 = x << 2;
    *(int *)0x6b4 = y << 2;
  }
  else if (iVar1 == 1) {
    func_0x000e7911();
  }
  else if ((((iVar1 == 2) || (iVar1 == 3)) || (iVar1 == 4)) || (iVar1 == 8)) {
    func_0x000e8e29();
    *(int *)0x6b2 = x << 2;
    *(int *)0x6b4 = y << 2;
  }
  return;
}



void __cdecl16far FUN_000e_7ebf(void)

{
  undefined2 unaff_DS;
  int local_a;
  
  *(undefined2 *)0x6d8 = 0;
  *(undefined2 *)(code *)verify_prc_readback_pattern = 0x8000;
  *(undefined2 *)0x6ec = 0x4000;
  if (*(int *)0x6ca != 0) {
    for (local_a = 0; local_a < 0x800; local_a = local_a + 1) {
      *(undefined2 *)((int)*(undefined4 *)(code *)verify_prc_readback_pattern + local_a * 2) = 0;
    }
  }
  return;
}



/* poll_error_flag_and_reset_channels (confidence: Mechanism confirmed; exact meaning of `[0x403]`
   not confirmed)
   
   Evidence: Checks status byte `[0x403]`; while set, increments a tick counter `[0x728]` and once
   it exceeds 500 (`0x1F4`) force-clears both channels via
   `clear_display_chip_int_reset`/`clear_display_chip_frame_trigger` and resets the counter; returns
   whether `[0x403]` was set - another persistent-error watchdog in the same family as
   `escalate_acq_timeout_reset` */

undefined2 __cdecl16far poll_error_flag_and_reset_channels(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  
  if (*"" == '\0') {
    uVar1 = 0;
  }
  else {
    *(int *)0x728 = *(int *)0x728 + 1;
    if (500 < *(int *)0x728) {
      func_0x000fa00c();
      func_0x000f9fee(0xf9fe);
      *(undefined2 *)0x728 = 0;
    }
    uVar1 = 1;
  }
  return uVar1;
}



/* plot_scaled_point (confidence: Confirmed mechanism)
   
   Evidence: Conditionally plots a scaled acquisition point (gated on `[0x70E]`/`[0x6CA]`, the HPGL
   PU/PD mode variable), then unconditionally plots it again as the new "from" point and caches it
   in `[0x70A]`/`[0x70C]` for the next line segment */

void __stdcall16far
plot_scaled_point(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
                 int param_5)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (param_5 != 0) {
    if ((*(int *)0x70e == 0) && ((*(int *)0x6ca == 0 || (*(int *)0x6ca == 1)))) {
      unaff_CS = 0xe90a;
      func_0x000e90a5();
    }
    func_0x000e90a5(unaff_CS,param_1,param_2,param_3,param_4,1);
    *(undefined2 *)0x70a = param_3;
    *(undefined2 *)0x70c = param_4;
  }
  *(int *)0x70e = param_5;
  return;
}



void __cdecl16far FUN_000e_7fa6(void)

{
  func_0x000e90a5();
  return;
}



/* update_indexed_value_if_changed (confidence: Mechanism confirmed)
   
   Evidence: Compares a table entry at `es:[bx+si]` against `dx`; if equal, returns immediately -
   else writes `dx` into the per-index table at far ptr `[0x1D1C]`, marks a "changed" byte at `[+4]`
   of that entry, and sets global flag `[0x532]=1`. Called right alongside `SUB_E8E29` from 2
   parallel code paths that both go on to update the plot-position cache `[0x6B2]`/`[0x6B4]` - the
   simpler ("just set it") half of that update, vs. `SUB_E8E29`'s more elaborate bounds-checked
   version */

void __stdcall16far update_indexed_value_if_changed(void)

{
  undefined2 in_AX;
  int iVar1;
  int in_DX;
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  char in_CF;
  
  iVar1 = CONCAT11((char)((uint)in_AX >> 8),((char)in_AX + -0x1d) - in_CF);
  if (*(int *)(iVar1 + unaff_SI) != in_DX) {
    *(undefined1 *)(iVar1 + (int)*(undefined4 *)0x1d1c + 4) = 1;
    *(undefined2 *)(iVar1 + (int)*(undefined4 *)0x1d1c) = *(undefined2 *)(unaff_BP + -8);
    *(undefined2 *)0x532 = 1;
  }
  return;
}



undefined2 __stdcall16far FUN_000e_8078(void)

{
  undefined2 unaff_DS;
  
  return unaff_DS;
}



void __cdecl16far FUN_000e_8086(void)

{
  byte bVar1;
  int iVar2;
  byte bVar3;
  undefined2 unaff_CS;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 unaff_DS;
  
  if (*(char *)0x1b7a == '\0') {
    bVar1 = *(byte *)0x4e7 & *(byte *)0x4e8;
    bVar3 = bVar1 & 99;
    if (((((*(byte *)0x4e8 & 99) == 0) && ((*(byte *)0x4f0 & 0x10) == 0)) && (*(int *)0x532 != 0))
       && (0 < *(int *)0x54a)) {
      *(undefined2 *)0x54a = 0;
      func_0x000f03f4();
    }
    else {
      if ((bVar1 & 99) != 0) {
        *(uint *)0x53a = *(uint *)0x53a | 0x200;
      }
      if ((bVar1 & 0x40) != 0) {
        if (*(int *)0x54a < 1) {
          *(undefined2 *)0x54a = 100;
          *(undefined2 *)0x544 = *(undefined2 *)0x752;
          *(int *)0x546 = *(int *)0x544 + 0x783;
          *(undefined2 *)0x54c = 0;
          *(byte *)0x1b76 = *(byte *)0x1b76 | 8;
          if (*(char *)0x1b76 == '\b') {
            unaff_CS = 0xe6a8;
            func_0x000e6ce8();
          }
        }
        else {
          unaff_CS = 0xf008;
          func_0x000f03f4();
          bVar3 = 0;
        }
      }
      if ((bVar3 & 0x23) != 0) {
        if (*(int *)0x54a < 1) {
          uVar5 = unaff_CS;
          if ((bVar3 & 2) != 0) {
            if (((*(byte *)0x1be4 | *(byte *)0x1be5) & 1) == 0) {
              uVar5 = 0xf8e6;
              func_0x000f8e66(unaff_CS,1);
            }
            else {
              uVar4 = unaff_CS;
              if (*(int *)0x570 == 1) {
                uVar4 = 0xeda2;
                func_0x000ee13b(unaff_CS,0);
              }
              uVar5 = 0xf008;
              func_0x000f0414(uVar4,1);
            }
          }
          uVar4 = uVar5;
          if ((bVar3 & 0x20) != 0) {
            if (((*(byte *)0x1be6 | *(byte *)0x1be7) & 1) == 0) {
              uVar4 = 0xf8e6;
              func_0x000f8e66(uVar5,2);
            }
            else {
              uVar6 = uVar5;
              if (*(int *)0x570 == 2) {
                uVar6 = 0xeda2;
                func_0x000ee13b(uVar5,0);
              }
              uVar4 = 0xf008;
              func_0x000f0414(uVar6,2);
            }
          }
          unaff_CS = uVar4;
          if ((bVar3 & 1) != 0) {
            if (((*(byte *)0x1be8 | *(byte *)0x1be9) & 1) == 0) {
              unaff_CS = 0xf8e6;
              func_0x000f8e66(uVar4,3);
            }
            else {
              uVar5 = uVar4;
              if (*(int *)0x570 == 3) {
                uVar5 = 0xeda2;
                func_0x000ee13b(uVar4,0);
              }
              unaff_CS = 0xf008;
              func_0x000f0414(uVar5,3);
            }
          }
        }
        else if ((bVar3 & 2) == 0) {
          if ((bVar3 & 0x20) == 0) {
            if ((bVar3 & 1) != 0) {
              func_0x000f0446(unaff_CS);
              iVar2 = func_0x000f0c2a(0xf008,1);
              if (iVar2 == 0) {
                func_0x000f706e(0xf0c2,0,3,1);
                func_0x000f0c81(0xf706,1);
              }
              unaff_CS = 0xf008;
              func_0x000f03f4(0xf0c2);
            }
          }
          else {
            if (*(int *)0x54c == 0) {
              func_0x000f0446(unaff_CS);
              iVar2 = func_0x000f0c2a(0xf008,1);
              if (iVar2 == 0) {
                func_0x000f706e(0xf0c2,0,2,1);
                func_0x000f0c81(0xf706,1);
              }
              func_0x000f03f4(0xf0c2);
            }
            else {
              iVar2 = func_0x000f0c2a(unaff_CS,1);
              if (iVar2 == 0) {
                func_0x000f706e(0xf0c2,0,1,1);
                func_0x000f0c81(0xf706,1);
              }
              func_0x000f03f4(0xf0c2);
              *(undefined2 *)0x54c = 0;
            }
            unaff_CS = 0xf008;
          }
        }
        else if ((((char *)s_sys_rom_0_A10_U9109_000e_009c)[0x10] & 1U) == 0) {
          iVar2 = func_0x000f0c2a(unaff_CS,1);
          if (iVar2 == 0) {
            func_0x000f706e(0xf0c2,0,1,1);
            func_0x000f0c81(0xf706,1);
          }
          func_0x000f03f4(0xf0c2);
          unaff_CS = 0xf008;
        }
        else if (*(int *)0x54c == 0) {
          *(undefined2 *)0x54c = 1;
          *(undefined2 *)0x544 = *(undefined2 *)0x752;
          *(int *)0x546 = *(int *)0x544 + 0x783;
        }
        else {
          *(undefined2 *)0x54c = 0;
          iVar2 = func_0x000f0c2a(unaff_CS,1);
          if (iVar2 == 0) {
            func_0x000f706e(0xf0c2,0,1,4);
            func_0x000f0414(0xf706,2);
            func_0x000f0414(0xf008,3);
            func_0x000f8f24(0xf008,2);
            func_0x000f8f24(0xf8e6,3);
            func_0x000f0c81(0xf8e6,1);
          }
          func_0x000f03f4(0xf0c2);
          unaff_CS = 0xf008;
        }
      }
      if (1 < *(int *)0x54a) {
        if (*(uint *)0x544 < *(uint *)0x546) {
          if ((*(uint *)0x546 < *(uint *)0x752) || (*(uint *)0x752 < *(uint *)0x544)) {
            func_0x000f03f4(unaff_CS);
          }
        }
        else if ((*(uint *)0x546 < *(uint *)0x752) && (*(uint *)0x752 < *(uint *)0x544)) {
          func_0x000f03f4(unaff_CS);
        }
      }
    }
  }
  return;
}



void __cdecl16far FUN_000e_83f4(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x54a = 0;
  *(uint *)0x53a = *(uint *)0x53a | 0x200;
  *(byte *)0x1b76 = *(byte *)0x1b76 & 0xf7;
  func_0x000e6d04();
  return;
}



void __stdcall16far FUN_000e_8414(int param_1)

{
  undefined2 unaff_DS;
  int iVar1;
  
  iVar1 = param_1;
  func_0x000f8e98();
  if (*(int *)0x570 == param_1) {
    *(undefined2 *)0x570 = 0;
    *(undefined2 *)0x56e = 0;
    func_0x000ee13b(0xf8e6,*(undefined2 *)0x570,iVar1);
  }
  return;
}



void __cdecl16far FUN_000e_8446(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  
  if ((((*(byte *)0x1be4 | *(byte *)0x1be5) & 2) != 0) && ((*(byte *)0x19c & 1) != 0)) {
    uVar1 = 1;
    func_0x000f0414();
    func_0x000f8f24(0xf008,1,uVar1);
  }
  return;
}



void __cdecl16far FUN_000e_847c(void)

{
  undefined2 unaff_DS;
  
  if (*(int *)0x570 != 0) {
    *(undefined2 *)0x570 = 0;
    *(undefined2 *)0x56e = 0;
    func_0x000ee13b();
  }
  return;
}



void __cdecl16far FUN_000e_84a2(void)

{
  byte bVar1;
  byte bVar2;
  char cVar3;
  undefined1 uVar4;
  byte bVar5;
  byte bVar6;
  int iVar7;
  byte bVar8;
  undefined2 unaff_CS;
  undefined2 uVar9;
  undefined2 unaff_DS;
  undefined2 uVar10;
  
  bVar1 = *(byte *)0x4fc;
  bVar5 = *(byte *)0x4f8;
  bVar8 = bVar5 & 0xc;
  bVar2 = *(byte *)0x4fb;
  bVar6 = *(byte *)0x4f7;
  if ((*(byte *)0x4fc & 8) != 0) {
    if ((*(byte *)0x4fb & 8) == 0) {
      *(byte *)0x565 = *(byte *)0x565 & 0xfe;
      *(uint *)0x552 = *(uint *)0x552 | 0x10;
      *(byte *)0x567 = *(byte *)0x567 & 0xfe;
      *(uint *)0x556 = *(uint *)0x556 | 0x10;
    }
    else {
      *(byte *)0x565 = *(byte *)0x565 | 1;
      *(uint *)0x552 = *(uint *)0x552 | 0x10;
      *(byte *)0x567 = *(byte *)0x567 | 1;
      *(uint *)0x556 = *(uint *)0x556 | 0x10;
    }
  }
  if (bVar8 != 0 || (bVar1 & 4) != 0) {
    *(uint *)0x53a = *(uint *)0x53a | 0x200;
    if ((bVar8 & bVar6 & 0xc & 8) == 0) {
      if ((bVar5 & 8) != 0) {
        unaff_CS = 0xf0c2;
        iVar7 = func_0x000f0c2a();
        if (iVar7 == 0) {
          func_0x000f0414(0xf0c2,1);
          func_0x000f8f24(0xf008,1);
          func_0x000f0414(0xf8e6,3);
          func_0x000f8f24(0xf008,3);
          unaff_CS = 0xf0c2;
          func_0x000f0c81(0xf8e6,1);
        }
      }
    }
    else {
      uVar10 = 1;
      uVar9 = 0xf0c2;
      iVar7 = func_0x000f0c2a();
      if (iVar7 == 0) {
        *(undefined2 *)0x54c = 0;
        *(byte *)0x1b76 = *(byte *)0x1b76 | 8;
        if (*(char *)0x1b76 == '\b') {
          uVar9 = 0xe6a8;
          func_0x000e6ce8(0xf0c2,uVar10);
        }
        func_0x000f706e(uVar9,0,1,4);
        func_0x000f0414(0xf706,2);
        func_0x000f0414(0xf008,3);
        func_0x000f8f24(0xf008,2);
        func_0x000f8f24(0xf8e6,3);
        func_0x000f706e(0xf8e6,0,4,1);
        cVar3 = ((char *)s_sys_rom_0_A10_U9109_000e_009c)[0x10];
        uVar4 = *(undefined1 *)0x20;
        ((char *)s_sys_rom_0_A10_U9109_000e_009c)[0x10] =
             ((char *)s_sys_rom_0_A10_U9109_000e_009c)[0x10] | 0x40;
        if ((*(byte *)0x20 & 0x10) != 0) {
          *(byte *)0x20 = *(byte *)0x20 | 3;
        }
        if (*(byte *)0x1f < 3) {
          func_0x000f6817(0xf706);
        }
        else {
          func_0x000f690b(0xf706);
        }
        func_0x000f706e(0xf670,0,3,1);
        ((char *)s_sys_rom_0_A10_U9109_000e_009c)[0x10] = cVar3;
        *(undefined1 *)0x20 = uVar4;
        func_0x000f0c81(0xf706,1);
      }
      unaff_CS = 0xf008;
      func_0x000f03f4(0xf0c2);
    }
    if ((bVar8 & bVar6 & 0xc & 4) == 0) {
      if ((bVar5 & 4) != 0) {
        func_0x000f0414(unaff_CS,1);
        unaff_CS = 0xf008;
        func_0x000f0414(0xf008,3);
      }
    }
    else if ((bVar2 & 4) == 0) {
      func_0x000f8e66(unaff_CS,1);
      unaff_CS = 0xf8e6;
    }
    else {
      func_0x000f8e66(unaff_CS,3);
      unaff_CS = 0xf8e6;
    }
    if (((*(byte *)0x1be4 | *(byte *)0x1be5 | *(byte *)0x1be8 | *(byte *)0x1be9) & 1) != 0) {
      if ((bVar2 & 4) == 0) {
        func_0x000f0414(unaff_CS,3);
        func_0x000f8e66(0xf008,1);
      }
      else {
        func_0x000f0414(unaff_CS,1);
        func_0x000f8e66(0xf008,3);
      }
    }
  }
  return;
}



void __stdcall16far FUN_000e_8c81(int param_1)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  uVar3 = 0x41;
  uVar2 = func_0x000f0078();
  iVar1 = *(int *)0x54e;
  if (iVar1 != param_1) {
    *(undefined2 *)0x54e = 0;
  }
  else {
    *(undefined2 *)0x54e = 0;
  }
  *(uint *)0x548 = *(uint *)0x548 | 0x200;
  func_0x000f0078(0xf007,uVar2,uVar3,iVar1 != param_1);
  return;
}



void __cdecl16far FUN_000e_8ccc(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x52f = 1;
  return;
}



void __cdecl16far FUN_000e_8cdb(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (*(char *)0x52f != '\0') {
    if ((*(char *)0x1bf9 != '\0') && (*"" != '\0')) {
      unaff_CS = 0x911e;
      func_0x000916d6();
    }
    *(byte *)0x1b8d = *(byte *)0x1b8d | 1;
    *(byte *)0x1b8e = *(byte *)0x1b8e | 1;
    func_0x000e6c85(unaff_CS);
    *(undefined1 *)0x52f = 0;
  }
  return;
}



void __cdecl16far FUN_000e_8d64(void)

{
  int *piVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  int local_16;
  char local_13;
  code *local_12;
  int local_10;
  undefined4 local_e;
  int local_a;
  
  local_e = (int *)*(undefined4 *)0x1d58;
  do {
    local_13 = '\0';
    do {
      uVar3 = (undefined2)((ulong)local_e >> 0x10);
      piVar1 = (int *)local_e;
      if ((**(byte **)local_e & *(byte *)(piVar1 + 2)) != 0) {
        local_13 = '\x01';
      }
      local_10 = (int)((ulong)*(code **)(piVar1 + 3) >> 0x10);
      local_12 = (code *)*(code **)(piVar1 + 3);
      local_e = (int *)CONCAT22(uVar3,piVar1 + 5);
    } while (local_10 == 0 && local_12 == (code *)0x0);
    if (local_13 == '\x01') {
      local_16 = local_10;
      (*local_12)();
    }
  } while (*local_e != 0 || ((int *)local_e)[1] != 0);
  if (*"" != '\0') {
    *(undefined2 *)0x1b84 = *(undefined2 *)0x50e;
  }
  if (*(char *)0x51a != '\0') {
    *(undefined2 *)0x1b86 = *(undefined2 *)0x516;
  }
  if (*(char *)0x522 != '\0') {
    *(undefined2 *)0x1b88 = *(undefined2 *)0x51e;
    *(uint *)0x53a = *(uint *)0x53a | 0x40;
  }
  if ((*(int *)0x52a != 0) ||
     (((*(byte *)0x4e7 & 0x80) == 0x80 && ((*(byte *)0x4e8 & 0x80) == 0x80)))) {
    unaff_CS = 0xef44;
    func_0x000ef440();
  }
  func_0x000ef346(unaff_CS);
  func_0x000ef393(0xef34);
  uVar3 = 0xeea5;
  func_0x000eea58(0xef34);
  if (((*(char *)0x1bf9 != '\0') && (*"" != '\0')) && ((*(byte *)0x4e8 & 0x73) != 0)) {
    uVar3 = 0x911e;
    func_0x000916d6(0xeea5);
  }
  func_0x000f0086(uVar3);
  func_0x000efb64(0xf008);
  for (local_a = 0; local_a < 5; local_a = local_a + 1) {
    iVar2 = local_a * 2;
    if (*(int *)(iVar2 + 0x550) != 0) {
      *(uint *)(iVar2 + 0x55a) = *(uint *)(iVar2 + 0x55a) | *(uint *)(iVar2 + 0x550);
      *(undefined2 *)(iVar2 + 0x550) = 0;
    }
  }
  for (local_a = 0; local_a < 4; local_a = local_a + 1) {
    *(undefined1 *)(local_a * 4 + 0x4e8) = 0;
  }
  for (local_a = 5; local_a < 0x10; local_a = local_a + 1) {
    *(undefined1 *)(local_a * 6 + (int)*(undefined4 *)0x1d1c + 4) = 0;
  }
  return;
}



void __stdcall16far FUN_000e_9001(undefined1 param_1)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x4e4 = param_1;
  func_0x000f15f8();
  func_0x000e6224(0xf15f);
  func_0x000f15a1(0xe60b);
  return;
}



uint __stdcall16far FUN_000e_9023(int param_1)

{
  undefined2 uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  undefined2 uVar6;
  undefined2 local_a;
  
  if (param_1 == 0) {
    uVar6 = 0x5d;
    uVar1 = func_0x000f1001();
    uVar2 = func_0x000f1611(0xf0eb,uVar1,0,uVar6,uVar1);
    uVar1 = func_0x000f1001(0xf161,0xfd);
    uVar3 = func_0x000f1611(0xf0eb,uVar1,1);
    local_a = uVar2 | uVar3;
  }
  else if (param_1 == 1) {
    uVar6 = 0x9f;
    uVar1 = func_0x000f1001();
    local_a = func_0x000f1611(0xf0eb,uVar1,2,uVar6,uVar1);
  }
  else if (param_1 == 2) {
    uVar6 = 0xbf;
    uVar1 = func_0x000f1001();
    local_a = func_0x000f1611(0xf0eb,uVar1,3,uVar6,uVar1);
  }
  else if (param_1 == 3) {
    uVar6 = 0x1f;
    uVar1 = func_0x000f1001();
    local_a = func_0x000f1611(0xf0eb,uVar1,4,uVar6,uVar1);
  }
  else if (param_1 == 4) {
    uVar6 = 0x7d;
    uVar1 = func_0x000f1001();
    local_a = func_0x000f1611(0xf0eb,uVar1,5,uVar6,uVar1);
  }
  else if (param_1 == 5) {
    uVar6 = 0xdd;
    uVar1 = func_0x000f1001();
    local_a = func_0x000f1611(0xf0eb,uVar1,6,uVar6,uVar1);
  }
  else if (param_1 == 6) {
    uVar6 = 0x7f;
    uVar1 = func_0x000f1001();
    local_a = func_0x000f1611(0xf0eb,uVar1,7,uVar6,uVar1);
  }
  else if (param_1 == 7) {
    uVar6 = 0xff;
    uVar1 = func_0x000f1001();
    local_a = func_0x000f1611(0xf0eb,uVar1,8,uVar6,uVar1);
  }
  else if (param_1 == 8) {
    uVar6 = 0xdf;
    uVar1 = func_0x000f1001();
    local_a = func_0x000f1611(0xf0eb,uVar1,9,uVar6,uVar1);
  }
  else if (param_1 == 9) {
    uVar6 = 0x5f;
    uVar1 = func_0x000f1001();
    local_a = func_0x000f1611(0xf0eb,uVar1,10,uVar6,uVar1);
  }
  else if (param_1 == 10) {
    uVar6 = 0x3d;
    uVar1 = func_0x000f1001();
    uVar2 = func_0x000f1611(0xf0eb,uVar1,0xb,uVar6,uVar1);
    uVar1 = func_0x000f1001(0xf161,0x9d);
    uVar3 = func_0x000f1611(0xf0eb,uVar1,0xc);
    uVar1 = func_0x000f1001(0xf161,0xbd);
    uVar4 = func_0x000f1611(0xf0eb,uVar1,0xd);
    uVar1 = func_0x000f1001(0xf161,0x1d);
    uVar5 = func_0x000f1611(0xf0eb,uVar1,0xe);
    local_a = uVar2 | uVar3 | uVar4 | uVar5;
  }
  return local_a;
}



uint __cdecl16far FUN_000e_9254(void)

{
  undefined2 uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  undefined2 uVar6;
  
  uVar6 = 0x3d;
  uVar1 = func_0x000f1001();
  uVar2 = func_0x000f1611(0xf0eb,uVar1,0xb,uVar6,uVar1);
  uVar1 = func_0x000f1001(0xf161,0x9d);
  uVar3 = func_0x000f1611(0xf0eb,uVar1,0xc);
  uVar1 = func_0x000f1001(0xf161,0x9f);
  uVar4 = func_0x000f1611(0xf0eb,uVar1,2);
  uVar1 = func_0x000f1001(0xf161,0xbf);
  uVar5 = func_0x000f1611(0xf0eb,uVar1,3);
  return uVar2 | uVar3 | uVar4 | uVar5;
}



/* assert_and_halt (confidence: Confirmed)
   
   Evidence: Computes a value via `convert_sample_value`, then calls `halt_cpu` with that value plus
   a small constant tag - the firmware's assertion-failure/panic mechanism */

uint __cdecl16far assert_and_halt(void)

{
  undefined2 uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  undefined2 uVar6;
  
  uVar6 = 0x3d;
  uVar1 = func_0x000f1001();
  uVar2 = func_0x000f1611(0xf0eb,uVar1,0xb,uVar6,uVar1);
  uVar1 = func_0x000f1001(0xf161,0x9d);
  uVar3 = func_0x000f1611(0xf0eb,uVar1,0xc);
  uVar1 = func_0x000f1001(0xf161,0x9f);
  uVar4 = func_0x000f1611(0xf0eb,uVar1,2);
  uVar1 = func_0x000f1001(0xf161,0xbf);
  uVar5 = func_0x000f1611(0xf0eb,uVar1,3);
  return uVar2 | uVar3 | uVar4 | uVar5;
}



void __cdecl16far FUN_000e_92d1(void)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_DS;
  undefined2 local_10;
  undefined2 local_c;
  
  if (*"" == '\x14') {
    *(undefined1 *)0x4e7 = 0x10;
    *(undefined1 *)0x4eb = 0xbf;
    *(undefined1 *)0x1b72 = 1;
    *(undefined1 *)0x4f8 = 0xff;
    *(undefined1 *)0x4fc = 0xff;
    *(undefined1 *)0x4f4 = 0xff;
    *(undefined1 *)0x4f0 = 0xff;
    *(byte *)0x4fb = *(byte *)0x4fb | 0x10;
    *(undefined1 *)((int)*(undefined4 *)0x1d1c + 0x46) = 1;
    *(undefined1 *)((int)*(undefined4 *)0x1d1c + 0x4c) = 1;
    iVar2 = func_0x000ee879();
    for (local_10 = 0; local_10 < 4; local_10 = local_10 + 1) {
      iVar3 = local_10 * 0x1c;
      *(int *)(iVar3 + 0x574) = iVar2 / 2;
      *(int *)(iVar3 + 0x576) = iVar2 / 2;
      *(undefined1 *)(iVar3 + 0x580) = 4;
      *(undefined1 *)(local_10 + 0x564) = 4;
      puVar1 = (uint *)(local_10 * 2 + 0x55a);
      *puVar1 = *puVar1 | 0x100;
    }
    *(undefined1 *)0x4e0 = 1;
  }
  else {
    *(undefined1 *)0x4f7 = 0;
    *(undefined1 *)0x4fb = 0xff;
    *(undefined1 *)0x4e8 = 0xff;
    *(undefined1 *)0x4ec = 0xff;
    *(undefined1 *)0x4f4 = 0xff;
    *(undefined1 *)0x4f0 = 0xff;
    for (local_c = 5; local_c < 0xf; local_c = local_c + 1) {
      *(undefined1 *)(local_c * 6 + (int)*(undefined4 *)0x1d1c + 4) = 1;
    }
    func_0x000f1023();
    *(undefined2 *)0x52a = 0;
  }
  *(undefined2 *)0x54e = 0;
  return;
}



void __cdecl16far FUN_000e_93d8(void)

{
  byte bVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  int local_18;
  undefined4 local_16;
  char *local_12;
  int local_e;
  
  if (*"" == '\x14') {
    local_e = 2;
    local_18 = 5;
  }
  else {
    local_e = 0;
    local_18 = 3;
  }
  for (; local_e <= local_18; local_e = local_e + 1) {
    if (((*(char *)0x530 == '\0') || ((*(char *)0x530 != '\0' && (local_e == 0)))) ||
       ((*(char *)0x530 != '\0' && (local_e == 1)))) {
      iVar3 = local_e * 4;
      _local_12 = (char *)CONCAT22(unaff_DS,(char *)(iVar3 + 0x4e6));
      uVar4 = (undefined2)((ulong)*(undefined4 *)0x1d50 >> 0x10);
      puVar2 = (undefined4 *)((int)*(undefined4 *)0x1d50 + local_e * 6);
      local_16 = (undefined4 *)CONCAT22(uVar4,puVar2);
      bVar1 = *(byte *)*local_16;
      if (local_e == 2) {
        bVar1 = bVar1 | 1;
      }
      bVar1 = bVar1 ^ *(byte *)(puVar2 + 1);
      if (*_local_12 == '\0') {
        if (bVar1 != *(byte *)(iVar3 + 0x4e9)) {
          *(byte *)(iVar3 + 0x4e9) = bVar1;
          *_local_12 = '\x01';
        }
      }
      else if (bVar1 == *(byte *)(iVar3 + 0x4e9)) {
        *(byte *)(iVar3 + 0x4e8) = *(byte *)(iVar3 + 0x4e7) ^ bVar1;
        *(byte *)(iVar3 + 0x4e7) = bVar1;
        *_local_12 = '\0';
        if (*(char *)(iVar3 + 0x4e8) != '\0') {
          *(undefined2 *)0x532 = 1;
        }
      }
      else {
        *(byte *)(iVar3 + 0x4e9) = bVar1;
      }
    }
  }
  *(bool *)0x1b74 = (*(byte *)0x4f1 & 0x10) != 0;
  return;
}



void __stdcall16far FUN_000e_9500(int param_1,int param_2)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x52e == 'a') {
    *(int *)0x52a = *(int *)0x526 - param_1;
    if ((param_1 < 0xff) || (0x300 < param_1)) {
      *(undefined1 *)0x52e = 0x62;
    }
    else {
      *(undefined1 *)0x52e = 0x61;
    }
  }
  else {
    *(int *)0x52a = param_2 - *(int *)0x528;
    if ((param_2 < 0xff) || (0x300 < param_2)) {
      *(undefined1 *)0x52e = 0x61;
    }
    else {
      *(undefined1 *)0x52e = 0x62;
    }
  }
  *(int *)0x526 = param_1;
  *(int *)0x528 = param_2;
  return;
}



uint __cdecl16far FUN_000e_95a1(void)

{
  undefined2 unaff_DS;
  
  DAT_4000_37f6 = *(undefined1 *)0x4e4;
  return (uint)(DAT_4000_37fa >> 6) | (uint)DAT_4000_37fa << 2;
}



uint __stdcall16far FUN_000e_9611(undefined2 param_1,uint param_2)

{
  undefined2 unaff_DS;
  
  if (param_2 == 0) {
    func_0x000efdc2();
    param_2 = (uint)*(byte *)0x502;
  }
  else if (param_2 == 1) {
    func_0x000efdc2();
    func_0x000f1500(0xefdc,*(undefined2 *)0x4fe,*(undefined2 *)0x506,param_2);
    param_2 = (uint)*(byte *)0x50a;
  }
  else if (((param_2 == 2) || (param_2 == 3)) || (param_2 == 4)) {
    func_0x000efdc2();
    param_2 = (uint)*(byte *)(param_2 * 8 + 0x502);
  }
  else if ((((param_2 == 5) || (param_2 == 6)) ||
           ((param_2 == 7 || ((param_2 == 8 || (param_2 == 9)))))) ||
          ((param_2 == 10 ||
           ((((param_2 == 0xb || (param_2 == 0xc)) || (param_2 == 0xd)) || (param_2 == 0xe)))))) {
    func_0x000effee();
    param_2 = (uint)*(byte *)(param_2 * 6 + (int)*(undefined4 *)0x1d1c + 4);
  }
  return param_2;
}



/* extract_strided_channel_samples (confidence: Mechanism (strided copy, multi-entry shape)
   confirmed by direct trace; the specific "channel de-interleaving" interpretation is a strong
   inference from its 3 callers all being per-channel measurement/readout functions, not
   independently proven)
   
   Evidence: `(far ptr src, far ptr dst, count, skip, elem_size_flag, sub_offset_flag)` conceptually
   - **this primary entry point's own real stack order is `(src_off, src_seg, skip, dst_off,
   dst_seg, count, elem_size_flag)`**, confirmed 2026-09-16; `sub_offset_flag`'s offset wasn't found
   within this entry's own reachable body (only used via the other 2 entry points below, not
   independently confirmed) - found 2026-09-14 tracing what
   `compute_and_format_sample_delta_readout`, `compute_and_print_item_delta_readout`, and
   `compute_and_print_cursor_position_readout` (all 3 landing-artifact finds from this session) call
   as `SUB_E97DC`. The real entry point is here; `0xE97CA` and `0xE97DC` are legitimate **secondary
   entry points** (real, unambiguous `lcall` targets, not byte-corruption artifacts) that skip this
   function's own remainder-alignment preamble - the same "caller already has the parameters
   computed" pattern documented for `write_hw_shift_register`. Copies every Nth byte/word from `src`
   to `dst` with a caller-selected stride (2/3/6 bytes seen across the 3 entry points), gated by 2
   flag bytes selecting element size and which interleaved sub-stream to extract */

uint __stdcall16far
extract_strided_channel_samples
          (word src_off,word src_seg,word skip,word dst_off,word dst_seg,word count,
          word elem_size_flag)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  undefined2 *puVar5;
  undefined1 *puVar6;
  undefined2 *puVar7;
  
  iVar1 = 1;
  if ((char)elem_size_flag != '\0') {
    iVar1 = 2;
  }
  for (; 0 < (int)count; count = count - 1) {
    src_off = src_off + iVar1;
    skip = skip - 1;
  }
  uVar2 = 4 - (skip & 3) & 3;
  uVar3 = skip >> 2;
  if (uVar2 != 0) {
    uVar3 = uVar3 + 1;
  }
  if (uVar3 != 0) {
    if ((char)elem_size_flag == '\0') {
      do {
        do {
          puVar6 = (undefined1 *)dst_off;
          puVar5 = (undefined2 *)src_off;
          uVar4 = uVar3;
          *puVar6 = *(undefined1 *)puVar5;
          if (uVar4 - 1 == 0) {
            return uVar2;
          }
          uVar3 = uVar4 - 1;
          src_off = (word)(puVar5 + 2);
          dst_off = (word)(puVar6 + 1);
        } while (elem_size_flag._1_1_ != '\0');
        puVar6[1] = *(undefined1 *)((int)puVar5 + 5);
        if (uVar4 == 2) {
          return uVar2;
        }
        puVar6[2] = *(undefined1 *)((int)puVar5 + 9);
        if (uVar4 == 3) {
          return uVar2;
        }
        puVar6[3] = *(undefined1 *)(puVar5 + 6);
        uVar3 = uVar4 - 4;
        src_off = (word)(puVar5 + 8);
        dst_off = (word)(puVar6 + 4);
      } while (uVar4 - 4 != 0);
    }
    else {
      do {
        do {
          puVar7 = (undefined2 *)dst_off;
          puVar5 = (undefined2 *)src_off;
          uVar4 = uVar3;
          *puVar7 = *puVar5;
          if (uVar4 - 1 == 0) {
            return uVar2;
          }
          uVar3 = uVar4 - 1;
          src_off = (word)(puVar5 + 4);
          dst_off = (word)(puVar7 + 1);
        } while (elem_size_flag._1_1_ != '\0');
        puVar7[1] = puVar5[5];
        if (uVar4 == 2) {
          return uVar2;
        }
        puVar7[2] = puVar5[9];
        if (uVar4 == 3) {
          return uVar2;
        }
        puVar7[3] = puVar5[0xc];
        uVar3 = uVar4 - 4;
        src_off = (word)(puVar5 + 0x10);
        dst_off = (word)(puVar7 + 4);
      } while (uVar4 - 4 != 0);
    }
  }
  return uVar2;
}



uint __stdcall16far
FUN_000e_9808(uint *param_1,uint param_2,uint *param_3,int param_4,int param_5,int param_6)

{
  bool bVar1;
  byte bVar2;
  int iVar3;
  byte bVar4;
  uint uVar5;
  uint uVar6;
  byte bVar8;
  uint uVar7;
  uint *puVar9;
  uint *puVar10;
  uint *puVar11;
  uint *puVar12;
  uint *puVar13;
  undefined2 uVar14;
  undefined2 unaff_SS;
  undefined2 uVar15;
  char local_8;
  uint local_6;
  
  uVar15 = (undefined2)((ulong)param_1 >> 0x10);
  puVar9 = (uint *)param_1;
  uVar14 = (undefined2)((ulong)param_3 >> 0x10);
  puVar12 = (uint *)param_3;
  iVar3 = 1;
  if (param_5 != 0) {
    iVar3 = 2;
    param_2 = param_2 >> 1;
  }
  for (; 0 < param_4; param_4 = param_4 + -1) {
    puVar9 = (uint *)((int)puVar9 - iVar3);
    param_2 = param_2 + 1;
  }
  uVar6 = param_2 >> 3;
  if (uVar6 != 0) {
    puVar10 = puVar9;
    puVar13 = puVar12;
    if (param_5 == 0) {
      do {
        iVar3 = 8;
        bVar2 = (byte)*puVar10;
        bVar4 = bVar2;
        bVar8 = bVar2;
        while( true ) {
          puVar10 = (uint *)((int)puVar10 + 1);
          if (bVar2 < bVar4) {
            local_8 = '\x01';
            bVar4 = bVar2;
          }
          if (bVar8 < bVar2) {
            local_8 = '\0';
            bVar8 = bVar2;
          }
          iVar3 = iVar3 + -1;
          if (iVar3 == 0) break;
          bVar2 = (byte)*puVar10;
        }
        if (local_8 == '\0') {
          *(byte *)puVar13 = bVar4;
          if (bVar8 < (byte)*puVar10) {
            *(byte *)((int)puVar13 + 1) = (byte)(CONCAT11(CARRY1(bVar8,bVar4),bVar8 + bVar4) >> 1);
          }
          else {
            *(byte *)((int)puVar13 + 1) = bVar8;
          }
        }
        else {
          *(byte *)puVar13 = bVar8;
          if ((byte)*puVar10 < bVar4) {
            *(byte *)((int)puVar13 + 1) = (byte)(CONCAT11(CARRY1(bVar4,bVar8),bVar4 + bVar8) >> 1);
          }
          else {
            *(byte *)((int)puVar13 + 1) = bVar4;
          }
        }
        puVar13 = puVar13 + 1;
        uVar6 = uVar6 - 1;
      } while (uVar6 != 0);
    }
    else {
      do {
        iVar3 = 8;
        uVar5 = *puVar10;
        uVar7 = uVar5;
        puVar11 = puVar10;
        local_6 = uVar5;
        while( true ) {
          puVar10 = puVar11 + 1;
          if (uVar5 < uVar7) {
            local_8 = '\x01';
            uVar7 = uVar5;
          }
          if (local_6 < uVar5) {
            local_8 = '\0';
            local_6 = uVar5;
          }
          iVar3 = iVar3 + -1;
          if (iVar3 == 0) break;
          uVar5 = *puVar10;
          puVar11 = puVar10;
        }
        if (local_8 == '\0') {
          *puVar13 = uVar7;
          if (local_6 == *puVar11) {
            puVar13[1] = (uint)(CONCAT12(CARRY2(local_6,uVar7),local_6 + uVar7) >> 1);
          }
          else {
            puVar13[1] = local_6;
          }
        }
        else {
          *puVar13 = local_6;
          if (uVar7 == *puVar11) {
            puVar13[1] = (uint)(CONCAT12(CARRY2(uVar7,local_6),uVar7 + local_6) >> 1);
          }
          else {
            puVar13[1] = uVar7;
          }
        }
        puVar13 = puVar13 + 2;
        uVar6 = uVar6 - 1;
      } while (uVar6 != 0);
    }
  }
  if (param_6 == 0) {
    iVar3 = *(int *)0x1c02;
    uVar6 = param_2;
    if (param_5 == 0) {
      do {
        bVar2 = *(byte *)(iVar3 + (int)puVar9) & 3;
        if (bVar2 != 3) {
          bVar2 = 1;
        }
        if ((*(byte *)(iVar3 + (int)puVar12) != 8) &&
           (uVar5 = CONCAT11(*(byte *)(iVar3 + (int)puVar12),bVar2) & 0x3ff,
           (char)(uVar5 >> 8) != '\x02')) {
          uVar5 = CONCAT11(*(byte *)(iVar3 + (int)puVar12),(char)uVar5) & 0xfcff;
          *(byte *)(iVar3 + (int)puVar12) = (byte)(uVar5 >> 8) | (byte)uVar5;
        }
        puVar9 = puVar9 + 2;
        puVar12 = (uint *)((int)puVar12 + 1);
        uVar5 = uVar6 - 4;
        bVar1 = 3 < (int)uVar6;
        uVar6 = uVar5;
      } while (uVar5 != 0 && bVar1);
    }
    else {
      do {
        bVar2 = (byte)((uint)*(undefined2 *)(byte *)(iVar3 + (int)puVar9) >> 8) & 3;
        if (bVar2 != 3) {
          bVar2 = 1;
        }
        bVar4 = (byte)((uint)*(undefined2 *)(byte *)(iVar3 + (int)puVar12) >> 8);
        if ((bVar4 != 8) && ((bVar4 & 3) != 2)) {
          bVar2 = (byte)((uint)*(undefined2 *)(byte *)(iVar3 + (int)puVar12) >> 8) & 0xfc | bVar2;
          *(uint *)(byte *)(iVar3 + (int)puVar12) = CONCAT11(bVar2,bVar2);
        }
        puVar9 = puVar9 + 4;
        puVar12 = puVar12 + 1;
        uVar5 = uVar6 - 4;
        bVar1 = 3 < (int)uVar6;
        uVar6 = uVar5;
      } while (uVar5 != 0 && bVar1);
    }
  }
  return param_2 & 7;
}



void __stdcall16far FUN_000e_999e(int *param_1,uint param_2,int *param_3,int param_4,int param_5)

{
  int *piVar1;
  bool bVar2;
  char cVar3;
  byte bVar4;
  int iVar5;
  byte bVar6;
  uint uVar7;
  uint uVar8;
  int *piVar9;
  int *piVar10;
  int *piVar11;
  int *piVar12;
  undefined2 uVar13;
  undefined2 unaff_SS;
  undefined2 uVar14;
  
  uVar14 = (undefined2)((ulong)param_1 >> 0x10);
  piVar9 = (int *)param_1;
  uVar13 = (undefined2)((ulong)param_3 >> 0x10);
  piVar11 = (int *)param_3;
  uVar7 = param_2 >> 2;
  if (uVar7 != 0) {
    uVar8 = uVar7;
    piVar10 = piVar9;
    piVar12 = piVar11;
    if (param_4 == 0) {
      do {
        cVar3 = (char)*piVar10;
        if (cVar3 == '\0') {
          cVar3 = *(char *)((int)piVar10 + 1);
          if (cVar3 == '\0') {
            cVar3 = (char)piVar10[1];
            if (cVar3 == '\0') {
              cVar3 = *(char *)((int)piVar10 + 3);
            }
          }
        }
        piVar10 = piVar10 + 2;
        piVar1 = piVar12;
        piVar12 = (int *)((int)piVar12 + 1);
        *(char *)piVar1 = cVar3;
        uVar8 = uVar8 - 1;
      } while (uVar8 != 0);
    }
    else {
      uVar7 = param_2 >> 3;
      uVar8 = uVar7;
      if (param_5 == 0x1e) {
        do {
          iVar5 = *piVar10;
          if (iVar5 == 0) {
            iVar5 = piVar10[1];
            if (iVar5 == 0) {
              iVar5 = piVar10[2];
              if (iVar5 == 0) {
                iVar5 = piVar10[3];
              }
            }
          }
          piVar10 = piVar10 + 4;
          piVar1 = piVar12;
          piVar12 = piVar12 + 1;
          *piVar1 = iVar5;
          uVar8 = uVar8 - 1;
        } while (uVar8 != 0);
      }
      else {
        do {
          iVar5 = *piVar10;
          if (iVar5 == 0) {
            iVar5 = piVar10[1];
            if (iVar5 != 0) goto LAB_000e_9a29;
            iVar5 = piVar10[2];
            if (iVar5 != 0) goto LAB_000e_9a29;
            cVar3 = (char)((uint)piVar10[3] >> 8);
          }
          else {
LAB_000e_9a29:
            cVar3 = (char)((uint)iVar5 >> 8);
          }
          piVar10 = piVar10 + 4;
          piVar1 = piVar12;
          piVar12 = (int *)((int)piVar12 + 1);
          *(char *)piVar1 = cVar3;
          uVar8 = uVar8 - 1;
        } while (uVar8 != 0);
      }
    }
  }
  param_2 = uVar7;
  iVar5 = *(int *)0x1c02;
  if (param_4 == 0) {
    do {
      bVar4 = *(byte *)(iVar5 + (int)piVar9) & 3;
      if (bVar4 != 3) {
        bVar4 = 1;
      }
      if ((*(char *)(iVar5 + (int)piVar11) != '\b') &&
         (uVar7 = CONCAT11(*(char *)(iVar5 + (int)piVar11),bVar4) & 0x3ff,
         (char)(uVar7 >> 8) != '\x02')) {
        uVar7 = CONCAT11(*(char *)(iVar5 + (int)piVar11),(char)uVar7) & 0xfcff;
        *(byte *)(iVar5 + (int)piVar11) = (byte)(uVar7 >> 8) | (byte)uVar7;
      }
      piVar9 = piVar9 + 2;
      piVar11 = (int *)((int)piVar11 + 1);
      uVar7 = param_2 - 1;
      bVar2 = 0 < (int)param_2;
      param_2 = uVar7;
    } while (uVar7 != 0 && bVar2);
  }
  else if (param_5 == 0x1e) {
    do {
      bVar4 = (byte)((uint)*(undefined2 *)(char *)(iVar5 + (int)piVar9) >> 8) & 3;
      if (bVar4 != 3) {
        bVar4 = 1;
      }
      bVar6 = (byte)((uint)*(undefined2 *)(char *)(iVar5 + (int)piVar11) >> 8);
      if ((bVar6 != 8) && ((bVar6 & 3) != 2)) {
        bVar4 = (byte)((uint)*(undefined2 *)(char *)(iVar5 + (int)piVar11) >> 8) & 0xfc | bVar4;
        *(uint *)(char *)(iVar5 + (int)piVar11) = CONCAT11(bVar4,bVar4);
      }
      piVar9 = piVar9 + 4;
      piVar11 = piVar11 + 1;
      uVar7 = param_2 - 1;
      bVar2 = 0 < (int)param_2;
      param_2 = uVar7;
    } while (uVar7 != 0 && bVar2);
  }
  else {
    do {
      bVar4 = (byte)((uint)*(undefined2 *)(char *)(iVar5 + (int)piVar9) >> 8) & 3;
      if (bVar4 != 3) {
        bVar4 = 1;
      }
      if ((*(byte *)(iVar5 + (int)piVar11) != 8) && ((*(byte *)(iVar5 + (int)piVar11) & 3) != 2)) {
        *(byte *)(iVar5 + (int)piVar11) = *(byte *)(iVar5 + (int)piVar11) & 0xfc | bVar4;
      }
      piVar9 = piVar9 + 4;
      piVar11 = (int *)((int)piVar11 + 1);
      uVar7 = param_2 - 1;
      bVar2 = 0 < (int)param_2;
      param_2 = uVar7;
    } while (uVar7 != 0 && bVar2);
  }
  return;
}



void __stdcall16far FUN_000e_b25d(int param_1)

{
  int iVar1;
  undefined2 unaff_DS;
  
  if (param_1 == 1) {
    *(undefined2 *)0x4c6 = 0;
    *(undefined1 *)0x46a = 1;
    *(undefined2 *)0x4ce = 0;
    *(undefined2 *)0x4d0 = 0;
  }
  else if (param_1 == 3) {
    func_0x000f36ac();
    if (*(int *)0x52a != 0) {
      iVar1 = *(int *)(*(int *)0x4c8 * 10 + 0x1c16) - *(int *)0x52a / 6;
      func_0x000f32ec(0xf313,*(undefined2 *)0x4c8,iVar1,iVar1);
    }
  }
  else if (param_1 == 4) {
    func_0x000f36e7();
  }
  return;
}



void __stdcall16far FUN_000e_b2ec(uint param_1)

{
  uint *puVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  uVar3 = 0xff;
  uVar2 = func_0x000eecb2();
  *(undefined2 *)(param_1 * 10 + 0x1c16) = uVar2;
  func_0x000f6f4a(0xeea5,param_1,uVar3);
  if ((param_1 & 1) == 0) {
    puVar1 = (uint *)(((int)param_1 >> 1) * 2 + 0x550);
    *puVar1 = *puVar1 | 4;
  }
  else {
    puVar1 = (uint *)(((int)param_1 >> 1) * 2 + 0x550);
    *puVar1 = *puVar1 | 8;
  }
  return;
}



void __stdcall16far FUN_000e_b356(int param_1)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (param_1 == 1) {
    *(undefined2 *)0x4c6 = 0;
    *(undefined1 *)0x46a = 1;
    *(undefined2 *)0x4ce = 0;
    *(undefined2 *)0x4d0 = 0;
  }
  else if (param_1 == 3) {
    func_0x000f36ac();
    if ((*(int *)0x52a != 0) && ((*(byte *)((*(int *)0x4c8 >> 1) * 0x10 + 0x18c) & 0x20) != 0)) {
      *(int *)0x4c6 = *(int *)0x4c6 + *(int *)0x52a;
      iVar1 = (uint)*(byte *)(*(int *)0x4c8 * 10 + 0x1c19) -
              (uint)*(byte *)(*(int *)0x4c8 * 10 + 0x1c18);
      if (*(int *)0x4c6 < 0x191) {
        if (*(int *)0x4c6 < -400) {
          *(undefined2 *)0x4c6 = 0;
          *(undefined1 *)0x46a = 1;
          iVar1 = iVar1 + 1;
          if (3 < iVar1) {
            iVar1 = 3;
          }
        }
      }
      else {
        *(undefined2 *)0x4c6 = 0;
        *(undefined1 *)0x46a = 1;
        iVar1 = iVar1 + -1;
        if (iVar1 < -3) {
          iVar1 = -3;
        }
      }
      iVar2 = (uint)*(byte *)(*(int *)0x4c8 * 10 + 0x1c18) + iVar1;
      func_0x000f3495(0xf313,*(undefined2 *)0x4c8,iVar2,iVar1,iVar2);
    }
  }
  else if (param_1 == 4) {
    if ((*(byte *)((*(int *)0x4c8 >> 1) * 0x10 + 0x18c) & 0x20) == 0) {
      unaff_CS = 0xf313;
      func_0x000f405d();
    }
    func_0x000f36e7(unaff_CS,*(undefined2 *)0x4c8,1,1,1,0);
  }
  return;
}



undefined2 __stdcall16far FUN_000e_b495(uint param_1,int param_2)

{
  uint *puVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 local_c;
  
  local_c = 0;
  iVar2 = param_2 - (uint)*(byte *)(param_1 * 10 + 0x1c18);
  if (iVar2 < 4) {
    if (iVar2 < -3) {
      param_2 = *(byte *)(param_1 * 10 + 0x1c18) - 3;
      local_c = 1;
    }
  }
  else {
    param_2 = *(byte *)(param_1 * 10 + 0x1c18) + 3;
    local_c = 1;
  }
  if (param_2 < 0) {
    param_2 = 0;
    local_c = 1;
  }
  *(undefined1 *)(param_1 * 10 + 0x1c19) = (undefined1)param_2;
  func_0x000f6f4a();
  if ((param_1 & 1) == 0) {
    puVar1 = (uint *)(((int)param_1 >> 1) * 2 + 0x550);
    *puVar1 = *puVar1 | 1;
  }
  else {
    puVar1 = (uint *)(((int)param_1 >> 1) * 2 + 0x550);
    *puVar1 = *puVar1 | 2;
  }
  return local_c;
}



void __stdcall16far FUN_000e_b54f(int param_1)

{
  byte *pbVar1;
  uint *puVar2;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (param_1 == 1) {
    *(undefined2 *)0x4c6 = 0;
    *(undefined1 *)0x46a = 1;
    *(undefined2 *)0x4ce = 0;
    *(undefined2 *)0x4d0 = 0;
  }
  else if (param_1 == 3) {
    func_0x000f36ac();
    if ((*(int *)0x52a != 0) && ((*(byte *)((*(int *)0x4c8 >> 1) * 0x10 + 0x18c) & 0x20) != 0)) {
      *(int *)0x4c6 = *(int *)0x4c6 + *(int *)0x52a;
      if (*(int *)0x4c6 < 0x191) {
        if (*(int *)0x4c6 < -400) {
          *(undefined2 *)0x4c6 = 0;
          *(undefined1 *)0x46a = 1;
          pbVar1 = (byte *)((*(int *)0x4c8 >> 1) + 0x564);
          *pbVar1 = *pbVar1 & 0xfe;
          puVar2 = (uint *)((*(int *)0x4c8 >> 1) * 2 + 0x550);
          *puVar2 = *puVar2 | 0x10;
        }
      }
      else {
        *(undefined2 *)0x4c6 = 0;
        *(undefined1 *)0x46a = 1;
        pbVar1 = (byte *)((*(int *)0x4c8 >> 1) + 0x564);
        *pbVar1 = *pbVar1 | 1;
        puVar2 = (uint *)((*(int *)0x4c8 >> 1) * 2 + 0x550);
        *puVar2 = *puVar2 | 0x10;
      }
    }
  }
  else if (param_1 == 4) {
    if ((*(byte *)((*(int *)0x4c8 >> 1) * 0x10 + 0x18c) & 0x20) == 0) {
      unaff_CS = 0xf313;
      func_0x000f405d();
    }
    func_0x000f36e7(unaff_CS,*(undefined2 *)0x4c8,0,0,1,1);
  }
  return;
}



void __stdcall16far FUN_000e_b660(int param_1,int param_2)

{
  uint *puVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  
  uVar3 = 0x41;
  uVar2 = func_0x0009470e();
  if (param_2 == 10) {
    *(byte *)(param_1 + 0x564) = *(byte *)(param_1 + 0x564) | 1;
    puVar1 = (uint *)(param_1 * 2 + 0x55a);
    *puVar1 = *puVar1 | 0x10;
  }
  else {
    *(byte *)(param_1 + 0x564) = *(byte *)(param_1 + 0x564) & 0xfe;
    puVar1 = (uint *)(param_1 * 2 + 0x55a);
    *puVar1 = *puVar1 | 0x10;
  }
  func_0x0009470e(38000,uVar2,uVar3);
  return;
}



void __cdecl16far FUN_000e_b6ac(void)

{
  undefined2 unaff_DS;
  
  if (((*(byte *)0x4e8 & 0x80) != 0) && ((*(byte *)0x4e7 & 0x80) != 0)) {
    if ((*(uint *)0x4c8 & 1) == 1) {
      *(uint *)0x4c8 = *(uint *)0x4c8 & 0xfffe;
    }
    else {
      *(uint *)0x4c8 = *(uint *)0x4c8 | 1;
    }
    *(undefined1 *)0x46a = 1;
  }
  return;
}



void __stdcall16far FUN_000e_b6e7(uint param_1,int param_2,int param_3,int param_4,int param_5)

{
  byte bVar1;
  undefined4 uVar2;
  undefined1 *puVar3;
  char *pcVar4;
  int iVar5;
  byte bVar6;
  int iVar7;
  undefined2 unaff_CS;
  undefined2 uVar8;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined2 uVar9;
  undefined4 local_42;
  char local_3c [20];
  undefined1 local_28;
  undefined1 local_27;
  undefined4 local_14;
  char *local_10;
  undefined2 uVar10;
  
  iVar7 = (int)param_1 >> 1;
  if (*(char *)0x46a != '\0') {
    if (*(int *)0x4ce == 0 && *(int *)0x4d0 == 0) {
      uVar2 = *(undefined4 *)0x45e;
      *(undefined2 *)0x4ce = (int)uVar2;
      *(undefined2 *)0x4d0 = (int)((ulong)uVar2 >> 0x10);
      uVar2 = *(undefined4 *)0x472;
      *(undefined2 *)0x4da = (int)uVar2;
      *(undefined2 *)0x4dc = (int)((ulong)uVar2 >> 0x10);
    }
    else {
      uVar2 = *(undefined4 *)0x4ce;
      *(undefined2 *)0x45e = (int)uVar2;
      *(undefined2 *)0x460 = (int)((ulong)uVar2 >> 0x10);
      uVar2 = *(undefined4 *)0x4da;
      *(undefined2 *)0x472 = (int)uVar2;
      *(undefined2 *)0x474 = (int)((ulong)uVar2 >> 0x10);
    }
    uVar9 = 0x4b;
    func_0x000f43cc();
    *(undefined2 *)0x46e = 0;
    local_14 = *(undefined4 *)0x45e;
    _local_10 = (char *)*(undefined4 *)0x1cec;
    uVar8 = 0xf408;
    while (pcVar4 = _local_10, *_local_10 != '\0') {
      uVar10 = (undefined2)((ulong)_local_10 >> 0x10);
      _local_10 = (char *)CONCAT22(uVar10,local_10 + 1);
      iVar5 = func_0x000f6510(uVar8,*pcVar4,uVar9);
      *(int *)0x46e = *(int *)0x46e + iVar5;
      uVar8 = 0xf651;
    }
    func_0x000f5d89(uVar8,iVar7,&local_28);
    *(int *)0x46e = *(int *)0x46e + 7;
    iVar5 = func_0x000f6510(0xf59b,local_28);
    *(int *)0x46e = *(int *)0x46e + iVar5;
    *(int *)0x46e = *(int *)0x46e + 7;
    iVar5 = func_0x000f6510(0xf651,0x2d);
    *(int *)0x46e = *(int *)0x46e + iVar5;
    *(int *)0x46e = *(int *)0x46e + 7;
    if ((*(byte *)(iVar7 * 0x10 + 0x18c) & 1) == 0) {
      iVar5 = func_0x000f6510(0xf651,0x31);
      *(int *)0x46e = *(int *)0x46e + iVar5;
    }
    else {
      iVar5 = func_0x000f6510(0xf651,0x34);
      *(int *)0x46e = *(int *)0x46e + iVar5;
    }
    iVar5 = func_0x000f6510(0xf651,0x4b);
    *(int *)0x46e = *(int *)0x46e + iVar5;
    if (param_3 != 0) {
      *(int *)0x46e = *(int *)0x46e + 0xf;
      iVar5 = func_0x000f6510(0xf651,0x43);
      *(int *)0x46e = *(int *)0x46e + iVar5;
      iVar5 = func_0x000f6510(0xf651,0x48);
      *(int *)0x46e = *(int *)0x46e + iVar5;
      *(int *)0x46e = *(int *)0x46e + 7;
      if ((param_1 & 1) == 0) {
        iVar5 = func_0x000f6510(0xf651,0x31);
        *(int *)0x46e = *(int *)0x46e + iVar5;
      }
      else {
        iVar5 = func_0x000f6510(0xf651,0x32);
        *(int *)0x46e = *(int *)0x46e + iVar5;
      }
    }
    uVar8 = 0xf651;
    if (param_2 != 0) {
      func_0x000eceda(0xf651,local_3c);
      *(int *)0x46e = *(int *)0x46e + 0xf;
      _local_10 = (char *)CONCAT22(unaff_SS,local_3c);
      uVar8 = 0xead0;
      while (pcVar4 = _local_10, *_local_10 != '\0') {
        uVar10 = (undefined2)((ulong)_local_10 >> 0x10);
        _local_10 = (char *)CONCAT22(uVar10,local_10 + 1);
        iVar5 = func_0x000f6510(uVar8,*pcVar4);
        *(int *)0x46e = *(int *)0x46e + iVar5;
        uVar8 = 0xf651;
      }
    }
    if (param_5 != 0) {
      *(int *)0x46e = *(int *)0x46e + 0xf;
      iVar5 = func_0x000f6510(uVar8,0x31);
      *(int *)0x46e = *(int *)0x46e + iVar5;
      if ((*(byte *)(iVar7 + 0x564) & 1) != 0) {
        iVar5 = func_0x000f6510(0xf651,0x30);
        *(int *)0x46e = *(int *)0x46e + iVar5;
      }
      uVar8 = 0xf651;
      iVar5 = func_0x000f6510(0xf651,0x58);
      *(int *)0x46e = *(int *)0x46e + iVar5;
    }
    puVar3 = (undefined1 *)*(undefined4 *)0x45e;
    *(int *)0x45e = *(int *)0x45e + 1;
    *puVar3 = 0;
    puVar3 = (undefined1 *)*(undefined4 *)0x45e;
    *(int *)0x45e = *(int *)0x45e + 1;
    *puVar3 = 0;
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -1) = 10;
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -2) = 10;
    unaff_CS = 0xf408;
    func_0x000f44c8(uVar8,200,0x307,(int)local_14,(int)((ulong)local_14 >> 0x10));
    *(int *)0x472 = *(int *)0x472 + 9;
  }
  bVar1 = *(byte *)(iVar7 * 0x10 + 399);
  bVar6 = bVar1 & 0xf;
  if ((bVar1 & 8) == 8) {
    func_0x000fb5b7(unaff_CS,param_1 & 0xfffe,(int)*(undefined4 *)0x1ddc + 0x2a3,
                    (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),bVar6);
  }
  else {
    func_0x000fb5b7(unaff_CS,param_1,(int)*(undefined4 *)0x1ddc + 0x2a3,
                    (int)((ulong)*(undefined4 *)0x1ddc >> 0x10),bVar6);
  }
  if (((*(byte *)(param_1 + 0x1be2) & 2) == 0) &&
     (((*(byte *)((*(int *)0x4c8 >> 1) * 0x10 + 0x18c) & 0x20) != 0 ||
      ((*(byte *)(iVar7 * 2 + 0x1be2) & 2) == 0)))) {
    if (*(char *)0x46a != '\0') {
      local_28 = 0x20;
      local_27 = 0;
      func_0x000f3ea3(0xfb59,100,0x271,0x8a,0xff7b,0x9d,0xff7b);
    }
  }
  else if (param_4 == 0) {
    local_42 = (byte *)CONCAT22((int)((ulong)*(undefined4 *)0x1ddc >> 0x10),
                                (byte *)((int)*(undefined4 *)0x1ddc + 0x2a3));
    *local_42 = *local_42 & 0xfe;
    *(undefined1 *)*(undefined4 *)0x472 = 0;
  }
  else {
    local_42 = (byte *)CONCAT22((int)((ulong)*(undefined4 *)0x1ddc >> 0x10),
                                (byte *)((int)*(undefined4 *)0x1ddc + 0x2a3));
    *local_42 = *local_42 | 1;
    *(undefined1 *)*(undefined4 *)0x472 = 0;
  }
  if (*(char *)0x46a != '\0') {
    *(undefined1 *)0x46a = 0;
  }
  return;
}



void __stdcall16far FUN_000e_ba64(int param_1)

{
  byte bVar1;
  int iVar2;
  byte bVar3;
  byte bVar4;
  byte bVar5;
  int iVar6;
  undefined2 uVar7;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined2 local_42;
  undefined1 local_3d;
  undefined1 local_3c;
  undefined1 local_3b;
  undefined1 *local_34;
  undefined1 local_30;
  undefined1 local_2f;
  undefined1 local_2e [38];
  undefined1 local_8 [6];
  
  if (param_1 == 1) {
    *(undefined2 *)0x4c6 = 0;
    if (*(int *)0x4c8 < 2) {
      *(undefined2 *)0x4c8 = 2;
    }
    if (7 < *(int *)0x4c8) {
      *(undefined2 *)0x4c8 = 7;
    }
    *(undefined2 *)0x4ce = 0;
    *(undefined2 *)0x4d0 = 0;
    *(undefined1 *)0x468 = 1;
    *(undefined1 *)0x46a = 1;
  }
  else if (param_1 == 3) {
    func_0x000f3132();
  }
  else if ((param_1 == 4) && (*(char *)0x468 != '\0')) {
    iVar2 = *(int *)0x4c8 / 2;
    bVar1 = *(byte *)(iVar2 * 0x10 + 399);
    bVar3 = bVar1 & 0xf;
    bVar5 = *(byte *)(*(int *)0x4c8 * 10 + 0x1c1a);
    bVar4 = bVar5 & 0xe;
    if (bVar4 == 2) {
      local_3b = 7;
      local_3d = 0x56;
    }
    else if (bVar4 == 8) {
      local_3b = 0x20;
      local_3d = 0x16;
    }
    else {
      local_3b = 0x20;
      local_3d = 0x56;
    }
    if ((bVar5 & 1) == 1) {
      local_3c = 0x20;
    }
    else {
      local_3c = 0x3e;
    }
    func_0x000f36e7();
    local_30 = 0x20;
    local_2f = 0;
    if (((*(byte *)(*(int *)0x4c8 + 0x1be2) & 2) != 0) ||
       ((bVar3 == 8 && ((*(byte *)((*(uint *)0x4c8 & 0xfffe) + 0x1be2) & 2) != 0)))) {
      func_0x000f3ea3(0xf313,0x113,0x2a3,0x9e,0xff7b,&local_30);
      _local_34 = (undefined1 *)CONCAT22(unaff_SS,&local_30);
      while( true ) {
        uVar7 = (undefined2)((ulong)_local_34 >> 0x10);
        if (local_8 <= (undefined1 *)_local_34) break;
        *_local_34 = 0x20;
        _local_34 = (undefined1 *)CONCAT22(uVar7,(undefined1 *)_local_34 + 1);
      }
      local_30 = local_3c;
      local_2f = local_3b;
      iVar6 = *(int *)0x4c8 * 10;
      func_0x000eceda(0xf313,local_2e);
      *(undefined1 *)(iVar6 + -1) = local_3d;
      func_0x000f3ea3(0xead0,0,0x271,0xa3,0xff7b,&local_30);
      _local_34 = (undefined1 *)CONCAT22(unaff_SS,&local_30);
      while( true ) {
        uVar7 = (undefined2)((ulong)_local_34 >> 0x10);
        if (local_8 <= (undefined1 *)_local_34) break;
        *_local_34 = 0x20;
        _local_34 = (undefined1 *)CONCAT22(uVar7,(undefined1 *)_local_34 + 1);
      }
      local_30 = local_3c;
      iVar6 = *(int *)0x4c8 * 10;
      func_0x000eceda(0xf313,local_2e);
      *(undefined1 *)(iVar6 + -1) = local_3d;
      func_0x000f3ea3(0xead0,0,0x23f,0xad,0xff7b,&local_30);
      func_0x000eceda(0xf313,&local_30);
      func_0x000f3ea3(0xead0,0,0x20d,0xb8,0xff7b,&local_30);
      if ((bVar1 & 8) == 8) {
        local_42 = 0xc2;
      }
      else if ((bVar1 & 0xf) == 0) {
        local_42 = 0xc6;
      }
      else if (bVar3 == 1) {
        local_42 = 0xcb;
      }
      else if (bVar3 == 2) {
        local_42 = 0xd0;
      }
      else if (bVar3 == 3) {
        local_42 = 0xd5;
      }
      else if (bVar3 == 7) {
        local_42 = 0xda;
      }
      else {
        local_42 = 0xe0;
      }
      func_0x000f3ea3(0xf313,0,0x1a9,0xe4,0xff7b,local_42,0xff7b);
      bVar1 = *(byte *)(iVar2 * 0x10 + 0x18d);
      bVar5 = bVar1 & 0xf;
      if ((bVar1 & 0xf) == 0) {
        local_42 = 0xea;
      }
      else if (bVar5 == 1) {
        local_42 = 0xf2;
      }
      else if (bVar5 == 2) {
        local_42 = 0xfb;
      }
      else if (bVar5 == 3) {
        local_42 = 0x104;
      }
      else {
        local_42 = 0x10d;
      }
      func_0x000f3ea3(0xf313,0,0x145,0x110,0xff7b,local_42,0xff7b);
      if ((*(byte *)(iVar2 * 0x10 + 0x18d) & 0xf0) == 0) {
        local_42 = 0x119;
      }
      else {
        local_42 = 0x115;
      }
      func_0x000f3ea3(0xf313,0,0xe1,0x11e,0xff7b,local_42,0xff7b);
      func_0x000f43cc(0xf313,0x4b);
    }
  }
  return;
}



void __stdcall16far FUN_000e_bea3(undefined2 param_1,undefined2 param_2,char *param_3,char *param_4)

{
  undefined4 uVar1;
  char *pcVar2;
  int iVar3;
  undefined2 unaff_DS;
  undefined2 uVar4;
  
  *(undefined2 *)0x46e = 0;
  uVar1 = *(undefined4 *)0x45e;
  uVar4 = 0;
  iVar3 = func_0x000f6510();
  *(int *)0x46e = *(int *)0x46e + iVar3;
  while (pcVar2 = param_3, *param_3 != '\0') {
    param_3 = (char *)CONCAT22(param_3._2_2_,(char *)param_3 + 1);
    iVar3 = func_0x000f6510(0xf651,*pcVar2,uVar4);
    *(int *)0x46e = *(int *)0x46e + iVar3;
  }
  iVar3 = func_0x000f6510(0xf651,0x20,uVar4);
  *(int *)0x46e = *(int *)0x46e + iVar3;
  while (pcVar2 = param_4, *param_4 != '\0') {
    param_4 = (char *)CONCAT22(param_4._2_2_,(char *)param_4 + 1);
    iVar3 = func_0x000f6510(0xf651,*pcVar2);
    *(int *)0x46e = *(int *)0x46e + iVar3;
  }
  *(undefined1 *)((int)*(undefined4 *)0x45e + *(int *)0x1c02 + -1) = 10;
  *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -2) = 10;
  func_0x000f44c8(0xf651,param_1,param_2,(int)uVar1,(int)((ulong)uVar1 >> 0x10));
  *(int *)0x472 = *(int *)0x472 + 9;
  return;
}



void __stdcall16far FUN_000e_bf4c(int param_1)

{
  undefined4 uVar1;
  undefined2 unaff_DS;
  bool bVar2;
  undefined2 uVar3;
  
  bVar2 = param_1 < 0;
  if (bVar2) {
    param_1 = -param_1;
  }
  if (param_1 == 1) {
    *(undefined2 *)0x4ce = 0;
    *(undefined2 *)0x4d0 = 0;
  }
  else if (param_1 == 3) {
    *(undefined1 *)0x476 = 0x20;
    *(undefined1 *)0x477 = 0;
    if ((*(char *)0x1bf9 == '\0') || (bVar2)) {
      *(undefined1 *)0x46a = 1;
    }
  }
  else if (((param_1 == 4) && (*(char *)0x46a == '\x01')) && ((*(char *)0x1bf9 == '\0' || (bVar2))))
  {
    if (*(int *)0x4ce == 0 && *(int *)0x4d0 == 0) {
      uVar1 = *(undefined4 *)0x45e;
      *(undefined2 *)0x4ce = (int)uVar1;
      *(undefined2 *)0x4d0 = (int)((ulong)uVar1 >> 0x10);
      uVar1 = *(undefined4 *)0x472;
      *(undefined2 *)0x4da = (int)uVar1;
      *(undefined2 *)0x4dc = (int)((ulong)uVar1 >> 0x10);
    }
    else {
      uVar1 = *(undefined4 *)0x4ce;
      *(undefined2 *)0x45e = (int)uVar1;
      *(undefined2 *)0x460 = (int)((ulong)uVar1 >> 0x10);
      uVar1 = *(undefined4 *)0x4da;
      *(undefined2 *)0x472 = (int)uVar1;
      *(undefined2 *)0x474 = (int)((ulong)uVar1 >> 0x10);
    }
    uVar3 = 0xff7b;
    func_0x000f3ea3();
    func_0x000f3ea3(0xf313,10,0xaf,0x148,0xff7b,0x166,0xff7b,uVar3);
    *(undefined1 *)0x46a = 0;
  }
  return;
}



void __cdecl16far FUN_000e_c05d(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x46a != '\0') {
    func_0x000f3ea3();
  }
  return;
}



void __cdecl16far FUN_000e_c08e(void)

{
  undefined1 *puVar1;
  undefined4 uVar2;
  undefined1 *puVar3;
  undefined2 unaff_DS;
  undefined1 *local_e;
  int local_a;
  
  puVar1 = (undefined1 *)*(undefined2 *)0x1c02;
  local_e = (undefined1 *)ZEXT24(puVar1 + 1);
  puVar3 = local_e;
  *puVar1 = 0;
  local_e = (undefined1 *)ZEXT24(puVar1 + 2);
  *puVar3 = 2;
  *local_e = 2;
  uVar2 = *(undefined4 *)0x1ddc;
  *(int *)0x472 = (int)uVar2 + 0x201;
  *(undefined2 *)0x474 = (int)((ulong)uVar2 >> 0x10);
  for (local_a = 0; local_a < 0x14; local_a = local_a + 1) {
    puVar3 = (undefined1 *)*(undefined4 *)0x472;
    *(int *)0x472 = *(int *)0x472 + 9;
    *puVar3 = 8;
  }
  *(undefined1 *)((int)*(undefined4 *)0x1ddc + 0x2ba) = 0;
  *(undefined1 *)((int)*(undefined4 *)0x1ddc + 699) = 0;
  *(undefined1 *)((int)*(undefined4 *)0x1ddc + 700) = 0;
  *(undefined1 *)((int)*(undefined4 *)0x1ddc + 0x2bd) = 0;
  *(undefined1 *)((int)*(undefined4 *)0x1ddc + 0x2b5) = 0x81;
  *(undefined2 *)0x46e = 0;
  *(undefined2 *)0x470 = 0x339;
  uVar2 = *(undefined4 *)0x1ddc;
  *(int *)0x472 = (int)uVar2 + 0x201;
  *(undefined2 *)0x474 = (int)((ulong)uVar2 >> 0x10);
  uVar2 = *(undefined4 *)0x1cc4;
  *(undefined2 *)0x45e = (int)uVar2;
  *(undefined2 *)0x460 = (int)((ulong)uVar2 >> 0x10);
  return;
}



void __stdcall16far FUN_000e_c150(int param_1,int param_2,uint param_3)

{
  undefined4 uVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined2 unaff_DS;
  undefined2 uVar4;
  int local_18;
  char *local_16;
  int local_e;
  
  uVar1 = *(undefined4 *)0x45e;
  uVar4 = 0;
  iVar3 = func_0x000f6510();
  *(int *)0x46e = *(int *)0x46e + iVar3;
  if (((param_3 & 1) == 0) ||
     ((*(byte *)((int)*(undefined4 *)0x1d10 + param_1 * 0x10 + 0xe) & 2) == 0)) {
    if ((param_3 & 4) != 0) {
      if ((*(byte *)((int)*(undefined4 *)0x1d10 + param_1 * 0x10 + 0xe) & 2) == 0) {
        iVar3 = func_0x000f6510(0xf651,0x2d,uVar4);
        *(int *)0x46e = *(int *)0x46e + iVar3;
        iVar3 = func_0x000f6510(0xf651,0x3e);
        *(int *)0x46e = *(int *)0x46e + iVar3;
      }
      else {
        iVar3 = func_0x000f6510(0xf651,0x2d,uVar4);
        *(int *)0x46e = *(int *)0x46e + iVar3;
        iVar3 = func_0x000f6510(0xf651,0x2a);
        *(int *)0x46e = *(int *)0x46e + iVar3;
      }
    }
  }
  else {
    iVar3 = func_0x000f6510(0xf651,0x2a,uVar4);
    *(int *)0x46e = *(int *)0x46e + iVar3;
  }
  *(undefined2 *)0x4de = 0;
  local_16 = (char *)*(undefined4 *)((int)*(undefined4 *)0x1d10 + param_1 * 0x10);
  if ((param_3 & 8) == 0) {
    while (*local_16 != '\0') {
      iVar3 = func_0x000f6510(0xf651,*local_16,uVar4);
      *(int *)0x46e = *(int *)0x46e + iVar3;
      local_16 = (char *)CONCAT22(local_16._2_2_,(char *)local_16 + 1);
    }
  }
  else {
    while (*local_16 != '\0') {
      iVar3 = func_0x000f660c(0xf651,*local_16);
      *(int *)0x46e = *(int *)0x46e + iVar3;
      local_16 = (char *)CONCAT22(local_16._2_2_,(char *)local_16 + 1);
    }
  }
  uVar4 = 0xf651;
  if ((param_3 & 2) != 0) {
    if (((*(byte *)((int)*(undefined4 *)0x1d10 + param_1 * 0x10 + 0xe) & 1) != 0) ||
       (((*(byte *)((int)*(undefined4 *)0x1d10 + param_1 * 0x10 + 0xe) & 0x40) != 0 &&
        (*(char *)0x469 == '\0')))) {
      uVar4 = 0xf408;
      func_0x000f47ab(0xf651,param_2,0);
    }
    *(undefined1 *)*(undefined4 *)0x45e = 5;
    *(undefined1 *)((int)*(undefined4 *)0x45e + *(int *)0x1c02) = 8;
    *(int *)0x45e = *(int *)0x45e + 1;
    *(undefined1 *)*(undefined4 *)0x45e = 0;
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e) = 8;
    *(int *)0x45e = *(int *)0x45e + 1;
    local_18 = 3;
    do {
      *(undefined1 *)*(undefined4 *)0x45e = 5;
      *(undefined1 *)((int)*(undefined4 *)0x45e + *(int *)0x1c02 + 2) = 2;
      *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e) = 0xb;
      *(int *)0x45e = *(int *)0x45e + 1;
      *(undefined1 *)*(undefined4 *)0x45e = (undefined1)local_18;
      *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + 2) = 2;
      *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e) = 0xb;
      *(int *)0x45e = *(int *)0x45e + 1;
      local_18 = local_18 + 2;
    } while (local_18 < *(int *)0x46e);
    puVar2 = (undefined1 *)*(undefined4 *)0x45e;
    *(int *)0x45e = *(int *)0x45e + 1;
    *puVar2 = 0;
    puVar2 = (undefined1 *)*(undefined4 *)0x45e;
    *(int *)0x45e = *(int *)0x45e + 1;
    *puVar2 = 0;
  }
  *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -1) = 10;
  *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -2) = 10;
  local_e = param_2 * 100;
  if (*(int *)0x462 < param_2) {
    local_e = local_e + 100;
  }
  func_0x000f44c8(uVar4,local_e,*(undefined2 *)0x470,(int)uVar1,(int)((ulong)uVar1 >> 0x10));
  *(undefined2 *)0x46e = 0;
  *(int *)0x470 = *(int *)0x470 + -0x32;
  *(int *)0x472 = *(int *)0x472 + 9;
  return;
}



void __stdcall16far FUN_000e_c3cc(undefined2 param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  undefined2 unaff_DS;
  undefined2 uVar7;
  char *local_c;
  
  uVar3 = *(undefined4 *)0x45e;
  uVar4 = *(undefined4 *)0x472;
  uVar1 = *(undefined2 *)0x46e;
  uVar2 = *(undefined2 *)0x470;
  *(undefined2 *)0x46e = 8;
  *(undefined2 *)0x470 = param_1;
  uVar5 = *(undefined4 *)0x1cc4;
  *(int *)0x45e = (int)uVar5 + 0xe74;
  *(undefined2 *)0x460 = (int)((ulong)uVar5 >> 0x10);
  local_c = (char *)*(undefined4 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 10);
  *(undefined2 *)0x4de = 0;
  uVar7 = 0;
  iVar6 = func_0x000f6510();
  *(int *)0x46e = *(int *)0x46e + iVar6;
  while (*local_c != '\0') {
    iVar6 = func_0x000f660c(0xf651,*local_c,uVar7);
    *(int *)0x46e = *(int *)0x46e + iVar6;
    local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
  }
  *(undefined1 *)((int)*(undefined4 *)0x45e + *(int *)0x1c02 + -1) = 10;
  *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -2) = 10;
  uVar5 = *(undefined4 *)0x1ddc;
  *(int *)0x472 = (int)uVar5 + 0x288;
  *(undefined2 *)0x474 = (int)((ulong)uVar5 >> 0x10);
  func_0x000f44c8(0xf651,0,*(undefined2 *)0x470,(int)*(undefined4 *)0x1cc4 + 0xe74,
                  (int)((ulong)*(undefined4 *)0x1cc4 >> 0x10));
  *(undefined2 *)0x45e = (int)uVar3;
  *(undefined2 *)0x460 = (int)((ulong)uVar3 >> 0x10);
  *(undefined2 *)0x472 = (int)uVar4;
  *(undefined2 *)0x474 = (int)((ulong)uVar4 >> 0x10);
  *(undefined2 *)0x46e = uVar1;
  *(undefined2 *)0x470 = uVar2;
  return;
}



void __stdcall16far FUN_000e_c4c8(uint param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  undefined2 unaff_DS;
  undefined1 local_14;
  undefined1 uStack_13;
  
  *(byte *)((int)*(undefined4 *)0x472 + 1) = (byte)((param_2 & 7) << 5) | 0x10;
  *(undefined1 *)((int)*(undefined4 *)0x472 + 2) = (char)((int)param_2 >> 3);
  *(byte *)((int)*(undefined4 *)0x472 + 3) = (byte)((param_1 & 7) << 5) | 0x10;
  *(undefined1 *)((int)*(undefined4 *)0x472 + 4) = (char)((int)param_1 >> 3);
  iVar1 = (int)param_3 + (int)((ulong)param_3 >> 0x10) * 0x10;
  local_14 = (undefined1)iVar1;
  *(undefined1 *)((int)*(undefined4 *)0x472 + 5) = local_14;
  uStack_13 = (undefined1)((uint)iVar1 >> 8);
  *(undefined1 *)((int)*(undefined4 *)0x472 + 6) = uStack_13;
  *(undefined1 *)((int)*(undefined4 *)0x472 + 7) = local_14;
  *(undefined1 *)((int)*(undefined4 *)0x472 + 8) = uStack_13;
  *(undefined1 *)*(undefined4 *)0x472 = 0x11;
  return;
}



void __cdecl16far FUN_000e_c5a4(void)

{
  char cVar1;
  undefined4 uVar2;
  char *pcVar3;
  char *pcVar4;
  byte *pbVar5;
  int iVar6;
  char *pcVar7;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  char acStack_fe [12];
  undefined2 uStack_f2;
  undefined2 uStack_f0;
  undefined2 uStack_ee;
  undefined2 uStack_ec;
  undefined2 uStack_ea;
  uint uStack_e8;
  int local_e2;
  char *local_e0;
  byte *local_dc;
  undefined2 uVar8;
  char local_d8 [200];
  int local_10;
  int local_e;
  uint local_c;
  uint local_a;
  
  _local_dc = (byte *)CONCAT22(unaff_SS,local_d8);
  local_e = 1;
  local_c = 0;
  local_a = (uint)*(byte *)((int)*(undefined4 *)0x1d10 + 4);
  for (; pcVar3 = (char *)_local_dc, local_e <= *(int *)0x462; local_e = local_e + 1) {
    for (local_10 = 0; local_10 < (int)(uint)*(byte *)(local_c + 0x3e3); local_10 = local_10 + 1) {
      local_a = (uint)*(byte *)((int)*(undefined4 *)0x1d10 + local_a * 0x10 + 5);
    }
    uVar8 = (undefined2)((ulong)_local_dc >> 0x10);
    if (local_e < *(int *)0x462) {
      *pcVar3 = '/';
    }
    else {
      *pcVar3 = '<';
    }
    _local_dc = (byte *)CONCAT22(uVar8,local_dc + 1);
    local_e0 = (char *)*(undefined4 *)((int)*(undefined4 *)0x1d10 + local_a * 0x10);
    while (pcVar4 = (char *)_local_dc, pcVar3 = local_e0, *local_e0 != '\0') {
      uVar8 = (undefined2)((ulong)_local_dc >> 0x10);
      _local_dc = (byte *)CONCAT22(uVar8,local_dc + 1);
      local_e0 = (char *)CONCAT22(local_e0._2_2_,(char *)local_e0 + 1);
      *pcVar4 = *pcVar3;
    }
    *_local_dc = '\0';
    if (local_e < *(int *)0x462) {
      local_c = local_a;
      local_a = (uint)*(byte *)(local_a * 0x10 + (int)*(undefined4 *)0x1d10 + 4);
    }
  }
  _local_dc = (byte *)CONCAT22(unaff_SS,local_d8);
  local_e2 = 0;
  while (pcVar3 = (char *)_local_dc, uVar8 = (undefined2)((ulong)_local_dc >> 0x10),
        _local_dc = (byte *)CONCAT22(uVar8,local_dc + 1), *pcVar3 != '\0') {
    local_e2 = local_e2 + 1;
  }
  if (local_e2 < 0x27) {
    pcVar7 = local_d8;
  }
  else {
    pcVar7 = acStack_fe + local_e2;
  }
  _local_dc = (byte *)CONCAT22(unaff_SS,pcVar7);
  uVar2 = *(undefined4 *)0x1cc4;
  *(undefined2 *)0x45e = (int)uVar2;
  *(undefined2 *)0x460 = (int)((ulong)uVar2 >> 0x10);
  uStack_e8 = 0xc7e6;
  iVar6 = func_0x000f6510();
  *(int *)0x46e = *(int *)0x46e + iVar6;
  while (cVar1 = *_local_dc, uVar8 = (undefined2)((ulong)_local_dc >> 0x10), cVar1 != '<') {
    _local_dc = (byte *)CONCAT22(uVar8,local_dc + 1);
    uStack_e8 = 0xf651;
    uStack_ea = 0xc800;
    iVar6 = func_0x000f6510();
    *(int *)0x46e = *(int *)0x46e + iVar6;
  }
  uStack_e8 = 0xf651;
  uStack_ea = 0xc817;
  iVar6 = func_0x000f6510();
  *(int *)0x46e = *(int *)0x46e + iVar6;
  _local_dc = (byte *)CONCAT22(uVar8,local_dc + 1);
  while (pbVar5 = _local_dc, *_local_dc != 0) {
    uVar8 = (undefined2)((ulong)_local_dc >> 0x10);
    _local_dc = (byte *)CONCAT22(uVar8,local_dc + 1);
    uStack_e8 = (uint)*pbVar5;
    uStack_ea = 0xf651;
    uStack_ec = 0xc835;
    iVar6 = func_0x000f660c();
    *(int *)0x46e = *(int *)0x46e + iVar6;
  }
  *(undefined1 *)((int)*(undefined4 *)0x45e + *(int *)0x1c02 + -1) = 10;
  *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -2) = 10;
  uVar2 = *(undefined4 *)0x1ddc;
  *(int *)0x472 = (int)uVar2 + 0x201;
  *(undefined2 *)0x474 = (int)((ulong)uVar2 >> 0x10);
  uStack_e8 = (uint)((ulong)*(undefined4 *)0x1cc4 >> 0x10);
  uStack_ea = (undefined2)*(undefined4 *)0x1cc4;
  uStack_ec = *(undefined2 *)0x470;
  uStack_ee = 0;
  uStack_f0 = 0xf651;
  uStack_f2 = 0xc87f;
  func_0x000f44c8();
  *(int *)0x472 = *(int *)0x472 + 9;
  return;
}



void __stdcall16far FUN_000e_c7ab(int param_1,int param_2)

{
  int iVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  ulong uVar6;
  int iVar7;
  undefined2 unaff_DS;
  undefined2 uVar8;
  char *local_18;
  
  uVar3 = *(undefined4 *)0x45e;
  uVar4 = *(undefined4 *)0x472;
  iVar1 = *(int *)0x46e;
  uVar2 = *(undefined2 *)0x470;
  *(undefined2 *)0x46e = 0;
  uVar5 = *(undefined4 *)0x1cc4;
  *(int *)0x45e = (int)uVar5 + 0x11ee;
  *(undefined2 *)0x460 = (int)((ulong)uVar5 >> 0x10);
  uVar8 = 0x20;
  iVar7 = func_0x000f6510();
  *(int *)0x46e = *(int *)0x46e + iVar7;
  _local_18 = (char *)CONCAT22(unaff_DS,(char *)0x476);
  while (*_local_18 != '\0') {
    iVar7 = func_0x000f6510(0xf651,*_local_18,uVar8);
    *(int *)0x46e = *(int *)0x46e + iVar7;
    uVar6 = (ulong)_local_18 >> 0x10;
    _local_18 = (char *)CONCAT22((int)uVar6,local_18 + 1);
  }
  if (param_2 == 0) {
    uVar5 = *(undefined4 *)0x1ddc;
    *(int *)0x472 = (int)uVar5 + 0x291;
    *(undefined2 *)0x474 = (int)((ulong)uVar5 >> 0x10);
    func_0x000f44c8(0xf651,iVar1 * 4 + param_1 * 100,uVar2,(int)*(undefined4 *)0x1cc4 + 0x11ee,
                    (int)((ulong)*(undefined4 *)0x1cc4 >> 0x10));
  }
  *(undefined2 *)0x45e = (int)uVar3;
  *(undefined2 *)0x460 = (int)((ulong)uVar3 >> 0x10);
  *(undefined2 *)0x472 = (int)uVar4;
  *(undefined2 *)0x474 = (int)((ulong)uVar4 >> 0x10);
  *(int *)0x46e = iVar1;
  *(undefined2 *)0x470 = uVar2;
  return;
}



void __stdcall16far FUN_000e_c907(int param_1)

{
  undefined4 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  if (param_1 == 1) {
    *(undefined2 *)0x4ce = 0;
    *(undefined2 *)0x4d0 = 0;
  }
  else if (param_1 == 2) {
    func_0x000e6dc4();
  }
  else if (param_1 == 3) {
    if ((*(byte *)0x4e8 & *(byte *)0x4e7 & 0x80) != 0) {
      if (*(char *)0x1bf7 == '\0') {
        *(undefined1 *)0x706 = 1;
        func_0x000e92b0();
        *(undefined1 *)0x1bf7 = 1;
      }
      else {
        *(char *)0x706 = *(char *)0x706 + '\x01';
        if (2 < *(byte *)0x706) {
          func_0x000e6dc4();
        }
      }
    }
  }
  else if (param_1 == 4) {
    if (*(char *)0x1bf7 == '\0') {
      if (*(int *)0x4ce == 0 && *(int *)0x4d0 == 0) {
        uVar1 = *(undefined4 *)0x45e;
        *(undefined2 *)0x4ce = (int)uVar1;
        *(undefined2 *)0x4d0 = (int)((ulong)uVar1 >> 0x10);
        uVar1 = *(undefined4 *)0x472;
        *(undefined2 *)0x4da = (int)uVar1;
        *(undefined2 *)0x4dc = (int)((ulong)uVar1 >> 0x10);
      }
      else {
        uVar1 = *(undefined4 *)0x4ce;
        *(undefined2 *)0x45e = (int)uVar1;
        *(undefined2 *)0x460 = (int)((ulong)uVar1 >> 0x10);
        uVar1 = *(undefined4 *)0x4da;
        *(undefined2 *)0x472 = (int)uVar1;
        *(undefined2 *)0x474 = (int)((ulong)uVar1 >> 0x10);
      }
      uVar2 = 0xff7b;
      func_0x000f3ea3();
      func_0x000f3ea3(0xf313,10,0x145,0x32,0xff7b,0x4f,0xff7b,uVar2);
      func_0x000f3ea3(0xf313,10,0x113,0x50,0xff7b,0x6b,0xff7b);
      *(undefined1 *)0x46a = 0;
    }
  }
  else {
    func_0x000e6dc4();
  }
  return;
}



void __stdcall16far FUN_000e_ca46(int param_1)

{
  undefined2 unaff_DS;
  
  if (param_1 != 1) {
    if (param_1 == 2) {
      *(undefined1 *)0x476 = 0;
      return;
    }
    if (param_1 != 3) {
      return;
    }
  }
  if (*(int *)0x52a != 0) {
    *(int *)0x3e0 = *(int *)0x3e0 + *(int *)0x52a;
    if (*(int *)0x3e0 < 0) {
      *(undefined2 *)0x3e0 = 0;
    }
    else if (0x25f < *(int *)0x3e0) {
      *(undefined2 *)0x3e0 = 0x25f;
    }
  }
  func_0x000f5d89();
  return;
}



void __stdcall16far FUN_000e_cad3(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (param_1 == 1) {
    *(undefined2 *)0x4ce = 0;
    *(undefined2 *)0x4d0 = 0;
    if (*(char *)0x400 == '\0') {
      return;
    }
    *(undefined1 *)0x3de = 0;
    return;
  }
  if (param_1 != 4) {
    if (param_1 != 2) {
      return;
    }
    *(undefined1 *)0x476 = 0;
    return;
  }
  if (*(int *)0x4ce == 0 && *(int *)0x4d0 == 0) {
    uVar1 = *(undefined4 *)0x45e;
    *(undefined2 *)0x4ce = (int)uVar1;
    *(undefined2 *)0x4d0 = (int)((ulong)uVar1 >> 0x10);
    uVar1 = *(undefined4 *)0x472;
    *(undefined2 *)0x4da = (int)uVar1;
    *(undefined2 *)0x4dc = (int)((ulong)uVar1 >> 0x10);
  }
  else {
    uVar1 = *(undefined4 *)0x4ce;
    *(undefined2 *)0x45e = (int)uVar1;
    *(undefined2 *)0x460 = (int)((ulong)uVar1 >> 0x10);
    uVar1 = *(undefined4 *)0x4da;
    *(undefined2 *)0x472 = (int)uVar1;
    *(undefined2 *)0x474 = (int)((ulong)uVar1 >> 0x10);
  }
  if (((*(char *)0x400 == '\x01') && (*"" != '\0')) && (*(char *)0x1bf9 != '\0')) {
    iVar2 = func_0x0008392b();
    if (iVar2 == 0) {
      unaff_CS = 0x82c9;
      iVar2 = func_0x0008397e(0x82c9);
      if (iVar2 == 0) goto LAB_000e_cb78;
    }
    *(undefined1 *)0x400 = 0;
    unaff_CS = 0xf5f5;
    func_0x000f635e(0x82c9);
  }
LAB_000e_cb78:
  func_0x000f3ea3(unaff_CS,10,0xaf,0x6c,0xff7b,0x88,0xff7b);
  *(undefined1 *)0x46a = 0;
  return;
}



void __stdcall16far FUN_000e_cbdd(void)

{
  undefined2 unaff_DS;
  
  if ((*(byte *)0x1bf9 & 2) == 0) {
    func_0x000f3f4c();
  }
  else {
    func_0x000860b1();
  }
  return;
}



void __stdcall16far FUN_000e_cc08(void)

{
  undefined2 unaff_DS;
  
  if ((*(byte *)0x1bf9 & 2) == 0) {
    func_0x000f3f4c();
  }
  else {
    func_0x00086205();
  }
  return;
}



void __stdcall16far FUN_000e_cc33(void)

{
  undefined2 unaff_DS;
  
  if ((*(byte *)0x1bf9 & 2) == 0) {
    func_0x000f3f4c();
  }
  else {
    func_0x000863ea();
  }
  return;
}



void __stdcall16far FUN_000e_cc5e(void)

{
  undefined2 unaff_DS;
  
  if (*(char *)0x1bf9 == '\0') {
    func_0x000f3f4c();
  }
  else {
    func_0x00087d9c();
  }
  return;
}



void __stdcall16far FUN_000e_cc87(void)

{
  undefined2 unaff_DS;
  
  if ((*(char *)0x1bf9 == '\0') || (*(char *)0x1bfa != '\0')) {
    func_0x000f3f4c();
  }
  else {
    func_0x00087ec3();
  }
  return;
}



void __stdcall16far FUN_000e_ccb7(void)

{
  undefined2 unaff_DS;
  
  if ((*(char *)0x1bf9 == '\0') || (*(char *)0x1bfa != '\0')) {
    func_0x000f3f4c();
  }
  else {
    func_0x00087f44();
  }
  return;
}



void __cdecl16far FUN_000e_ce5b(void)

{
  undefined2 *puVar1;
  char cVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  
  if (*(int *)0x3d6 < 1) {
    uVar4 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
    iVar3 = (int)*(undefined4 *)0x1d10;
    if (*(int *)(*(int *)0x464 * 0x10 + iVar3 + 6) != 0 ||
        *(int *)(*(int *)0x464 * 0x10 + iVar3 + 8) != 0) {
      puVar1 = (undefined2 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 6);
      (*(code *)*puVar1)();
    }
  }
  else {
    uVar4 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
    iVar3 = (int)*(undefined4 *)0x1d10;
    if (*(int *)(*(int *)0x464 * 0x10 + iVar3 + 6) != 0 ||
        *(int *)(*(int *)0x464 * 0x10 + iVar3 + 8) != 0) {
      puVar1 = (undefined2 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 6);
      (*(code *)*puVar1)();
    }
    if ((int)(*(byte *)(*(int *)0x46c + 0x3e3) - 1) < 0) {
      cVar2 = (char)*(undefined2 *)0x3d6 + -1;
    }
    else {
      cVar2 = (char)((int)(*(byte *)(*(int *)0x46c + 0x3e3) - 1) % *(int *)0x3d6);
    }
    *(char *)(*(int *)0x46c + 0x3e3) = cVar2;
    func_0x000f5184();
  }
  return;
}



void __cdecl16far FUN_000e_cf19(void)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  
  if (*(int *)0x3d6 < 1) {
    uVar3 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
    iVar2 = (int)*(undefined4 *)0x1d10;
    if (*(int *)(*(int *)0x464 * 0x10 + iVar2 + 6) != 0 ||
        *(int *)(*(int *)0x464 * 0x10 + iVar2 + 8) != 0) {
      puVar1 = (undefined2 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 6);
      (*(code *)*puVar1)();
    }
  }
  else {
    uVar3 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
    iVar2 = (int)*(undefined4 *)0x1d10;
    if (*(int *)(*(int *)0x464 * 0x10 + iVar2 + 6) != 0 ||
        *(int *)(*(int *)0x464 * 0x10 + iVar2 + 8) != 0) {
      puVar1 = (undefined2 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 6);
      (*(code *)*puVar1)();
    }
    *(undefined1 *)(*(int *)0x46c + 0x3e3) =
         (char)((int)(*(byte *)(*(int *)0x46c + 0x3e3) + 1) % *(int *)0x3d6);
    func_0x000f5184();
  }
  return;
}



void __cdecl16far FUN_000e_cfb7(void)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  
  if (1 < *(int *)0x462) {
    uVar3 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
    iVar2 = (int)*(undefined4 *)0x1d10;
    if (*(int *)(*(int *)0x464 * 0x10 + iVar2 + 6) != 0 ||
        *(int *)(*(int *)0x464 * 0x10 + iVar2 + 8) != 0) {
      puVar1 = (undefined2 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 6);
      (*(code *)*puVar1)();
    }
    *(int *)0x462 = *(int *)0x462 + -1;
    uVar3 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
    iVar2 = (int)*(undefined4 *)0x1d10;
    if (*(int *)(*(int *)0x46c * 0x10 + iVar2 + 6) != 0 ||
        *(int *)(*(int *)0x46c * 0x10 + iVar2 + 8) != 0) {
      puVar1 = (undefined2 *)(*(int *)0x46c * 0x10 + (int)*(undefined4 *)0x1d10 + 6);
      (*(code *)*puVar1)();
    }
    *(undefined2 *)0x464 = *(undefined2 *)0x46c;
    *(undefined1 *)0x466 = *(undefined1 *)(*(int *)0x46c * 0x10 + (int)*(undefined4 *)0x1d10 + 0xe);
  }
  return;
}



void __cdecl16far FUN_000e_d058(void)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  
  if (*(char *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 4) == '\0') {
    *(undefined1 *)0x3e2 = 0x78;
    func_0x000f50ca();
  }
  else {
    uVar3 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
    iVar2 = (int)*(undefined4 *)0x1d10;
    if (*(int *)(*(int *)0x464 * 0x10 + iVar2 + 6) != 0 ||
        *(int *)(*(int *)0x464 * 0x10 + iVar2 + 8) != 0) {
      puVar1 = (undefined2 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 6);
      (*(code *)*puVar1)();
    }
    *(int *)0x462 = *(int *)0x462 + 1;
    *(undefined2 *)0x46c = *(undefined2 *)0x464;
    func_0x000f5184();
  }
  return;
}



/* snapshot_index_and_format_number (confidence: Mechanism confirmed)
   
   Evidence: Saves the current item index `[0x464]` into `[0x46C]` (a snapshot for whatever the
   formatter below reads next), then calls into the decimal-number-plus-units-suffix formatter
   (`SUB_F5184`/`L_F50FA` - see `docs/acquisition-and-plotting/mode-dispatcher-and-formatting.md`
   "Found: a decimal-formatting engine" for the full writeup); called from
   `compute_and_format_sample_delta_readout` */

void __cdecl16far snapshot_index_and_format_number(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x46c = *(undefined2 *)0x464;
  func_0x000f5184();
  return;
}



void __cdecl16far FUN_000e_d0ca(void)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  
  uVar3 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
  iVar2 = (int)*(undefined4 *)0x1d10;
  if (*(int *)(*(int *)0x464 * 0x10 + iVar2 + 6) != 0 ||
      *(int *)(*(int *)0x464 * 0x10 + iVar2 + 8) != 0) {
    puVar1 = (undefined2 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 6);
    (*(code *)*puVar1)();
  }
  return;
}



void __cdecl16far FUN_000e_d108(void)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  
  uVar3 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
  iVar2 = (int)*(undefined4 *)0x1d10;
  if (*(int *)(*(int *)0x464 * 0x10 + iVar2 + 6) != 0 ||
      *(int *)(*(int *)0x464 * 0x10 + iVar2 + 8) != 0) {
    puVar1 = (undefined2 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 6);
    (*(code *)*puVar1)();
  }
  return;
}



void __cdecl16far FUN_000e_d146(void)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  
  uVar3 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
  iVar2 = (int)*(undefined4 *)0x1d10;
  if (*(int *)(*(int *)0x464 * 0x10 + iVar2 + 6) != 0 ||
      *(int *)(*(int *)0x464 * 0x10 + iVar2 + 8) != 0) {
    puVar1 = (undefined2 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 6);
    (*(code *)*puVar1)();
  }
  return;
}



void __cdecl16far FUN_000e_d184(void)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  int local_a;
  
  local_a = 0;
  *(uint *)0x464 = (uint)*(byte *)(*(int *)0x46c * 0x10 + (int)*(undefined4 *)0x1d10 + 4);
  for (; local_a < (int)(uint)*(byte *)(*(int *)0x46c + 0x3e3); local_a = local_a + 1) {
    *(uint *)0x464 = (uint)*(byte *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 5);
  }
  uVar3 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
  iVar2 = (int)*(undefined4 *)0x1d10;
  if (*(int *)(*(int *)0x464 * 0x10 + iVar2 + 6) != 0 ||
      *(int *)(*(int *)0x464 * 0x10 + iVar2 + 8) != 0) {
    puVar1 = (undefined2 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 6);
    (*(code *)*puVar1)();
  }
  *(undefined1 *)0x466 = *(undefined1 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 0xe);
  return;
}



void __stdcall16far FUN_000e_d403(char *param_1,char *param_2)

{
  byte bVar1;
  undefined4 uVar2;
  char *pcVar3;
  int iVar4;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined2 uVar5;
  uint local_2c;
  int local_2a;
  int local_28;
  undefined4 uStack_26;
  int local_16;
  int local_14;
  int local_12;
  undefined4 local_10;
  char *local_c;
  
  if (*param_1 != '\0') {
    *param_1 = '\0';
    bVar1 = *(byte *)((int)*(undefined4 *)0x1d10 + 0x484);
    for (local_16 = 0; local_2c = (uint)bVar1, local_16 < 4; local_16 = local_16 + 1) {
      uVar2 = *(undefined4 *)((int)*(undefined4 *)0x1d10 + local_2c * 0x10);
      iVar4 = local_16 * 4;
      *(int *)(&uStack_26 + local_16) = (int)uVar2;
      *(undefined2 *)((int)&uStack_26 + iVar4 + 2) = (int)((ulong)uVar2 >> 0x10);
      bVar1 = *(byte *)((int)*(undefined4 *)0x1d10 + local_2c * 0x10 + 5);
    }
    uVar2 = *(undefined4 *)0x3ca;
    *(undefined2 *)0x45e = (int)uVar2;
    *(undefined2 *)0x460 = (int)((ulong)uVar2 >> 0x10);
    uVar2 = *(undefined4 *)0x3ce;
    *(undefined2 *)0x472 = (int)uVar2;
    *(undefined2 *)0x474 = (int)((ulong)uVar2 >> 0x10);
    local_14 = 0x2d5;
    *(undefined2 *)0x46e = 0;
    local_c = (char *)*(undefined4 *)0x1cd0;
    local_10 = *(undefined4 *)0x45e;
    uVar5 = 0x20;
    iVar4 = func_0x000f6510();
    *(int *)0x46e = *(int *)0x46e + iVar4;
    while (pcVar3 = local_c, *local_c != '\0') {
      local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
      iVar4 = func_0x000f6510(0xf651,*pcVar3,uVar5);
      *(int *)0x46e = *(int *)0x46e + iVar4;
    }
    *(undefined1 *)((int)*(undefined4 *)0x45e + *(int *)0x1c02 + -1) = 10;
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -2) = 10;
    func_0x000f44c8(0xf651,0xbe,local_14,(int)local_10,(int)((ulong)local_10 >> 0x10));
    *(int *)0x472 = *(int *)0x472 + 9;
    local_14 = local_14 + -0x32;
    *(undefined2 *)0x46e = 0;
    local_c = (char *)*(undefined4 *)0x1cd4;
    local_10 = *(undefined4 *)0x45e;
    iVar4 = func_0x000f6510(0xf408,0x20);
    *(int *)0x46e = *(int *)0x46e + iVar4;
    while (pcVar3 = local_c, *local_c != '\0') {
      local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
      iVar4 = func_0x000f6510(0xf651,*pcVar3);
      *(int *)0x46e = *(int *)0x46e + iVar4;
    }
    *(undefined1 *)((int)*(undefined4 *)0x45e + *(int *)0x1c02 + -1) = 10;
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -2) = 10;
    func_0x000f44c8(0xf651,300,local_14,(int)local_10,(int)((ulong)local_10 >> 0x10));
    *(int *)0x472 = *(int *)0x472 + 9;
    local_14 = local_14 + -0x32;
    *(undefined2 *)0x46e = 0;
    local_c = (char *)*(undefined4 *)0x1cd8;
    local_10 = *(undefined4 *)0x45e;
    iVar4 = func_0x000f6510(0xf408,0x20);
    *(int *)0x46e = *(int *)0x46e + iVar4;
    while (pcVar3 = local_c, *local_c != '\0') {
      local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
      iVar4 = func_0x000f6510(0xf651,*pcVar3);
      *(int *)0x46e = *(int *)0x46e + iVar4;
    }
    *(undefined1 *)((int)*(undefined4 *)0x45e + *(int *)0x1c02 + -1) = 10;
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -2) = 10;
    func_0x000f44c8(0xf651,300,local_14,(int)local_10,(int)((ulong)local_10 >> 0x10));
    *(int *)0x472 = *(int *)0x472 + 9;
    local_14 = local_14 + -0x32;
    *(undefined2 *)0x46e = 0;
    local_c = (char *)*(undefined4 *)0x1cdc;
    local_10 = *(undefined4 *)0x45e;
    iVar4 = func_0x000f6510(0xf408,0x20);
    *(int *)0x46e = *(int *)0x46e + iVar4;
    while (pcVar3 = local_c, *local_c != '\0') {
      local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
      iVar4 = func_0x000f6510(0xf651,*pcVar3);
      *(int *)0x46e = *(int *)0x46e + iVar4;
    }
    *(undefined1 *)((int)*(undefined4 *)0x45e + *(int *)0x1c02 + -1) = 10;
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -2) = 10;
    func_0x000f44c8(0xf651,300,local_14,(int)local_10,(int)((ulong)local_10 >> 0x10));
    *(int *)0x472 = *(int *)0x472 + 9;
    local_14 = local_14 + -0x19;
    for (local_28 = 0; local_28 < 4; local_28 = local_28 + 1) {
      *(undefined2 *)0x46e = 0;
      local_10 = *(undefined4 *)0x45e;
      local_14 = local_14 + -0x32;
      iVar4 = func_0x000f6510(0xf408,0);
      *(int *)0x46e = *(int *)0x46e + iVar4;
      local_c = (char *)(&uStack_26)[local_28];
      while (pcVar3 = local_c, *local_c != '\0') {
        local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
        iVar4 = func_0x000f6510(0xf651,*pcVar3);
        *(int *)0x46e = *(int *)0x46e + iVar4;
      }
      for (local_2a = 4; -1 < local_2a; local_2a = local_2a + -1) {
        *(int *)0x46e = (4 - local_2a) * 0x20 + 0x4e;
        if (*(char *)(local_2a + 0x3bc) ==
            *(char *)(local_2a * 4 + local_28 + (int)*(undefined4 *)0x1cf0)) {
          iVar4 = func_0x000f6510(0xf651,0x4f);
          *(int *)0x46e = *(int *)0x46e + iVar4;
          iVar4 = func_0x000f6510(0xf651,0x4e);
          *(int *)0x46e = *(int *)0x46e + iVar4;
        }
        else if (*(char *)((int)*(undefined4 *)0x1cf0 + local_2a * 4 + local_28) == '\x05') {
          iVar4 = func_0x000f6510(0xf651,0x2d);
          *(int *)0x46e = *(int *)0x46e + iVar4;
        }
        else {
          iVar4 = func_0x000f6510(0xf651,0x6f);
          *(int *)0x46e = *(int *)0x46e + iVar4;
          iVar4 = func_0x000f6510(0xf651,0x66);
          *(int *)0x46e = *(int *)0x46e + iVar4;
          iVar4 = func_0x000f6510(0xf651,0x66);
          *(int *)0x46e = *(int *)0x46e + iVar4;
        }
      }
      *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -1) = 10;
      *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -2) = 10;
      func_0x000f44c8(0xf651,0,local_14,(int)local_10,(int)((ulong)local_10 >> 0x10));
      *(int *)0x472 = *(int *)0x472 + 9;
    }
    local_14 = 0xe1;
    func_0x000f5898(0xf408,(int)*(undefined4 *)0x1ce0,(int)((ulong)*(undefined4 *)0x1ce0 >> 0x10),
                    0xe1);
    local_14 = local_14 + -0x32;
    unaff_CS = 0xf522;
    func_0x000f5898(0xf522,(int)*(undefined4 *)0x1ce4,(int)((ulong)*(undefined4 *)0x1ce4 >> 0x10),
                    local_14);
  }
  if (*param_2 != '\0') {
    *param_2 = '\0';
    local_12 = (4 - *(int *)0x3c2) * 0x80 + 0x131;
    local_14 = (*(int *)0x3c6 + 6) * -0x32 + 0x32f;
    func_0x000f6382(unaff_CS,(int)*(undefined4 *)0x1cfc,(int)((ulong)*(undefined4 *)0x1cfc >> 0x10),
                    local_12,local_14);
  }
  return;
}



void __cdecl16far FUN_000e_d848(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x3bc = *(undefined1 *)0x41d;
  *(undefined1 *)0x3bd = *(undefined1 *)0x424;
  *(undefined1 *)0x3be = *(undefined1 *)0x42b;
  *(undefined1 *)0x3bf = *(undefined1 *)0x433;
  *(undefined1 *)0x3c0 = *(undefined1 *)0x43b;
  return;
}



void __cdecl16far FUN_000e_d870(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x41d = *(undefined1 *)0x3bc;
  *(undefined1 *)0x424 = *(undefined1 *)0x3bd;
  *(undefined1 *)0x42b = *(undefined1 *)0x3be;
  *(undefined1 *)0x433 = *(undefined1 *)0x3bf;
  *(undefined1 *)0x43b = *(undefined1 *)0x3c0;
  return;
}



void __stdcall16far FUN_000e_d898(char *param_1,undefined2 param_2)

{
  undefined4 uVar1;
  char *pcVar2;
  int iVar3;
  undefined2 unaff_DS;
  undefined2 uVar4;
  
  *(undefined2 *)0x46e = 0;
  uVar1 = *(undefined4 *)0x45e;
  uVar4 = 0x20;
  iVar3 = func_0x000f6510();
  *(int *)0x46e = *(int *)0x46e + iVar3;
  while (pcVar2 = param_1, *param_1 != '\0') {
    param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
    iVar3 = func_0x000f6510(0xf651,*pcVar2,uVar4);
    *(int *)0x46e = *(int *)0x46e + iVar3;
  }
  *(undefined1 *)((int)*(undefined4 *)0x45e + *(int *)0x1c02 + -1) = 10;
  *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -2) = 10;
  func_0x000f44c8(0xf651,0,param_2,(int)uVar1,(int)((ulong)uVar1 >> 0x10));
  *(int *)0x472 = *(int *)0x472 + 9;
  return;
}



undefined2 __stdcall16far FUN_000e_d914(int param_1)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_DS;
  undefined2 local_a;
  
  param_1 = param_1 * 0x10;
  uVar3 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
  iVar2 = (int)*(undefined4 *)0x1d10;
  if (*(int *)(iVar2 + param_1 + 6) == 0 && *(int *)(iVar2 + param_1 + 8) == 0) {
    local_a = 0xffff;
  }
  else {
    puVar1 = (undefined2 *)((int)*(undefined4 *)0x1d10 + param_1 + 6);
    local_a = (*(code *)*puVar1)();
  }
  return local_a;
}



uint __stdcall16far FUN_000e_d952(int param_1)

{
  uint uVar1;
  undefined2 unaff_DS;
  
  uVar1 = (uint)*(byte *)((int)*(undefined4 *)0x1d10 + param_1 * 0x10 + 4);
  if (uVar1 == 0) {
    uVar1 = 0xffff;
  }
  return uVar1;
}



uint __stdcall16far FUN_000e_d983(int param_1)

{
  uint uVar1;
  undefined2 unaff_DS;
  
  uVar1 = (uint)*(byte *)((int)*(undefined4 *)0x1d10 + param_1 * 0x10 + 5);
  if (uVar1 == 0) {
    uVar1 = 0xffff;
  }
  return uVar1;
}



void __stdcall16far FUN_000e_d9b4(int param_1)

{
  int iVar1;
  undefined2 unaff_DS;
  int local_e;
  int local_a;
  
  if (param_1 == 1) {
    *(undefined1 *)0x468 = 1;
  }
  else {
    if (param_1 == 2) {
      *(undefined1 *)0x476 = 0;
      return;
    }
    if (param_1 != 3) {
      return;
    }
  }
  if ((((*(int *)0x52a != 0) || (*(char *)0x468 != '\0')) || ((*(byte *)0x4e8 & 8) != 0)) ||
     ((*(byte *)0x4ec & 2) != 0)) {
    if (*"" == '\0') {
      local_a = *(int *)0x1bec + *(int *)0x52a;
    }
    else {
      local_a = *(int *)0x1bec - *(int *)0x52a;
    }
    if (local_a < 0x20) {
      *(undefined2 *)0x1bec = 0x20;
    }
    else if (local_a < 0x1001) {
      *(int *)0x1bec = local_a;
    }
    else {
      *(undefined2 *)0x1bec = 0x1000;
    }
    if (*(char *)0x1b70 == '\x01') {
      local_e = 0x400;
    }
    else {
      local_e = 0x1000;
    }
    iVar1 = func_0x000f5d89();
    *(undefined1 *)(iVar1 + 0x476) = 0x2f;
    if (local_e == 0x400) {
      *(undefined1 *)(iVar1 + 0x477) = 0x31;
    }
    else {
      *(undefined1 *)(iVar1 + 0x477) = 0x34;
    }
    *(undefined1 *)(iVar1 + 0x478) = 0x4b;
    *(undefined1 *)(iVar1 + 0x479) = 0;
    if (*"" == '\0') {
      func_0x000f6382(0xf59b,(int)*(undefined4 *)0x1d00,(int)((ulong)*(undefined4 *)0x1d00 >> 0x10),
                      (undefined *)&DAT_000e_ff0c + (*(int *)0x1bec >> 3),500);
    }
    else {
      func_0x000f6382(0xf59b,(int)*(undefined4 *)0x1d00,(int)((ulong)*(undefined4 *)0x1d00 >> 0x10),
                      0x3ec - (*(int *)0x1bec >> 3),500);
    }
  }
  return;
}



void __stdcall16far FUN_000e_db5b(int param_1)

{
  undefined2 unaff_DS;
  
  if (param_1 != 1) {
    if (param_1 == 2) {
      *(undefined1 *)0x476 = 0;
      return;
    }
    if (param_1 != 3) {
      return;
    }
  }
  if (*(int *)0x52a != 0) {
    *(int *)0x1bee = *(int *)0x1bee + *(int *)0x52a;
    if (*(int *)0x1bee < 0) {
      *(undefined2 *)0x1bee = 0;
    }
    else if (0x800 < *(int *)0x1bee) {
      *(undefined2 *)0x1bee = 0x800;
    }
  }
  *(undefined1 *)0x476 = 0x31;
  *(undefined1 *)0x477 = 0x2f;
  func_0x000f5d89();
  return;
}



void __stdcall16far FUN_000e_dbf9(int param_1)

{
  uint uVar1;
  ulong uVar2;
  undefined1 *puVar3;
  int iVar4;
  int iVar5;
  undefined2 unaff_DS;
  uint local_1a;
  uint local_14;
  uint local_12;
  undefined1 *local_10;
  
  if (param_1 != 1) {
    if (param_1 == 2) {
      *(undefined1 *)0x476 = 0;
      return;
    }
    if (param_1 != 3) {
      return;
    }
  }
  if (*(int *)0x52a != 0) {
    iVar4 = *(int *)0x3ba;
    *(int *)0x3ba = *(int *)0x3ba + *(int *)0x52a;
    iVar4 = (*(int *)0x3ba >> 4) - (iVar4 >> 4);
    iVar5 = *(int *)0x1bf0 >> 4;
    local_14 = *(uint *)0x1bf0 & 0xf;
    if (iVar5 < 200) {
      local_12 = iVar5 + iVar4;
    }
    else {
      local_12 = iVar5 + iVar4 * 2 & 0xfffe;
    }
    if ((int)local_12 < 1000) {
      if ((int)local_12 < 0) {
        if (local_14 == 0) {
          local_12 = 0x3e6;
          local_14 = 3;
        }
        else {
          local_12 = 0;
          local_14 = 0;
        }
      }
      else if (((int)local_12 < 100) && (local_14 != 0)) {
        local_14 = local_14 - 1;
        local_12 = local_12 * 10;
      }
    }
    else {
      local_12 = (int)local_12 / 10;
      local_14 = local_14 + 1;
      if (3 < local_14) {
        local_14 = 0;
        local_12 = 0;
      }
    }
    *(uint *)0x1bf0 = local_12 << 4 | local_14;
  }
  if (*(int *)0x1bf0 >> 4 == 0) {
    func_0x000f5e1a();
  }
  else {
    uVar1 = *(uint *)0x1bf0;
    iVar4 = func_0x000f5d89();
    _local_10 = (undefined1 *)CONCAT22(unaff_DS,(undefined1 *)(iVar4 + 0x476));
    local_1a = uVar1 & 7;
    while (puVar3 = _local_10, local_1a != 0) {
      uVar2 = (ulong)_local_10 >> 0x10;
      _local_10 = (undefined1 *)CONCAT22((int)uVar2,local_10 + 1);
      *puVar3 = 0x30;
      local_1a = local_1a - 1;
    }
    *_local_10 = 0;
  }
  return;
}



int __stdcall16far FUN_000e_dd89(uint param_1,char *param_2)

{
  char *pcVar1;
  char *pcVar2;
  char *pcVar3;
  undefined2 unaff_SS;
  int local_18;
  char *local_16;
  undefined2 uVar4;
  char local_12 [9];
  char local_9 [7];
  
  local_18 = 0;
  _local_16 = (char *)CONCAT22(unaff_SS,local_12);
  while( true ) {
    pcVar3 = (char *)_local_16;
    if (local_9 <= pcVar3) break;
    *_local_16 = ' ';
    uVar4 = (undefined2)((ulong)_local_16 >> 0x10);
    _local_16 = (char *)CONCAT22(uVar4,pcVar3 + 1);
  }
  _local_16 = (char *)CONCAT22(unaff_SS,local_9);
  do {
    uVar4 = (undefined2)((ulong)_local_16 >> 0x10);
    _local_16 = (char *)CONCAT22(uVar4,local_16 + -1);
    *_local_16 = (char)((ulong)param_1 % 10) + '0';
    param_1 = param_1 / 10;
  } while (param_1 != 0);
  while (pcVar2 = _local_16, pcVar1 = param_2, *_local_16 != '\0') {
    param_2 = (char *)CONCAT22(param_2._2_2_,(char *)param_2 + 1);
    uVar4 = (undefined2)((ulong)_local_16 >> 0x10);
    _local_16 = (char *)CONCAT22(uVar4,local_16 + 1);
    *pcVar1 = *pcVar2;
    local_18 = local_18 + 1;
  }
  *param_2 = '\0';
  return local_18;
}



void __stdcall16far FUN_000e_de1a(char *param_1,char *param_2)

{
  char *pcVar1;
  char *pcVar2;
  
  do {
    pcVar2 = param_2;
    pcVar1 = param_1;
    param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
    param_2 = (char *)CONCAT22(param_2._2_2_,(char *)param_2 + 1);
    *pcVar1 = *pcVar2;
  } while (*param_2 != '\0');
  *param_1 = '\0';
  return;
}



void __cdecl16far FUN_000e_de4c(void)

{
  undefined2 unaff_DS;
  
  func_0x000f5f00();
  func_0x000f5e99(0xf5e4);
  func_0x000f60bb(0xf5e4);
  *(code *)selftest_acq_ram = (code)0x1;
  *(undefined2 *)0x53a = 0xffff;
  return;
}



void __stdcall16far FUN_000e_de70(int param_1)

{
  undefined2 unaff_DS;
  
  if (((param_1 != 4) && ((*(byte *)0x4e7 & 0x80) != 0)) && ((*(byte *)0x4e8 & 0x80) != 0)) {
    func_0x000e6ec0();
  }
  return;
}



void __cdecl16far FUN_000e_de99(void)

{
  undefined2 unaff_DS;
  
  func_0x000f5edd();
  func_0x000f5ecd(0xf5e4);
  func_0x000f5eb7(0xf5e4);
  *(byte *)0x467 = *(byte *)0x467 | 1;
  return;
}



void __cdecl16far FUN_000e_deb7(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x1bf2 = 0;
  *(undefined2 *)0x1bf4 = 0;
  return;
}



void __cdecl16far FUN_000e_decd(void)

{
  undefined2 unaff_DS;
  
  *(undefined2 *)0x1bee = 0x200;
  return;
}



void __cdecl16far FUN_000e_dedd(void)

{
  undefined2 unaff_DS;
  
  *(undefined1 *)0x41d = 2;
  *(undefined1 *)0x424 = 0;
  *(undefined1 *)0x42b = 3;
  *(undefined1 *)0x433 = 3;
  *(undefined1 *)0x43b = 1;
  return;
}



void __cdecl16far FUN_000e_df00(void)

{
  undefined2 unaff_DS;
  int local_a;
  
  for (local_a = 0; local_a < *(int *)*(undefined4 *)0x1d14; local_a = local_a + 1) {
    *(undefined1 *)(local_a + 0x3e3) = 0;
  }
  *(undefined1 *)0x3f0 = 1;
  *(undefined1 *)0x3ea = 0;
  *(undefined1 *)0x3ed = 1;
  *(undefined1 *)0x412 = 1;
  if (*(char *)0x1bf9 != '\0') {
    func_0x00096800();
  }
  if (*"" != '\x14') {
    *(undefined2 *)0x1bec = 0x400;
  }
  return;
}



void __cdecl16far FUN_000e_df56(void)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  
  if ((*(char *)0x3e2 != 'x') || (*(char *)0x468 != '\0')) {
    unaff_CS = 0xf408;
    func_0x000f408e();
  }
  if ((*(byte *)0x466 & 4) == 0) {
    if ((*(char *)0x3e2 == 'x') && (*(char *)0x468 == '\0')) {
      if (((*(byte *)0x466 & 1) != 0) ||
         ((uVar4 = unaff_CS, (*(byte *)0x466 & 0x40) != 0 && (*(char *)0x469 == '\0')))) {
        uVar4 = 0xf408;
        func_0x000f47ab(unaff_CS,0,1);
      }
      uVar3 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
      iVar2 = (int)*(undefined4 *)0x1d10;
      if ((*(int *)(*(int *)0x464 * 0x10 + iVar2 + 6) != 0 ||
           *(int *)(*(int *)0x464 * 0x10 + iVar2 + 8) != 0) && ((*(byte *)0x466 & 0x80) != 0)) {
        puVar1 = (undefined2 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 6);
        (*(code *)*puVar1)(uVar4,4);
      }
    }
    else {
      if (*(char *)0x468 != '\0') {
        *(undefined1 *)0x468 = 0;
      }
      uVar5 = 1;
      uVar4 = func_0x000f60f7(unaff_CS,0,1,1,1,1);
      uVar3 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
      iVar2 = (int)*(undefined4 *)0x1d10;
      if ((*(int *)(*(int *)0x464 * 0x10 + iVar2 + 6) != 0 ||
           *(int *)(*(int *)0x464 * 0x10 + iVar2 + 8) != 0) && ((*(byte *)0x466 & 0x80) != 0)) {
        puVar1 = (undefined2 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 6);
        (*(code *)*puVar1)(0xf5f5,4,uVar5,uVar4);
      }
    }
  }
  else {
    if ((*(char *)0x3e2 != 'x') || (uVar4 = unaff_CS, *(char *)0x468 != '\0')) {
      uVar4 = 0xf408;
      func_0x000f45a4(unaff_CS);
    }
    uVar3 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
    iVar2 = (int)*(undefined4 *)0x1d10;
    if (*(int *)(*(int *)0x464 * 0x10 + iVar2 + 6) != 0 ||
        *(int *)(*(int *)0x464 * 0x10 + iVar2 + 8) != 0) {
      puVar1 = (undefined2 *)(*(int *)0x464 * 0x10 + (int)*(undefined4 *)0x1d10 + 6);
      (*(code *)*puVar1)(uVar4,4);
    }
    if (*(char *)0x468 != '\0') {
      *(undefined1 *)0x468 = 0;
    }
  }
  return;
}



void __cdecl16far FUN_000e_e0bb(void)

{
  undefined2 unaff_DS;
  
  *(uint *)0x464 = (uint)*(byte *)((int)*(undefined4 *)0x1d10 + 4);
  *(undefined2 *)0x46c = 0;
  *(undefined2 *)0x462 = 1;
  *(undefined1 *)0x466 = *(undefined1 *)((int)*(undefined4 *)0x1d10 + *(int *)0x464 * 0x10 + 0xe);
  return;
}



int __stdcall16far FUN_000e_e0f7(int param_1,int param_2,int param_3)

{
  byte bVar1;
  byte bVar2;
  int iVar3;
  undefined2 unaff_CS;
  undefined2 uVar4;
  undefined2 unaff_DS;
  undefined2 uVar5;
  uint local_10;
  uint local_e;
  uint local_c;
  int local_a;
  
  local_c = (uint)*(byte *)((int)*(undefined4 *)0x1d10 + param_1 * 0x10 + 4);
  if ((param_2 <= *(int *)0x462 + 1) && (local_c != 0)) {
    if (param_2 < *(int *)0x462) {
      for (local_a = 0; local_a < (int)(uint)*(byte *)(param_1 + 0x3e3); local_a = local_a + 1) {
        local_c = (uint)*(byte *)((int)*(undefined4 *)0x1d10 + local_c * 0x10 + 5);
      }
      uVar5 = 1;
      func_0x000f4150();
      param_3 = func_0x000f60f7(0xf408,local_c,param_2 + 1,param_3 + 1,uVar5);
    }
    else if (param_2 == *(int *)0x462) {
      bVar1 = *(byte *)((int)*(undefined4 *)0x1d10 + param_1 * 0x10 + 4);
      while ((local_e = (uint)bVar1, local_e != 0 && (param_3 < 0xf))) {
        if (local_e == *(uint *)0x464) {
          func_0x000f4150(unaff_CS,local_e,param_2,0xb);
          uVar5 = (undefined2)((ulong)*(undefined4 *)0x1d10 >> 0x10);
          iVar3 = (int)*(undefined4 *)0x1d10;
          if (*(int *)(iVar3 + local_e * 0x10 + 10) != 0 ||
              *(int *)(iVar3 + local_e * 0x10 + 0xc) != 0) {
            func_0x000f43cc(0xf408,0x57);
          }
          uVar4 = 0xf408;
          if ((*(byte *)((int)*(undefined4 *)0x1d10 + local_e * 0x10 + 0xe) & 8) == 0) {
            if ((*(byte *)((int)*(undefined4 *)0x1d10 + local_e * 0x10 + 0xe) & 0x10) != 0) {
              uVar4 = 0xf638;
              func_0x000f6382(0xf408,(int)*(undefined4 *)0x1cf8,
                              (int)((ulong)*(undefined4 *)0x1cf8 >> 0x10),800,700);
            }
          }
          else {
            uVar4 = 0xf638;
            func_0x000f6382(0xf408,(int)*(undefined4 *)0x1cf4,
                            (int)((ulong)*(undefined4 *)0x1cf4 >> 0x10),800,700);
          }
          uVar5 = 0xf5f5;
          param_3 = func_0x000f60f7(uVar4,local_e,param_2 + 1,param_3 + 1);
        }
        else {
          uVar5 = 0xf408;
          func_0x000f4150(unaff_CS,local_e,param_2,0);
          param_3 = param_3 + 1;
        }
        bVar1 = *(byte *)((int)*(undefined4 *)0x1d10 + local_e * 0x10 + 5);
        unaff_CS = uVar5;
      }
    }
    else if (*(int *)0x462 < param_2) {
      bVar2 = *(byte *)((int)*(undefined4 *)0x1d10 + param_1 * 0x10 + 4);
      bVar1 = *(byte *)(param_1 + 0x3e3);
      local_10 = 0;
      while ((local_e = (uint)bVar2, local_e != 0 && (param_3 < 0xf))) {
        if (local_10 == bVar1) {
          func_0x000f4150(unaff_CS,local_e,param_2,0xc);
        }
        else {
          func_0x000f4150(unaff_CS,local_e,param_2,8);
        }
        param_3 = param_3 + 1;
        unaff_CS = 0xf408;
        bVar2 = *(byte *)((int)*(undefined4 *)0x1d10 + local_e * 0x10 + 5);
        local_10 = local_10 + 1;
      }
    }
  }
  return param_3;
}



void __cdecl16far FUN_000e_e35e(void)

{
  undefined2 uVar1;
  undefined2 unaff_DS;
  undefined2 uVar2;
  
  uVar2 = 0x41;
  uVar1 = func_0x000f0078();
  *(undefined1 *)0x467 = 1;
  func_0x000f0078(0xf007,uVar1,uVar2,uVar1);
  return;
}



void __stdcall16far FUN_000e_e382(void)

{
  func_0x000f63bc();
  return;
}



void __stdcall16far
FUN_000e_e3bc(byte *param_1,undefined2 param_2,undefined2 param_3,byte *param_4,undefined4 param_5,
             int param_6)

{
  byte *pbVar1;
  undefined4 uVar2;
  byte *pbVar3;
  byte *pbVar4;
  byte bVar5;
  byte bVar6;
  byte *pbVar7;
  undefined2 unaff_DS;
  undefined4 local_10;
  
  local_10 = param_4;
  while( true ) {
    pbVar3 = param_1;
    pbVar7 = (byte *)param_1;
    param_1 = (byte *)CONCAT22(param_1._2_2_,(byte *)param_1 + 1);
    pbVar4 = param_1;
    bVar5 = *pbVar3;
    if (bVar5 == 0xff) break;
    param_1 = (byte *)CONCAT22(param_1._2_2_,pbVar7 + 2);
    if ((*pbVar4 == 0xff) ||
       (pbVar7 = (byte *)local_10, (byte *)((int)*(undefined4 *)0x1cc4 + 0x1400) <= pbVar7)) break;
    *local_10 = *pbVar4 & 0x7f;
    pbVar1 = pbVar7 + 1;
    local_10 = (byte *)CONCAT22(local_10._2_2_,pbVar1);
    *local_10 = bVar5 & 0x7f;
    pbVar1[*(int *)0x1c02 + 1] = 2;
    pbVar1[*(int *)0x1c02 + 2] = 2;
    if (param_6 == 0) {
      pbVar1[*(int *)0x1c02] = (bVar5 & 0x80) == 0 | 8;
      pbVar1[*(int *)0x1c02 + -1] = (bVar5 & 0x80) == 0 | 8;
    }
    else {
      if ((bVar5 & 0x80) == 0) {
        bVar6 = 3;
      }
      else {
        bVar6 = 0;
      }
      pbVar1[*(int *)0x1c02] = bVar6 | 8;
      if ((bVar5 & 0x80) == 0) {
        bVar5 = 3;
      }
      else {
        bVar5 = 0;
      }
      pbVar1[*(int *)0x1c02 + -1] = bVar5 | 8;
    }
    local_10 = (byte *)CONCAT22(local_10._2_2_,pbVar7 + 2);
  }
  uVar2 = *(undefined4 *)0x472;
  *(undefined2 *)0x472 = (int)param_5;
  *(undefined2 *)0x474 = (int)((ulong)param_5 >> 0x10);
  func_0x000f44c8();
  *(undefined2 *)0x472 = (int)uVar2;
  *(undefined2 *)0x474 = (int)((ulong)uVar2 >> 0x10);
  return;
}



int __stdcall16far FUN_000e_e510(byte param_1)

{
  byte bVar1;
  byte *pbVar2;
  byte *pbVar3;
  undefined2 unaff_DS;
  undefined4 local_c;
  
  local_c = (byte *)*(undefined4 *)((uint)param_1 * 4 + (int)*(undefined4 *)0x1db0);
  while( true ) {
    pbVar3 = local_c;
    pbVar2 = (byte *)local_c;
    local_c = (byte *)CONCAT22(local_c._2_2_,(byte *)local_c + 1);
    bVar1 = *pbVar3;
    if ((bVar1 == 0) || ((int)*(undefined4 *)0x1cc4 + 0x1400U <= (uint)*(undefined4 *)0x45e)) break;
    *(byte *)*(undefined4 *)0x45e = bVar1 & 0xf;
    *(int *)0x45e = *(int *)0x45e + 1;
    *(char *)*(undefined4 *)0x45e = (char)((int)(bVar1 & 0x70) >> 4) + (char)*(undefined2 *)0x46e;
    *(undefined1 *)((int)*(undefined4 *)0x45e + *(int *)0x1c02 + 1) = 2;
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + 2) = 2;
    *(byte *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e) = (bVar1 & 0x80) != 0 | 8;
    *(byte *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -1) = (bVar1 & 0x80) != 0 | 8;
    *(int *)0x45e = *(int *)0x45e + 1;
    *(int *)0x4de = *(int *)0x4de + 1;
  }
  return (int)(pbVar2[-1] & 0x70) >> 4;
}



int __stdcall16far FUN_000e_e60c(byte param_1)

{
  byte *pbVar1;
  byte *pbVar2;
  byte bVar3;
  byte bVar4;
  undefined2 unaff_DS;
  undefined4 local_c;
  
  local_c = (byte *)*(undefined4 *)((uint)param_1 * 4 + (int)*(undefined4 *)0x1db0);
  while( true ) {
    pbVar2 = local_c;
    pbVar1 = (byte *)local_c;
    local_c = (byte *)CONCAT22(local_c._2_2_,(byte *)local_c + 1);
    bVar3 = *pbVar2;
    if ((bVar3 == 0) || ((int)*(undefined4 *)0x1cc4 + 0x1400U <= (uint)*(undefined4 *)0x45e)) break;
    *(byte *)*(undefined4 *)0x45e = bVar3 & 0xf;
    *(int *)0x45e = *(int *)0x45e + 1;
    *(char *)*(undefined4 *)0x45e = (char)((int)(bVar3 & 0x70) >> 4) + (char)*(undefined2 *)0x46e;
    *(undefined1 *)((int)*(undefined4 *)0x45e + *(int *)0x1c02 + 1) = 2;
    *(undefined1 *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + 2) = 2;
    if ((bVar3 & 0x80) == 0) {
      bVar4 = 0;
    }
    else {
      bVar4 = 3;
    }
    *(byte *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e) = bVar4 | 8;
    if ((bVar3 & 0x80) == 0) {
      bVar3 = 0;
    }
    else {
      bVar3 = 3;
    }
    *(byte *)(*(int *)0x1c02 + (int)*(undefined4 *)0x45e + -1) = bVar3 | 8;
    *(int *)0x45e = *(int *)0x45e + 1;
    *(int *)0x4de = *(int *)0x4de + 1;
  }
  return (int)(pbVar1[-1] & 0x70) >> 4;
}



void __cdecl16far FUN_000e_e708(void)

{
  undefined2 unaff_DS;
  undefined2 uVar1;
  
  *(undefined1 *)0x33f = *(undefined1 *)0x1b8b;
  uVar1 = 3;
  func_0x000f6dc9();
  if ((*(char *)0x1f == '\x04') || (*(char *)0x1f == '\x03')) {
    *(undefined2 *)0x342 = 1;
  }
  else {
    *(undefined2 *)0x342 = 0;
  }
  *(undefined1 *)0x33e = 0;
  if ((*(code *)FUN_000e_1aaa == (code)0x3) || ((*(byte *)0x1b75 & 8) != 0)) {
    *(byte *)0x33e = *(byte *)0x33e | 1;
  }
  if (*(byte *)0x1f < 3) {
    *(byte *)0x33e = *(byte *)0x33e | 2;
  }
  if (*(char *)0x1f == '\0') {
    if (*(code *)FUN_000e_1aaa == (code)0x2) {
      *(undefined1 *)0x340 = 3;
    }
    else if (*(code *)FUN_000e_1aaa == (code)0x3) {
      *(undefined1 *)0x340 = 2;
    }
    else {
      *(undefined1 *)0x340 = 5;
    }
  }
  else if ((*(char *)0x1f == '\x01') || (*(char *)0x1f == '\x02')) {
    *(undefined1 *)0x340 = 0;
  }
  else {
    *(undefined1 *)0x340 = 1;
  }
  if ((((char *)s_sys_rom_0_A10_U9109_000e_009c)[0x10] & 0x20U) == 0) {
    *(undefined1 *)0x340 = 4;
  }
  func_0x000e6884(0xf670,uVar1);
  return;
}



void __cdecl16far FUN_000e_e7bf(void)

{
  undefined2 unaff_DS;
  
  if ((*(uint *)0x22 & 0x80) == 0) {
    if (((*(byte *)0x20 & 0x40) == 0) || ((*(uint *)0x22 & 0x100) != 0)) {
      *(uint *)0x22 = *(uint *)0x22 & 0xfeff;
      *(uint *)0x548 = *(uint *)0x548 | 0x800;
      if (*(byte *)0x1f < 3) {
        func_0x000f6817();
      }
      else {
        func_0x000f690b();
      }
      *(uint *)0x22 = *(uint *)0x22 | 0x200;
    }
  }
  else {
    *(uint *)0x22 = *(uint *)0x22 & 0xff7f;
  }
  return;
}



void __cdecl16far FUN_000e_e817(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  byte *local_c;
  
  _local_c = (byte *)CONCAT22(unaff_DS,(char *)s_sys_rom_0_A10_U9109_000e_009c + 0x10);
  if (*(char *)0x33f != '\0') {
    if (((*(byte *)0x20 & 0x10) == 0) || (*(char *)0x1f != '\0')) {
      unaff_CS = 0xf670;
      func_0x000f6b62();
    }
    else {
      unaff_CS = 0xf670;
      func_0x000f6b62();
    }
  }
  func_0x000f6e05(unaff_CS);
  if ((*_local_c & 0x40) != 0) {
    if (((*(byte *)0x20 & 0x10) == 0) || (*(char *)0x1f != '\0')) {
      func_0x000f69a5(0xf670,*(undefined1 *)0x20);
    }
    else {
      func_0x000f69a5(0xf670,3);
    }
  }
  return;
}



void __stdcall16far FUN_000e_e891(void)

{
  func_0x000f1744();
  return;
}



void __cdecl16far FUN_000e_e90b(void)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  byte *local_e;
  uint local_a;
  
  _local_e = (byte *)CONCAT22(unaff_DS,(char *)s_sys_rom_0_A10_U9109_000e_009c + 0x10);
  if (*(char *)0x33f != '\0') {
    unaff_CS = 0xf670;
    func_0x000f6c1b();
  }
  func_0x000f6e05(unaff_CS);
  if ((*_local_e & 1) != 0) {
    local_a = *(byte *)0x33e & 1;
    *(undefined2 *)0x3b8 = *(undefined2 *)0x3a;
    if ((*(byte *)0x1b75 & 8) != 0) {
      local_a = local_a + 0x100;
      *(uint *)0x3b8 = *(int *)0x3b8 >> 1 & 0xfffe;
    }
    if ((*(byte *)0x20 & 1) != 0) {
      func_0x000f6891(0xf670,0,8,0,local_a);
    }
    if ((*(byte *)0x20 & 2) != 0) {
      func_0x000f6891(0xf670,1,9,1,local_a);
    }
  }
  return;
}



void __stdcall16far FUN_000e_e9a5(byte param_1)

{
  int iVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if ((((char *)s_sys_rom_0_A10_U9109_000e_009c)[0x10] & 1U) != 0) {
    iVar1 = *(int *)0x2a;
    if ((param_1 & 1) != 0) {
      unaff_CS = 0xf670;
      func_0x000f69fa();
    }
    if ((param_1 & 2) != 0) {
      func_0x000f69fa(unaff_CS,1,9,iVar1 + 1);
    }
  }
  return;
}



void __stdcall16far FUN_000e_e9fa(int param_1,int param_2)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined1 local_17;
  
  uVar3 = *(undefined4 *)0x1c80;
  iVar2 = *(int *)(param_2 * 4 + 0x3c);
  cVar1 = *(char *)0x340;
  if ((cVar1 == '\0') || (cVar1 == '\x01')) {
    unaff_CS = 0xf180;
    func_0x000f1808();
    goto LAB_000e_eaeb;
  }
  if (cVar1 != '\x02') {
    if (cVar1 == '\x03') {
      unaff_CS = 0xf180;
      func_0x000f1a34();
      goto LAB_000e_eaeb;
    }
    if (cVar1 == '\x04') {
      unaff_CS = 0xf180;
      func_0x000f1a7c();
      goto LAB_000e_eaeb;
    }
    if (cVar1 != '\x05') goto LAB_000e_eaeb;
  }
  unaff_CS = 0xf180;
  func_0x000f199e();
LAB_000e_eaeb:
  if ((((*(byte *)0x233 & 1) == 0) && (param_1 == 0)) ||
     (((*(byte *)0x233 & 2) == 0 && (param_1 == 1)))) {
    if ((*(byte *)0x1cc & 0x10) == 0) {
      if ((*(byte *)0x1cc & 0x20) == 0) {
        local_17 = 2;
      }
      else {
        local_17 = 0;
      }
    }
    else {
      local_17 = 1;
    }
    func_0x000fbd86(unaff_CS,(int)uVar3 + iVar2,(int)((ulong)uVar3 >> 0x10),*(undefined2 *)0x1d4,
                    local_17);
  }
  return;
}



void __stdcall16far FUN_000e_eb62(byte param_1)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  undefined2 local_c;
  
  if ((param_1 & 1) != 0) {
    unaff_CS = 0xf670;
    func_0x000f6c86();
  }
  _local_c = (int *)CONCAT22(unaff_DS,(int *)0x6e);
  if ((param_1 & 2) != 0) {
    func_0x000f6c86(unaff_CS,1,(int)*(undefined4 *)0x30e + *(int *)0x40 + *_local_c,
                    (int)((ulong)*(undefined4 *)0x30e >> 0x10),
                    (int)*(undefined4 *)0x30e + *(int *)0x40 + *(int *)0x70,
                    (int)((ulong)*(undefined4 *)0x30e >> 0x10),0x6e,unaff_DS);
  }
  return;
}



void __stdcall16far FUN_000e_ec1b(byte param_1)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if ((param_1 & 1) != 0) {
    unaff_CS = 0xf670;
    func_0x000f6c86();
    *(byte *)0x344 = *(byte *)0x344 & 0xfe;
  }
  if ((param_1 & 2) != 0) {
    func_0x000f6c86(unaff_CS,1,(int)*(undefined4 *)0x312,(int)((ulong)*(undefined4 *)0x312 >> 0x10),
                    (int)*(undefined4 *)0x312 + *(int *)0x3a + -1,
                    (int)((ulong)*(undefined4 *)0x312 >> 0x10));
    *(byte *)0x344 = *(byte *)0x344 & 0xfd;
  }
  return;
}



void __stdcall16far FUN_000e_ec86(int param_1)

{
  undefined2 unaff_DS;
  undefined1 local_11;
  
  if (param_1 == 0) {
    local_11 = 1;
  }
  else {
    local_11 = 2;
  }
  if (*(char *)0x33f == '\x02') {
    if ((*(byte *)0x344 & local_11) == 0) {
      func_0x000f1c6a();
    }
    else {
      func_0x000f1bf9();
    }
  }
  else if (*(char *)0x33f == '\x03') {
    if ((*(byte *)0x344 & local_11) == 0) {
      func_0x000f1c5d();
    }
    else {
      func_0x000f1bec();
    }
  }
  return;
}



void __stdcall16far FUN_000e_edc9(byte param_1)

{
  undefined2 unaff_DS;
  
  if ((param_1 & 1) == 0) {
    *(byte *)0x344 = *(byte *)0x344 & 0xfe;
  }
  else {
    *(byte *)0x344 = *(byte *)0x344 | 1;
  }
  if ((param_1 & 2) == 0) {
    *(byte *)0x344 = *(byte *)0x344 & 0xfd;
  }
  else {
    *(byte *)0x344 = *(byte *)0x344 | 2;
  }
  return;
}



void __cdecl16far FUN_000e_ee05(void)

{
  undefined2 uVar1;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if ((*(byte *)0x20 & 1) != 0) {
    unaff_CS = 0xf80a;
    uVar1 = func_0x000f830e();
    *(undefined2 *)0x35c = uVar1;
    *(byte *)0x35b = *(byte *)0x35b | 1;
  }
  if ((*(byte *)0x20 & 2) != 0) {
    uVar1 = func_0x000f830e(unaff_CS,(int)*(undefined4 *)0x1c80,
                            (int)((ulong)*(undefined4 *)0x1c80 >> 0x10),1,2);
    *(undefined2 *)0x36e = uVar1;
    *(byte *)0x36d = *(byte *)0x36d | 1;
  }
  return;
}



void __stdcall16far FUN_000e_ee5c(byte param_1)

{
  undefined2 unaff_DS;
  undefined2 local_c;
  
  if ((*(byte *)(((int)(uint)param_1 >> 1) * 0x10 + 0x18c) & 0x20) == 0) {
    if ((param_1 & 1) == 0) {
      _local_c = (byte *)CONCAT22(unaff_DS,(byte *)((uint)param_1 * 0x14 + 0xa6));
    }
    else {
      _local_c = (byte *)CONCAT22(unaff_DS,(byte *)((param_1 - 1) * 0x14 + 0xa6));
    }
  }
  else {
    _local_c = (byte *)CONCAT22(unaff_DS,(byte *)((uint)param_1 * 0x14 + 0xa6));
  }
  func_0x000fbc84();
  *_local_c = *_local_c | 4;
  return;
}



undefined2 __stdcall16far FUN_000e_f015(int param_1,int param_2)

{
  long lVar1;
  undefined2 unaff_DS;
  undefined2 local_e;
  
  local_e = param_2 - param_1;
  if (local_e < 4) {
    if (local_e < -3) {
      local_e = -3;
    }
  }
  else {
    local_e = 3;
  }
  lVar1 = (long)(int)(uint)*(byte *)(param_1 + (int)*(undefined4 *)0x1c88) * 7;
  return CONCAT11((char)((ulong)lVar1 >> 8),
                  *(undefined1 *)((int)lVar1 + local_e + 3 + (int)*(undefined4 *)0x1c8c));
}



void __stdcall16far FUN_000e_f2b0(int param_1,int param_2,uint param_3)

{
  byte *pbVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  int iVar6;
  undefined4 uVar7;
  int iVar8;
  undefined2 unaff_DS;
  int local_22;
  int local_12;
  int *local_10;
  int *local_c;
  
  pbVar1 = (byte *)(param_2 * 9 + (int)*(undefined4 *)0x1ddc);
  *pbVar1 = *pbVar1 & 0xfe;
  _local_c = (int *)CONCAT22(unaff_DS,(int *)(param_2 * 4 + 0x3c));
  _local_10 = (int *)CONCAT22(unaff_DS,(int *)(param_1 * 4 + 0x3c));
  iVar3 = *_local_10;
  iVar4 = *_local_c;
  uVar7 = *(undefined4 *)0x1c80;
  uVar5 = *(undefined2 *)(param_2 * 4 + 0x3e);
  iVar6 = *(int *)(param_1 * 4 + 0x3e);
  iVar8 = param_1 >> 1;
  if ((*(byte *)(param_1 * 0x14 + 0xa7) & 1) != 0) {
    local_12 = *(int *)(param_1 * 10 + 0x68);
    if (param_1 == 8) {
      local_12 = *(int *)0x68 >> 2;
    }
    if (param_1 == 9) {
      local_12 = *(int *)0x72 >> 2;
    }
    local_22 = 0x10ff;
    if (7 < param_1) {
      local_22 = 0x43f;
    }
    if ((*(byte *)(iVar8 * 0x10 + 0x18c) & 4) != 0) {
      local_22 = local_22 >> 1;
    }
    if (local_22 < (int)((local_12 - iVar6) + param_3)) {
      param_3 = (((local_12 - iVar6) + param_3) - local_22) - 1;
    }
    else if ((int)((local_12 - iVar6) + param_3) < 0) {
      param_3 = (local_12 - iVar6) + param_3 + local_22 + 1;
    }
    else {
      param_3 = (local_12 - iVar6) + param_3;
    }
    if (((*(byte *)0x20 & 4) != 0) && (*(char *)0x1b8b == '\0')) {
      param_3 = param_3 & 0xfffe;
    }
  }
  iVar6 = *_local_10;
  func_0x000fbc09();
  bVar2 = *(byte *)(iVar8 * 0x10 + 399);
  if (((bVar2 & 8) == 8) || ((bVar2 & 0xf) == 0)) {
    func_0x000fbc09(0xfbbe,(param_1 + 1) * 10 + 0x1c14);
  }
  func_0x000f750a(0xfbbe,param_1,param_2,0,(int)uVar7 + iVar4,(int)((ulong)uVar7 >> 0x10));
  if ((param_2 >> 1 != 4) && (((*"" == '\x1e' || (iVar8 != 1)) || (param_2 >> 1 != 3)))) {
    func_0x000f7603(0xf706,(int)*(undefined4 *)0x1c80 + iVar6 + param_3,
                    (int)((ulong)*(undefined4 *)0x1c80 >> 0x10),(int)*(undefined4 *)0x1c80 + iVar3,
                    (int)((ulong)*(undefined4 *)0x1c80 >> 0x10),(int)*(undefined4 *)0x1c80 + iVar4,
                    (int)((ulong)*(undefined4 *)0x1c80 >> 0x10),uVar5);
  }
  return;
}



/* copy_char_cell_template_and_sync (confidence: Mechanism confirmed)
   
   Evidence: `(dest_idx=[bp+6], src_idx=[bp+8])` - checks a per-channel config nibble
   (`[dest_idx*16+0x18F]` bits 0-3/bit 3); if not gated off, copies a `[0x34]`-byte (10-byte)
   template chunk between slots `src_idx+1` and `dest_idx+1` in the shared char-cell table at
   `[0x1C14]` via `memcpy_far`, then calls `sync_shift_register_output(dest_idx, src_idx, 0, far ptr
   [bp-0x1C])` - part of the print-record character-row-copy engine near `SUB_EF346`/`SUB_EF393`
   (called separately with arg `0` from `SUB_E8E03`, skipping that engine's position-wraparound
   preamble entirely) */

void __stdcall16far copy_char_cell_template_and_sync(void)

{
  uint uVar1;
  int unaff_BP;
  int unaff_DI;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  uVar1 = (uint)*(byte *)(unaff_DI * 0x10 + 399);
  *(uint *)((undefined *)&DAT_000e_ff02 + unaff_BP) = uVar1 & 0xf;
  if (((uVar1 & 8) == 8) || (*(int *)((undefined *)&DAT_000e_ff02 + unaff_BP) == 0)) {
    unaff_CS = 0xfbbe;
    func_0x000fbc09();
  }
  func_0x000f750a(unaff_CS,*(undefined2 *)(unaff_BP + 6),*(undefined2 *)(unaff_BP + 8),0,
                  (int)*(undefined4 *)((undefined *)&DAT_000e_ff04 + unaff_BP),
                  (int)((ulong)*(undefined4 *)((undefined *)&DAT_000e_ff04 + unaff_BP) >> 0x10));
  if ((*(int *)(unaff_BP + 8) >> 1 != 4) &&
     (((*"" == '\x1e' || (*(int *)(unaff_BP + 6) >> 1 != 1)) || (*(int *)(unaff_BP + 8) >> 1 != 3)))
     ) {
    func_0x000f7603(0xf706,(int)*(undefined4 *)0x1c80 +
                           *(int *)((undefined *)&DAT_000e_ff0e + unaff_BP),
                    (int)((ulong)*(undefined4 *)0x1c80 >> 0x10),
                    (int)*(undefined4 *)0x1c80 + *(int *)((undefined *)&DAT_000e_ff0c + unaff_BP),
                    (int)((ulong)*(undefined4 *)0x1c80 >> 0x10),
                    (int)*(undefined4 *)0x1c80 + *(int *)((undefined *)&DAT_000e_ff0a + unaff_BP),
                    (int)((ulong)*(undefined4 *)0x1c80 >> 0x10),
                    *(undefined2 *)((undefined *)&DAT_000e_ff08 + unaff_BP));
  }
  return;
}



void __stdcall16far FUN_000e_f50a(int param_1,int param_2,undefined2 param_3,undefined4 param_4)

{
  int iVar1;
  byte *pbVar2;
  undefined2 unaff_DS;
  undefined2 uVar3;
  undefined2 local_12;
  byte *local_10;
  undefined1 *local_c;
  
  pbVar2 = (byte *)(param_2 * 0x14 + 0xb0);
  _local_10 = (byte *)CONCAT22(unaff_DS,pbVar2);
  uVar3 = unaff_DS;
  func_0x000fb593();
  if ((*"" != '\x1e') && (param_2 >> 1 == 3)) {
    *_local_10 = *_local_10 & 0xfb;
  }
  iVar1 = param_2 * 0x14;
  _local_c = (undefined1 *)CONCAT22(unaff_DS,(undefined1 *)(iVar1 + 0xa6));
  local_12 = func_0x000fbc2f(0xfb59,(int)param_4,(int)((ulong)param_4 >> 0x10),pbVar2,uVar3);
  func_0x000fbc4d(0xfbc2,&local_12);
  *(undefined1 *)(iVar1 + 0xb7) = *(undefined1 *)(iVar1 + 0xb5);
  *(undefined1 *)(iVar1 + 0xb8) = *(undefined1 *)(iVar1 + 0xb6);
  if (param_2 >> 1 == 4) {
    *(undefined1 *)(iVar1 + 0xa7) = *(undefined1 *)(param_1 * 0x14 + 0xa7);
  }
  else {
    *(byte *)(iVar1 + 0xa7) = *(byte *)(iVar1 + 0xa7) & 0xfe;
  }
  *(undefined2 *)(iVar1 + 0xa8) = param_3;
  *(undefined2 *)(iVar1 + 0xac) = param_3;
  *(undefined2 *)(iVar1 + 0xaa) = param_3;
  *(undefined2 *)(iVar1 + 0xae) = param_3;
  *_local_c = 6;
  return;
}



void __stdcall16far
FUN_000e_f603(undefined1 *param_1,undefined1 *param_2,undefined1 *param_3,int param_4)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_DS;
  int local_a;
  
  local_a = 0;
  do {
    local_a = local_a + 1;
    uVar3 = (undefined2)((ulong)param_3 >> 0x10);
    puVar1 = (undefined1 *)param_3;
    *param_3 = *param_1;
    puVar1[*(int *)0x1c02] = 2;
    if (local_a != 1) {
      puVar1[*(int *)0x1c02 + -1] = *(undefined1 *)0x3b2;
    }
    uVar4 = (undefined2)((ulong)param_1 >> 0x10);
    puVar2 = (undefined1 *)param_1;
    *(byte *)0x3b2 = puVar2[*(int *)0x1c02] & 0xfb;
    if ((*(byte *)0x3b2 & 3) == 2) {
      *(undefined1 *)0x3b2 = 0;
    }
    if ((puVar2[*(int *)0x1c02 + -1] & 4) == 0) {
      param_1 = (undefined1 *)CONCAT22(uVar4,puVar2 + 1);
    }
    else {
      param_1 = param_2;
    }
    param_3 = (undefined1 *)CONCAT22(uVar3,puVar1 + 1);
  } while (local_a < param_4);
  return;
}



void __stdcall16far FUN_000e_f699(int param_1,int param_2,byte param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  undefined2 uVar5;
  undefined2 unaff_DS;
  int local_18;
  int *local_14;
  byte *local_c;
  
  _local_c = (byte *)CONCAT22(unaff_DS,(byte *)(param_1 * 0x10 + 0x18c));
  iVar3 = param_2 * 8;
  _local_14 = (int *)CONCAT22(unaff_DS,(int *)(iVar3 + 0x3c));
  iVar2 = param_1 * 8;
  if ((*_local_c & 4) == 0) {
    if ((*(byte *)(param_1 * 0x10 + 399) & 0xf) == 2) {
      *_local_14 = 0;
      *(undefined2 *)(iVar3 + 0x3e) = 0;
      _local_14 = (int *)CONCAT22(unaff_DS,(int *)(iVar3 + 0x40));
      local_18 = *(int *)(iVar2 + 0x42) >> (param_3 & 0x1f);
      if ((*"" == '\x1e') || (param_2 != 3)) {
        *_local_14 = *(int *)((int)*(undefined4 *)0x1c94 + param_2 * 0x10);
      }
      else {
        *_local_14 = 0x4444;
      }
      *(int *)(iVar3 + 0x42) = local_18;
    }
    else {
      local_18 = *(int *)(iVar2 + 0x3e) >> (param_3 & 0x1f);
    }
    if ((*"" == '\x1e') || (param_2 != 3)) {
      *_local_14 = *(int *)((int)*(undefined4 *)0x1c94 + param_2 * 0x10);
    }
    else {
      *_local_14 = 0x4444;
    }
    uVar5 = (undefined2)((ulong)_local_14 >> 0x10);
    piVar4 = (int *)_local_14;
    piVar4[1] = local_18;
    _local_14 = (int *)CONCAT22(uVar5,piVar4 + 2);
    *_local_14 = 0;
    piVar4[3] = 0;
  }
  else {
    iVar2 = *(int *)(iVar2 + 0x3e) >> (param_3 & 0x1f);
    if ((*"" == '\x1e') || (param_2 != 3)) {
      *_local_14 = *(int *)(param_2 * 0x10 + (int)*(undefined4 *)0x1c94);
    }
    else {
      *_local_14 = 0x4444;
    }
    *(int *)(iVar3 + 0x3e) = iVar2;
    iVar1 = *_local_14;
    _local_14 = (int *)CONCAT22(unaff_DS,(int *)(iVar3 + 0x40));
    *_local_14 = iVar1 + *(int *)(iVar3 + 0x3e);
    *(int *)(iVar3 + 0x42) = iVar2;
  }
  return;
}



int __stdcall16far FUN_000e_f84d(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 local_c;
  
  iVar2 = param_1 * 0x10;
  _local_c = (byte *)CONCAT22(unaff_DS,(byte *)(iVar2 + 0x18c));
  if (param_2 == 4) {
    iVar1 = 0;
  }
  else if ((*_local_c & 1) == 0) {
    iVar1 = 0;
  }
  else if ((*_local_c & 8) == 0) {
    iVar1 = *(int *)(param_1 * 0xe + 0x57c);
  }
  else {
    iVar1 = func_0x000ee15c();
    if (*(int *)(iVar2 + 0x196) < 0) {
      iVar1 = iVar1 << (-(char)*(undefined2 *)(iVar2 + 0x196) & 0x1fU);
    }
    else {
      iVar1 = iVar1 >> ((byte)*(undefined2 *)(iVar2 + 0x196) & 0x1f);
    }
  }
  return iVar1;
}



void __stdcall16far FUN_000e_f8e4(int param_1,int param_2,int param_3)

{
  uint *puVar1;
  int iVar2;
  undefined2 unaff_DS;
  undefined2 local_10;
  undefined2 local_c;
  
  param_1 = param_1 * 0x1c;
  _local_c = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(param_1 + 0x574));
  iVar2 = param_2 * 0x1c;
  _local_10 = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(iVar2 + 0x574));
  *_local_10 = *_local_c;
  *(undefined2 *)(iVar2 + 0x576) = *(undefined2 *)(param_1 + 0x576);
  *(undefined2 *)(iVar2 + 0x578) = *(undefined2 *)(param_1 + 0x578);
  *(undefined1 *)(iVar2 + 0x580) = *(undefined1 *)(param_1 + 0x580);
  *(undefined1 *)(param_2 + 0x564) = *(undefined1 *)(param_1 + 0x580);
  _local_c = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(param_1 + 0x582));
  _local_10 = (undefined2 *)CONCAT22(unaff_DS,(undefined2 *)(iVar2 + 0x582));
  *_local_10 = *_local_c;
  *(undefined2 *)(iVar2 + 0x584) = *(undefined2 *)(param_1 + 0x584);
  *(undefined2 *)(iVar2 + 0x586) = *(undefined2 *)(param_1 + 0x586);
  *(undefined1 *)(iVar2 + 0x58e) = *(undefined1 *)(param_1 + 0x58e);
  if (param_3 == 1) {
    *(byte *)(param_2 + 0x564) = *(byte *)(param_2 + 0x564) & 0xfb;
  }
  if (param_3 == 4) {
    *(byte *)(param_2 + 0x564) = *(byte *)(param_2 + 0x564) & 0xfd;
  }
  puVar1 = (uint *)(param_2 * 2 + 0x55a);
  *puVar1 = *puVar1 | 0x300;
  return;
}



void __stdcall16far FUN_000e_fc94(int param_1)

{
  byte bVar1;
  int iVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  int iVar8;
  int iVar9;
  undefined4 uVar10;
  undefined1 local_13;
  byte *local_c;
  
  iVar5 = param_1 * 2;
  _local_c = (byte *)CONCAT22(unaff_DS,(byte *)(param_1 * 0x10 + 0x18c));
  iVar2 = *(int *)(param_1 * 0x10 + 400);
  if (-1 < iVar2) {
    if (param_1 == 0) {
      local_13 = *(undefined1 *)0x1b76;
    }
    else {
      local_13 = 0xff;
    }
    bVar1 = *_local_c;
    if ((*(byte *)(iVar5 + 0x1be2) & 2) != 0) {
      iVar4 = param_1 * 0x28 + 0xb1;
      iVar8 = iVar5;
      iVar9 = iVar2;
      uVar3 = func_0x000f830e();
      uVar7 = (undefined2)((ulong)*(undefined4 *)0x1ddc >> 0x10);
      iVar6 = (int)*(undefined4 *)0x1ddc + 0x80;
      uVar10 = CONCAT22(uVar7,iVar6);
      unaff_CS = 0xfafc;
      func_0x000fb529(0xf80a,local_13,uVar3,iVar4,unaff_DS,iVar6,uVar7,iVar8,iVar9);
      if ((bVar1 & 0x20) == 0) {
        iVar6 = param_1 * 0x28 + 0xb3;
        uVar3 = func_0x000f830e(0xfafc,(int)*(undefined4 *)0x1c80,
                                (int)((ulong)*(undefined4 *)0x1c80 >> 0x10),iVar5,iVar2 + 1,uVar10,
                                iVar6);
        unaff_CS = 0xfafc;
        func_0x000fb529(0xf80a,local_13,uVar3,iVar6,unaff_DS,(int)*(undefined4 *)0x1ddc + 0x82,
                        (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
      }
    }
    if ((*(byte *)(iVar5 + 0x1be3) & 2) != 0) {
      iVar6 = (iVar5 + 1) * 0x14 + 0xb1;
      uVar3 = func_0x000f830e(unaff_CS,(int)*(undefined4 *)0x1c80,
                              (int)((ulong)*(undefined4 *)0x1c80 >> 0x10),iVar5 + 1,iVar2);
      func_0x000fb529(0xf80a,local_13,uVar3,iVar6,unaff_DS,(int)*(undefined4 *)0x1ddc + 0x92,
                      (int)((ulong)*(undefined4 *)0x1ddc >> 0x10));
    }
  }
  return;
}



/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Removing unreachable block (ram,0x00010004) */

void __stdcall16far FUN_000e_fe2a(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}



void __stdcall16far FUN_000e_ff2a(byte param_1)

{
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  if (*(int *)0x570 == 0) {
    if ((param_1 & 1) == 0) {
      if (*"" != '\0') {
        unaff_CS = 0xf7bf;
        func_0x000f802b();
      }
      if (*(char *)0x1c25 != '\0') {
        func_0x000f802b(unaff_CS,1);
      }
    }
    else {
      func_0x000f7f72();
    }
  }
  return;
}



/* WARNING: Control flow encountered bad instruction data */

void FUN_000e_ff72(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


