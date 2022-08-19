/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "fbpcf/scheduler/gate_keeper/GateKeeper.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include "fbpcf/scheduler/IScheduler.h"
#include "fbpcf/scheduler/gate_keeper/ArithmeticGate.h"
#include "fbpcf/scheduler/gate_keeper/BatchArithmeticGate.h"
#include "fbpcf/scheduler/gate_keeper/BatchCompositeGate.h"
#include "fbpcf/scheduler/gate_keeper/BatchNormalGate.h"
#include "fbpcf/scheduler/gate_keeper/CompositeGate.h"
#include "fbpcf/scheduler/gate_keeper/IArithmeticGate.h"
#include "fbpcf/scheduler/gate_keeper/IGate.h"
#include "fbpcf/scheduler/gate_keeper/INormalGate.h"
#include "fbpcf/scheduler/gate_keeper/NormalGate.h"

namespace fbpcf::scheduler {
GateKeeper::GateKeeper(std::shared_ptr<IWireKeeper> wireKeeper)
    : wireKeeper_{wireKeeper} {}

IScheduler::WireId<IScheduler::Boolean> GateKeeper::inputGate(
    BoolType<false> initialValue) {
  auto level = getOutputLevel(
      GateClass<false>::isFree(INormalGate::GateType::Input),
      firstUnexecutedLevel_);
  auto outputWire = allocateNewWire(initialValue, level);
  addGate(
      std::make_unique<NormalGate>(
          INormalGate::GateType::Input,
          outputWire,
          IScheduler::WireId<IScheduler::Boolean>(),
          IScheduler::WireId<IScheduler::Boolean>(),
          0,
          *wireKeeper_),
      level);
  return outputWire;
}

IScheduler::WireId<IScheduler::Arithmetic> GateKeeper::inputGate(
    IntType<false> initialValue) {
  auto level = getOutputLevel(
      IArithmeticGate::isFree(IArithmeticGate::GateType::Input),
      firstUnexecutedLevel_);
  auto outputWire = allocateNewWire(initialValue, level);
  addGate(
      std::make_unique<ArithmeticGate>(
          IArithmeticGate::GateType::Input,
          outputWire,
          IScheduler::WireId<IScheduler::Arithmetic>(),
          IScheduler::WireId<IScheduler::Arithmetic>(),
          0,
          *wireKeeper_),
      level);
  return outputWire;
}

IScheduler::WireId<IScheduler::Boolean> GateKeeper::inputGateBatch(
    BoolType<true> initialValue) {
  auto size = initialValue.size();
  auto level = getOutputLevel(
      GateClass<false>::isFree(INormalGate::GateType::Input),
      firstUnexecutedLevel_);

  auto outputWire = allocateNewWire(initialValue, level, size);
  addGate(
      std::make_unique<BatchNormalGate>(
          INormalGate::GateType::Input,
          outputWire,
          IScheduler::WireId<IScheduler::Boolean>(),
          IScheduler::WireId<IScheduler::Boolean>(),
          0,
          size,
          *wireKeeper_),
      level);
  return outputWire;
}

IScheduler::WireId<IScheduler::Arithmetic> GateKeeper::inputGateBatch(
    IntType<true> initialValue) {
  auto size = initialValue.size();
  auto level = getOutputLevel(
      IArithmeticGate::isFree(IArithmeticGate::GateType::Input),
      firstUnexecutedLevel_);

  auto outputWire = allocateNewWire(initialValue, level, size);
  addGate(
      std::make_unique<BatchArithmeticGate>(
          IArithmeticGate::GateType::Input,
          outputWire,
          IScheduler::WireId<IScheduler::Arithmetic>(),
          IScheduler::WireId<IScheduler::Arithmetic>(),
          0,
          size,
          *wireKeeper_),
      level);
  return outputWire;
}

IScheduler::WireId<IScheduler::Boolean> GateKeeper::outputGate(
    IScheduler::WireId<IScheduler::Boolean> src,
    int partyID) {
  auto level = getOutputLevel(
      GateClass<false>::isFree(INormalGate::GateType::Output),
      getMaxLevel<false, IScheduler::Boolean>(src));
  auto outputWire = allocateNewWire(false, level);

  addGate(
      std::make_unique<NormalGate>(
          INormalGate::GateType::Output,
          outputWire,
          src,
          IScheduler::WireId<IScheduler::Boolean>(),
          partyID,
          *wireKeeper_),
      level);

  return outputWire;
}

IScheduler::WireId<IScheduler::Arithmetic> GateKeeper::outputGate(
    IScheduler::WireId<IScheduler::Arithmetic> src,
    int partyID) {
  auto level = getOutputLevel(
      IArithmeticGate::isFree(IArithmeticGate::GateType::Output),
      getMaxLevel<false, IScheduler::Arithmetic>(src));
  auto outputWire = allocateNewWire((uint64_t)0, level);

  addGate(
      std::make_unique<ArithmeticGate>(
          IArithmeticGate::GateType::Output,
          outputWire,
          src,
          IScheduler::WireId<IScheduler::Arithmetic>(),
          partyID,
          *wireKeeper_),
      level);

  return outputWire;
}

IScheduler::WireId<IScheduler::Boolean> GateKeeper::outputGateBatch(
    IScheduler::WireId<IScheduler::Boolean> src,
    int partyID) {
  auto level = getOutputLevel(
      GateClass<false>::isFree(INormalGate::GateType::Output),
      getMaxLevel<true, IScheduler::Boolean>(src));
  auto expectedBatchSize = wireKeeper_->getBatchSize(src);
  auto outputWire =
      allocateNewWire(std::vector<bool>(), level, expectedBatchSize);

  addGate(
      std::make_unique<BatchNormalGate>(
          INormalGate::GateType::Output,
          outputWire,
          src,
          IScheduler::WireId<IScheduler::Boolean>(),
          partyID,
          expectedBatchSize,
          *wireKeeper_),
      level);

  return outputWire;
}

IScheduler::WireId<IScheduler::Arithmetic> GateKeeper::outputGateBatch(
    IScheduler::WireId<IScheduler::Arithmetic> src,
    int partyID) {
  auto level = getOutputLevel(
      IArithmeticGate::isFree(IArithmeticGate::GateType::Output),
      getMaxLevel<true, IScheduler::Arithmetic>(src));
  auto expectedBatchSize = wireKeeper_->getBatchSize(src);
  auto outputWire =
      allocateNewWire(std::vector<uint64_t>(), level, expectedBatchSize);

  addGate(
      std::make_unique<BatchArithmeticGate>(
          IArithmeticGate::GateType::Output,
          outputWire,
          src,
          IScheduler::WireId<IScheduler::Arithmetic>(),
          partyID,
          expectedBatchSize,
          *wireKeeper_),
      level);

  return outputWire;
}

IScheduler::WireId<IScheduler::Boolean> GateKeeper::normalGate(
    INormalGate::GateType gateType,
    IScheduler::WireId<IScheduler::Boolean> left,
    IScheduler::WireId<IScheduler::Boolean> right) {
  auto level = getOutputLevel(
      GateClass<false>::isFree(gateType),
      std::max(
          getMaxLevel<false, IScheduler::Boolean>(left),
          getMaxLevel<false, IScheduler::Boolean>(right)));
  auto outputWire = allocateNewWire(false, level);

  addGate(
      std::make_unique<NormalGate>(
          gateType, outputWire, left, right, 0, *wireKeeper_),
      level);

  return outputWire;
}

IScheduler::WireId<IScheduler::Boolean> GateKeeper::normalGateBatch(
    INormalGate::GateType gateType,
    IScheduler::WireId<IScheduler::Boolean> left,
    IScheduler::WireId<IScheduler::Boolean> right) {
  auto level = getOutputLevel(
      GateClass<false>::isFree(gateType),
      std::max(
          getMaxLevel<true, IScheduler::Boolean>(left),
          getMaxLevel<true, IScheduler::Boolean>(right)));
  auto expectedBatchSize = wireKeeper_->getBatchSize(left);
  auto outputWire =
      allocateNewWire(std::vector<bool>(), level, expectedBatchSize);

  addGate(
      std::make_unique<BatchNormalGate>(
          gateType,
          outputWire,
          left,
          right,
          0,
          expectedBatchSize,
          *wireKeeper_),
      level);

  return outputWire;
}

IScheduler::WireId<IScheduler::Arithmetic> GateKeeper::arithmeticGate(
    IArithmeticGate::GateType gateType,
    IScheduler::WireId<IScheduler::Arithmetic> left,
    IScheduler::WireId<IScheduler::Arithmetic> right) {
  auto level = getOutputLevel(
      IArithmeticGate::isFree(gateType),
      std::max(
          getMaxLevel<false, IScheduler::Arithmetic>(left),
          getMaxLevel<false, IScheduler::Arithmetic>(right)));
  auto outputWire = allocateNewWire((uint64_t)0, level);

  addGate(
      std::make_unique<ArithmeticGate>(
          gateType, outputWire, left, right, 0, *wireKeeper_),
      level);

  return outputWire;
}

IScheduler::WireId<IScheduler::Arithmetic> GateKeeper::arithmeticGateBatch(
    IArithmeticGate::GateType gateType,
    IScheduler::WireId<IScheduler::Arithmetic> left,
    IScheduler::WireId<IScheduler::Arithmetic> right) {
  auto level = getOutputLevel(
      IArithmeticGate::isFree(gateType),
      std::max(
          getMaxLevel<true, IScheduler::Arithmetic>(left),
          getMaxLevel<true, IScheduler::Arithmetic>(right)));
  auto expectedBatchSize = wireKeeper_->getBatchSize(left);
  auto outputWire =
      allocateNewWire(std::vector<uint64_t>(), level, expectedBatchSize);

  addGate(
      std::make_unique<BatchArithmeticGate>(
          gateType,
          outputWire,
          left,
          right,
          0,
          expectedBatchSize,
          *wireKeeper_),
      level);

  return outputWire;
}

std::vector<IScheduler::WireId<IScheduler::Boolean>> GateKeeper::compositeGate(
    ICompositeGate::GateType gateType,
    IScheduler::WireId<IScheduler::Boolean> left,
    std::vector<IScheduler::WireId<IScheduler::Boolean>> rights) {
  auto compositeSize = rights.size();
  std::vector<IScheduler::WireId<IScheduler::Boolean>> outputWires(
      compositeSize);
  auto level = getOutputLevel(
      GateClass<true>::isFree(gateType),
      std::max(
          getMaxLevel<false, IScheduler::Boolean>(left),
          getMaxLevel<false>(rights)));
  for (size_t i = 0; i < compositeSize; i++) {
    outputWires[i] = allocateNewWire(false, level);
  }

  addGate(
      std::make_unique<CompositeGate>(
          gateType, outputWires, left, rights, *wireKeeper_),
      level);

  return outputWires;
}

std::vector<IScheduler::WireId<IScheduler::Boolean>>
GateKeeper::compositeGateBatch(
    ICompositeGate::GateType gateType,
    IScheduler::WireId<IScheduler::Boolean> left,
    std::vector<IScheduler::WireId<IScheduler::Boolean>> rights) {
  auto compositeSize = rights.size();
  std::vector<IScheduler::WireId<IScheduler::Boolean>> outputWires(
      compositeSize);
  auto level = getOutputLevel(
      GateClass<true>::isFree(gateType),
      std::max(
          getMaxLevel<true, IScheduler::Boolean>(left),
          getMaxLevel<true>(rights)));
  auto expectedBatchSize = wireKeeper_->getBatchSize(left);
  for (size_t i = 0; i < compositeSize; i++) {
    outputWires[i] =
        allocateNewWire(std::vector<bool>(), level, expectedBatchSize);
  }

  addGate(
      std::make_unique<BatchCompositeGate>(
          gateType, outputWires, left, rights, *wireKeeper_),
      level);

  return outputWires;
}

// band a number of boolean batches into one batch.
IScheduler::WireId<IScheduler::Boolean> GateKeeper::batchingUp(
    std::vector<IScheduler::WireId<IScheduler::Boolean>> src) {
  auto level = getOutputLevel(true, getMaxLevel<true>(src));
  uint32_t batchSize = 0;
  for (auto& item : src) {
    batchSize += wireKeeper_->getBatchBooleanValue(item).size();
  }
  auto outputWire = allocateNewWire(std::vector<bool>(), level, batchSize);
  addGate(
      std::make_unique<RebatchingBooleanGate>(src, outputWire, *wireKeeper_),
      level);
  return outputWire;
}

// decompose a batch of boolean values into several smaller batches.
std::vector<IScheduler::WireId<IScheduler::Boolean>> GateKeeper::unbatching(
    IScheduler::WireId<IScheduler::Boolean> src,
    std::shared_ptr<std::vector<uint32_t>> unbatchingStrategy) {
  auto level =
      getOutputLevel(true, wireKeeper_->getBatchFirstAvailableLevel(src));
  std::vector<IScheduler::WireId<IScheduler::Boolean>> outputWires(
      unbatchingStrategy->size());
  for (size_t i = 0; i < outputWires.size(); i++) {
    outputWires[i] =
        allocateNewWire(std::vector<bool>(), level, unbatchingStrategy->at(i));
  }
  addGate(
      std::make_unique<RebatchingBooleanGate>(
          src, outputWires, *wireKeeper_, unbatchingStrategy),
      level);
  return outputWires;
}

uint32_t GateKeeper::getFirstUnexecutedLevel() const {
  return firstUnexecutedLevel_;
}

std::vector<std::unique_ptr<IGate>> GateKeeper::popFirstUnexecutedLevel() {
  auto gates = std::move(gatesByLevelOffset_.front());
  gatesByLevelOffset_.pop_front();
  ++firstUnexecutedLevel_;
  numUnexecutedGates_ -= gates.size();
  return gates;
}

bool GateKeeper::hasReachedBatchingLimit() const {
  return numUnexecutedGates_ > kMaxUnexecutedGates;
}

} // namespace fbpcf::scheduler
