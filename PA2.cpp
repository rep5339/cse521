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
#include "string"
#include "llvm/IR/Instruction.h"

using namespace llvm;
using namespace std;

namespace {
  struct PA2 : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    int loopcounter;
    PA2() : FunctionPass(ID) {}

    void handleLoop(Loop *L, Function &F) {
      // init variables
      int start_loopcounter = loopcounter;
      bool has_subloops = false;
      int depth =  L -> getLoopDepth() - 1; 
      int bb_count = 0;
      int i_count = 0;
      int br_count = 0;
      int at_count = 0;
      // recursively execute on nested loops
      for (Loop *SL : L -> getSubLoops()) {
	handleLoop(SL, F);
      }
      // check if it has recursively entered any sub loops
      has_subloops = start_loopcounter != loopcounter;
      loopcounter++;
      
      // iterate over all basic blocks
      for (auto Iter = L->block_begin(); Iter != L->block_end(); Iter++){
	      LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
	      // Check if basic block is in a more inner loop
	      if (L == LI.getLoopFor((*Iter))){
                // tracking bb and instr counts
  	        i_count += (*Iter)->size();
	        bb_count++;
		
		// track branch and atomic count
	        for( auto Inst = (*Iter)->begin(); Inst != (*Iter)->end(); Inst++){
	          if (strcmp((Inst-> getOpcodeName()), "br") == 0){ br_count++;}
		  else if (strcmp((Inst ->getOpcodeName()), "cmpxchg") == 0) {at_count++;}
		  else if (strcmp((Inst -> getOpcodeName()), "atomicrmw") == 0) {at_count++;}
	        }
	      } 

      }

      // convert bool to string
      string subloops = has_subloops ? "yes" : "no"; 

      
      //print loop info here
      errs() << loopcounter << ": func=" << F.getName() << ", depth=" << depth  <<", subloops=" << subloops << ", BBs=" << bb_count << ", instrs=" << i_count << ", atomics=" << at_count << ", branches=" << br_count <<"\n";
    }

    virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
      AU.addRequired<LoopInfoWrapperPass>(); 
    }

    bool doInitialization(Module &M) override {
      loopcounter = 0;
      return false;
    }
    bool runOnFunction(Function &F) override {
	LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
	for (LoopInfo::iterator LIT = LI.begin(); LIT != LI.end(); LIT++){
          handleLoop(*LIT, F);
	}
      
      return false;
    }
  };
}

char PA2::ID = 0;
static RegisterPass<PA2> X("pa2", "Loop Pass");


