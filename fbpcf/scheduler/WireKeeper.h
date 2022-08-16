/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <stdint.h>
#include <cassert>
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "fbpcf/scheduler/IAllocator.h"
#include "fbpcf/scheduler/IWireKeeper.h"
#include "fbpcf/scheduler/UnorderedMapAllocator.h"
#include "fbpcf/scheduler/VectorArenaAllocator.h"

namespace fbpcf::scheduler {

class WireKeeper final : public IWireKeeper {
 public:
  WireKeeper(
      std::unique_ptr<IAllocator<WireRecord<bool, false>>> boolAllocator,
      std::unique_ptr<IAllocator<WireRecord<std::vector<bool>, true>>>
          boolBatchAllocator,
      std::unique_ptr<IAllocator<WireRecord<uint64_t, false>>> intAllocator,
      std::unique_ptr<IAllocator<WireRecord<std::vector<uint64_t>, true>>>
          intBatchAllocator_)
      : boolAllocator_{std::move(boolAllocator)},
        boolBatchAllocator_{std::move(boolBatchAllocator)},
        intAllocator_{std::move(intAllocator)},
        intBatchAllocator_{std::move(intBatchAllocator_)} {}

  template <bool unsafe>
  static std::unique_ptr<IWireKeeper> createWithVectorArena() {
    return std::make_unique<WireKeeper>(
        std::make_unique<
            VectorArenaAllocator<WireRecord<bool, false>, unsafe>>(),
        std::make_unique<VectorArenaAllocator<
            WireRecord<std::vector<bool>, true>,
            unsafe>>(),
        std::make_unique<
            VectorArenaAllocator<WireRecord<uint64_t, false>, unsafe>>(),
        std::make_unique<VectorArenaAllocator<
            WireRecord<std::vector<uint64_t>, true>,
            unsafe>>());
  }

  static std::unique_ptr<IWireKeeper> createWithUnorderedMap() {
    return std::make_unique<WireKeeper>(
        std::make_unique<UnorderedMapAllocator<WireRecord<bool, false>>>(),
        std::make_unique<
            UnorderedMapAllocator<WireRecord<std::vector<bool>, true>>>(),
        std::make_unique<UnorderedMapAllocator<WireRecord<uint64_t, false>>>(),
        std::make_unique<
            UnorderedMapAllocator<WireRecord<std::vector<uint64_t>, true>>>());
  }

  /**
   * @inherit doc
   */
  IScheduler::WireId<IScheduler::Boolean> allocateBooleanValue(
      bool v = 0,
      uint32_t firstAvailableLevel = 0) override;

  /**
   * @inherit doc
   */
  IScheduler::WireId<IScheduler::Arithmetic> allocateIntegerValue(
      uint64_t v = 0,
      uint32_t firstAvailableLevel = 0) override;

  /**
   * @inherit doc
   */
  bool getBooleanValue(
      IScheduler::WireId<IScheduler::Boolean> id) const override;

  /**
   * @inherit doc
   */
  uint64_t getIntegerValue(
      IScheduler::WireId<IScheduler::Arithmetic> id) const override;

  /**
   * @inherit doc
   */
  void setBooleanValue(IScheduler::WireId<IScheduler::Boolean> id, bool v)
      override;

  /**
   * @inherit doc
   */
  void setIntegerValue(
      IScheduler::WireId<IScheduler::Arithmetic> id,
      uint64_t v) override;

  /**
   * @inherit doc
   */
  uint32_t getFirstAvailableLevel(
      IScheduler::WireId<IScheduler::Boolean> id) const override;

  /**
   * @inherit doc
   */
  uint32_t getFirstAvailableLevel(
      IScheduler::WireId<IScheduler::Arithmetic> id) const override;

  /**
   * @inherit doc
   */
  void setFirstAvailableLevel(
      IScheduler::WireId<IScheduler::Boolean> id,
      uint32_t level) override;

  /**
   * @inherit doc
   */
  void setFirstAvailableLevel(
      IScheduler::WireId<IScheduler::Arithmetic> id,
      uint32_t level) override;

  /**
   * @inherit doc
   */
  void increaseReferenceCount(
      IScheduler::WireId<IScheduler::Boolean> id) override;

