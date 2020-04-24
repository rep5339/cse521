//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
//#include "llvm/IR/GlobalVariable.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

namespace {
  struct PA2 : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    int loopcounter;
    PA2() : ModulePass(ID) {}

    void handleLoop(Loop *L) {
      ++loopcounter;
      for (Loop *SL : L -> getSubLoops()) {
	handleLoop(SL);
      }
    }

    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
      AU.addRequired<LoopInfoWrapperPass>();
      //AU.addRequired<LoopInfo>(); 
    }

    bool runOnModule(Module &M) override {
      loopcounter = 0;
      for (Module::iterator f =M.begin(); f != M.end(); f++){
	LoopInfo *LI;
        LI = &getAnalysis<LoopInfo>();
	//LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
	//for (LoopInfo::iterator LIT = LI.begin(); LIT != LI.end(); LIT++){
	for(LoopInfo::iterator LIT = LI -> begin(); LIT != LI -> end(); LIT++){
          handleLoop(*LIT);
	}
      }
      errs() << "Found " << loopcounter << "\n";
      return false;
    }
  };
}

char PA2::ID = 0;
static RegisterPass<PA2> X("pa2", "Loop Pass");


