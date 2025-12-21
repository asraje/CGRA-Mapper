/*
 * ======================================================================
 * mapperPass.cpp
 * ======================================================================
 * Mapper pass implementation.
 *
 * Author : Cheng Tan
 * Date   : Aug 16, 2021
 * Updated for LLVM 21 New Pass Manager
 */

#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopAnalysisManager.h>

#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>

#include "json.hpp"
#include "Mapper.h"

// Workaround for LLVM opcode offset issue
int testing_opcode_offset = 0;

// Explicit alias to avoid collision with llvm::json
using NlohmannJson = nlohmann::json;

// Definition of addDefaultKernels - adds default kernel names to look for
void addDefaultKernels(std::map<std::string, std::list<int>*>* functionWithLoop) {
  // This can be extended to add default kernel names if needed
  // For now, kernels are specified via param.json
}

// Helper function to collect target loops from LoopInfo
// Helper function to check if a loop contains vectorized instructions
bool loopContainsVectorOps(llvm::Loop* L) {
  for (llvm::BasicBlock* BB : L->blocks()) {
    for (llvm::Instruction& I : *BB) {
      // Check if the instruction result type is a vector
      if (I.getType()->isVectorTy()) {
        return true;
      }
      // Check if any operand is a vector type (for stores, etc.)
      for (unsigned i = 0; i < I.getNumOperands(); ++i) {
        if (I.getOperand(i)->getType()->isVectorTy()) {
          return true;
        }
      }
    }
  }
  return false;
}

std::list<llvm::Loop*>* getTargetLoops(
    llvm::Function& F,
    llvm::LoopInfo& LI,
    std::map<std::string, std::list<int>*>* functionWithLoop,
    bool targetNested) {

  auto targetLoops = new std::list<llvm::Loop*>();
  std::string funcName = F.getName().str();

  if (functionWithLoop->find(funcName) == functionWithLoop->end()) {
    return targetLoops;
  }

  std::list<int>* targetLoopIDs = (*functionWithLoop)[funcName];

  // Collect all loops in the function, separating vectorized and scalar loops
  std::vector<llvm::Loop*> vectorizedLoops;
  std::vector<llvm::Loop*> scalarLoops;

  for (llvm::Loop* L : LI) {
    if (loopContainsVectorOps(L)) {
      vectorizedLoops.push_back(L);
    } else {
      scalarLoops.push_back(L);
    }
    if (targetNested) {
      // Also collect nested loops
      for (llvm::Loop* SubLoop : L->getSubLoops()) {
        if (loopContainsVectorOps(SubLoop)) {
          vectorizedLoops.push_back(SubLoop);
        } else {
          scalarLoops.push_back(SubLoop);
        }
      }
    }
  }

  // Prefer vectorized loops over scalar loops
  // Build combined list with vectorized loops first
  std::vector<llvm::Loop*> allLoops;
  for (llvm::Loop* L : vectorizedLoops) {
    allLoops.push_back(L);
  }
  for (llvm::Loop* L : scalarLoops) {
    allLoops.push_back(L);
  }

  // Select loops by ID
  int loopIdx = 0;
  for (llvm::Loop* L : allLoops) {
    for (int targetID : *targetLoopIDs) {
      if (loopIdx == targetID) {
        targetLoops->push_back(L);
        std::cout << "[Loop Selection] Selected loop " << loopIdx
                  << " (vectorized: " << (loopContainsVectorOps(L) ? "yes" : "no") << ")\n";
        break;
      }
    }
    loopIdx++;
  }

  // If no specific loops selected, prefer vectorized loops
  if (targetLoops->empty()) {
    if (!vectorizedLoops.empty()) {
      // Add vectorized loops first
      for (llvm::Loop* L : vectorizedLoops) {
        targetLoops->push_back(L);
        std::cout << "[Loop Selection] Auto-selected vectorized loop\n";
      }
    } else if (!scalarLoops.empty()) {
      // Fall back to scalar loops
      for (llvm::Loop* L : scalarLoops) {
        targetLoops->push_back(L);
        std::cout << "[Loop Selection] Auto-selected scalar loop (no vectorized loops found)\n";
      }
    }
  }

  return targetLoops;
}

namespace {

class MapperPass : public llvm::PassInfoMixin<MapperPass> {
public:
  Mapper* mapper = nullptr;