  /**
   * @inherit doc
   */
  void increaseReferenceCount(
      IScheduler::WireId<IScheduler::Arithmetic> id) override;

  /**
   * @inherit doc
   */
  void decreaseReferenceCount(
      IScheduler::WireId<IScheduler::Boolean> id) override;

  /**
   * @inherit doc
   */
  void decreaseReferenceCount(
      IScheduler::WireId<IScheduler::Arithmetic> id) override;

  /**
   * @inherit doc
   */
  uint64_t getBatchSize(
      IScheduler::WireId<IScheduler::Boolean> id) const override;

  /**
   * @inherit doc
   */
  uint64_t getBatchSize(
      IScheduler::WireId<IScheduler::Arithmetic> id) const override;

  /**
   * @inherit doc
   */
  IScheduler::WireId<IScheduler::Boolean> allocateBatchBooleanValue(
      const std::vector<bool>& v,
      uint32_t firstAvailableLevel = 0,
      size_t expectedBatchSize = 0) override;

  /**
   * @inherit doc
   */
  IScheduler::WireId<IScheduler::Arithmetic> allocateBatchIntegerValue(
      const std::vector<uint64_t>& v,
      uint32_t firstAvailableLevel = 0,
      size_t expectedBatchSize = 0) override;

  /**
   * @inherit doc
   */
  const std::vector<bool>& getBatchBooleanValue(
      IScheduler::WireId<IScheduler::Boolean> id) const override;

  /**
   * @inherit doc
   */
  const std::vector<uint64_t>& getBatchIntegerValue(
      IScheduler::WireId<IScheduler::Arithmetic> id) const override;

  // get the writable batch of value associated with boolean wire with given id.
  std::vector<bool>& getWritableBatchBooleanValue(
      IScheduler::WireId<IScheduler::Boolean> id) const override;

  // get the writable batch of value associated with integer wire with given id.
  std::vector<uint64_t>& getWritableBatchIntegerValue(
      IScheduler::WireId<IScheduler::Arithmetic> id) const override;

  /**
   * @inherit doc
   */
  void setBatchBooleanValue(
      IScheduler::WireId<IScheduler::Boolean> id,
      const std::vector<bool>& v) override;

  /**
   * @inherit doc
   */
  void setBatchIntegerValue(
      IScheduler::WireId<IScheduler::Arithmetic> id,
      const std::vector<uint64_t>& v) override;

  /**
   * @inherit doc
   */
  uint32_t getBatchFirstAvailableLevel(
      IScheduler::WireId<IScheduler::Boolean> id) const override;

  /**
   * @inherit doc
   */
  uint32_t getBatchFirstAvailableLevel(
      IScheduler::WireId<IScheduler::Arithmetic> id) const override;

  /**
   * @inherit doc
   */
  void setBatchFirstAvailableLevel(
      IScheduler::WireId<IScheduler::Boolean> id,
      uint32_t level) override;

  /**
   * @inherit doc
   */
  void setBatchFirstAvailableLevel(
      IScheduler::WireId<IScheduler::Arithmetic> id,
      uint32_t level) override;

  /**
   * @inherit doc
   */
  void increaseBatchReferenceCount(
      IScheduler::WireId<IScheduler::Boolean> id) override;

  /**
   * @inherit doc
   */
  void increaseBatchReferenceCount(
      IScheduler::WireId<IScheduler::Arithmetic> id) override;

  /**
   * @inherit doc
   */
  void decreaseBatchReferenceCount(
      IScheduler::WireId<IScheduler::Boolean> id) override;

  /**
   * @inherit doc
   */
  void decreaseBatchReferenceCount(
      IScheduler::WireId<IScheduler::Arithmetic> id) override;

 private:
  std::unique_ptr<IAllocator<WireRecord<bool, false>>> boolAllocator_;
  std::unique_ptr<IAllocator<WireRecord<std::vector<bool>, true>>>
      boolBatchAllocator_;
  std::unique_ptr<IAllocator<WireRecord<uint64_t, false>>> intAllocator_;
  std::unique_ptr<IAllocator<WireRecord<std::vector<uint64_t>, true>>>
      intBatchAllocator_;
};

} // namespace fbpcf::scheduler
