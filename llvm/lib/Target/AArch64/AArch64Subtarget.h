//===--- AArch64Subtarget.h - Define Subtarget for the AArch64 -*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the AArch64 specific subclass of TargetSubtarget.
//
//===----------------------------------------------------------------------===//

#ifndef AArch64SUBTARGET_H
#define AArch64SUBTARGET_H

#include "AArch64FrameLowering.h"
#include "AArch64ISelLowering.h"
#include "AArch64InstrInfo.h"
#include "AArch64RegisterInfo.h"
#include "AArch64SelectionDAGInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetSubtargetInfo.h"
#include <string>

#define GET_SUBTARGETINFO_HEADER
#include "AArch64GenSubtargetInfo.inc"

namespace llvm {
class GlobalValue;
class StringRef;

class AArch64Subtarget : public AArch64GenSubtargetInfo {
protected:
  enum ARMProcFamilyEnum {Others, CortexA53, CortexA57, Cyclone};

  /// ARMProcFamily - ARM processor family: Cortex-A53, Cortex-A57, and others.
  ARMProcFamilyEnum ARMProcFamily;

  bool HasFPARMv8;
  bool HasNEON;
  bool HasCrypto;
  bool HasCRC;

  // HasZeroCycleRegMove - Has zero-cycle register mov instructions.
  bool HasZeroCycleRegMove;

  // HasZeroCycleZeroing - Has zero-cycle zeroing instructions.
  bool HasZeroCycleZeroing;

  /// CPUString - String name of used CPU.
  std::string CPUString;

  /// TargetTriple - What processor and OS we're targeting.
  Triple TargetTriple;

  const DataLayout DL;
  AArch64FrameLowering FrameLowering;
  AArch64InstrInfo InstrInfo;
  AArch64SelectionDAGInfo TSInfo;
  AArch64TargetLowering TLInfo;
private:
  /// initializeSubtargetDependencies - Initializes using CPUString and the
  /// passed in feature string so that we can use initializer lists for
  /// subtarget initialization.
  AArch64Subtarget &initializeSubtargetDependencies(StringRef FS);

public:
  /// This constructor initializes the data members to match that
  /// of the specified triple.
  AArch64Subtarget(const std::string &TT, const std::string &CPU,
		   const std::string &FS, TargetMachine &TM, bool LittleEndian);

  const AArch64SelectionDAGInfo *getSelectionDAGInfo() const override {
    return &TSInfo;
  }
  const AArch64FrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }
  const AArch64TargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }
  const AArch64InstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const DataLayout *getDataLayout() const override { return &DL; }
  const AArch64RegisterInfo *getRegisterInfo() const override {
    return &getInstrInfo()->getRegisterInfo();
  }
  bool enableMachineScheduler() const override { return true; }

  bool hasZeroCycleRegMove() const { return HasZeroCycleRegMove; }

  bool hasZeroCycleZeroing() const { return HasZeroCycleZeroing; }

  bool hasFPARMv8() const { return HasFPARMv8; }
  bool hasNEON() const { return HasNEON; }
  bool hasCrypto() const { return HasCrypto; }
  bool hasCRC() const { return HasCRC; }

  bool isLittleEndian() const { return DL.isLittleEndian(); }

  bool isTargetDarwin() const { return TargetTriple.isOSDarwin(); }

  bool isTargetELF() const { return TargetTriple.isOSBinFormatELF(); }

  bool isTargetMachO() const { return TargetTriple.isOSBinFormatMachO(); }

  bool isCyclone() const { return CPUString == "cyclone"; }
  bool isCortexA57() const { return CPUString == "cortex-a57"; }
  bool isCortexA53() const { return CPUString == "cortex-a53"; }

  /// getMaxInlineSizeThreshold - Returns the maximum memset / memcpy size
  /// that still makes it profitable to inline the call.
  unsigned getMaxInlineSizeThreshold() const { return 64; }

  /// ParseSubtargetFeatures - Parses features string setting specified
  /// subtarget options.  Definition of function is auto generated by tblgen.
  void ParseSubtargetFeatures(StringRef CPU, StringRef FS);

  /// ClassifyGlobalReference - Find the target operand flags that describe
  /// how a global value should be referenced for the current subtarget.
  unsigned char ClassifyGlobalReference(const GlobalValue *GV,
                                        const TargetMachine &TM) const;

  /// This function returns the name of a function which has an interface
  /// like the non-standard bzero function, if such a function exists on
  /// the current subtarget and it is considered prefereable over
  /// memset with zero passed as the second argument. Otherwise it
  /// returns null.
  const char *getBZeroEntry() const;

  void overrideSchedPolicy(MachineSchedPolicy &Policy, MachineInstr *begin,
                           MachineInstr *end,
                           unsigned NumRegionInstrs) const override;

  bool enableEarlyIfConversion() const override;
};
} // End llvm namespace

#endif // AArch64SUBTARGET_H
