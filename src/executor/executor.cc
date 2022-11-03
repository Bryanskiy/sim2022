#include "executor/executor.hh"

namespace sim {

static auto applyOffset(RegVal val, RegVal imm) {
  int64_t res = static_cast<int64_t>(val) + signCast(imm);
  return static_cast<RegVal>(res);
}

static void executeADD(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 + rs2);
}

static void executeSUB(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 - rs2);
}

static void executeMUL(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 * rs2);
}

static void executeDIV(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 / rs2);
}

static void executeLW(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal word = state.mem.loadWord(applyOffset(rs1, inst.imm));
  state.regs.set(inst.rd, word);
}

static void executeSW(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.mem.storeWord(applyOffset(rs1, inst.imm), rs2);
}

static void executeJAL(const Instruction &inst, State &state) {
  state.branchIsTaken = true;
  state.regs.set(inst.rd, state.pc + kXLENInBytes);
  state.npc = applyOffset(state.pc, inst.imm);
}

static void executeJALR(const Instruction &inst, State &state) {
  state.branchIsTaken = true;
  state.regs.set(inst.rd, state.pc + kXLENInBytes);
  RegVal rs1 = state.regs.get(inst.rs1);
  state.npc =
      setBit<0, 0>(applyOffset(rs1, inst.imm)); // setting the least-significant
                                                // bit of the result to zero.
}

Executor::Executor()
    : executors{{OpType::ADD, executeADD}, {OpType::SUB, executeSUB},
                {OpType::MUL, executeMUL}, {OpType::DIV, executeDIV},
                {OpType::LW, executeLW},   {OpType::SW, executeSW},
                {OpType::JAL, executeJAL}, {OpType::JALR, executeJALR}} {}

void Executor::execute(const Instruction &inst, State &state) const {
  executors.at(inst.type)(inst, state);
}

} // namespace sim
