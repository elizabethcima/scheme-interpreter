#pragma once
#include "cell.h"

std::shared_ptr<Cell> eval(Environment* env, const std::shared_ptr<Cell>& cell);

std::shared_ptr<Cell> symbol_eval(
    Environment* env, const std::shared_ptr<Cell>& cell);

std::shared_ptr<Cell> operations_eval(
    Environment* env, const std::shared_ptr<Cell>& cell, int func(int, int));

std::shared_ptr<Cell> define_eval(
    Environment* env, const std::shared_ptr<Cell>& cell);

void insert_to_env(
    Environment* env, std::string key, std::shared_ptr<Cell> val);

std::shared_ptr<Cell> function_eval(
    Environment* env, const std::shared_ptr<Cell>& cell);

std::shared_ptr<Cell> if_eval(
    Environment* env, const std::shared_ptr<Cell>& cell);

std::shared_ptr<Cell> let_eval(
    Environment* env, const std::shared_ptr<Cell>& cell);

std::shared_ptr<Cell> lambda_eval(
    Environment* env, const std::shared_ptr<Cell>& cell);

std::shared_ptr<Cell> cons_eval(
    Environment* env, const std::shared_ptr<Cell>& cell);

std::shared_ptr<Cell> car_eval(
    Environment* env, const std::shared_ptr<Cell>& cell);

std::shared_ptr<Cell> cdr_eval(
    Environment* env, const std::shared_ptr<Cell>& cell);

std::shared_ptr<Cell> list_eval(
    Environment* env, const std::shared_ptr<Cell>& cell);

inline int top_level_length(const std::shared_ptr<Cell>& cell) {
  int result = 0;
  for (auto current_cell = cell; current_cell != nullptr;
       current_cell = current_cell->cdr()) {
    result = result + 1;
  }
  return result;
}
