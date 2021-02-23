#pragma once

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

class Cell;

enum class CellType { Cell, Num, Symbol, Procedure };

using Environment = std::list<
    std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<Cell>>>>;

class Cell {
 public:
  Cell(std::shared_ptr<Cell> car, std::shared_ptr<Cell> cdr)
      : car_(std::move(car))
      , cdr_(std::move(cdr)) {
  }

  virtual ~Cell(){};

  virtual std::string quote() const {
    if (cdr_ == nullptr && car_ != nullptr) {
      return car_->quote();
    } else if (cdr_ == nullptr && car_ == nullptr) {
      return "";
    }
    auto pair_seperator = "";
    if (cdr_->get_type() != CellType::Cell) {
      pair_seperator = ". ";
    }
    return car_->quote() + " " + +pair_seperator + cdr_->quote();
  }

  virtual CellType get_type() const {
    return CellType::Cell;
  }

  const std::shared_ptr<Cell> car() const {
    return car_;
  }

  const std::shared_ptr<Cell> cdr() const {
    return cdr_;
  }

 protected:
  // first
  std::shared_ptr<Cell> car_;
  // rest
  std::shared_ptr<Cell> cdr_;
};

template <typename T>
class NumberCell_ : public Cell {
 public:
  NumberCell_(T val)
      : Cell(nullptr, nullptr)
      , val_(val) {
  }

  CellType get_type() const override {
    return CellType::Num;
  }

  std::string quote() const override {
    return std::to_string(val_);
  }

  T get_value() const {
    return val_;
  }

 private:
  T val_;
};

using IntCell = NumberCell_<int>;
using FloatCell = NumberCell_<float>;

// handle operation
class SymbolCell : public Cell {
 public:
  SymbolCell(std::string val)
      : Cell(nullptr, nullptr)
      , val_(val) {
  }

  CellType get_type() const override {
    return CellType::Symbol;
  }

  std::string quote() const override {
    return val_;
  }

 private:
  std::string val_;
};

class ProcedureCell : public Cell {
 public:
  ProcedureCell(
      std::shared_ptr<Cell> param,
      std::shared_ptr<Cell> operation,
      Environment env)
      : Cell(nullptr, nullptr)
      , param_(param)
      , operation_(operation)
      , env_(env) {
  }
  CellType get_type() const override {
    return CellType::Procedure;
  }

  std::shared_ptr<Cell> getParam() {
    return param_;
  }

  std::shared_ptr<Cell> getOperation() {
    return operation_;
  }

  Environment getEnvironment() {
    return env_;
  }

  std::string quote() const override {
    return "#<procedure>";
  }

 private:
  std::shared_ptr<Cell> param_;
  std::shared_ptr<Cell> operation_;
  Environment env_;
};