  llvm::PreservedAnalyses
  run(llvm::Function& F, llvm::FunctionAnalysisManager& FAM) {

    llvm::LoopInfo& LI = FAM.getResult<llvm::LoopAnalysis>(F);

    // ---------------- Configuration defaults ----------------
    int rows = 4;
    int columns = 4;
    bool targetEntireFunction = false;
    bool targetNested = false;
    bool doCGRAMapping = true;
    bool isStaticElasticCGRA = false;
    bool isTrimmedDemo = true;
    int ctrlMemConstraint = 200;
    int bypassConstraint = 4;
    int regConstraint = 8;
    bool precisionAware = false;
    std::string vectorizationMode = "all";
    bool heuristicMapping = true;
    bool parameterizableCGRA = false;

    bool incrementalMapping = false;
    bool supportDVFS = false;
    bool DVFSAwareMapping = false;
    int DVFSIslandDim = 2;
    bool enablePowerGating = false;
    bool enableExpandableMapping = false;

    int vectorFactorForIdiv = 1;
    std::string multiCycleStrategy = "exclusive";

    auto execLatency     = new std::map<std::string, int>();
    auto pipelinedOpt    = new std::list<std::string>();
    auto fusionStrategy  = new std::list<std::string>();
    auto additionalFunc  = new std::map<std::string, std::list<int>*>();
    auto fusionPattern   = new std::map<std::string, std::list<std::string>*>();

    auto functionWithLoop = new std::map<std::string, std::list<int>*>();
    addDefaultKernels(functionWithLoop);

    // ---------------- JSON parsing ----------------
    std::ifstream i("./param.json");
    if (i.good()) {
      NlohmannJson param;
      i >> param;

      try {
        for (const auto& key : {
          "row","column","targetFunction","kernel","targetNested",
          "targetLoopsID","isTrimmedDemo","doCGRAMapping",
          "isStaticElasticCGRA","ctrlMemConstraint",
          "bypassConstraint","regConstraint","precisionAware",
          "vectorizationMode","fusionStrategy",
          "heuristicMapping","parameterizableCGRA"
        }) {
          param.at(key);
        }
      } catch (const NlohmannJson::out_of_range& e) {
        std::cerr << "Missing parameter in param.json: " << e.what() << "\n";
        std::exit(1);
      }

      (*functionWithLoop)[param["kernel"]] = new std::list<int>();
      NlohmannJson loops = param["targetLoopsID"];
      for (size_t idx = 0; idx < loops.size(); ++idx)
        (*functionWithLoop)[param["kernel"]]->push_back(loops[idx]);

      rows                 = param["row"];
      columns              = param["column"];
      targetEntireFunction = param["targetFunction"];
      targetNested         = param["targetNested"];
      doCGRAMapping        = param["doCGRAMapping"];
      isStaticElasticCGRA  = param["isStaticElasticCGRA"];
      isTrimmedDemo        = param["isTrimmedDemo"];
      ctrlMemConstraint    = param["ctrlMemConstraint"];
      bypassConstraint     = param["bypassConstraint"];
      regConstraint        = param["regConstraint"];
      precisionAware       = param["precisionAware"];
      vectorizationMode    = param["vectorizationMode"];
      heuristicMapping     = param["heuristicMapping"];
      parameterizableCGRA  = param["parameterizableCGRA"];
    }

    if (functionWithLoop->find(F.getName().str()) ==
        functionWithLoop->end()) {
      return llvm::PreservedAnalyses::all();
    }

    auto targetLoops =
      getTargetLoops(F, LI, functionWithLoop, targetNested);

    auto dfg = new DFG(
      F, targetLoops, targetEntireFunction, precisionAware,
      fusionStrategy, execLatency, pipelinedOpt, fusionPattern,
      supportDVFS, DVFSAwareMapping, vectorFactorForIdiv,
      multiCycleStrategy == "distributed"
    );

    auto cgra = new CGRA(
      rows, columns, vectorizationMode, fusionStrategy,
      parameterizableCGRA, additionalFunc, supportDVFS,
      DVFSIslandDim, multiCycleStrategy == "inclusive"
    );

    cgra->setRegConstraint(regConstraint);
    cgra->setCtrlMemConstraint(ctrlMemConstraint);
    cgra->setBypassConstraint(bypassConstraint);

    // Generate DFG visualization
    dfg->generateDot(F, isTrimmedDemo);
    dfg->generateJSON();

    std::cout << "DFG generated with " << dfg->getNodeCount() << " nodes\n";
    std::cout << "Output files: " << F.getName().str() << ".dot and dfg.json\n";

    mapper = new Mapper(DVFSAwareMapping);

    return llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK
::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION,
    "MapperPass",
    LLVM_VERSION_STRING,
    [](llvm::PassBuilder& PB) {
      PB.registerPipelineParsingCallback(
        [](llvm::StringRef Name,
           llvm::FunctionPassManager& FPM,
           llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
          if (Name == "mapperPass") {
            FPM.addPass(MapperPass());
            return true;
          }
          return false;
        }
      );
    }
  };
}
